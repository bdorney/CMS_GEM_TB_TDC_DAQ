#ifndef __DATAWORDV775_H_
#define __DATAWORDV775_H_

//C++ Includes
#include <chrono>
#include <memory>
#include <thread>

//Framework Includes
#include <IDaqVmeTypes.h>
#include <IDaqVmeModule.h>

//ROOT Includes

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
}; //End class DataWordV775

std::ostream &operator<< ( std::ostream &s, DataWordV775 &dw );
std::istream &operator>> ( std::istream &s, DataWordV775 &dw );

#endif
