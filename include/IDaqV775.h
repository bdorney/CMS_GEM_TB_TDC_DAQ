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
  	
  		uint16_t GetGeoAddress(){ return geoAddress; }
		void SetGeoAddress( uint16_t aGeo );
		void CheckGeoAddress();
  	
  		uint16_t GetEventCountLow();
		uint16_t GetEventCountHigh();
		uint32_t GetEventCount();
		
		void CheckEventCount();
  		void CheckEventCountLow();
  		void CheckEventCountHigh();
  		void ClearEventCount();
		
		
		virtual void Initialize();
		virtual void Initialize( V775AcqMode am, V775ReadoutMode rm, uint16_t ped, unsigned int th);
		
		virtual void Arm();
		virtual int Readout( FILE *fp, uint64_t &fs );
        virtual int Readout( std::vector<uint32_t>& aData );
		
		void CheckBitReg1();
  		uint16_t GetBitReg1();
    		void SetBitReg1( V775BitSet1Mask bm, IDaqSwitch s );
		
		void CheckBitReg2();
  		uint16_t GetBitReg2();
  		void SetBitReg2( V775BitSet2Mask bm, IDaqSwitch s );
  	
// 	void CheckPedestal(){ CheckRegister( V775_IPED, iPed ); }
// 		void SetPedestal( uint16_t ped );
//  	uint16_t GetPedestal(){return iPed;}
  	
  		void CheckThresholds();
  		int16_t GetThresholdValue( unsigned int ch );
  	
  		IDaqSwitch GetChannelStatus( unsigned int ch);
  		void SetThresholdValue( unsigned int ch, unsigned int th );
  		void SetChannelStatus( unsigned int ch, IDaqSwitch s );
  	
  	int GetIterLimit(){ return iterLimit; }
  	V775ReadoutMode GetReadoutMode(){ return readoutMode; }
		void SetFullScaleRange( unsigned int aFullRange );
		unsigned int GetFullScaleRange();
  	
	private:
		V775AcqMode      acqMode;
		V775ReadoutMode  readoutMode;
		
		uint16_t iPed;
		uint16_t threshold[ 32 ];
		uint16_t geoAddress;
		
		uint16_t controlReg;
		uint16_t bitReg1;
		uint16_t bitReg2;
		uint16_t statusReg1;
		uint16_t statusReg2;
		
		
		
		uint16_t eventCountLow;
		uint16_t eventCountHigh;
		
		uint16_t fastClearWindowWidth;
		uint16_t firmwareRevision;
		
		uint16_t eventTrigger;
		uint16_t interruptLevel;
		uint16_t interruptVector;
		
		int iterLimit;
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

