{
  gStyle->SetMarkerStyle(20.);

  Float_t abs[51];
  Float_t y[51];

  // read data points from file
  ifstream inputfile( "s-curve.txt");  
  for ( Int_t i = 0; i < 51; i++ )  {
    inputfile >> abs[i];
    inputfile >> y[i];
  }

  // fitfunction = p2 / ( 1 + exp(-p0*x+p1) )
  TF1 *fitfunc = new TF1( "fitfunc", "[2]/(1.+exp(-[0]*x+[1]))", 50., 140.);

  // plot and do the fit
  TGraph *plot = new TGraph ( 51, abs, y );
  plot->SetTitle("s-curve");
  plot->Draw("AP");
  plot->Fit(fitfunc);
}
