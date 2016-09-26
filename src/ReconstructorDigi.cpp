//
//  ReconstructorDigi.cpp
//  
//
//  Created by Brian L Dorney on 26/09/16.
//
//

#include "ReconstructorDigi.h"

//std namespace objects
using std::cout;
using std::endl;
using std::ifstream;
using std::pair;
using std::make_pair;
using std::string;
using std::vector;

//QualityControl::Timing namespace objects
using QualityControl::Timing::EventDigi;
using QualityControl::Timing::EventRaw;
using QualityControl::Timing::kVMETDC;
using QualityControl::getString;

void QualityControl::Timing::ReconstructorDigi::recoEvents(){
    //Initialize Output TTree
    //------------------------------------------------------
    initTree();
    
    //Get Input Data File
    //------------------------------------------------------
    ifstream file_Input;
    setFileStream(m_rSetup.m_strFile_Output_Name, file_Input, m_bVerboseMode_IO);
    
    //Loop Over Input Data File
    //------------------------------------------------------
    //Read the file via std::getline().  Obey good coding practice rules:
    //  -first the I/O operation, then error check, then data processing
    //  -failbit and badbit prevent data processing, eofbit does not
    //See: http://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/
    int uiNEvtProcessed=0;
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
            
            /*for (auto iterTDC = evtDigi.m_map_TDCData.begin(); iterTDC != evtDigi.m_map_TDCData.end(); ++iterTDC) { //Loop Over TDC's
                for (auto iterTDCChan = (*iterTDC).second.m_map_fTime.begin(); iterTDCChan != (*iterTDC).second.m_map_fTime.end(); ++iterTDCChan) { //Loop Over iterTDC's Channels
                    cout<<(*iterTDC).first<<"\t"<<(*iterTDCChan).first<<"\t"<<(*iterTDCChan).second<<endl;
                } //End Loop Over iterTDC's Channels
            }*/ //End Loop Over TDC's
            
            //Fill Evt in TTree
            fillTree(evtDigi);
            
            //Check for Max Evt?
            ++uiNEvtProcessed;

	    if( (uiNEvtProcessed % 1000) == 0){
		cout<<"QualityControl::Timing::ReconstructorDigi::recoEvents() - Processed " << uiNEvtProcessed << " Events\n";
	    }
            /*if (uiNEvtProcessed > m_rSetup.m_uiEvtNum) {
                break;
            }*/
        } //End Case: Run Info Header
    } //End Loop over input file
    
    //Check to see if we had problems while reading the file
    //------------------------------------------------------
    if (file_Input.bad() && m_bVerboseMode_IO) {
        //perror( ("Uniformity::QualityControl::Timing::ParameterLoaderDAQ::loadParameters(): error while reading file: " + strInputSetupFile).c_str() );
        perror( "Timing::QualityControl::Timing::ReconstructorDigi::recoEvents(): error while reading file" );
        printStreamStatus(file_Input);
    }
    
    //Close Input File
    //------------------------------------------------------
    file_Input.close();
    
    //Write TTree
    //------------------------------------------------------
    writeTree();
    
    return;
} //End QualityControl::Timing::ReconstructorDigi::recoEvents

//Fills a TTree
void QualityControl::Timing::ReconstructorDigi::fillTree(QualityControl::Timing::EventDigi & inputEvtDIGI){
    //Transfer the inputEvtDIGI content to m_map_vecOfTDCChan
    //Wish I knew how to get TTree to just take the struct EventDigi as input =/
    for (auto iterTDC = m_map_vecOfTDCChan.begin(); iterTDC != m_map_vecOfTDCChan.end(); ++iterTDC) { //Loop Over TDC's
        
        for (int i=0; i < ((*iterTDC).second).size(); ++i) { //Loop Over TDC Channels;
            
            //Check if TDC Channel was active this event
            //If so update channel content, if not reset to -1
            if ( inputEvtDIGI.m_map_TDCData[(*iterTDC).first].m_map_fTime.count( i ) > 0) { //Case: Channel Active This Event
                ((*iterTDC).second)[i]=inputEvtDIGI.m_map_TDCData[(*iterTDC).first].m_map_fTime[i];
            } //End Case: Channel Active This Event
            else{ //Case: Channel NOT Active This Event
                ((*iterTDC).second)[i]=-1.;
            } //End Case: Channel NOT Active This Event
        } //End Loop Over TDC Channels
    } //End Loop Over TDC's
    
    m_tree_Output->Fill();
    
    return;
} //End QualityControl::Timing::ReconstructorDigi::fillTree()

//Initialize The TTree and it's branches
void QualityControl::Timing::ReconstructorDigi::initTree(){
    //Initialize the channel vector
    //------------------------------------------------------
    //pair<int, double> pair_dChTime = make_pair(-1,-1.);
    //vector<pair<int, double> > vec_TDCChan;
    vector<double> vec_TDCChan;
    vec_TDCChan.resize(32, -1);
    /*for (int i=0; i < 32; ++i) {
        vec_TDCChan[i] = make_pair(i,-1.);
    }*/
    
    //Set up the channel vector for each TDC
    //------------------------------------------------------
    for (auto iterVMEBoard = m_rSetup.m_map_vmeBoards.begin(); iterVMEBoard != m_rSetup.m_map_vmeBoards.end(); ++iterVMEBoard) {
        //Skip board if it's not a TDC
        if ( (*iterVMEBoard).second.m_vme_type != kVMETDC ) {continue;}
        
        //Store the vector of channels
        m_map_vecOfTDCChan[(*iterVMEBoard).second.m_strBaseAddress]=vec_TDCChan;
    } //End Loop Over Defined VME Boards
    
    //Initialize the TTree
    //------------------------------------------------------
    string strTempBaseAddr;
    m_tree_Output->SetName("Timing");
    for (auto iterTDC = m_map_vecOfTDCChan.begin(); iterTDC != m_map_vecOfTDCChan.end(); ++iterTDC ) { //Loop Over TDC's
        //Format Base Address
        strTempBaseAddr = (*iterTDC).first;
        strTempBaseAddr.erase(6,4); //Remove trailing 0's
        strTempBaseAddr.erase(0,2); //Remove "0x"
        
        //Set Channels
        for (int i=0; i < ((*iterTDC).second).size(); ++i) { //Loop Over TDC Channels
            m_tree_Output->Branch( ("TDC" + strTempBaseAddr + "_Ch" + getString(i) ).c_str(), &((*iterTDC).second)[i], ("TDC" + strTempBaseAddr + "_Ch" + getString(i) + "/D" ).c_str() );
        } //End Loop Over TDC Channels
    } //End Loop Over TDC's
    
    return;
} //End QualityControl::Timing::ReconstructorDigi::initTree()

void QualityControl::Timing::ReconstructorDigi::writeTree(){
    //Make Output ROOT File
    string strOutputFilename = m_rSetup.m_strFile_Output_Name;
    
    if (strOutputFilename.find(".raw") != std::string::npos) { //Case: contains ".raw"
        size_t posFileExt = strOutputFilename.find(".raw");
        
        strOutputFilename.erase(posFileExt, std::string::npos);
    } //End Case: contains ".raw"
    /*else{ //Case: does not contain ".raw"
        strOutputFilename+=".root"
    }*/ //Case: does not contain ".raw"
    strOutputFilename+=".root";
    
    //Declare Output ROOT File
    TFile *file_ROOTOutput = new TFile(strOutputFilename.c_str(),m_rSetup.m_strFile_Output_Option.c_str() );
    
    //Write the Output TTree
    file_ROOTOutput->cd();
    (m_tree_Output.get())->Write();
    
    //Close the Output ROOT File
    file_ROOTOutput->Close();
    
    return;
} //End QualityControl::Timing::ReconstructorDigi::writeTree()

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
            tdcDigi.m_map_fTime[dwTDCData.GetChannel()] = dwTDCData.GetData() * m_map_TDCTimeLSB[(*iterTDC).first];
        } //End Loop Over iterTDC's data words
        
        //Add to event
        retEvtDIGI.m_map_TDCData[(*iterTDC).first] = tdcDigi;
        
        //Clear container for next TDC
        tdcDigi.clear();
    } //End Loop Over TDC's in Raw Evt
    
    return retEvtDIGI;
} //End QualityControl::Timing::ReconstructorDigi::getEventDIGI()
