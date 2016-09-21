//
//  ManagerDAQ.h
//  
//
//  Created by Brian L Dorney on 20/09/16.
//
//

#ifndef ____ManagerDAQ__
#define ____ManagerDAQ__

//C++ Includes
#include <chrono>
#include <fstream>
#include <map>
#include <stdio.h>
#include <string>
#include <thread>
#include <time.h>
#include <utility>

//Framework Includes
#include "HardwareCrateVME.h"
#include "IDaqVmeTypes.h"
#include "IDaqV775.h"
#include "QualityControlUtilityFunctions.h"
#include "TimingRunSetup.h"

//ROOT Includes

namespace QualityControl {
    namespace Timing {
        
        class ManagerDAQ {
            
        public:
            //Constructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Actions - Methods that Do Something
            //------------------------------------------------------------------------------------------------------------------------------------------            
            virtual void daqConfigure();					//Configure DAQ chain
            virtual void daqInitialize();					//Initialize DAQ chain
            virtual void daqRelease();						//Release DAQ chain
            virtual void daqStartRun();						//start taking Data
            virtual bool daqStopRun(int iAcquiredEvt, int iRequestedEvt);	//stop taking data
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Converts Data Words to time stamps
            virtual std::map<std::string, std::vector<std::pair<int, float> > > getConvertedDataRAW2DIGI(std::map<std::string, std::vector<uint32_t> > map_InputVecTDCData);

            //Printers - Methods that Print Something
            //------------------------------------------------------------------------------------------------------------------------------------------
	    template<typename DataType>
	    void printData(std::map<std::string, std::vector<DataType> > map_vecOfData, bool bIsHex);

            //Setters - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            virtual void setRunSetup(Timing::RunSetup inputRunSetup){
                m_rSetup = inputRunSetup;
                return;
            }
            
        private:
            //Constructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Actions - Methods that Do Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------

            //Printers - Methods that Print Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Setters - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Attributes - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            QualityControl::Timing::RunSetup m_rSetup;
            
            QualityControl::Timing::HardwareCrateVME crate_VME;
            
            std::map<std::string, std::vector<uint32_t> > m_map_vecTDCData; //Key -> Base Addr; Value -> Vector of Data Words

            
        }; //End Class ManagerDAQ
    } //End namespace Timing
} //End namespace QualityControl


#endif /* defined(____ManagerDAQ__) */
