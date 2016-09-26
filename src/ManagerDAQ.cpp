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
using std::dec;
using std::endl;
using std::hex;
using std::map;
using std::pair;
using std::showbase;
using std::string;
using std::vector;

using QualityControl::convert2bool;
using QualityControl::getlineNoSpaces;

using QualityControl::Timing::TDCDataDigi;
using QualityControl::Timing::TDCDataRaw;
using QualityControl::Timing::EventDigi;
using QualityControl::Timing::EventRaw;

vector<EventDigi> QualityControl::Timing::ManagerDAQ::getEventsDIGI(map<string, vector<uint32_t> > map_InputVecTDCData){
	//Variable Declaration
	//double dTimeStamp = -1.;

	int iChan = -1;

	//Translate Input Buffers into a vector of events per each TDC
	DataWordV775 dwTDCData;
	map<string, vector<TDCDataDigi> > map_vecTDCDigiPerEvt;
	TDCDataDigi tdcDigi;
	vector<TDCDataDigi> vec_tdcDigiPerEvt;
	
	//cout<<"QualityControl::Timing::ManagerDAQ::getEventsDIGI() - map_InputVecTDCData.size() = " << map_InputVecTDCData.size() << endl;

	for(auto iterTDCData = map_InputVecTDCData.begin(); iterTDCData != map_InputVecTDCData.end(); ++iterTDCData){ //Loop Over Input Buffers
		for(auto iterDataWord = (*iterTDCData).second.begin(); iterDataWord != (*iterTDCData).second.end(); ++iterDataWord){ //Loop Over Words Stored in (*iterTDCData).second's buffer

			dwTDCData.SetData( (*iterDataWord) );
			if( !( dwTDCData.GetDataType() == (uint32_t)DWV775_DataFiller ) 
				&& (dwTDCData.GetDataType() == (uint32_t)DWV775_EventHeader) ){ //Case: Header

				//Placeholder

			} //End Case: Header
			else if( !( dwTDCData.GetDataType() == (uint32_t)DWV775_DataFiller ) 
				&& (dwTDCData.GetDataType() == (uint32_t)DWV775_EventTrailer) ){ //Case: End of Block
				tdcDigi.m_strBaseAddress = (*iterTDCData).first;
                tdcDigi.m_uiEvtCount = dwTDCData.GetEventCount();
                
				vec_tdcDigiPerEvt.push_back(tdcDigi);

				//cout<<"tdcDigi.m_map_fTime.size() = " << tdcDigi.m_map_fTime.size() << endl;
				tdcDigi.clear();
			} //End Case: End of Block
			else if( !( dwTDCData.GetDataType() == (uint32_t)DWV775_DataFiller ) 
				&& (dwTDCData.GetDataType() == (uint32_t)DWV775_EventDatum) ){ //Case: Datum
				iChan = dwTDCData.GetChannel();

				tdcDigi.m_map_fTime[iChan]=dwTDCData.GetData() * m_map_TDCTimeLSB[(*iterTDCData).first];
			} //End Case: Datum

		} //End Loop Over Words Stored in (*iterTDCData).second's buffer

		map_vecTDCDigiPerEvt[(*iterTDCData).first]=vec_tdcDigiPerEvt;

		//Debugging
		//cout<<"TDC " << showbase << hex << (*iterTDCData).first << dec << " vec_tdcDigiPerEvt.size() = " << vec_tdcDigiPerEvt.size() << endl;

		vec_tdcDigiPerEvt.clear();
	} //End Loop Over Input Buffers

	//cout<<"QualityControl::Timing::ManagerDAQ::getEventsDIGI() - map_vecTDCDigiPerEvt.size() = " << map_vecTDCDigiPerEvt.size() << endl;

	//Get Map of Iterators
	unsigned int uiMaxSize = 0;
	vector<pair<vector<TDCDataDigi>::iterator, vector<TDCDataDigi>::iterator> > vec_pairIterNEnd; //first beginning; second ending
	for(auto iterTDCEvts = map_vecTDCDigiPerEvt.begin(); iterTDCEvts != map_vecTDCDigiPerEvt.end(); ++iterTDCEvts){ //Loop Over Evts in Each TDC
		if( (*iterTDCEvts).second.size() > uiMaxSize ){ 
			uiMaxSize = (*iterTDCEvts).second.size(); 
		}

		vec_pairIterNEnd.push_back(std::make_pair( (*iterTDCEvts).second.begin(), (*iterTDCEvts).second.end() ) );
	} //End Loop Over Evts in Each TDC

	//cout<<"QualityControl::Timing::ManagerDAQ::getEventsDigi() - vec_pairIterNEnd.size() = " << vec_pairIterNEnd.size() << endl;

	//Combine TDC events into a global event
    bool bEvtAligned = true;
	EventDigi evtDigi;
	vector<EventDigi> vec_evtDigi;
	for( unsigned int i=0; i < uiMaxSize; ++i ){ //Loop Over Expected Global Events
		for(int j=0; j<vec_pairIterNEnd.size(); ++j){ //Loop Over TDC's
			if( vec_pairIterNEnd[j].first != vec_pairIterNEnd[j].second ){ //Check: Reached end of events for j^th TDC?
                
                if (j==0) { //Case: First TDC, Evt number not yet set
                    evtDigi.m_uiEvtCount = (*vec_pairIterNEnd[j].first).m_uiEvtCount;
                    evtDigi.m_map_TDCData[(*vec_pairIterNEnd[j].first).m_strBaseAddress]=(*vec_pairIterNEnd[j].first);
                } //End Case: First TDC, Evt number not yet set
                else{ //Case: j^th TDC, Evt number already set
                    //Check to make sure j^th TDC has same evt number as evtDigi
                    if (evtDigi.m_uiEvtCount == (*vec_pairIterNEnd[j].first).m_uiEvtCount ) { //Case: Event numbers match, record!
                        evtDigi.m_map_TDCData[(*vec_pairIterNEnd[j].first).m_strBaseAddress]=(*vec_pairIterNEnd[j].first);
                    } //End Case: Event numbers match, record!
                    else{ //Case: Event numbers do NOT match, do not record, flag!
                        bEvtAligned = false;
                    } //End Case: Event numbers do NOT match, do not record, flag!
                } //End Case: j^th TDC, Evt bumber already set
                
				++vec_pairIterNEnd[j].first; //Advanced j^th TDC to the next digi event
			} //End Check: Reached end of events for j^th TDC
			else{ //Case: No more events for j^th TDC
				if( m_bVerboseMode ){
					cout<<"QualityControl::Timing::ManagerDAQ::getEventsDigi() - Problem TDC Event Streams not Sync'd!\n";
				}

				//auto tempIter = vec_pairIterNEnd[j].first;
				//--tempIter;
				//cout<<"TDC = " << showbase << hex << (*tempIter).m_strBaseAddress << dec << endl;
				//cout<<i<<"\tuiMaxSize = "<<uiMaxSize<<endl;
			} //End Case: No more events for j^th TDC
		} //Loop Over TDCs
		
        //Check to make sure TDC's were all aligned (basd on Event Counter)
        if (bEvtAligned) { //Case: TDC's algined, record
            vec_evtDigi.push_back(evtDigi);
        } //End Case: TDC's aligned, record
        else { //Case: TDC's NOT aligned, do not record!
            bEvtAligned = true; //Reset flag for next Evt!
        } //End Case: TDC's NOT aligned, do not record!
        
		evtDigi.clear();
	} //End Loop Over Expected Global Events

	return vec_evtDigi;
} //End QualityControl::Timing::ManagerDAQ::getEventsDigi()

vector<EventRaw> QualityControl::Timing::ManagerDAQ::getEventsRAW(map<string, vector<uint32_t> > map_InputVecTDCData){
	//Variable Declaration

	//Translate Input Buffers into a vector of events per each TDC
	DataWordV775 dwTDCData;
	map<string, vector<TDCDataRaw> > map_vecTDCRawPerEvt;
	TDCDataRaw tdcRaw;
	vector<TDCDataRaw> vec_tdcRawPerEvt;
	
	//cout<<"QualityControl::Timing::ManagerDAQ::getEventsDIGI() - map_InputVecTDCData.size() = " << map_InputVecTDCData.size() << endl;

	for(auto iterTDCData = map_InputVecTDCData.begin(); iterTDCData != map_InputVecTDCData.end(); ++iterTDCData){ //Loop Over Input Buffers
		for(auto iterDataWord = (*iterTDCData).second.begin(); iterDataWord != (*iterTDCData).second.end(); ++iterDataWord){ //Loop Over Words Stored in (*iterTDCData).second's buffer
			//DataWordV775 dwTDCData( (*iterDataWord) );

			dwTDCData.SetData( (*iterDataWord) );
			if( !( dwTDCData.GetDataType() == (uint32_t)DWV775_DataFiller ) 
				&& (dwTDCData.GetDataType() == (uint32_t)DWV775_EventHeader) ){ //Case: Header

				//Placeholder
				//cout<<"Header = " << showbase << hex << dwTDCData.GetRawData() << dec << endl;

			} //End Case: Header
			else if( !( dwTDCData.GetDataType() == (uint32_t)DWV775_DataFiller ) 
				&& (dwTDCData.GetDataType() == (uint32_t)DWV775_EventTrailer) ){ //Case: End of Block
				tdcRaw.m_strBaseAddress = (*iterTDCData).first;
                		tdcRaw.m_uiEvtCount = dwTDCData.GetEventCount();

				//Debugging
				//cout<<"TDC = " << tdcRaw.m_strBaseAddress << " Evt Count = " << tdcRaw.m_uiEvtCount << endl;
                
				vec_tdcRawPerEvt.push_back(tdcRaw);

				tdcRaw.clear();
				//cout<<"Trailer = " << showbase << hex << dwTDCData.GetRawData() << dec << endl;
			} //End Case: End of Block
			else if( !( dwTDCData.GetDataType() == (uint32_t)DWV775_DataFiller ) 
				&& (dwTDCData.GetDataType() == (uint32_t)DWV775_EventDatum) ){ //Case: Datum
				tdcRaw.m_vec_DataWord.push_back( dwTDCData.GetRawData() );
			} //End Case: Datum
		} //End Loop Over Words Stored in (*iterTDCData).second's buffer

		map_vecTDCRawPerEvt[(*iterTDCData).first]=vec_tdcRawPerEvt;
		vec_tdcRawPerEvt.clear();
	} //End Loop Over Input Buffers

	//cout<<"QualityControl::Timing::ManagerDAQ::getEventsDIGI() - map_vecTDCDigiPerEvt.size() = " << map_vecTDCDigiPerEvt.size() << endl;

	//Get Map of Iterators
	unsigned int uiMaxSize = 0;
	vector<pair<vector<TDCDataRaw>::iterator, vector<TDCDataRaw>::iterator> > vec_pairIterNEnd; //first beginning; second ending
	for(auto iterTDCEvts = map_vecTDCRawPerEvt.begin(); iterTDCEvts != map_vecTDCRawPerEvt.end(); ++iterTDCEvts){ //Loop Over Evts in Each TDC
		if( (*iterTDCEvts).second.size() > uiMaxSize ){ 
			uiMaxSize = (*iterTDCEvts).second.size(); 
		}

		vec_pairIterNEnd.push_back(std::make_pair( (*iterTDCEvts).second.begin(), (*iterTDCEvts).second.end() ) );
	} //End Loop Over Evts in Each TDC

	//cout<<"QualityControl::Timing::ManagerDAQ::getEventsDigi() - vec_pairIterNEnd.size() = " << vec_pairIterNEnd.size() << endl;

	//Combine TDC events into a global event
    bool bEvtAligned = true;
	EventRaw evtRaw;
	vector<EventRaw> vec_evtRaw;
	for( unsigned int i=0; i < uiMaxSize; ++i ){ //Loop Over Expected Global Events
		for(int j=0; j<vec_pairIterNEnd.size(); ++j){ //Loop Over TDC's
			if( vec_pairIterNEnd[j].first != vec_pairIterNEnd[j].second ){ //Check: Reached end of events for j^th TDC?
                if (j==0) { //Case: First TDC, Evt number not yet set
                    evtRaw.m_uiEvtCount = (*vec_pairIterNEnd[j].first).m_uiEvtCount;
                    evtRaw.m_map_TDCData[(*vec_pairIterNEnd[j].first).m_strBaseAddress]=(*vec_pairIterNEnd[j].first);
                } //End Case: First TDC, Evt number not yet set
                else{ //Case: j^th TDC, Evt number already set
                    //Check to make sure j^th TDC has same evt number as evtRaw
                    if ( evtRaw.m_uiEvtCount == (*vec_pairIterNEnd[j].first).m_uiEvtCount ) { //Case: Event numbers match, record!
                        evtRaw.m_map_TDCData[(*vec_pairIterNEnd[j].first).m_strBaseAddress]=(*vec_pairIterNEnd[j].first);
                    } //End Case: Event numbers match, record!
                    else{ //Case: Event numbers do NOT match, do not record, flag!
                        bEvtAligned = false;
                    } //End Case: Event numbers do NOT match, do not record, flag!
                } //End Case: j^th TDC, Evt number already set
                
				++vec_pairIterNEnd[j].first; //Advanced j^th TDC to the next digi event
			} //End Check: Reached end of events for j^th TDC
			else{ //Case: No more events for j^th TDC
				if( m_bVerboseMode ){
					cout<<"QualityControl::Timing::ManagerDAQ::getEventsRaw() - Problem TDC Event Streams not Sync'd!\n";
				}
	
			} //End Case: No more events for j^th TDC
		} //Loop Over TDCs
        
        //Check to make sure TDC's were all aligned (based on Event Counter)
        if (bEvtAligned) { //Case: TDC's aligned, record
            vec_evtRaw.push_back(evtRaw);
        } //End Case: TDC's aligned, record
        else{ //Case: TDC's NOT aligned, do not record, reset flag!
            bEvtAligned = true; //Reset flag for next Evt!
        } //End Case: TDC's NOT aligned, do not record, reset flag!
        
		evtRaw.clear();
	} //End Loop Over Expected Global Events

	return vec_evtRaw;
} //End QualityControl::Timing::ManagerDAQ::getEventsRaw()

void QualityControl::Timing::ManagerDAQ::daqInitialize(){
    crate_VME.setRunSetup(m_rSetup);
    crate_VME.initializeCrate();
    
    return;
} //End QualityControl::Timing::ManagerDAQ::daqInitialize()

void QualityControl::Timing::ManagerDAQ::daqConfigure(){
    crate_VME.configureCrate();
    
    //Determine T_LSB for each TDC
    for(auto iterVMEBoard = crate_VME.m_map_vmeTDC.begin(); iterVMEBoard != crate_VME.m_map_vmeTDC.end(); ++iterVMEBoard){
        m_map_TDCTimeLSB[(*iterVMEBoard).first] = 8.9 / (*iterVMEBoard).second->GetFullScaleRange();
    } //End Loop Over TDC's

    return;
} //End QualityControl::Timing::ManagerDAQ::daqConfigure()

void QualityControl::Timing::ManagerDAQ::daqRelease(){
    crate_VME.releaseCrate();
    
    return;
} //End QualityControl::Timing::ManagerDAQ::daqRelease()

void QualityControl::Timing::ManagerDAQ::daqStartRun(){
    //Variable Declaration
    timespec tspecSleepInterval;
    
    std::vector<uint32_t> vec_DataWord;
    std::vector<unsigned int> vec_uiEvtsInReadout;
    
    //Open Output File
    std::fstream file_Output(m_rSetup.m_strFile_Output_Name.c_str(), std::ios::out);
    
	//Reset the Event Count For each board
	//Set the busy
	crate_VME.m_vmeIO->SetOutput( 1, 0 );
	for (auto iterVMEBoard = crate_VME.m_map_vmeTDC.begin(); iterVMEBoard != crate_VME.m_map_vmeTDC.end(); ++iterVMEBoard) {
		(*iterVMEBoard).second->ClearEventCount();
	}

    //Event Loop
    bool bResetEvtCounter = false;
    int uiNEvt = 0;
    unsigned int uiNEvt_Max = 0, uiNEvt_Min = 0;
    //int uiNEvtTillReset = 0;
    int iTrigCond = 0;
    tspecSleepInterval.tv_sec = 0;
    tspecSleepInterval.tv_nsec = 1;  //Artificial Dead time in ns
    while ( !daqStopRun(uiNEvt, m_rSetup.m_uiEvtNum) ) {
        //Release the busy signal
        crate_VME.m_vmeIO->SetOutput( 1, 1 );
        
        //Check for trigger
        iTrigCond = crate_VME.m_vmeIO->GetInput( 0 );
        if (iTrigCond) { //Case: Trigger!
            //Set the busy
            crate_VME.m_vmeIO->SetOutput( 1, 0 );
            
            //Get Data from Each TDC
            for (auto iterVMEBoard = crate_VME.m_map_vmeTDC.begin(); iterVMEBoard != crate_VME.m_map_vmeTDC.end(); ++iterVMEBoard) { //Loop Over Defined TDC's
                //Sleep if TDC is Not ready
                //while ( !(*iterVMEBoard).second->DReady() || !(*iterVMEBoard).second->IsBusy() ) {
                while ( !(*iterVMEBoard).second->IsBusy() ) {
                    nanosleep(&tspecSleepInterval, (struct timespec *)NULL);
                } //End Loop While TDC Not Ready
                
                //Readout TDC
                vec_uiEvtsInReadout.push_back( (*iterVMEBoard).second->Readout( vec_DataWord ) );
                m_map_vecTDCData[(*iterVMEBoard).first]=vec_DataWord;
                
                //Reset TDC for next trigger
                (*iterVMEBoard).second->DataReset();
                //(*iterVMEBoard).second->ClearEventCount();

                //Reset vec_DataWord for next trigger/tdc
                vec_DataWord.clear();
            } //End Loop Over Defined TDC's
            
            //Build Events Stored in Board Buffers
            vector<EventRaw> vec_GlobalEvtRaw = getEventsRAW(m_map_vecTDCData);
		
		//Debugging
		//cout<<"QualityControl::Timing::ManagerDAQ::daqStartRun() - vec_GlobalEvtRaw.size() = " << vec_GlobalEvtRaw.size() << endl;
		//printEvents(vec_GlobalEvtRaw);
		
		//Check if boards still in sync, if not clear their event contents
		if( crate_VME.m_map_vmeTDC.size() > 1){
			uiNEvt_Min = (*std::min_element(vec_uiEvtsInReadout.begin(),vec_uiEvtsInReadout.end() ) );
			uiNEvt_Max = (*std::max_element(vec_uiEvtsInReadout.begin(),vec_uiEvtsInReadout.end() ) );
			if(uiNEvt_Min != uiNEvt_Max){
				cout<<"QualityControl::Timing::ManagerDAQ::daqStartRun() - Issuing TDC Resync\n";
				for (auto iterVMEBoard = crate_VME.m_map_vmeTDC.begin(); iterVMEBoard != crate_VME.m_map_vmeTDC.end(); ++iterVMEBoard) { //Loop Over Defined TDC's
					(*iterVMEBoard).second->ClearEventCount();
		        	} //End Loop Over Defined TDC's			
			}
		}

            //Update Number of Acquired Events
            uiNEvt += vec_GlobalEvtRaw.size();
            cout<<"============"<<uiNEvt <<" Events Acquired============\n";
            
            //Write Data
            write2DiskRAW(file_Output, vec_GlobalEvtRaw);
            
            //Drop the busy
            crate_VME.m_vmeIO->SetOutput( 1, 1 );
            crate_VME.m_vmeIO->Clear();
            
            //Clear Stored Events
            vec_uiEvtsInReadout.clear();
        } //End Case: Trigger!
        
        //Pause the run?
        daqPauseRunCheck(uiNEvt);
    } //End Event Loop
    
    return;
} //End QualityControl::Timing::ManagerDAQ::daqStartRun()

void QualityControl::Timing::ManagerDAQ::daqPauseRunCheck(unsigned int uiAcquiredEvt){
    
    //if ( uiAcquiredEvt % 1000 == 0 ){ //Case: Check File Flag
    if ( (uiAcquiredEvt % 1000) < 100 ){ //Case: Check File Flag
        //Open the Stop File
        std::ifstream file_Pause;
        
        //Read the input Stop File
        std::string strLine;
        bool bExitSuccess = false, bPause = false;
        
        do {
            file_Pause.open("config/pauseDAQ.cfg" );
            
            getlineNoSpaces(file_Pause,strLine);
            
            //Stop Requested?
            bPause = convert2bool(strLine, bExitSuccess);
            
            //Check if Input was understood, if not do not stop
            if (!bExitSuccess) { bPause = false; }
            
            //Sleep if Pause Requested
            if (bPause) {
	        //Set the busy
	        crate_VME.m_vmeIO->SetOutput( 1, 0 );

		cout<<"QualityControl::Timing::ManagerDAQ::daqPauseRunCheck() - DAQ PAUSED\n";
                std::this_thread::sleep_for (std::chrono::seconds(60));

	        //Drop the busy
        	crate_VME.m_vmeIO->SetOutput( 1, 1 );
	        crate_VME.m_vmeIO->Clear();
            }
            
            file_Pause.close();
        } while (bPause);
    } //End Case: Check File Flag
    
    return;
} //End QualityControl::Timing::ManagerDAQ::daqPauseRunCheck()

bool QualityControl::Timing::ManagerDAQ::daqStopRun(unsigned int uiAcquiredEvt, unsigned int uiRequestedEvt){
    //Variable Declaration
    bool bRetStopCond = false;
    
    if ( uiAcquiredEvt >= uiRequestedEvt){ //Case: Acquired All Events
        bRetStopCond = true ;
    } //End Case: Acquired All Events
    //else if ( uiAcquiredEvt % 1000 == 0 ){ //Case: Check File Flag
    else if ( (uiAcquiredEvt % 1000) < 100 ){ //Case: Check File Flag
        //Open the Stop File
        std::ifstream file_Stop("config/stopDAQ.cfg" );
        
        //Read the input Stop File
        std::string strLine;
        bool bExitSuccess = false;
        getlineNoSpaces(file_Stop,strLine);
        
        //Stop Requested?
        bRetStopCond = convert2bool(strLine, bExitSuccess);
        
        //Check if Input was understood, if not do not stop
        if (!bExitSuccess) {
            bRetStopCond = false;
        }
    } //End Case: Check File Flag
    
    return bRetStopCond;
} //End QualityControl::Timing::ManagerDAQ::daqStopRun()

//Prints one column of data for each buffer source in an N-Column Matrix
template<typename DataType>
void QualityControl::Timing::ManagerDAQ::printDataRAW(std::map<std::string, std::vector<DataType> > map_vecOfData, bool bIsHex){
	//Get Map of Iterators
	unsigned int uiMaxSize = 0;
	vector<pair<typename vector<DataType>::iterator, typename vector<DataType>::iterator> > vec_pairIterNEnd; //first beginning; second ending
	for(auto iterBufferSources = map_vecOfData.begin(); iterBufferSources != map_vecOfData.end(); ++iterBufferSources){ //Loop Over Data Buffers
		if( (*iterBufferSources).second.size() > uiMaxSize ){ 
			uiMaxSize = (*iterBufferSources).second.size(); 
		}

		vec_pairIterNEnd.push_back(std::make_pair( (*iterBufferSources).second.begin(), (*iterBufferSources).second.end() ) );

		cout<<"VME"<<(*iterBufferSources).first<<"\t";	//Make Table Heading
	} //End Loop Over Data Buffers
	cout<<endl;	//Finish Table Heading

	//Print the Data
	for( unsigned int i=0; i < uiMaxSize; ++i ){ //Loop Over Buffer w/Maximum Size
		for(int j=0; j<vec_pairIterNEnd.size(); ++j){ //Loop Over Buffer Sources
			if( vec_pairIterNEnd[j].first != vec_pairIterNEnd[j].second ){ //Check: Reached end of j^th Buffer Source?
				if(bIsHex){ //Case: entry is hex
					cout<< std::showbase << std::hex << (*vec_pairIterNEnd[j].first) << std::dec << "\t";
				} //End Case: entry is hex
				else{ //Case: entry is not hex
					cout<<(*vec_pairIterNEnd[j].first) << "\t";
				} //End Case: entry is not hex

				++vec_pairIterNEnd[j].first; //Advanced j^th Data source to next event
			} //End Check: Reached end of j^th Buffer Source?
			else{ //Case: j^th Buffer Source is Empty
				cout<<"empty\t";
			} //End Case: j^th Buffer Source is Empty
		} //Loop Over Buffer Sources
		cout<<endl;
	} //End Loop Over Buffer w/Maximum Size

	return;
} //End QualityControl::Timing::ManagerDAQ::printData()

//Print built events - DIGI
void QualityControl::Timing::ManagerDAQ::printEvents(std::vector<QualityControl::Timing::EventDigi> vec_InputGlobalEvtDigi){
    
    for(auto iterEvtDigi = vec_InputGlobalEvtDigi.begin(); iterEvtDigi != vec_InputGlobalEvtDigi.end(); ++iterEvtDigi){ //End Loop Over Events
        cout<<"============New Global DIGI Evt============\n";
        cout<<"TDC\tChan\tTime(ns)\n";
        for(auto iterTDCDigi = (*iterEvtDigi).m_map_TDCData.begin(); iterTDCDigi != (*iterEvtDigi).m_map_TDCData.end(); ++iterTDCDigi){ //End Loop Over TDCs
            for(auto iterTDCChan = (*iterTDCDigi).second.m_map_fTime.begin(); iterTDCChan != (*iterTDCDigi).second.m_map_fTime.end(); ++iterTDCChan){ //End Loop Over Digis
                cout<<(*iterTDCDigi).first<<"\t"<<(*iterTDCChan).first<<"\t"<<(*iterTDCChan).second<<endl;
            } //End Loop Over Digis
        } //End Loop Over TDCs
    } //End Loop Over Events
    
    return;
} //End QualityControl::Timing::ManagerDAQ::printEvents() - DIGI

//Print built events - RAW
void QualityControl::Timing::ManagerDAQ::printEvents(std::vector<QualityControl::Timing::EventRaw> vec_InputGlobalEvtRaw){
    
    for(auto iterEvtRaw = vec_InputGlobalEvtRaw.begin(); iterEvtRaw != vec_InputGlobalEvtRaw.end(); ++iterEvtRaw){ //Loop Over Events
        cout<<"============New Global RAW Evt============\n";
        cout<<"TDC\tData Word\n";
        for(auto iterTDCRaw = (*iterEvtRaw).m_map_TDCData.begin(); iterTDCRaw != (*iterEvtRaw).m_map_TDCData.end(); ++iterTDCRaw){ //Loop Over TDCs
            for(auto iterDataWord = (*iterTDCRaw).second.m_vec_DataWord.begin(); iterDataWord != (*iterTDCRaw).second.m_vec_DataWord.end(); ++iterDataWord){ //Loop Over Data Words
                cout<<(*iterTDCRaw).first<<"\t"<<showbase<<hex<<(*iterDataWord)<<dec<<endl;
            } //End Loop Over Data Words
        } //End Loop Over TDCs
    } //End Loop Over Events
    
    return;
} //End QualityControl::Timing::ManagerDAQ::printEvents() - RAW

void QualityControl::Timing::ManagerDAQ::write2DiskRAW(std::fstream &file_InputStream, std::vector<QualityControl::Timing::EventRaw> vec_InputGlobalEvtRaw){
    
    for (auto iterEvtRaw = vec_InputGlobalEvtRaw.begin(); iterEvtRaw != vec_InputGlobalEvtRaw.end(); ++iterEvtRaw) { //Loop over Raw Events
        
        file_InputStream<<"[BEGIN EVENT]\n";
        for (auto iterTDCRaw = (*iterEvtRaw).m_map_TDCData.begin(); iterTDCRaw != (*iterEvtRaw).m_map_TDCData.end(); ++iterTDCRaw) { //Loop over TDC Boards in the Event
            
            for (auto iterDataWord = (*iterTDCRaw).second.m_vec_DataWord.begin(); iterDataWord != (*iterTDCRaw).second.m_vec_DataWord.end(); ++iterDataWord) { //Loop Over (*iterTDCRaw) Data Words
                file_InputStream<< showbase << hex << (*iterTDCRaw).second.m_strBaseAddress << dec << "\t";
                file_InputStream<< showbase << hex << (*iterDataWord) << dec << "\n";
            } //End Loop Over (*iterTDCRaw) Data Words
        } //End Loop over TDC Boards in the Event
        file_InputStream<<"[END EVENT]\n";
    } //End Loop over Raw Events
    
    return;
} //End QualityControl::Timing::ManagerDAQ::write2DiskRAW()
