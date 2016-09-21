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

//Framework Includes
#include "HardwareCrateVME.h"
#include "IDaqVmeTypes.h"
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
            //Initialize DAQ chain
            virtual void initialize();
            
            //Configure DAQ chain
            virtual void configure();
            
            //Release DAQ chain
            virtual void release();
            
            //start taking Data
            virtual void startRun();
            
            //Stop taking data
            virtual bool stopRun(int iAcquiredEvt, int iRequestedEvt);
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Printers - Methods that Print Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
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
            
            std::map<uint32_t, std::vector<uint32_t> > m_map_vecTDCData; //Key -> Base Addr; Value -> Vector of Data Words
            
        }; //End Class ManagerDAQ
    } //End namespace Timing
} //End namespace QualityControl


#endif /* defined(____ManagerDAQ__) */
