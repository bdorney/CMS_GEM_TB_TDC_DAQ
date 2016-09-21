//
//  ManagerDAQ.cpp
//  
//
//  Created by Brian L Dorney on 20/09/16.
//
//

//C++ Includes

//Framework Includes
//#include "HardwareCrateVME.h"
#include "ManagerDAQ.h"

using std::cout;
using std::endl;

using QualityControl::convert2bool;

void QualityControl::Timing::ManagerDAQ::initialize(){
    crate_VME.setRunSetup(m_rSetup);
    crate_VME.initializeCrate();
    
    return;
} //End QualityControl::Timing::ManagerDAQ::initialize()

void QualityControl::Timing::ManagerDAQ::configure(){
    crate_VME.configureCrate();
    
    return;
} //End QualityControl::Timing::ManagerDAQ::configure()

void QualityControl::Timing::ManagerDAQ::release(){
    crate_VME.releaseCrate();
    
    return;
} //End QualityControl::Timing::ManagerDAQ::release()

void QualityControl::Timing::ManagerDAQ::startRun(){
    //Variable Declaration
    timespec tspecSleepInterval;
    
    std::vector<uint32_t> vec_DataWord;
    
    //Event Loop
    int iAcquiredEvt = 0;
    int iTrigCond = 0;
    tspecSleepInterval.tv_sec = 0;
    tspecSleepInterval.tv_nsec = 5;  //Artificial Dead time in ns
    while ( !stopRun(iAcquiredEvt, m_rSetup.m_iEvtNum) ) {
        //Release the busy signal
        crate_VME.m_vmeIO->SetOutput( 1, 1 );
        
        //Check for trigger
        iTrigCond = crate_VME.m_vmeIO->GetInput( 0 );
        if (iTrigCond) { //Case: Trigger!
            //Set the busy
            crate_VME.m_vmeIO->SetOutput( 1, 0 );
            
            cout<<"============New Trigger============\n";
            
            //Get Data from Each TDC
            for (auto iterVMEBoard = crate_VME.m_map_vmeTDC.begin(); iterVMEBoard != crate_VME.m_map_vmeTDC.end(); ++iterVMEBoard) { //Loop Over Defined TDC's
                //Sleep if TDC is not ready
                while ( !(*iterVMEBoard).second->DReady() || !(*iterVMEBoard).second->IsBusy() ) {
                    //if ( nanosleep( &rt1, &at1 ) != 0 ) cout << "Nanosleep failed" << endl;
                    nanosleep(&tspecSleepInterval, (struct timespec *)NULL);
                } //End Loop While TDC Not Ready
                
                //Readout TDC
                (*iterVMEBoard).second->Readout( vec_DataWord );
                m_map_vecTDCData[(*iterVMEBoard).second->GetBaseAddress()]=vec_DataWord;
                
                //Reset TDC for next trigger
                (*iterVMEBoard).second->DataReset();
                (*iterVMEBoard).second->ClearEventCount();
                
                //Reset vec_DataWord for next trigger/tdc
                vec_DataWord.clear();
            } //End Loop Over Defined TDC's
            
            //Print Data for Each TDC
            for (auto iterTDCData = m_map_vecTDCData.begin(); iterTDCData != m_map_vecTDCData.end(); ++iterTDCData) {
                cout<<(*iterTDCData).first<<"\t"<<(*iterTDCData).second.size()<<endl;
                
                cout<<"------------New TDC------------\n";
                cout<<"Data Words of TDC: "<< (*iterTDCData).first <<endl;
                
                //std::hex;
                
                for (auto iterWord = (*iterTDCData).second.begin(); iterWord != (*iterTDCData).second.end(); ++iterWord) {
                    //cout<<(*iterWord)<<endl;
                    cout<< std::showbase << std::hex << (*iterWord) << std::dec << endl;
                } //End Loop Over Data for This TDC
            } //End Loop Over TDCs
            
            //Drop the busy
            crate_VME.m_vmeIO->SetOutput( 1, 1 );
            crate_VME.m_vmeIO->Clear();
        } //End Case: Trigger!
    } //End Event Loop
    
    return;
} //End QualityControl::Timing::ManagerDAQ::startRun()

bool QualityControl::Timing::ManagerDAQ::stopRun(int iAcquiredEvt, int iRequestedEvt){
    //Variable Declaration
    bool bRetStopCond = false;
    
    if ( iAcquiredEvt >= iRequestedEvt){ //Case: Acquired All Events
        bRetStopCond = true ;
    } //End Case: Acquired All Events
    else{ //Case: Check File Flag
        //I think we'll put a check here to only run the below every X number of events
        //For now we leave it as is
        
        //Open the Stop File
        std::ifstream file_Stop("config/stopDAQ.cfg" );
        
        //Read the input Stop File
        std::string strLine;
        bool bExitSuccess = false;
        getline(file_Stop,strLine);
        
        //Stop Requested?
        bRetStopCond = convert2bool(strLine, bExitSuccess);
        
        //Check if Input was understood, if not do not stop
        if (!bExitSuccess) {
            bRetStopCond = false;
        }
    } //End Case: Check File Flag
    
    return bRetStopCond;
} //End QualityControl::Timing::ManagerDAQ::stopRun()