#include "DataWordV775.h"

using std::cerr;
using std::cout;
using std::dec;
using std::endl;
using std::flush;
using std::hex;
using std::istream;
using std::ostream;
using std::showbase;
using std::vector;

DataWordV775::DataWordV775( uint32_t d ) : DataWord< uint32_t >( d ){}

uint32_t DataWordV775::GetDataType(){
  return ( ( data>>24 ) & 0x7 );
}

uint32_t DataWordV775::GetData(){
	if ( (GetDataType() != ( uint32_t )DWV775_EventDatum ) ) return 0xFFFF;
  return ( data & 0x0FFF );
}

uint32_t DataWordV775::GetDataStatus(){
	if ( (GetDataType() != (uint32_t)DWV775_EventDatum ) ) return DWV775_InvalidData;
  return ( ( data >> 12 ) & 0x07 );
}

uint32_t DataWordV775::GetCrateNumber(){
	if ( ( GetDataType() != (uint32_t)DWV775_EventHeader ) ) return DWV775_InvalidData;
  return ( ( data >> 16 ) & 0xFF );
}

uint32_t DataWordV775::GetChannel(){
	if ( ( GetDataType() != ( uint32_t ) DWV775_EventDatum ) ) return DWV775_InvalidData;
  return ( ( data >> 16 ) & 0x3F );
}

uint32_t DataWordV775::GetNChannel(){
	if ( ( GetDataType() != (uint32_t)DWV775_EventHeader ) ) return DWV775_InvalidData;
  return ( ( data >> 8 ) & 0x3F );
}

uint32_t DataWordV775::GetGeo(){
  uint32_t dt = GetDataType();
	if ( ( dt != ( uint32_t ) DWV775_EventDatum ) && 
			 ( dt != ( uint32_t ) DWV775_EventHeader )&&  
			 ( dt != ( uint32_t ) DWV775_EventTrailer ) ) return DWV775_InvalidData;
  return ( ( data >> 27 ) & 0x1F );
}

uint32_t DataWordV775::GetEventCount(){
  if ( GetDataType() != ( uint32_t ) DWV775_EventTrailer ) return DWV775_InvalidData;
  return ( data & 0xFFFFFF );
}

void DataWordV775::WriteDataBlockLimit(FILE *fp){
	/*  uint32_t data = DWV775_DataBlockLimit;
  data = data << 24;
  fwrite( &data, sizeof( data ), 1, fp );
	*/
}

ostream &operator << ( ostream &s, DataWordV775 &dw){
  // temporary

  uint32_t dt= dw.GetDataType();
  s << showbase << hex;
  s << " CAEN V775:";

  switch (dt) {
  case DWV775_EventDatum:
    s << " Geo Address : " << dw.GetGeo() << " " ;
    s << "(" << dw.GetDataStatus()  << ") ";
    switch( dw.GetDataStatus() ){
    	case DWMSV775_OverFlow:
      	s << "OverFlow Measurement: ";
      	break;
    	case DWMSV775_UnderThreshold:
      	s << "Under Threshold Measurement: ";
      	break;
    	case DWMSV775_Normal:
      	s << "Measurement: ";
      	break;
    	default:
      	s << "Invalid Data!";
				//s << "Normal: ";
				//return s;
      	break;
    }
    s << dec << dw.GetData() << "; Channel: " ;
    s << dw.GetChannel() << endl;
    break;

  case DWV775_EventHeader:
    s << " Geo Address : " << dw.GetGeo() << " " ;
    s << "Global Header : " ;
    s <<  " Crate Number : " ;
    s << dec << dw.GetCrateNumber();
    s << "; N Channels " ;
    s <<  dw.GetNChannel() << endl;
    break;

  case DWV775_EventTrailer:
    s << " Geo Address : " << dw.GetGeo() << " " ;
    s << "Global Trailer : ";
    s << dec << " Event Count : " ;
    s << dw.GetEventCount() << endl;
    break;
  
  case DWV775_DataFiller:
		s << "No data: " << showbase << hex << dw.GetRawData() << dec << endl;
    break;

  default:
    s << "Data Type not implemented " << endl;
    s << showbase << hex << dw.GetData() << endl;
    break;
  }
  return s;
}

istream &operator>>(istream &s,DataWordV775 &dw){
  uint32_t d;
  s >> d;
  dw = DataWordV775(d);
  return s; 
}

