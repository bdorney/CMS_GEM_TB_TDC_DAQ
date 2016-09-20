//
//  ParameterLoader.h
//  
//
//  Created by Brian L Dorney on 19/09/16.
//
//

#ifndef ____ParameterLoader__
#define ____ParameterLoader__

//C++ Includes
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>

//Framework Includes
#include "QualityControlUtilityFunctions.h"
#include "TimingRunSetup.h"
#include "TimingSectionNames.h"

//ROOT Includes

namespace QualityControl {
    namespace Timing {
        class ParameterLoader {
            
        public:
            //Constructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Default
            ParameterLoader();
            
            //Actions - Methods that Do Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Opens a text file set by the user and loads the requested parameters
            //Over-written by inherited classes
            virtual void loadParameters(std::ifstream &file_Input, bool bVerboseMode, Timing::RunSetup & inputRunSetup);
            
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //As loadRunParameters() above but does not need an RunSetup argument
            virtual Timing::RunSetup getRunParameters(std::ifstream &file_Input, bool bVerboseMode){
                Timing::RunSetup rSetup;
                loadParameters(file_Input, bVerboseMode, rSetup);
                return rSetup;
            };
            
            //Printers - Methods that Print Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Setters - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Takes an input file name & stream; opens the file
            virtual void setFileStream(std::string strInputSetupFile, std::ifstream &file_Input, bool bVerboseMode);

            virtual void setVerboseMode(bool bInput){
                m_bVerboseMode_IO = bInput;
                return;
            }
            
            //Data Memebers
            
        protected:
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Gets the run number from an input run name
            //virtual int getRunNumber(std::string strRunName, std::string strIdent);
            
            //Attributes
            //------------------------------------------------------------------------------------------------------------------------------------------
            bool m_bVerboseMode_IO;
            
            Timing::SecNamesDAQ m_headers_DAQ;
        }; //End class ParameterLoader
    } //End namespace Uniformity
} //End namespace QualityControl

#endif /* defined(____ParameterLoader__) */
