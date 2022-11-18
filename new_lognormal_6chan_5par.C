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

void new_lognormal_6chan_5par(){//2gaus+expo tail
 TCanvas *can= new TCanvas("can","can",700,500); 
 TFile *ifile = new TFile("templates-3chan-HC-CorreectedNov16_wqcdy.root", "READ");
 TFile *ifile0 = new TFile("templates-3chan-HC-NoFwdJetR-CorreectedNov16_wqcdy.root", "READ");
 TFile *ifile2 = new TFile("templates-6chan-HC-CorrectedNov16_wqcdy.root", "READ"); 
 TFile *ofile;
 int param_num = 5;
 int bin_scale = 2;   
 ofile = new TFile("nuisance_hist_6chan_HC_CorrectedNov16_wqcdy_5par.root", "RECREATE");
 ofstream tablefile;
 tablefile.open ("table.txt");
 tablefile<<"HistName\t"<<"Chi^2/ndf\t"<<"Ngaus"<<"\t"<<"Mean"<<"\t"<<"sigma"<<"\t"<<"sigma2"<<"\t"<<"slope"<<"n"<<endl;


 //--------------------------------------------------------
 TIter nextkey2(ifile2->GetListOfKeys());
 TKey *key2;
 while ((key2 = (TKey*)nextkey2())){
	 TObject *obj1 = key2->ReadObj();
	 TH1F *hpx1 = (TH1F*)(obj1);
	 const char *name1 = hpx1->GetName();
	 std::string s1 = name1;
	 if (s1.find("data") < s1.length() || s1.find("signal") < s1.length()){//clone signal histograms
	   TH1F *clone1 = (TH1F*) hpx1->Clone();
	   hpx1->Write();
	 }
 }
//----------------------------------------------------------
 TIter nextkey(ifile->GetListOfKeys());
 TKey *key;
 while ((key = (TKey*)nextkey())){
   TObject *obj = key->ReadObj();
   TH1F *hpx = (TH1F*)(obj);TH1F *hpx2 = (TH1F*)hpx->Clone("hpx2");
   const char *name = hpx->GetName();
   std::string s = name;
   hpx->Rebin(2);
	 int xmin = hpx->GetXaxis()->GetXmin(); 
	 int xmax = hpx->GetXaxis()->GetXmax();
	 Int_t nbins = hpx->GetNbinsX();
	 int binw = (xmax-xmin)/nbins;
	 nbins = bin_scale*hpx->GetNbinsX(); binw = (xmax-xmin)/nbins;

   
	 if ( s.find("signal") < s.length()|| s.find("data") < s.length() || s.find("DATA") < s.length() || s.find("qcd") < s.length()|| s.find("stop") < s.length()) continue;
	 //if (s.find("wjets__b2M") > s.length()) continue;// || s.find("minus") > s.length())
	 //if (s.find("Up")<s.length()||s.find("Down") <s.length()) continue;
	 
         for (int i = 0; i<=nbins; i++){//remove negative bins or too huge bins
     	   if (hpx->GetBinContent(i) < 0 || hpx->GetBinContent(i) > 100){
     	     hpx->SetBinContent(i, 0); hpx->SetBinError(i, 0);}
         }

//-----------------------------------------------------
/*		//finding stddev and mean for log(x) histogram
		TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
		for (int i = 0; i < nbins+2; i++){
		 int yaxis = hpx->GetBinContent(i);
		 double xaxis = std::log(xmin+i*binw);
		 h1->Fill(xaxis,yaxis);
		}*/
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
	 hpx->Fit(fitFn,"R");//gStyle->SetOptFit(1111);
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
	 nbins = hpx2->GetNbinsX(); binw = (xmax-xmin)/nbins; //cout<<nbins<<endl;

	 
	 for(int i = 0; i < param_num; i++){
	 	tablefile<< fitFn->GetParameter(i) + fitFn->GetParError(i)<<"\t";
	 }
	 tablefile<<endl;
	 
	 TIter nextkey1(ifile2->GetListOfKeys());
	 TKey *key1;
	 while ((key1 = (TKey*)nextkey1())){//while2
		 TObject *obj1 = key1->ReadObj();
		 TH1F *hpx1 = (TH1F*)(obj1);
		 const char *name1 = hpx1->GetName();
		 std::string s1 = name1;
		 if (s1.find("DATA") < s1.length() || s1.find("signal") < s1.length()||s1.find("data") < s1.length() ||s1.find("qcd") < s1.length() ) continue; //everything except data and signal

	    		for (int i = 0; i<=nbins; i++){//remove negative bins
     		      if (hpx1->GetBinContent(i) < 0)
     	       	     hpx1->SetBinContent(i, 0);
    	      }
			 /*if(s1.find("wjets__pdf") < s1.length()){
				for (int i = 0; i<=nbins; i++){//remove too huge bins
					 if (hpx1->GetBinContent(i) > 100)
					 	 hpx1->SetBinContent(i, 0);
			 }}*/		 		
			 

		 int del1 = s.find_first_of("_"), del2 = s.find_last_of("_"), del3 = s1.find_last_of("_");
		 //cout<<del2<<"\t"<<del3<<endl;
		 if(del2 != del3 && del2+2 != del3) continue; //cout<< del2<<" == "<<del3<<endl;
		 if(s1.compare(0, del1+5, s,0,del1+5)!=0)continue;//checking channels are same
		 //cout<<name<<"\t"<<name1<<endl;
	 	 if(del1+1 != del2 && s1.compare(del2+3,s1.length()-del2-2, s,del2+1,s.length()-del2)!=0) continue;//check if sys are same
		 cout<<name<<"\t"<<name1<<endl;
		 
		 
			 nbins = hpx1->GetNbinsX(); binw = (xmax-xmin)/nbins; 
		    //cout<<name<<"\t"<<name1<<endl;
			 TF1 *fitFn1 = new TF1("fitFn1", "test_new", xmin+binw*bin_scale, xmax-binw*bin_scale,param_num);
			 double renorm = fitFn->GetParameter(0)*hpx1->Integral()*hpx->GetNbinsX()/(hpx->Integral()*hpx1->GetNbinsX());
			 fitFn1->FixParameter(0,renorm);//renormalising
			 fitFn1->FixParameter(1,fitFn->GetParameter(1));
			 fitFn1->FixParameter(2,fitFn->GetParameter(2));
			 fitFn1->FixParameter(3,fitFn->GetParameter(3));
			 fitFn1->FixParameter(4,fitFn->GetParameter(4));
			 //fitFn1->FixParameter(5,fitFn->GetParameter(5));

	 		 
	 		 bin_scale = 1;
			 TH1F *fit_hist1 = new TH1F(name1, name1, nbins, xmin, xmax); 
			 fit_hist1->SetBinContent(1,hpx1->GetBinContent(1));
			 fit_hist1->SetBinContent(2,hpx1->GetBinContent(2));
			 fit_hist1->SetBinContent(nbins-1, fitFn1->Eval(xmax-binw));
			 fit_hist1->SetBinContent(nbins, fitFn1->Eval(xmax)+fitFn1->Eval(xmax+1*binw)+fitFn1->Eval(xmax+2*binw)+fitFn1->Eval(xmax+3*binw)+fitFn1->Eval(xmax+4*binw)+fitFn1->Eval(xmax+5*binw)+fitFn1->Eval(xmax+6*binw)+fitFn1->Eval(xmax+7*binw)+fitFn1->Eval(xmax+8*binw)+fitFn1->Eval(xmax+9*binw)+fitFn1->Eval(xmax+10*binw));//extrapolating overflow bin
			 for (int j = 3; j < nbins-1; j++){
				int xval = xmin+(j-.5)*binw;
				fit_hist1->SetBinContent(j,fitFn1->Eval(xval)/bin_scale); 
			 }
			 fit_hist1->Write(); 
			 hpx1->Draw();fitFn1->Draw("same"); 
			 //tablefile<<name1<<"\t"<<"\t"<<hpx1->Integral()<<"\t"<<fit_hist1->Integral()<<endl;
			 
			 if (s1.find("Up") < s1.length() || s1.find("Down") < s1.length()) continue;
			 cout<<name<<"\t"<<name1<<endl;
			 TF1 *fitFn1_plus[param_num];
			 TF1 *fitFn1_minus[param_num];
			 for(int i = 0; i < param_num; i++){
				fitFn1_plus[i] = new TF1("fitFn1_plus", "test_new", xmin,xmax,param_num);
			 	fitFn1_minus[i] = new TF1("fitFn1_minus", "test_new", xmin,xmax,param_num);
				fitFn1_plus[i]->SetParameters(fitFn1->GetParameters());//fix parameters??
				fitFn1_minus[i]->SetParameters(fitFn1->GetParameters());
				if (i == 0){//for NGaus
					fitFn1_plus[i]->SetParameter(i,fitFn1->GetParameter(i) + fitFn->GetParError(i)*hpx1->Integral()/hpx->Integral());
					fitFn1_minus[i]->SetParameter(i,fitFn1->GetParameter(i) - fitFn->GetParError(i)*hpx1->Integral()/hpx->Integral());	
				}			  	
				else{
					fitFn1_plus[i]->SetParameter(i,fitFn1->GetParameter(i) + fitFn->GetParError(i));
					fitFn1_minus[i]->SetParameter(i,fitFn1->GetParameter(i) - fitFn->GetParError(i));

			   	//fitFn1_plus[i]->SetLineStyle(2);fitFn1_plus[i]->SetLineColor(i+2);fitFn1_plus[i]->Draw("same");	
					//fitFn1_minus[i]->SetLineStyle(1);fitFn1_minus[i]->SetLineColor(i+2);fitFn1_minus[i]->Draw("same");	
				}
			 }

			 std::string name_str_up = name1, name_str_down = name1, name_str_left = name1,name_str_right = name1;
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
				 
		    fitHist_up->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale);
			 fitHist_up->SetBinContent(nbins,fit_hist1->GetBinContent(nbins));
		    fitHist_down->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale);
		    fitHist_down->SetBinContent(nbins,fit_hist1->GetBinContent(nbins));
		    fitHist_left->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale);
			 fitHist_left->SetBinContent(nbins,fit_hist1->GetBinContent(nbins));
		    fitHist_right->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale);
			 fitHist_right->SetBinContent(nbins,fit_hist1->GetBinContent(nbins)); 
			 for(int i = 2; i < nbins; i++){ 
				//-----------------------------
			 	double xval = xmin+(i-0.5)*binw; double yval = fitFn1->GetMaximum()*(i-0.5)/nbins; 
			 	/*fitHist_up->SetBinContent(i, std::max({fitFn1->Eval(xval), fitFn1_plus[1]->Eval(xval),fitFn1_plus[2]->Eval(xval),fitFn1_plus[3]->Eval(xval),fitFn1_plus[4]->Eval(xval),fitFn1_plus[5]->Eval(xval),fitFn1_minus[1]->Eval(xval),fitFn1_minus[2]->Eval(xval),fitFn1_minus[3]->Eval(xval),fitFn1_minus[4]->Eval(xval),fitFn1_minus[5]->Eval(xval)}));
			 	fitHist_down->SetBinContent(i, std::min({fitFn1->Eval(xval), fitFn1_plus[1]->Eval(xval),fitFn1_plus[2]->Eval(xval),fitFn1_plus[3]->Eval(xval),fitFn1_plus[4]->Eval(xval),fitFn1_plus[5]->Eval(xval),fitFn1_minus[1]->Eval(xval),fitFn1_minus[2]->Eval(xval),fitFn1_minus[3]->Eval(xval),fitFn1_minus[4]->Eval(xval),fitFn1_minus[5]->Eval(xval)}));*/
				double fitFn_array_UD[(param_num-1)*2+1]; fitFn_array_UD[0] = fitFn1->Eval(xval);
				for (int j = 1; j <= (param_num-1); j++){
					fitFn_array_UD[j] = fitFn1_plus[j]->Eval(xval);
					fitFn_array_UD[(param_num-1)+j] = fitFn1_minus[j]->Eval(xval);
				}
				//Sort the array in ascending order    
				sort(fitFn_array_UD, fitFn_array_UD + sizeof(fitFn_array_UD)/sizeof(fitFn_array_UD[0]));
				
				fitHist_up->SetBinContent(i, fitFn_array_UD[(param_num-1)*2]/bin_scale);
				fitHist_down->SetBinContent(i, fitFn_array_UD[0]/bin_scale);
				/*if(fitFn_array_UD[(param_num-1)*2] < 0.01)//picks the 2nd largest
				    fitHist_up->SetBinContent(i, fitFn_array_UD[(param_num-1)*2-1]/bin_scale);
				//if(fitFn_array_UD[0] < 0.01)
				if(abs(fitFn1->GetParError(5)/fitFn1->GetParameter(5)) > 0.4 || abs(fitFn1->GetParError(5)/fitFn1->GetParameter(5)) > 0.4  )//picks the 2nd smallest and eliminates the problematic sigma2 and slope fns 
				    fitHist_down->SetBinContent(i, fitFn_array_UD[1]/bin_scale);*/    					
				//-----------------------------
				double fitFn_array_LR[(param_num-2)*2+1]; fitFn_array_LR[0] = fitFn1->Eval(xval);
				for (int j = 1; j <= (param_num-2); j++){
					fitFn_array_LR[j] = fitFn1_plus[j+1]->Eval(xval);//j+1 so as to not use mean
					fitFn_array_LR[(param_num-2)+j] = fitFn1_minus[j+1]->Eval(xval);
				}
				//Sort the array in ascending order
				sort(fitFn_array_LR, fitFn_array_LR + sizeof(fitFn_array_LR)/sizeof(fitFn_array_LR[0]));
				
				double fitMean_left = fitFn1->GetParameter(1),fitMean_right = fitFn1->GetParameter(1);//will give same as using min max from param_num=2++
			 	if(xval <= fitMean_left){
				 	fitHist_left->SetBinContent(i, fitFn_array_LR[(param_num-2)*2]/bin_scale);
				 	/*if(fitFn_array_LR[(param_num-2)*2] < 0.01)
					    fitHist_left->SetBinContent(i, fitFn_array_LR[(param_num-2)*2-1]/bin_scale);*/
				}
				else if(xval > fitMean_left){
				 	fitHist_left->SetBinContent(i, fitFn_array_LR[0]/bin_scale);
					/*if(fitFn_array_LR[0] < 0.01)
					    fitHist_left->SetBinContent(i, fitFn_array_LR[1]/bin_scale);*/
				}
			 	if(xval <= fitMean_right){
					fitHist_right->SetBinContent(i, fitFn_array_LR[0]/bin_scale);
				 	/*if(fitFn_array_LR[0] < 0.01)
					    fitHist_right->SetBinContent(i, fitFn_array_LR[1]/bin_scale);*/
				}
				else if(xval > fitMean_right){
				 	fitHist_right->SetBinContent(i, fitFn_array_LR[(param_num-2)*2]/bin_scale);
				 	/*if(fitFn_array_LR[(param_num-2)*2] < 0.01)
					    fitHist_right->SetBinContent(i, fitFn_array_LR[(param_num-2)*2-1]/bin_scale);*/
				}
				fitHist_left->Draw("same");fitHist_right->Draw("same");
				//cout<< std::min({fitFn1->Eval(xval),fitFn1_plus[1]->Eval(xval),fitFn1_minus[1]->Eval(xval),fitFn1_plus[2]->Eval(xval),fitFn1_plus[3]->Eval(xval),fitFn1_plus[4]->Eval(xval),fitFn1_plus[5]->Eval(xval),fitFn1_minus[2]->Eval(xval),fitFn1_minus[3]->Eval(xval),fitFn1_minus[4]->Eval(xval),fitFn1_minus[5]->Eval(xval)})<<"\t"<<  std::max({fitFn1->Eval(xval),fitFn1_plus[1]->Eval(xval),fitFn1_minus[1]->Eval(xval),fitFn1_plus[2]->Eval(xval),fitFn1_plus[3]->Eval(xval),fitFn1_plus[4]->Eval(xval),fitFn1_plus[5]->Eval(xval),fitFn1_minus[2]->Eval(xval),fitFn1_minus[3]->Eval(xval),fitFn1_minus[4]->Eval(xval),fitFn1_minus[5]->Eval(xval)})<<"\t"<<fitFn_array_UD[0]<<"\t"<<fitFn_array_UD[(param_num-1)*2]<<endl;		
			 }
				 
			 fitHist_up->Write();fitHist_down->Write();fitHist_left->Write();fitHist_right->Write();
			 fitHist_up->SetLineColor(4);fitHist_down->SetLineColor(4);fitHist_left->SetLineColor(2);fitHist_right->SetLineColor(2);
			 std::string name_str = name; 	 	
			 //name_str.append("_UD.png"); fitHist_up->Draw("same");fitHist_down->Draw("same");	
			 //name_str.append("_LR.png"); fitHist_left->Draw("same");fitHist_right->Draw("same");
			 char* title = const_cast<char*>(name_str.c_str());//converting string to char
			 can->SaveAs(title);
	 }//while2
 }//while	 
 //========================================================================================================================================
 TIter nextkey0(ifile0->GetListOfKeys());
 TKey *key0;
 while ((key0 = (TKey*)nextkey0())){
   TObject *obj0 = key0->ReadObj();
   TH1F *hpx = (TH1F*)(obj0);TH1F *hpx2 = (TH1F*)hpx->Clone("hpx2");
   const char *name = hpx->GetName();
   std::string s = name;
   hpx->Rebin(2);
	 int xmin = hpx->GetXaxis()->GetXmin(); 
	 int xmax = hpx->GetXaxis()->GetXmax();
	 Int_t nbins = hpx->GetNbinsX();
	 int binw = (xmax-xmin)/nbins;
	 nbins = bin_scale*hpx->GetNbinsX(); binw = (xmax-xmin)/nbins;

	 if (s.find("stop") > s.length()) continue;// || s.find("minus") > s.length())
	 //if (s.find("Up")<s.length()||s.find("Down") <s.length()) continue;
	 
         for (int i = 0; i<=nbins; i++){//remove negative bins or too huge bins
     	   if (hpx->GetBinContent(i) < 0 || hpx->GetBinContent(i) > 100){
     	     hpx->SetBinContent(i, 0); hpx->SetBinError(i, 0);}
         }

//-----------------------------------------------------
/*		//finding stddev and mean for log(x) histogram
		TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
		for (int i = 0; i < nbins+2; i++){
		 int yaxis = hpx->GetBinContent(i);
		 double xaxis = std::log(xmin+i*binw);
		 h1->Fill(xaxis,yaxis);
		}*/
	 TF1 *fitFn = new TF1("fit_new", "test_new", xmin+binw, xmax-binw, param_num);		  
	 fitFn->SetParNames("Ngaus","Mean","sigma","slope","n");

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
	 hpx->Fit(fitFn,"R");//gStyle->SetOptFit(1111);
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
	 nbins = hpx2->GetNbinsX(); binw = (xmax-xmin)/nbins; //cout<<nbins<<endl;

	 
	 for(int i = 0; i < param_num; i++){
	 	tablefile<< fitFn->GetParameter(i) + fitFn->GetParError(i)<<"\t";
	 }
	 tablefile<<endl;
	 
	 TIter nextkey1(ifile2->GetListOfKeys());
	 TKey *key1;
	 while ((key1 = (TKey*)nextkey1())){//while2
		 TObject *obj1 = key1->ReadObj();
		 TH1F *hpx1 = (TH1F*)(obj1);
		 const char *name1 = hpx1->GetName();
		 std::string s1 = name1;
		 if (s1.find("DATA") < s1.length() || s1.find("signal") < s1.length()||s1.find("data") < s1.length() ||s1.find("qcd") < s1.length() ) continue; //everything except data and signal

	    		for (int i = 0; i<=nbins; i++){//remove negative bins
     		      if (hpx1->GetBinContent(i) < 0)
     	       	     hpx1->SetBinContent(i, 0);
    	      }
			 /*if(s1.find("wjets__pdf") < s1.length()){
				for (int i = 0; i<=nbins; i++){//remove too huge bins
					 if (hpx1->GetBinContent(i) > 100)
					 	 hpx1->SetBinContent(i, 0);
			 }}*/		 		
			 

		 int del1 = s.find_first_of("_"), del2 = s.find_last_of("_"), del3 = s1.find_last_of("_");
		 //cout<<del2<<"\t"<<del3<<endl;
		 if(del2 != del3 && del2+2 != del3) continue; //cout<< del2<<" == "<<del3<<endl;
		 if(s1.compare(0, del1+5, s,0,del1+5)!=0)continue;//checking channels are same
		 //cout<<name<<"\t"<<name1<<endl;
	 	 if(del1+1 != del2 && s1.compare(del2+3,s1.length()-del2-2, s,del2+1,s.length()-del2)!=0) continue;//check if sys are same
		 cout<<name<<"\t"<<name1<<endl;
		 
		 
			 nbins = hpx1->GetNbinsX(); binw = (xmax-xmin)/nbins; 
		    //cout<<name<<"\t"<<name1<<endl;
			 TF1 *fitFn1 = new TF1("fitFn1", "test_new", xmin+binw*bin_scale, xmax-binw*bin_scale,param_num);
			 double renorm = fitFn->GetParameter(0)*hpx1->Integral()*hpx->GetNbinsX()/(hpx->Integral()*hpx1->GetNbinsX());
			 fitFn1->FixParameter(0,renorm);//renormalising
			 fitFn1->FixParameter(1,fitFn->GetParameter(1));
			 fitFn1->FixParameter(2,fitFn->GetParameter(2));
			 fitFn1->FixParameter(3,fitFn->GetParameter(3));
			 fitFn1->FixParameter(4,fitFn->GetParameter(4));
			 //fitFn1->FixParameter(5,fitFn->GetParameter(5));

	 		 
	 		 bin_scale = 1;
			 TH1F *fit_hist1 = new TH1F(name1, name1, nbins, xmin, xmax); 
			 fit_hist1->SetBinContent(1,hpx1->GetBinContent(1));
			 fit_hist1->SetBinContent(2,hpx1->GetBinContent(2));
			 fit_hist1->SetBinContent(nbins-1, fitFn1->Eval(xmax-binw));
			 fit_hist1->SetBinContent(nbins, fitFn1->Eval(xmax)+fitFn1->Eval(xmax+1*binw)+fitFn1->Eval(xmax+2*binw)+fitFn1->Eval(xmax+3*binw)+fitFn1->Eval(xmax+4*binw)+fitFn1->Eval(xmax+5*binw)+fitFn1->Eval(xmax+6*binw)+fitFn1->Eval(xmax+7*binw)+fitFn1->Eval(xmax+8*binw)+fitFn1->Eval(xmax+9*binw)+fitFn1->Eval(xmax+10*binw));//extrapolating overflow bin
			 for (int j = 3; j < nbins-1; j++){
				int xval = xmin+(j-.5)*binw;
				fit_hist1->SetBinContent(j,fitFn1->Eval(xval)/bin_scale); 
			 }
			 fit_hist1->Write(); 
			 hpx1->Draw();fitFn1->Draw("same"); 
			 //tablefile<<name1<<"\t"<<"\t"<<hpx1->Integral()<<"\t"<<fit_hist1->Integral()<<endl;
			 
			 if (s1.find("Up") < s1.length() || s1.find("Down") < s1.length()) continue;
			 cout<<name<<"\t"<<name1<<endl;
			 TF1 *fitFn1_plus[param_num];
			 TF1 *fitFn1_minus[param_num];
			 for(int i = 0; i < param_num; i++){
				fitFn1_plus[i] = new TF1("fitFn1_plus", "test_new", xmin,xmax,param_num);
			 	fitFn1_minus[i] = new TF1("fitFn1_minus", "test_new", xmin,xmax,param_num);
				fitFn1_plus[i]->SetParameters(fitFn1->GetParameters());//fix parameters??
				fitFn1_minus[i]->SetParameters(fitFn1->GetParameters());
				if (i == 0){//for NGaus
					fitFn1_plus[i]->SetParameter(i,fitFn1->GetParameter(i) + fitFn->GetParError(i)*hpx1->Integral()/hpx->Integral());
					fitFn1_minus[i]->SetParameter(i,fitFn1->GetParameter(i) - fitFn->GetParError(i)*hpx1->Integral()/hpx->Integral());	
				}			  	
				else{
					fitFn1_plus[i]->SetParameter(i,fitFn1->GetParameter(i) + fitFn->GetParError(i));
					fitFn1_minus[i]->SetParameter(i,fitFn1->GetParameter(i) - fitFn->GetParError(i));

			   	//fitFn1_plus[i]->SetLineStyle(2);fitFn1_plus[i]->SetLineColor(i+2);fitFn1_plus[i]->Draw("same");	
					//fitFn1_minus[i]->SetLineStyle(1);fitFn1_minus[i]->SetLineColor(i+2);fitFn1_minus[i]->Draw("same");	
				}
			 }

			 std::string name_str_up = name1, name_str_down = name1, name_str_left = name1,name_str_right = name1;
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
				 
		    fitHist_up->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale);
			 fitHist_up->SetBinContent(nbins,fit_hist1->GetBinContent(nbins));
		    fitHist_down->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale);
		    fitHist_down->SetBinContent(nbins,fit_hist1->GetBinContent(nbins));
		    fitHist_left->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale);
			 fitHist_left->SetBinContent(nbins,fit_hist1->GetBinContent(nbins));
		    fitHist_right->SetBinContent(1,hpx1->GetBinContent(1)/bin_scale);
			 fitHist_right->SetBinContent(nbins,fit_hist1->GetBinContent(nbins)); 
			 for(int i = 2; i < nbins; i++){ 
				//-----------------------------
			 	double xval = xmin+(i-0.5)*binw; double yval = fitFn1->GetMaximum()*(i-0.5)/nbins; 
			 	/*fitHist_up->SetBinContent(i, std::max({fitFn1->Eval(xval), fitFn1_plus[1]->Eval(xval),fitFn1_plus[2]->Eval(xval),fitFn1_plus[3]->Eval(xval),fitFn1_plus[4]->Eval(xval),fitFn1_plus[5]->Eval(xval),fitFn1_minus[1]->Eval(xval),fitFn1_minus[2]->Eval(xval),fitFn1_minus[3]->Eval(xval),fitFn1_minus[4]->Eval(xval),fitFn1_minus[5]->Eval(xval)}));
			 	fitHist_down->SetBinContent(i, std::min({fitFn1->Eval(xval), fitFn1_plus[1]->Eval(xval),fitFn1_plus[2]->Eval(xval),fitFn1_plus[3]->Eval(xval),fitFn1_plus[4]->Eval(xval),fitFn1_plus[5]->Eval(xval),fitFn1_minus[1]->Eval(xval),fitFn1_minus[2]->Eval(xval),fitFn1_minus[3]->Eval(xval),fitFn1_minus[4]->Eval(xval),fitFn1_minus[5]->Eval(xval)}));*/
				double fitFn_array_UD[(param_num-1)*2+1]; fitFn_array_UD[0] = fitFn1->Eval(xval);
				for (int j = 1; j <= (param_num-1); j++){
					fitFn_array_UD[j] = fitFn1_plus[j]->Eval(xval);
					fitFn_array_UD[(param_num-1)+j] = fitFn1_minus[j]->Eval(xval);
				}
				//Sort the array in ascending order    
				sort(fitFn_array_UD, fitFn_array_UD + sizeof(fitFn_array_UD)/sizeof(fitFn_array_UD[0]));
				
				fitHist_up->SetBinContent(i, fitFn_array_UD[(param_num-1)*2]/bin_scale);
				fitHist_down->SetBinContent(i, fitFn_array_UD[0]/bin_scale);
				/*if(fitFn_array_UD[(param_num-1)*2] < 0.01)//picks the 2nd largest
				    fitHist_up->SetBinContent(i, fitFn_array_UD[(param_num-1)*2-1]/bin_scale);
				//if(fitFn_array_UD[0] < 0.01)
				if(abs(fitFn1->GetParError(5)/fitFn1->GetParameter(5)) > 0.4 || abs(fitFn1->GetParError(5)/fitFn1->GetParameter(5)) > 0.4  )//picks the 2nd smallest and eliminates the problematic sigma2 and slope fns 
				    fitHist_down->SetBinContent(i, fitFn_array_UD[1]/bin_scale);*/    					
				//-----------------------------
				double fitFn_array_LR[(param_num-2)*2+1]; fitFn_array_LR[0] = fitFn1->Eval(xval);
				for (int j = 1; j <= (param_num-2); j++){
					fitFn_array_LR[j] = fitFn1_plus[j+1]->Eval(xval);//j+1 so as to not use mean
					fitFn_array_LR[(param_num-2)+j] = fitFn1_minus[j+1]->Eval(xval);
				}
				//Sort the array in ascending order
				sort(fitFn_array_LR, fitFn_array_LR + sizeof(fitFn_array_LR)/sizeof(fitFn_array_LR[0]));
				
				double fitMean_left = fitFn1->GetParameter(1),fitMean_right = fitFn1->GetParameter(1);//will give same as using min max from param_num=2++
			 	if(xval <= fitMean_left){
				 	fitHist_left->SetBinContent(i, fitFn_array_LR[(param_num-2)*2]/bin_scale);
				 	/*if(fitFn_array_LR[(param_num-2)*2] < 0.01)
					    fitHist_left->SetBinContent(i, fitFn_array_LR[(param_num-2)*2-1]/bin_scale);*/
				}
				else if(xval > fitMean_left){
				 	fitHist_left->SetBinContent(i, fitFn_array_LR[0]/bin_scale);
					/*if(fitFn_array_LR[0] < 0.01)
					    fitHist_left->SetBinContent(i, fitFn_array_LR[1]/bin_scale);*/
				}
			 	if(xval <= fitMean_right){
					fitHist_right->SetBinContent(i, fitFn_array_LR[0]/bin_scale);
				 	/*if(fitFn_array_LR[0] < 0.01)
					    fitHist_right->SetBinContent(i, fitFn_array_LR[1]/bin_scale);*/
				}
				else if(xval > fitMean_right){
				 	fitHist_right->SetBinContent(i, fitFn_array_LR[(param_num-2)*2]/bin_scale);
				 	/*if(fitFn_array_LR[(param_num-2)*2] < 0.01)
					    fitHist_right->SetBinContent(i, fitFn_array_LR[(param_num-2)*2-1]/bin_scale);*/
				}
				fitHist_left->Draw("same");fitHist_right->Draw("same");
				//cout<< std::min({fitFn1->Eval(xval),fitFn1_plus[1]->Eval(xval),fitFn1_minus[1]->Eval(xval),fitFn1_plus[2]->Eval(xval),fitFn1_plus[3]->Eval(xval),fitFn1_plus[4]->Eval(xval),fitFn1_plus[5]->Eval(xval),fitFn1_minus[2]->Eval(xval),fitFn1_minus[3]->Eval(xval),fitFn1_minus[4]->Eval(xval),fitFn1_minus[5]->Eval(xval)})<<"\t"<<  std::max({fitFn1->Eval(xval),fitFn1_plus[1]->Eval(xval),fitFn1_minus[1]->Eval(xval),fitFn1_plus[2]->Eval(xval),fitFn1_plus[3]->Eval(xval),fitFn1_plus[4]->Eval(xval),fitFn1_plus[5]->Eval(xval),fitFn1_minus[2]->Eval(xval),fitFn1_minus[3]->Eval(xval),fitFn1_minus[4]->Eval(xval),fitFn1_minus[5]->Eval(xval)})<<"\t"<<fitFn_array_UD[0]<<"\t"<<fitFn_array_UD[(param_num-1)*2]<<endl;		
			 }
				 
			 fitHist_up->Write();fitHist_down->Write();fitHist_left->Write();fitHist_right->Write();
			 fitHist_up->SetLineColor(4);fitHist_down->SetLineColor(4);fitHist_left->SetLineColor(2);fitHist_right->SetLineColor(2);
			 std::string name_str = name; 	 	
			 //name_str.append("_UD.png"); fitHist_up->Draw("same");fitHist_down->Draw("same");	
			 //name_str.append("_LR.png"); fitHist_left->Draw("same");fitHist_right->Draw("same");
			 char* title = const_cast<char*>(name_str.c_str());//converting string to char
			 can->SaveAs(title);
	 }//while2
 }//while	  
 ofile->Close(); tablefile.close();
 return 0;
}
