#include "IDaqVmeInterface.hh"

IDaqVmeInterface::IDaqVmeInterface( IDaqVmeModuleType aModule, short aLink, short aBoardNum, long aBaseAddress ){
	isConnected = false;
	handle = 0;
	boardNum = aBoardNum;
	link = aLink;
	module = aModule;
}

IDaqVmeInterface::~IDaqVmeInterface(){
	if( isConnected ) this->Disconnect();
}

void IDaqVmeInterface::Read( uint32_t aAddress, uint32_t &aData){
	status = (IDaqVmeCode)CAENVME_ReadCycle( handle, (unsigned long)aAddress, &aData, cvA32_U_DATA, cvD32);
}

void IDaqVmeInterface::Read( uint32_t aAddress, uint16_t &aData){
	status = (IDaqVmeCode)CAENVME_ReadCycle( handle, (unsigned long)aAddress, &aData, cvA32_U_DATA, cvD16);
}


void IDaqVmeInterface::ReadBLT( uint32_t aAddress, uint32_t &aData, const uint16_t nData, uint16_t &nDataRead){
	int nbread(0);
  status = (IDaqVmeCode)CAENVME_BLTReadCycle( handle, (unsigned long)aAddress, &aData, nData * 4, cvA32_U_BLT, cvD32, &nbread );
  nDataRead = nbread / sizeof( aData );
}

void IDaqVmeInterface::ReadBLT( uint32_t aAddress, uint64_t &aData, const uint16_t nData, uint16_t &nDataRead){
	int nbread( 0 );
  status = (IDaqVmeCode)CAENVME_BLTReadCycle( handle, ( unsigned long )aAddress, &aData, nData * 8, cvA32_U_MBLT, cvD64,&nbread );
  nDataRead = nbread/ sizeof( aData );
}

void IDaqVmeInterface::Write( uint32_t aAddress, const uint32_t &aData){
	status = ( IDaqVmeCode ) CAENVME_WriteCycle( handle, ( unsigned long ) aAddress, ( void* ) &aData, cvA32_U_DATA, cvD32 );
}

void IDaqVmeInterface::Write( uint32_t aAddress, const uint16_t &aData){
  //  cout << "Writing A24 D16 ... " << endl; 
	status = ( IDaqVmeCode ) CAENVME_WriteCycle( handle, ( unsigned long ) aAddress, ( void* ) &aData, cvA32_U_DATA, cvD16 );
}

void IDaqVmeInterface::WriteA24( uint32_t aAddress, const uint16_t &aData){
  //  cout << "Writing A24 D16 ... " << endl;
  status = ( IDaqVmeCode ) CAENVME_WriteCycle( handle, ( unsigned long ) aAddress, ( void* ) &aData, cvA24_U_DATA, cvD16 );
}


void IDaqVmeInterface::Connect(){
  long BHandle;
  if( !isConnected ){
  	
  	CVBoardTypes type = cvV2718;
  	switch( module ){
  		case idmV2718: type = cvV2718; break;
  		case idmV1718: type = cvV1718; break;
  		case idmGenericModule:
  		case idmV787:
  		case idmV792:
  		case idmV792N:
  		default: status = IDaqInvalidParam;
		}  	
		CVErrorCodes stat = CAENVME_Init( type, link, boardNum, &BHandle );
	  if(  stat != cvSuccess )  {
  	 	std::cout << std::endl << std::endl << " Error opening the device with following parameters: " << std::endl
  	 						<< "Module Type = " << (int)module << std::endl << "Link = " << link << std::endl 
  	 						<< "Board Number = " << boardNum << std::endl;
    	status = (IDaqVmeCode)stat; 
  	} else {
	  	isConnected = true;
  		handle = BHandle;
  		status = IDaqSuccess;
  	}
	}
}

void IDaqVmeInterface::Disconnect(){
	if( isConnected ){  
  	status = IDaqCommError; 
  	status = (IDaqVmeCode)CAENVME_End( handle );
  	isConnected = false;
  	handle = 0;
  }
}

void IDaqVmeInterface::Reset(){
	status = (IDaqVmeCode)CAENVME_SystemReset( handle );
}

void IDaqVmeInterface::CheckRegister( CVRegisters reg ) {
  unsigned short data;
	//cout << "Going to check register " << reg << endl;
  status = (IDaqVmeCode)CAENVME_ReadRegister( handle, reg, &data );
	//cout << " Status = " << " Data = " << data << endl;
  if ( status == IDaqSuccess) internalReg[ reg ] = data;
	//cout << "Done." << endl;
}

void IDaqVmeInterface::CheckInputLevel(){
	CheckRegister( cvInputReg );
	//cout << "Checking inputs... IntReg = " << internalReg[ cvInputReg ] <<  endl;
	
  if ( status == IDaqSuccess ){
	  inputLevel[ 0 ] = ( cvIn0Bit   & internalReg[ cvInputReg ] );
		//cout << "inputLevel[ 0 ] = " << inputLevel[ 0 ] << endl;
  	inputLevel[ 1 ] = ( cvIn1Bit   & internalReg[ cvInputReg ] );
  	inputLevel[ 2 ] = ( cvCoincBit & internalReg[ cvInputReg ] );
	} else { cerr << "Couldn't check the input level!" << endl; }
}

void IDaqVmeInterface::CheckInputConf( int aInput ){
	status = (IDaqVmeCode) CAENVME_GetInputConf( handle, (CVInputSelect)aInput,  &inPol[ aInput ], &inLedPol[ aInput ] );
}

void IDaqVmeInterface::SetInputConf( int aInput, bool aPolarity ){
	
	status = (IDaqVmeCode) CAENVME_SetInputConf( handle, ( CVInputSelect ) aInput, (CVIOPolarity) aPolarity, (CVLEDPolarity) aPolarity );
	if( status == IDaqSuccess ){
		CheckInputConf( aInput );
	}
	/*
	status = (IDaqVmeCode)CAENVME_SetInputConf( handle, cvInput0, cvInverted, cvActiveLow );
	status = (IDaqVmeCode)CAENVME_SetInputConf( handle, cvInput1, cvInverted, cvActiveLow );
	
	if( status == IDaqSuccess ){
		CheckInputConf( aInput );
	}
	*/
}

void IDaqVmeInterface::SetOutputLevel(int aOutput, IDaqSwitch sw ){
  unsigned short dc = 0x1 << ( 6 + aOutput );
  switch ( sw ) {
  case IDaqEnable:
    status = ( IDaqVmeCode ) CAENVME_SetOutputRegister( handle, dc );
    break;
  case IDaqDisable:
    status = (IDaqVmeCode)CAENVME_ClearOutputRegister( handle, dc );
    break;
  default:
    status = IDaqInvalidParam;
    break;
  }
}

void IDaqVmeInterface::CheckOuputConf( int aOutput ){
     status = (IDaqVmeCode) CAENVME_GetOutputConf( handle, (CVOutputSelect)aOutput, &outPol[ aOutput ], &outLedPol[ aOutput ], &outSource[ aOutput ] );
}


void IDaqVmeInterface::SetOutputSource( int aOutput, CVIOSources s){
	CheckOuputConf( aOutput );
  if( status == IDaqSuccess ){
     status = (IDaqVmeCode) CAENVME_SetOutputConf( handle, ( CVOutputSelect ) aOutput, outPol[ aOutput ], outLedPol[ aOutput ], s );
	}
}

void IDaqVmeInterface::SetScalerConf(){
	status = (IDaqVmeCode) CAENVME_SetScalerConf( handle, 1024, 0, cvInputSrc0, cvManualSW, cvManualSW );
}

void IDaqVmeInterface::ResetScaler(){
	status = (IDaqVmeCode) CAENVME_ResetScalerCount( handle );
}

void IDaqVmeInterface::EnableScaler(){
	status = (IDaqVmeCode) CAENVME_EnableScalerGate( handle );
}

void IDaqVmeInterface::DisableScaler(){
	status = (IDaqVmeCode) CAENVME_DisableScalerGate( handle );
}

void IDaqVmeInterface::CheckScaler(){
	CheckRegister( cvScaler1 );
	if( status == IDaqSuccess ){
		scaler = internalReg[ cvScaler1 ];
	} else { scaler = 6666; }
}
