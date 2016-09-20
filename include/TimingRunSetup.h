//
//  TimingRunInfo.h
//  
//
//  Created by Brian L Dorney on 19/09/16.
//
//

#ifndef _TimingRunSetup_h
#define _TimingRunSetup_h

//C++ Includes
#include <map>
#include <stdio.h>
#include <string>

//Framework Includes

//ROOT Includes

namespace QualityControl {
    namespace Timing {
        enum VMETypes{
            kVMEUnrecognized = -1,
            //kVMEBridge_V2718 = 0,   //Optical Bridge
            //kVMEIO_V513,            //IO
            //kVMETDC_V775,           //TDC
            kVMEBridge = 0,         //Optical Bridge
            kVMEIO,                 //IO
            kVMETDC,                //TDC
            n_vme_boards
        };
        
        struct HwVMEBoard{
            Timing::VMETypes m_vme_type;    //Type of Board
            
            std::string m_strBaseAddress;   //Base Address
        };
        
        struct RunSetup{
            bool m_bLoadSuccess;
            
            int m_iEvtNum;    //Acquire this many events
            
            std::string m_strFile_Output_Name;      //Name of output TFile
            std::string m_strFile_Output_Option;    //Option for TFile: CREATE, RECREATE, UPDATE, etc...
            
            std::map<std::string, Timing::HwVMEBoard> m_map_vmeBoards;
            
            //Default Constructor
            RunSetup(){
                m_bLoadSuccess = false;
            }
        };
    } //End namespace Timing
} //End namespace QualityControl

#endif
