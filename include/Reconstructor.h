//
//  Reconstructor.h
//  
//
//  Created by Brian L Dorney on 26/09/16.
//
//

#ifndef ____Reconstructor__
#define ____Reconstructor__

//C++ Includes
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sstream>
#include <string>

//Framework Includes
#include "ParameterLoader.h"
#include "QualityControlUtilityFunctions.h"
#include "TimingEvent.h"
#include "TimingRunSetup.h"
#include "TimingSectionNames.h"

//ROOT Includes

namespace QualityControl {
    namespace Timing {
        class Reconstructor : public ParameterLoader {
        public:
            //Constructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            Reconstructor(){
                m_bVerboseMode_IO = false;
            } //End Constructor
            
            //Destructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            /*~Reconstructor(){
                
            }*/ //End Destructor
            
            //Actions - Methods that Do Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            virtual void recoEvents();
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Printers - Methods that Print Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //virtual void
            
            //Setters - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Set Run Setup
            virtual void setRunSetup(Timing::RunSetup inputRunSetup){
                m_rSetup = inputRunSetup;
                return;
            }
            
            //Set TDC time LSB (e.g. resolution)
            virtual void setTDCResolution(std::string strInputBaseAddr, double dInputTimeLSB){
                m_map_TDCTimeLSB[strInputBaseAddr] = dInputTimeLSB;
                return;
            }
            virtual void setTDCResolution(std::map<std::string, double > inputTDCTimeLSBMap){
                m_map_TDCTimeLSB = inputTDCTimeLSBMap;
                return;
            }
            
            //Set Verbose Printing Mode
            /*virtual void setVerboseMode(bool bInput){
                m_bVerboseMode_IO = bInput;
                return;
            }*/
            
        protected:
            //Constructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Actions - Methods that Do Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Returns a raw event
            virtual QualityControl::Timing::EventRaw getEventRAW(std::ifstream &file_Input);
            
            //Printers - Methods that Print Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Setters - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Attributes - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //bool m_bVerboseMode_IO;
            
            std::map<std::string, double > m_map_TDCTimeLSB;
            
            QualityControl::Timing::RunSetup m_rSetup;
            
            QualityControl::Timing::SecNamesReco m_headers_RECO;
        }; //End class Reconstructor
    } //End namespace Timing
} //End namespace QualityControl

#endif /* defined(____Reconstructor__) */
