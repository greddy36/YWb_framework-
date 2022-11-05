#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"

Double_t cb_fitfn(Double_t *x, Double_t *par) {
  Double_t m_digamma = x[0]; 
  Double_t c = par[0];
  Double_t mean = par[1]; 
  Double_t sigma = par[2];
  Double_t alpha_high = abs(par[3]);
  Double_t alpha_low = abs(par[4]);
  Double_t n_high = par[5];
  Double_t n_low = par[6];

  Double_t z = (m_digamma - mean)/sigma; 

  if (z >= -alpha_low && z <= alpha_high)
    return c*std::exp(- 0.5 * z * z);
  
  else if (z < -alpha_low){
    Double_t nDivAlpha = n_low/alpha_low;
    Double_t AA =  c*std::exp(-0.5*alpha_low*alpha_low);
    Double_t B = nDivAlpha - alpha_low;
    Double_t arg = nDivAlpha/(B-z);
    return AA * std::pow(arg,n_low);
  }
  else if (z > alpha_high){
    Double_t nDivAlpha = n_high/alpha_high;
    Double_t AA =  c*std::exp(-0.5*alpha_high*alpha_high);
    Double_t B = nDivAlpha - alpha_high;
    Double_t arg = nDivAlpha/(B+z);
    return AA * std::pow(arg,n_high);
  } 
  else
    return 0;

}

void dcb2(){
 Double_t xmin = 500., xmax = 3000.; // whatever you need

 TF1 *crystalball = new TF1("crystalball", cb_fitfn, xmin, xmax, 7);
 crystalball->SetParNames("Constant", "Mean", "Sigma", "Alpha_high", "Alpha_low", "N_high", "N_low");
 TFile *f = new TFile("templates-single.root");
 TH1F *hpx = (TH1F*)f->Get("ljets__signal1200");
 crystalball->SetParameters(5000, hpx->GetMean(), hpx->GetRMS(), 1, 1, 2,2);
 TFitResultPtr hp = hpx->Fit("crystalball","S");
 hp->Print("V");

// cout<<crystalball->GetParameter(0)<<endl;

}