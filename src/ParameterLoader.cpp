//
//  ParameterLoader.cpp
//  
//
//  Created by Brian L Dorney on 10/09/16.
//
//

#include "ParameterLoader.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::pair;
using std::string;
using std::vector;

using QualityControl::getCharSeparatedList;
using QualityControl::getlineNoSpaces;

using QualityControl::printStreamStatus;
using QualityControl::stoiSafe;

//Default Constructor
QualityControl::Timing::ParameterLoader::ParameterLoader(){
    m_bVerboseMode_IO       = false;
}

//Opens a text file set by the user and loads the requested parameters
//Over-written by inherited classes
void QualityControl::Timing::ParameterLoader::loadParameters(std::ifstream &file_Input, bool bVerboseMode, RunSetup & inputRunSetup){
    
    cout<<"Brian it doesn't work\n";
    cout<<"Press Ctrl+C Now\n";
    
    int dummy;
    std::cin>>dummy;
    
    return;
} //End

void QualityControl::Timing::ParameterLoader::setFileStream(std::string strInputSetupFile, ifstream &file_Input, bool bVerboseMode){
    //Open the Data File
    //------------------------------------------------------
    if (bVerboseMode) { //Case: User Requested Verbose Error Messages - I/O
        printClassMethodMsg("QualityControl::Timing::ParameterLoader","setFileStream", ("trying to open and read: " + strInputSetupFile).c_str() );
    } //End Case: User Requested Verbose Error Messages - I/O
    
    if ( !file_Input.is_open() ) {
        file_Input.open( strInputSetupFile.c_str() );
    }
    else {
	if ( bVerboseMode ) {
       		printClassMethodMsg("QualityControl::Timing::ParameterLoader","setFileStream", ("file: " + strInputSetupFile + " already open, doing nothing").c_str() );
	}

	return;
    }
    
    //Check to See if Data File Opened Successfully
    //------------------------------------------------------
    if (!file_Input.is_open() && bVerboseMode) {
        perror( ("QualityControl::Timing::ParameterLoader::setFileStream(): error while opening file: " + strInputSetupFile).c_str() );
        printStreamStatus(file_Input);
    }
    
    return;
} //End ParameterLoader::getFileStream()

/*int QualityControl::Timing::ParameterLoader::getRunNumber(std::string strRunName, string strIdent){
    //Variable Declaration
    int iRetVal;
    
    std::transform(strRunName.begin(), strRunName.end(), strRunName.begin(), toupper);
    std::transform(strIdent.begin(), strIdent.end(), strIdent.begin(), toupper);
    
    vector<string> vec_strParsedName = getCharSeparatedList(strRunName, '_');
    
    //auto iterStr = std::find_if(vec_strParsedName.begin(), vec_strParsedName.end(), QualityControl::Uniformity::contains("RUN") );
    auto iterStr = std::find_if(vec_strParsedName.begin(), vec_strParsedName.end(), QualityControl::Uniformity::contains(strIdent) );
    
    //Input file name did not contain the phrase "RUN"
    if ( iterStr == vec_strParsedName.end() ) {
        return -1;
    }
    
    (*iterStr).erase( (*iterStr).find(strIdent), 3);
    iRetVal = stoiSafe( (*iterStr) );
    
    return iRetVal;
}*/ //End ParameterLoader::getRunNumber()
