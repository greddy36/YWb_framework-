#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
//------------------------------------------------------------------------
double cb_fitfn(double *var, double *par) {
  double x = var[0], c = par[0], mean = par[1], sigma = par[2], alpha_high = abs(par[3]), alpha_low = abs(par[4]), n_high = par[5], n_low = par[6];

  double z = (x - mean)/sigma; 
  if (z >= -alpha_low && z <= alpha_high)
    return c*std::exp(- 0.5 * z * z);
  
  else if (z < -alpha_low){
    double nDivAlpha = n_low/alpha_low;
    double AA =  c*std::exp(-0.5*alpha_low*alpha_low);
    double B = nDivAlpha - alpha_low;
    double arg = nDivAlpha/(B-z);
    return AA * std::pow(arg,n_low);
  }
  else if (z > alpha_high){
    double nDivAlpha = n_high/alpha_high;
    double AA =  c*std::exp(-0.5*alpha_high*alpha_high);
    double B = nDivAlpha - alpha_high;
    double arg = nDivAlpha/(B+z);
    return AA * std::pow(arg,n_high);
  } 
  else
    return 0;

}

double lognormal_pdf(double *var, double *par){  
  double x = var[0], x0 = par[0], sigma = par[1], mean= par[2], constant = par[3], a =par[4];
  if ((x-x0) <= 0)
    return 0.0;
  else{
    double tmp = (std::log(x-x0) - mean)/sigma;
    return constant * std::exp(-tmp *tmp*a/2)/((x-x0)*std::fabs(sigma) * std::sqrt(2 * M_PI));//40000-top, 100000-wjet, 51000-stop, 1129-zjets
  }
}

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

double test_new(double *var, double *par){  
  double x = var[0], Ngaus = par[0], mean = par[1], sigma = par[2], sigma2 = par[3],slope= par[4], n = par[5], N = abs(par[6]), mean3 = abs(par[7]), sigma3 = par[8];
  if (x <= mean){
	double tmp = (x-mean)/sigma, tmp2 = (x-mean3)/sigma3;
	return Ngaus*exp(-tmp*tmp/2)- N*exp(-tmp2*tmp2/2);
  }
  else{
	double tmp = std::log(x/mean)/sigma2, tmp2 = (x-mean3)/sigma3; 
	return Ngaus*exp(-exp(slope)*pow(tmp*sigma2,1+exp(n))-tmp*tmp/2) - N*exp(-tmp2*tmp2/2);
  }
}

/*double test_new(double *var, double *par){  
	double x = var[0], Ngaus = par[0], mean = par[1], sigma = par[2], sigma2 = par[3], lambda= par[4], a = par[5];
	if (x <= mean){
		  double tmp = (x-mean)/sigma;
			return Ngaus*exp(-tmp*tmp/2);
		}
  else{
	return Ngaus*exp(lambda*(log((1-a*x)/(1-a*mean))+a*mean/(1-a*mean)*log(x/mean))-0.5*pow(log(x/mean)/sigma2,2));
	}
}*/

/*double test_new(double *var, double *par){  
	double x = var[0], Ngaus = par[0], mean = par[1], sigma = par[2], sigma2 = par[3], slope= par[4], a= par[5];
	if (x <= mean){
		  double tmp = (x-mean)/sigma;
			return Ngaus*exp(-tmp*tmp/2);
		}
  else{
	double tmp = std::log(x/mean)/sigma2;
	return Ngaus*exp(-abs(slope)*tmp*tmp/2 - abs(a)*tmp*tmp*tmp/6);
	}
} */
//--------------------------------------------------------------------------

void ftest_wjet(){
 TCanvas *can= new TCanvas("can","can",700,500);
 TFile *ifile0 = new TFile("templates-3chan-Theta-noFwd-Stop.root");
 TFile *ifile = new TFile("templates-3chan-HC_wqcdy.root");
 TFile *ifile1 = new TFile("templates-6chan-Theta_rebin_wqcdy.root");  //TFile *ifile2 = new TFile("templates-6chan-Theta_qcdy.root");  
 TFile *ofile = new TFile("analytic_hist_6chan_noFwdStop_25bins_wqcdy.root", "RECREATE");
 ofstream tablefile;
 tablefile.open ("table_ftest.txt");
 tablefile<<"HistName\t"<<"ChiSquare\t"<<"NDF\t"<<"RSS"<<endl;
 int param_num = 9;
 int bin_scale = 1; 
 //--------------------------------------------------------
 TIter nextkey(ifile->GetListOfKeys());
 TKey *key;
 while ((key = (TKey*)nextkey())){
   TObject *obj = key->ReadObj();
   TH1F *hpx = (TH1F*)(obj);
   const char *name = hpx->GetName();
   std::string s = name;
   hpx->Rebin(2);
	 int xmin = hpx->GetXaxis()->GetXmin(); 
	 int xmax = hpx->GetXaxis()->GetXmax();
	 Int_t nbins = hpx->GetNbinsX();
	 int binw = (xmax-xmin)/nbins;
	   
	/* if (s.find("signal") < s.length()){  cout<<name<<endl; 
		 TF1 *crystalball = new TF1("cb", cb_fitfn, xmin, xmax, 7);
		 crystalball->SetParNames("Constant", "Mean", "Sigma", "Alpha_high", "Alpha_low", "N_high", "N_low");
		 crystalball->SetParameters(500, hpx->GetMean(), hpx->GetRMS(), .5, .5, 1,1);
		 hpx->Fit(crystalball,"R");
		 std::string name_str = name;
		 name_str.append(".png");
		 char* title = const_cast<char*>(name_str.c_str());//converting string to char
		 //fitFn->SetTitle(title);
		 can->SaveAs(title);
	 }*/
   
	 //if (s.find("zjets") < s.length() || s.find("qcd") < s.length() || s.find("signal") < s.length()|| s.find("data") < s.length() || s.find("DATA") < s.length() || s.find("stop") < s.length()) continue;//|| s.find("minus") < s.length()|| s.find("plus") < s.length())
	 if (s.find("wjets__b1M") > s.length())  continue;
	 cout<<name<<endl;
	 
  // if(s.find("wjets__pdf") > s.length()) continue;	 
  
//-----------------------------------------------------
	 TF1 *fitFn = new TF1("fit_new","test_new", xmin+binw, xmax-binw, param_num);		  
/*		//finding stddev and mean for log(x) histogram
		TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
		for (int i = 0; i < nbins+2; i++){
		 int yaxis = hpx->GetBinContent(i);
		 double xaxis = std::log(xmin+i*binw);
		 h1->Fill(xaxis,yaxis);
		}*/
	 fitFn->SetParNames("Ngaus","Mean","sigma","sigma2","slope","n","N","mean3","sigma3");
	 if(s.find("b1M") < s.length()){
	 	 //fitFn->SetParameters(86, 1200, 220, 0.3, -8,0.117,18,1153,82);
	 	 fitFn->SetParameters(86, 1000, 150, 0.4, 0.5,0.117,25,1070,125);
	 }
	 if(s.find("b1T") < s.length()){
	 	 fitFn->SetParameters(21, 1300, 230, 0.26, -1.2,-0.2,8,1350,50);
	 }
	 if(s.find("b2M") < s.length()){
	 	 fitFn->SetParameters(13, 1100, 170, 1.3, 1.5,0.15,8,1150,50);
	 }	 
	 //fitFn->SetParameter(6,1);//fitFn->SetParameter(7,fitFn->GetParameter(4));fitFn->SetParameter(8,fitFn->GetParameter(5));//fitFn->SetParameter(9,1);
	 
	 //fitFn->SetParNames("Ngaus","Mean","sigma","sigma2","lambda","a"); fitFn->SetParameters(10, 1000, 300, h1->GetStdDev(), 2,1/sqrt(1300));
   //fitFn->SetParNames("Ngaus","Mean","sigma","sigma2"); fitFn->SetParameters(42, 1100, 170,0.3);//,h1->GetStdDev());
   fitFn->SetLineColorAlpha(kBlack,0);fitFn->SetLineWidth(3);
	 hpx->Fit(fitFn,"R");gStyle->SetOptFit(1111);
	 
	 TH1F *fit_hist = new TH1F(name, name, nbins, xmin, xmax); 
         fit_hist->SetBinContent(1,hpx->GetBinContent(1));
	 fit_hist->SetBinContent(nbins,hpx->GetBinContent(nbins));
	 float RSS = 0;
	 for (int j = 2; j < nbins; j++){
			int xval = xmin+(j-.5)*binw;
			fit_hist->SetBinContent(j,fitFn->Eval(xval)); 
		        RSS += pow(fitFn->Eval(xval) - hpx->GetBinContent(j),2);
	 }
	 //fit_hist->Draw("same");
   tablefile<<name<<"\t"<<fitFn->GetChisquare()<<"\t"<<fitFn->GetNDF()<<"\t"<<RSS<<endl;
   /*for(int i = 0; i < param_num; i++){
   	  tablefile<< fitFn->GetParameter(i) + fitFn->GetParError(i)<<"\t";
   }
   tablefile<<endl;*/
//----------------------------------------------------------------------  
	 TF1 *fitFn_plus[param_num];
	 TF1 *fitFn_minus[param_num];
	 for(int i = 0; i < param_num; i++){
		  fitFn_plus[i] = new TF1("fitFn_plus", "test_new", xmin,xmax,param_num);
			fitFn_minus[i] = new TF1("fitFn_minus", "test_new", xmin,xmax,param_num);
			fitFn_plus[i]->SetParameters(fitFn->GetParameters());
			fitFn_minus[i]->SetParameters(fitFn->GetParameters());
			fitFn_plus[i]->SetParameter(i,fitFn->GetParameter(i) + fitFn->GetParError(i));
			fitFn_minus[i]->SetParameter(i,fitFn->GetParameter(i) - fitFn->GetParError(i));	
			//fitFn_plus[i]->SetLineStyle(2);fitFn_plus[i]->SetLineColor(i+2);fitFn_plus[i]->Draw("same");
			//fitFn_minus[i]->SetLineStyle(3);fitFn_minus[i]->SetLineColor(i+2);fitFn_minus[i]->Draw("same");   	   
	 }
	 std::string name_str = name;
	 name_str.append(".png");
	 char* title = const_cast<char*>(name_str.c_str());//converting string to char
	 //fitFn->SetTitle(title);
	 can->SaveAs(title);
 }//while	  
 ofile->Close(); tablefile.close();
 return 0;
}
