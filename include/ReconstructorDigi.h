//
//  ReconstructorDigi.h
//  
//
//  Created by Brian L Dorney on 26/09/16.
//
//

#ifndef ____ReconstructorDigi__
#define ____ReconstructorDigi__

//C++ Includes
#include <stdio.h>
#include <string>

//Framework Includes
#include "IDaqV775.h"
#include "QualityControlUtilityFunctions.h"
#include "Reconstructor.h"
#include "TimingEvent.h"

//ROOT Includes
#include "TBranch.h"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"

//For working with stl containers and TTree
#ifdef __MAKECINT__
#pragma link C++ class vector<double>+;
#pragma link C++ class map<int,double>+;
#endif


namespace QualityControl {
    namespace Timing {
        class ReconstructorDigi : public Reconstructor {
        public:
            //Constructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            ReconstructorDigi(){
                m_bVerboseMode_IO = false;
            } //End Constructor
            
            //Destructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            /*~Reconstructor(){
             
             }*/ //End Destructor
            
            //Actions - Methods that Do Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Reconstruct Events
            virtual void recoEvents();
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Printers - Methods that Print Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //virtual void
            
            //Setters - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
        private:
            //Constructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Actions - Methods that Do Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //virtual void initTree();
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Returns a DIGI event
            virtual QualityControl::Timing::EventDigi getEventDIGI(QualityControl::Timing::EventRaw & inputEvtRAW);
            
            //Printers - Methods that Print Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Setters - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Attributes - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
        }; //End class ReconstructorDigi
    } //End namespace Timing
} //End namespace QualityControl

#endif /* defined(____ReconstructorDigi__) */
