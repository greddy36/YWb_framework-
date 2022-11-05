#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"

// This is a function to fit the jets data
// It is a combination of a gaussian and an exponential
// Currently there is an error with the exponential function. 
// When it is used to fit, there is an error saying that the fit is out of range of the histogram
double jets_fit(double *x, double *par){
// Define Parameters
double a = par[0]; // Gaussian
double b = par[1]; // Gaussian
double c = par[2]; // Gaussian
double d = par[3]; // Exponential
double f = par[4]; // Exponential
double g = par[5]; // Determines which function
// Define functions
if(x[0] < g)
return a * std::exp(-.5 * std::pow(((x[0] - b)/c),2));
else if(x[0] >= g)
return std::exp(d + f * x[0]);
else
return 0;
}

// Fit the Graph
void jets_fit(){
  TCanvas *c1 = new TCanvas("c1","c1",700,500);
  TPad* thePad = (TPad*)c1->cd();
  TFile *f = new TFile("templates-single.root");
  TH1F *hpx = (TH1F*)f->Get("ljets__wjets");
  TF1 *gexp = new TF1("gexp", jets_fit, 2000,3000, 6);
  hpx->GetYaxis()->SetRangeUser(0, 1250);
  //gStyle->SetOptFit(1);
  gexp->SetParameters(hpx->GetMean(), hpx->GetMean(), hpx->GetRMS(), hpx->GetMean(), hpx->GetMean(), hpx->GetMean()); 
  gexp->SetParNames ("a", "b", "c", "d", "f", "g");
  hpx->Fit(gexp, "", "", 255, 420);
  Double_t chi2 = gexp->GetChisquare();
  cout << "chi2 = " << chi2 << endl; 
  hpx->Draw("gexp");
  hpx->GetXaxis()->SetRangeUser(500, 3400);
  thePad->Print("powerFit.pdf");
  int fitStatus = hpx->Fit("gexp","", "", 500, 3000); // Set last to based on histogram x-axis
}