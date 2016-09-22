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

//Default Constructor
QualityControl::Timing::HardwareCrateVME::HardwareCrateVME() : 
	m_vmeBridge( new IDaqVmeInterface(idmV2718, 0, 0, 0x0 ) ),
	m_vmeIO( new IDaqV513){

	//placeholder

    //m_vmeBridge = make_shared<IDaqVmeInterface>( IDaqVmeInterface(idmV2718, 0, 0, 0x0 ) );

    //IDaqV513 vmeIOBoard;
    //m_vmeIO	= make_shared<IDaqV513>( vmeIOBoard );
} //End Default Constructor

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
    
    //Configure - TDC
    //------------------------------------------------------
    for (auto iterVMEBoard = m_map_vmeTDC.begin(); iterVMEBoard != m_map_vmeTDC.end(); ++iterVMEBoard) {
	cout<<"QualityControl::Timing::HardwareCrateVME::configureCrate() - TDC: Initializing Board\n";
        (*iterVMEBoard).second->Initialize();
        
        //Set Common Stop Mode
	cout<<"QualityControl::Timing::HardwareCrateVME::configureCrate() - TDC: Configuring TDC Mode\n";
        (*iterVMEBoard).second->SetBitReg2( V775_BS2_StartStop, IDaqEnable );
        
	//Set All Trig Mode
        //(*iterVMEBoard).second->SetBitReg2( V775_BS2_AllTrg, IDaqEnable );

        //Set Full Scale Range
	    //int FullScaleSet = 0x5A;
	cout<<"QualityControl::Timing::HardwareCrateVME::configureCrate() - TDC: Setting Full Scale Range\n";
        (*iterVMEBoard).second->SetFullScaleRange( std::stol( m_rSetup.m_map_vmeBoards[(*iterVMEBoard).first].m_strFullScaleRange, nullptr, 0 ) ); //400ns window
        

	cout<<"QualityControl::Timing::HardwareCrateVME::configureCrate() - TDC: Performing Data Reset\n";
        (*iterVMEBoard).second->DataReset();

	cout<<"QualityControl::Timing::HardwareCrateVME::configureCrate() - TDC: Clearing Event Content\n";
        (*iterVMEBoard).second->ClearEventCount();
    }

    cout<<"QualityControl::Timing::HardwareCrateVME::configureCrate() - Crate Configured. Ready to use!\n";
    
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
    m_vmeBridge->setBoardNumber( inputBoard.m_iNumBoard );
    m_vmeBridge->setLinkNumber( inputBoard.m_iNumLink );
    //m_vmeBridge->setModuleType( idmV2718 )
    m_vmeBridge->SetBaseAddress( std::stol( inputBoard.m_strBaseAddress, nullptr, 0 ) );

	//cout<<"Bridge Address via stol = " << std::stol( inputBoard.m_strBaseAddress, nullptr, 0 ) << endl;
	//cout<<"Bridge Address Converted = " << std::showbase << std::hex << inputBoard.m_strBaseAddress << std::dec << endl;

    return;
} //End QualityControl::Timing::HardwareCrateVME::addVMEBridge()

//Add a VME IO Board to the Crate, note this will over-write any pre-existing interface
void QualityControl::Timing::HardwareCrateVME::addVMEIO( Timing::HwVMEBoard &inputBoard ){
    //Set Base Address
    m_vmeIO->SetBaseAddress( std::stol( inputBoard.m_strBaseAddress, nullptr, 0 ) );

	//cout<<"IO Address via stol = " << std::stol( inputBoard.m_strBaseAddress, nullptr, 0 ) << endl;
	//cout<<"IO Address Converted = " << std::showbase << std::hex << inputBoard.m_strBaseAddress << std::dec << endl;
    
    //m_map_vmeIO[inputBoard.m_strBaseAddress] = make_shared<IDaqV513>( vmeBoard );
    
    return;
} //End QualityControl::Timing::HardwareCrateVME::addVMEIO()

//Add a VME TDC Board to the Crate
void QualityControl::Timing::HardwareCrateVME::addVMETDC( Timing::HwVMEBoard &inputBoard ){
    //Declare Board
    IDaqV775 vmeBoard;
    
    //Set Base Address
    vmeBoard.SetBaseAddress( std::stol( inputBoard.m_strBaseAddress, nullptr, 0 ) );
    
	//cout<<"TDC Address via stol = " << std::stol( inputBoard.m_strBaseAddress, nullptr, 0 ) << endl;
	//cout<<"TDC Address Converted = " << std::showbase << std::hex << inputBoard.m_strBaseAddress << std::dec << endl;

    //Set Common Stop Mode
    //vmeBoard.SetBitReg2( V775_BS2_StartStop, IDaqEnable );
    
    //Set Full Scale Range
    //vmeBoard.SetFullScaleRange( std::stoi(inputBoard.m_strFullScaleRange, nullptr, 0 ) );
    
    m_map_vmeTDC[inputBoard.m_strBaseAddress] = std::make_shared<IDaqV775>( vmeBoard );
    
    return;
} //End QualityControl::Timing::HardwareCrateVME::addVMETDC()
