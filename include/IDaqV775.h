#ifndef __IDAQV775_H_
#define __IDAQV775_H_

//C++ Includes
#include <chrono>
#include <thread>

//Framework Includes
#include <IDaqVmeTypes.h>
#include <IDaqVmeModule.h>

//ROOT Includes

class IDaqV775 : public IDaqVmeModule< uint32_t, uint32_t >
{
	public:
		IDaqV775();
		virtual ~IDaqV775();
		
		void DataReset();
  		void SoftwareReset();
  	
  		bool IsBusy();
  		bool DReady();
  		bool Evrdy();
   		bool IsPurged();
  		bool IsFull();
  	
  		uint16_t GetGeoAddress(){ return m_geoAddr; }
		void SetGeoAddress( uint16_t inputAddr );
		void CheckGeoAddress();
  	
  		uint16_t GetEventCountLow();
		uint16_t GetEventCountHigh();
		uint32_t GetEventCount();
		
		void CheckEventCount();
  		void CheckEventCountLow();
  		void CheckEventCountHigh();
  		void ClearEventCount();
		
		
		virtual void Initialize();
		virtual void Initialize( V775AcqMode inputModeAcq, V775ReadoutMode inputModeReadout, uint16_t uiInputPed, unsigned int uiInputThresh);
		
		virtual void Arm();
		virtual uint16_t Readout( FILE *fp, uint64_t &fs );
        virtual uint16_t Readout( std::vector<uint32_t>& vec_uiInputData );
		
		void CheckBitReg1();
        void SetBitReg1( V775BitSet1Mask inputBitMask, IDaqSwitch inputSwitch );
  		uint16_t GetBitReg1();
		
		void CheckBitReg2();
  		void SetBitReg2( V775BitSet2Mask inputBitMask, IDaqSwitch inputSwitch );
  		uint16_t GetBitReg2();
  	
        //void CheckPedestal(){ CheckRegister( V775_IPED, m_uiPed ); }
        //void SetPedestal( uint16_t uiInputPed );
        //uint16_t GetPedestal(){return m_uiPed;}
  	
  		void CheckThresholds();
  		int16_t GetThresholdValue( unsigned int uiInputCh );
  	
  		IDaqSwitch GetChannelStatus( unsigned int uiInputCh);
  		void SetThresholdValue( unsigned int uiInputCh, unsigned int uiInputThresh );
  		void SetChannelStatus( unsigned int uiInputCh, IDaqSwitch inputSwitch );
  	
        int GetIterLimit(){ return m_iIterLimit; }
        V775ReadoutMode GetReadoutMode(){ return m_modeReadout; }
		void SetFullScaleRange( unsigned int uiInputFSR );
		unsigned int GetFullScaleRange();
  	
	private:
		V775AcqMode      m_modeAcq;
		V775ReadoutMode  m_modeReadout;
		
		uint16_t m_uiPed;
		uint16_t m_uiThreshold[ 32 ];
		uint16_t m_geoAddr;
		
		uint16_t m_regCtrl;     //Control Reg
		uint16_t m_regBit1;     //Bit Reg 1
		uint16_t m_regBit2;     //Bit Reg 2
		uint16_t m_regStatus1;  //Status Reg 1
		uint16_t m_regStatus2;  //Status Reg 2
		
		
		
		uint16_t m_uiEvtCountLow;
		uint16_t m_uiEvtCountHigh;
		
		uint16_t m_uiFastCLRWindowWidth;
		uint16_t m_uifwRev;     //Firmware Revision
		
		uint16_t m_uiEvtTrig;
		uint16_t m_uiInterruptLevel;
		uint16_t m_uiInterruptVector;
		
		int m_iIterLimit;
};

class DataWordV775 : public DataWord< uint32_t > {

public:
  
  DataWordV775( uint32_t d = 0 );
  ~DataWordV775(){}
  uint32_t GetDataType();
	uint32_t GetRawData(){ return data; };
  uint32_t GetData();
  uint32_t GetDataStatus();

  uint32_t GetChannel();
  uint32_t GetNChannel();
  uint32_t GetCrateNumber();
  
  uint32_t GetEventCount();
  uint32_t GetGeo();

  static  void WriteDataBlockLimit( FILE *fp );

};

std::ostream &operator<< ( std::ostream &s, DataWordV775 &dw );
std::istream &operator>> ( std::istream &s, DataWordV775 &dw );

#endif

