#include "IDaqManager.h"

IDaqManager::IDaqManager(){
	maxEventCount = 2;
	eventCount = 0;
	fileSize = 0;
	
  stopRequest=false;
  statusBit = DAQ_NOTREADY;
  fp = NULL;
}

IDaqManager::~IDaqManager(){

}
		
void IDaqManager::SetVmeInterface( IDaqVmeInterface * aVmeInt){ 
	if( aVmeInt ){ 
		vmeInt = aVmeInt; 
		cout << "VME Interface set to "<< vmeInt << endl;
	}
}

int IDaqManager::AddModule( IDaqVmeVirtualModule * aModule ){
	modules.push_back( aModule );
	return modules.size() - 1; // return the index of new module
}

void IDaqManager::RemoveModule( unsigned int aIndex ){
	if( aIndex < modules.size() ){
		IDaqVmeVirtualModule * mod = modules[ aIndex ];
		modules.erase( modules.begin() + aIndex );
		delete mod;
		status = IDaqSuccess;
	} else status = IDaqInvalidParam;
}

void IDaqManager::Arm(){
	 //cout << "Status  " << showbase << hex << status << endl;
  // cout << showbase << hex << GetStatus() << endl;
  cout << " Arm DAQ " << endl;
  vmeInt->SetOutputSource( 4, cvManualSW);
  vmeInt->SetOutputLevel( 4, IDaqEnable);
  vmeInt->SetOutputLevel( 4, IDaqDisable);
	cout << "Trigger and Veto configured." << endl;
  vector< IDaqVmeVirtualModule* >::const_iterator it;
  
  for( it = modules.begin(); it != modules.end(); ++it ){ (*it)->Arm(); }
  
 	vmeInt->SetOutputLevel( 4, IDaqEnable );
  status = vmeInt->GetStatus();
  if ( status == IDaqSuccess ){
    SetStatusBit( DAQ_ARMED );
  } else {
    cout << "Error while arming DAQ. Check the Connection to the VME crate" << endl;
    ResetStatusBit( DAQ_TDCINIT );
    ResetStatusBit( DAQ_ADCINIT );
  }
}

void IDaqManager::StartDAQ(){
	cout << "Run Start" << endl;
  if ( GetStatusBit( DAQ_READY ) ) {
    Arm();
    fileSize = 0;
    
    if ( GetStatusBit( DAQ_ARMED ) && GetStatusBit( DAQ_DATAFILEOPEN ) ){
      SetStatusBit( DAQ_RUNNING );
      //DataWordMetaDAQ::WriteStartOfRun( fp, runNumber++ );
      fileSize += DAQ_DATAWORDSIZE;
      //DataWordMetaDAQ::WriteTimeStamp(_fp);
      fileSize += DAQ_TIMESTAMPSIZE;
      DaqLoop();
      //RunStop();
    }
  }
}

void IDaqManager::OpenDataFile(std::string fn){
	if ( fp ) {
  	cout << "Error : Data file already open" << endl;
    return;
  }
  fp = fopen( fn.c_str(), "wb" );
  if ( fp ) {
    dataFileName = fn;
    SetStatusBit( DAQ_DATAFILEOPEN );
  } else { cout << " Can not open data file " << endl; }
}

void IDaqManager::CloseDataFile(){
	if ( fp )  fclose( fp );
  fp=NULL;
  ResetStatusBit( DAQ_DATAFILEOPEN );
}

void IDaqManager::StopDAQ(){
	cout << "Run Stop" << endl;
	DataWordMetaDAQ::WriteTimeStamp( fp);
  fileSize += DAQ_TIMESTAMPSIZE;
	fileSize += DAQ_ENDOFFILESIZE;
	DataWordMetaDAQ::WriteEndOfFile( fp, fileSize);
  ResetStatusBit( DAQ_ARMED );
  ResetStatusBit( DAQ_RUNNING );
  CloseDataFile();
  ResetStatusBit( DAQ_DATAFILEOPEN );
  vmeInt->SetOutputLevel( 4, IDaqDisable );    // Drop the busy
}

void IDaqManager::DaqLoop(){
	// ofstream nDaqEvtLog;
  long ttot( 0 ), tread( 0 ), thead( 0 );
  uint64_t fs;
  uint16_t wc;
  timeval t0, t1, t2, t3;
  timespec rt, at;
  rt.tv_sec = 0;
  //  rt.tv_nsec = 100000;
  rt.tv_nsec = 50000;

//  _nPartEvt = 0;
  eventCount = 0;
  int nPart( 0 );
  cout << " Acq Loop" << endl;
  while( ( eventCount     <  maxEventCount )  &&
         //( nPartEvt    <  nPartEvtMax) &&
         ( stopRequest == false ) ){
		if( eventCount % 10 == 0 ) cout << eventCount << " events read." << endl;
		gettimeofday( &t0, NULL );
	 	vmeInt->CheckInputLevel();
   	if ( vmeInt->GetStatus() != IDaqSuccess ) break;
   	if ( vmeInt->GetInputLevel( 0 ) ){   // Trigger received
			cout << "Trigger received!" << endl;
			vmeInt->SetOutputLevel( 4, IDaqDisable );
    	if ( vmeInt->GetStatus() != IDaqSuccess ) break;      // Drop the busy      

      cout << " DAQ Trigger Received: Event " << eventCount << endl;
      DataWordMetaDAQ::WriteStartOfEvent( fp, eventCount );
      fileSize += DAQ_DATAWORDSIZE;

      // Write time stamp for each event commented out, takes 10 ms!
      //      DataWordMetaDAQ::WriteTimeStamp(_fp);
      //      _fileSize += METABTF_TIMESTAMPSIZE;

      fs = fileSize;
			
			gettimeofday( &t1, NULL );
      
      vector< IDaqVmeVirtualModule* >::const_iterator it;
      for( it = modules.begin(); it != modules.end(); ++it ) {
				DataWordMetaDAQ::WriteStartOfDataBlock( fp,
					 (*it)->GetModuleType(),
					 (*it)->GetWordSize() );
				fileSize += DAQ_DATAWORDSIZE;
				
				nPart = ( *it )->Readout( fp, fileSize );
				
				switch ( nPart ) {
					case -1:
	  				stopRequest = true; return; 
	  				break;
					case 0:
	  				cout << " No particle in DAQ event: " << eventCount;
	  				cout << " in module " << ( *it )->GetName() << endl; 
	  				break;
					case 1:
	  				break;
					default:
	  				cout << " More than 1 particle in DAQ event " <<  eventCount;
	  				cout << " in module " << ( *it )->GetName() << endl; 
	  				break;
				}
      }

      gettimeofday( &t2, NULL );

      //_nPartEvt += nPart;
      eventCount++;

      wc = ( fileSize - fs) / DAQ_DATAWORDSIZE ;
      DataWordMetaDAQ::WriteEndOfEvent( fp, wc );
      fileSize += DAQ_DATAWORDSIZE;
     
      fflush( fp );

      if ( nanosleep( &rt, &at ) != 0 ) cout << "Nanosleep failed" << endl;
    
      //      nDaqEvtLog.open(".MDnDaqEvtLog.txt",ios::out);
      //      nDaqEvtLog << _nDaqEvt;
      //      nDaqEvtLog.close();
      vmeInt->SetOutputLevel( 4, IDaqEnable );    // release the busy
      
      gettimeofday( &t3, NULL );
      ttot  += t3.tv_usec - t0.tv_usec + 1000000 * ( t3.tv_sec - t0.tv_sec );
      thead += t1.tv_usec - t0.tv_usec + 1000000 * ( t1.tv_sec - t0.tv_sec );
      tread += t2.tv_usec - t1.tv_usec + 1000000 * ( t2.tv_sec - t1.tv_sec );
      
    }
  }
  if ( stopRequest ) {
    cout << "Acq Stop Request Received " << endl;
    stopRequest = false;
  }
  cout << "AcqLoop time: " << ( ( double ) ttot ) / eventCount << " usec" << endl;
  cout << "      Header: " << ( ( double ) thead ) / eventCount << " usec" << endl;
  cout << "     Readout: " << ( ( double ) tread ) / eventCount << " usec" << endl;
}

//================================= Meta word

DataWordMetaDAQ::DataWordMetaDAQ(uint32_t d):DataWord<uint32_t>( d ){}

uint32_t DataWordMetaDAQ::GetDataType(){
  //  return (( data>>60 ) & 0xF); 
  return (( data >>28 ) & 0xF);
}

uint32_t DataWordMetaDAQ::GetEventCount(){
  if ( GetDataType() != (uint32_t) DW_StartOfEvent ) return DW_InvalidData;
  return ( data & 0xFFFF );
}

uint32_t DataWordMetaDAQ::GetWordCount(){
  if ( GetDataType() != (uint32_t) DW_EndOfEvent ) return DW_InvalidData;
  return ( data & 0xFFFF );
}

uint32_t DataWordMetaDAQ::GetWordCountStatus(){
  if ( GetDataType() != (uint32_t) DW_StartOfDataBlock ) return DW_InvalidData;
  return ( (data>>19) & 0x1 );
}

uint32_t DataWordMetaDAQ::GetWordSize(){
  if ( GetDataType() != (uint32_t) DW_StartOfDataBlock ) return DW_InvalidData;
  return ( (data>>12) & 0xFF );
}

uint32_t DataWordMetaDAQ::GetVmeModuleType(){
  if ( GetDataType() != (uint32_t) DW_StartOfDataBlock ) return DW_InvalidData;
  return ( (data>>20) & 0xFF );
}

uint32_t DataWordMetaDAQ::GetRunNumber(){
  if ( GetDataType() != (uint32_t) DW_StartOfRun ) return DW_InvalidData;
  return ( data & 0xFFFF );
}

uint32_t DataWordMetaDAQ::GetFileSize(){
  if ( GetDataType() != (uint32_t) DW_EndOfFile ) return DW_InvalidData;
  return ( (data>>4) & 0xFFFFFF );
}

void DataWordMetaDAQ::WriteTimeStamp(FILE *fp){
  uint32_t data= DW_StartOfTimeStamp;
  data = data<<28;
  fwrite(&data,sizeof(data),1,fp);
  /*  time_t timeval;
  timeval = time(NULL);
  fwrite(&timeval,sizeof(timeval),1,fp);
  */
  timeval timev;
  gettimeofday(&timev,NULL);

    time_t timer;
    timer = timev.tv_sec;
    cout << ctime(&timer) << ":" << dec << timev.tv_usec << endl ;

  fwrite(&timev,sizeof(timev),1,fp);                                       
}

void DataWordMetaDAQ::WriteStartOfEvent(FILE *fp, uint16_t ec){
  uint32_t data= DW_StartOfEvent;
  uint32_t evtc= ec;
  data = data<<28;
  data = data | evtc;
  fwrite(&data,sizeof(data),1,fp);
}

void DataWordMetaDAQ::WriteEndOfEvent(FILE *fp, uint16_t wc){
  uint32_t data= DW_EndOfEvent;
  uint32_t wordc= wc;
  data = data<<28;
  data = data | wordc;
  fwrite(&data,sizeof(data),1,fp);
}

void DataWordMetaDAQ::WriteStartOfRun(FILE *fp, uint16_t rn){
  uint32_t data= DW_StartOfRun;
  uint32_t runn= rn;
  data = data<<28;
  data = data | runn;
  fwrite(&data,sizeof(data),1,fp);
}

void DataWordMetaDAQ::WriteEndOfFile(FILE *fp, uint64_t fs){
  uint32_t data;
  //if ( fs > 0xFFFFFFFFFFFF  ) cout << "WriteEndOfFile: Exceeding file size limit" << endl;
  if ( fs > ULONG_MAX  ) cout << "WriteEndOfFile: Exceeding file size limit" << endl;
  
  for (int i=0; i<2; i++) {
    data= DW_EndOfFile;
    data = data<<24;
    data = data | (uint32_t)(fs & 0xFFFFFF);
    data = data<<4;
    fwrite(&data,sizeof(data),1,fp);
    fs = fs>>24;
  }
}

void DataWordMetaDAQ::WriteStartOfDataBlock(FILE *fp,uint16_t mt,uint16_t ws){
  uint32_t data= DW_StartOfDataBlock;
  data = data<<8;
  data = data | (uint32_t)(mt & 0xFF);
  data = data<<8;
  data = data | (uint32_t)(ws & 0xFF);
  data = data<<12;
  fwrite(&data,sizeof(data),1,fp);
}

ostream &operator<<(ostream &s,DataWordMetaDAQ &dw){
  // temporary

  uint32_t dt = dw.GetDataType();
  s << dec ;
  s << " BTF: ";
  switch (dt) {
  case DW_StartOfEvent:
    s << " New Event: "; 
    s << "Event Number: " << dw.GetEventCount();
    s << endl ;
    break;
  case DW_EndOfEvent:
    s << " End Of Event: "; 
    s << "Word Count: " << dw.GetWordCount();
    s << endl ;
    break;
   case DW_StartOfTimeStamp:
    s << " TimeStamp: " << endl ;
    break;
  case DW_StartOfRun:
    s << " Start of Run: " ;
    s << "Run Number: " << dw.GetRunNumber();
    s << endl ;
    break;
   case DW_StartOfDataBlock:
     s << " Data Block Info: ";
     s << "Module: " << hex << showbase << dw.GetVmeModuleType();
     s << " ; Word Size: " << dec << dw.GetWordSize();
     s << endl ;
    break;
   case DW_EndOfFile:
     s << " EndOfFile: "; 
     s <<  dw.GetFileSize() << endl ;
    break;
   default:
    s << "Data Type not implemented" << endl;
    break;
  }
  return s;
}

istream &operator>>(istream &s,DataWordMetaDAQ &dw){
  uint32_t d;
  s >> d;
  dw = DataWordMetaDAQ(d);
  return s; 
}

