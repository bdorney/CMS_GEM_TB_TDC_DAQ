//
//  TimingEvent.h
//  
//
//  Created by Brian L Dorney on 19/09/16.
//
//

#ifndef _TimingEvent_h
#define _TimingEvent_h

//C++ Includes
//#include <algorithm>
#include <map>
#include <stdio>
#include <string>

//Framework Includes

//ROOT Includes

namespace QualityControl {
    namespace Timing {
        struct TDCDataDigi {
            std::map<int, float> m_map_fTime;   //Key -> Chan #; Val -> Time
            
            //Default Constructor
            TDCData(){
                
                //Placeholder
                
            }
            
            //Copy Constructor
            TDCData(const TDCData &other){
                m_map_fTime = other.m_map_fTime;
            }
        }; //End TDCData;
        
        struct EventDigi{
            std::map<std::string, Timing::DataTDC> m_map_TDCData; //Key -> Hex Board Address; Val -> Data
        }; //End EventRAW
    } //End namespace Timing
} //End namespace QualityControl


#endif
