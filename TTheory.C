#include "TTheory.h"
#include "./includes.h"

#if !defined(__CINT__)
ClassImp(TTheory)
#endif

// Default constructor
TTheory::TTheory(){
}

// Destructor
TTheory::~TTheory(){
}

//----------------------------------------------
// Finite Geometry Asymmetry Calculation
// Snyder 1948 
// Angular Correlation of Scattered Annihilation Radiation
// http://link.aps.org/doi/10.1103/PhysRev.73.440

// Member functions below are used to calculate
// rho = N(90)/N(0)

Float_t TTheory::X(Float_t theta){
  return ( 2. - Cos(theta) );
}

Float_t TTheory::JLim(Float_t theta){
  return ( Log(X(theta)) - 1./( 2.*X(theta)*X(theta) ) );
}

Float_t TTheory::J(Float_t theta, Float_t semiSpan){
  return ( JLim(theta+semiSpan) - JLim(theta-semiSpan) ) ;
}

Float_t TTheory::JdashLim(Float_t theta){
  return ( -X(theta) + 4.*Log(X(theta)) + 3./( X(theta) ) );  
}

Float_t TTheory::Jdash(Float_t theta, Float_t semiSpan){
  return ( JdashLim(theta+semiSpan) - JdashLim(theta-semiSpan) ) ;
}

// Asymmetry for finite theta only
// semi-span is half the size of the detector in theta
Float_t TTheory::rho1(Float_t theta, Float_t semiSpan){
  return ( 1. + 1./( 1./2.*( J(theta,semiSpan)/Jdash(theta,semiSpan) )*( J(theta,semiSpan)/Jdash(theta,semiSpan) ) - ( J(theta,semiSpan)/Jdash(theta,semiSpan) ) ) );
}

Float_t TTheory::u(Float_t alpha){
  return ( 2*alpha*alpha - 1./2.*Sin(2*alpha)*Sin(2*alpha) );
}

Float_t TTheory::w(Float_t alpha){
  return ( 2*alpha*alpha + 1./2.*Sin(2*alpha)*Sin(2*alpha) );
}

Float_t TTheory::Z(Float_t alpha){
  return (u(alpha)/w(alpha));
}

// Asymmetry for finite theta and finite phi
// alpha is half the size of the detector in phi
Float_t TTheory::rho2(Float_t theta, Float_t semiSpan, Float_t alpha){
  return ( (Z(alpha) + rho1(theta,semiSpan) )/( 1 + Z(alpha)*rho1(theta,semiSpan)) ); 
}  

//--------------------------------------------------

// Compton scattering variable conversions
Float_t TTheory::ThetaToPhotonEnergy(Float_t theta){
  return (511./(2 - Cos(TMath::DegToRad()*theta)));
}

Float_t TTheory::ThetaToElectronEnergy(Float_t theta){
  return (511. - (511./(2. - Cos(TMath::DegToRad()*theta))));
}

//--------------------------------------------------
// Plotting routine 


void TTheory::GraphFiniteAsymmetry(Int_t nBins,
				   Float_t semiSpan,
				   Char_t xVariable){
  
  
  semiSpan = DegToRad()*semiSpan;
  
  TCanvas *canvas = new TCanvas("canvas","canvas",
				10,10,1200,800);
  
  // This is a dummy histogram which is 
  // used for setting the axis on the TGraph
  TH1F * hr = new TH1F();
  
  // Plot as a function of energy or theta
  if     ( xVariable=='t') hr = canvas->DrawFrame(0.0,0.5,180.0,3.0);
  else if( xVariable=='e'){ 
    hr = canvas->DrawFrame(0.0,0.5,
			   511.,3.0);
  }

  TGraphErrors * gr[3];

  // this second graph array is used for simultaneously
  // plotting as a function of photon energy when plotting
  // as a function of electron energy (energy deposited
  // in 1st crystal)
  TGraphErrors * gr2[3];
  
  Float_t theta[nBins];
  
  Float_t elecNRG[nBins];
  Float_t gammaNRG[nBins];
  
  Float_t asymm1[nBins];
  Float_t asymm2[nBins];
  Float_t asymm3[nBins];
  
  Float_t alpha1   = DegToRad()*1.;
  Float_t alpha2   = DegToRad()*30.;
  Float_t alpha3   = DegToRad()*45.;

  for(Int_t i = 0 ; i < nBins ; i++){
    
    // symmetric bins around 90 deg
    theta[i]  = 90. - (nBins-1)*semiSpan*RadToDeg() + i*2*semiSpan*RadToDeg();
    elecNRG[i]  = ThetaToElectronEnergy(theta[i]);
    gammaNRG[i] = ThetaToPhotonEnergy(theta[i]);
    
    theta[i]  = theta[i]*DegToRad();
    
    asymm1[i] = rho2(theta[i],semiSpan,alpha1);
    asymm2[i] = rho2(theta[i],semiSpan,alpha2);
    asymm3[i] = rho2(theta[i],semiSpan,alpha3);
    
    theta[i] = theta[i]*RadToDeg();
  }


  if     ( xVariable=='t'){
    hr->GetXaxis()->SetTitle("#theta (deg)");
    gr[0] = new TGraphErrors(nBins,theta,asymm1,0,0);
    gr[1] = new TGraphErrors(nBins,theta,asymm2,0,0);
    gr[2] = new TGraphErrors(nBins,theta,asymm3,0,0);    
  }
  else if( xVariable=='e'){
    hr->GetXaxis()->SetTitle("energy (keV): electron (dashed), photon (solid)");
    gr[0]  = new TGraphErrors(nBins,elecNRG,asymm1,0,0);
    gr[1]  = new TGraphErrors(nBins,elecNRG,asymm2,0,0);
    gr[2]  = new TGraphErrors(nBins,elecNRG,asymm3,0,0);    
    gr2[0] = new TGraphErrors(nBins,gammaNRG,asymm1,0,0);
    gr2[1] = new TGraphErrors(nBins,gammaNRG,asymm2,0,0);
    gr2[2] = new TGraphErrors(nBins,gammaNRG,asymm3,0,0);    
  }

  gr[0]->SetLineColor(kRed+1);
  gr[0]->SetMarkerColor(kRed+1);
  gr[0]->SetMarkerStyle(20);
  gr[0]->SetLineStyle(2);

  if( xVariable=='e'){  
    gr2[0]->SetLineColor(kRed-2);
    gr2[0]->SetMarkerColor(kRed-2);
    gr2[0]->SetMarkerStyle(20);
  }
  
  gr[1]->SetLineColor(kBlue+1);
  gr[1]->SetMarkerColor(kBlue+1);
  gr[1]->SetMarkerStyle(20);
  gr[1]->SetLineStyle(2);
  
  if( xVariable=='e'){
    gr2[1]->SetLineColor(kBlue-2);
    gr2[1]->SetMarkerColor(kBlue-2);
    gr2[1]->SetMarkerStyle(20);
  }

  gr[2]->SetLineColor(kGreen+3);
  gr[2]->SetMarkerColor(kGreen+3);
  gr[2]->SetMarkerStyle(20);
  gr[2]->SetLineStyle(2);
  
  if( xVariable=='e'){
    gr2[2]->SetLineColor(kGreen+1);
    gr2[2]->SetMarkerColor(kGreen+1);
    gr2[2]->SetMarkerStyle(20);
  }
  
  TString plotStyle = "PL";
  
  gr[0]->Draw(plotStyle);
  
  plotStyle = plotStyle + "same";
  
  gr[1]->Draw(plotStyle);
  gr[2]->Draw(plotStyle);

  if( xVariable=='e'){
    gr2[0]->Draw(plotStyle);
    gr2[1]->Draw(plotStyle);
    gr2[2]->Draw(plotStyle);
  }
  
  hr->GetYaxis()->SetTitle("P(#Delta#phi = 90)/P(#Delta#phi = 0)");
  
  Char_t plotName[128]; 
    
  if     ( xVariable=='t')
    sprintf(plotName,"../Plots/FiniteAsymmetry_%d_theta.pdf", nBins);
  else if( xVariable=='e')
    sprintf(plotName,"../Plots/FiniteAsymmetry_%d_energy.pdf", nBins);
  
  canvas->SaveAs(plotName);
  
}

//--------------------------------------------------
