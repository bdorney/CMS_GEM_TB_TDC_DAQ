//
//  ParameterLoaderDAQ.cpp
//  
//
//  Created by Brian L Dorney on 03/05/16.
//
//

#include "ParameterLoaderDAQ.h"

//std namespace objects
using std::cout;
using std::endl;
using std::ifstream;
using std::pair;
using std::string;

//QualityControl namespace objects
using QualityControl::convert2bool;
using QualityControl::getlineNoSpaces;
using QualityControl::getParsedLine;
using QualityControl::printStreamStatus;
using QualityControl::stoiSafe;

//QualityControl::Timing namespace objects
using QualityControl::Timing::VMETypes;

//Default Constructor
QualityControl::Timing::ParameterLoaderDAQ::ParameterLoaderDAQ(){
    m_bVerboseMode_IO       = false;
} //End Default Constructor

void QualityControl::Timing::ParameterLoaderDAQ::loadParameters(ifstream &file_Input, bool bVerboseMode, Timing::RunSetup & inputRunSetup){
    //Variable Declaration
    string strLine = "";
    
    ////Loop Over data Input File
    //------------------------------------------------------
    //Read the file via std::getline().  Obey good coding practice rules:
    //  -first the I/O operation, then error check, then data processing
    //  -failbit and badbit prevent data processing, eofbit does not
    //See: http://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/
    while ( getlineNoSpaces(file_Input, strLine) ) {
        //Does the user want to comment out this line?
        if ( 0 == strLine.compare(0,1,"#") ) continue;
        
        //Identify Section Headers
        if ( 0 == strLine.compare( m_headers_DAQ.m_strSecEnd_InfoHw ) || 0 == strLine.compare( m_headers_DAQ.m_strSecEnd_InfoRun ) ) { //Case: Reached End of Interest
            
            //Placeholder
            
        } //End Case: Reached End of Interest
        else if ( 0 == strLine.compare( m_headers_DAQ.m_strSecBegin_InfoHw ) ){ //Case: Run Info Header
            loadParametersHw(file_Input, bVerboseMode, inputRunSetup);
            continue;
        } //End Case: Run Info Header
        else if ( 0 == strLine.compare( m_headers_DAQ.m_strSecBegin_InfoRun ) ){ //Case: Hardware Info Header
            loadParametersRun(file_Input, bVerboseMode, inputRunSetup);
            continue;
        } //End Case: Hardware Info Header
        else { //Case: Unsorted Parameters
            
            //Place holder
            
        } //End Case: Unsorted Parameters
    } //End Loop over input file
    
    //Check to see if we had problems while reading the file
    if (file_Input.bad() && bVerboseMode) {
        //perror( ("Uniformity::QualityControl::Timing::ParameterLoaderDAQ::loadParameters(): error while reading file: " + strInputSetupFile).c_str() );
        perror( "Timing::QualityControl::Timing::ParameterLoaderDAQ::loadParameters(): error while reading file" );
        printStreamStatus(file_Input);
    }
    
    //Close Input File
    file_Input.close();
    inputRunSetup.m_bLoadSuccess = true;
    
    return;
} //End QualityControl::Timing::ParameterLoaderDAQ::loadParameters()

//Sets up inputRunSetup for running in analysis mode
void QualityControl::Timing::ParameterLoaderDAQ::loadParametersRun(std::ifstream &file_Input, bool bVerboseMode, Timing::RunSetup & inputRunSetup){
    //Variable Declaration
    bool bExitSuccess = false;
    
    std::pair<string,string> pair_strParam;

    std::streampos spos_Previous; //previous stream position    

    string strLine = "";
    
    if (bVerboseMode) { //Case: User Requested Verbose Error Messages - I/O
        printClassMethodMsg("QualityControl::Timing::ParameterLoaderDAQ","loadParametersRun", "Found Run Heading");
    } //End Case: User Requested Verbose Error Messages - I/O
    
    //Set Defaults
    //------------------------------------------------------
    
    //Placeholder
    
    //Loop through input file
    //Check for faults immediately afterward
    //------------------------------------------------------
    while ( getlineNoSpaces(file_Input, strLine) ) { //Loop through input file
        //Skip commented lines
        if (strLine.compare(0,1,"#") == 0) continue;
        
        //Do we reach the end of the section?
        if ( 0 == strLine.compare( m_headers_DAQ.m_strSecEnd_InfoRun ) ) {
            if (bVerboseMode) { //Case: User Requested Verbose Input/Output
                cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersRun(): End of run info header reached!\n";
            } //End Case: User Requested Verbose Input/Output
            
            file_Input.seekg(spos_Previous);
            break;
        }
        
        //Parse the line
        pair_strParam = getParsedLine(strLine, bExitSuccess);
        
        if (bExitSuccess) { //Case: Parameter Fetched Successfully
            //Transform input field name to all capitals for case-insensitive string comparison
            string strTmp = pair_strParam.first;
            std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), toupper);
            
            pair_strParam.first = strTmp;
            
            if ( pair_strParam.first.compare("EVENTS_TOTAL") == 0 ) {
                inputRunSetup.m_iEvtNum = stoiSafe(pair_strParam.first, pair_strParam.second);
            }
            else if ( pair_strParam.first.compare("OUTPUT_FILE_NAME") == 0 ) {
                inputRunSetup.m_strFile_Output_Name = pair_strParam.second;
            }
            else if ( pair_strParam.first.compare("OUTPUT_FILE_OPTION") == 0 ) {
                inputRunSetup.m_strFile_Output_Option = pair_strParam.second;
            }
            else{ //Case: Parameter Not Recognized
                cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersRun(): input field name:\n";
                cout<<"\t"<<pair_strParam.first<<endl;
                cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersRun(): not recognized! Please cross-check input file!!!\n";
            } //End Case: Parameter Not Recognized
        } //End Case: Parameter Fetched Successfully
        else{ //Case: Input line did NOT parse correctly
            cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersRun(): input line:\n";
            cout<<"\t"<<strLine<<endl;
            cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersRun(): did not parse correctly, please cross-check input file\n";
        } //End Case: Input line did NOT parse correctly

        //Store previous stream position so main loop over file exits
        //After finding the end header we will return file_Input to the previous stream position so loadParameters loop will exit properly 
        spos_Previous = file_Input.tellg();
    } //End Loop through input file
    if ( file_Input.bad() && bVerboseMode) {
        perror( "QualityControl::Timing::ParameterLoaderDAQ::loadParametersRun(): error while reading config file" );
        printStreamStatus(file_Input);
    }
    
	//cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersRun() - strLine = " << strLine << endl;

    return;
} //End QualityControl::Timing::ParameterLoaderDAQ::loadParametersRun()

//Sets up inputRunSetup for running in comparison mode
void QualityControl::Timing::ParameterLoaderDAQ::loadParametersHw(std::ifstream &file_Input, bool bVerboseMode, Timing::RunSetup & inputRunSetup){
    //Variable Declaration
    bool bExitSuccess = false;
    
    std::pair<string,string> pair_strParam;

    std::streampos spos_Previous; //previous stream position

    string strLine = "";
    
    if (bVerboseMode) { //Case: User Requested Verbose Error Messages - I/O
        printClassMethodMsg("QualityControl::Timing::ParameterLoaderDAQ","loadParametersHw", "Found Hardware Heading");
    } //End Case: User Requested Verbose Error Messages - I/O
    
    //Set Defaults
    //------------------------------------------------------
    
    //Placeholder
    
    //Loop through input file
    //Check for faults immediately afterward
    //------------------------------------------------------
    while ( getlineNoSpaces(file_Input, strLine) ) { //Loop through input file
        //Skip commented lines
        if (strLine.compare(0,1,"#") == 0) continue;
        
        //Do we reach the end of the section?
        if ( 0 == strLine.compare( m_headers_DAQ. m_strSecEnd_InfoHw ) ) {
            if (bVerboseMode) { //Case: User Requested Verbose Input/Output
                cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersHw(): End of compare info header reached!\n";
            } //End Case: User Requested Verbose Input/Output
            
            file_Input.seekg(spos_Previous);
            break;
        }
        
        if ( 0 == strLine.compare( m_headers_DAQ.m_strSecBegin_BoardVME ) ){ //Case: New VME Board
            loadParametersBoardVME(file_Input, bVerboseMode, inputRunSetup);
            continue;
        } //End Case: New VME Board
        
        //Store previous stream position so main loop over file exits
        //After finding the end header we will return file_Input to the previous stream position so loadParameters loop will exit properly 
        spos_Previous = file_Input.tellg();
    } //End Loop through input file
    if ( file_Input.bad() && bVerboseMode) {
        perror( "QualityControl::Timing::ParameterLoaderDAQ::loadParametersHw(): error while reading config file" );
        printStreamStatus(file_Input);
    }
    
    return;
} //End QualityControl::Timing::ParameterLoaderDAQ::loadParametersHw()


//Sets up inputRunSetup for running in comparison mode
void QualityControl::Timing::ParameterLoaderDAQ::loadParametersBoardVME(std::ifstream &file_Input, bool bVerboseMode, Timing::RunSetup & inputRunSetup){
    //Variable Declaration
    bool bExitSuccess = false;
    
    std::pair<string,string> pair_strParam;
    
    std::streampos spos_Previous; //previous stream position
    
    string strLine = "";
    
    Timing::HwVMEBoard setupBoardVME;
    
    if (bVerboseMode) { //Case: User Requested Verbose Error Messages - I/O
        printClassMethodMsg("QualityControl::Timing::ParameterLoaderDAQ","loadParametersBoardVME", "Found VME Board Heading");
    } //End Case: User Requested Verbose Error Messages - I/O
    
    //Set Defaults
    //------------------------------------------------------
    
    //Placeholder
    
    //Loop through input file
    //Check for faults immediately afterward
    //------------------------------------------------------
    while ( getlineNoSpaces(file_Input, strLine) ) { //Loop through input file
        //Skip commented lines
        if (strLine.compare(0,1,"#") == 0) continue;
        
        //Do we reach the end of the section?
        if ( 0 == strLine.compare( m_headers_DAQ. m_strSecEnd_BoardVME ) ) {
            if (bVerboseMode) { //Case: User Requested Verbose Input/Output
                cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersBoardVME(): End of compare info header reached!\n";
            } //End Case: User Requested Verbose Input/Output
            
            file_Input.seekg(spos_Previous);
            break;
        }
        
        //Parse the line
        pair_strParam = getParsedLine(strLine, bExitSuccess);
        
        if (bExitSuccess) { //Case: Parameter Fetched Successfully
            //Transform input field name to all capitals for case-insensitive string comparison
            string strTmp = pair_strParam.first;
            std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), toupper);
            
            pair_strParam.first = strTmp;
            
            if ( pair_strParam.first.compare("VME_TYPE") == 0 ) {
                setupBoardVME.m_vme_type = getVMEType( pair_strParam.second );
            }
            else if ( pair_strParam.first.compare("VME_ADDRESS_BASE") == 0 ) {
                setupBoardVME.m_strBaseAddress = pair_strParam.second;
            }
            else{ //Case: Parameter Not Recognized
                cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersBoardVME(): input field name:\n";
                cout<<"\t"<<pair_strParam.first<<endl;
                cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersBoardVME(): not recognized! Please cross-check input file!!!\n";
            } //End Case: Parameter Not Recognized
        } //End Case: Parameter Fetched Successfully
        else{ //Case: Input line did NOT parse correctly
            cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersBoardVME(): input line:\n";
            cout<<"\t"<<strLine<<endl;
            cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersBoardVME(): did not parse correctly, please cross-check input file\n";
        } //End Case: Input line did NOT parse correctly
        
        //Store previous stream position so main loop over file exits
        //After finding the end header we will return file_Input to the previous stream position so loadParameters loop will exit properly
        spos_Previous = file_Input.tellg();
    } //End Loop through input file
    if ( file_Input.bad() && bVerboseMode) {
        perror( "QualityControl::Timing::ParameterLoaderDAQ::loadParametersBoardVME(): error while reading config file" );
        printStreamStatus(file_Input);
    }

    //Store this VME Board if the User Properly Entered a Base Address
    if ( setupBoardVME.m_vme_type != kVMEUnrecognized ) { //Case: VME Base Address non-null
        if ( inputRunSetup.m_map_vmeBoards.count(setupBoardVME.m_strBaseAddress) > 0 ) { //Case: VME Base Address Already Exists!
            cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersBoardVME() - Error: You have defined two boards with the same base address\n";
            cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersBoardVME() - The second will be ignored\n";
            cout<<"QualityControl::Timing::ParameterLoaderDAQ::loadParametersBoardVME() - This may lead to undefined behavior\n";
        } //End Case: VME Base Address Already Exists!
        else{ //Case: New VME Base Address!
            inputRunSetup.m_map_vmeBoards[setupBoardVME.m_strBaseAddress] = setupBoardVME;
        } //End Case: New VME Base Address!
    } //End Case: VME Base Address non-null
    
    return;
} //End QualityControl::Timing::ParameterLoaderDAQ::loadParametersBoardVME()

VMETypes QualityControl::Timing::ParameterLoaderDAQ::getVMEType(std::string strInputType){
    //Variable Declaration
    VMETypes kVMEType;
    
    //Transform to upper case
    std::transform( strInputType.begin(), strInputType.end(), strInputType.begin(), toupper );
    
    if ( strInputType.compare("BRIDGE") == 0 ) {
        kVMEType = kVMEBridge;
    }
    else if ( strInputType.compare("IO") == 0 ) {
        kVMEType = kVMEIO;
    }
    else if ( strInputType.compare("TDC") == 0 ) {
        kVMEType = kVMETDC;
    }
    else{
        cout<<"QualityControl::Timing::ParameterLoaderDAQ::getVMEType(): Did not recognize input board type:\n";
        cout<<"\t"<<strInputType.c_str()<<endl;
        cout<<"QualityControl::Timing::ParameterLoaderDAQ::getVMEType(): Please check input config file\n";
        cout<<"QualityControl::Timing::ParameterLoaderDAQ::getVMEType(): Undefined Behavior may occur\n";
        
        kVMEType = kVMEUnrecognized;
    }
    
    return kVMEType;
} //End QualityControl::Timing::ParameterLoaderDAQ::getVMEType
