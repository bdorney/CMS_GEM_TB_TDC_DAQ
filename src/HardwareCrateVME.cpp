//
//  HardwareCrateVME.cpp
//  
//
//  Created by Brian L Dorney on 20/09/16.
//
//

#include "HardwareCrateVME.h"

//std namespace objects
using std::cout;
using std::endl;
using std::make_shared;

//QualityControl::Timing namespace objects
using QualityControl::Timing::VMETypes;

//Configure All VME Boards Initialized in initializeCrate()
void QualityControl::Timing::HardwareCrateVME::configureCrate(){
    //Configure - Bridge
    //------------------------------------------------------
    m_vmeBridge->Connect();
    m_vmeBridge->Reset();
    
    //Configure - IO
    //------------------------------------------------------
    m_vmeIO->Reset();
    m_vmeIO->SetChannelStatusReg( 0, V513_Input, V513_Negative, V513_Glitched, V513_Transparent );
    m_vmeIO->SetChannelStatusReg( 1, V513_Output, V513_Negative, V513_Glitched, V513_Transparent );
    m_vmeIO->SetChannelStatusReg( 2, V513_Input, V513_Negative, V513_Glitched, V513_Transparent );
    m_vmeIO->Clear();
    m_vmeIO->SetOutput( 1, 0 ); //Set busy
    
    /*for (auto iterVMEBoard = m_map_vmeIO.begin(); iterVMEBoard != m_map_vmeIO.end(); ++iterVMEBoard) {
        (*iterVMEBoard).second->Reset();
        (*iterVMEBoard).second->SetChannelStatusReg( 0, V513_Input, V513_Negative, V513_Glitched, V513_Transparent );
        (*iterVMEBoard).second->SetChannelStatusReg( 1, V513_Output, V513_Negative, V513_Glitched, V513_Transparent );
        (*iterVMEBoard).second->SetChannelStatusReg( 2, V513_Input, V513_Negative, V513_Glitched, V513_Transparent );
        (*iterVMEBoard).second->Clear();
        (*iterVMEBoard).second->SetOutput( 1, 0 ); //Set busy
    }*/
    
    //Configure - TDC
    //------------------------------------------------------
    for (auto iterVMEBoard = m_map_vmeTDC.begin(); iterVMEBoard != m_map_vmeTDC.end(); ++iterVMEBoard) {
        (*iterVMEBoard).second->Initialize();
        
        //Set Common Stop Mode
        (*iterVMEBoard).second->SetBitReg2( V775_BS2_StartStop, IDaqEnable );
        
        //Set Full Scale Range
        (*iterVMEBoard).second->SetFullScaleRange( 0x5A ); //400ns window
        
        (*iterVMEBoard).second->DataReset();
        (*iterVMEBoard).second->ClearEventCount();
    }
    
    return;
} //End QualityControl::Timing::HardwareCrateVME::configureCrate()

//Initialize All VME Boards Defined in m_rSetup
void QualityControl::Timing::HardwareCrateVME::initializeCrate(){
    //Add All Boards
    //------------------------------------------------------
    for (auto iterVMEBoard = m_rSetup.m_map_vmeBoards.begin(); iterVMEBoard != m_rSetup.m_map_vmeBoards.end() ; ++iterVMEBoard) { //Loop Over Input Boards
        switch ( (*iterVMEBoard).second.m_vme_type ) { //switch on board type
            case kVMEBridge:    addVMEBridge( (*iterVMEBoard).second );    break;
            case kVMEIO:        addVMEIO( (*iterVMEBoard).second );        break;
            case kVMETDC:       addVMETDC( (*iterVMEBoard).second );       break;
            default:
                cout<<"QualityControl::Timing::HardwareCrateVME::initializeCrate() - Board Type Not Recognized. Skipping\n";
                break;
        } //End switch on board type
    } //End Loop Over Input Boards
    
    //Set the VME Interface on all Boards - IO
    //------------------------------------------------------
    m_vmeIO->SetVmeInterface( m_vmeBridge );
    
    /*for (auto iterVMEBoard = m_map_vmeIO.begin(); iterVMEBoard != m_map_vmeIO.end(); ++iterVMEBoard) {
        (*iterVMEBoard).second->SetVmeInterface( m_vmeBridge );
    }*/
    
    //Set the VME Interface on all Boards - TDC
    //------------------------------------------------------
    for (auto iterVMEBoard = m_map_vmeTDC.begin(); iterVMEBoard != m_map_vmeTDC.end(); ++iterVMEBoard) {
        (*iterVMEBoard).second->SetVmeInterface( m_vmeBridge );
    }
    
    return;
} //End QualityControl::Timing::initializeCrate

//Add VME Bridge to the Crate, note this will over-write any pre-existing interface
void QualityControl::Timing::HardwareCrateVME::addVMEBridge( Timing::HwVMEBoard &inputBoard ){
    m_vmeBridge = make_shared<IDaqVmeInterface>( IDaqVmeInterface(idmV2718, inputBoard.m_iNumLink, inputBoard.m_iNumBoard, std::stol( inputBoard.m_strBaseAddress ) ) );
    
    return;
} //End QualityControl::Timing::HardwareCrateVME::addVMEBridge()

//Add a VME IO Board to the Crate, note this will over-write any pre-existing interface
void QualityControl::Timing::HardwareCrateVME::addVMEIO( Timing::HwVMEBoard &inputBoard ){
    //Declare Board
    //IDaqV513 vmeBoard;
    
    //Set Base Address
    //vmeBoard.SetBaseAddress( std::stol( inputBoard.m_strBaseAddress ) );
    m_vmeIO->SetBaseAddress( std::stol( inputBoard.m_strBaseAddress ) );
    
    //m_map_vmeIO[inputBoard.m_strBaseAddress] = make_shared<IDaqV513>( vmeBoard );
    
    return;
} //End QualityControl::Timing::HardwareCrateVME::addVMEIO()

//Add a VME TDC Board to the Crate
void QualityControl::Timing::HardwareCrateVME::addVMETDC( Timing::HwVMEBoard &inputBoard ){
    //Declare Board
    IDaqV775 vmeBoard;
    
    //Set Base Address
    vmeBoard.SetBaseAddress( std::stol( inputBoard.m_strBaseAddress ) );
    
    //Set Common Stop Mode
    //vmeBoard.SetBitReg2( V775_BS2_StartStop, IDaqEnable );
    
    //Set Full Scale Range
    //vmeBoard.SetFullScaleRange( std::stoi(inputBoard.m_strFullScaleRange ) );
    
    m_map_vmeTDC[inputBoard.m_strBaseAddress] = std::make_shared<IDaqV775>( vmeBoard );
    
    return;
} //End QualityControl::Timing::HardwareCrateVME::addVMETDC()