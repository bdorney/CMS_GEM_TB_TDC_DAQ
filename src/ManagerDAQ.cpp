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
using std::map;
using std::pair;
using std::string;
using std::vector;

using QualityControl::convert2bool;

using QualityControl::Timing::TDCDataDigi;
using QualityControl::Timing::EventDigi;

vector<EventDigi> QualityControl::Timing::ManagerDAQ::getConvertedDataRAW2DIGI(map<string, vector<uint32_t> > map_InputVecTDCData){
	//Variable Declaration
	//double dTimeStamp = -1.;

	int iChan = -1;

	//Translate Input Buffers into a vector of events per each TDC
	DataWordV775 dwTDCData;
	map<string, vector<TDCDataDigi> > map_vecTDCDigiPerEvt;
	TDCDataDigi tdcDigi;
	vector<TDCDataDigi> vec_tdcDigiPerEvt;
	
	cout<<"QualityControl::Timing::ManagerDAQ::getConvertedDataRAW2DIGI() - map_InputVecTDCData.size() = " << map_InputVecTDCData.size() << endl;

	for(auto iterTDCData = map_InputVecTDCData.begin(); iterTDCData != map_InputVecTDCData.end(); ++iterTDCData){ //Loop Over Input Buffers
		for(auto iterWords = (*iterTDCData).second.begin(); iterWords != (*iterTDCData).second.end(); ++iterWords){ //Loop Over Words Stored in (*iterTDCData).second's buffer
			//DataWordV775 dwTDCData( (*iterWords) );

			dwTDCData.SetData( (*iterWords) );
			if( !( dwTDCData.GetDataType() == (uint32_t)DWV775_DataFiller ) 
				&& (dwTDCData.GetDataType() == (uint32_t)DWV775_EventHeader) ){ //Case: Header

				//Placeholder

			} //End Case: Header
			else if( !( dwTDCData.GetDataType() == (uint32_t)DWV775_DataFiller ) 
				&& (dwTDCData.GetDataType() == (uint32_t)DWV775_EventTrailer) ){ //Case: End of Block
				tdcDigi.m_strBaseAddress = (*iterTDCData).first;
				vec_tdcDigiPerEvt.push_back(tdcDigi);

				cout<<"tdcDigi.m_map_fTime.size() = " << tdcDigi.m_map_fTime.size() << endl;
				tdcDigi.clear();
			} //End Case: End of Block
			else if( !( dwTDCData.GetDataType() == (uint32_t)DWV775_DataFiller ) 
				&& (dwTDCData.GetDataType() == (uint32_t)DWV775_EventDatum) ){ //Case: Datum
				//dTimeStamp = dwTDCData.GetData() * m_map_TDCTimeLSB[(*iterTDCData).first];
				iChan = dwTDCData.GetChannel();

				tdcDigi.m_map_fTime[iChan]=dwTDCData.GetData() * m_map_TDCTimeLSB[(*iterTDCData).first];

				//vecTDCDataConverted.push_back(std::make_pair<int,double>(iChan,dTimeStamp) );
			} //End Case: Datum

		} //End Loop Over Words Stored in (*iterTDCData).second's buffer

		map_vecTDCDigiPerEvt[(*iterTDCData).first]=vec_tdcDigiPerEvt;
		vec_tdcDigiPerEvt.clear();
	} //End Loop Over Input Buffers

	//cout<<"QualityControl::Timing::ManagerDAQ::getConvertedDataRAW2DIGI() - map_vecTDCDigiPerEvt.size() = " << map_vecTDCDigiPerEvt.size() << endl;

	//Get Map of Iterators
	unsigned int uiMaxSize = 0;
	vector<pair<vector<TDCDataDigi>::iterator, vector<TDCDataDigi>::iterator> > vec_pairIterNEnd; //first beginning; second ending
	for(auto iterTDCEvts = map_vecTDCDigiPerEvt.begin(); iterTDCEvts != map_vecTDCDigiPerEvt.end(); ++iterTDCEvts){ //Loop Over Evts in Each TDC
		if( (*iterTDCEvts).second.size() > uiMaxSize ){ 
			uiMaxSize = (*iterTDCEvts).second.size(); 
		}

		vec_pairIterNEnd.push_back(std::make_pair( (*iterTDCEvts).second.begin(), (*iterTDCEvts).second.end() ) );
	} //End Loop Over Evts in Each TDC

	//cout<<"QualityControl::Timing::ManagerDAQ::getConvertedDataRAW2DIGI() - vec_pairIterNEnd.size() = " << vec_pairIterNEnd.size() << endl;

	//Combine TDC events into a global event
	EventDigi evtDigi;
	vector<EventDigi> vec_evtDigi;
	for( unsigned int i=0; i < uiMaxSize; ++i ){ //Loop Over Expected Global Events
		for(int j=0; j<vec_pairIterNEnd.size(); ++j){ //Loop Over TDC's
			if( vec_pairIterNEnd[j].first != vec_pairIterNEnd[j].second ){ //Check: Reached end of events for j^th TDC?
				evtDigi.m_map_TDCData[(*vec_pairIterNEnd[j].first).m_strBaseAddress]=(*vec_pairIterNEnd[j].first);
				++vec_pairIterNEnd[j].first; //Advanced j^th TDC to the next digi event
			} //End Check: Reached end of events for j^th TDC
			else{ //Case: No more events for j^th TDC
				cout<<"QualityControl::Timing::ManagerDAQ::getConvertedDataRAW2DIGI() - Problem TDC Event Streams not Sync'd!";
			} //End Case: No more events for j^th TDC
		} //Loop Over TDCs
		vec_evtDigi.push_back(evtDigi);
		evtDigi.clear();
	} //End Loop Over Expected Global Events

	return vec_evtDigi;
} //End QualityControl::Timing::ManagerDAQ::getConvertedDataRAW2DIGI()

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
    
    //Event Loop
    //unsigned int uiAcquiredEvt = 0;

	int uiNEvt = 0;

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
            
            cout<<"============New Trigger============\n";
            
            //Get Data from Each TDC
            for (auto iterVMEBoard = crate_VME.m_map_vmeTDC.begin(); iterVMEBoard != crate_VME.m_map_vmeTDC.end(); ++iterVMEBoard) { //Loop Over Defined TDC's
                //Sleep if TDC is not ready
                while ( !(*iterVMEBoard).second->DReady() || !(*iterVMEBoard).second->IsBusy() ) {
		//while ( !(*iterVMEBoard).second->IsBusy() ) {
                    //if ( nanosleep( &rt1, &at1 ) != 0 ) cout << "Nanosleep failed" << endl;
                    nanosleep(&tspecSleepInterval, (struct timespec *)NULL);
                } //End Loop While TDC Not Ready
                
                //Readout TDC
                //(*iterVMEBoard).second->Readout( vec_DataWord );
		vec_uiEvtsInReadout.push_back( (*iterVMEBoard).second->Readout( vec_DataWord ) );
		m_map_vecTDCData[(*iterVMEBoard).first]=vec_DataWord;
                
                //Reset TDC for next trigger
                (*iterVMEBoard).second->DataReset();
                (*iterVMEBoard).second->ClearEventCount();
                
                //Reset vec_DataWord for next trigger/tdc
                vec_DataWord.clear();
            } //End Loop Over Defined TDC's
            
            //Print Data for Each TDC - RAW
            /*for (auto iterTDCData = m_map_vecTDCData.begin(); iterTDCData != m_map_vecTDCData.end(); ++iterTDCData) {
                cout<<"------------New TDC------------\n";
                cout<<"Base Address of TDC: "<< (*iterTDCData).first << endl;
		cout<<"N_Data Words: " << (*iterTDCData).second.size() << endl;

            }*/ //End Loop Over TDCs

		//Check If All TDCs Stored Same Number of Events
		auto pair_MinMaxEvts = std::minmax_element(vec_uiEvtsInReadout.begin(), vec_uiEvtsInReadout.end() );
		if( (*pair_MinMaxEvts.first) != (*pair_MinMaxEvts.second) ){
			cout<<"Boards Do Not Have Same Number of Events - Resetting!\n";
			m_map_vecTDCData.clear();
			/*for (auto iterVMEBoard = crate_VME.m_map_vmeTDC.begin(); iterVMEBoard != crate_VME.m_map_vmeTDC.end(); ++iterVMEBoard){
				(*iterVMEBoard).second->Arm();
			}*/
		}
		else{
			//Update Number of Acquired Events
			//uiAcquiredEvt=uiAcquiredEvt+(*pair_MinMaxEvts.first);

			//cout<<"Printing\n";
			//printData<uint32_t>(m_map_vecTDCData, true);

			vector<EventDigi> vec_GlobalEvt = getConvertedDataRAW2DIGI(m_map_vecTDCData);
			//cout<<"Global Events Made; vec_GlobalEvt.size() = " << vec_GlobalEvt.size() << endl;

			//cout<<"vec_GlobalEvt[0].m_map_TDCData.size() = " << vec_GlobalEvt[0].m_map_TDCData.size()<<endl;

			uiNEvt = uiNEvt + vec_GlobalEvt.size();
			//uiNEvt++;

			for(auto iterEvt = vec_GlobalEvt.begin(); iterEvt != vec_GlobalEvt.end(); ++iterEvt){
				cout<<"============New Global Evt============\n";
				cout<<"TDC\tChan\tTime(ns)\n";
				for(auto iterTDCDigi = (*iterEvt).m_map_TDCData.begin(); iterTDCDigi != (*iterEvt).m_map_TDCData.end(); ++iterTDCDigi){
					for(auto iterTDCChan = (*iterTDCDigi).second.m_map_fTime.begin(); iterTDCChan != (*iterTDCDigi).second.m_map_fTime.end(); ++iterTDCChan){
						cout<<(*iterTDCDigi).first<<"\t"<<(*iterTDCChan).first<<"\t"<<(*iterTDCChan).second<<endl;
					}
				}
			}
		}


            //Drop the busy
            crate_VME.m_vmeIO->SetOutput( 1, 1 );
            crate_VME.m_vmeIO->Clear();

		//Clear Stored Events 
		vec_uiEvtsInReadout.clear();
        } //End Case: Trigger!
    } //End Event Loop
    
    return;
} //End QualityControl::Timing::ManagerDAQ::daqStartRun()

bool QualityControl::Timing::ManagerDAQ::daqStopRun(unsigned int uiAcquiredEvt, unsigned int uiRequestedEvt){
    //Variable Declaration
    bool bRetStopCond = false;
    
    if ( uiAcquiredEvt >= uiRequestedEvt){ //Case: Acquired All Events
        bRetStopCond = true ;
    } //End Case: Acquired All Events
    else if ( uiAcquiredEvt % 1000 == 0 ){ //Case: Check File Flag
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
} //End QualityControl::Timing::ManagerDAQ::daqStopRun()

//Prints one column of data for each buffer source in an N-Column Matrix
template<typename DataType>
void QualityControl::Timing::ManagerDAQ::printData(std::map<std::string, std::vector<DataType> > map_vecOfData, bool bIsHex){
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
