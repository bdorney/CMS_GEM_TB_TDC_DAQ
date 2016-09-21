
#include "IDaqV775.h"

using std::cerr;
using std::cout;
using std::dec;
using std::endl;
using std::flush;
using std::hex;
using std::istream;
using std::ostream;
using std::showbase;
using std::vector;

IDaqV775::IDaqV775(){
	moduleType = idmV775;
	moduleKind = idmkAdc;
	SetName( "CAEN V775 TDC" );
	iterLimit = 5;
  bufferSize = V775_OUTPUTBUFFER_LAST - V775_OUTPUTBUFFER + V775_DATAWORDSIZE;
  data = new uint32_t[ bufferSize / sizeof( uint32_t ) ];
	acqMode = V775_ALL;
  //readoutMode = V775_D32_EVTCOUNT;
	readoutMode = V775_D32;
}

//IDaqV775::~IDaqV775(){
	//delete[] data;
	//delete data;
//}

void IDaqV775::SetGeoAddress( uint16_t aGeo ){
	uint32_t  addr = ba + V775_GEO_ADDRESS;
	vmeInt->Write( addr, aGeo );
	status = vmeInt->GetStatus();
	if ( status == IDaqSuccess) CheckGeoAddress();
}

void IDaqV775::CheckGeoAddress(){
	uint16_t geo = 0;
	CheckRegister( 0x1002, geo );
	cout << "geo = " << geo << endl;
	geoAddress = geo & 0x1F;
}

void IDaqV775::DataReset(){
	SetBitReg2( V775_BS2_ClearData, IDaqEnable);
  CheckBitReg2();
  if ( !( bitReg2 & V775_BS2_ClearData ) || ( status != IDaqSuccess) ){
    cout << "[V775] Problem with Data Reset " << endl;
  }

  SetBitReg2( V775_BS2_ClearData, IDaqDisable);

  while( IsBusy() ){
    int iter( 0 );
    iter++;
    if ( iter > iterLimit){ 
      cout << "[V775] Module looks dead ... " << endl;
      break;
    } 
  }
}

void IDaqV775::SoftwareReset(){
	uint32_t addr; 
	cout << "[V775] Performing software reset. Please wait...";
	addr = ba + V775_BIT_SET1;
	vmeInt->Write( addr, (uint16_t)V775_BS1_SoftReset );
	status = vmeInt->GetStatus();
	//sleep( 1 );
    std::this_thread::sleep_for (std::chrono::seconds(1));
	if( status == IDaqSuccess ){
		addr = ba + V775_BIT_CLEAR1;
		vmeInt->Write( addr, (uint16_t)V775_BS1_SoftReset );
		status = vmeInt->GetStatus();
	}
	//sleep( 1 );
    std::this_thread::sleep_for (std::chrono::seconds(1));
    cout << " Done." << endl;
	/*
	SetBitReg1( V775_BS1_SoftReset, IDaqEnable );
  CheckBitReg1();
  
  if ( !( bitReg1 & V775_BS1_SoftReset ) || ( status != IDaqSuccess ) ){
    cout << "[V775] Problem with Software Reset " << endl;
  } else {
	  SetBitReg1( V775_BS1_SoftReset, IDaqDisable );
  	while( IsBusy() ){
    	int iter( 0 );
    	iter++;
    	if (iter > iterLimit){ 
      	cout << "[V775] Module looks dead ... " << endl;
      	break;
    	} 
  	}
  }
	*/
}

bool IDaqV775::IsBusy(){ 
	CheckRegister( V775_STATUS_REGISTER1, statusReg1 );
	if( status != IDaqSuccess ) return 1; 
	return !!( V775_Status1_Busy & statusReg1 ); 
}

bool IDaqV775::DReady(){ 
	CheckRegister( V775_STATUS_REGISTER1, statusReg1 ); 
	//cout << "SR = " << hex << statusReg1 << dec << endl;
	return !!( V775_Status1_DReady & statusReg1 ); 
}

bool IDaqV775::Evrdy(){ 
	CheckRegister( V775_STATUS_REGISTER1, statusReg1 ); 
	return !!( V775_Status1_Evrdy & statusReg1 ); 
}

bool IDaqV775::IsPurged(){ 
	CheckRegister( V775_STATUS_REGISTER1, statusReg1 ); 
	return !!( V775_Status1_Purged & statusReg1 ); 
}

bool IDaqV775::IsFull(){ 
	CheckRegister( V775_STATUS_REGISTER2, statusReg2 ); 
	return !!( V775_Status2_BufferFull & statusReg2 ); 
}

uint16_t IDaqV775::GetEventCountLow(){ 
	return eventCountLow;
}
 
uint16_t IDaqV775::GetEventCountHigh(){
	return eventCountHigh;
}

uint32_t IDaqV775::GetEventCount(){
	uint32_t evtc = ( uint32_t ) eventCountHigh;
	//cout << "high = " << eventCountHigh << " evtc = " << evtc << endl;
  evtc += ( evtc << 16 ) + eventCountLow;
	//cout << "low = " << eventCountLow << " evtc = " << evtc << endl;
  return evtc;
}

void IDaqV775::CheckEventCount(){
	CheckEventCountLow();
  if ( status == IDaqSuccess ){
	  CheckEventCountHigh();
  }
}

void IDaqV775::CheckEventCountLow(){ 
	CheckRegister( V775_EVENT_COUNTER_LOW, eventCountLow ); 
}

void IDaqV775::CheckEventCountHigh(){
	CheckRegister( V775_EVENT_COUNTER_HIGH, eventCountHigh );
}

void IDaqV775::ClearEventCount(){
	uint16_t d1 = 0;
	uint16_t d2 = 0;
  SetRegister( V775_EVENT_COUNTER_RESET, d1, d2 );
}
  
void IDaqV775::Initialize(){ 
	Initialize( V775_ALL, V775_D32, 180, 10 ); 
}
/*
void IDaqV775::SetPedestal( uint16_t ped ){
	if ( ped < 60 || ped > 255 ) status = IDaqInvalidParam;
  else SetRegister( V775_IPED, ped, iPed );
}
*/

void IDaqV775::CheckThresholds(){
	uint32_t  addr;
  for ( unsigned int ch = 0; ch < 32; ++ch ) {
    addr = ba + V775_THRESHOLDS + 2 * ch ;
    vmeInt->Read( addr, threshold[ ch ] );
    status = vmeInt->GetStatus();
  }
}

int16_t IDaqV775::GetThresholdValue( unsigned int ch ){
	if ( ch > 31 ) return -1;
  return ( int16_t )( threshold[ ch ] & 0x00FF );
}

IDaqSwitch IDaqV775::GetChannelStatus( unsigned int ch){
	if ( ch > 31 ) return IDaqUndefined;
  return ( IDaqSwitch )( ~( threshold[ ch ] & 0x0100 ) );
}

void IDaqV775::SetThresholdValue( unsigned int ch, unsigned int th ){
	uint32_t addr;
  uint16_t dt;
  CheckThresholds();
  if ( status != IDaqSuccess) return;

  if ( th > 255 ) status = IDaqInvalidParam;

  if ( ch == 32 ) { // all channels
    for( unsigned int ich = 0; ich < 32; ++ich ){
      addr = ba + V775_THRESHOLDS + 2 * ich ;
      dt = ( ( threshold[ ich ] & 0x0100 ) | ( th & 0x00FF ) );
      vmeInt->Write( addr, dt );
      status = vmeInt->GetStatus();
      if ( status == IDaqSuccess) threshold[ ich ] = dt;
      else return;
    }
  }
  else if ( ch < 32 ){
    addr = ba + V775_THRESHOLDS + 2 * ch;
    dt = ( ( threshold[ ch ] & 0x0100 ) | ( th & 0x00FF ) );
    vmeInt->Write( addr, dt );
    status = vmeInt->GetStatus();
    if ( status == IDaqSuccess) threshold[ ch ] = dt;
  } else status = IDaqInvalidParam;
}

void IDaqV775::SetChannelStatus( unsigned int ch, IDaqSwitch s ){
	uint32_t addr;
  	uint16_t dt;
  	CheckThresholds();
  	if ( status == IDaqSuccess){ 
		if (ch == 32) {
			if( ( s == IDaqEnable ) || ( s == IDaqDisable ) )
			  for ( unsigned int ich = 0; ich < 32; ++ich ) {
				addr = ba + V775_THRESHOLDS + 2 * ich;
			  	if( s == IDaqEnable ) dt = ( threshold[ ich ] & 0x00FF );
			  	else if ( s == IDaqDisable ) dt = ( threshold[ ich ] | 0x0100 );
				vmeInt->Write( addr, dt );
				status = vmeInt->GetStatus();
				if ( status == IDaqSuccess ) threshold[ ich ] = dt;
				else return;
			  }
			else{
				status = IDaqInvalidParam;
				cout << "Invalid SetChannelStatus parameter: " << (int)s << endl; 
				return;			
			}
		}
		else if ( ( ch >= 0 ) && ( ch < 32 ) ) {
			
			addr = ba + V775_THRESHOLDS + 2 * ch;
			if( s == IDaqEnable ){
				 dt = ( threshold[ ch ] & 0x00FF );
				 cout << "Enable channel " << ch;
			}
			else if ( s == IDaqDisable ){							
				dt = ( threshold[ ch ] | 0x0100 );
				cout << "Disable channel " << ch;
			} else {
				status = IDaqInvalidParam;
				cout << "Invalid SetChannelStatus parameter: " << (int)s << endl;
				return;			
			}
			vmeInt->Write( addr, dt );
			status = vmeInt->GetStatus();
			if ( status == IDaqSuccess) threshold[ ch ] = dt;
			cout << " Done." << endl;
		} 
		else{
			status = IDaqInvalidParam;		
			cout << "IDaqV775::SetChannelStatus: Invalid Channel Number: " << ch << endl;
		}	
	}
}

void IDaqV775::Initialize( V775AcqMode am, V775ReadoutMode rm, uint16_t ped, unsigned int th){
	cout << "[V775] Start initialization (be patient) ..." << flush;
  // Reset Module 
	SoftwareReset();
  //sleep( 1 );
    std::this_thread::sleep_for (std::chrono::seconds(1));
  if ( status != IDaqSuccess) return;
  cout << "."<< flush;
  //Initialize Pedestal Value
//  SetPedestal( ped );
  cout << "."<< flush;
  //Initialize Threshold Value (all channels)
//  SetThresholdValue( 32, th );
  cout << ".."<< flush;
  //Initialize Channel Status : All Enable
  SetChannelStatus( 32, IDaqEnable );
  cout << "."<< flush;
  //Disable auto increase
  SetBitReg2( V775_BS2_AutoIncr, IDaqDisable );

  acqMode = am;
  switch ( acqMode ) {
  
		case V775_ALL:

    	//SetBitReg2( V775_BS2_OverRange, IDaqEnable );    // Enable OverRange
    	cout << "."<< flush; 
    	//SetBitReg2( V775_BS2_LowThreshold, IDaqEnable ); // Enable UnderThreshold
    	cout << "."<< flush;
    	break;
  
		case V775_OVERFLOWSUP:
    	SetBitReg2( V775_BS2_OverRange, IDaqDisable );   //Disable OverRange
    	cout << "."<< flush; 
    	SetBitReg2( V775_BS2_LowThreshold, IDaqEnable ); //Enable UnderThreshold
    	cout << "."<< flush;
    	break;
  
		case V775_ZEROSUP:
    	SetBitReg2( V775_BS2_OverRange, IDaqEnable );    // Enable OverRange
    	cout << "."<< flush; 
    	SetBitReg2( V775_BS2_LowThreshold, IDaqDisable );// Disable UnderThreshold
    	cout << "."<< flush;
    	break;

		case V775_OVERFLOWSUP_ZEROSUP:
    	SetBitReg2( V775_BS2_OverRange, IDaqDisable);   //Disable OverRange
    	cout << "."<< flush; 
    	SetBitReg2( V775_BS2_LowThreshold, IDaqDisable);// Disable UnderThreshold
    	cout << "."<< flush;
    	break;

  default:
    cout << " V775 Acquisition Mode not implemented " << endl;
    break;
  }

  readoutMode = rm;
  uint16_t nc( 0 );
  switch ( readoutMode ) {
  	case V775_BLT32_BERR:
  	case V775_BLT32_BERR_DR:
  	case V775_BLT32_BERR_ER:
    	//Enable Berr Mode
    	CheckRegister( V775_CONTROL_REGISTER1, controlReg );
    	nc = controlReg | V775_Control_BerrEnable;
    	SetRegister( V775_CONTROL_REGISTER1, nc, controlReg );
    	cout << showbase << hex << controlReg << endl;
    	cout << "."<< flush;
    break;
    
  	case V775_D32_EVTCOUNT:
    	//Enable AllTrg Mode
    	SetBitReg2( V775_BS2_AllTrg, IDaqEnable);
    	cout << "." << flush;
     //Disable Berr Mode
    	CheckRegister( V775_CONTROL_REGISTER1, controlReg );
    	nc = controlReg & ~V775_Control_BerrEnable;
    	SetRegister( V775_CONTROL_REGISTER1, nc, controlReg );
    break;
		case V775_D32:{
			SetBitReg2( V775_BS2_AutoIncr, IDaqEnable );
			break;
		}
  default:
    cout << " V775 Readout Mode not implemented " << endl;
    break;
  }

  
 
  //Verification
  CheckBitReg2();
  cout << " Bit Register 2 : " << GetBitReg2() << endl ;
	/*
  CheckPedestal();
  cout << dec ;
  cout <<" Pedestal : " << GetPedestal() << endl;
	*/
  CheckThresholds();
  cout << " Thresholds : " << endl;
  for ( int id = 0; id < 32; ++id ){
    if ( id % 8 == 0 ) cout << endl;
    cout << GetThresholdValue( id ) << " ";
	}
 
  if ( status == IDaqSuccess) {
    cout << " Done" << endl;
    initStatus = true;
  } else { 
    cout << " Not successful." << endl;
    initStatus = false;
    status = IDaqGenericError; 
  }  
}

void IDaqV775::Arm() {  
	DataReset(); 
	ClearEventCount();
}

int IDaqV775::Readout( vector<uint32_t>& aData ){
	uint16_t dum( 0 );
	uint16_t nAdcGate( 0 );
	int iter( 0 );

	while ( IsBusy() ){
		cout << " [V775] TDC in conversion... " << endl; 
		if ( IsFull() ) {
			cout << " [V775] Buffer is full! Possible data loss." << endl;
			break;
		}
    iter++;
		if ( iter > GetIterLimit() ){ 
			cerr << " *** [V775] Module looks dead ...Stopping run *** " << endl;
			break;
		} 
	}
	cout << "Readout mode = " << GetReadoutMode()  << endl;
	switch( GetReadoutMode() ){
		case V775_D32:{
			uint32_t addr = ba;
			uint32_t dt = 0;
			CheckEventCountLow();
			if( vmeInt->GetStatus() != IDaqSuccess ) return -1;
			nAdcGate = GetEventCountLow() + 1;
			cout << nAdcGate << " particle events to read in TDC" << endl;
			int evCount = 0;
			while( evCount < nAdcGate ){
				vmeInt->Read( addr, dt );
				DataWordV775 dw( dt );
				if( dw.GetDataType() == (uint32_t)DWV775_EventHeader){
					aData.push_back( dt );
					unsigned int N = dw.GetNChannel() + 1;
					for( unsigned int i = 0; i < N; ++i ){
						vmeInt->Read( addr, dt );
						aData.push_back( dt );
					}
				}
				++evCount;
			}	
			break;
		}
		case V775_D32_EVTCOUNT: {
			CheckEventCountLow();
			if( vmeInt->GetStatus() != IDaqSuccess ) return -1;
			
			nAdcGate = GetEventCountLow() + 1;
			cout << nAdcGate << " particle events to read in TDC" << endl;
			if( nAdcGate > 0 ) {
				for ( int ievt = 0; ievt < nAdcGate; ++ievt ){
					ReadAndWriteOutputBuffer( aData, 34 );
					SetRegister( V775_INCREMENT_EVENT, dum, dum );
				}
				ClearEventCount();
				//fs += 34 * nAdcGate * V775_DATAWORDSIZE;
			}
			break;
		}
		case V775_BLT32_BERR:
		default:
			cerr << " V775 readout mode not implemented " << endl;
			break;
	}
  
	//DataWordV775::WriteDataBlockLimit( fp );
	//fs += GetWordSize();
	return nAdcGate;
 
}

int IDaqV775::Readout( FILE *fp, uint64_t &fs ){
	uint16_t dum( 0 );
  uint16_t nAdcGate( 0 );
  int iter( 0 );

  while ( IsBusy() ){
    cout << " [V775] TDC in conversion... " << endl; 
    if ( IsFull() ) {
      cout << " [V775] Buffer is full! Possible data loss." << endl;
      break;
    }
    
    iter++;
    if ( iter > GetIterLimit() ){ 
      cerr << " *** [V775] Module looks dead ...Stopping run *** " << endl;
      break;
    } 
  }
  
  switch( GetReadoutMode() ){
  
  case V775_D32_EVTCOUNT: {
    CheckEventCountLow();
		
		if( vmeInt->GetStatus() != IDaqSuccess ){
      return -1;
    }
    nAdcGate = GetEventCountLow() + 1;
    cout << nAdcGate << " particle events to read in adc" << endl;
    if (nAdcGate>0) {
      for (int ievt=0; ievt< nAdcGate; ievt++){
				ReadAndWriteOutputBuffer( fp, 34 );
				SetRegister( V775_INCREMENT_EVENT, dum, dum );
      }
      ClearEventCount();
      fs += 34 * nAdcGate * V775_DATAWORDSIZE;
    }
    break;
  }
  case V775_BLT32_BERR:
  default:
    cerr << " V775 readout mode not implemented " << endl;
    break;
  }
  
  DataWordV775::WriteDataBlockLimit( fp );
  fs += GetWordSize();
  return nAdcGate;
}

void IDaqV775::CheckBitReg1(){ 
	CheckRegister( V775_BIT_SET1, bitReg1 ); 
}

uint16_t IDaqV775::GetBitReg1(){ 
	return bitReg1; 
}

void IDaqV775::SetBitReg1( V775BitSet1Mask bm, IDaqSwitch s ){
	uint32_t  addr;
  uint16_t  dt = (uint16_t)( bm );
  
  switch ( s ) {
  case IDaqEnable:
    addr = ba + V775_BIT_SET1;
    vmeInt->Write( addr, dt );
    status = vmeInt->GetStatus();
    if ( status == IDaqSuccess) bitReg1 = ( bitReg1 | dt );
    break;
    
  case IDaqDisable:
     addr = ba + V775_BIT_CLEAR1;
     vmeInt->Write( addr, dt );
     status = vmeInt->GetStatus();
     if ( status == IDaqSuccess ) bitReg1 = ( bitReg1 & ~dt ) ;
    break;
  default:
    status = IDaqInvalidParam;
    break;
  }
}

void IDaqV775::CheckBitReg2(){ 
	CheckRegister( V775_BIT_SET2, bitReg2); 
}

uint16_t IDaqV775::GetBitReg2(){ 
	return bitReg2;
}

void IDaqV775::SetBitReg2( V775BitSet2Mask bm, IDaqSwitch s){
	uint32_t  addr ;
  uint16_t  dt = ( uint16_t )( bm );
  
  switch ( s ) {
  case IDaqEnable:
    addr = ba + V775_BIT_SET2;
    vmeInt->Write( addr, dt );
    status = vmeInt->GetStatus();
    if ( status == IDaqSuccess ) bitReg2 = ( bitReg2 | dt );
    break;
  case IDaqDisable:
     addr = ba + V775_BIT_CLEAR2;
     vmeInt->Write( addr, dt );
     status = vmeInt->GetStatus();
     if ( status == IDaqSuccess) bitReg2 = ( bitReg2 & ~dt );
    break;
  default:
    status = IDaqInvalidParam;
    break;
  }
}

void IDaqV775::SetFullScaleRange( unsigned int aFullScaleRange ){
  uint32_t addr = ba + V775_FULL_SCALE_RANGE;
  if( ( aFullScaleRange <= 0xFF ) && ( aFullScaleRange >= 0x1E ) ){
    uint16_t dt = aFullScaleRange;
    vmeInt->Write( addr, dt );
    status = vmeInt->GetStatus();
    if( status == IDaqSuccess ) cout << "[V775] Full Scale Range set to " << hex << dt << dec << " = " << (double) 8.9/dt << " [nsec/bin]." << endl;
    else cerr << "*** [V775] Full Scale Range error: " << (int) status << endl;
  } else { 
    cerr << "*** [V775] Full Scale Range is outside limits [ 0x1E; 0xFF ]" << endl; 
  }
}
		
unsigned int IDaqV775::GetFullScaleRange(){
  uint32_t addr = ba + V775_FULL_SCALE_RANGE;
  uint16_t dt = 0;
  vmeInt->Read( addr, dt );
  status = vmeInt->GetStatus();
  if ( status == IDaqSuccess ) return dt; else return 0;
}

//===============================================================

DataWordV775::DataWordV775( uint32_t d ) : DataWord< uint32_t >( d ){}

uint32_t DataWordV775::GetDataType(){
  return ( ( data>>24 ) & 0x7 );
}

uint32_t DataWordV775::GetData(){
	if ( (GetDataType() != ( uint32_t )DWV775_EventDatum ) ) return 0xFFFF;
  return ( data & 0x0FFF );
}

uint32_t DataWordV775::GetDataStatus(){
	if ( (GetDataType() != (uint32_t)DWV775_EventDatum ) ) return DWV775_InvalidData;
  return ( ( data >> 12 ) & 0x07 );
}

uint32_t DataWordV775::GetCrateNumber(){
	if ( ( GetDataType() != (uint32_t)DWV775_EventHeader ) ) return DWV775_InvalidData;
  return ( ( data >> 16 ) & 0xFF );
}

uint32_t DataWordV775::GetChannel(){
	if ( ( GetDataType() != ( uint32_t ) DWV775_EventDatum ) ) return DWV775_InvalidData;
  return ( ( data >> 16 ) & 0x3F );
}

uint32_t DataWordV775::GetNChannel(){
	if ( ( GetDataType() != (uint32_t)DWV775_EventHeader ) ) return DWV775_InvalidData;
  return ( ( data >> 8 ) & 0x3F );
}

uint32_t DataWordV775::GetGeo(){
  uint32_t dt = GetDataType();
	if ( ( dt != ( uint32_t ) DWV775_EventDatum ) && 
			 ( dt != ( uint32_t ) DWV775_EventHeader )&&  
			 ( dt != ( uint32_t ) DWV775_EventTrailer ) ) return DWV775_InvalidData;
  return ( ( data >> 27 ) & 0x1F );
}

uint32_t DataWordV775::GetEventCount(){
  if ( GetDataType() != ( uint32_t ) DWV775_EventTrailer ) return DWV775_InvalidData;
  return ( data & 0xFFFFFF );
}

void DataWordV775::WriteDataBlockLimit(FILE *fp){
	/*  uint32_t data = DWV775_DataBlockLimit;
  data = data << 24;
  fwrite( &data, sizeof( data ), 1, fp );
	*/
}


ostream &operator << ( ostream &s, DataWordV775 &dw){
  // temporary

  uint32_t dt= dw.GetDataType();
  s << showbase << hex;
  s << " CAEN V775:";

  switch (dt) {
  case DWV775_EventDatum:
    s << " Geo Address : " << dw.GetGeo() << " " ;
    s << "(" << dw.GetDataStatus()  << ") ";
    switch( dw.GetDataStatus() ){
    	case DWMSV775_OverFlow:
      	s << "OverFlow Measurement: ";
      	break;
    	case DWMSV775_UnderThreshold:
      	s << "Under Threshold Measurement: ";
      	break;
    	case DWMSV775_Normal:
      	s << "Measurement: ";
      	break;
    	default:
      	s << "Invalid Data!";
				//s << "Normal: ";
				//return s;
      	break;
    }
    s << dec << dw.GetData() << "; Channel: " ;
    s << dw.GetChannel() << endl;
    break;

  case DWV775_EventHeader:
    s << " Geo Address : " << dw.GetGeo() << " " ;
    s << "Global Header : " ;
    s <<  " Crate Number : " ;
    s << dec << dw.GetCrateNumber();
    s << "; N Channels " ;
    s <<  dw.GetNChannel() << endl;
    break;

  case DWV775_EventTrailer:
    s << " Geo Address : " << dw.GetGeo() << " " ;
    s << "Global Trailer : ";
    s << dec << " Event Count : " ;
    s << dw.GetEventCount() << endl;
    break;
  
  case DWV775_DataFiller:
		s << "No data: " << showbase << hex << dw.GetRawData() << dec << endl;
    break;

  default:
    s << "Data Type not implemented " << endl;
    s << showbase << hex << dw.GetData() << endl;
    break;
  }
  return s;
}

istream &operator>>(istream &s,DataWordV775 &dw){
  uint32_t d;
  s >> d;
  dw = DataWordV775(d);
  return s; 
}

