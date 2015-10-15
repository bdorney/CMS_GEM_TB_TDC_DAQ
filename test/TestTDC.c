//CAEN Boards
//#include "IDaqVmeInterface.hh"
#include "IDaqV513.hh"
#include "IDaqV775.hh"
#include "IDaqV792.hh"

//C++ Includes
#include <iostream>
#include <fstream>			// needed for IO files
#include <unistd.h>   // needed for sleep
#include <sstream>
#include <sys/time.h> // needed for gettimeofday
#include <time.h>     // needed for nanosleep
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
const unsigned FULL_SCALE = 1200;

IDaqVmeInterface * vmeInt;
vector<TH1F *> hTDC;
vector<TH1F *> hToF;
TH1F * hTrig;
TH1F * hTrig1;
TH1I * hStripProf;
TH1I * hClusterSize;
TTree * fTree;
int fData[ 32 ];
int Run;
double timeBin = 0.0;

#ifdef SHOW_HISTO
	int zero = 0;
	TApplication app( "appKey", &zero, NULL );
	TCanvas * c;
	//TH1F * hCh0;
#endif

int32_t ReadEvents( vector< uint32_t >, TH1F* );
//int ReadQDC( IDaqV792 * q );

int GetRunNumber();
void SetRunNumber( int RunNum );
int GetNumberOfTriggers();
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
  int FullScaleSet = (int)36454.4/FULL_SCALE;
  FullScaleSet = 0x1E;
  if( FullScaleSet < 0x1E ) FullScaleSet = 0x1E;
  if( FullScaleSet > 0xFF ) FullScaleSet = 0xFF; 
	
  Run = GetRunNumber();
  //stringstream fName;
  std::string fName;
  //fName << "./data/Run_" << Run << ".root";
  std::cout << "Please enter file name: ";
  std::cin >> fName;
  fName = "./data/" + fName + ".root"; 
  f.Open( fName.c_str(), "RECREATE" );
  fTree = new TTree( "CMS_RPC_TREE", "CMS RPC TEST RAW DATA" );

  // Create TDC and trig - channel (ToF) histograms for 8 channels
  for( unsigned int i = 0; i < 33; ++i ){
    stringstream hName;
    stringstream hDescr;
    
    hName << "TDC_Ch" <<  i;
    hDescr << "TDC Channel " << i <<";Time [ns];Count";
    TH1F * h1 = new TH1F( hName.str().c_str(), hDescr.str().c_str(), FULL_SCALE, 0, FULL_SCALE ); 
    hTDC.push_back( h1 );
    if( i < 32 ) fTree->Branch( hName.str().c_str(), &fData[ i ] );
    hName.str("");

  }
  //  hTrig = new TH1F( "hTrig", "hTrig", FULL_SCALE + 100, 0, FULL_SCALE + 100 );
  // hTrig1 = new TH1F( "hCh18", "hCh18", FULL_SCALE + 100, 0, FULL_SCALE + 100 );
  hStripProf = new TH1I( "hStripProf", "Strips Profile;Channels;Count", 32, 0, 32 );
  hClusterSize = new TH1I( "hClusSize", "Cluster Size;TDC Hits per Event;Count", 32, 0, 32 );

  vmeInt = new IDaqVmeInterface();
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
      
	cout << "Event Triggered!!!" << endl;
	
      while( ( !tdc->DReady() ) || (tdc->IsBusy()) ){ 
	if ( nanosleep( &rt1, &at1 ) != 0 ) cout << "Nanosleep failed" << endl;
      } //HACK1
	    
	cout << "TDC Ready!!!" << endl;

      evCount = tdc->Readout( Data );
      cout << "Iterations  = " << iter << endl;
      globEventCount += ReadEvents( Data, hTDC[ 0 ] );
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
  }
  
  //Write the histograms to the root file
  cout << "Going to write root file..." << endl;
  for( unsigned int i = 0; i < hTDC.size(); ++i ){
    cout << "Writing hTDC[ " << i << " ]" << endl;
    hTDC[ i ]->Write();
  } 

  hClusterSize->Write();
  delete hClusterSize;
  int TriggerCount = hStripProf->GetBinContent( 1 );//Get the trigger count from the first bin in histogram. 0 bin is underflow;
  double Eff = 0.;
  for( unsigned int i = 2; i < 33; ++i ){
	Eff += hStripProf->GetBinContent( i );
  }
  Eff /= TriggerCount;
  hStripProf->Write();
  delete hStripProf;

  //hTrig->Write();
  //  delete hTrig;
  //hTrig1->Write();
  //delete hTrig1;
  cout << "Done." << endl;
  fTree->Write(); delete fTree;
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
  delete c;
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

  fTree->Fill();
  if( isData ) hClusterSize->Fill( hits );
#ifdef SHOW_HISTO
  hStripProf->Draw();
  c->Update();
#endif
  return EventCount;
}


int GetRunNumber(){
  ifstream ifile( "run.pid" );
  int RunNum = 1;
  if( ifile.is_open() ){
    ifile >> RunNum;
    ifile.close();
  }
  return RunNum;
}

void SetRunNumber( int RunNum ){
  //Open file for writing and erase it
  ofstream ofile( "run.pid", ios::trunc );
  if( ofile.is_open() ){
    ofile << RunNum;
    ofile.close();
  }
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
