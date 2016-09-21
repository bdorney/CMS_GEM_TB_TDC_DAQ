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

map<string, vector<pair<int, float> > > QualityControl::Timing::ManagerDAQ::getConvertedDataRAW2DIGI(map<string, vector<uint32_t> > map_InputVecTDCData){
	//Loop Over Input Buffers
	DataWordV775 dwTDCData;
	vector<pair<int, float> > vecTDCDataConverted;
	map<string, vector<pair<int, float> > > map_vecTDCDataConverted;
	for(auto iterTDCData = map_InputVecTDCData.begin(); iterTDCData != map_InputVecTDCData.end(); ++iterTDCData){ //Loop Over Input Buffers
		for(auto iterWords = (*iterTDCData).second.begin(); iterWords != (*iterTDCData).second.end(); ++iterWords){
			dwTDCData.SetData( (*iterWords) );
		}

		vecTDCDataConverted.clear();
	} //End Loop Over Input Buffers

	return map_vecTDCDataConverted;
} //End QualityControl::Timing::ManagerDAQ::getConvertedDataRAW2DIGI()

void QualityControl::Timing::ManagerDAQ::daqInitialize(){
    crate_VME.setRunSetup(m_rSetup);
    crate_VME.initializeCrate();
    
    return;
} //End QualityControl::Timing::ManagerDAQ::daqInitialize()

void QualityControl::Timing::ManagerDAQ::daqConfigure(){
    crate_VME.configureCrate();
    
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
    int iAcquiredEvt = 0;
    int iTrigCond = 0;
    tspecSleepInterval.tv_sec = 0;
    tspecSleepInterval.tv_nsec = 1;  //Artificial Dead time in ns
    while ( !daqStopRun(iAcquiredEvt, m_rSetup.m_iEvtNum) ) {
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
			for (auto iterVMEBoard = crate_VME.m_map_vmeTDC.begin(); iterVMEBoard != crate_VME.m_map_vmeTDC.end(); ++iterVMEBoard){
				(*iterVMEBoard).second->Arm();
			}
		}
		else{
			//Update Number of Acquired Events
			//iAcquiredEvt=iAcquiredEvt+(*pair_MinMaxEvts.first);

			cout<<"Printing\n";
			printData<uint32_t>(m_map_vecTDCData, true);
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

bool QualityControl::Timing::ManagerDAQ::daqStopRun(int iAcquiredEvt, int iRequestedEvt){
    //Variable Declaration
    bool bRetStopCond = false;
    
    if ( iAcquiredEvt >= iRequestedEvt){ //Case: Acquired All Events
        bRetStopCond = true ;
    } //End Case: Acquired All Events
    else if ( iAcquiredEvt % 1000 == 0 ){ //Case: Check File Flag
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
