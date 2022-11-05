#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
//------------------------------------------------------------------------
double Gaus_expo(double *var, double *par){  
  //double x = var[0], Ngaus = par[0], mean = par[1], sigma1 = par[2], Ngaus2 = par[3], sigma2 = par[4], t = par[5], Nexp = par[6], exp_slope = par[7], exp_pow = par[8];
  double x = var[0], Ngaus = par[0], mean = par[1], sigma1 = par[2], sigma2 = par[3], t = par[4], n = par[5];
		if (x < mean){
		  double tmp = (x-mean)/sigma1;
			return Ngaus*exp(-tmp*tmp/2);
		}
		else if (x >= mean && x < t){
			double tmp = (x-mean)/sigma2;
		  return Ngaus*exp(-tmp*tmp/2);
		}
	/*  else if (x >= t){
		  double tmp = (t-mean)/sigma2;
		  double Nexp = Ngaus*exp(tmp*tmp*(1/n-1/2));
		  double s = tmp*tmp*pow((t-mean),-n)/n;
		  return Nexp*exp(-s*pow(x-mean,n));
		}*/
	 else if (x >= t){
		  double tmp = abs(t-mean)/(sigma2*sigma2);
		  double Nexp = Ngaus*exp(tmp*(t/n-abs(t-mean)/2));
		  double s = tmp*pow((t),1-n)/n;
		  return Nexp*exp(-s*pow(x,n));
		}
		else return 10;	
}

//try Ngaus*exp{-slope * (ln(x/mean))^n - (ln x/mean)^2/2*sigma^2)}
double test_new(double *var, double *par){  
	double x = var[0], Ngaus = par[0], mean = par[1], sigma = par[2], sigma2 = par[3], slope= par[4], n = par[5];
		if (x < mean){
		  double tmp = (x-mean)/sigma;
			return Ngaus*exp(-tmp*tmp/2);
		}
  else{
	double tmp = std::log(x/mean)/sigma2;
	return Ngaus*exp(-slope*pow(tmp*sigma2,n)-tmp*tmp/2);
	}
}

double lognormal_pdf(double *var, double *par){  
  double x = var[0], x0 = par[0], sigma = par[1], mean = par[2], constant = par[3], a =par[4];
  if ((x-x0) <= 0)
    return 0.0;
  else{
    double tmp = (std::log(x-x0) - mean)/sigma;
    return constant * std::exp(-tmp *tmp*a/2)/((x-x0)*std::fabs(sigma) * std::sqrt(2 * M_PI));//40000-top, 100000-wjet, 51000-stop, 1129-zjets
  }
}


//--------------------------------------------------------------------------

void gaus_expo(){//2gaus+expo tail
 TCanvas *can= new TCanvas("can","can",700,500);
 TFile *ifile = new TFile("templates-3chan-Theta_rebin.root");
 TFile *ifile1 = new TFile("templates-6chan-Theta_rebin.root");
 TFile *ofile = new TFile("analytic_hist_6chan_rebin.root", "RECREATE");
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
	 if (s.find("zjets") < s.length() || s.find("qcd") < s.length() || s.find("signal") < s.length() || s.find("data") < s.length() || s.find("DATA") < s.length()|| s.find("minus") < s.length()|| s.find("plus") < s.length())
	   continue;
	 cout<<name<<endl;
	 //if (s.find("b2M__stop") > s.length()) continue;
	 
	 int xmin = hpx->GetXaxis()->GetXmin(); 
	 int xmax = hpx->GetXaxis()->GetXmax();
	 Int_t nbins = hpx->GetNbinsX();


	 double t;
	 if (s.find("b2M") < s.length())
	 	 t = 1350; 
	 else if (s.find("b1T") < s.length())
	   t = 1450;
	 else t = 1450;
	 int param_num = 6;
	 double cut1 = xmin + hpx->GetMaximumBin()*(xmax-xmin)/nbins-100;
	 double cut2 = t, mean_=9999, tee = t; 
	 
	// while(tee-mean_ < 100 || tee-mean_ > 1000){cut2 = cut2-10; cout<<cut2<<endl; TCanvas *can= new TCanvas("can","can",700,500);
			 double par[9];
			 TF1 *fitting1 = new TF1("fitting1", "gaus", xmin, cut1); 
			 fitting1->FixParameter(1,cut1-50);fitting1->SetParameter(0,9);fitting1->SetParameter(2,500);
			 hpx->Fit(fitting1, "BR");	 
			 fitting1->GetParameters(&par[0]); 
			 
			 TF1 *fitting2 = new TF1("fitting2", "gaus", cut1, cut2);
			 fitting2->FixParameter(0,par[0]); fitting2->SetParameter(2,800);
			 fitting2->FixParameter(1,par[1]);
			 hpx->Fit(fitting2, "BR+");
			 
			 TF1 *fitting3 = new TF1("fitting3", "[0]*std::exp(-[1]*std::pow(x,[2]))", cut2,xmax-50);
			 fitting3->SetParameters(3, 0.0005,1);//,hpx->GetMean());
			 hpx->Fit(fitting3, "BR+");
			 fitting2->GetParameters(&par[3]);
			 fitting3->GetParameters(&par[6]);

			 
			 TF1 *fitFn = new TF1("fitFn", "Gaus_expo",xmin,xmax,param_num);
			 fitFn->SetParNames("Ngaus","Mean","sigma1","sigma2","t","exp_pow");
			 fitFn->SetParameters(par[0], par[1], par[2],par[5],cut2,par[8]);
			 fitFn->SetLineColor(1); 			 
			 hpx->Fit(fitFn,"BR");

			 TF1 *fitFn_plus[param_num];
			 TF1 *fitFn_minus[param_num];
			 for(int i = 0; i < param_num; i++){
				  fitFn_plus[i] = new TF1("fitFn_plus", "Gaus_expo", xmin,xmax,param_num);
					fitFn_minus[i] = new TF1("fitFn_minus", "Gaus_expo", xmin,xmax,param_num);
					fitFn_plus[i]->SetParameters(fitFn->GetParameters());
				  fitFn_minus[i]->SetParameters(fitFn->GetParameters());
					fitFn_plus[i]->SetParameter(i,fitFn->GetParameter(i) + fitFn->GetParError(i));
					fitFn_minus[i]->SetParameter(i,fitFn->GetParameter(i) - fitFn->GetParError(i));
					fitFn_plus[i]->SetLineColor(2);fitFn_plus[i]->Draw("same");
			 		fitFn_minus[i]->SetLineColor(4);fitFn_minus[i]->Draw("same");
			 		
			 		if (s.find("plus") < s.length() || s.find("minus") < s.length())//only make param+- on nominal hist
			 			continue;
			 		std::string name_str_plus = name, name_str_minus = name;
			 		char* name_plus = const_cast<char*>(name_str_plus.append("__").append(fitFn->GetParName(i)).append("__plus").c_str());
		 	    char* name_minus = const_cast<char*>(name_str_minus.append("__").append(fitFn->GetParName(i)).append("__minus").c_str());	 
			 		TH1F *fit_hist_plus = new TH1F(name_plus, name_plus, nbins, xmin, xmax); 
			 		TH1F *fit_hist_minus = new TH1F(name_minus, name_minus, nbins, xmin, xmax); 
					for (int k = 0; k < nbins+1; k++){
						int xval = xmin+k*(xmax-xmin)/nbins;
						fit_hist_plus->SetBinContent(k+1,fitFn_plus[i]->Eval(xval)); 
					  fit_hist_minus->SetBinContent(k+1,fitFn_minus[i]->Eval(xval)); 
					}
					fit_hist_plus->Write(); fit_hist_minus->Write();
			 }

			 std::string name_str = name;
			 name_str.append(".png");
			 char* title = const_cast<char*>(name_str.c_str());//converting string to char
			 //fitFn->SetTitle(title);
			 can->SaveAs(title);
			 
			// tee = fitFn->GetParameter(4); mean_ = fitFn->GetParameter(1);}
	 TIter nextkey1(ifile1->GetListOfKeys());
	 TKey *key1;
	 while ((key1 = (TKey*)nextkey1())){//while2
		 TObject *obj1 = key1->ReadObj();
		 TH1F *hpx1 = (TH1F*)(obj1);
		 const char *name1 = hpx1->GetName();
		 std::string s1 = name1;
		 if (s1.compare(0,3, s,0,3)==0 && s1.compare(5,s1.length()-5, s,3,s.length()-3)==0){//checking channels are similar
			 TF1 *fitFn1 = new TF1("fitFn1", "Gaus_expo", xmin, xmax,param_num);
			 fitFn1->FixParameter(1,fitFn->GetParameter(1));
			 fitFn1->FixParameter(2,fitFn->GetParameter(2));
			 fitFn1->FixParameter(3,fitFn->GetParameter(3));
			 fitFn1->FixParameter(4,fitFn->GetParameter(4));
			 fitFn1->FixParameter(5,fitFn->GetParameter(5));
			 fitFn1->SetParName(0,"Ngaus");
			 hpx1->Fit(fitFn1,"R+");
		   //fitFn1->Write();
		   //hpx1->Write();
			 TH1F *fit_hist1 = new TH1F(name1, name1, nbins, xmin, xmax); 
			 for (int i = 0; i < nbins+1; i++){
				int xval = xmin+i*(xmax-xmin)/nbins;
				fit_hist1->SetBinContent(i+1,fitFn1->Eval(xval)); 
			 }
			 fit_hist1->Write();
		 }	
	 }//while2
 }//while	 
 ofile->Close();
 return 0;
}
