#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
//-----------------------------------------------------------------------
double lognormal_pdf(double *var, double *par){  
  double x = var[0], x0 = par[0], sigma = par[1], mean = par[2], constant = par[3], a =par[4];

  if ((x-x0) <= 0)
    return 0.0;
  else{
    double tmp = (std::log(x-x0) - mean)/sigma;
    return constant * std::exp(-tmp *tmp*a/2)/((x-x0)*std::fabs(sigma) * std::sqrt(2 * M_PI));//40000-top, 100000-wjet, 51000-stop, 1129-zjets
  }
}
//------------------------------------------------------------------------
/*double 2gaus_expo(double *var, double *par){  
  double x = var[0], cut1 = par[0], cut2 = par[1];
  if (
}*/
//--------------------------------------------------------------------------
double new_lognormal_pdf(double *var, double *par){  
  double x = var[0], x0 = par[0], sigma = par[1], mean = par[2], constant = par[3], a =par[4], b=par[5], c=par[6];
  if ((x-x0) <= 0)
    return 0.0;
  else {
    double tmp = (std::log(x-x0) - mean)/sigma;
    return constant * std::exp(-a*tmp-b*tmp*tmp)/(((x-x0)+c*(x-x0)*(x-x0))* std::fabs(sigma) * std::sqrt(2 * M_PI));//40000-top, 100000-wjet, 51000-stop, 1129-zjets
  }

}
//S = S0+S1* ln(x) +S2*x+S3*x^2 use this sometime
//-----------------------------------------------------------------------------
/*
void log_normal(){//lognormal
 TFile *f = new TFile("templates-single.root");
 TH1F *hpx = (TH1F*)f->Get("ljets__zjets");
 int xmin = hpx->GetXaxis()->GetXmin(); 
 int xmax = hpx->GetXaxis()->GetXmax();

 TF1 *fitting = new TF1("fitting", lognormal_pdf, xmin, xmax, 5);
 fitting->SetParNames("X0", "Sigma", "Mean", "Constant", "alpha");
    
  Int_t nbins = hpx->GetNbinsX();
  //finding stddev and mean for log(x) histogram
  TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
  for (int i = 0; i < nbins+2; i++){
   int yaxis = hpx->GetBinContent(i);
   double xaxis = std::log(xmin+i*(xmax-xmin)/nbins);
   h1->Fill(xaxis,yaxis);
  }
 fitting->SetParNames("X0", "Sigma", "Mean", "Constant","A");
 fitting->SetParameters(0, h1->GetStdDev(), h1->GetMean(), 10000,1);*/

/*void log_normal(){//Double_lognormal
 TFile *f = new TFile("templates-single.root");
 TH1F *hpx = (TH1F*)f->Get("ljets__zjets");
 int xmin = hpx->GetXaxis()->GetXmin(); 
 int xmax = hpx->GetXaxis()->GetXmax();

 TF1 *fitting = new TF1("fitting", double_lognormal_pdf, xmin, xmax, 8);
 fitting->SetParNames("X0", "Sigma", "Mean", "Constant", "X1","Sigma1", "Mean1", "Constant1");
    
  Int_t nbins = hpx->GetNbinsX();
  //finding stddev and mean for log(x) histogram
  TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
  for (int i = 0; i < nbins+2; i++){
   int yaxis = hpx->GetBinContent(i);
   double xaxis = std::log(xmin+i*(xmax-xmin)/nbins);
   h1->Fill(xaxis,yaxis);
  }

 fitting->SetParameters(0, h1->GetStdDev(), h1->GetMean(), 10000,1,h1->GetStdDev(), h1->GetMean(), 10000,1);*/

/* void log_normal(){//lognormal+gaus
 TFile *f = new TFile("templates-single.root");
 TH1F *hpx = (TH1F*)f->Get("ljets__zjets");
 int xmin = hpx->GetXaxis()->GetXmin(); 
 int xmax = hpx->GetXaxis()->GetXmax();
 
 double cut = 1400;
 TF1 *fitting1 = new TF1("fitting1", "gaus", xmin, cut);
 //fitting1->SetParNames("X0", "Sigma", "Mean", "Constant", "B","A");//,"RM");
 TF1 *fitting2 = new TF1("fitting2", "lognormal_pdf", cut,xmax,5);
 TF1 *fitting = new TF1("fitting", "gaus(0)+[6]*exp(-([7]/2*(log(x-[3])-[5])/[4])**2)/((x-[3])*abs([4])*sqrt(2*3.14))",500,2950);
    
  Int_t nbins = hpx->GetNbinsX();
  //finding stddev and mean for log(x) histogram
  TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
  for (int i = 0; i < nbins+2; i++){
   int yaxis = hpx->GetBinContent(i);
   double xaxis = std::log(xmin+i*(xmax-xmin)/nbins);
   h1->Fill(xaxis,yaxis);
  }

 //fitting1->SetParameters(0, h1->GetStdDev(), h1->GetMean(), 10000,1,1);
 fitting2->SetParameters(0, h1->GetStdDev(), h1->GetMean(), 10000,1,1);//,hpx->GetMean());
 hpx->Fit(fitting1,"R");
 hpx->Fit(fitting2,"R+");
 
 double par[8];
 fitting1->GetParameters(&par[0]); 
 fitting2->GetParameters(&par[3]);
 fitting->SetParameters(par);
 fitting->SetLineColor(4);*/
 
void log_normal(){//gaus+expo tail
 //TFile *f = new TFile("templates-single.root");
 //TH1F *hpx = (TH1F*)f->Get("ljets__stop");
 TFile *f = new TFile("templates_All.root");
 TH1F *hpx = (TH1F*)f->Get("All__stop");
 int xmin = hpx->GetXaxis()->GetXmin(); 
 int xmax = hpx->GetXaxis()->GetXmax();
 Int_t nbins = hpx->GetNbinsX();
 double cut1 = xmin + hpx->GetMaximumBin()*(xmax-xmin)/nbins, cut2 = 1500;
 TF1 *fitting1 = new TF1("fitting1", "gaus", xmin, cut1);
 TF1 *fitting2 = new TF1("fitting2", "gaus", cut1, cut2);
 //fitting1->SetParNames("X0", "Sigma", "Mean", "Constant", "B","A");//,"RM");
 TF1 *fitting3 = new TF1("fitting3", "[0]*std::exp(-[1]*std::pow(x,[2]))", cut2,xmax-50);
 TF1 *fitting = new TF1("fitting", "gaus(0)+gaus(3)+[6]*std::exp(-[7]*std::pow(x,[8]))",500,2950);

 //fitting1->SetParameters(0, h1->GetStdDev(), h1->GetMean(), 10000,1,1);
 fitting3->SetParameters(3, 0.0005,1);//,hpx->GetMean());
 hpx->Fit(fitting1, "R");
 hpx->Fit(fitting2, "R+");
 hpx->Fit(fitting3, "R+");
 
 double par[9];
 fitting1->GetParameters(&par[0]); 
 fitting2->GetParameters(&par[3]);
 fitting3->GetParameters(&par[6]);
 fitting->SetParameters(par);
 fitting->SetLineColor(4);
 //h1->Draw();

/*void log_normal(){//landau,gaus
 TFile *f = new TFile("templates_All.root");
 TH1F *hpx = (TH1F*)f->Get("All__stop");
 int xmin = hpx->GetXaxis()->GetXmin(); 
 int xmax = hpx->GetXaxis()->GetXmax();
 
 double cut = 2950;
 TF1 *g2 = new TF1("m2", "gaus", 1300, cut);
 TF1 *g1 = new TF1("m1", "landau", 500, 1300);
 TF1 *fitting = new TF1("fitting", "landau(0)*gaus(3)", xmin, cut);//for stop
 //TF1 *fitting = new TF1("fitting", "landau(0)-gaus(3)", xmin, cut);//for top

 //fitting->SetLineColor(4);
    Double_t par[6];
    hpx->Fit(g1,"R");
    hpx->Fit(g2,"R+");
    g1->GetParameters(&par[0]);
    g2->GetParameters(&par[3]); 
	  fitting->SetParameters(par);  
	  fitting->SetLineColor(4);*/
	   
 hpx->Fit(fitting,"R+");
 cout<<cut1<<endl;
}
