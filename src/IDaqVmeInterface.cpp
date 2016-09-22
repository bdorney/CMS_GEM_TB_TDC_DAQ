#include "IDaqVmeInterface.h"

using std::cerr;
using std::endl;

IDaqVmeInterface::IDaqVmeInterface( IDaqVmeModuleType aModule, short aLink, short aBoardNum, long aBaseAddress ) :
        m_uiInputLevel( new uint16_t[3] ),
        m_RegInternal( new uint16_t[44] ),
        m_cvOutIOPol( new CVIOPolarity[5] ),
        m_cvOutLedPol( new CVLEDPolarity[5] ),
        m_cvOutSource( new CVIOSources[5] ),
        m_cvInIOPol( new CVIOPolarity[2] ),
        m_cvInLedPol( new CVLEDPolarity[2] ) {
	m_bisConnected = false;
	m_lHandle = 0;
    m_baseAddr = aBaseAddress;
	m_sNumBoard = aBoardNum;
	m_sNumLink = aLink;
	m_vmeType = aModule;
}

IDaqVmeInterface::~IDaqVmeInterface(){
	if( m_bisConnected ) this->Disconnect();
    
    m_uiInputLevel.reset();
    m_RegInternal.reset();
    m_cvOutIOPol.reset();
    m_cvOutLedPol.reset();
    m_cvOutSource.reset();
    m_cvInIOPol.reset();
    m_cvInLedPol.reset();
}

/*void IDaqVmeInterface::Read( uint32_t aAddress, uint32_t &aData){
	m_vmeStatus = (IDaqVmeCode)CAENVME_ReadCycle( m_lHandle, (unsigned long)aAddress, &aData, cvA32_U_DATA, cvD32);
}*/

/*void IDaqVmeInterface::Read( uint32_t aAddress, uint16_t &aData){
	m_vmeStatus = (IDaqVmeCode)CAENVME_ReadCycle( m_lHandle, (unsigned long)aAddress, &aData, cvA32_U_DATA, cvD16);
}*/


/*void IDaqVmeInterface::ReadBLT( uint32_t aAddress, uint32_t &aData, const uint16_t nData, uint16_t &nDataRead){
	int nbread(0);
  m_vmeStatus = (IDaqVmeCode)CAENVME_BLTReadCycle( m_lHandle, (unsigned long)aAddress, &aData, nData * 4, cvA32_U_BLT, cvD32, &nbread );
  nDataRead = nbread / sizeof( aData );
}*/

/*void IDaqVmeInterface::ReadBLT( uint32_t aAddress, uint64_t &aData, const uint16_t nData, uint16_t &nDataRead){
	int nbread( 0 );
  m_vmeStatus = (IDaqVmeCode)CAENVME_BLTReadCycle( m_lHandle, ( unsigned long )aAddress, &aData, nData * 8, cvA32_U_MBLT, cvD64,&nbread );
  nDataRead = nbread/ sizeof( aData );
}*/

/*void IDaqVmeInterface::Write( uint32_t aAddress, const uint32_t &aData){
	m_vmeStatus = ( IDaqVmeCode ) CAENVME_WriteCycle( m_lHandle, ( unsigned long ) aAddress, ( void* ) &aData, cvA32_U_DATA, cvD32 );
}*/

/*void IDaqVmeInterface::Write( uint32_t aAddress, const uint16_t &aData){
	m_vmeStatus = ( IDaqVmeCode ) CAENVME_WriteCycle( m_lHandle, ( unsigned long ) aAddress, ( void* ) &aData, cvA32_U_DATA, cvD16 );
}*/

void IDaqVmeInterface::WriteA24( uint32_t aAddress, const uint16_t &aData){
  //  cout << "Writing A24 D16 ... " << endl;
  m_vmeStatus = ( IDaqVmeCode ) CAENVME_WriteCycle( m_lHandle, ( unsigned long ) aAddress, ( void* ) &aData, cvA24_U_DATA, cvD16 );
}


void IDaqVmeInterface::Connect(){
  long BHandle;
  if( !m_bisConnected ){
  	
  	CVBoardTypes type = cvV2718;
  	switch( m_vmeType ){
  		case idmV2718: type = cvV2718; break;
  		case idmV1718: type = cvV1718; break;
  		case idmGenericModule:
  		case idmV787:
  		case idmV792:
  		case idmV792N:
  		default: m_vmeStatus = IDaqInvalidParam;
		}  	
		CVErrorCodes stat = CAENVME_Init( type, m_sNumLink, m_sNumBoard, &BHandle );
	  if(  stat != cvSuccess )  {
  	 	std::cout << std::endl << std::endl << " Error opening the device with following parameters: " << std::endl
  	 						<< "Module Type = " << (int)m_vmeType << std::endl << "Link = " << m_sNumLink << std::endl 
  	 						<< "Board Number = " << m_sNumBoard << std::endl;
    	m_vmeStatus = (IDaqVmeCode)stat; 
  	} else {
	  	m_bisConnected = true;
  		m_lHandle = BHandle;
  		m_vmeStatus = IDaqSuccess;
  	}
	}
}

void IDaqVmeInterface::Disconnect(){
	if( m_bisConnected ){  
  	m_vmeStatus = IDaqCommError; 
  	m_vmeStatus = (IDaqVmeCode)CAENVME_End( m_lHandle );
  	m_bisConnected = false;
  	m_lHandle = 0;
  }
}

void IDaqVmeInterface::Reset(){
	m_vmeStatus = (IDaqVmeCode)CAENVME_SystemReset( m_lHandle );
}

void IDaqVmeInterface::CheckRegister( CVRegisters reg ) {
  unsigned short data;
	//cout << "Going to check register " << reg << endl;
  m_vmeStatus = (IDaqVmeCode)CAENVME_ReadRegister( m_lHandle, reg, &data );
	//cout << " m_vmeStatus = " << " Data = " << data << endl;
  if ( m_vmeStatus == IDaqSuccess) (m_RegInternal.get())[ reg ] = data;
	//cout << "Done." << endl;
}

void IDaqVmeInterface::CheckInputLevel(){
	CheckRegister( cvInputReg );
	//cout << "Checking inputs... IntReg = " << (m_RegInternal.get())[ cvInputReg ] <<  endl;
	
  if ( m_vmeStatus == IDaqSuccess ){
	  (m_uiInputLevel.get())[ 0 ] = ( cvIn0Bit   & (m_RegInternal.get())[ cvInputReg ] );
		//cout << "(m_uiInputLevel.get())[ 0 ] = " << (m_uiInputLevel.get())[ 0 ] << endl;
  	(m_uiInputLevel.get())[ 1 ] = ( cvIn1Bit   & (m_RegInternal.get())[ cvInputReg ] );
  	(m_uiInputLevel.get())[ 2 ] = ( cvCoincBit & (m_RegInternal.get())[ cvInputReg ] );
	} else { cerr << "Couldn't check the input level!" << endl; }
}

void IDaqVmeInterface::CheckInputConf( int aInput ){
	m_vmeStatus = (IDaqVmeCode) CAENVME_GetInputConf( m_lHandle, (CVInputSelect)aInput,  &(m_cvInIOPol.get())[ aInput ], &(m_cvInLedPol.get())[ aInput ] );
}

void IDaqVmeInterface::SetInputConf( int aInput, bool aPolarity ){
	
	m_vmeStatus = (IDaqVmeCode) CAENVME_SetInputConf( m_lHandle, ( CVInputSelect ) aInput, (CVIOPolarity) aPolarity, (CVLEDPolarity) aPolarity );
	if( m_vmeStatus == IDaqSuccess ){
		CheckInputConf( aInput );
	}
	/*
	m_vmeStatus = (IDaqVmeCode)CAENVME_SetInputConf( m_lHandle, cvInput0, cvInverted, cvActiveLow );
	m_vmeStatus = (IDaqVmeCode)CAENVME_SetInputConf( m_lHandle, cvInput1, cvInverted, cvActiveLow );
	
	if( m_vmeStatus == IDaqSuccess ){
		CheckInputConf( aInput );
	}
	*/
}

void IDaqVmeInterface::SetOutputLevel(int aOutput, IDaqSwitch sw ){
  unsigned short dc = 0x1 << ( 6 + aOutput );
  switch ( sw ) {
  case IDaqEnable:
    m_vmeStatus = ( IDaqVmeCode ) CAENVME_SetOutputRegister( m_lHandle, dc );
    break;
  case IDaqDisable:
    m_vmeStatus = (IDaqVmeCode)CAENVME_ClearOutputRegister( m_lHandle, dc );
    break;
  default:
    m_vmeStatus = IDaqInvalidParam;
    break;
  }
}

void IDaqVmeInterface::CheckOuputConf( int aOutput ){
     m_vmeStatus = (IDaqVmeCode) CAENVME_GetOutputConf( m_lHandle, (CVOutputSelect)aOutput, &(m_cvOutIOPol.get())[ aOutput ], &(m_cvOutLedPol.get())[ aOutput ], &(m_cvOutSource.get())[ aOutput ] );
}


void IDaqVmeInterface::SetOutputSource( int aOutput, CVIOSources s){
	CheckOuputConf( aOutput );
  if( m_vmeStatus == IDaqSuccess ){
     m_vmeStatus = (IDaqVmeCode) CAENVME_SetOutputConf( m_lHandle, ( CVOutputSelect ) aOutput, (m_cvOutIOPol.get())[ aOutput ], (m_cvOutLedPol.get())[ aOutput ], s );
	}
}

void IDaqVmeInterface::SetScalerConf(){
	m_vmeStatus = (IDaqVmeCode) CAENVME_SetScalerConf( m_lHandle, 1024, 0, cvInputSrc0, cvManualSW, cvManualSW );
}

void IDaqVmeInterface::ResetScaler(){
	m_vmeStatus = (IDaqVmeCode) CAENVME_ResetScalerCount( m_lHandle );
}

void IDaqVmeInterface::EnableScaler(){
	m_vmeStatus = (IDaqVmeCode) CAENVME_EnableScalerGate( m_lHandle );
}

void IDaqVmeInterface::DisableScaler(){
	m_vmeStatus = (IDaqVmeCode) CAENVME_DisableScalerGate( m_lHandle );
}

void IDaqVmeInterface::CheckScaler(){
	CheckRegister( cvScaler1 );
	if( m_vmeStatus == IDaqSuccess ){
		m_usScaler = (m_RegInternal.get())[ cvScaler1 ];
	} else { m_usScaler = 6666; }
}
