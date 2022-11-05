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
	double x = var[0], Ngaus = par[0], mean = par[1], sigma = par[2],slope= par[3], n = par[4];
	if (x <= mean){
		  double tmp = (x-mean)/sigma;
			return Ngaus*exp(-tmp*tmp/2);
		}
  else{
	double tmp = std::log(x/mean); 
	return Ngaus*exp(-exp(slope)*pow(tmp,1+exp(n))); //add ln( (1-ax)/(1-a*mu) )  and ln ( (1-ax -bx^2) / (1-a*mu*b*mu^2) ) 
	//return Ngaus*exp(-tmp*tmp/2);//-tmp/sigma3);
	}
}

//--------------------------------------------------------------------------

void new_lognormal_3chan_5par(){//2gaus+expo tail
 TCanvas *can= new TCanvas("can","can",700,500); 
 TFile *ifile = new TFile("templates-3chan-HC_wqcdy.root", "READ"); 
 TFile *ofile;
 int param_num = 5;
 int bin_scale = 2;   
 ofile = new TFile("nuisance_hist_3chan_HC_wqcdy_5par.root", "RECREATE");
 ofstream tablefile;
 tablefile.open ("table.txt");
 tablefile<<"HistName\t"<<"HistIntegral\t"<<"FitIntegral\t"<<"Ngaus"<<"\t"<<"Mean"<<"\t"<<"sigma"<<"\t"<<"sigma2"<<"\t"<<"lambda"<<"a"<<endl;

 //--------------------------------------------------------
 TIter nextkey(ifile->GetListOfKeys());
 TKey *key;
 while ((key = (TKey*)nextkey())){
   TObject *obj = key->ReadObj();
   TH1F *hpx = (TH1F*)(obj); TH1F *hpx2 = (TH1F*)hpx->Clone("hpx2");
   const char *name = hpx->GetName();
   std::string s = name;
	if (s.find("DATA") < s.length() || s.find("signal") < s.length()){//clone signal histograms
	   TH1F *clone = (TH1F*) hpx->Clone();
	   hpx->Write();
	}
   
	if (  s.find("qcd") < s.length() || s.find("signal") < s.length()|| s.find("data") < s.length() || s.find("DATA") < s.length()) continue;
	//if (s.find("xxx") > s.length()) continue;// || s.find("minus") > s.length())   
	cout<<name<<endl;
	
   hpx->Rebin(bin_scale);
	int xmin = hpx->GetXaxis()->GetXmin(); 
	int xmax = hpx->GetXaxis()->GetXmax();
	Int_t nbins = hpx->GetNbinsX();
	int binw = (xmax-xmin)/nbins;
	//nbins = bin_scale*hpx->GetNbinsX(); binw = (xmax-xmin)/nbins;
	 
	 
/*         for (int i = 0; i<=nbins; i++){//remove negative bins or too huge bins
     	   if (hpx->GetBinContent(i) < 0 || hpx->GetBinContent(i) > 100){
     	     hpx->SetBinContent(i, 0); hpx->SetBinError(i, 0);}
         }
*/
//-----------------------------------------------------
	 TF1 *fitFn = new TF1("fit_new", "test_new", xmin+binw, xmax-binw, param_num);		  
	 fitFn->SetParNames("Ngaus","Mean","sigma","slope","n");
	 
	 if(s.find("top__b1M") < s.length()){
	 	 fitFn->SetParameters(15, 1100, 187, .67,2.4);
	 }
	 if(s.find("wjets__b1M") < s.length()){
	 	 fitFn->SetParameters(86, 1200, 220, -8,0.117);
	 }
	 if(s.find("top__b1T") < s.length()){
	 	 fitFn->SetParameters(10, 1000,136,2.0,1.0);
	 }
	 if(s.find("wjets__b1T") < s.length()){
	 	 fitFn->SetParameters(12, 1130, 130,1.6,0.4);
	 }
	 if(s.find("top__b2M") < s.length()){
	 	 fitFn->SetParameters(10, 1100, 170,1.4,2.5);
	 }
	 if(s.find("wjets__b2M") < s.length()){
	 	 fitFn->SetParameters(13, 1100, 170,1.5,0.15);
	 }	 
	 if(s.find("stop__b1M") < s.length()){
	 	 fitFn->SetParameters(10, 1200, 170, 2.0,0.65);
	 }
	 if(s.find("stop__b1T") < s.length()){
	 	 fitFn->SetParameters(13, 1300, 240, 2.3,0.2);
	 }
	 if(s.find("stop__b2M") < s.length()){
	 	 fitFn->SetParameters(50, 1100, 170, 2.3,1.1);
	 }

  	 fitFn->SetLineColorAlpha(kBlack,0);fitFn->SetLineWidth(3);
	 hpx->Fit(fitFn,"R");gStyle->SetOptFit(1111);
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
//===========================================================================================================	 
	 nbins = hpx2->GetNbinsX(); binw = (xmax-xmin)/nbins; //cout<<nbins<<endl;

	 TH1F *fit_hist = new TH1F(name, name, nbins, xmin, xmax); 
    fit_hist->SetBinContent(1,hpx2->GetBinContent(1));
    fit_hist->SetBinContent(2,hpx2->GetBinContent(2));
	 fit_hist->SetBinContent(nbins-1,hpx2->GetBinContent(nbins-1));
	 fit_hist->SetBinContent(nbins,hpx2->GetBinContent(nbins));
	 for (int j = 3; j < nbins-1; j++){
		int xval = xmin+(j-.5)*binw;
		fit_hist->SetBinContent(j,fitFn->Eval(xval)/bin_scale); 
	 }
	 fit_hist->Write(); 
	 fit_hist->SetLineWidth(2);fit_hist->Draw("same"); 

	 for(int i = 0; i < param_num; i++){
	 	tablefile<< fitFn->GetParameter(i) + fitFn->GetParError(i)<<"\t";
	 }
	 tablefile<<endl;

	 if( s.find("Up") < s.length()|| s.find("Down") < s.length() ) continue;
	 
	 std::string name_str_up = name, name_str_down = name, name_str_left = name,name_str_right = name;
	 
	 name_str_up.append("__UD_").append(s.substr(0,s.length()-4)).append(s.substr(s.length()-3,3)).append("Up");
	 name_str_down.append("__UD_").append(s.substr(0,s.length()-4)).append(s.substr(s.length()-3,3)).append("Down");
	 name_str_left.append("__LR_").append(s.substr(0,s.length()-4)).append(s.substr(s.length()-3,3)).append("Up");
	 name_str_right.append("__LR_").append(s.substr(0,s.length()-4)).append(s.substr(s.length()-3,3)).append("Down");
	 char* name_up = const_cast<char*>(name_str_up.c_str());//converting string to char
	 char* name_down = const_cast<char*>(name_str_down.c_str());
	 char* name_left = const_cast<char*>(name_str_left.c_str());
	 char* name_right = const_cast<char*>(name_str_right.c_str());
	 
	 TH1F *fitHist_up = new TH1F(name_up, name_up, nbins, xmin,xmax);
	 TH1F *fitHist_down = new TH1F(name_down, name_down, nbins, xmin,xmax);
	 TH1F *fitHist_left = new TH1F(name_left, name_left, nbins, xmin,xmax);
	 TH1F *fitHist_right = new TH1F(name_right, name_right, nbins, xmin,xmax);
	 
    fitHist_up->SetBinContent(1,hpx2->GetBinContent(1));
	 fitHist_up->SetBinContent(nbins,hpx2->GetBinContent(nbins));
	 fitHist_up->SetBinContent(2,hpx2->GetBinContent(2));
	 fitHist_up->SetBinContent(nbins-1,hpx2->GetBinContent(nbins-1));
    fitHist_down->SetBinContent(1,hpx2->GetBinContent(1));
	 fitHist_down->SetBinContent(nbins,hpx2->GetBinContent(nbins));
    fitHist_down->SetBinContent(2,hpx2->GetBinContent(1));
	 fitHist_down->SetBinContent(nbins-1,hpx2->GetBinContent(nbins-1));
    fitHist_left->SetBinContent(1,hpx2->GetBinContent(1));
	 fitHist_left->SetBinContent(nbins,hpx2->GetBinContent(nbins));
    fitHist_left->SetBinContent(2,hpx2->GetBinContent(1));
	 fitHist_left->SetBinContent(nbins-1,hpx2->GetBinContent(nbins-1));
    fitHist_right->SetBinContent(1,hpx2->GetBinContent(1));
	 fitHist_right->SetBinContent(nbins,hpx2->GetBinContent(nbins));
    fitHist_right->SetBinContent(2,hpx2->GetBinContent(1));
	 fitHist_right->SetBinContent(nbins-1,hpx2->GetBinContent(nbins-1));
	 for(int i = 3; i < nbins-1; i++){ 
		//-----------------------------
	 	double xval = xmin+(i-0.5)*binw; double yval = fitFn->GetMaximum()*(i-0.5)/nbins; 
	 	/*fitHist_up->SetBinContent(i, std::max({fitFn->Eval(xval), fitFn_plus[1]->Eval(xval),fitFn_plus[2]->Eval(xval),fitFn_plus[3]->Eval(xval),fitFn_plus[4]->Eval(xval),fitFn_plus[5]->Eval(xval),fitFn_minus[1]->Eval(xval),fitFn_minus[2]->Eval(xval),fitFn_minus[3]->Eval(xval),fitFn_minus[4]->Eval(xval),fitFn_minus[5]->Eval(xval)}));
	 	fitHist_down->SetBinContent(i, std::min({fitFn->Eval(xval), fitFn_plus[1]->Eval(xval),fitFn_plus[2]->Eval(xval),fitFn_plus[3]->Eval(xval),fitFn_plus[4]->Eval(xval),fitFn_plus[5]->Eval(xval),fitFn_minus[1]->Eval(xval),fitFn_minus[2]->Eval(xval),fitFn_minus[3]->Eval(xval),fitFn_minus[4]->Eval(xval),fitFn_minus[5]->Eval(xval)}));*/
		double fitFn_array_UD[(param_num-1)*2+1]; fitFn_array_UD[0] = fitFn->Eval(xval);
		for (int j = 1; j <= (param_num-1); j++){
			fitFn_array_UD[j] = fitFn_plus[j]->Eval(xval);
			fitFn_array_UD[(param_num-1)+j] = fitFn_minus[j]->Eval(xval);
		}
		//Sort the array in ascending order
		double temp = 0;    
	   for (int j = 0; j < (param_num-1)*2+1; j++) {     
			for (int k = j+1; k < (param_num-1)*2+1; k++) {     
		          if(fitFn_array_UD[j] > fitFn_array_UD[k]) {    
		          temp = fitFn_array_UD[j];    
		          fitFn_array_UD[j] = fitFn_array_UD[k];    
		          fitFn_array_UD[k] = temp;    
		          }     
		   }     
	   }
	        fitHist_up->SetBinContent(i, fitFn_array_UD[(param_num-1)*2]/bin_scale);
	        fitHist_down->SetBinContent(i, fitFn_array_UD[0]/bin_scale);
	        if(fitFn_array_UD[(param_num-1)*2] < 0.01)//picks the 2nd largest
	            fitHist_up->SetBinContent(i, fitFn_array_UD[(param_num-1)*2-1]/bin_scale);
	        //if(fitFn_array_UD[0] < 0.01)
	        if(abs(fitFn->GetParError(5)/fitFn->GetParameter(5)) > 0.4 || abs(fitFn->GetParError(5)/fitFn->GetParameter(5)) > 0.4  )//picks the 2nd smallest and eliminates the problematic sigma2 and slope fns 
	                fitHist_down->SetBinContent(i, fitFn_array_UD[1]/bin_scale);            
	 	//-----------------------------
	 	/*double xval_max = -9999, xval_min = 9999, fitMean = fitFn->GetParameter(1);
		if(xval <= fitMean){
		 	xval_min = std::min({fitFn_plus[1]->GetX(yval, xmin, fitMean),fitFn_plus[2]->GetX(yval, xmin, fitMean),fitFn_plus[3]->GetX(yval, xmin, fitMean),fitFn_plus[4]->GetX(yval, xmin, fitMean),fitFn_plus[5]->GetX(yval, xmin, fitMean),fitFn_minus[1]->GetX(yval, xmin, fitMean),fitFn_minus[2]->GetX(yval, xmin, fitMean),fitFn_minus[3]->GetX(yval, xmin, fitMean),fitFn_minus[4]->GetX(yval, xmin, fitMean),fitFn_minus[5]->GetX(yval, xmin, fitMean)});
		 	xval_max = std::max({fitFn_plus[1]->GetX(yval, xmin, fitMean),fitFn_plus[2]->GetX(yval, xmin, fitMean),fitFn_plus[3]->GetX(yval, xmin, fitMean),fitFn_plus[4]->GetX(yval, xmin, fitMean),fitFn_plus[5]->GetX(yval, xmin, fitMean),fitFn_minus[1]->GetX(yval, xmin, fitMean),fitFn_minus[2]->GetX(yval, xmin, fitMean),fitFn_minus[3]->GetX(yval, xmin, fitMean),fitFn_minus[4]->GetX(yval, xmin, fitMean),fitFn_minus[5]->GetX(yval, xmin, fitMean)});
		}
		else if(xval > fitMean){
		 	xval_min = std::min({fitFn_plus[1]->GetX(yval, fitMean, xmax),fitFn_plus[2]->GetX(yval, fitMean, xmax),fitFn_plus[3]->GetX(yval, fitMean, xmax),fitFn_plus[4]->GetX(yval, fitMean, xmax),fitFn_plus[5]->GetX(yval, fitMean, xmax),fitFn_minus[1]->GetX(yval, fitMean, xmax),fitFn_minus[2]->GetX(yval, fitMean, xmax),fitFn_minus[3]->GetX(yval, fitMean, xmax),fitFn_minus[4]->GetX(yval, fitMean, xmax),fitFn_minus[5]->GetX(yval, fitMean, xmax)});
		 	xval_max = std::max({fitFn_plus[1]->GetX(yval, fitMean, xmax),fitFn_plus[2]->GetX(yval, fitMean, xmax),fitFn_plus[3]->GetX(yval, fitMean, xmax),fitFn_plus[4]->GetX(yval, fitMean, xmax),fitFn_plus[5]->GetX(yval, fitMean, xmax),fitFn_minus[1]->GetX(yval, fitMean, xmax),fitFn_minus[2]->GetX(yval, fitMean, xmax),fitFn_minus[3]->GetX(yval, fitMean, xmax),fitFn_minus[4]->GetX(yval, fitMean, xmax),fitFn_minus[5]->GetX(yval, fitMean, xmax)});
		}
	 	fitHist_left->SetBinContent((xval_min-xmin)/binw, yval);
	 	fitHist_right->SetBinContent((xval_max-xmin)/binw, yval);*/
		//----------------------------------------
/*		double fitMean_left = fitFn->GetParameter(1),fitMean_right = fitFn->GetParameter(1);
	 	if(xval <= fitMean_left){
		 	fitHist_left->SetBinContent(i, std::max({fitFn->Eval(xval),fitFn_plus[2]->Eval(xval),fitFn_plus[3]->Eval(xval),fitFn_plus[4]->Eval(xval),fitFn_plus[5]->Eval(xval),fitFn_minus[2]->Eval(xval),fitFn_minus[3]->Eval(xval),fitFn_minus[4]->Eval(xval),fitFn_minus[5]->Eval(xval)})/bin_scale);
		}
		else if(xval > fitMean_left){
		 	fitHist_left->SetBinContent(i, std::min({fitFn->Eval(xval),fitFn_plus[2]->Eval(xval),fitFn_plus[3]->Eval(xval),fitFn_plus[4]->Eval(xval),fitFn_plus[5]->Eval(xval),fitFn_minus[2]->Eval(xval),fitFn_minus[3]->Eval(xval),fitFn_minus[4]->Eval(xval),fitFn_minus[5]->Eval(xval)})/bin_scale);
		}
	 	if(xval <= fitMean_right){
		 	fitHist_right->SetBinContent(i, std::min({fitFn->Eval(xval),fitFn_plus[2]->Eval(xval),fitFn_plus[3]->Eval(xval),fitFn_plus[4]->Eval(xval),fitFn_plus[5]->Eval(xval),fitFn_minus[2]->Eval(xval),fitFn_minus[3]->Eval(xval),fitFn_minus[4]->Eval(xval),fitFn_minus[5]->Eval(xval)})/bin_scale);
		}
		else if(xval > fitMean_right){
		 	fitHist_right->SetBinContent(i, std::max({fitFn->Eval(xval),fitFn_plus[2]->Eval(xval),fitFn_plus[3]->Eval(xval),fitFn_plus[4]->Eval(xval),fitFn_plus[5]->Eval(xval),fitFn_minus[2]->Eval(xval),fitFn_minus[3]->Eval(xval),fitFn_minus[4]->Eval(xval),fitFn_minus[5]->Eval(xval)})/bin_scale);
		}*/
		//-----------------------------
		double fitFn_array_LR[(param_num-2)*2+1]; fitFn_array_LR[0] = fitFn->Eval(xval);//will give same as using min max from param_num=2++
		for (int j = 1; j <= (param_num-2); j++){
			fitFn_array_LR[j] = fitFn_plus[j+1]->Eval(xval);
			fitFn_array_LR[(param_num-2)+j] = fitFn_minus[j+1]->Eval(xval);
		}
		//Sort the array in ascending order
		temp = 0;    
	        for (int j = 0; j < (param_num-2)*2+1; j++) {     
			for (int k = j+1; k < (param_num-2)*2+1; k++) {     
		          if(fitFn_array_LR[j] > fitFn_array_LR[k]) {    
		          temp = fitFn_array_LR[j];    
		          fitFn_array_LR[j] = fitFn_array_LR[k];    
		          fitFn_array_LR[k] = temp;    
		          }     
		        }     
	        }
		double fitMean_left = fitFn->GetParameter(1),fitMean_right = fitFn->GetParameter(1);
	 	if(xval <= fitMean_left){
		 	fitHist_left->SetBinContent(i, fitFn_array_LR[(param_num-2)*2]/bin_scale);
		 	if(fitFn_array_LR[(param_num-2)*2] < 0.01)
	        	    fitHist_left->SetBinContent(i, fitFn_array_LR[(param_num-2)*2-1]/bin_scale);
		}
		else if(xval > fitMean_left){
		 	fitHist_left->SetBinContent(i, fitFn_array_LR[0]/bin_scale);
	        	if(fitFn_array_LR[0] < 0.01)
	        	    fitHist_left->SetBinContent(i, fitFn_array_LR[1]/bin_scale);
		}
	 	if(xval <= fitMean_right){
		 	fitHist_right->SetBinContent(i, fitFn_array_LR[0]/bin_scale);
		 	if(fitFn_array_LR[0] < 0.01)
	        	    fitHist_right->SetBinContent(i, fitFn_array_LR[1]/bin_scale);
		}
		else if(xval > fitMean_right){
		 	fitHist_right->SetBinContent(i, fitFn_array_LR[(param_num-2)*2]/bin_scale);
		 	if(fitFn_array_LR[(param_num-2)*2] < 0.01)
	        	    fitHist_right->SetBinContent(i, fitFn_array_LR[(param_num-2)*2-1]/bin_scale);
		}
		//cout<< std::min({fitFn->Eval(xval),fitFn_plus[1]->Eval(xval),fitFn_minus[1]->Eval(xval),fitFn_plus[2]->Eval(xval),fitFn_plus[3]->Eval(xval),fitFn_plus[4]->Eval(xval),fitFn_plus[5]->Eval(xval),fitFn_minus[2]->Eval(xval),fitFn_minus[3]->Eval(xval),fitFn_minus[4]->Eval(xval),fitFn_minus[5]->Eval(xval)})<<"\t"<<  std::max({fitFn->Eval(xval),fitFn_plus[1]->Eval(xval),fitFn_minus[1]->Eval(xval),fitFn_plus[2]->Eval(xval),fitFn_plus[3]->Eval(xval),fitFn_plus[4]->Eval(xval),fitFn_plus[5]->Eval(xval),fitFn_minus[2]->Eval(xval),fitFn_minus[3]->Eval(xval),fitFn_minus[4]->Eval(xval),fitFn_minus[5]->Eval(xval)})<<"\t"<<fitFn_array_UD[0]<<"\t"<<fitFn_array_UD[(param_num-1)*2]<<endl;		
	 }
	 fitHist_up->Write();fitHist_down->Write();fitHist_left->Write();fitHist_right->Write();
	 fitHist_up->SetLineColor(4);fitHist_down->SetLineColor(4);fitHist_left->SetLineColor(2);fitHist_right->SetLineColor(2);
	 std::string name_str = name; 	 	
	 //name_str.append("_UD.png"); fitHist_up->Draw("same");fitHist_down->Draw("same");	
	 name_str.append("_LR.png"); fitHist_left->Draw("same");fitHist_right->Draw("same");
	 char* title = const_cast<char*>(name_str.c_str());//converting string to char
	 can->SaveAs(title);
 }//while	 
 ofile->Close(); tablefile.close();
 return 0;
}
