#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"

// This is three Gaussians
double signal_fit(double *x, double *par){
// Define parameters
double a = par[0]; // First Gaussian
double b = par[1]; // First Gaussian
double c = par[2]; // First Gaussian
double d = par[3]; // Second Gaussian
double f = par[4]; // Second Gaussian
double g = par[5]; // Second Gaussian
double h = par[6]; // Third Gaussian
double i = par[7]; // Third Gaussian
double j = par[8]; // Third Gaussian
double k = par[9]; // Determines Gaussian between one and two
double l = par[10]; // Determines Gaussian between two and three
//create functions
if(x[0] <= k)
return a * std::exp(-.5 * std::pow(((x[0] - b)/c),2));
else if(x[0] >k && x[0] <= l)
return d * std::exp(-.5 * std::pow(((x[0] - f)/g),2));
else if(x[0] > l)
return h * std::exp(-.5 * std::pow(((x[0] - i)/j),2));
else
return 0;
}

// Fit the Graph
void signal_fit(){
  TCanvas *c1 = new TCanvas("c1","c1",700,500);
  TPad* thePad = (TPad*)c1->cd();
  TFile *f = new TFile("templates-single.root");
  TH1F *hpx = (TH1F*)f->Get("ljets__signal2400");
  TF1 *MultiGauss = new TF1("MultiGauss", signal_fit, 2000,3000, 11);
  hpx->GetYaxis()->SetRangeUser(0, 1250);
  //gStyle->SetOptFit(1);
  MultiGauss->SetParameters(hpx->GetMean(), hpx->GetMean(), hpx->GetRMS(), hpx->GetMean(), hpx->GetMean(), hpx->GetRMS(), hpx->GetMean(), hpx->GetMean(), hpx->GetRMS(), hpx->GetMean(), hpx->GetMean()); 
  MultiGauss->SetParNames ("a", "b", "c", "d", "f", "g", "h", "i", "j", "k", "l");
  hpx->Fit(MultiGauss, "", "", 255, 420);
  Double_t chi2 = MultiGauss->GetChisquare();
  cout << "chi2 = " << chi2 << endl; 
  hpx->Draw("MultiGauss");
  hpx->GetXaxis()->SetRangeUser(500, 3400);
  thePad->Print("powerFit.pdf");
  int fitStatus = hpx->Fit("MultiGauss","", "", 500, 3000); // Set last to based on histogram x-axis
}