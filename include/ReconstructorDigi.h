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
#include <map>
#include <memory>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>

//Framework Includes
#include "DataWordV775.h"
#include "QualityControlUtilityFunctions.h"
#include "Reconstructor.h"
#include "TimingEvent.h"
#include "TimingRunSetup.h"

//ROOT Includes
#include "TBranch.h"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"

//For working with stl containers and TTree
#ifdef __MAKECINT__
//#pragma link C++ class vector<pair<int,double> >+;
#pragma link C++ class vector<double>+;
//#pragma link C++ class map<int,double>+;
#pragma ling C++ class map<string, vector<double> >+;
#endif

using namespace ROOT;

namespace QualityControl {
    namespace Timing {
        class ReconstructorDigi : public Reconstructor {
        public:
            //Constructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            ReconstructorDigi() : m_tree_Output(new TTree) {
                m_bVerboseMode_IO = false;
            } //End Constructor
            
            //Destructor
            //------------------------------------------------------------------------------------------------------------------------------------------
            ~ReconstructorDigi(){
                m_tree_Output.reset();
            } //End Destructor
            
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
            //Fills a TTree
            virtual void fillTree(QualityControl::Timing::EventDigi & inputEvtDIGI);
            
            //Initializes a TTree and it's branches
            virtual void initTree();
            virtual void writeTree();
            
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
            std::unique_ptr<TTree> m_tree_Output;
            //std::map<std::string, std::vector<std::pair<int, double> > m_map_vecOfTDCChan;
            std::map<std::string, std::vector<double> > m_map_vecOfTDCChan;
        }; //End class ReconstructorDigi
    } //End namespace Timing
} //End namespace QualityControl

#endif /* defined(____ReconstructorDigi__) */
