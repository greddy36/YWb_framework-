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
//-----------------------------------------------------------------------
void make_hist2(){//lognormal
 TCanvas *can= new TCanvas("can","can",700,500);
 TFile *ifile = new TFile("templates_All.root");
 TFile *ifile1 = new TFile("templates-Theta.root");
 TFile *ofile = new TFile("sys_fn_All.root", "RECREATE");
 TIter nextkey(ifile->GetListOfKeys());
 TKey *key;
/* //--------------------------------------------------------
 TIter nextkey0(ifile1->GetListOfKeys());
 TKey *key0;
 while ((key0 = (TKey*)nextkey0())){
	 TObject *obj0 = key0->ReadObj();
	 TH1F *hpx0 = (TH1F*)(obj0);
	 const char *name0 = hpx0->GetName();
	 std::string s0 = name0;
	 if (s0.find("signal") < s0.length()){//clone signal histograms
	   TH1F *clone0 = (TH1F*) hpx0->Clone();
	   hpx0->Write();
	 }
 }*/
//-----------------------------------------------------------
 while ((key = (TKey*)nextkey())){
   TObject *obj = key->ReadObj();
   TH1F *hpx = (TH1F*)(obj);
   const char *name = hpx->GetName();
   std::string s = name;
   
  /* if (s.find("signal") < s.length()){//clone signal histograms
	   TH1F *clone = (TH1F*) hpx->Clone();
	   hpx->Write();
	 }*/
	 if (s.find("zjets") < s.length() || s.find("qcd") < s.length() || s.find("signal") < s.length() || s.find("data") < s.length())
	   continue;//only, wjets, stop and top pass
	 cout<<name<<endl;
	 
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
	 //fitFn->SetParNames("X0", "Sigma", "Mean", "Constant","A");
	 fitFn->SetParameters(0, h1->GetStdDev(), h1->GetMean(), 10000,1); 
			
	 fitFn->SetLineColor(4);
	 hpx->Fit(fitFn,"R+");  
	 
/*	 if (s.find("plus") < s.length() || s.find("minus") < s.length())
	 	 continue;
	 TF1 *fitFn_plus = new TF1("fitFn_plus", "lognormal_pdf", xmin, cut,5);
	 TF1 *fitFn_minus = new TF1("fitFn_minus", "lognormal_pdf", xmin, cut,5);
	 for (int i = 0; i < 5; i++){
		 	std::string s_tmp_plus = name, s_tmp_minus = name;
		 	char* name_plus = const_cast<char*>(s_tmp_plus.append("__").append(fitFn->GetParName(i)).append("__plus").c_str());
		 	char* name_minus = const_cast<char*>(s_tmp_minus.append("__").append(fitFn->GetParName(i)).append("__minus").c_str());	 
			TH1F *fit_hist_plus = new TH1F(name_plus, name_plus, nbins, xmin, cut);
			TH1F *fit_hist_minus = new TH1F(name_minus, name_minus, nbins, xmin, cut); 

		 	fitFn_plus->SetParameters(fitFn->GetParameters());
			fitFn_minus->SetParameters(fitFn->GetParameters());		 
			fitFn_plus->SetParameter(i,fitFn->GetParameter(i) + fitFn->GetParError(i));
		 	fitFn_minus->SetParameter(i,fitFn->GetParameter(i) - fitFn->GetParError(i));
			for (int j = 0; j < nbins+1; j++){
			  int xval = xmin+j*(xmax-xmin)/nbins;
				fit_hist_minus->SetBinContent(j+1,fitFn_minus->Eval(xval)); 
				fit_hist_plus->SetBinContent(j+1,fitFn_plus->Eval(xval));
			}
			fit_hist_plus->Write();
			fit_hist_minus->Write();
	 }*/
	 
	 s.append(".pdf");
			char* title = const_cast<char*>(s.c_str());//converting string to char
			fitFn->SetTitle(title);
			//fitFn->Draw();//this will plot only the fn but not the histograms
			can->SaveAs(title);
//------------------------------------------------------------------------------	 
/*	 TIter nextkey1(ifile1->GetListOfKeys());
	 TKey *key1;
	 while ((key1 = (TKey*)nextkey1())){
		 TObject *obj1 = key1->ReadObj();
		 TH1F *hpx1 = (TH1F*)(obj1);
		 const char *name1 = hpx1->GetName();
		 std::string s1 = name1;
		 if (s1.compare(5,s1.length()-5, s,3,s.length()-3)==0){//checking names are same after first "__"
			 TF1 *fitFn1 = new TF1("fitFn1", "lognormal_pdf", xmin, cut,5);
			 fitFn1->FixParameter(0,fitFn->GetParameter(0));
			 fitFn1->FixParameter(1,fitFn->GetParameter(1));
			 fitFn1->FixParameter(2,fitFn->GetParameter(2));
			 fitFn1->FixParameter(4,fitFn->GetParameter(4));
			 fitFn1->SetParName(3,"Constant");
			 hpx1->Fit(fitFn1,"R+");
		   //fitFn1->Write();
		   //hpx1->Write();
			 TH1F *fit_hist1 = new TH1F(name1, name1, nbins, xmin, cut); 
			 for (int i = 0; i < nbins+1; i++){
				int xval = xmin+i*(xmax-xmin)/nbins;
				fit_hist1->SetBinContent(i+1,fitFn1->Eval(xval)); 
			 }
			 fit_hist1->Write();*/
			 			 
			/* s1.append(".pdf");
			 char* title1 = const_cast<char*>(s1.c_str());//converting string to char
			 fitFn1->SetTitle(title1);
			 //fitFn1->Draw();//this will plot only the fn but not the histograms
			 can->SaveAs(title1);*/
		 //}
	 //}
//---------------------------------------------------------------------------
 
/*	 TH1F *fit_hist = new TH1F(name, name, nbins, xmin, cut); 
	 for (int i = 0; i < nbins+1; i++){
		int xval = xmin+i*(xmax-xmin)/nbins;
		fit_hist->SetBinContent(i+1,fitFn->Eval(xval)); 
	 }
	 fit_hist->Write();*/
 }//while	 
 ofile->Close();
 return 0;
}
