
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

//Relying on m_data to be initialized from base class...we'll see if this is the case
IDaqV775::IDaqV775() : m_uiThreshold(new uint16_t[32]) {
	//m_vmeKind = idmkAdc;
	m_vmeKind = idmkTdc;
    m_vmeType = idmV775;
	SetName( "CAEN V775 TDC" );
	m_iIterLimit = 5;
    m_bufferSize = V775_OUTPUTBUFFER_LAST - V775_OUTPUTBUFFER + V775_DATAWORDSIZE;
    //data = new uint32_t[ bufferSize / sizeof( uint32_t ) ];
	m_modeAcq = V775_ALL;
	m_modeReadout = V775_D32;
	m_bVerboseMode = false;
}

IDaqV775::~IDaqV775(){
	//delete[] data;
	//delete data;
    m_vmeInt.reset();
    m_data.reset();
    m_strName.clear();
}

void IDaqV775::SetGeoAddress( uint16_t inputAddr ){
	uint32_t  addr = m_baseAddr + V775_GEO_ADDRESS;
	m_vmeInt->Write( addr, inputAddr );
    //m_vmeInt->Write<uint16_t>( m_baseAddr + V775_GEO_ADDRESS, inputAddr );
    //m_vmeInt->Write<uint16_t>( addr, inputAddr );
    m_vmeStatus = m_vmeInt->GetStatus();
	if ( m_vmeStatus == IDaqSuccess) CheckGeoAddress();
    
    return;
}

void IDaqV775::CheckGeoAddress(){
	uint16_t geoAddr = 0;
	CheckRegister( 0x1002, geoAddr );
	cout << "[V775] Geographic Address = " << showbase << hex << geoAddr << dec << endl;
	m_geoAddr = geoAddr & 0x1F;
    
    return;
}

void IDaqV775::DataReset(){
    SetBitReg2( V775_BS2_ClearData, IDaqEnable);
    CheckBitReg2();

	//Debugging
	//cout<<"IDaqV775::DataReset() - m_regBit2 = " << showbase << hex << m_regBit2 << dec << endl;
	//cout<<"IDaqV775::DataReset() - V775_BS2_ClearData = " << showbase << hex << V775_BS2_ClearData << dec << endl;
	//cout<<"IDaqV775::DataReset() - ( m_regBit2 & V775_BS2_ClearData ) = " << showbase << hex << ( m_regBit2 & V775_BS2_ClearData ) << dec << endl;
	//cout<<"IDaqV775::DataReset() - IDaqSuccess = " << showbase << hex << IDaqSuccess << dec << endl;
	//cout<<"IDaqV775::DataReset() - m_vmeStatus = " << showbase << hex << m_vmeStatus << dec << endl;

    if ( !( m_regBit2 & V775_BS2_ClearData ) || ( m_vmeStatus != IDaqSuccess) ){
        cout << "[V775] Problem with Data Reset " << endl;
    }

  SetBitReg2( V775_BS2_ClearData, IDaqDisable);

	//Debugging
	//cout<<"IDaqV775::DataReset() - m_regBit2 = " << showbase << hex << m_regBit2 << dec << endl;

  int iter( 0 );
  while( IsBusy() ){
	if( iter == 0 ){
		//cout<<"IsBusy() = " << IsBusy() << endl;
		cout<<"[V775] Module " << showbase << hex << m_baseAddr << dec << " is busy\n";
	}
	else if( iter % 1000 ) {
		//cout<<"Still Busy" << endl;
		cout<<"[V775] Module " << showbase << hex << m_baseAddr << dec << " is still busy\n";
	}

    /*if ( iter > m_iIterLimit){
      cout << "[V775] Module looks dead ... " << endl;
      m_bIsDead = true;

	DataReset();
      break;
    } */
    iter++;
  }
    
    return;
}

void IDaqV775::SoftwareReset(){
	uint32_t addr;
	cout << "[V775] Performing software reset. Please wait...";
	addr = m_baseAddr + V775_BIT_SET1;
	m_vmeInt->Write( addr, (uint16_t)V775_BS1_SoftReset );
    //m_vmeInt->Write<uint16_t>( m_baseAddr + V775_BIT_SET1, (uint16_t)V775_BS1_SoftReset );
    //m_vmeInt->Write<uint16_t>( addr, (uint16_t)V775_BS1_SoftReset );
    m_vmeStatus = m_vmeInt->GetStatus();

    std::this_thread::sleep_for (std::chrono::seconds(1));
	if( m_vmeStatus == IDaqSuccess ){
		addr = m_baseAddr + V775_BIT_CLEAR1;
		m_vmeInt->Write( addr, (uint16_t)V775_BS1_SoftReset );
	       //m_vmeInt->Write<uint16_t>( m_baseAddr + V775_BIT_CLEAR1, (uint16_t)V775_BS1_SoftReset );
	       //m_vmeInt->Write<uint16_t>( addr, (uint16_t)V775_BS1_SoftReset );
	        m_vmeStatus = m_vmeInt->GetStatus();
	}

    std::this_thread::sleep_for (std::chrono::seconds(1));
    cout << " Done." << endl;
	
    return;
}

bool IDaqV775::IsBusy(){ 
	CheckRegister( V775_STATUS_REGISTER1, m_regStatus1 );
	if( m_vmeStatus != IDaqSuccess ) return 1; 
	return !!( V775_Status1_Busy & m_regStatus1 ); 
}

bool IDaqV775::DReady(){ 
	CheckRegister( V775_STATUS_REGISTER1, m_regStatus1 ); 
	return !!( V775_Status1_DReady & m_regStatus1 );
}

bool IDaqV775::Evrdy(){ 
	CheckRegister( V775_STATUS_REGISTER1, m_regStatus1 ); 
	return !!( V775_Status1_Evrdy & m_regStatus1 ); 
}

bool IDaqV775::IsPurged(){ 
	CheckRegister( V775_STATUS_REGISTER1, m_regStatus1 ); 
	return !!( V775_Status1_Purged & m_regStatus1 ); 
}

bool IDaqV775::IsFull(){ 
	CheckRegister( V775_STATUS_REGISTER2, m_regStatus2 ); 
	return !!( V775_Status2_BufferFull & m_regStatus2 ); 
}

uint16_t IDaqV775::GetEventCountLow(){ 
	return m_uiEvtCountLow;
}
 
uint16_t IDaqV775::GetEventCountHigh(){
	return m_uiEvtCountHigh;
}

uint32_t IDaqV775::GetEventCount(){
	uint32_t evtc = ( uint32_t ) m_uiEvtCountHigh;
    evtc += ( evtc << 16 ) + m_uiEvtCountLow;
	return evtc;
}

void IDaqV775::CheckEventCount(){
    CheckEventCountLow();
    if ( m_vmeStatus == IDaqSuccess ){
        CheckEventCountHigh();
    }
    
    return;
}

void IDaqV775::CheckEventCountLow(){ 
	CheckRegister( V775_EVENT_COUNTER_LOW, m_uiEvtCountLow );
    
    return;
}

void IDaqV775::CheckEventCountHigh(){
	CheckRegister( V775_EVENT_COUNTER_HIGH, m_uiEvtCountHigh );
    
    return;
}

void IDaqV775::ClearEventCount(){
	//uint16_t uiRegData = 0;
	//uint16_t d1 = 0, d2 = 0;
	uint16_t d1(0), d2(0);
    SetRegister( V775_EVENT_COUNTER_RESET, d1, d2 );
    //SetRegister<uint16_t>( V775_EVENT_COUNTER_RESET, d1, d2 );
    //SetRegister<uint16_t>( V775_EVENT_COUNTER_RESET, (uint16_t) 0, uiRegData );
    
    return;
}
  
void IDaqV775::Initialize(){ 
	Initialize( V775_ALL, V775_D32, 180, 10 );
    
    return;
}

void IDaqV775::Initialize( V775AcqMode inputModeAcq, V775ReadoutMode inputModeReadout, uint16_t ped, unsigned int th){
    cout << "[V775] Start initialization for TDC " << showbase << hex << m_baseAddr << dec << " (be patient) ...\n";
    // Reset Module
    SoftwareReset();
    std::this_thread::sleep_for (std::chrono::seconds(1));

    if ( m_vmeStatus != IDaqSuccess){ return; }
    
    //Initialize Channel Status : All Enable
    SetChannelStatus( 32, IDaqEnable );
    //cout << "."<< flush;
    
    //Disable auto increase
    SetBitReg2( V775_BS2_AutoIncr, IDaqDisable );
    
    m_modeAcq = inputModeAcq;
    switch ( m_modeAcq ) {
            
        case V775_ALL:
            
            //Place holder
            break;
            
        case V775_OVERFLOWSUP:
            cout<<"Setting Overflow Suppression\n";
            SetBitReg2( V775_BS2_OverRange, IDaqDisable );   //Disable OverRange
            SetBitReg2( V775_BS2_LowThreshold, IDaqEnable ); //Enable UnderThreshold
            break;
            
        case V775_ZEROSUP:
            cout<<"Setting Zero Suppression\n";
            SetBitReg2( V775_BS2_OverRange, IDaqEnable );    // Enable OverRange
            SetBitReg2( V775_BS2_LowThreshold, IDaqDisable );// Disable UnderThreshold
            break;
            
        case V775_OVERFLOWSUP_ZEROSUP:
            cout<<"Setting Overflow & Zero Suppression\n";
            SetBitReg2( V775_BS2_OverRange, IDaqDisable);   //Disable OverRange
            SetBitReg2( V775_BS2_LowThreshold, IDaqDisable);// Disable UnderThreshold
            break;
            
        default:
            cout << "[V775] Acquisition Mode not implemented " << endl;
            break;
    }
    
    m_modeReadout = inputModeReadout;
    uint16_t uiRegData( 0 );
    switch ( m_modeReadout ) {
        case V775_BLT32_BERR:
        case V775_BLT32_BERR_DR:
        case V775_BLT32_BERR_ER:
            //Enable Berr Mode
            CheckRegister( V775_CONTROL_REGISTER1, m_regCtrl );
            uiRegData = m_regCtrl | V775_Control_BerrEnable;
            SetRegister( V775_CONTROL_REGISTER1, uiRegData, m_regCtrl );
            //SetRegister<uint16_t>( V775_CONTROL_REGISTER1, uiRegData, m_regCtrl );
            
            cout << "[V775] Readout Mode Set, Control Register = " << showbase << hex << m_regCtrl << dec << endl;
            break;
            
        case V775_D32_EVTCOUNT:
            //Enable AllTrg Mode
            SetBitReg2( V775_BS2_AllTrg, IDaqEnable);
            
            //Disable Berr Mode
            CheckRegister( V775_CONTROL_REGISTER1, m_regCtrl );
            uiRegData = m_regCtrl & ~V775_Control_BerrEnable;
            SetRegister( V775_CONTROL_REGISTER1, uiRegData, m_regCtrl );
            //SetRegister<uint16_t>( V775_CONTROL_REGISTER1, uiRegData, m_regCtrl );
            
            cout << "[V775] Readout Mode Set, Control Register = " << showbase << hex << m_regCtrl << dec << endl;
            break;
            
        case V775_D32:
            SetBitReg2( V775_BS2_AutoIncr, IDaqEnable );
            
            cout << "[V775] Readout Mode Set, Control Register = " << showbase << hex << m_regCtrl << dec << endl;
            break;
            
        default:
            cout << "[V775] Readout Mode not implemented " << endl;
            break;
    }
    
    //Verification
    //cout<<"Begin Verification\n";
    CheckBitReg2();
    //cout << " Bit Register 2 : " << GetBitReg2() << endl ;
    CheckThresholds();
    /*cout << " Thresholds : " << endl;
    for ( int iChIdx = 0; iChIdx < 32; ++iChIdx ){
        if ( iChIdx % 8 == 0 ) cout << endl;
        cout << GetThresholdValue( iChIdx ) << " ";
    }*/
    
    if ( m_vmeStatus == IDaqSuccess) {
        cout << "[V775] Initialization Successful" << endl;
        m_bInitStatus = true;
    } else { 
        cout << "[V775] Initialization Failure" << endl;
        m_bInitStatus = false;
        m_vmeStatus = IDaqGenericError; 
    }
    
    return;
}

/*
void IDaqV775::SetPedestal( uint16_t uiInputPed ){
    if ( uiInputPed < 60 || uiInputPed > 255 ){ m_vmeStatus = IDaqInvalidParam;}
    else{ SetRegister( V775_IPED, uiInputPed, m_uiPed );}
    return;
}
*/

void IDaqV775::CheckThresholds(){
	uint32_t  addr;
    for ( unsigned int iChIdx = 0; iChIdx < 32; ++iChIdx ) {
        addr = m_baseAddr + V775_THRESHOLDS + 2 * iChIdx ;
        m_vmeInt->Read( addr, (m_uiThreshold.get())[ iChIdx ] );
        //m_vmeInt->Read<uint16_t>( (m_baseAddr + V775_THRESHOLDS + 2 * iChIdx), (m_uiThreshold.get())[ iChIdx ] );
        //m_vmeInt->Read<uint16_t>( addr, (m_uiThreshold.get())[ iChIdx ] );
        m_vmeStatus = m_vmeInt->GetStatus();
    }
    
    return;
}

int16_t IDaqV775::GetThresholdValue( unsigned int uiInputCh ){
	if ( uiInputCh > 31 ) return -1;
    return ( int16_t )( (m_uiThreshold.get())[ uiInputCh ] & 0x00FF );
}

IDaqSwitch IDaqV775::GetChannelStatus( unsigned int uiInputCh){
	if ( uiInputCh > 31 ) return IDaqUndefined;
    return ( IDaqSwitch )( ~( (m_uiThreshold.get())[ uiInputCh ] & 0x0100 ) );
}

void IDaqV775::SetThresholdValue( unsigned int uiInputCh, unsigned int uiInputThresh ){
    uint32_t addr;
    uint16_t uiRegValThresh;
    
    CheckThresholds();
    if ( m_vmeStatus != IDaqSuccess) return;

    if ( uiInputThresh > 255 ) m_vmeStatus = IDaqInvalidParam;

    if ( uiInputCh == 32 ) { // all channels
        for( unsigned int iChIdx = 0; iChIdx < 32; ++iChIdx ){
            addr = m_baseAddr + V775_THRESHOLDS + 2 * iChIdx ;
            uiRegValThresh = ( ( (m_uiThreshold.get())[ iChIdx ] & 0x0100 ) | ( uiInputThresh & 0x00FF ) );
            m_vmeInt->Write( addr, uiRegValThresh );
            //m_vmeInt->Write<uint16_t>( (m_baseAddr + V775_THRESHOLDS + 2 * iChIdx), uiRegValThresh );
            //m_vmeInt->Write<uint16_t>( addr, uiRegValThresh );
            m_vmeStatus = m_vmeInt->GetStatus();
            if ( m_vmeStatus == IDaqSuccess){
                (m_uiThreshold.get())[ iChIdx ] = uiRegValThresh;
            }
            else{
                break;
            }
        }
    }
    else if ( uiInputCh < 32 ){
        addr = m_baseAddr + V775_THRESHOLDS + 2 * uiInputCh;
        uiRegValThresh = ( ( (m_uiThreshold.get())[ uiInputCh ] & 0x0100 ) | ( uiInputThresh & 0x00FF ) );
        m_vmeInt->Write( addr, uiRegValThresh );
        //m_vmeInt->Write<uint16_t>( (m_baseAddr + V775_THRESHOLDS + 2 * uiInputCh), uiRegValThresh );
	//m_vmeInt->Write<uint16_t>( addr, uiRegValThresh );
        m_vmeStatus = m_vmeInt->GetStatus();
        if ( m_vmeStatus == IDaqSuccess) (m_uiThreshold.get())[ uiInputCh ] = uiRegValThresh;
    }
    else{
        m_vmeStatus = IDaqInvalidParam;
    }
    
    return;
}

void IDaqV775::SetChannelStatus( unsigned int uiInputCh, IDaqSwitch inputSwitch ){
	uint32_t addr;
  	uint16_t uiRegValThresh;
  	CheckThresholds();
  	if ( m_vmeStatus == IDaqSuccess){
        if (uiInputCh == 32) {
            if( ( inputSwitch == IDaqEnable ) || ( inputSwitch == IDaqDisable ) ){
                for ( unsigned int iChIdx = 0; iChIdx < 32; ++iChIdx ) {
                    addr = m_baseAddr + V775_THRESHOLDS + 2 * iChIdx;
                    if( inputSwitch == IDaqEnable ) {
                        uiRegValThresh = ( (m_uiThreshold.get())[ iChIdx ] & 0x00FF );
                    }
                    else if ( inputSwitch == IDaqDisable ){
                        uiRegValThresh = ( (m_uiThreshold.get())[ iChIdx ] | 0x0100 );
                    }
                    m_vmeInt->Write( addr, uiRegValThresh );
                    //m_vmeInt->Write<uint16_t>( (m_baseAddr + V775_THRESHOLDS + 2 * iChIdx), uiRegValThresh );
                    //m_vmeInt->Write<uint16_t>( addr, uiRegValThresh );
                    m_vmeStatus = m_vmeInt->GetStatus();
                    if ( m_vmeStatus == IDaqSuccess ){
                        (m_uiThreshold.get())[ iChIdx ] = uiRegValThresh;
                    }
                    else{
                        //break;
                        return;
                    }
                }
            }
            else{
                m_vmeStatus = IDaqInvalidParam;
                //cout << "IDaqV775::SetChannelStatus() - Invalid SetChannelStatus parameter: " << (int)inputSwitch << endl;
                cout << "[V775] Invalid SetChannelStatus parameter: " << (int)inputSwitch << endl;
                return;
            }
        }
		else if ( ( uiInputCh < 32 ) ) {
			addr = m_baseAddr + V775_THRESHOLDS + 2 * uiInputCh;
			if( inputSwitch == IDaqEnable ){
				 uiRegValThresh = ( (m_uiThreshold.get())[ uiInputCh ] & 0x00FF );
				 cout << "[V775] Enabled channel " << uiInputCh;
			}
			else if ( inputSwitch == IDaqDisable ){							
				uiRegValThresh = ( (m_uiThreshold.get())[ uiInputCh ] | 0x0100 );
				cout << "[V775] Disabled channel " << uiInputCh;
			}
            else {
				m_vmeStatus = IDaqInvalidParam;
				cout << "[V775] Invalid SetChannelStatus parameter: " << (int)inputSwitch << endl;
				return;
			}
			m_vmeInt->Write( addr, uiRegValThresh );
            //m_vmeInt->Write<uint16_t>( (m_baseAddr + V775_THRESHOLDS + 2 * uiInputCh), uiRegValThresh );
            //m_vmeInt->Write<uint16_t>( addr, uiRegValThresh );
            m_vmeStatus = m_vmeInt->GetStatus();
            if ( m_vmeStatus == IDaqSuccess){
                (m_uiThreshold.get())[ uiInputCh ] = uiRegValThresh;
            }
			cout << "[V775] Set Channel " << uiInputCh << " Status Successful" << endl;
		} 
		else{
			m_vmeStatus = IDaqInvalidParam;		
			//cout << "IDaqV775::SetChannelStatus: Invalid Channel Number: " << uiInputCh << endl;
			cout << "[V775] Set Channel " << uiInputCh << " Status Failure; Invalid Channel Number\n";
		}	
	}
    
    return;
}

void IDaqV775::Arm() {
	DataReset(); 
	ClearEventCount();
}

//int IDaqV775::Readout( vector<uint32_t>& vec_uiInputData ){
uint16_t IDaqV775::Readout( vector<uint32_t>& vec_uiInputData ){
	uint16_t uiRegValAllZeros( 0 );
	uint16_t nAdcGate( 0 );
	int iter( 0 );

	while ( IsBusy() ){
		if(m_bVerboseMode){ cout << "[V775] TDC in conversion... " << endl; }
		if ( IsFull() ) {
			if(m_bVerboseMode){ cout << "[V775] Buffer is full! Possible data loss." << endl; }
			break;
		}
        
        iter++;
		if ( iter > GetIterLimit() ){ 
			cerr << " *** [V775] Module looks dead ...Stopping Readout *** " << endl;
			m_bIsDead = true;
			break;
		} 
	}
	//cout << "Readout mode = " << GetReadoutMode()  << endl;
	switch( GetReadoutMode() ){
		case V775_D32:{
			uint32_t addr = m_baseAddr;
			uint32_t uiRegValDataWord = 0;
            
			CheckEventCountLow();
            if( m_vmeInt->GetStatus() != IDaqSuccess ){
                return 0;
            }
            
			nAdcGate = GetEventCountLow() + 1;
			if(m_bVerboseMode){ cout << "[V775] " << nAdcGate << " particle events to read in TDC " << showbase << hex << m_baseAddr << dec << endl; }
            
			int evCount = 0;
			while( evCount < nAdcGate ){
				m_vmeInt->Read( addr, uiRegValDataWord );
				//m_vmeInt->Read<uint32_t>( addr, uiRegValDataWord );
				DataWordV775 dw( uiRegValDataWord );
				if( dw.GetDataType() == (uint32_t)DWV775_EventHeader){
					vec_uiInputData.push_back( uiRegValDataWord );
                    
                    //This seems like it is not right... (BLD)
                    unsigned int uiNChan4Readout = dw.GetNChannel() + 1;
					for( unsigned int iChIdx = 0; iChIdx < uiNChan4Readout; ++iChIdx ){
						m_vmeInt->Read( addr, uiRegValDataWord );
						//m_vmeInt->Read<uint32_t>( addr, uiRegValDataWord );
						vec_uiInputData.push_back( uiRegValDataWord );
					}
				}
				++evCount;
			}
            
			break;
		}
		case V775_D32_EVTCOUNT: {
			CheckEventCountLow();
			if( m_vmeInt->GetStatus() != IDaqSuccess ) return -1;
			
			nAdcGate = GetEventCountLow() + 1;
			if(m_bVerboseMode){ cout << "[V775] " << nAdcGate << " particle events to read in TDC " << showbase << hex << m_baseAddr << dec << endl; }
			if( nAdcGate > 0 ) {
				for ( int iNEvt = 0; iNEvt < nAdcGate; ++iNEvt ){
					ReadAndWriteOutputBuffer( vec_uiInputData, 34 );
					SetRegister( V775_INCREMENT_EVENT, uiRegValAllZeros, uiRegValAllZeros );
					//SetRegister<uint16_t>( V775_INCREMENT_EVENT, uiRegValAllZeros, uiRegValAllZeros );
				}
				ClearEventCount();
			}
            
			break;
		}
		case V775_BLT32_BERR:
		default:
			//cerr << " V775 readout mode not implemented " << endl;
	            	cerr << "[V775] Readout Mode not implemented " << endl;
			break;
	}
  
	return nAdcGate;
}

//int IDaqV775::Readout( FILE *fp, uint64_t &fs ){
uint16_t IDaqV775::Readout( FILE *fp, uint64_t &fs ){
	uint16_t uiRegValAllZeros( 0 );
    uint16_t nAdcGate( 0 );
    int iter( 0 );

    while ( IsBusy() ){
        if(m_bVerboseMode){ cout << "[V775] TDC in conversion... " << endl; }
        if ( IsFull() ) {
            if(m_bVerboseMode){ cout << "[V775] Buffer is full! Possible data loss." << endl; }
            break;
        }

        iter++;
        if ( iter > GetIterLimit() ){ 
            cerr << " *** [V775] Module looks dead ...Stopping Readout *** " << endl;
	    m_bIsDead = true;
            break;
        }
    }
  
    switch( GetReadoutMode() ){
        case V775_D32_EVTCOUNT:
            CheckEventCountLow();

            if( m_vmeInt->GetStatus() != IDaqSuccess ){
                return 0;
            }
            
            nAdcGate = GetEventCountLow() + 1;
	    if(m_bVerboseMode){ cout << "[V775] " << nAdcGate << " particle events to read in TDC " << showbase << hex << m_baseAddr << dec << endl; }
            if (nAdcGate>0) {
                for (int iNEvt=0; iNEvt< nAdcGate; iNEvt++){
                    ReadAndWriteOutputBuffer( fp, 34 );
                    SetRegister( V775_INCREMENT_EVENT, uiRegValAllZeros, uiRegValAllZeros );
                    //SetRegister<uint16_t>( V775_INCREMENT_EVENT, uiRegValAllZeros, uiRegValAllZeros );
                }
                
                ClearEventCount();
                fs += 34 * nAdcGate * V775_DATAWORDSIZE;
            }
            
            break;
    
        case V775_BLT32_BERR:
        default:
            //cerr << " V775 readout mode not implemented " << endl;
	    cerr << "[V775] Readout Mode not implemented " << endl;
            break;
    }
  
    DataWordV775::WriteDataBlockLimit( fp );
    fs += GetWordSize();
    
    return nAdcGate;
}

void IDaqV775::CheckBitReg1(){ 
	CheckRegister( V775_BIT_SET1, m_regBit1 ); 
}

uint16_t IDaqV775::GetBitReg1(){ 
	return m_regBit1; 
}

void IDaqV775::SetBitReg1( V775BitSet1Mask inputBitMask, IDaqSwitch inputSwitch ){
    uint32_t  addr;
    uint16_t  uiRegValBitMask = (uint16_t)( inputBitMask );
  
  
    switch ( inputSwitch ) {
        case IDaqEnable:
            addr = m_baseAddr + V775_BIT_SET1;
            m_vmeInt->Write( addr, uiRegValBitMask );
            //m_vmeInt->Write<uint16_t>( (m_baseAddr + V775_BIT_SET1), uiRegValBitMask );
            //m_vmeInt->Write<uint16_t>( addr, uiRegValBitMask );
            m_vmeStatus = m_vmeInt->GetStatus();
            if ( m_vmeStatus == IDaqSuccess){
                m_regBit1 = ( m_regBit1 | uiRegValBitMask );
            }
    
            break;
        case IDaqDisable:
            addr = m_baseAddr + V775_BIT_CLEAR1;
            m_vmeInt->Write( addr, uiRegValBitMask );
            //m_vmeInt->Write<uint16_t>( (m_baseAddr + V775_BIT_CLEAR1), uiRegValBitMask );
            //m_vmeInt->Write<uint16_t>( addr, uiRegValBitMask );
            m_vmeStatus = m_vmeInt->GetStatus();
            if ( m_vmeStatus == IDaqSuccess ){
                m_regBit1 = ( m_regBit1 & ~uiRegValBitMask ) ;
            }
            
            break;
        default:
            m_vmeStatus = IDaqInvalidParam;
            break;
    }
    
    return;
}

void IDaqV775::CheckBitReg2(){ 
	CheckRegister( V775_BIT_SET2, m_regBit2); 
}

uint16_t IDaqV775::GetBitReg2(){ 
	return m_regBit2;
}

void IDaqV775::SetBitReg2( V775BitSet2Mask inputBitMask, IDaqSwitch inputSwitch){
	uint32_t addr ;
    uint16_t uiRegValBitMask = ( uint16_t )( inputBitMask );
  
    switch ( inputSwitch ) {
        case IDaqEnable:
            addr = m_baseAddr + V775_BIT_SET2;
            m_vmeInt->Write( addr, uiRegValBitMask );
            //m_vmeInt->Write<uint16_t>( (m_baseAddr + V775_BIT_SET2), uiRegValBitMask );
            //m_vmeInt->Write<uint16_t>( addr, uiRegValBitMask );
            m_vmeStatus = m_vmeInt->GetStatus();
            if ( m_vmeStatus == IDaqSuccess ){
                m_regBit2 = ( m_regBit2 | uiRegValBitMask );
            }
            
            break;
        case IDaqDisable:
            addr = m_baseAddr + V775_BIT_CLEAR2;
            m_vmeInt->Write( addr, uiRegValBitMask );
            //m_vmeInt->Write<uint16_t>( (m_baseAddr + V775_BIT_CLEAR2), uiRegValBitMask );
            //m_vmeInt->Write<uint16_t>( addr, uiRegValBitMask );
            m_vmeStatus = m_vmeInt->GetStatus();
            if ( m_vmeStatus == IDaqSuccess){
                m_regBit2 = ( m_regBit2 & ~uiRegValBitMask );
            }
            
            break;
        default:
            m_vmeStatus = IDaqInvalidParam;
            break;
    }
    
    return;
}

void IDaqV775::SetFullScaleRange( unsigned int uiInputFSR ){
  uint32_t addr = m_baseAddr + V775_FULL_SCALE_RANGE;
    if( ( uiInputFSR <= 0xFF ) && ( uiInputFSR >= 0x1E ) ){
        uint16_t uiRegValFSR = uiInputFSR;
        //m_vmeInt->Write( addr, uiInputFSR );
	m_vmeInt->Write( addr, uiRegValFSR);
	//m_vmeInt->Write<uint16_t>( addr, uiRegValFSR);
        m_vmeStatus = m_vmeInt->GetStatus();
        
        if( m_vmeStatus == IDaqSuccess ){
            cout << "[V775] Full Scale Range set to " << showbase << hex << GetFullScaleRange() << dec << " = " << (double) 8.9/GetFullScaleRange() << " [nsec/bin]." << endl;
        }
        else{
            cerr << "*** [V775] Full Scale Range error: " << (int) m_vmeStatus << endl;
        }
    }
    else {
        cerr << "*** [V775] Full Scale Range is outside limits [ 0x1E; 0xFF ]" << endl;
    }
    
    return;
}

unsigned int IDaqV775::GetFullScaleRange(){
    uint32_t addr = m_baseAddr + V775_FULL_SCALE_RANGE;
    uint16_t uiRegValFSR = 0;
    m_vmeInt->Read( addr, uiRegValFSR );
    //m_vmeInt->Read<uint16_t>( addr, uiRegValFSR );
    m_vmeStatus = m_vmeInt->GetStatus();
    if ( m_vmeStatus == IDaqSuccess ){
        return uiRegValFSR;
    }
    else{
        return 0;
    }
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

