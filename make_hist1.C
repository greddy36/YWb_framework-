#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include <string>
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
double double_lognormal_pdf(double *var, double *par){  
  double x = var[0], x0 = par[0], sigma = par[1], mean = par[2], constant = par[3], x1 = par[4], sigma1 = par[5], mean1 = par[6], constant1 = par[7];

  if ((x-x0) <= 0)
    return 0.0;
  else if ((x-x0) > 0 && (x-x0) < mean){
    double tmp = (std::log(x-x0) - mean)/sigma;
    return constant * std::exp(-tmp *tmp/2)/((x-x0)* std::fabs(sigma) * std::sqrt(2 * M_PI));//40000-top, 100000-wjet, 51000-stop, 1129-zjets
  }
  else {
    double tmp = (std::log(x-x0) - mean1)/sigma1;
    return constant1 * std::exp(-tmp *tmp/2)/((x-x0)* std::fabs(sigma1) * std::sqrt(2 * M_PI));//40000-top, 100000-wjet, 51000-stop, 1129-zjets
  }
}
//--------------------------------------------------------------------------
double new_lognormal_pdf(double *var, double *par){  
  double x = var[0], x0 = par[0], sigma = par[1], mean = par[2], constant = par[3], a =par[4], b=par[5];
  if ((x-x0) <= 0)
    return 0.0;
  else {
    double tmp = (std::log(x-x0) - mean)/sigma;
    return constant * std::exp(std::abs(tmp)*a/2)/(std::pow((x-x0),b)*std::fabs(sigma) * std::sqrt(2 * M_PI));//40000-top, 100000-wjet, 51000-stop, 1129-zjets
  }

}
//S = S0+S1* ln(x) +S2*x+S3*x^2 use this sometime
//-----------------------------------------------------------------------------
/*
void log_normal(){
 TFile *f = new TFile("templates-single.root");
 TH1F *hpx = (TH1F*)f->Get("ljets__zjets");
 int xmin = hpx->GetXaxis()->GetXmin(); 
 int xmax = hpx->GetXaxis()->GetXmax();

 TF1 *fitFn = new TF1("fitFn", lognormal_pdf, xmin, xmax, 5);
 fitFn->SetParNames("X0", "Sigma", "Mean", "Constant", "alpha");
    
  Int_t nbins = hpx->GetNbinsX();
  //finding stddev and mean for log(x) histogram
  TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
  for (int i = 0; i < nbins+2; i++){
   int yaxis = hpx->GetBinContent(i);
   double xaxis = std::log(xmin+i*(xmax-xmin)/nbins);
   h1->Fill(xaxis,yaxis);
  }

 fitFn->SetParameters(0, h1->GetStdDev(), h1->GetMean(), 10000,1);*/

/*void log_normal(){//Double_lognormal
 TFile *f = new TFile("templates-single.root");
 TH1F *hpx = (TH1F*)f->Get("ljets__zjets");
 int xmin = hpx->GetXaxis()->GetXmin(); 
 int xmax = hpx->GetXaxis()->GetXmax();

 TF1 *fitFn = new TF1("fitFn", double_lognormal_pdf, xmin, xmax, 8);
 fitFn->SetParNames("X0", "Sigma", "Mean", "Constant", "X1","Sigma1", "Mean1", "Constant1");
    
  Int_t nbins = hpx->GetNbinsX();
  //finding stddev and mean for log(x) histogram
  TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
  for (int i = 0; i < nbins+2; i++){
   int yaxis = hpx->GetBinContent(i);
   double xaxis = std::log(xmin+i*(xmax-xmin)/nbins);
   h1->Fill(xaxis,yaxis);
  }

 fitFn->SetParameters(0, h1->GetStdDev(), h1->GetMean(), 10000,1,h1->GetStdDev(), h1->GetMean(), 10000,1);*/

/* void log_normal(){//lognormal+gaus
 TFile *f = new TFile("templates-single.root");
 TH1F *hpx = (TH1F*)f->Get("ljets__zjets");
 int xmin = hpx->GetXaxis()->GetXmin(); 
 int xmax = hpx->GetXaxis()->GetXmax();
 
 double cut = 1400;
 TF1 *fitFn1 = new TF1("fitFn1", "gaus", xmin, cut);
 //fitFn1->SetParNames("X0", "Sigma", "Mean", "Constant", "B","A");//,"RM");
 TF1 *fitFn2 = new TF1("fitFn2", "lognormal_pdf", cut,xmax,5);
 TF1 *fitFn = new TF1("fitFn", "gaus(0)+[6]*exp(-([7]/2*(log(x-[3])-[5])/[4])**2)/((x-[3])*abs([4])*sqrt(2*3.14))",500,3000);
    
  Int_t nbins = hpx->GetNbinsX();
  //finding stddev and mean for log(x) histogram
  TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
  for (int i = 0; i < nbins+2; i++){
   int yaxis = hpx->GetBinContent(i);
   double xaxis = std::log(xmin+i*(xmax-xmin)/nbins);
   h1->Fill(xaxis,yaxis);
  }

 //fitFn1->SetParameters(0, h1->GetStdDev(), h1->GetMean(), 10000,1,1);
 fitFn2->SetParameters(0, h1->GetStdDev(), h1->GetMean(), 10000,1,1);//,hpx->GetMean());
 hpx->Fit(fitFn1,"R");
 hpx->Fit(fitFn2,"R+");
 
 double par[8];
 fitFn1->GetParameters(&par[0]); 
 fitFn2->GetParameters(&par[3]);
 fitFn->SetParameters(par);
 fitFn->SetLineColor(4);*/
 
 //h1->Draw();

void make_hist1(){//lognormal
 TFile *ifile = new TFile("templates_All.root");
 TFile *ofile = new TFile("analytic_fn.root", "RECREATE");
 TIter nextkey(ifile->GetListOfKeys());
 TKey *key;
 while ((key = (TKey*)nextkey())){
   TObject *obj = key->ReadObj();
   TH1F *hpx = (TH1F*)(obj);
   const char *name = hpx->GetName();
   std::string s = name;
	 if (s.find("wjets") < s.length() || s.find("stop") < s.length() || s.find("top") < s.length()){
	   cout<<name<<endl;
		 //TH1F *hpx = (TH1F*)ifile->Get("All__stop__scalestop__plus"); 
		 int xmin = hpx->GetXaxis()->GetXmin(); 
		 int xmax = hpx->GetXaxis()->GetXmax();
		 
		 double cut = 2950;
		 TF1 *fitFn = new TF1("fitFn", "lognormal_pdf", xmin, cut,5);
				
			Int_t nbins = hpx->GetNbinsX();
			//finding stddev and mean for log(x) histogram
			TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
			for (int i = 0; i < nbins+2; i++){
			 int yaxis = hpx->GetBinContent(i);
			 double xaxis = std::log(xmin+i*(xmax-xmin)/nbins);
			 h1->Fill(xaxis,yaxis);
			}
		 fitFn->SetParNames("X0", "Sigma", "Mean", "Constant","A");
		 fitFn->SetParameters(0, h1->GetStdDev(), h1->GetMean(), 10000,1); 
			
		 fitFn->SetLineColor(4);
		 hpx->Fit(fitFn,"R+");  
		 //fitFn->Write();
		 
		 TH1F *fit_hist = new TH1F(name, name, nbins, xmin, cut); 
		 for (int i = 0; i < nbins+1; i++){
			int xval = xmin+i*(xmax-xmin)/nbins;
			fit_hist->SetBinContent(i+1,fitFn->Eval(xval)); 
		 }
		 fit_hist->Write();
	 }//if
 }//while	 
 		
/*	 TF1 *fitFn_minus = new TF1("fitFn_minus", "lognormal_pdf", xmin, cut,5);
	 fitFn_minus->SetParameter(0,fitFn->GetParameter(0) - fitFn->GetParError(0));
	 fitFn_minus->SetParameter(1,fitFn->GetParameter(1) - fitFn->GetParError(1));
	 fitFn_minus->SetParameter(2,fitFn->GetParameter(2) - fitFn->GetParError(2));
	 fitFn_minus->SetParameter(3,fitFn->GetParameter(3) - fitFn->GetParError(3));
	 fitFn_minus->SetParameter(4,fitFn->GetParameter(4) - fitFn->GetParError(4));
	 fitFn_minus->Write();
	 
	 TH1F *fit_hist_minus = new TH1F("fit_hist_minus", "test hist 2", nbins, xmin, cut); 
	 for (int i = 0; i < nbins+1; i++){
		int xval = xmin+i*(xmax-xmin)/nbins;
		fit_hist_minus->SetBinContent(i+1,fitFn_minus->Eval(xval)); 
	 }
	 fit_hist_minus->Write();*/
	/* TH1F *fit_hist1 = new TH1F("fit_hist1", "test hist 1", hpx->GetNbinsX(), xmin, cut); 
	 fit_hist1->FillRandom("fitFn",TMath::Nint(hpx->Integral())); 
	 fit_hist1->Draw("same");*/
 ofile->Close();
}
