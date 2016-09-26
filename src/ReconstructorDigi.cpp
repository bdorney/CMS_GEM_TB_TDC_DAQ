//
//  ReconstructorDigi.cpp
//  
//
//  Created by Brian L Dorney on 26/09/16.
//
//

#include "ReconstructorDigi.h"

//std namespace objects
using std::string;

//QualityControl::Timing namespace objects
using QualityControl::Timing::EventDigi;
using QualityControl::Timing::EventRaw;

void QualityControl::Timing::ReconstructorDigi::recoEvents(){
    //Get Input Data File
    //------------------------------------------------------
    ifstream file_Input;
    setFileStream(m_rSetup.m_strFile_Output_Name, file_Input);
    
    //Loop Over Input Data File
    //------------------------------------------------------
    //Read the file via std::getline().  Obey good coding practice rules:
    //  -first the I/O operation, then error check, then data processing
    //  -failbit and badbit prevent data processing, eofbit does not
    //See: http://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/
    string strLine = "";
    EventRaw evtRaw;
    EventDigi evtDigi;
    while ( getline(file_Input, strLine) ) {
        //Identify Section Headers
        if ( 0 == strLine.compare( m_headers_RECO.m_strSecEnd_Evt ) ) { //Case: End of Event Interest
            continue;
        } //End Case: Reached End of Interest
        else if ( 0 == strLine.compare( m_headers_RECO.m_strSecBegin_Evt ) ){ //Case: New Event!
            
            evtRaw  = getEventRAW(file_Input);
            evtDigi = getEventDIGI(evtRaw);
            
            for (auto iterTDC = evtDigi.m_map_TDCData.begin(); iterTDC != evtDigi.m_map_TDCData.end(); ++iterTDC) { //Loop Over TDC's
                for (auto iterTDCChan = (*iterTDC).second.m_map_fTime.begin(); iterTDCChan != (*iterTDC).second.m_map_fTime.end(); ++iterTDCChan) { //Loop Over iterTDC's Channels
                    cout<<(*iterTDC).first<<"\t"<<(*iterTDCChan).first<<"\t"<<(*iterTDCChan).second;
                } //End Loop Over iterTDC's Channels
            } //End Loop Over TDC's
            
        } //End Case: Run Info Header
        else { //Case: Unsorted Parameters
            
            //Place holder
            
        } //End Case: Unsorted Parameters
    } //End Loop over input file
    
    //Check to see if we had problems while reading the file
    if (file_Input.bad() && bVerboseMode) {
        //perror( ("Uniformity::QualityControl::Timing::ParameterLoaderDAQ::loadParameters(): error while reading file: " + strInputSetupFile).c_str() );
        perror( "Timing::QualityControl::Timing::ReconstructorDigi::recoEvents(): error while reading file" );
        printStreamStatus(file_Input);
    }
    
    //Close Input File
    file_Input.close();
    
    return;
} //End QualityControl::Timing::ReconstructorDigi::recoEvents

//Returns a DIGI event
EventDigi QualityControl::Timing::ReconstructorDigi::getEventDIGI(EventRaw & inputEvtRAW){
    //Variable Declaration
    EventDigi retEvtDIGI;
    
    //Translate Input Buffers into a vector of events per each TDC
    //int iChan = -1;
    DataWordV775 dwTDCData;
    TDCDataDigi tdcDigi;
    for (auto iterTDC = inputEvtRAW.m_map_TDCData.begin(); iterTDC != inputEvtRAW.m_map_TDCData.end(); ++iterTDC) { //Loop Over TDC's in Raw Evt
        
        tdcDigi.m_strBaseAddress = (*iterTDC).first;
        
        for (auto iterDataWord = (*iterTDC).second.m_vec_DataWord.begin(); iterDataWord != (*iterTDC).second.m_vec_DataWord.end(); ++iterDataWord) { //Loop Over iterTDC's data words
            
            dwTDCData.SetData( (*iterDataWord) );
            tdcDigi.m_map_fTime[dwTDCData.GetChannel()] = dwTDCData.GetData() * m_map_TDCTimeLSB[(*iterTDCData).first];
        } //End Loop Over iterTDC's data words
        
        //Add to event
        retEvtDIGI.m_map_TDCData[(*iterTDC).first] = tdcDigi;
        
        //Clear container for next TDC
        tdcDigi.clear();
    } //End Loop Over TDC's in Raw Evt
    
    return retEvtDIGI;
} //End QualityControl::Timing::ReconstructorDigi::getEventDIGI()