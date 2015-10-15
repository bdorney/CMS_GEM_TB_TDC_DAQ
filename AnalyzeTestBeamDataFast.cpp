#include <TFile>

//Input File----------------------------------------------------------------------
TFile *inputData	 = new TFile("data/Test08_WithSBits.root");

void AnalyzeTestBeamDataFast(){
	//Load Histograms---------------------------------------------------------
	//Scintillators
	TH1F *hScint_A		= new TH1F((TFile->Get("TDC_Ch0")));	hScint_A->SetName("hScint_A");
	TH1F *hScint_B		= new TH1F((TFile->Get("TDC_Ch1")));	hScint_A->SetName("hScint_B");
	TH1F *hScint_C		= new TH1F((TFile->Get("TDC_Ch2")));	hScint_A->SetName("hScint_C");

	//Triggers
	TH1F *hScint_Coin	= new TH1F((TFile->Get("TDC_Ch4")));	hScint_Coin->SetName("hScint_Coin");
	TH1F *hTrig2TURBO	= new TH1F((TFile->Get("TDC_Ch5")));	hTrig2TURBO->SetName("hTrig2TURBO");

	//SBITs
	TH1F *hSBIT_Master	= new TH1F((TFile->Get("TDC_Ch8")));	hSBIT_Master->SetName("hSBIT_Master");
	TH1F *hSBIT_Slave1	= new TH1F((TFile->Get("TDC_Ch9")));	hSBIT_Slave1->SetName("hSBIT_Slave1");
	TH1F *hSBIT_Slave2	= new TH1F((TFile->Get("TDC_Ch10")));	hSBIT_Slave2->SetName("hSBIT_Slave2");

	TH1F *hSBIT_OR		= new TH1F((TFile->Get("TDC_Ch12")));	hSBIT_OR->SetName("hSBIT_OR");
	TH1F *hSBIT_AND		= new TH1F((TFile->Get("TDC_Ch13")));	hSBIT_OR->SetName("hSBIT_AND");
	
	cout<<"Channel"<<"\t"<<"Eff"<<"\t"<<endl;
	cout<<"SBIT_M" <<"\t"<<hSBIT_Master->GetBinContent(hSBIT_Master->GetMaximum()) / hSBIT_Master->Integral();
	cout<<"SBIT_S1"<<"\t"<<hSBIT_Slave1->GetBinContent(hSBIT_Slave1->GetMaximum()) / hSBIT_Slave1->Integral();
	cout<<"SBIT_S2"<<"\t"<<hSBIT_Slave2->GetBinContent(hSBIT_Slave2->GetMaximum()) / hSBIT_Slave2->Integral();

	cout<<"SBIT_OR"<<"\t"<<hSBIT_OR->GetBinContent(hSBIT_OR->GetMaximum()) / hSBIT_OR->Integral();
	cout<<"SBIT_AND"<<"\t"<<SBIT_AND->GetBinContent(SBIT_AND->GetMaximum()) / SBIT_AND->Integral();
} //End AnalyzeTestBeamData