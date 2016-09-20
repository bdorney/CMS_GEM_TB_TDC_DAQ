//
//  ParameterLoaderDAQ.h
//  
//
//  Created by Brian L Dorney on 03/05/16.
//
//

#ifndef ____ParameterLoaderDAQ__
#define ____ParameterLoaderDAQ__

//C++ Includes
#include <algorithm>
#include <ios>
#include <iostream>
#include <fstream>
#include <map>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>

//Framework Includes
#include "ParameterLoader.h"
#include "QualityControlUtilityFunctions.h"
#include "TimingRunSetup.h"

//ROOT Includes

namespace QualityControl {
    namespace Timing {
        class ParameterLoaderDAQ : public Timing::ParameterLoader {
            
        public:
            //Constructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Default
            ParameterLoaderDAQ();
            
            //Actions - Methods that Do Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Opens a text file set by the user and loads the requested parameters
            virtual void loadParameters(std::ifstream &file_Input, bool bVerboseMode, RunSetup & inputRunSetup);
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Printers - Methods that Print Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Setters - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Data Memebers
            
        private:
            //Actions - Methods that Do Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Sets up inputRunSetup for running in analysis mode
            virtual void loadParametersRun(std::ifstream &file_Input, bool bVerboseMode, RunSetup & inputRunSetup);
            
            //Sets up inputRunSetup for running in comparison mode
            virtual void loadParametersHw(std::ifstream &file_Input, bool bVerboseMode, RunSetup & inputRunSetup);
            
            virtual void loadParametersBoardVME(std::ifstream &file_Input, bool bVerboseMode, RunSetup & inputRunSetup);
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            virtual Timing::VMETypes getVMEType(std::string strInputType);
            
            //Attributes
            //------------------------------------------------------------------------------------------------------------------------------------------
            
        }; //End ParameterLoaderDAQ
    } //End namespace Uniformity
} //End namespace QualityControl

#endif /* defined(____ParameterLoaderDAQ__) */
