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
	double x = var[0], Ngaus = par[0], mean = par[1], sigma = par[2], sigma2 = par[3],slope= par[4], n = par[5];
	if (x <= mean){
		  double tmp = (x-mean)/sigma;
			return Ngaus*exp(-tmp*tmp/2);
		}
  else{
	double tmp = std::log(x/mean)/sigma2; 
	return Ngaus*exp(-exp(slope)*pow(tmp*sigma2,1+exp(n))-tmp*tmp/2); //add ln( (1-ax)/(1-a*mu) )  and ln ( (1-ax -bx^2) / (1-a*mu*b*mu^2) ) 
	//return Ngaus*exp(-tmp*tmp/2);//-tmp/sigma3);
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

void new_lognormal2(){//2gaus+expo tail
 TCanvas *can= new TCanvas("can","can",700,500); 
 //TCanvas *can1= new TCanvas("can1","can1",700,500); 
 TFile *ifile0 = new TFile("templates-3chan-Theta-noFwd-Stop.root", "READ");
 TFile *ifile = new TFile("templates-3chan-Theta_rebin_wqcdy.root", "READ");
 TFile *ifile1 = new TFile("templates-6chan-Theta_rebin_wqcdy.root", "READ"); 
 TFile *ifile2 = new TFile("templates-3chan-Theta_wqcdy.root", "READ"); 
 TFile *ofile;
 int param_num = 6;
 int bin_scale = 2;   
 if(bin_scale == 2){ 
	//ofile = new TFile("analytic_hist_6chan_noFwdStop_50bins_wqcdy.root", "RECREATE");
	ofile = new TFile("nuisance_hist_6chan_noFwdStop_50bins_wqcdy.root", "RECREATE");
 }
 else if(bin_scale == 4){ 
	//ofile = new TFile("analytic_hist_6chan_noFwdStop_100bins_wqcdy.root", "RECREATE");
	ofile = new TFile("nuisance_hist_6chan_noFwdStop_100bins_wqcdy.root", "RECREATE");
 }
 else{
	//ofile = new TFile("analytic_hist_6chan_noFwdStop_25bins_wqcdy.root", "RECREATE");
	ofile = new TFile("nuisance_hist_6chan_noFwdStop_25bins_wqcdy.root", "RECREATE");
 }
 ofstream tablefile;
 tablefile.open ("table.txt");
 tablefile<<"HistName\t"<<"HistIntegral\t"<<"FitIntegral\t"<<"Ngaus"<<"\t"<<"Mean"<<"\t"<<"sigma"<<"\t"<<"sigma2"<<"\t"<<"lambda"<<"a"<<endl;


 //--------------------------------------------------------
 TIter nextkey2(ifile2->GetListOfKeys());
 TKey *key2;
 while ((key2 = (TKey*)nextkey2())){
	 TObject *obj1 = key2->ReadObj();
	 TH1F *hpx1 = (TH1F*)(obj1);
	 const char *name1 = hpx1->GetName();
	 std::string s1 = name1;
	 if (s1.find("DATA") < s1.length() || s1.find("signal") < s1.length()){//clone signal histograms
	   TH1F *clone1 = (TH1F*) hpx1->Clone();
	   hpx1->Write();
	 }
 }
//----------------------------------------------------------
 TIter nextkey(ifile->GetListOfKeys());
 TKey *key;
 while ((key = (TKey*)nextkey())){
   TObject *obj = key->ReadObj();
   TH1F *hpx = (TH1F*)(obj);
   const char *name = hpx->GetName();
   std::string s = name;
 
	 int xmin = hpx->GetXaxis()->GetXmin(); 
	 int xmax = hpx->GetXaxis()->GetXmax();
	 Int_t nbins = hpx->GetNbinsX();
	 int binw = (xmax-xmin)/nbins;
	 nbins = bin_scale*hpx->GetNbinsX(); binw = (xmax-xmin)/nbins;
/*	 if (s.find("signal") < s.length()){  cout<<name<<endl; 
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
   
	 if ( s.find("signal") < s.length()|| s.find("data") < s.length() || s.find("DATA") < s.length() || s.find("stop") < s.length()) continue;
	 //if (s.find("xxx") > s.length()) continue;// || s.find("minus") > s.length())
	   
	 cout<<name<<endl;
	 
         for (int i = 0; i<=nbins; i++){//remove negative bins or too huge bins
     	   if (hpx->GetBinContent(i) < 0 || hpx->GetBinContent(i) > 100){
     	     hpx->SetBinContent(i, 0); hpx->SetBinError(i, 0);}
         }

//-----------------------------------------------------
	 TF1 *fitFn = new TF1("fit_new", "test_new", xmin+binw, xmax-binw, param_num);		  
/*		//finding stddev and mean for log(x) histogram
		TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
		for (int i = 0; i < nbins+2; i++){
		 int yaxis = hpx->GetBinContent(i);
		 double xaxis = std::log(xmin+i*binw);
		 h1->Fill(xaxis,yaxis);
		}*/
	 fitFn->SetParNames("Ngaus","Mean","sigma","sigma2","slope","n");	 	 fitFn->SetParameters(10, 1200, 170, 0.35, 1.6,1.43);
	 if(s.find("b1M__stop") < s.length()){
	 	 fitFn->SetParameters(10, 1200, 170, 0.35, 1.6,1.43);
	 }
	 if(s.find("b1M__top") < s.length()){
	 	 fitFn->SetParameters(15, 1100, 187, 0.29, .67,2.4);
	 }
	 if(s.find("b1M__qcd") < s.length()){
	 	 fitFn->SetParameters(86, 1200, 220, 0.28, -8,0.117);
	 }
	 if(s.find("b1M__wjet") < s.length()){//if(s.find("b1M__wjets") < s.length()){
	 	 fitFn->SetParameters(86, 1200, 220, 0.29, -8,0.117);
	 }
	 if(s.find("b1T__stop") < s.length()){
	 	 fitFn->SetParameters(13, 1300, 240, 0.25, 2.3,2);
	 }
	 if(s.find("b1T__top") < s.length()){
	 	 fitFn->SetParameters(10, 1200, -219, 0.4, 1.7,0.45);
	 }
	 if(s.find("b1T__qcd") < s.length()){
	 	 fitFn->SetParameters(16, 1300, 210, 0.26, -39,9);
	 }
	 if(s.find("b1T__wjet") < s.length()){//if(s.find("b1T__wjets") < s.length()){
	 	 fitFn->SetParameters(21, 1200, 230, 0.26, -20,6);
	 }
	 if(s.find("b2M__stop") < s.length()){
	 	 fitFn->SetParameters(50, 1100, 170, 0.3, 12,5);
	 }
	 if(s.find("b2M__top") < s.length()){
	 	 fitFn->SetParameters(10, 1100, 170, 0.35, 1.4,2.5);
	 }
	 if(s.find("b2M__qcd") < s.length()){
	 	 fitFn->SetParameters(10, 900, 90, 10, 1.2,0.3);
	 }	 
	 if(s.find("b2M__wjet") < s.length()){//if(s.find("b2M__wjets") < s.length()){
	 	 fitFn->SetParameters(10, 900, 90, 10, 1.2,0.3);
	 }
	 //fitFn->SetParNames("Ngaus","Mean","sigma","sigma2","lambda","a"); fitFn->SetParameters(10, 1000, 300, h1->GetStdDev(), 2,1/sqrt(1300));
   //fitFn->SetParNames("Ngaus","Mean","sigma","sigma2"); fitFn->SetParameters(42, 1100, 170,0.3);//,h1->GetStdDev());
  	 fitFn->SetLineColorAlpha(kBlack,0);fitFn->SetLineWidth(3);
	 hpx->Fit(fitFn,"R");gStyle->SetOptFit(1111);
	 
	 TH1F *fit_hist = new TH1F(name, name, nbins, xmin, xmax); 
   	 fit_hist->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fit_hist->SetBinContent(nbins,hpx->GetBinContent(hpx->GetNbinsX())/bin_scale);
	 for (int j = 2; j < nbins; j++){
		int xval = xmin+(j-.5)*binw;
		fit_hist->SetBinContent(j,fitFn->Eval(xval)/bin_scale); 
	 }
	 fit_hist->Write();
	 //fit_hist->SetLineWidth(2);fit_hist->Draw("same"); 
	 tablefile<<name<<"\t"<<hpx->Integral()<<"\t"<<fitFn->Integral(xmin+binw,xmax-binw)/(bin_scale*binw)<<"\t";
	 for(int i = 0; i < param_num; i++){
	 	tablefile<< fitFn->GetParameter(i) + fitFn->GetParError(i)<<"\t";
	 }
	 tablefile<<endl;
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
	 
	 
//----------------------------------------------------------------------
	
	 TIter nextkey1(ifile1->GetListOfKeys());
	 TKey *key1;
	 while ((key1 = (TKey*)nextkey1())){//while2
		 TObject *obj1 = key1->ReadObj();
		 TH1F *hpx1 = (TH1F*)(obj1);
		 const char *name1 = hpx1->GetName();
		 std::string s1 = name1;
		 if (s1.find("DATA") < s1.length() || s1.find("signal") < s1.length()) continue; //everything except data and signal
     
	         for (int i = 0; i<=nbins; i++){//remove negative bins
     		   if (hpx1->GetBinContent(i) < 0)
     	       	     hpx1->SetBinContent(i, 0);
    	         }
		 if(s1.find("wjets__pdf") < s1.length()){
		   for (int i = 0; i<=nbins; i++){//remove too huge bins
		   	 if (hpx1->GetBinContent(i) > 100)
		   	 	 hpx1->SetBinContent(i, 0);
		 }}		 		
		 
		 nbins = bin_scale*hpx1->GetNbinsX(); binw = (xmax-xmin)/nbins; 
		 if (s1.compare(0,3, s,0,3)==0 && s1.compare(5,s1.length()-5, s,3,s.length()-3)==0){//checking channels are similar
			 TF1 *fitFn1 = new TF1("fitFn1", "test_new", xmin, xmax,param_num);
			 double renorm = fitFn->GetParameter(0)*hpx1->Integral()*hpx->GetNbinsX()/(fit_hist->Integral()*hpx1->GetNbinsX());
			 fitFn1->FixParameter(0,renorm);//renormalising
			 fitFn1->FixParameter(1,fitFn->GetParameter(1));
			 fitFn1->FixParameter(2,fitFn->GetParameter(2));
			 fitFn1->FixParameter(3,fitFn->GetParameter(3));
			 fitFn1->FixParameter(4,fitFn->GetParameter(4));
			 fitFn1->FixParameter(5,fitFn->GetParameter(5));
			 fitFn1->SetParNames("Ngaus","Mean","sigma","sigma2","slope","n"); 

		 	 /*TF1 *fitFn1_plus[param_num];
			 TF1 *fitFn1_minus[param_num];
			 for(int i = 0; i < param_num; i++){
				  fitFn1_plus[i] = new TF1("fitFn1_plus", "test_new", xmin,xmax,param_num);
					fitFn1_minus[i] = new TF1("fitFn1_minus", "test_new", xmin,xmax,param_num);
					fitFn1_plus[i]->SetParameters(fitFn1->GetParameters());
				  fitFn1_minus[i]->SetParameters(fitFn1->GetParameters());
				  if (i == 0){//for NGaus
						fitFn1_plus[i]->SetParameter(i,fitFn1->GetParameter(i) + fitFn->GetParError(i)*hpx1->Integral()/fit_hist->Integral());
						fitFn1_minus[i]->SetParameter(i,fitFn1->GetParameter(i) - fitFn->GetParError(i)*hpx1->Integral()/fit_hist->Integral());	
					}			  	
				  else{
						fitFn1_plus[i]->SetParameter(i,fitFn1->GetParameter(i) + fitFn->GetParError(i));
						fitFn1_minus[i]->SetParameter(i,fitFn1->GetParameter(i) - fitFn->GetParError(i));
					}
			 		
			 		if (s1.find("plus") < s1.length() || s1.find("minus") < s1.length())//only make param+- hist on nominal hist
			 			continue;
			 		std::string name_str_plus = name1, name_str_minus = name1, dup_plus = name, dup_minus = name;
			 		char* name_plus = const_cast<char*>(name_str_plus.append("__").append(fitFn1->GetParName(i)).append("_").append(dup_plus.replace(dup_plus.begin()+3,dup_plus.begin()+4,"")).append("__plus").c_str());
		 	    char* name_minus = const_cast<char*>(name_str_minus.append("__").append(fitFn1->GetParName(i)).append("_").append(dup_minus.replace(dup_minus.begin()+3,dup_minus.begin()+4,"")).append("__minus").c_str());	 
			 		TH1F *fit_hist_plus = new TH1F(name_plus, name_plus, nbins, xmin, xmax); 
			 		TH1F *fit_hist_minus = new TH1F(name_minus, name_minus, nbins, xmin, xmax); 
				  fit_hist_plus->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale); 
					fit_hist_minus->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale); 	 		
				  fit_hist_plus->SetBinContent(nbins,hpx1->GetBinContent(nbins)/bin_scale); 
					fit_hist_minus->SetBinContent(nbins,hpx1->GetBinContent(nbins)/bin_scale); 	
					for (int k = 2; k < nbins; k++){
						int xval = xmin+(k-.5)*binw;
						fit_hist_plus->SetBinContent(k,fitFn1_plus[i]->Eval(xval)/bin_scale); 
					  fit_hist_minus->SetBinContent(k,fitFn1_minus[i]->Eval(xval)/bin_scale); 
					}
					fit_hist_plus->Write(); fit_hist_minus->Write();
			 }*/				 		   
			 TH1F *fit_hist1 = new TH1F(name1, name1, nbins, xmin, xmax); 
			 fit_hist1->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale);
			 fit_hist1->SetBinContent(nbins,hpx1->GetBinContent(hpx1->GetNbinsX())/bin_scale);
			 for (int j = 2; j < nbins; j++){
				int xval = xmin+(j-.5)*binw;
				fit_hist1->SetBinContent(j,fitFn1->Eval(xval)); 
			 }
			 fit_hist1->Write();
			 tablefile<<name1<<"\t"<<"\t"<<hpx1->Integral()<<"\t"<<fit_hist1->Integral()<<endl;
		 }//if
	 }//while2
//===========================================================================================================	 
	 if( s.find("minus") < s.length()|| s.find("plus") < s.length() ) continue;
	 
	 std::string name_str_up = name, name_str_down = name, name_str_left = name,name_str_right = name;
	 name_str_up.append("__UD__plus"); name_str_down.append("__UD__minus"); name_str_left.append("__LR__minus"); name_str_right.append("__LR__plus");
	 char* name_up = const_cast<char*>(name_str_up.c_str());//converting string to char
	 char* name_down = const_cast<char*>(name_str_down.c_str());
	 char* name_left = const_cast<char*>(name_str_left.c_str());
	 char* name_right = const_cast<char*>(name_str_right.c_str());
	 
	 TH1F *fitHist_up = new TH1F(name_up, name_up, nbins, xmin,xmax);
	 TH1F *fitHist_down = new TH1F(name_down, name_down, nbins, xmin,xmax);
	 TH1F *fitHist_left = new TH1F(name_left, name_left, nbins, xmin,xmax);
	 TH1F *fitHist_right = new TH1F(name_right, name_right, nbins, xmin,xmax);
	 
   	 fitHist_up->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fitHist_up->SetBinContent(nbins,hpx->GetBinContent(hpx->GetNbinsX())/bin_scale);
   	 fitHist_down->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fitHist_down->SetBinContent(nbins,hpx->GetBinContent(hpx->GetNbinsX())/bin_scale);
   	 fitHist_left->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fitHist_left->SetBinContent(nbins,hpx->GetBinContent(hpx->GetNbinsX())/bin_scale);
   	 fitHist_right->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fitHist_right->SetBinContent(nbins,hpx->GetBinContent(hpx->GetNbinsX())/bin_scale); 
	 for(int i = 2; i < nbins; i++){ 
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
	 name_str.append("_UD.png"); fitHist_up->Draw("same");fitHist_down->Draw("same");	
	 //name_str.append("_LR.png"); fitHist_left->Draw("same");fitHist_right->Draw("same");
	 char* title = const_cast<char*>(name_str.c_str());//converting string to char
	 can->SaveAs(title);
 }//while	 
 //========================================================================================================================================
 TIter nextkey0(ifile0->GetListOfKeys());
 TKey *key0;
 while ((key0 = (TKey*)nextkey0())){
   TObject *obj = key0->ReadObj();
   TH1F *hpx = (TH1F*)(obj);
   const char *name = hpx->GetName();
   std::string s = name;

	 //if (s.find("_") < s.length()) continue;   
	 if (s.find("stop") > s.length())continue;
	 //if (s.find("minus") < s.length()|| s.find("plus") < s.length()) continue;
	 cout<<name<<endl;
	 
	 int xmin = hpx->GetXaxis()->GetXmin(); 
	 int xmax = hpx->GetXaxis()->GetXmax();
	 Int_t nbins = hpx->GetNbinsX();
	 int binw = (xmax-xmin)/nbins;
	 nbins = hpx->GetNbinsX(); binw = (xmax-xmin)/nbins;

//-----------------------------------------------------
	 
	 TF1 *fitFn = new TF1("fit_new", "test_new", xmin+binw, xmax-binw, param_num);		  
/*		//finding stddev and mean for log(x) histogram
		TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
		for (int i = 0; i < nbins+2; i++){
		 int yaxis = hpx->GetBinContent(i);
		 double xaxis = std::log(xmin+i*binw);
		 h1->Fill(xaxis,yaxis);
		}*/
	 fitFn->SetParNames("Ngaus","Mean","sigma","sigma2","slope","n");
	 if(s.find("b1M__stop") < s.length()){
	 	 fitFn->SetParameters(10, 1200, 170, 0.35, 1.6,1.43);
	 }
	 if(s.find("b1T__stop") < s.length()){
	 	 fitFn->SetParameters(13, 1300, 240, 80, 5,.3);
	 }
	 if(s.find("b2M__stop") < s.length()){
	 	 fitFn->SetParameters(50, 1100, 170, 0.5, 12,5.5);
	 }	 
	 //fitFn->SetParNames("Ngaus","Mean","sigma","sigma2","lambda","a"); fitFn->SetParameters(10, 1000, 300, h1->GetStdDev(), 2,1/sqrt(1300));
   //fitFn->SetParNames("Ngaus","Mean","sigma","sigma2"); fitFn->SetParameters(42, 1100, 170,0.3);//,h1->GetStdDev());
  	 //fitFn->SetLineColorAlpha(kBlack,0);fitFn->SetLineWidth(3);
  	 fitFn->SetLineColorAlpha(kBlack,0);fitFn->SetLineWidth(3);
	 hpx->Fit(fitFn,"R");gStyle->SetOptFit(1111);
	 
	 bin_scale = 1;
	 TH1F *fit_hist = new TH1F(name, name, hpx->GetNbinsX(), xmin, xmax); 
   	 fit_hist->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fit_hist->SetBinContent(nbins,hpx->GetBinContent(hpx->GetNbinsX())/bin_scale);
	 for (int j = 2; j < nbins; j++){
		int xval = xmin+(j-.5)*binw;
		fit_hist->SetBinContent(j,fitFn->Eval(xval)/bin_scale); 
	 }
	 fit_hist->Write();
	 //fit_hist->SetLineWidth(2);fit_hist->Draw("same"); 
	 tablefile<<name<<"\t"<<hpx->Integral()<<"\t"<<fitFn->Integral(xmin+binw,xmax-binw)/(binw)<<"\t";
	 for(int i = 0; i < param_num; i++){
	 	tablefile<< fitFn->GetParameter(i) + fitFn->GetParError(i)<<"\t";
	 }
	 tablefile<<endl;
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
//----------------------------------------------------------------------
	
	 TIter nextkey1(ifile1->GetListOfKeys());
	 TKey *key1;
	 while ((key1 = (TKey*)nextkey1())){//while2
		 TObject *obj1 = key1->ReadObj();
		 TH1F *hpx1 = (TH1F*)(obj1);
		 const char *name1 = hpx1->GetName();
		 std::string s1 = name1;
		 if (s1.find("stop") > s1.length()) continue; //everything except data and signal
     
	         for (int i = 0; i<=nbins; i++){//remove negative bins
     		   if (hpx1->GetBinContent(i) < 0)
     	       	     hpx1->SetBinContent(i, 0);
    	         }	 		
		 bin_scale = 2;
		 nbins = bin_scale*hpx1->GetNbinsX(); binw = (xmax-xmin)/nbins; 
		 if (s1.compare(0,3, s,0,3)==0 && s1.compare(5,s1.length()-5, s,3,s.length()-3)==0){//checking channels are similar
			 TF1 *fitFn1 = new TF1("fitFn1", "test_new", xmin, xmax,param_num);
			 double renorm = fitFn->GetParameter(0)*hpx1->Integral()*hpx->GetNbinsX()/(fit_hist->Integral()*hpx1->GetNbinsX());
			 fitFn1->FixParameter(0,renorm);//renormalising
			 fitFn1->FixParameter(1,fitFn->GetParameter(1));
			 fitFn1->FixParameter(2,fitFn->GetParameter(2));
			 fitFn1->FixParameter(3,fitFn->GetParameter(3));
			 fitFn1->FixParameter(4,fitFn->GetParameter(4));
			 fitFn1->FixParameter(5,fitFn->GetParameter(5));
			 fitFn1->SetParNames("Ngaus","Mean","sigma","sigma2","slope","n"); 

		 	 /*TF1 *fitFn1_plus[param_num];
			 TF1 *fitFn1_minus[param_num];
			 for(int i = 0; i < param_num; i++){
				  fitFn1_plus[i] = new TF1("fitFn1_plus", "test_new", xmin,xmax,param_num);
					fitFn1_minus[i] = new TF1("fitFn1_minus", "test_new", xmin,xmax,param_num);
					fitFn1_plus[i]->SetParameters(fitFn1->GetParameters());
				  fitFn1_minus[i]->SetParameters(fitFn1->GetParameters());
				  if (i == 0){//for NGaus
						fitFn1_plus[i]->SetParameter(i,fitFn1->GetParameter(i) + fitFn->GetParError(i)*hpx1->Integral()/fit_hist->Integral());
						fitFn1_minus[i]->SetParameter(i,fitFn1->GetParameter(i) - fitFn->GetParError(i)*hpx1->Integral()/fit_hist->Integral());	
					}			  	
				  else{
						fitFn1_plus[i]->SetParameter(i,fitFn1->GetParameter(i) + fitFn->GetParError(i));
						fitFn1_minus[i]->SetParameter(i,fitFn1->GetParameter(i) - fitFn->GetParError(i));
					}
			 		
			 		if (s1.find("plus") < s1.length() || s1.find("minus") < s1.length())//only make param+- hist on nominal hist
			 			continue;
			 		std::string name_str_plus = name1, name_str_minus = name1, dup_plus = name, dup_minus = name;
			 		char* name_plus = const_cast<char*>(name_str_plus.append("__").append(fitFn1->GetParName(i)).append("_").append(dup_plus.replace(dup_plus.begin()+3,dup_plus.begin()+4,"")).append("__plus").c_str());
		 	    char* name_minus = const_cast<char*>(name_str_minus.append("__").append(fitFn1->GetParName(i)).append("_").append(dup_minus.replace(dup_minus.begin()+3,dup_minus.begin()+4,"")).append("__minus").c_str());	 
			 		TH1F *fit_hist_plus = new TH1F(name_plus, name_plus, nbins, xmin, xmax); 
			 		TH1F *fit_hist_minus = new TH1F(name_minus, name_minus, nbins, xmin, xmax); 
				  fit_hist_plus->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale); 
					fit_hist_minus->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale); 	 		
				  fit_hist_plus->SetBinContent(nbins,hpx1->GetBinContent(nbins)/bin_scale); 
					fit_hist_minus->SetBinContent(nbins,hpx1->GetBinContent(nbins)/bin_scale); 	
					for (int k = 2; k < nbins; k++){
						int xval = xmin+(k-.5)*binw;
						fit_hist_plus->SetBinContent(k,fitFn1_plus[i]->Eval(xval)/bin_scale); 
					  fit_hist_minus->SetBinContent(k,fitFn1_minus[i]->Eval(xval)/bin_scale); 
					}
					fit_hist_plus->Write(); fit_hist_minus->Write();
			 }*/				 		   
			 TH1F *fit_hist1 = new TH1F(name1, name1, nbins, xmin, xmax); 
			 fit_hist1->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale);
			 fit_hist1->SetBinContent(nbins,hpx1->GetBinContent(hpx1->GetNbinsX())/bin_scale);
			 for (int j = 2; j < nbins; j++){
				int xval = xmin+(j-.5)*binw;
				fit_hist1->SetBinContent(j,fitFn1->Eval(xval)); 
			 }
			 fit_hist1->Write();
			 tablefile<<name1<<"\t"<<"\t"<<hpx1->Integral()<<"\t"<<fit_hist1->Integral()<<endl;
		 }//if
	 }//while2
//===========================================================================================================	 
	 if( s.find("minus") < s.length()|| s.find("plus") < s.length() ) continue;
	 
	 std::string name_str_up = name, name_str_down = name, name_str_left = name,name_str_right = name;
	 name_str_up.append("__UD__plus"); name_str_down.append("__UD__minus"); name_str_left.append("__LR__minus"); name_str_right.append("__LR__plus");
	 char* name_up = const_cast<char*>(name_str_up.c_str());//converting string to char
	 char* name_down = const_cast<char*>(name_str_down.c_str());
	 char* name_left = const_cast<char*>(name_str_left.c_str());
	 char* name_right = const_cast<char*>(name_str_right.c_str());
	 
	 TH1F *fitHist_up = new TH1F(name_up, name_up, nbins, xmin,xmax);
	 TH1F *fitHist_down = new TH1F(name_down, name_down, nbins, xmin,xmax);
	 TH1F *fitHist_left = new TH1F(name_left, name_left, nbins, xmin,xmax);
	 TH1F *fitHist_right = new TH1F(name_right, name_right, nbins, xmin,xmax);
	 
	 bin_scale = 1;
	 nbins = bin_scale*hpx->GetNbinsX(); binw = (xmax-xmin)/nbins; 
   	 fitHist_up->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fitHist_up->SetBinContent(nbins,hpx->GetBinContent(hpx->GetNbinsX())/bin_scale);
   	 fitHist_down->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fitHist_down->SetBinContent(nbins,hpx->GetBinContent(hpx->GetNbinsX())/bin_scale);
   	 fitHist_left->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fitHist_left->SetBinContent(nbins,hpx->GetBinContent(hpx->GetNbinsX())/bin_scale);
   	 fitHist_right->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fitHist_right->SetBinContent(nbins,hpx->GetBinContent(hpx->GetNbinsX())/bin_scale); 
	 for(int i = 2; i < nbins; i++){ 
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
	 name_str.append("_UD.png"); fitHist_up->Draw("same");fitHist_down->Draw("same");	
	 //name_str.append("_LR.png"); fitHist_left->Draw("same");fitHist_right->Draw("same");
	 char* title = const_cast<char*>(name_str.c_str());//converting string to char
	 can->SaveAs(title);
 }//while
 ofile->Close(); tablefile.close();
 return 0;
}
