#ifndef __IDAQMANAGER_H_
#define __IDAQMANAGER_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h> // needed for gettimeofday
#include <time.h>     // needed for nanosleep

#include <unistd.h>   // needed for sleep

#include <string>
#include <iostream>
#include <fstream>

#include <vector>

#include "IDaqVmeTypes.hh"
#include "IDaqVmeInterface.hh"
#include "IDaqVmeModule.hh"

//using namespace std;

typedef enum IDaqAcqStatusMask {
  DAQ_NOTREADY       = 0x00,
  DAQ_READY          = 0x80,
  DAQ_DATAFILEOPEN   = 0x40,
  DAQ_ARMED          = 0x20,
  DAQ_TDCINIT        = 0x08,
  DAQ_ADCINIT        = 0x04,
  DAQ_RUNNING        = 0x01,
  DAQ_ALL            = 0xFF
} IDaqAcqStatusMask;


class IDaqManager{
	public:
		IDaqManager();
		virtual ~IDaqManager();
		
		void SetVmeInterface( IDaqVmeInterface * aVmeInt );
		int AddModule( IDaqVmeVirtualModule * aModule );
		void RemoveModule( unsigned int aIndex );
		
		void OpenDataFile(std::string fn);
		void CloseDataFile();
		
		void Arm();
		void StartDAQ();
		void StopDAQ();
		
		void SetMaxEventsCount( long aMaxCount ){ maxEventCount = aMaxCount; };
		long GetMaxEventsCount(){ return maxEventCount; };
		long GetEventsCount(){ return eventCount; };
		
		IDaqVmeCode GetStatus(){ return status; };
		uint16_t GetStatusBit ( IDaqAcqStatusMask sm  = DAQ_ALL) { return ( statusBit & sm ); };
  	uint16_t SetStatusBit( IDaqAcqStatusMask bm){ statusBit = ( statusBit |  bm ); return statusBit; };
  	uint16_t ResetStatusBit( IDaqAcqStatusMask bm){ statusBit = ( statusBit & ~bm ); return statusBit; };
  	
  	void StopRequest(){ stopRequest = true; }
  	bool IsRunning()  { return ( bool )( statusBit & DAQ_RUNNING); }
		
	private:
		void DaqLoop();
		long maxEventCount;
		long eventCount;
		vector< IDaqVmeVirtualModule * > modules;
		IDaqVmeInterface * vmeInt;
		IDaqVmeCode status;  
  	string dataFileName;
  	FILE *fp;
  	uint64_t fileSize;
		volatile bool stopRequest;  
  	volatile uint16_t statusBit;
};


class DataWordMetaDAQ : public DataWord< uint32_t > {

 public:
  
  DataWordMetaDAQ( uint32_t d = 0 );
  ~DataWordMetaDAQ(){}
  uint32_t GetDataType();
  uint32_t GetEventCount();
  uint32_t GetRunNumber ();
  uint32_t GetWordCount();
  uint32_t GetWordCountStatus();
  uint32_t GetFileSize();
  uint32_t GetVmeModuleType();
  uint32_t GetWordSize();


  static  void WriteEndOfFile( FILE *fp, uint64_t fs = 0 );
  static  void WriteStartOfRun( FILE *fp, uint16_t rn = 0 );
  static  void WriteStartOfEvent( FILE *fp, uint16_t ec = 0 );
  static  void WriteEndOfEvent( FILE *fp, uint16_t wc = 0 );
  static  void WriteTimeStamp( FILE *fp );
  static  void WriteStartOfDataBlock( FILE *fp, uint16_t mt, uint16_t ws );

};



#endif


