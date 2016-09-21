#ifndef __IDAQV792_H_
#define __IDAQV792_H_

#include <IDaqVmeTypes.h>
#include <IDaqVmeModule.h>

class IDaqV792 : public IDaqVmeModule< uint32_t, uint32_t >
{
	public:
		IDaqV792();
		virtual ~IDaqV792();
		
		void DataReset();
  	void SoftwareReset();
  	
  	bool IsBusy();
  	bool DReady();
  	bool Evrdy();
   	bool IsPurged();
  	bool IsFull();
  	
  	uint16_t GetGeoAddress(){ return geoAddress; }
  	void CheckGeoAddress();
  	
  	uint16_t GetEventCountLow();
		uint16_t GetEventCountHigh();
		uint32_t GetEventCount();
		
		void CheckEventCount();
  	void CheckEventCountLow();
  	void CheckEventCountHigh();
  	void ClearEventCount();
		  
		virtual void Initialize();
		virtual void Initialize(V792AcqMode am, V792ReadoutMode rm, uint16_t ped, unsigned int th);
		
		virtual void Arm();
		virtual int Readout( FILE *fp, uint64_t &fs );
		
		
		void CheckBitReg1();
  	uint16_t GetBitReg1();
    void SetBitReg1( V792BitSet1Mask bm, IDaqSwitch s );
		
		void CheckBitReg2();
  	uint16_t GetBitReg2();
  	void SetBitReg2( V792BitSet2Mask bm, IDaqSwitch s );
  	
  	void CheckPedestal(){ CheckRegister( V792_IPED, iPed ); }
 		void SetPedestal( uint16_t ped );
  	uint16_t GetPedestal(){return iPed;}
  	
  	void CheckThresholds();
  	int16_t GetThresholdValue( unsigned int ch );
  	
  	IDaqSwitch GetChannelStatus( unsigned int ch);
  	void SetThresholdValue( unsigned int ch, unsigned int th );
  	void SetChannelStatus( unsigned int ch, IDaqSwitch s );
  	
  	int GetIterLimit(){ return iterLimit; }
  	V792ReadoutMode GetReadoutMode(){ return readoutMode; }
  	
  	
	private:
		V792AcqMode      acqMode;
		V792ReadoutMode  readoutMode;
		
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

class DataWordV792 : public DataWord< uint32_t > {

 public:
  
  DataWordV792( uint32_t d = 0 );
  ~DataWordV792(){}
  uint32_t GetDataType();

	uint32_t GetMeasurement();
	uint32_t GetMeasurementStatus();

  uint32_t GetChannel();
  uint32_t GetNChannel();
  uint32_t GetCrateNumber();
  
  uint32_t GetEventCount();
  uint32_t GetGeo();

  static  void WriteDataBlockLimit( FILE *fp );

};

ostream &operator<< ( ostream &s, DataWordV792 &dw );
istream &operator>> ( istream &s, DataWordV792 &dw );

#endif

