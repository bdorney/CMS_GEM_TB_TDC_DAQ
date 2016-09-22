#include "IDaqV513.h"

using std::bitset;
using std::cout;
using std::dec;
using std::endl;
using std::fwrite;
using std::hex;
using std::showbase;

IDaqV513::IDaqV513(){
	m_IOReg.reset();
	
}

IDaqV513::~IDaqV513(){
    m_vmeInt.reset();
    m_data.reset();
    m_strName.clear();
}

void IDaqV513::Clear(){
	//uint32_t addr;
	uint16_t uiRegVal( 1 );
	//addr = m_baseAddr + V513_CLEAR_INPUT_REGISTER;
	//m_vmeInt->Write( addr, uiRegVal );
	m_vmeInt->Write<uint16_t>( (m_baseAddr + V513_CLEAR_INPUT_REGISTER), uiRegVal );
	m_vmeStatus = m_vmeInt->GetStatus();
	cout << "Status = " << m_vmeStatus << endl;
	m_IOReg.reset();
    
    return;
}

void IDaqV513::Reset(){
	uint32_t addr;
	uint16_t uiRegVal( 1 );
	cout << "[V513] Performing software reset. Please wait...";
	addr = m_baseAddr + V513_MODULE_RESET;
	m_vmeInt->Write<uint16_t>( addr, uiRegVal );
	m_vmeStatus = m_vmeInt->GetStatus();
    std::this_thread::sleep_for (std::chrono::seconds(1));
	if( m_vmeStatus == 0 ) cout << " Done." << endl;
    
    return;
}

int IDaqV513::CheckInputRegister(){
	uint32_t addr;
	uint16_t uiRegVal( 0 );
	addr = m_baseAddr + V513_INPUT_REGISTER;
	m_vmeInt->Read<uint16_t>( addr, uiRegVal );
	m_vmeStatus = m_vmeInt->GetStatus();
	m_IOReg = uiRegVal;
	return uiRegVal;
}

int IDaqV513::GetInput( const int iInputCh ){
	this->CheckInputRegister();
	return m_IOReg[ iInputCh ];
}

void IDaqV513::SetOutput( const int iInputCh, const uint16_t uiInputRegVal ){
	uint32_t addr;
	uint16_t uiRegVal( 0 );
	m_IOReg[ iInputCh ] = !!uiInputRegVal;
	uiRegVal = (uint16_t)m_IOReg.to_ulong();
	//cout << "Going to write 0x" << hex << data << dec << " into output register" << endl;
	addr = m_baseAddr + V513_OUTPUT_REGISTER;
	m_vmeInt->Write<uint16_t>( addr, uiRegVal );
	m_vmeStatus = m_vmeInt->GetStatus();
    
    return;
}

void IDaqV513::SetChannelStatusReg( int iInputCh, V513ChannelDirection inputChDir,
    V513ChannelPolarity inputChPol,
    V513ChannelInputMode inputChInMode,
    V513ChannelTransferMode  inputChTransMode ){
  uint32_t addr;
  uint16_t uiRegVal = this->GetChannelStatusReg( iInputCh );
  cout << "data = " << showbase << hex << uiRegVal << dec << endl;
  bitset<16> Reg = uiRegVal;

  Reg[ 0 ] = !!inputChDir;
  Reg[ 1 ] = !!inputChPol;
  Reg[ 2 ] = !!inputChInMode;
  Reg[ 3 ] = !!inputChTransMode;

  uiRegVal = (uint16_t)Reg.to_ulong();
  addr = m_baseAddr + V513_CHANNEL_STATUS_REG_START + (2 * iInputCh);
  cout << "[V513] Setting reg @ addr " << showbase<< hex << addr << " to " << uiRegVal << dec << endl;

  m_vmeInt->Write<uint16_t>( addr, uiRegVal );
  m_vmeStatus = m_vmeInt->GetStatus();	
  cout << "Status = " << m_vmeStatus << endl;
  uiRegVal = this->GetChannelStatusReg( iInputCh );
  cout << "data = " << showbase << hex << uiRegVal << dec << endl;
    
    return;
}

int IDaqV513::GetChannelStatusReg( int iInputCh ){
	uint32_t addr( m_baseAddr + V513_CHANNEL_STATUS_REG_START + (2 * iInputCh) );
	uint16_t uiRegVal( 0 );
	cout << "[V513] Reading from address " << showbase << hex << addr << dec << endl;
	m_vmeInt->Read<uint16_t>( addr, uiRegVal );
	m_vmeStatus = m_vmeInt->GetStatus();
	return uiRegVal;
}

void IDaqV513::ClearStrobe(){
	uint32_t addr;
	uint16_t uiRegVal( 1 );
	addr = m_baseAddr + V513_CLEAR_STROBE_BIT;
	m_vmeInt->Write<uint16_t>( addr, uiRegVal );
	m_vmeStatus = m_vmeInt->GetStatus();
	m_IOReg.reset();
}

