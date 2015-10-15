//Input File----------------------------------------------------------------------
TFile *inputData	 = new TFile("data/Test08_WithSBits.root");

Double_t determineFractionalBinContent(TH1F *histOfInt){
	double histInt = histOfInt->Integral();

	int nBins	= histOfInt->GetNbinsX();	//number of bins in the histogram
	int maxBin	= histOfInt->GetMaximum();	//Bin with maximum bin content in the histogram

	Double_t ret_fracContent[100];

	if( (maxBin - 50)>= 1){ //Begin Range Check
		int lastBin=0					//final bin considered in the loop

		//Check upper range
		if( (maxBin+50) < nBins){ lastBin=maxBin+50;}	//Symmetric Limits
		else{ lastBin=nBins; }				//Use last bin of histogram

		for(int i=maxBins-50; i<lastBin; i++){ //Loop Over Selected Histogram Bins
			ret_fracContent[i] = histOfInt->GetBinContent(i) / histInt; //Determine fractional bin content
		} //End Loop Over Selected Histogram Bins
	} //End Range check
	else{ //Default Range
		int lastBin=100					//final bin considered in the loop

		for(int i=1; i<lastBin; i++){ //Loop Over Selected Histogram Bins
			ret_fracContent[i] = histOfInt->GetBinContent(i) / histInt; //Determine fractional bin content
		} //End Loop Over Selected Histogram Bins
	} //End Default Range

	return ret_fracContent;
} //End determineFractionalBinContent()


void AnalyzeTestBeamData(bool checkTimeRes, bool checkTimeEff){
	Double_t fracCont_Master;	//Factional Bin Content of hSBIT_Master
	Double_t fracCont_Slave1;	//As above  but for hSBIT_Slave1
	Double_t fracCont_Slave2;	//"			"_Slave2

	Double_t fracCont_OR;		//"			"_OR
	Double_t fracCont_AND;		//"			"_AND

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

	//Check Time Resolution---------------------------------------------------
	if(checkTimeRes){

	} //End checkTimeRes
	
	//Check Efficiency of One Clock Cycle-------------------------------------
	if(checkTimeEff){
// 		fracCont_Master	= determineFractionalBinContent(hSBIT_Master);
// 		fracCont_Slave1	= determineFractionalBinContent(hSBIT_Slave1);
// 		fracCont_Slave2	= determineFractionalBinContent(hSBIT_Slave1);

// 		fracCont_OR	= determineFractionalBinContent(hSBIT_OR);
// 		fracCont_AND	= determineFractionalBinContent(hSBIT_AND);
	} //End checkTimeEff

} //End AnalyzeTestBeamData

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