#include "TFile.h"
#include "TList.h"
#include "TH1.h"
#include "TROOT.h"
#include <vector>
#include <algorithm>
#include <iostream>

void Merge( string aRun1FileName, string aRun2FileName ){
				gROOT->Reset();
				TFile * f1 = new TFile( aRun1FileName.c_str() );
				TFile * f2 = new TFile( aRun2FileName.c_str() );
				TH1F * h1 = ( TH1F* ) f1->Get( "hTrig" )->Clone( "h2" );
				TH1F * h2 = ( TH1F* ) f2->Get( "hTrig" )->Clone( "h1" );

				TFile *newfile0 = new TFile("dummy.root","recreate");
				TList *list = new TList;
				list->Add( h1 );
				list->Add( h2 );
				TFile * f3 = new TFile( "merge.root", "RECREATE" );
				TH1F *hTM = new TH1F( "hTrigMerge", "hTrig", 1200, 0, 1200 );//( TH1F* )h1->Clone( "hTrigMerge" );
				hTM->Reset();
				hTM->Merge( list );
				
				vector< float > vec( 1200 );
				for( int i = 1; i < 1201; ++i ) vec[ i ] = hTM->GetBinContent( i );
				reverse( vec.begin(), vec.end());
				for( int i = 0; i < vec.size(); ++i ) hTM->SetBinContent( i, vec[ i ] );
				hTM->GetXaxis()->SetRange( 1, 1200 );
				//hTM->Draw();
				TH1F *hTR = ( TH1F* )hTM->Clone( "hTrigRebin" );
				hTR->Rebin( 80 );
				//hTR->GetXaxis()->SetRange( 0, 1200 );
				//hTR->Draw();
				hTR->Write();
				hTM->Write();
				f3->Close();
}	
