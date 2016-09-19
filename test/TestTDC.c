//CAEN Boards
//#include "IDaqVmeInterface.hh"
#include "IDaqV513.hh"
#include "IDaqV775.hh"
#include "IDaqV792.hh"

//C++ Includes
#include <algorithm>        //transform
#include <cctype>           //toupper
#include <cstddef>          //std::size_t
#include <cstdlib>
#include <iostream>
#include <fstream>			//needed for IO files
#include <sstream>
#include <string>
#include <sys/time.h> // needed for gettimeofday
#include <time.h>     // needed for nanosleep
#include <unistd.h>   // needed for sleep
#include <vector>

//ROOT Includes
#include "TApplication.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TTree.h"

//#define SHOW_HISTO 
// Full Scale in ns
//const unsigned FULL_SCALE = 500;
const unsigned FULL_SCALE = 400;
//const unsigned FULL_SCALE = 1200;

IDaqVmeInterface * vmeInt;
vector<TH1F *> hTDC;
vector<TH1F *> hToF;
TH1F * hTrig;
TH1F * hTrig1;
TH1I * hStripProf;
TH1I * hClusterSize;
TTree * tree_RawData;
//TTree * tree_RunParam;
//int fData[ 32 ];
float fData[ 32 ];
int Run;
double timeBin = 0.0;

#ifdef SHOW_HISTO
	int zero = 0;
	TApplication app( "appKey", &zero, NULL );
	TCanvas * c;
	//TH1F * hCh0;
#endif

//Container for Detector Parameters
struct PARAMETERS_DUT{
    //HV
    //float fHV_Det1_DividerCurrent;       //Micro-Amps
    float fHV_Det1_Drift;                //Volts
    float fHV_Det1_G1Top, fHV_Det1_G1Bot;
    float fHV_Det1_G2Top, fHV_Det1_G2Bot;
    float fHV_Det1_G3Top, fHV_Det1_G3Bot;
    
    float fHV_Det2_DividerCurrent;       //Micro-Amps
    //float fHV_Det2_Drift;                //Volts
    //float fHV_Det2_G1Top, fHV_Det2_G1Bot;
    //float fHV_Det2_G2Top, fHV_Det2_G2Bot;
    //float fHV_Det2_G3Top, fHV_Det2_G3Bot;
    
    //int iHV_Det1_nFilters;  //Drift + X External Filters
    //int iHV_Det2_nFilters;  //Drift + X External Filters
    
    float fHV_Det2_nFilters;  //Drift + X External Filters
    
    //FTM Specific
    /*float fReadout_Amp_Gain_Coarse; //Arbitrary Units
    float fReadout_Amp_Gain_Fine;
    
    float fReadout_Amp_Time_Diff;   //Nanoseconds
    float fReadout_Amp_Time_Int;
    
    bool bReadout_Disc_CFD;
    float fReadout_Disc_Threshold;  //mV
    float fReadout_Disc_WalkAdj;
    float fReadout_Disc_ExtDly;   //nanonseconds
    */
    //FTM Specific
    /*float fDrift_Amp_Gain_Coarse; //Arbitrary Units
    float fDrift_Amp_Gain_Fine;
    
    float fDrift_Amp_Time_Diff;   //Nanoseconds
    float fDrift_Amp_Time_Int;
    
    bool bDrift_Disc_CFD;
    float fDrift_Disc_Threshold;  //mV
    float fDrift_Disc_WalkAdj;
    float fDrift_Disc_ExtDly;   //nanonseconds
    */
    //Initial Values
    PARAMETERS_DUT(){
        //fHV_Det1_DividerCurrent=fHV_Det1_Drift=fHV_Det1_G1Top=fHV_Det1_G1Bot=fHV_Det1_G2Top=fHV_Det1_G2Bot=fHV_Det1_G3Top=fHV_Det1_G3Bot=-1;   //Volts
        //fHV_Det2_DividerCurrent=fHV_Det2_Drift=fHV_Det2_G1Top=fHV_Det2_G1Bot=fHV_Det2_G2Top=fHV_Det2_G2Bot=fHV_Det2_G3Top=fHV_Det2_G3Bot=-1;   //Volts
        
        //iHV_Det1_nFilters=iHV_Det2_nFilters=-1;
        
	fHV_Det1_Drift=fHV_Det1_G1Top=fHV_Det1_G1Bot=fHV_Det1_G2Top=fHV_Det1_G2Bot=fHV_Det1_G3Top=fHV_Det1_G3Bot=-1;   //Volts
        fHV_Det2_DividerCurrent=-1;   //Volts
        
        fHV_Det2_nFilters=-1;
        
        //FTM Specific
        /*fReadout_Amp_Gain_Coarse=fReadout_Amp_Gain_Fine=-1; //Arbitrary units
        
        fReadout_Amp_Time_Diff=fReadout_Amp_Time_Int=-1; //nanoseconds
        
        bReadout_Disc_CFD=false;
        fReadout_Disc_Threshold=fReadout_Disc_WalkAdj=fReadout_Disc_ExtDly=-1;   //nanonseconds
        */
        //FTM Specific
        /*fDrift_Amp_Gain_Coarse=fDrift_Amp_Gain_Fine=-1;
        fDrift_Amp_Time_Diff=fDrift_Amp_Time_Int=-1; //nanoseconds
        
        bDrift_Disc_CFD=false;
        fDrift_Disc_Threshold=fDrift_Disc_WalkAdj=-1; //mV
        fDrift_Disc_ExtDly=-1;   //nanonseconds
	*/
    }

    std::string tostring(){
	return "fHV_Det1_Drift/F:fHV_Det1_G1Top:fHV_Det1_G1Bot:fHV_Det1_G2Top:fHV_Det1_G2Bot:fHV_Det1_G3Top:fHV_Det1_G3Bot:fHV_Det2_DividerCurrent:fHV_Det2_nFilters";
    }
};

//Container for Trigger Parameters
struct PARAMETERS_PMT{
    //bool bPMT1_Disc_CFD;        //true/false
    float fPMT1_HV;             //Volts      (V)
    float fPMT1_Threshold;      //millivolts (mV)
    float fPMT1_WalkAdj;        //millivolts (mV)
    float fPMT1_Disc_ExtDly;    //External Delay used in Constant Fraction Technique
    float fPMT1_Disc_CoinDly;   //Delay used between Discriminator Output and PMT Coincidence Input
    
    //bool bPMT2_Disc_CFD;
    float fPMT2_HV;
    float fPMT2_Threshold;
    float fPMT2_WalkAdj;
    float fPMT2_Disc_ExtDly;    //External Delay used in Constant Fraction Technique
    float fPMT2_Disc_CoinDly;   //Delay used between Discriminator Output and PMT Coincidence Input
    
    //bool bPMT3_Disc_CFD;
    float fPMT3_HV;
    float fPMT3_Threshold;
    float fPMT3_WalkAdj;
    float fPMT3_Disc_ExtDly;    //External Delay used in Constant Fraction Technique
    float fPMT3_Disc_CoinDly;   //Delay used between Discriminator Output and PMT Coincidence Input
    
    //bool bPMTDUT_Disc_CFD;
    float fPMTDUT_HV;
    float fPMTDUT_Threshold;
    float fPMTDUT_WalkAdj;
    float fPMTDUT_Disc_ExtDly;    //External Delay used in Constant Fraction Technique
    float fPMTDUT_Disc_CoinDly;   //Delay used between Discriminator Output and PMT Coincidence Input
    
    //Initial Values
    PARAMETERS_PMT(){
        //bPMT1_Disc_CFD=false;
        fPMT1_HV=fPMT1_Threshold=fPMT1_WalkAdj=fPMT1_Disc_ExtDly=fPMT1_Disc_CoinDly=-1;
        
        //bPMT2_Disc_CFD=false;
        fPMT2_HV=fPMT2_Threshold=fPMT2_WalkAdj=fPMT2_Disc_ExtDly=fPMT2_Disc_CoinDly=-1;
        
        //bPMT3_Disc_CFD=false;
        fPMT3_HV=fPMT3_Threshold=fPMT3_WalkAdj=fPMT3_Disc_ExtDly=fPMT3_Disc_CoinDly=-1;
        
        //bPMTDUT_Disc_CFD=false;
        fPMTDUT_HV=fPMTDUT_Threshold=fPMTDUT_WalkAdj=fPMTDUT_Disc_ExtDly=fPMTDUT_Disc_CoinDly=-1;
    }
    
    std::string tostring(){
	//return "PMT1_Disc_CFD/O:fPMT1_HV/F:fPMT1_Threshold:fPMT1_WalkAdj:fPMT1_Disc_ExtDly:fPMT1_Disc_CoinDly:bPMT2_Disc_CFD/O:fPMT2_HV/F:fPMT2_Threshold:fPMT2_WalkAdj:fPMT2_Disc_ExtDly:fPMT2_Disc_CoinDly:bPMT3_Disc_CFD/O:fPMT3_HV/F:fPMT3_Threshold:fPMT3_WalkAdj:fPMT3_Disc_ExtDly:fPMT3_Disc_CoinDly:bPMTDUT_Disc_CFD/O:fPMTDUT_HV/F:fPMTDUT_Threshold:fPMTDUT_WalkAdj:fPMTDUT_Disc_ExtDly:fPMTDUT_Disc_CoinDly";
	return "fPMT1_HV/F:fPMT1_Threshold:fPMT1_WalkAdj:fPMT1_Disc_ExtDly:fPMT1_Disc_CoinDly:fPMT2_HV:fPMT2_Threshold:fPMT2_WalkAdj:fPMT2_Disc_ExtDly:fPMT2_Disc_CoinDly:fPMT3_HV:fPMT3_Threshold:fPMT3_WalkAdj:fPMT3_Disc_ExtDly:fPMT3_Disc_CoinDly:fPMTDUT_HV:fPMTDUT_Threshold:fPMTDUT_WalkAdj:fPMTDUT_Disc_ExtDly:fPMTDUT_Disc_CoinDly";
    }
};

//Container for Tracker Parameters
struct PARAMETERS_TRACKER{
    float fTracker1_HV;
    float fTracker2_HV;
    float fTracker3_HV;
    
    //Initial Values
    PARAMETERS_TRACKER(){
        fTracker1_HV=fTracker2_HV=fTracker3_HV=-1;
    }
};

int32_t ReadEvents( vector< uint32_t >, TH1F* );
//int ReadQDC( IDaqV792 * q );

//Getters
bool GetParsedBool(std::string strInputParam, std::string strInputVal, std::string strInputFileName);
float GetParsedFloat(std::string strInputParam, std::string strInputVal, std::string strInputFileName);
int GetParsedInt(std::string strInputParam, std::string strInputVal, std::string strInputFileName);
int GetNumberOfTriggers();
int GetRunNumber();
std::string GetRunListForWriting();
std::size_t GetPosEquals(std::string strFilename, std::string & strInput, bool & bInputLineSkip);
std::vector< short > GetVmeInterfaceInfo();

//Setters
void SetParametersDUT( PARAMETERS_DUT &param);
void SetParametersTracker( PARAMETERS_TRACKER &param);
void SetParametersPMT( PARAMETERS_PMT &param);
void SetRunNumber( int RunNum );

vector< int > DisableChannels();

int main( int argc, char **argv ){
#ifdef SHOW_HISTO	
  gROOT->SetStyle("Plain");
  //	gStyle->SetPalette(1);
  c = new TCanvas( "c1", "c1", 800, 600 );
  //  hCh0 = new TH1F( "TDC", "Data", FULL_SCALE + 101, 0, FULL_SCALE + 100 );
  c->Draw();
#endif

    TFile f;
    timespec rt;//, at;
    rt.tv_sec = 0;
    //  rt.tv_nsec = 100000;
    rt.tv_nsec = 10; // timeout in nsec
    int maxEvents = GetNumberOfTriggers();
    // Calculate the FullScale Register Setting
    //int FullScaleSet = (int)36454.4/FULL_SCALE;
    //int FullScaleSet = 0x1E;	// 1200ns window
    int FullScaleSet = 0x5A;	// 400ns window
    //int FullScaleSet = 0xFF;	// 140ns window
    if( FullScaleSet < 0x1E ) FullScaleSet = 0x1E;
    if( FullScaleSet > 0xFF ) FullScaleSet = 0xFF;

    Run = GetRunNumber();
    //stringstream fName;
    
    std::string strRunList = GetRunListForWriting();
    std::ofstream outputRunList(strRunList.c_str(), std::ios::app);
    
    std::string fName;
    //fName << "./data/Run_" << Run << ".root";
    std::cout << "Please enter file name: ";
    std::cin >> fName;
    outputRunList<< ( fName + ".root\n" ).c_str();
    
    //Load the Run Parameters List
    PARAMETERS_DUT param_dut;       SetParametersDUT(param_dut);
    PARAMETERS_PMT param_pmt;       SetParametersPMT(param_pmt);
    //PARAMETERS_TRACKER param_trk;   SetParametersTracker(param_trk);
    
    //fName = "./data/" + fName + ".root";
    //fName = "./data/2015/GE11/" + fName + ".root";
    fName = "./data/2016/Cosmic_Stand/" + fName + ".root";
    f.Open( fName.c_str(), "RECREATE" );
    //tree_RawData = new TTree( "CMS_RPC_TREE", "CMS RPC TEST RAW DATA" );
    tree_RawData = new TTree( "CMS_GEM_RAW_DATA", "CMS GEM TEST RAW DATA" );
    TTree * tree_RunParam = new TTree( "CMS_GEM_RUN_PARAM","Run Parameters");
    
    // Create TDC and trig - channel (ToF) histograms for 8 channels
    for( unsigned int i = 0; i < 33; ++i ){
        stringstream hName;
        stringstream hDescr;
        
        hName << "TDC_Ch" <<  i;
        hDescr << "TDC Channel " << i <<";Time [ns];Count";
        //TH1F * h1 = new TH1F( hName.str().c_str(), hDescr.str().c_str(), FULL_SCALE, 0, FULL_SCALE ); 
        //TH1F * h1 = new TH1F( hName.str().c_str(), hDescr.str().c_str(), 4000, 0, FULL_SCALE ); 
	TH1F * h1 = new TH1F( hName.str().c_str(), hDescr.str().c_str(), 4096, 0, FULL_SCALE ); 
        hTDC.push_back( h1 );
        if( i < 32 ) tree_RawData->Branch( hName.str().c_str(), &fData[ i ] );
        hName.str("");

    }

    //Create the Parameter Branches
    //tree_RunParam->Branch("ParamDUT",&param_dut,"fHV_Det1_DividerCurrent/F:fHV_Det1_Drift/F:fHV_Det1_G1Top/F:fHV_Det1_G1Bot/F:fHV_Det1_G2Top/F:fHV_Det1_G2Bot/F:fHV_Det1_G3Top/F:fHV_Det1_G3Bot/F:iHV_Det1_nFilters/F:fHV_Det2_DividerCurrent/F:fHV_Det2_Drift/F:fHV_Det2_G1Top/F:fHV_Det2_G1Bot/F:fHV_Det2_G2Top/F:fHV_Det2_G2Bot/F:fHV_Det2_G3Top/F:fHV_Det2_G3Bot/F:iHV_Det2_nFilters/F:fReadout_Amp_Gain_Coarse/F:fReadout_Amp_Gain_Fine/F:fReadout_Amp_Time_Diff/F:fReadout_Amp_Time_Int/F:bReadout_Disc_CFD/O:fReadout_Disc_Threshold/F:fReadout_Disc_WalkAdj/F:fReadout_Disc_ExtDly/F:fDrift_Amp_Gain_Coarse/F:fDrift_Amp_Gain_Fine/F:fDrift_Amp_Time_Diff/F:fDrift_Amp_Time_Int/F:bDrift_Disc_CFD/O:fDrift_Disc_Threshold/F:fDrift_Disc_WalkAdj/F:fDrift_Disc_ExtDly/F");
    //tree_RunParam->Branch("ParamDUT",&param_dut,"fHV_Det1_Drift/F:fHV_Det1_G1Top:fHV_Det1_G1Bot:fHV_Det1_G2Top:fHV_Det1_G2Bot:fHV_Det1_G3Top:fHV_Det1_G3Bot:fHV_Det2_DividerCurrent:fHV_Det2_nFilters");

    //tree_RunParam->Branch("ParamPMT",&param_pmt,"bPMT1_Disc_CFD/O:fPMT1_HV/F:fPMT1_Threshold/F:fPMT1_WalkAdj/F:fPMT1_Disc_ExtDly/F:fPMT1_Disc_CoinDly/F:bPMT2_Disc_CFD/O:fPMT2_HV/F:fPMT2_Threshold/F:fPMT2_WalkAdj/F:fPMT2_Disc_ExtDly/F:fPMT2_Disc_CoinDly/F:bPMT3_Disc_CFD/O:fPMT3_HV/F:fPMT3_Threshold/F:fPMT3_WalkAdj/F:fPMT3_Disc_ExtDly/F:fPMT3_Disc_CoinDly/F:bPMTDUT_Disc_CFD/O:fPMTDUT_HV/F:fPMTDUT_Threshold/F:fPMTDUT_WalkAdj/F:fPMTDUT_Disc_ExtDly/F:fPMTDUT_Disc_CoinDly/F");
    //tree_RunParam->Branch("ParamTRK",&param_trk,"fTracker1_HV/F:fTracker2_HV/F:fTracker3_HV/F");

    tree_RunParam->Branch("ParamDUT",&param_dut, (param_dut.tostring() ).c_str() );
    tree_RunParam->Branch("ParamPMT",&param_pmt, (param_pmt.tostring() ).c_str() );

    //tree_RunParam->Branch("ParamDUT",&param_dut,8000,1);
    //tree_RunParam->Branch("ParamPMT",&param_pmt,8000,1);
    //tree_RunParam->Branch("ParamTRK",&param_trk,8000,1);

    //Fill the run parameters
    tree_RunParam->Fill();
    
    //  hTrig = new TH1F( "hTrig", "hTrig", FULL_SCALE + 100, 0, FULL_SCALE + 100 );
    // hTrig1 = new TH1F( "hCh18", "hCh18", FULL_SCALE + 100, 0, FULL_SCALE + 100 );
    hStripProf = new TH1I( "hStripProf", "Strips Profile;Channels;Count", 32, 0, 32 );
    hClusterSize = new TH1I( "hClusSize", "Cluster Size;TDC Hits per Event;Count", 32, 0, 32 );

    //VME Intercace Constructor
    //IDaqVmeInterface(IDaqVmeModuleType aModule = idmV2718, short aLink = 0, short aBoardNum = 0, long aBaseAddress = 0 );
    //vmeInt = new IDaqVmeInterface();
    vector<short> vec_vmeBridgeInfo = GetVmeInterfaceInfo();
    vmeInt = new IDaqVmeInterface(idmV2718, vec_vmeBridgeInfo[0], vec_vmeBridgeInfo[1], 0 );
    vmeInt->Connect();
    vmeInt->Reset();
    sleep( 1 );
    /////vmeInt->SetInputConf( 0, 1 ); ///DON'T SET! if set the input 0 will always be 1
    //vmeInt->SetOutputSource( 4, cvManualSW );
    //vmeInt->SetOutputSource( 4, cvInputSrc0 );
    //vmeInt->SetOutputLevel( 4, IDaqEnable );
    //vmeInt->SetOutputLevel( 4, IDaqDisable );
    /*	
    IDaqV792 * qdc = new IDaqV792();
    qdc->SetBaseAddress( 0xF00B0000 );
    qdc->SetVmeInterface( vmeInt );
    qdc->SoftwareReset();
    */
    //Configuring the IO
    IDaqV513 * trigRecv = new IDaqV513();
    trigRecv->SetBaseAddress( 0x00C00B00 );
    trigRecv->SetVmeInterface( vmeInt );
    trigRecv->Reset();
    //Set the trigger reciever channel (0) and veto channel (1)
    trigRecv->SetChannelStatusReg( 0, V513_Input, V513_Negative, V513_Glitched, V513_Transparent );
    trigRecv->SetChannelStatusReg( 1, V513_Output, V513_Negative, V513_Glitched, V513_Transparent );
    trigRecv->SetChannelStatusReg( 2, V513_Input, V513_Negative, V513_Glitched, V513_Transparent );
    //vmeInt->ResetStatus();
    trigRecv->Clear();
    trigRecv->SetOutput( 1, 0 ); //Set busy

    IDaqV775 * tdc = new IDaqV775();
    //Set the base address of the module
    tdc->SetBaseAddress( 0x400C0000 );
    //Set the vme interface
    tdc->SetVmeInterface( vmeInt );
    //Set the Geo address
    tdc->SetGeoAddress( 0x5 );
    //cout << "Geo = " << showbase << hex << q->GetGeoAddress() << dec << endl; 
    //Initialize...
    //tdc->SoftwareReset();
    tdc->Initialize();
    //Set the trigger to common stop mode
    tdc->SetBitReg2( V775_BS2_StartStop, IDaqEnable );//Common Stop
    //  tdc->SetBitReg2( V775_BS2_StartStop, IDaqDisable ); //Common Start
    cout << "TDC Bit Set Reg 2 = 0x" << hex << tdc->GetBitReg2() << dec << endl;
    //tdc->SetFullScaleRange( 0xFF );
    // tdc->SetFullScaleRange( 0x1E ); //Full range
    tdc->SetFullScaleRange( FullScaleSet );
    //Disable channels
    vector< int > DisCh = DisableChannels();
    for( unsigned j = 0; j < DisCh.size(); ++j ){
        cout<<j<<"\tDisCh["<<j<<"] = " << DisCh[j] << "\tIDaqDisable = " << IDaqDisable << endl;
        tdc->SetChannelStatus( DisCh[ j ], IDaqDisable );
    }

    //tdc->SetFullScaleRange( 0x49 ); // ~500 ns full range
    cout << "Full Scale Range = " << 8.9 / tdc->GetFullScaleRange() * 4096 << " ns ( setting is 0x" << hex << tdc->GetFullScaleRange( ) << dec << " )" <<  endl;
    timeBin = 8.9/tdc->GetFullScaleRange();
    //vmeInt->SetScalerConf();
        
    int globEventCount = 0;
    int globEventCountFlush = 0;

    vector< uint32_t > Data;
    //vmeInt->ResetScaler();
    //vmeInt->EnableScaler();
    //sleep( 1 );
    int iter = 0;
    tdc->DataReset();
    tdc->ClearEventCount();

    while( globEventCount < maxEvents ){
        trigRecv->SetOutput( 1, 1 ); //Release  the busy
        int trigger = trigRecv->GetInput( 0 );
        iter++;
        unsigned int evCount = 0;

        //cout<<"trigger = " << trigger << endl;

        if( trigger ){
            //cout << "trig 0 =  " << trigRecv->GetInput( 2 ) << endl;
            trigRecv->SetOutput( 1, 0 ); //Set the busy
            timespec rt1, at1;
            rt1.tv_sec = 0;
            rt1.tv_nsec = 5; // timeout in nsec

            //cout << "Event Triggered!!!" << endl;

            while( ( !tdc->DReady() ) || (tdc->IsBusy()) ){
                if ( nanosleep( &rt1, &at1 ) != 0 ) cout << "Nanosleep failed" << endl;
            } //HACK1
	    
            //cout << "TDC Ready!!!" << endl;

            evCount = tdc->Readout( Data );
            //cout << "Iterations  = " << iter << endl;
            //globEventCount += ReadEvents( Data, hTDC[ 0 ] );

	    //Change from Filip
	    int _read_events = ReadEvents( Data, hTDC[ 0 ] );
            globEventCount += _read_events;
	    globEventCountFlush += _read_events;

            cout << dec << globEventCount << " events read. Data.size() = " << Data.size() << endl;
            tdc->DataReset();
            tdc->ClearEventCount();
            Data.clear();
            iter = 0;
            trigRecv->SetOutput( 1, 1 );//Drop busy
            trigRecv->Clear();
        }
        else{
            //cout<<"Waiting for Triggers"<<endl;
        }

	//Change from Filip
	//Here this makes the data dump every so often
	if (globEventCountFlush > 990) {
	  globEventCountFlush -= 990;
	  tree_RawData->FlushBaskets();
	}
    }
  
    //Write the histograms to the root file
    cout << "Going to write root file..." << endl;
    for( unsigned int i = 0; i < hTDC.size(); ++i ){
        cout << "Writing hTDC[ " << i << " ]" << endl;
        hTDC[ i ]->Write();
    }

    hClusterSize->Write();
    //delete hClusterSize;
    int TriggerCount = hStripProf->GetBinContent( 1 );//Get the trigger count from the first bin in histogram. 0 bin is underflow;
    double Eff = 0.;
    for( unsigned int i = 2; i < 33; ++i ){
        Eff += hStripProf->GetBinContent( i );
    }
    
    Eff /= TriggerCount;
    hStripProf->Write();
  
    //delete hStripProf;

    //hTrig->Write();
    //  delete hTrig;
    //hTrig1->Write();
    //delete hTrig1;
    cout << "Done." << endl;
    tree_RawData->Write(); //delete tree_RawData;
    tree_RunParam->Write(); //delete tree_RunParam;
    f.Close();
    cout << "Vme dsiconnect.." << endl;
    vmeInt->Disconnect();
    cout << "Done." << endl;
    ++Run;
    cout << "Set Run Number... ";
    SetRunNumber( Run );
    cout << "Done." << endl;
    cout << endl << endl << "Triggers counted: " << TriggerCount << endl;
    cout << "Efficiency: " << Eff << endl;
 
    #ifdef SHOW_HISTO
        //delete c;
        //app.Run();
    #endif
 
    return 0;
}

int32_t ReadEvents( vector< uint32_t > aData, TH1F * histo ){
  int EventCount = 0;
  int hits = 0;
  bool isData = false;

  //Reset all channels data to 0
  memset( fData, 0, sizeof( int ) * 32 );

  for( unsigned int i = 0; i < aData.size(); ++i ){
    uint32_t data = aData[ i ];
    DataWordV775 dw( data );
    //cout << dw;
    if( !( dw.GetDataType() == (uint32_t)DWV775_DataFiller ) && (dw.GetDataType() == (uint32_t)DWV775_EventTrailer) ){ 
      EventCount++;// = dw.GetEventCount() + 1;
      //cout << "Got trailer. EventCount = " << EventCount << endl;
    } else 
      if( !( dw.GetDataType() == (uint32_t)DWV775_DataFiller ) && ( dw.GetDataType() == (uint32_t)DWV775_EventDatum ) ){ 
	double d = dw.GetData() * timeBin;
	++hits;
	int ch = dw.GetChannel();
	fData[ ch ] = d;
	if( ( ch > 0 ) && !isData ) isData = true;
	hStripProf->Fill( ch );
	hTDC[ ch ]->Fill( d );
	cout << "Data channel: " << ch << endl;
      }
  }

  tree_RawData->Fill();
  if( isData ) hClusterSize->Fill( hits );
#ifdef SHOW_HISTO
  hStripProf->Draw();
  c->Update();
#endif
  return EventCount;
}

int GetNumberOfTriggers(){
    ifstream ifile( "trig.conf" );
    int TrigNum = 10;
    if( ifile.is_open() ){
        ifile >> TrigNum;
        ifile.close();
    }
    return TrigNum;
}

//For File I/O
//Returns the position of "=" in the input *.conf files for further processing in SetParameters*() Functions
std::size_t GetPosEquals(std::string strFilename, std::string & strInput, bool & bInputLineSkip){
    //Variable Declaration
    size_t ret_PosEquals;
    
    std::string strManEntry = "";
    
    //Find the "=" symbole (separates parameter name from parameter value)
    ret_PosEquals = strInput.find_first_of("=");
    
    //Loop Improper Parameter Entry
    while ( std::string::npos == ret_PosEquals && strInput.compare("") !=0 ) { //Loop Improper Entry
        cout<<"GetPosEquals() - Parsing: " <<  strFilename.c_str() << endl;
        cout<<"\tParsing line:\n";
        cout<<"\t\t"<<strInput.c_str()<<endl;
        cout<<"\tNo '=' symbole found\n";
        cout<<"\tWould you like manual entry? (y/N)";
        cin>>strManEntry;
        
        if ( 0 == strManEntry.compare("y") || 0 == strManEntry.compare("Y") ) { //Case: User Gives Manual Entry
            cout<<"\tOkay Please Give Manual Entry";
            cin>>strInput;
        } //End Case: User Gives Manual Entry
        else if ( 0 == strManEntry.compare("n") || 0 == strManEntry.compare("N") ) { //Case: User Declines Manual Entry
            bInputLineSkip = true;
            break;
        } //End Case: User Declines Manual Entry
        else{ //Case: User Input Not Understood
            cout<<"\tInput not understood, please give (y/N) for manual entry\n";
            cin>>strManEntry;
        } //End Case: User Input Not Understood
        
        ret_PosEquals = strInput.find_first_of("=");
    } //End Loop Improper Entry
    
    return ret_PosEquals;
} //End GetPosEquals()

bool GetParsedBool(std::string strInputParam, std::string strInputVal, std::string strInputFileName){
    //std::transform(strInputVal.begin(),strInputVal.end(),strInputVal.begin(),toupper);
    
    if (0 == strInputVal.compare("true") || 0 == strInputVal.compare("t") || 0 == strInputVal.compare("TRUE") || 0 == strInputVal.compare("T") || 0 == strInputVal.compare("1") ) {
        return true;
    }
    else if (0 == strInputVal.compare("false") || 0 == strInputVal.compare("f") || 0 == strInputVal.compare("FALSE") || 0 == strInputVal.compare("F") || 0 == strInputVal.compare("0") ) {
        return false;
    }
    else {
        cout<<"GetParsedBool() - Parsing " << strInputParam.c_str() << endl;
        cout<<"\tReceived " << strInputVal.c_str()<< endl;
        cout<<"\tExpected {TRUE,T,1,FALSE,F,0}, Please Cross Check Input File: " << strInputFileName.c_str() <<endl;
        cout<<"\tTHE VALUE OF FALSE HAS BEEN SET!\n";
        return false;
    }
} //End GetParsedBool()

float GetParsedFloat(std::string strInputParam, std::string strInputVal, std::string strInputFileName){
    if ( std::string::npos == strInputVal.find_first_not_of("0123456789.") ) { //Case: Only Numeric Data Present
        return atof( strInputVal.c_str() );
    } //End Case: Only Numeric Data Present
    else{ //Case: Non-numeric Data Present
        cout<<"GetParsedFloat() - Parsing " << strInputParam.c_str() << endl;
        cout<<"\tReceived " << strInputVal.c_str()<< endl;
        cout<<"\tExpected Numeric Input Please Cross Check Input File: " << strInputFileName.c_str() <<endl;
        return -1.;
    } //End Case: Non-numeric Data Present
} //End GetParsedFloat

int GetParsedInt(std::string strInputParam, std::string strInputVal, std::string strInputFileName){
    if ( std::string::npos == strInputVal.find_first_not_of("0123456789") ) { //Case: Only Numeric Data Present
        return atoi( strInputVal.c_str() );
    } //End Case: Only Numeric Data Present
    else{ //Case: Non-numeric Data Present
        cout<<"GetParsedInt() - Parsing " << strInputParam.c_str() << endl;
        cout<<"\tReceived " << strInputVal.c_str()<< endl;
        cout<<"\tExpected Numeric Input Please Cross Check Input File: " << strInputFileName.c_str()<<endl;
        return -1;
    } //End Case: Non-numeric Data Present
} //End GetParsedInt

int GetRunNumber(){
  ifstream ifile( "run.pid" );
  int RunNum = 1;
  if( ifile.is_open() ){
    ifile >> RunNum;
    ifile.close();
  }
  return RunNum;
}

//This is designed to be the full physical filepath
std::string GetRunListForWriting(){
    ifstream ifile( "runlist.conf" );
    
    string strRunList = "";
    
    if( ifile.is_open() ){
        ifile >> strRunList;
        ifile.close();
    }
    
    return strRunList;
} //End GetRunListForWriting()

/*
 vmebridge.conf -> first line aLink
 vmebridge.conf -> second line aBoardNum
 */
//Oase address of the bridge should always be 0
//ret_vector[0] -> aLink; ret_vector[1] -> aBoardNum
//This is designed to tell the code at runtime what board it should connect to
vector< short > GetVmeInterfaceInfo(){
    ifstream ifile( "vmebridge.conf");
    
    vector<short> ret_vector;
    
    short sInput;
    
    if( ifile.is_open() ){
        while( ifile.good() ){
            if(ret_vector.size() > 2){
                cout<<"vmebridge.conf does not have the expected structure\n";
                cout<<"vmebridge.conf should have only two lines: 1) optical link number, 2) number of board on the link\n";
                cout<<"please cross-check vmebridge.conf\n";
                
                break;
            }
            
            ifile >> sInput;
            
            ret_vector.push_back( sInput );
        }
        ifile.close();
    }
    
    return ret_vector;
}

void SetParametersDUT( PARAMETERS_DUT &param){
    //Variable Declaration
    bool bSkipLine = false; //Do we skip a line of a file?
    
    std::size_t iPosEquals = std::string::npos; //Location of "=" within file line
    
    string strLine, strParamName, strParamVal, strFilename = "param_dut.conf"; //File Parsing strings
    
    ifstream ifile( strFilename.c_str() );  //file input stream
    
    strLine=strParamName=strParamVal="";
    
    if ( ifile.is_open() ){ //File is Open
        while ( getline(ifile, strLine ) ) { //Loop Through File
            //Skip commented out lines
            if ( strLine.compare(0,1,"#") == 0 ) continue;
            
            //Get the position of the equals
            iPosEquals = GetPosEquals(strFilename, strLine, bSkipLine);
            
            //Skip this line due to improper parameter entry and user declines manual Entry?
            if ( bSkipLine ) continue;
            
            //Okay Check input File
            //strParamName = strLine.substr(0, iPosEquals-1);
            strParamName = strLine.substr(0, iPosEquals);
            strParamVal = strLine.substr(iPosEquals+1,strLine.length() - iPosEquals);
            
            //Debugging
            //cout<<"SetParametersDUT() - I found (strParamName,strParamVal):\n";
            //cout<<"\t("<<strParamName.c_str()<<","<<strParamVal.c_str()<<")\n";
            
            //compare strParamName to accepted inputs:
            if ( 0 == strParamName.compare( "Det1_HV_Drift" ) ) {
                param.fHV_Det1_Drift = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_HV_G1Top" ) ){
                param.fHV_Det1_G1Top = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_HV_G1Bot" ) ){
                param.fHV_Det1_G1Bot = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_HV_G2Top" ) ){
                param.fHV_Det1_G2Top = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_HV_G2Bot" ) ){
                param.fHV_Det1_G2Bot = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_HV_G3Top" ) ){
                param.fHV_Det1_G3Top = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_HV_G3Bot" ) ){
                param.fHV_Det1_G3Bot = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            /*else if( 0 == strParamName.compare( "Det1_HV_DividerCurrent" ) ){
                param.fHV_Det1_DividerCurrent = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_HV_nFilters" ) ){
                param.iHV_Det1_nFilters = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "Det2_HV_Drift" ) ) {
                param.fHV_Det2_Drift = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det2_HV_G1Top" ) ){
                param.fHV_Det2_G1Top = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det2_HV_G1Bot" ) ){
                param.fHV_Det2_G1Bot = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det2_HV_G2Top" ) ){
                param.fHV_Det2_G2Top = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det2_HV_G2Bot" ) ){
                param.fHV_Det2_G2Bot = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det2_HV_G3Top" ) ){
                param.fHV_Det2_G3Top = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det2_HV_G3Bot" ) ){
                param.fHV_Det2_G3Bot = GetParsedFloat(strParamName, strParamVal, strFilename);
            }*/
            else if( 0 == strParamName.compare( "Det2_HV_DividerCurrent" ) ){
                param.fHV_Det2_DividerCurrent = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det2_HV_nFilters" ) ){
                //param.iHV_Det2_nFilters = GetParsedInt(strParamName, strParamVal, strFilename);
		param.fHV_Det2_nFilters = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            /*else if( 0 == strParamName.compare( "Det1_Readout_Amp_Gain_Coarse" ) ){
                param.fReadout_Amp_Gain_Coarse = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Readout_Amp_Gain_Fine" ) ){
                param.fReadout_Amp_Gain_Fine = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Readout_Amp_Time_Diff" ) ){
                param.fReadout_Amp_Time_Diff = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Readout_Amp_Time_Int" ) ){
                param.fReadout_Amp_Time_Int = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Readout_Disc_CFD" ) ){
                param.bReadout_Disc_CFD = GetParsedBool(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Readout_Disc_Thresh" ) ){
                param.fReadout_Disc_Threshold = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Readout_Disc_WalkAdj" ) ){
                param.fReadout_Disc_WalkAdj = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Readout_Disc_ExtDly" ) ){
                param.fReadout_Disc_ExtDly = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Drift_Amp_Gain_Coarse" ) ){
                param.fDrift_Amp_Gain_Coarse = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Drift_Amp_Gain_Fine" ) ){
                param.fDrift_Amp_Gain_Fine = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Drift_Amp_Time_Diff" ) ){
                param.fDrift_Amp_Time_Diff = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Drift_Amp_Time_Int" ) ){
                param.fDrift_Amp_Time_Int = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Drift_Disc_CFD" ) ){
                param.bDrift_Disc_CFD = GetParsedBool(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Drift_Disc_Thresh" ) ){
                param.fDrift_Disc_Threshold = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Drift_Disc_WalkAdj" ) ){
                param.fDrift_Disc_WalkAdj = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "Det1_Drift_Disc_ExtDly" ) ){
                param.fDrift_Disc_ExtDly = GetParsedFloat(strParamName, strParamVal, strFilename);
            }*/
            else { //Case: Unrecognized Line
                cout<<"SetParametersDUT() - Parameter Pair";
                cout<<"\t("<<strParamName.c_str()<<","<<strParamVal.c_str()<<")\n";
                cout<<"\tNot Undestood. Please Cross Check Input File: " << strFilename.c_str() <<endl;
            } //End Case: Unrecognized Line
        } // End Loop Through File
    } //End File is Open
    
    ifile.close();
    
    //Debugging
    cout<<"SetParametersDUT() - Loaded Parameter List:\n";
    //cout<<"\tparam.fHV_Det1_DividerCurrent = " << param.fHV_Det1_DividerCurrent << endl;
    cout<<"\tparam.fHV_Det1_Drift = " << param.fHV_Det1_Drift << endl;
    cout<<"\tparam.fHV_Det1_G1Top = " << param.fHV_Det1_G1Top << endl;
    cout<<"\tparam.fHV_Det1_G1Bot = " << param.fHV_Det1_G1Bot << endl;
    cout<<"\tparam.fHV_Det1_G2Top = " << param.fHV_Det1_G2Top << endl;
    cout<<"\tparam.fHV_Det1_G2Bot = " << param.fHV_Det1_G2Bot << endl;
    cout<<"\tparam.fHV_Det1_G3Top = " << param.fHV_Det1_G3Top << endl;
    cout<<"\tparam.fHV_Det1_G3Bot = " << param.fHV_Det1_G3Bot << endl;
    //cout<<"\tparam.iHV_Det1_nFilters = " << param.iHV_Det1_nFilters << endl;
    
    cout<<"\tparam.fHV_Det2_DividerCurrent = " << param.fHV_Det2_DividerCurrent << endl;
    /*cout<<"\tparam.fHV_Det2_Drift = " << param.fHV_Det2_Drift << endl;
    cout<<"\tparam.fHV_Det2_G1Top = " << param.fHV_Det2_G1Top << endl;
    cout<<"\tparam.fHV_Det2_G1Bot = " << param.fHV_Det2_G1Bot << endl;
    cout<<"\tparam.fHV_Det2_G2Top = " << param.fHV_Det2_G2Top << endl;
    cout<<"\tparam.fHV_Det2_G2Bot = " << param.fHV_Det2_G2Bot << endl;
    cout<<"\tparam.fHV_Det2_G3Top = " << param.fHV_Det2_G3Top << endl;
    cout<<"\tparam.fHV_Det2_G3Bot = " << param.fHV_Det2_G3Bot << endl;*/
    cout<<"\tparam.fHV_Det2_nFilters = " << param.fHV_Det2_nFilters << endl;
    
    /*cout<<"\tparam.fReadout_Amp_Gain_Coarse = " << param.fReadout_Amp_Gain_Coarse << endl;
    cout<<"\tparam.fReadout_Amp_Gain_Fine = " << param.fReadout_Amp_Gain_Fine << endl;
    cout<<"\tparam.fReadout_Amp_Time_Diff = " << param.fReadout_Amp_Time_Diff << endl;
    cout<<"\tparam.fReadout_Amp_Time_Int = " << param.fReadout_Amp_Time_Int << endl;
    cout<<"\tparam.bReadout_Disc_CFD = " << param.bReadout_Disc_CFD << endl;
    cout<<"\tparam.fReadout_Disc_Threshold = " << param.fReadout_Disc_Threshold << endl;
    cout<<"\tparam.fReadout_Disc_WalkAdj = " << param.fReadout_Disc_WalkAdj << endl;
    cout<<"\tparam.fReadout_Disc_ExtDly = " << param.fReadout_Disc_ExtDly << endl;
    
    cout<<"\tparam.fDrift_Amp_Gain_Coarse = " << param.fDrift_Amp_Gain_Coarse << endl;
    cout<<"\tparam.fDrift_Amp_Gain_Fine = " << param.fDrift_Amp_Gain_Fine << endl;
    cout<<"\tparam.fDrift_Amp_Time_Diff = " << param.fDrift_Amp_Time_Diff << endl;
    cout<<"\tparam.fDrift_Amp_Time_Int = " << param.fDrift_Amp_Time_Int << endl;
    cout<<"\tparam.bDrift_Disc_CFD = " << param.bDrift_Disc_CFD << endl;
    cout<<"\tparam.fDrift_Disc_Threshold = " << param.fDrift_Disc_Threshold << endl;
    cout<<"\tparam.fDrift_Disc_WalkAdj = " << param.fDrift_Disc_WalkAdj << endl;
    cout<<"\tparam.fDrift_Disc_ExtDly = " << param.fDrift_Disc_ExtDly << endl;*/
    
    return;
} //End SetParametersDUT()

void SetParametersTracker( PARAMETERS_TRACKER &param){
    //Variable Declaration
    bool bSkipLine = false; //Do we skip a line of a file?
    
    std::size_t iPosEquals = std::string::npos; //Location of "=" within file line
    
    string strLine, strParamName, strParamVal, strFilename = "param_trk.conf"; //File Parsing strings
    
    ifstream ifile( strFilename.c_str() );  //file input stream
    
    strLine=strParamName=strParamVal="";
    
    if ( ifile.is_open() ){ //File is Open
        while ( getline(ifile, strLine ) ) { //Loop Through File
            //Skip commented out lines
            if ( strLine.compare(0,1,"#") == 0 ) continue;
            
            //Get the position of the equals
            iPosEquals = GetPosEquals(strFilename, strLine, bSkipLine);
            
            //Skip this line due to improper parameter entry and user declines manual Entry?
            if ( bSkipLine ) continue;
            
            //Okay Check input File
            //strParamName = strLine.substr(0, iPosEquals-1);
            strParamName = strLine.substr(0, iPosEquals);
            strParamVal = strLine.substr(iPosEquals+1,strLine.length() - iPosEquals);
            
            //Debugging
            //cout<<"SetParametersTracker() - I found (strParamName,strParamVal):\n";
            //cout<<"\t("<<strParamName.c_str()<<","<<strParamVal.c_str()<<")\n";
            
            //compare strParamName to accepted inputs:
            
            if ( 0 == strParamName.compare( "Trk1_HV" ) ) {
                param.fTracker1_HV = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "Trk2_HV" ) ) {
                param.fTracker2_HV = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "Trk3_HV" ) ) {
                param.fTracker3_HV = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else { //Case: Unrecognized Line
                cout<<"SetParametersPMT() - Parameter Pair";
                cout<<"\t("<<strParamName.c_str()<<","<<strParamVal.c_str()<<")\n";
                cout<<"\tNot Undestood. Please Cross Check Input File: " << strFilename.c_str() <<endl;
            } //End Case: Unrecognized Line
        } // End Loop Through File
    } //End File is Open
    
    cout<<"SetParametersTracker() - Loaded Parameter List:\n";
    
    cout<<"\tparam.fTracker1_HV = " << param.fTracker1_HV << endl;
    cout<<"\tparam.fTracker2_HV = " << param.fTracker2_HV << endl;
    cout<<"\tparam.fTracker3_HV = " << param.fTracker3_HV << endl;
    
    return;
} //End SetParametersTracker()

void SetParametersPMT( PARAMETERS_PMT &param){
    //Variable Declaration
    bool bSkipLine = false; //Do we skip a line of a file?
    
    std::size_t iPosEquals = std::string::npos; //Location of "=" within file line
    
    string strLine, strParamName, strParamVal, strFilename = "param_pmt.conf"; //File Parsing strings
    
    ifstream ifile( strFilename.c_str() );  //file input stream
    
    strLine=strParamName=strParamVal="";
    
    if ( ifile.is_open() ){ //File is Open
        while ( getline(ifile, strLine ) ) { //Loop Through File
            //Skip commented out lines
            if ( strLine.compare(0,1,"#") == 0 ) continue;
            
            //Get the position of the equals
            iPosEquals = GetPosEquals(strFilename, strLine, bSkipLine);
            
            //Skip this line due to improper parameter entry and user declines manual Entry?
            if ( bSkipLine ) continue;
            
            //Okay Check input File
            //strParamName = strLine.substr(0, iPosEquals-1);
            strParamName = strLine.substr(0, iPosEquals);
            strParamVal = strLine.substr(iPosEquals+1,strLine.length() - iPosEquals);
            
            //Debugging
            //cout<<"SetParametersPMT() - I found (strParamName,strParamVal):\n";
            //cout<<"\t("<<strParamName.c_str()<<","<<strParamVal.c_str()<<")\n";
            
            //compare strParamName to accepted inputs:
            if( 0 == strParamName.compare( "PMT1_Disc_CFD" ) ){
                ////param.bPMT1_Disc_CFD = GetParsedBool(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT1_HV" ) ) {
                param.fPMT1_HV = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT1_Disc_Thresh" ) ) {
                param.fPMT1_Threshold = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT1_Disc_WalkAdj" ) ) {
                param.fPMT1_WalkAdj = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT1_Disc_ExtDly" ) ) {
                param.fPMT1_Disc_ExtDly = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT1_Disc_CoinDly" ) ) {
                param.fPMT1_Disc_CoinDly = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "PMT2_Disc_CFD" ) ){
                ////param.bPMT2_Disc_CFD = GetParsedBool(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT2_HV" ) ) {
                param.fPMT2_HV = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT2_Disc_Thresh" ) ) {
                param.fPMT2_Threshold = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT2_Disc_WalkAdj" ) ) {
                param.fPMT2_WalkAdj = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT2_Disc_ExtDly" ) ) {
                param.fPMT2_Disc_ExtDly = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT2_Disc_CoinDly" ) ) {
                param.fPMT2_Disc_CoinDly = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "PMT3_Disc_CFD" ) ){
                //param.bPMT3_Disc_CFD = GetParsedBool(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT3_HV" ) ) {
                param.fPMT3_HV = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT3_Disc_Thresh" ) ) {
                param.fPMT3_Threshold = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT3_Disc_WalkAdj" ) ) {
                param.fPMT3_WalkAdj = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT3_Disc_ExtDly" ) ) {
                param.fPMT3_Disc_ExtDly = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMT3_Disc_CoinDly" ) ) {
                param.fPMT3_Disc_CoinDly = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if( 0 == strParamName.compare( "PMTDUT_Disc_CFD" ) ){
                //param.bPMTDUT_Disc_CFD = GetParsedBool(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMTDUT_HV" ) ) {
                param.fPMTDUT_HV = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMTDUT_Disc_Thresh" ) ) {
                param.fPMTDUT_Threshold = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMTDUT_Disc_WalkAdj" ) ) {
                param.fPMTDUT_WalkAdj = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMTDUT_Disc_ExtDly" ) ) {
                param.fPMTDUT_Disc_ExtDly = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else if ( 0 == strParamName.compare( "PMTDUT_Disc_CoinDly" ) ) {
                param.fPMTDUT_Disc_CoinDly = GetParsedFloat(strParamName, strParamVal, strFilename);
            }
            else { //Case: Unrecognized Line
                cout<<"SetParametersPMT() - Parameter Pair";
                cout<<"\t("<<strParamName.c_str()<<","<<strParamVal.c_str()<<")\n";
                cout<<"\tNot Undestood. Please Cross Check Input File: " << strFilename.c_str() <<endl;
            } //End Case: Unrecognized Line
        } // End Loop Through File
    } //End File is Open
    
    //Debugging
    cout<<"SetParametersPMT() - Loaded Parameter List:\n";
    
    cout<<"\t//param.bPMT1_Disc_CFD = " << //param.bPMT1_Disc_CFD << endl;
    cout<<"\tparam.fPMT1_HV = " << param.fPMT1_HV << endl;
    cout<<"\tparam.fPMT1_Threshold = " << param.fPMT1_Threshold << endl;
    cout<<"\tparam.fPMT1_WalkAdj = " << param.fPMT1_WalkAdj << endl;
    cout<<"\tparam.fPMT1_Disc_ExtDly = " << param.fPMT1_Disc_ExtDly << endl;
    cout<<"\tparam.fPMT1_Disc_CoinDly = " << param.fPMT1_Disc_CoinDly << endl;
    
    cout<<"\t//param.bPMT2_Disc_CFD = " << //param.bPMT2_Disc_CFD << endl;
    cout<<"\tparam.fPMT2_HV = " << param.fPMT2_HV << endl;
    cout<<"\tparam.fPMT2_Threshold = " << param.fPMT2_Threshold << endl;
    cout<<"\tparam.fPMT2_WalkAdj = " << param.fPMT2_WalkAdj << endl;
    cout<<"\tparam.fPMT2_Disc_ExtDly = " << param.fPMT2_Disc_ExtDly << endl;
    cout<<"\tparam.fPMT2_Disc_CoinDly = " << param.fPMT2_Disc_CoinDly << endl;
    
    cout<<"\t//param.bPMT3_Disc_CFD = " << //param.bPMT3_Disc_CFD << endl;
    cout<<"\tparam.fPMT3_HV = " << param.fPMT3_HV << endl;
    cout<<"\tparam.fPMT3_Threshold = " << param.fPMT3_Threshold << endl;
    cout<<"\tparam.fPMT3_WalkAdj = " << param.fPMT3_WalkAdj << endl;
    cout<<"\tparam.fPMT3_Disc_ExtDly = " << param.fPMT3_Disc_ExtDly << endl;
    cout<<"\tparam.fPMT3_Disc_CoinDly = " << param.fPMT3_Disc_CoinDly << endl;
    
    cout<<"\t//param.bPMTDUT_Disc_CFD = " << //param.bPMTDUT_Disc_CFD << endl;
    cout<<"\tparam.fPMTDUT_HV = " << param.fPMTDUT_HV << endl;
    cout<<"\tparam.fPMTDUT_Threshold = " << param.fPMTDUT_Threshold << endl;
    cout<<"\tparam.fPMTDUT_WalkAdj = " << param.fPMTDUT_WalkAdj << endl;
    cout<<"\tparam.fPMTDUT_Disc_ExtDly = " << param.fPMTDUT_Disc_ExtDly << endl;
    cout<<"\tparam.fPMTDUT_Disc_CoinDly = " << param.fPMTDUT_Disc_CoinDly << endl;
    
    return;
} //End SetParametersPMT()

void SetRunNumber( int RunNum ){
  //Open file for writing and erase it
  ofstream ofile( "run.pid", ios::trunc );
  if( ofile.is_open() ){
    ofile << RunNum;
    ofile.close();
  }
}

vector< int > DisableChannels(){
  ifstream ifile( "killch.conf" );
  int ChNumber = -1;
  vector< int > Res;
  if( ifile.is_open() ){
    while( ifile.good() ){  
      ifile >> ChNumber;
      cout << "Found channel " << ChNumber << " to kill." << endl;
      Res.push_back( ChNumber );
    }
    ifile.close();
  }
  return Res;
}
