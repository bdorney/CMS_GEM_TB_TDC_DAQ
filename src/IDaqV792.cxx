#include "IDaqV792.hh"

IDaqV792::IDaqV792(){
	moduleType = idmV792;
	moduleKind = idmkAdc;
	SetName( "CAEN V792 QDC" );
	iterLimit = 5;
  bufferSize = V792_OUTPUTBUFFERLAST - V792_OUTPUTBUFFER + V792_DATAWORDSIZE;
  data = new uint32_t[ bufferSize / sizeof( uint32_t ) ];
	acqMode = V792_ALL;
  readoutMode = V792_D32_EVTCOUNT;
}

IDaqV792::~IDaqV792(){
	delete[] data;
}

void IDaqV792::DataReset(){
	SetBitReg2( V792_BS2_ClearData, IDaqEnable);
  CheckBitReg2();
  if ( !( bitReg2 & V792_BS2_ClearData ) || ( status != IDaqSuccess) ){
    cout << " Problem with Data Reset " << endl;
  }

  SetBitReg2( V792_BS2_ClearData, IDaqDisable);

  while( IsBusy() ){
    int iter( 0 );
    iter++;
    if ( iter > iterLimit){ 
      cout << " V792 looks dead ... " << endl;
      break;
    } 
  }
}

void IDaqV792::SoftwareReset(){
	cout << "[V775] Performing software reset. Please wait...";
	SetBitReg1( V792_BS1_SoftReset, IDaqEnable );
	sleep( 1 );
	CheckBitReg1();
  if ( !( bitReg1 & V792_BS1_SoftReset ) || ( status != IDaqSuccess ) ){
    cout << " Problem with Software Reset " << endl;
  } else {
	  SetBitReg1( V792_BS1_SoftReset, IDaqDisable );
  	while( IsBusy() ){
    	int iter( 0 );
    	iter++;
    	if (iter > iterLimit){ 
      	cout << " V792 looks dead ... " << endl;
      	break;
    	} 
  	}
  }
	sleep( 1 );
	cout << " Done." << endl;
}

bool IDaqV792::IsBusy(){ 
	CheckRegister( V792_STATUS1, statusReg1 );
	if( status != IDaqSuccess ) return 1; 
	return !!( V792_Status1_Busy & statusReg1 ); 
}

bool IDaqV792::DReady(){ 
	CheckRegister( V792_STATUS1, statusReg1 ); 
	return !!( V792_Status1_DReady & statusReg1 ); 
}

bool IDaqV792::Evrdy(){ 
	CheckRegister( V792_STATUS1, statusReg1 ); 
	return !!( V792_Status1_Evrdy & statusReg1 ); 
}

bool IDaqV792::IsPurged(){ 
	CheckRegister( V792_STATUS1, statusReg1 ); 
	return !!( V792_Status1_Purged & statusReg1 ); 
}

bool IDaqV792::IsFull(){ 
	CheckRegister( V792_STATUS2, statusReg2 ); 
	return !!( V792_Status2_BufferFull & statusReg2 ); 
}

uint16_t IDaqV792::GetEventCountLow(){ 
	return eventCountLow;
}
 
uint16_t IDaqV792::GetEventCountHigh(){
	return eventCountHigh;
}

uint32_t IDaqV792::GetEventCount(){
	uint32_t evtc = ( uint32_t ) eventCountHigh;
  evtc += ( evtc <<16) + eventCountLow;
  return evtc;
}


void IDaqV792::CheckEventCount(){
	CheckEventCountLow();
  if ( status == IDaqSuccess){
	  CheckEventCountHigh();
  }
}

void IDaqV792::CheckEventCountLow(){ 
	CheckRegister( V792_EVENTCOUNTER_L, eventCountLow ); 
}

void IDaqV792::CheckEventCountHigh(){
	CheckRegister( V792_EVENTCOUNTER_H, eventCountHigh);
}

void IDaqV792::ClearEventCount(){
	uint16_t d1, d2;
  SetRegister( V792_EVENTCOUNTERRESET, d1, d2 );
}
  
void IDaqV792::Initialize(){ 
	Initialize( V792_ALL, V792_D32_EVTCOUNT, 180, 10 ); 
}

void IDaqV792::SetPedestal( uint16_t ped ){
	if ( ped < 60 || ped > 255 ) status = IDaqInvalidParam;
  else SetRegister( V792_IPED, ped, iPed );
}

void IDaqV792::CheckThresholds(){
	uint32_t  addr;
  for ( unsigned int ch = 0; ch < 32; ++ch ) {
    addr = ba + V792_THRESHOLDS + 2 * ch ;
    vmeInt->Read( addr, threshold[ ch ] );
    status = vmeInt->GetStatus();
  }
}

int16_t IDaqV792::GetThresholdValue( unsigned int ch ){
	if ( ch > 31 ) return -1;
  return ( int16_t )( threshold[ ch ] & 0x00FF );
}

IDaqSwitch IDaqV792::GetChannelStatus( unsigned int ch){
	if ( ch>31 ) return IDaqUndefined;
  return ( IDaqSwitch )(~( threshold[ ch ] & 0x0100 ) );
}

void IDaqV792::SetThresholdValue( unsigned int ch, unsigned int th ){
	uint32_t addr;
  uint16_t dt;
  CheckThresholds();
  if ( status != IDaqSuccess) return;

  if ( th > 255 ) status = IDaqInvalidParam;

  if ( ch == 32 ) { // all channels
    for( unsigned int ich = 0; ich < 32; ++ich ){
      addr = ba + V792_THRESHOLDS + 2 * ich ;
      dt = ( (threshold[ ich ] & 0x0100) | ( th & 0x00FF ) );
      vmeInt->Write( addr, dt );
      status = vmeInt->GetStatus();
      if ( status == IDaqSuccess) threshold[ ich ] = dt;
      else return;
    }
  }
  else if ( ch < 32 ){
    addr = ba + V792_THRESHOLDS + 2 * ch;
    dt = ( ( threshold[ ch ] & 0x0100 ) | ( th & 0x00FF ) );
    vmeInt->Write( addr, dt );
    status = vmeInt->GetStatus();
    if ( status == IDaqSuccess) threshold[ ch ] = dt;
  } else status = IDaqInvalidParam;
}

void IDaqV792::SetChannelStatus( unsigned int ch, IDaqSwitch s ){
	uint32_t addr;
  uint16_t dt;
  CheckThresholds();
  if ( status == IDaqSuccess){ 
		if (ch == 32) {
			for ( unsigned int ich = 0; ich < 32; ++ich ) {
				addr = ba + V792_THRESHOLDS + 2*ich;
			  
			  if( s == IDaqEnable ) dt = ( threshold[ ich ] & 0x00FF );
			  else if ( s == IDaqDisable ) dt = ( threshold[ ich ] | 0x0100 );
			  
				vmeInt->Write( addr, dt );
				status = vmeInt->GetStatus();
				if ( status == IDaqSuccess) threshold[ ich ] = dt;
				else return;
			}
		}
		else if ( ch < 32 ) {
			addr = ba + V792_THRESHOLDS + 2 * ch;
			
			if( s == IDaqEnable ) dt = ( threshold[ ch ] & 0x00FF );
			else if ( s == IDaqDisable ) dt = ( threshold[ ch ] | 0x0100 );
			
			vmeInt->Write( addr, dt );
			status = vmeInt->GetStatus();
			if ( status == IDaqSuccess) threshold[ ch ] = dt;
		}
		else status = IDaqInvalidParam;
	}
}

void IDaqV792::Initialize( V792AcqMode am, V792ReadoutMode rm, uint16_t ped, unsigned int th){
  cout << " Start V792 Initialization (be patient) ..." << flush;

  // Reset Module 
  SoftwareReset() ;
  sleep( 1 );
  if ( status != IDaqSuccess) return;
  cout << "."<< flush;
  
  //Initialize Pedestal Value
  SetPedestal( ped );
  cout << "."<< flush;
  
  //Initialize Threshold Value (all channels)
  SetThresholdValue( 32, th );
  cout << ".."<< flush;
  
  //Initialize Channel Status : All Enable
  SetChannelStatus( 32, IDaqEnable );
  cout << "."<< flush;
  
  //Disable auto increase
  SetBitReg2( V792_BS2_AutoIncr, IDaqDisable );

  acqMode = am;
  switch ( acqMode ) {
  case V792_ALL:

    SetBitReg2( V792_BS2_OverRange, IDaqEnable );    // Enable OverRange
    cout << "."<< flush; 
    SetBitReg2( V792_BS2_LowThreshold, IDaqEnable ); // Enable UnderThreshold
    cout << "."<< flush;
    break;
    
  case V792_OVERFLOWSUP:
    
    SetBitReg2( V792_BS2_OverRange, IDaqDisable );   //Disable OverRange
    cout << "."<< flush; 
    SetBitReg2( V792_BS2_LowThreshold, IDaqEnable ); //Enable UnderThreshold
    cout << "."<< flush;
    break;
    
  case V792_ZEROSUP:
    
    SetBitReg2( V792_BS2_OverRange, IDaqEnable );    // Enable OverRange
    cout << "."<< flush; 
    SetBitReg2( V792_BS2_LowThreshold, IDaqDisable );// Disable UnderThreshold
    cout << "."<< flush;
    break;
    
   case V792_OVERFLOWSUP_ZEROSUP:
    
    SetBitReg2( V792_BS2_OverRange, IDaqDisable);   //Disable OverRange
    cout << "."<< flush; 
    SetBitReg2( V792_BS2_LowThreshold, IDaqDisable);// Disable UnderThreshold
    cout << "."<< flush;
    break;
    
  default:
    cout << " V792 Acquisition Mode not implemented " << endl;
    break;
  }

  readoutMode = rm;
  uint16_t nc( 0 );
  switch ( readoutMode ) {
  	case V792_BLT32_BERR:
  	case V792_BLT32_BERR_DR:
  	case V792_BLT32_BERR_ER:
    	//Enable Berr Mode
    	CheckRegister( V792_CONTROL, controlReg );
    	nc = controlReg | V792_Control_BerrEnable;
    	SetRegister( V792_CONTROL, nc, controlReg );
    	cout << showbase << hex << controlReg << endl;
    	cout << "."<< flush;
    break;
    
  	case V792_D32_EVTCOUNT:
    	//Enable AllTrg Mode
    	SetBitReg2( V792_BS2_AllTrg, IDaqEnable);
    	cout << "."<< flush;
     //Disable Berr Mode
    	CheckRegister( V792_CONTROL, controlReg );
    	nc = controlReg & ~V792_Control_BerrEnable;
    	SetRegister( V792_CONTROL, nc, controlReg );
    break;
    
  default:
    cout << " V792 Readout Mode not implemented " << endl;
    break;
  }

  
 
  //Verification
  CheckBitReg2();
  cout << " Bit Register 2 : " << GetBitReg2() << endl ;
  
  CheckPedestal();
  cout << dec ;
  cout <<" Pedestal : " << GetPedestal() << endl;

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

void IDaqV792::Arm() {  
	DataReset(); 
	ClearEventCount();
}

int IDaqV792::Readout( FILE *fp, uint64_t &fs ){
	uint16_t dum( 0 );
  uint16_t nAdcGate( 0 );
  int iter( 0 );

  while ( IsBusy() ){
    cout << " ADC in conversion... " << endl; 
    if ( IsFull() ) {
      cout << " V792 Buffer is full! Possible data loss." << endl;
      break;
    }
    
    iter++;
    if ( iter > GetIterLimit() ){ 
      cerr << " *** V792 looks dead ...Stopping run *** " << endl;
      break;
    } 
  }
  
  switch( GetReadoutMode() ){
  
  case V792_D32_EVTCOUNT: {
    CheckEventCountLow();
    if( status != IDaqSuccess ){
      return -1;
    }
    nAdcGate = GetEventCountLow() + 1;
    cout << nAdcGate << " particle events to read in adc" << endl;
    
    if (nAdcGate>0) {
      for (int ievt=0; ievt< nAdcGate; ievt++){
				ReadAndWriteOutputBuffer(fp,34);
				SetRegister(V792_INCREMENTEVENT, dum, dum );
      }
      ClearEventCount();
      
      fs += 34 * nAdcGate * V792_DATAWORDSIZE;
    }

    break;
  }
  case V792_BLT32_BERR:
  
  default:
    cerr << " V792 readout mode not implemented " << endl;
    break;
  }
  
  DataWordV792::WriteDataBlockLimit( fp );
  fs += GetWordSize();
  return nAdcGate;
}

void IDaqV792::CheckBitReg1(){ 
	CheckRegister( V792_BITSET1, bitReg1 ); 
}

uint16_t IDaqV792::GetBitReg1(){ 
	return bitReg1; 
}

void IDaqV792::SetBitReg1( V792BitSet1Mask bm, IDaqSwitch s ){
	uint32_t  addr ;
  uint16_t  dt = (uint16_t)( bm );
  
  switch ( s ) {
  case IDaqEnable:
    addr = ba + V792_BITSET1;
    vmeInt->Write( addr, dt );
    status = vmeInt->GetStatus();
    if ( status == IDaqSuccess) bitReg1 = ( bitReg1 | dt );
    break;
    
  case IDaqDisable:
     addr = ba + V792_BITCLEAR1;
     vmeInt->Write( addr, dt );
     status = vmeInt->GetStatus();
     if ( status == IDaqSuccess ) bitReg1 = ( bitReg1 & ~dt ) ;
    break;
  default:
    status = IDaqInvalidParam;
    break;
  }
}

void IDaqV792::CheckBitReg2(){ 
	CheckRegister( V792_BITSET2, bitReg2); 
}

uint16_t IDaqV792::GetBitReg2(){ 
	return bitReg2;
}

void IDaqV792::SetBitReg2( V792BitSet2Mask bm, IDaqSwitch s){
	uint32_t  addr ;
  uint16_t  dt = ( uint16_t )( bm );
  
  switch ( s ) {
  case IDaqEnable:
    addr = ba + V792_BITSET2;
    vmeInt->Write( addr, dt );
    status = vmeInt->GetStatus();
    if ( status == IDaqSuccess ) bitReg2 = ( bitReg2 | dt );
    break;
  case IDaqDisable:
     addr = ba + V792_BITCLEAR2;
     vmeInt->Write( addr, dt );
     status = vmeInt->GetStatus();
     if ( status == IDaqSuccess) bitReg2 = ( bitReg2 & ~dt );
    break;
  default:
    status = IDaqInvalidParam;
    break;
  }
}

//===============================================================

DataWordV792::DataWordV792( uint32_t d ) : DataWord< uint32_t >( d ){}

uint32_t DataWordV792::GetDataType(){
  return ( ( data>>24 ) & 0x7 );
}

uint32_t DataWordV792::GetMeasurement(){
  if ( GetDataType() != ( uint32_t )DWV792_Measurement &&  GetDataType() != ( uint32_t ) DWV792_Myst ) return DWV792_InvalidData;
  return ( data & 0x0FFF );
}

uint32_t DataWordV792::GetMeasurementStatus(){
  if ( GetDataType() != (uint32_t)DWV792_Measurement &&  GetDataType() != (uint32_t)DWV792_Myst) return DWV792_InvalidData;
  return ( ( data >> 12 ) & 0x03 );
}

uint32_t DataWordV792::GetCrateNumber(){
  if ( GetDataType() != (uint32_t)DWV792_GlobalHeader ) return DWV792_InvalidData;
  return ( ( data >> 16 ) & 0x3F );
}

uint32_t DataWordV792::GetChannel(){
  if ( GetDataType() != ( uint32_t ) DWV792_Measurement &&  GetDataType() != (uint32_t)DWV792_Myst ) return DWV792_InvalidData;
  return ( ( data >> 16 ) & 0x3F );
}

uint32_t DataWordV792::GetNChannel(){
  if ( GetDataType() != (uint32_t)DWV792_GlobalHeader ) return DWV792_InvalidData;
  return ( ( data >> 8 ) & 0x3F );
}

uint32_t DataWordV792::GetGeo(){
  uint32_t dt = GetDataType();
  if ( dt != ( uint32_t ) DWV792_Measurement &&  
       dt != ( uint32_t ) DWV792_Myst &&  
       dt != ( uint32_t ) DWV792_GlobalHeader &&  
       dt != ( uint32_t ) DWV792_GlobalTrailer   ) return DWV792_InvalidData;
  return ( ( data >> 27 ) & 0x1F );
}

uint32_t DataWordV792::GetEventCount(){
  if ( GetDataType() != ( uint32_t ) DWV792_GlobalTrailer ) return DWV792_InvalidData;
  return ( data & 0xFFFFFF );
}

void DataWordV792::WriteDataBlockLimit(FILE *fp){
  uint32_t data = DWV792_DataBlockLimit;
  data = data << 24;
  fwrite( &data, sizeof( data ), 1, fp );
}


ostream &operator << ( ostream &s, DataWordV792 &dw){
  // temporary

  uint32_t dt= dw.GetDataType();
  s << showbase << hex;
  s << " CAEN V792 ";

  switch (dt) {
  case DWV792_Myst:
  case DWV792_Measurement:
    s << " Geo Address : " << dw.GetGeo() << " " ;
    switch(dw.GetMeasurementStatus()){
    case DWV792_OverFlow:
      s << "OverFlow Measurement: ";
      break;
    case DWV792_UnderThreshold:
      s << "Under Threshold Measurement: ";
      break;
    case DWV792_Normal:
      s << "Measurement: ";
      break;
    default:
      s << "Invalid Measurement!";
      return s;
      break;
    }
    s << dec << dw.GetMeasurement() << " ; Channel: " ;
    s << dw.GetChannel() << endl;
    break;

  case DWV792_GlobalHeader:
    s << " Geo Address : " << dw.GetGeo() << " " ;
    s << "Global Header : " ;
    s <<  " Crate Number : " ;
    s << dec << dw.GetCrateNumber();
    s << "; N Channels " ;
    s <<  dw.GetNChannel() << endl;
    break;

  case DWV792_GlobalTrailer:
    s << " Geo Address : " << dw.GetGeo() << " " ;
    s << "Global Trailer : ";
    s << dec << " Event Count : " ;
    s << dw.GetEventCount() << endl;
    break;
  
  case DWV792_Filler:
    s << "no data"<< endl;
    break;

  case DWV792_DataBlockLimit:
    s << "Data Block Limit"<< endl;
    break;

  default:
    s << "Data Type not implemented " << endl;
    s << showbase << hex << dw.GetData() << endl;
    break;
  }
  return s;
}

istream &operator>>(istream &s,DataWordV792 &dw){
  uint32_t d;
  s >> d;
  dw = DataWordV792(d);
  return s; 
}

