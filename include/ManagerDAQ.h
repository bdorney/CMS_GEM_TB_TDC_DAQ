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
#include "TimingEvent.h"
#include "TimingRunSetup.h"

//ROOT Includes

namespace QualityControl {
    namespace Timing {
        
        class ManagerDAQ {
            
        public:
            //Constructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            ManagerDAQ(){
                m_bVerboseMode = false;
            } //End Constructor

            //Destructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            ~ManagerDAQ(){
                daqRelease();
            } //End Destructor
            
            //Actions - Methods that Do Something
            //------------------------------------------------------------------------------------------------------------------------------------------            
            virtual void daqConfigure();	//Configure DAQ chain
            virtual void daqInitialize();	//Initialize DAQ chain
            virtual void daqRelease();		//Release DAQ chain
            virtual void daqStartRun();		//start taking Data

            //checks to see if run should be paused
            virtual void daqPauseRunCheck(unsigned int uiAcquiredEvt);
            
            //stop taking data
            virtual bool daqStopRun(unsigned int uiAcquiredEvt, unsigned int uiRequestedEvt);
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Given a map of output Buffers from all defined TDCs builds all events found in the buffers
            //Event output is at the DIGI level (e.g. time stamps)
            virtual std::vector<QualityControl::Timing::EventDigi> getEventsDIGI(std::map<std::string, std::vector<uint32_t> > map_InputVecTDCData);

            //Given a map of output Buffers from all defined TDCs builds all events found in the buffers
            //Event output is at the Raw level (e.g. hex words)
            virtual std::vector<QualityControl::Timing::EventRaw> getEventsRAW(std::map<std::string, std::vector<uint32_t> > map_InputVecTDCData);

            //Return the TDC Resolution (e.g. TimeLSB) for a specific TDC
            double getTDCResolution(std::string strInputBaseAddr){
                if (m_map_TDCTimeLSB.count(strInputBaseAddr) > 0 ) {
                    return m_map_TDCTimeLSB[strInputBaseAddr];
                }
                else{
                    return 0;
                }
            } //End getTDCResolution()
            
            //Return the TDC Resolution (e.g. TimeLSB) for all TDC's
            std::map<std::string, double> getTDCResolutionMap(){
                return m_map_TDCTimeLSB;
            } //End getTDCResolution()
            
            //Printers - Methods that Print Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Print Raw Data
            template<typename DataType>
            void printDataRAW(std::map<std::string, std::vector<DataType> > map_vecOfData, bool bIsHex);
            
            //Print Built Events
            void printEvents(std::vector<QualityControl::Timing::EventDigi> vec_InputGlobalEvtDigi);
            void printEvents(std::vector<QualityControl::Timing::EventRaw> vec_InputGlobalEvtRaw);
            
            //Write Raw Events to Disk
            virtual void write2DiskRAW(std::fstream &file_InputStream, std::vector<QualityControl::Timing::EventRaw> vec_InputGlobalEvtRaw);
            
            //Setters - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            virtual void setRunSetup(Timing::RunSetup inputRunSetup){
                m_rSetup = inputRunSetup;
                return;
            }

            virtual void setVerboseMode(bool bInput){
                m_bVerboseMode = bInput;
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
	    bool m_bVerboseMode;

            QualityControl::Timing::RunSetup m_rSetup;
            
            QualityControl::Timing::HardwareCrateVME crate_VME;

            std::map<std::string, double > m_map_TDCTimeLSB; //Key -> Base Addr; Value -> timeLSB for each TDC            
            std::map<std::string, std::vector<uint32_t> > m_map_vecTDCData; //Key -> Base Addr; Value -> Vector of Data Words
        }; //End Class ManagerDAQ
    } //End namespace Timing
} //End namespace QualityControl


#endif /* defined(____ManagerDAQ__) */
