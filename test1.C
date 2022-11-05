#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
  
double lognormal_pdf(double *var, double *par){  
  double x = var[0], sigma = par[0], x0 = par[1], mean = par[2];

  if ((x-x0) <= 0)
    return 0.0;
  else{
    double tmp = (std::log(x-x0) - mean)/sigma;
    return 51100*std::exp(-tmp * tmp/2)/((x-x0) * std::fabs(sigma) * std::sqrt(2 * M_PI));//40000-top, 100000-wjet, 51000-stop, 1129-zjets
  } 
}

double poisson_pdf(unsigned int n, double mu){
  if (n >  0)
    return std::exp (n*std::log(mu) - ROOT::Math::lgamma(n+1) - mu);
  else{
  //  when  n = 0 and mu = 0,  1 is returned
  if (mu >= 0) return std::exp(-mu);
  // return a nan for mu < 0 since it does not make sense
  return std::log(mu);
  }
}

double fitfn(double *x, double *par) {
  double mass = x[0]; 
  double mean = par[0]; 
  double sigma1 = par[1];
  double sigma2 = par[2];
  double n_gaus = par[3];
  double n_exp = par[4];
  double s = par[5];
  double n = par[6];
  double t = par[7]; 

  if (mass < mean)
    return n_gaus*std::exp(- 0.5 * pow((mass-mean)/sigma1, 2));
  
  else if (mass >= mean && mass < t)
    return n_gaus*std::exp(- 0.5 * pow((mass-mean)/sigma2, 2));

  else if (mass >= t)
    return n_exp*std::exp(-s*pow(mass,n));

  else
    return 0;

}
//S = S0+S1* ln(x) +S2*x+S3*x^2 use this sometime


void test1(){
 TFile *f = new TFile("templates-single.root");
 TH1F *hpx = (TH1F*)f->Get("ljets__stop");
 int xmin = hpx->GetXaxis()->GetXmin(); 
 int xmax = hpx->GetXaxis()->GetXmax();

 TF1 *asymgausexp = new TF1("asymgausexp", lognormal_pdf, xmin, xmax, 3);
 asymgausexp->SetParNames("Sigma", "X0", "Mean");
    
  Int_t nbins = hpx->GetNbinsX();
  TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
  for (int i = 0; i < nbins+2; i++){
   int yaxis = hpx->GetBinContent(i);
   double xaxis = std::log(xmin+i*(xmax-xmin)/nbins);
   h1->Fill(xaxis,yaxis);
  }

 asymgausexp->SetParameters(h1->GetStdDev(),0, h1->GetMean());
 hpx->Fit("asymgausexp");
 //h1->Draw();

// cout<<asymgausexp->GetParameter(0)<<endl;

}