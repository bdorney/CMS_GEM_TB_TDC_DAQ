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
#include <stdio.h>
#include <string>

//Framework Includes

//ROOT Includes

namespace QualityControl {
    namespace Timing {
        struct TDCDataDigi {
            std::map<int, double> m_map_fTime;  //Key -> Chan #; Val -> Time
            std::string m_strBaseAddress;   	//Base Address of originating module
            
            //Default Constructor
            TDCDataDigi(){
                
                //Placeholder
                
            }
            
            //Copy Constructor
            TDCDataDigi(const TDCDataDigi &other){
                m_map_fTime = other.m_map_fTime;
		m_strBaseAddress = other.m_strBaseAddress;
            }

            //Default Destructor
            /*~TDCDataDigi(){
		clear();
            }*/

            //Clear Stored Digi
            void clear(){
		m_map_fTime.clear();
		m_strBaseAddress.clear();
            }
        }; //End TDCDataDigi;
        
        struct EventDigi{
            std::map<std::string, Timing::TDCDataDigi> m_map_TDCData; //Key -> Hex Board Address; Val -> Data

            //Default Destructor
            /*~EventDigi(){
		clear();
            };*/

            //Clear Stored Digi
            void clear(){
		m_map_TDCData.clear();
            };
        }; //End EventDigi
    } //End namespace Timing
} //End namespace QualityControl


#endif
