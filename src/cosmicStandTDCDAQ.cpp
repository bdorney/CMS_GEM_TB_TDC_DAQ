//
//  cosmicStandTDCDAQ.cpp
//  
//
//  Created by Brian L Dorney on 20/09/16.
//
//

//C++ Includes
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>

//Framework Includes
#include "ManagerDAQ.h"
#include "ParameterLoaderDAQ.h"
#include "ReconstructorDigi.h"
#include "TimingRunSetup.h"

//ROOT Includes

//std namespace objects
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;

//QualityControl namespace objects
using QualityControl::convert2bool;
using QualityControl::printStreamStatus;

//QualityControl::Timing namespace objects
using QualityControl::Timing::ManagerDAQ;
using QualityControl::Timing::ParameterLoaderDAQ;
using QualityControl::Timing::ReconstructorDigi;

int main( int argc_, char * argv_[] ){
    //Transfer Input Arguments into vec_strInputArgs
    //------------------------------------------------------
    vector<string> vec_strInputArgs;
    vec_strInputArgs.resize(argc_);
    std::copy(argv_, argv_ + argc_, vec_strInputArgs.begin() );

    //Check input Arguments
    //------------------------------------------------------
    bool bVerboseMode = false;
    ifstream file_Config;
    ParameterLoaderDAQ loaderDAQ;
    if (vec_strInputArgs.size() == 1) {
        //printHelpMenu();
        
        //placeholder for help menu
        
        return 1;
    }
    else if (vec_strInputArgs.size() == 2 && vec_strInputArgs[1].compare("-h") == 0) { //Case: Help Menu
        //printHelpMenu();
        
        //placeholder for help menu
        
        return 0;
    } //End Case: Help Menu
    else if (vec_strInputArgs.size() == 3 ) { //Case: DAQ
        bool bExitSuccess = false;
        
        //Set the verbose mode
        bVerboseMode = convert2bool(vec_strInputArgs[2], bExitSuccess);
        if (!bExitSuccess) { //Case: Input Not Understood
            cout<<"main() - vec_strInputArgs[3] expected to be boolean!!!\n";
            cout<<"main() - Parameter given:\n";
            cout<<"\t"<<vec_strInputArgs[2]<<endl;
            cout<<"main(): for help menu call: ./cosmicStandTDCDAQ -h\n";
            cout<<"main(): exitting\n";
            
            return -2;
        } //End Case: Input Not Understood

        //Load the input config file
        loaderDAQ.setFileStream( vec_strInputArgs[1], file_Config, bVerboseMode );
        
        //Check to see if the config file opened successfully
        if (!file_Config.is_open()) {
            perror( ("main() - error while opening file: " + vec_strInputArgs[1]).c_str() );
            printStreamStatus(file_Config);
            cout<<"main(): for help menu call: ./analyzeUniformity -h\n";
            cout<<"main(): exitting\n";
            
            return -1;
        } //End Case: Input Not Understood
    } //End Case: DAQ
    
    //Load DAQ Parameters
    //------------------------------------------------------
    QualityControl::Timing::RunSetup rSetup = loaderDAQ.getRunParameters(file_Config, bVerboseMode);
    
    //Check rSetup
    //cout<<"rSetup.m_bLoadSuccess = " << ( (rSetup.m_bLoadSuccess) ? "true" : "false" ) << endl;
    //cout<<"rSetup.m_uiEvtNum = " << rSetup.m_uiEvtNum << endl;
    //cout<<"rSetup.m_strFile_Output_Name = " << rSetup.m_strFile_Output_Name << endl;
    //cout<<"rSetup.m_strFile_Output_Option = " << rSetup.m_strFile_Output_Option << endl;
    
    /*for (auto iterVMEBoard = rSetup.m_map_vmeBoards.begin(); iterVMEBoard != rSetup.m_map_vmeBoards.end(); ++iterVMEBoard) { //Loop Over Defined VME Boards
        cout<<"New VME Board!\n";
        cout<<"Type = " << (*iterVMEBoard).second.m_vme_type << endl;
        cout<<"Address = " << (*iterVMEBoard).second.m_strBaseAddress.c_str() << endl;
    }*/ //End Loop Over Defined VME Boards
    
    //Start the DAQ Manager
    //------------------------------------------------------
    ManagerDAQ daqManager;
    
    daqManager.setRunSetup(rSetup);
    daqManager.daqInitialize();
    daqManager.daqConfigure();
    
    //Run the DAQ Manager
    //------------------------------------------------------
	cout<<"Starting DAQ LOOP"<<endl;
    daqManager.daqStartRun();
    cout<<"DAQ LOOP ENDED"<<endl;
    
    //Perform the Reconstruction
    //------------------------------------------------------
    ReconstructorDigi recoDigi;
    
    recoDigi.setRunSetup(rSetup);
    recoDigi.setTDCResolution( daqManager.getTDCResolutionMap() );
    recoDigi.recoEvents();
    
    return 0;
} //End main()
