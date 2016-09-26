//
//  Reconstructor.cpp
//  
//
//  Created by Brian L Dorney on 26/09/16.
//
//

#include "Reconstructor.h"

//std namespace objects
using std::cout;
using std::endl;
using std::getline;
using std::string;
using std::stringstream;

//QualityControl namespace objects
//using QualityControl::getlineNoSpaces;
using QualityControl::printStreamStatus;

//QualityControl::Timing namespace objects
using QualityControl::Timing::EventRaw;

//Over-written in inherited classes
void QualityControl::Timing::Reconstructor::recoEvents(){
    
    return;
} //End QualityControl::Timing::Reconstructor::recoEvents()

//Returns a raw event
EventRaw QualityControl::Timing::Reconstructor::getEventRAW(std::ifstream &file_Input){
    //Variable Declaration
    EventRaw retEvtRAW;
    
    cout<<"QualityControl::Timing::Reconstructor::getEventRAW() - Start of Event Reached!\n";
    
    //Get the Event from the file_Input
    std::streampos spos_Previous; //previous stream position
    string strLine;
    string strBaseAddr, strDataWord;
    stringstream sstream;
    while ( getline(file_Input, strLine) ) {
        //Do we reach the end of the event?
        if (0 == strLine.compare( m_headers_RECO.m_strSecEnd_Evt ) ) {
            
            cout<<"QualityControl::Timing::Reconstructor::getEventRAW() - End of Event Reached!\n";
            
            file_Input.seekg(spos_Previous);
            break;
        }
        
        sstream.str(strLine);
        sstream>>strBaseAddr>>strDataWord;
        
        retEvtRAW.m_map_TDCData[strBaseAddr].m_vec_DataWord( std::stol( strDataWord, nullptr, 0 ) );
        
        //Store previous stream position so main loop over file exits correctly
        //After finding the end header we will return file_Input to the previous stream position so recoEvents() loop in inherited classes will exit properly
        spos_Previous = file_Input.tellg();
    } //End Loop through input file
    if ( file_Input.bad() && m_bVerboseMode_IO ) {
        perror( "QualityControl::Timing::Reconstructor::getEventRAW(): error while reading raw file" );
        printStreamStatus(file_Input);
    }
    
    return retEvtRAW;
} //End QualityControl::Timing::Reconstructor::getEventRAW()