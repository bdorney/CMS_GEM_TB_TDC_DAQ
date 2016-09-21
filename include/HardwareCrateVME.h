//
//  HardwareCrateVME.h
//  
//
//  Created by Brian L Dorney on 20/09/16.
//
//

#ifndef ____HardwareCrateVME__
#define ____HardwareCrateVME__

//C++ Includes
#include <iostream>
#include <map>
#include <memory>
#include <stdio.h>
#include <string>

//Framework Includes
#include "IDaqV513.h"
#include "IDaqV775.h"
#include "IDaqVmeInterface.h"
#include "IDaqVmeTypes.h"
#include "TimingRunSetup.h"

//ROOT Includes

namespace QualityControl {
    namespace Timing {
        class ManagerDAQ;   //Defined in "ManagerDAQ.h"
        
        class HardwareCrateVME {
            friend class QualityControl::Timing::ManagerDAQ;
            
        public:
            //Constructors
            //------------------------------------------------------------------------------------------------------------------------------------------
            HardwareCrateVME();
            
            //Actions - Methods that Do Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Configure All VME Boards Initialized in initializeCrate()
            virtual void configureCrate();

            //Initialize All VME Boards Defined in rSetup
            virtual void initializeCrate();
            
            //Disconnect from Crate
            virtual void releaseCrate(){
                m_vmeBridge->Disconnect();
                
                return;
            };
            
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
            virtual void addVMEBridge( Timing::HwVMEBoard &inputBoard );
            
            virtual void addVMEIO( Timing::HwVMEBoard &inputBoard );
            
            virtual void addVMETDC( Timing::HwVMEBoard &inputBoard );
            
            //Getters - Methods that Get (i.e. Return) Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Printers - Methods that Print Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            
            //Setters - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------

            //Attributes - Methods that Set Something
            //------------------------------------------------------------------------------------------------------------------------------------------
            //Run Setup
            Timing::RunSetup m_rSetup;
            
            //VME Boards
            std::shared_ptr<IDaqVmeInterface> m_vmeBridge;
            std::shared_ptr<IDaqV513> m_vmeIO;
            
            //std::map<std::string, std::shared_ptr<IDaqV513> > m_map_vmeIO;
            std::map<std::string, std::shared_ptr<IDaqV775> > m_map_vmeTDC;
            
        }; //End Class HardwareCrateVME
    } //End namespace Timing
} //End namespace QualityControl

#endif /* defined(____HardwareCrateVME__) */
