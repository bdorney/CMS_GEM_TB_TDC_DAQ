#include "IDaqV513.h"

using std::bitset;
using std::cout;
using std::dec;
using std::endl;
using std::hex;

IDaqV513::IDaqV513(){
	IOReg.reset();
	
}

IDaqV513::~IDaqV513(){
	
}

void IDaqV513::Clear(){
	uint32_t addr;
	uint16_t dummy( 1 );
	addr = ba + V513_CLEAR_INPUT_REGISTER;
	vmeInt->Write( addr, dummy );
	status = vmeInt->GetStatus();
	cout << "Status = " << status << endl;
	IOReg.reset();
}

void IDaqV513::Reset(){
	uint32_t addr;
	uint16_t dummy( 1 );
	cout << "[V513] Performing software reset. Please wait...";
	addr = ba + V513_MODULE_RESET;
	vmeInt->Write( addr, dummy );
	status = vmeInt->GetStatus();
	//sleep( 1 );
    std::this_thread::sleep_for (std::chrono::seconds(1));
	if( status == 0 ) cout << " Done." << endl;
}

int IDaqV513::CheckInputRegister(){
	uint32_t addr;
	uint16_t data( 0 );
	addr = ba + V513_INPUT_REGISTER;
	vmeInt->Read( addr, data );
	status = vmeInt->GetStatus();
	//cout << "Input reg = 0x" << hex << data << dec << endl;
	IOReg = data;
	return data;
}

int IDaqV513::GetInput( const int aChannel ){
	this->CheckInputRegister();
	return IOReg[ aChannel ];
}

void IDaqV513::SetOutput( const int aChannel, const uint16_t aData ){
	uint32_t addr;
	uint16_t data( 0 );
	IOReg[ aChannel ] = !!aData;
	data = (uint16_t)IOReg.to_ulong();
	//cout << "Going to write 0x" << hex << data << dec << " into output register" << endl;
	addr = ba + V513_OUTPUT_REGISTER;
	vmeInt->Write( addr, data );
	status = vmeInt->GetStatus();
}

void IDaqV513::SetChannelStatusReg( int aChannel, V513ChannelDirection aChDir, V513ChannelPolarity aChPol,
				    V513ChannelInputMode aChInMode, V513ChannelTransferMode  aChTransMode ){
  uint32_t addr;
  uint16_t data = this->GetChannelStatusReg( aChannel );
  cout << hex << "data = " << data << dec << endl;
  bitset<16> Reg = data;

  Reg[ 0 ] = !!aChDir;
  Reg[ 1 ] = !!aChPol;
  Reg[ 2 ] = !!aChInMode;
  Reg[ 3 ] = !!aChTransMode;

  data = (uint16_t)Reg.to_ulong();
  addr = ba + V513_CHANNEL_STATUS_REG_START + (2 * aChannel);
  cout << "[V513] Setting reg @ addr " << hex << addr << " to " << data << dec << endl; 

  vmeInt->Write( addr, data );
  status = vmeInt->GetStatus();	
  cout << "Status = " << status << endl;
  data = this->GetChannelStatusReg( aChannel );
  cout << hex << "data = " << data << dec << endl;
}


int IDaqV513::GetChannelStatusReg( int aChannel ){
	uint32_t addr( ba + V513_CHANNEL_STATUS_REG_START + (2 * aChannel) );
	uint16_t data( 0 );
	cout << "[V513] Reading from address " << hex << addr << dec << endl;
	vmeInt->Read( addr, data );
	status = vmeInt->GetStatus();
	return data;
}

void IDaqV513::ClearStrobe(){
	uint32_t addr;
	uint16_t dummy( 1 );
	addr = ba + V513_CLEAR_STROBE_BIT;
	vmeInt->Write( addr, dummy );
	status = vmeInt->GetStatus();
	IOReg.reset();
}

