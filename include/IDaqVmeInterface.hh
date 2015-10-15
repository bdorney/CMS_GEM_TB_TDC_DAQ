#ifndef __IDAQVMEINTERFACE_H_
#define __IDAQVMEINTERFACE_H_

#include "IDaqVmeTypes.hh"

class IDaqVmeInterface {
	public:
		IDaqVmeInterface( IDaqVmeModuleType aModule = idmV2718, short aLink = 0, short aBoardNum = 0, long aBaseAddress = 0 );
		virtual ~IDaqVmeInterface();
		
		//Base Address
		long GetBaseAddress(){ return baseAddress; };
		void SetBaseAddress( long aBaseAddress ){ baseAddress = aBaseAddress; };
		//Reset I/F
		void Reset();
		
		// Read functions
		virtual void Read( uint32_t aAddress, uint32_t &aData);
  	virtual void Read( uint32_t aAddress, uint16_t &aData);
  virtual void ReadBLT( uint32_t aAddress, uint16_t &aData, const uint16_t nData, uint16_t &nDataRead){};
	  virtual void ReadBLT( uint32_t aAddress, uint32_t &aData, const uint16_t nData, uint16_t &nDataRead);
	  virtual void ReadBLT( uint32_t aAddress, uint64_t &aData, const uint16_t nData, uint16_t &nDataRead);
		
		// Write functions
  	virtual void Write( uint32_t aAddress, const uint32_t &aData);
  	virtual void Write( uint32_t aAddress, const uint16_t &aData);
  virtual void WriteA24(uint32_t, const uint16_t&);
		//Status
		virtual IDaqVmeCode GetStatus(){ return status; };
  virtual void ResetStatus(){ status = IDaqSuccess; };
  
		//Connection
		virtual void Connect();
  	virtual void Disconnect();
		
		//Registers
		virtual void CheckRegister( CVRegisters reg );
		
		virtual void SetOutputLevel( int aOutput, IDaqSwitch sw );
		virtual void CheckOuputConf( int aOutput );
		virtual void SetOutputSource( int aOutput, CVIOSources s);
	
		virtual void SetScalerConf();
		virtual void ResetScaler();
		virtual void CheckScaler();
		virtual unsigned short GetScaler(){ return scaler; };
		virtual void EnableScaler();
		virtual void DisableScaler();
		
		virtual uint16_t GetInputLevel( int aInput ) { return inputLevel[ aInput ]; }
  	virtual void CheckInputLevel();
		virtual void SetInputConf( int aInput, bool aPolarity );
		virtual void GetInputConf( int aInput, int &aPol, int &aLedPol ){ aPol = inPol[ aInput ]; aLedPol = inLedPol[ aInput ]; };
		virtual void CheckInputConf( int aInput = 0 );
		
  	
	private:
		IDaqVmeModuleType module;
		short link;
		short boardNum;
		long  baseAddress;
		long handle;
		bool isConnected;
		IDaqVmeCode status;
		uint16_t       inputLevel[ 3 ];
  	uint16_t       internalReg[ 44 ];
  	CVIOPolarity   outPol[ 5 ];
  	CVLEDPolarity  outLedPol[ 5 ];
  	CVIOSources    outSource[ 5 ];
		CVIOPolarity   inPol[ 2 ];
		CVLEDPolarity  inLedPol[ 2 ];
		unsigned short scaler;
};


#endif
