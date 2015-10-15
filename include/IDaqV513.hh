#ifndef __IDAQV513_H_
#define __IDAQV513_H_

#include <IDaqVmeTypes.hh>
#include <IDaqVmeModule.hh>
#include <bitset>

class IDaqV513 : public IDaqVmeModule< uint32_t, uint16_t >
{
	public:
		IDaqV513();
		virtual ~IDaqV513();
		
		void Clear(); //Clear Input Register
		void Reset(); //Reset Module
		int CheckInputRegister();
		int GetInput( const int aChannel );
		void SetOutput( const int aChannel, const uint16_t aData );
		void SetChannelStatusReg( int aChannel, V513ChannelDirection aChDir, V513ChannelPolarity aChPol, V513ChannelInputMode aChInMode, V513ChannelTransferMode  aChTransMode );
		int GetChannelStatusReg( int aChannel );
		int GetInputReg(){ CheckInputRegister(); return IOReg.to_ulong(); };
		void ClearStrobe();
		
  private:
    std::bitset<16> IOReg;
};

#endif
