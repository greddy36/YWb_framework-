#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"

Double_t fitfn(Double_t *x, Double_t *par) {
  Double_t mass = x[0]; 
  Double_t mean = par[0]; 
  Double_t sigma1 = par[1];
  Double_t sigma2 = par[2];
  Double_t n_gaus = par[3];
  Double_t n_exp = par[4];
  Double_t s = par[5];
  Double_t n = par[6];
  Double_t t = par[7]; 

  if (mass < mean)
    return n_gaus*std::exp(- 0.5 * pow((mass-mean)/sigma1, 2));
  
  else if (mass >= mean && mass < t)
    return n_gaus*std::exp(- 0.5 * pow((mass-mean)/sigma2, 2));

  else if (mass >= t)
    return n_exp*std::exp(-s*pow(mass,n));

  else
    return 0;

}

void asymmgausexp(){
 Double_t xmin = 500., xmax = 3000.; // whatever you need

 TF1 *asymgausexp = new TF1("asymgausexp", fitfn, xmin, xmax, 8);
 asymgausexp->SetParNames("Mean", "Sigma1", "Sigma2", "N_gaus", "N_exp", "s", "n", "t");
 TFile *f = new TFile("templates-single.root");
 TH1F *hpx = (TH1F*)f->Get("ljets__wjets");
 asymgausexp->SetParameters(hpx->GetMean(), hpx->GetRMS(), hpx->GetRMS(), 140, 140, 1,2,1200);
 hpx->Fit("asymgausexp");

// cout<<asymgausexp->GetParameter(0)<<endl;

}