{

  Double_t referencePlaneLocUp = -1.7644813;
  Double_t referencePlaneLocDown = 1.7643602;
  Double_t xmin = -3.5;
  Double_t xmax = 3.5;
  Double_t ymin = 5.4;
  Double_t ymax = 6.2;
  Double_t C_ymin = 5.4;
  Double_t C_ymax = 6.2;

  TFile* errorsFile = new TFile( "measure_emittance_errors.root", "READ" );
  TFile* graphsFile = new TFile( "measure_emittance_graphs.root", "READ" );

  TGraph* emittanceGraph = (TGraph*)graphsFile->Get( "mean( z ):emittance( x y )" );

  TH1F* upstreamEmittance = (TH1F*)errorsFile->Get( "emittance_mc_upstream" );
  TH1F* downstreamEmittance = (TH1F*)errorsFile->Get( "emittance_mc_downstream" );
  TH1F* upstreamEmittance_corrected = (TH1F*)errorsFile->Get( "emittance_mc_upstream_corrected" );
  TH1F* downstreamEmittance_corrected = (TH1F*)errorsFile->Get( "emittance_mc_downstream_corrected" );

  Int_t upstreamN = upstreamEmittance->GetEntries();
  Int_t downstreamN = downstreamEmittance->GetEntries();

  TFile* outfile = new TFile( "emittance_errors.root", "RECREATE" );

  gStyle->SetOptStat( 0 );

  Double_t x[2];
  Double_t y[2];
  Double_t dx[2];
  Double_t dy[2];

  x[0] = referencePlaneLocUp;
  x[1] = referencePlaneLocDown;

  y[0] = upstreamEmittance->GetMean();
  y[1] = downstreamEmittance->GetMean();

  ymin = 0.975 * y[1];
  ymax = 1.025 * y[0];

  dx[0] = 0.0;
  dx[1] = 0.0;

  dy[0] = upstreamEmittance->GetRMS() * ( 1.0 / sqrt( upstreamN ) );
  dy[1] = downstreamEmittance->GetRMS() * ( 1.0 / sqrt( downstreamN ) );
//  dy[0] = upstreamEmittance->GetRMS();
//  dy[1] = downstreamEmittance->GetRMS();


  TGraphErrors* emittanceErrorsGraph = new TGraphErrors( 2, x, y, dx, dy );

  emittanceErrorsGraph->SetMarkerColor( 2 );
  emittanceErrorsGraph->SetLineColor( 2 );
  emittanceErrorsGraph->SetFillColor( 2 );
  emittanceErrorsGraph->SetMarkerStyle( 21 );

  TCanvas* emittanceCanvas = new TCanvas( "emittanceCanvas", "MICE Emittance Measurements" );

  TH1F* frame = emittanceCanvas->DrawFrame( xmin, ymin, xmax, ymax ); 

  frame->GetXaxis()->SetTitle( "Z Position  /  m" );
  frame->GetXaxis()->SetTitleSize( 0.05 );
  frame->GetXaxis()->SetLabelSize( 0.045 );
  frame->GetYaxis()->SetTitle( "Emittance  /  mm");
  frame->GetYaxis()->SetTitleSize( 0.05 );
  frame->GetYaxis()->SetLabelSize( 0.045 );

  emittanceGraph->Draw( "L" );
  emittanceErrorsGraph->Draw( "P" );


  TLegend* legend = new TLegend( 0.5, 0.75, 0.9, 0.9 );

  legend->SetFillColor( 0 );

  legend->AddEntry( emittanceGraph, "MC Beam Emittance", "l" );
  legend->AddEntry( emittanceErrorsGraph, "Reconstruction of 2500 Muons", "p" );
  legend->SetTextSize( 0.04 );

  legend->Draw();


  emittanceCanvas->Update();

  emittanceCanvas->SaveAs( "emittance_errors.png" );

  emittanceGraph->Write();
  emittanceErrorsGraph->Write();

//////////////////////////////////////////////////////////////////////

  if ( ( upstreamEmittance_corrected->GetEntries() > 0 ) && ( downstreamEmittance_corrected->GetEntries() > 0 ) )
  {

    Double_t C_x[2];
    Double_t C_y[2];
    Double_t C_dx[2];
    Double_t C_dy[2];

    C_x[0] = referencePlaneLocUp;
    C_x[1] = referencePlaneLocDown;

    C_y[0] = upstreamEmittance_corrected->GetMean();
    C_y[1] = downstreamEmittance_corrected->GetMean();

    C_ymin = 0.975 * y[1];
    C_ymax = 1.025 * y[0];

    C_dx[0] = 0.0;
    C_dx[1] = 0.0;

    C_dy[0] = upstreamEmittance->GetRMS() * ( 1.0 / sqrt( upstreamN ) );
    C_dy[1] = downstreamEmittance->GetRMS() * ( 1.0 / sqrt( downstreamN ) );


    TGraphErrors* emittanceErrorsGraph_corrected = new TGraphErrors( 2, C_x, C_y, C_dx, C_dy );

    emittanceErrorsGraph_corrected->SetMarkerColor( 4 );
    emittanceErrorsGraph_corrected->SetLineColor( 4 );
    emittanceErrorsGraph_corrected->SetFillColor( 4 );
    emittanceErrorsGraph_corrected->SetMarkerStyle( 21 );


    TCanvas* emittanceCanvas_corrected = new TCanvas( "emittanceCanvas_corrected", "MICE Corrected Emittance Measurements" );

    TH1F* C_frame = emittanceCanvas_corrected->DrawFrame( xmin, C_ymin, xmax, C_ymax ); 

    C_frame->GetXaxis()->SetTitle( "Z Position  /  m" );
    C_frame->GetXaxis()->SetTitleSize( 0.05 );
    C_frame->GetXaxis()->SetLabelSize( 0.045 );
    C_frame->GetYaxis()->SetTitle( "Emittance  /  mm");
    C_frame->GetYaxis()->SetTitleSize( 0.05 );
    C_frame->GetYaxis()->SetLabelSize( 0.045 );

    emittanceGraph->Draw( "L" );
    emittanceErrorsGraph_corrected->Draw( "P" );


    TLegend* C_legend = new TLegend( 0.5, 0.75, 0.9, 0.9 );

    C_legend->SetFillColor( 0 );

    C_legend->AddEntry( emittanceGraph, "MC Beam Emittance", "l" );
    C_legend->AddEntry( emittanceErrorsGraph_corrected, "Reconstruction of 2500 Muons", "p" );
    C_legend->SetTextSize( 0.04 );

    C_legend->Draw();


    emittanceCanvas_corrected->Update();

    emittanceCanvas_corrected->SaveAs( "emittance_errors_corrected.png" );

    emittanceErrorsGraph_corrected->Write();

  }


//////////////////////////////////////////////////////////////////////




  TCanvas* upstreamEmittanceCanvas = new TCanvas( "upstreamEmittanceCanvas", "Upstream Emittance Histogram" );

  upstreamEmittance->GetXaxis()->SetTitle( "Emittance  /  mm" );
  upstreamEmittance->GetYaxis()->SetTitle( "Frequency" );

  upstreamEmittance->Draw();
  upstreamEmittance->Write();





  TCanvas* downstreamEmittanceCanvas = new TCanvas( "downstreamEmittanceCanvas", "Downstream Emittance Histogram" );

  downstreamEmittance->GetXaxis()->SetTitle( "Emittance  /  mm" );
  downstreamEmittance->GetYaxis()->SetTitle( "Frequency" );

  downstreamEmittance->Draw();
  downstreamEmittance->Write();

}

