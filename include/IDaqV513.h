#ifndef __IDAQV513_H_
#define __IDAQV513_H_

//C++ Includes
#include <bitset>
#include <chrono>
#include <cstdio>
#include <thread>

//Framework Includes
#include <IDaqVmeTypes.h>
#include <IDaqVmeModule.h>

//ROOT Includes

class IDaqV513 : public IDaqVmeModule< uint32_t, uint16_t >
{
	public:
		IDaqV513();
		virtual ~IDaqV513();
		
		void Clear(); //Clear Input Register
		void Reset(); //Reset Module
		int CheckInputRegister();
		int GetInput( const int iInputCh );
		void SetOutput( const int iInputCh, const uint16_t uiInputRegVal );
		void SetChannelStatusReg( int iInputCh, V513ChannelDirection inputChDir, V513ChannelPolarity inputChPol, V513ChannelInputMode inputaChInMode, V513ChannelTransferMode inputChTransMode );
		int GetChannelStatusReg( int iInputCh );
		int GetInputReg(){
            CheckInputRegister();
            return m_IOReg.to_ulong();
        };
		void ClearStrobe();
		
  private:
    std::bitset<16> m_IOReg;
};

#endif
