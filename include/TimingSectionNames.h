//
//  TimingSectionNames.h
//  
//
//  Created by Brian L Dorney on 19/09/16.
//
//

#ifndef _TimingSectionNames_h
#define _TimingSectionNames_h

//C++ Includes
#include <stdio.h>
#include <string>

//Framework Includes

//ROOT Includes

namespace QualityControl {
    namespace Timing {
        //Section names - DAQ
        struct SecNamesDAQ{
            //Section Header Beginnings
            std::string m_strSecBegin_BoardVME; //VME Board
            
            std::string m_strSecBegin_InfoCond; //Conditions Info
            std::string m_strSecBegin_InfoHw;   //Hardware Info
            std::string m_strSecBegin_InfoRun;  //Run Info
            
            //Section Header Endings
            std::string m_strSecEnd_BoardVME; //VME Board
            
            std::string m_strSecEnd_InfoCond; //Conditions Info
            std::string m_strSecEnd_InfoHw;   //Hardware Info
            std::string m_strSecEnd_InfoRun;  //Run Info
            
            //Default Constructor
            SecNamesDAQ(){
                //Section Header Beginnings
                m_strSecBegin_BoardVME  = "[BEGIN_VME_BOARD]";
                
                m_strSecBegin_InfoCond  = "[BEGIN_COND_INFO]";
                m_strSecBegin_InfoHw    = "[BEGIN_HW_INFO]";
                m_strSecBegin_InfoRun   = "[BEGIN_RUN_INFO]";
                
                //Section Header Endings
                m_strSecEnd_BoardVME    = "[END_VME_BOARD]";
                
                m_strSecEnd_InfoCond    = "[END_COND_INFO]";
                m_strSecEnd_InfoHw      = "[END_HW_INFO]";
                m_strSecEnd_InfoRun     = "[END_RUN_INFO]";
            } //End Default Constructor

            //Destructor
            /*~SecNamesDAQ(){
                //Section Header Beginnings
                m_strSecBegin_BoardVME.clear();
                
                m_strSecBegin_InfoCond.clear();
                m_strSecBegin_InfoHw.clear();
                m_strSecBegin_InfoRun.clear();
                
                //Section Header Endings
                m_strSecEnd_BoardVME.clear();
                
                m_strSecEnd_InfoCond.clear();
                m_strSecEnd_InfoHw.clear();
                m_strSecEnd_InfoRun.clear();
            }*/ //End Destructor
        }; //End SecNamesDAQ
    } //End namespace Timing
} //End namespace QualityControl



#endif
