#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
//------------------------------------------------------------------------
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

void new_lognormal(){//2gaus+expo tail
 TCanvas *can= new TCanvas("can","can",700,500);
 TFile *ifile0 = new TFile("templates-3chan-Theta-noFwd-Stop.root");
 TFile *ifile = new TFile("templates-3chan-Theta_rebin.root");
 TFile *ofile = new TFile("analytic_hist_3chan.root", "RECREATE");
 ofstream tablefile;
 tablefile.open ("table.txt");
 tablefile<<"HistName\t"<<"ChiSquare/NDF\t"<<"HistIntegral\t"<<"FitIntegral\t"<<"Ngaus"<<"\t"<<"Mean"<<"\t"<<"sigma"<<"\t"<<"sigma2"<<"\t"<<"lambda"<<"a"<<endl;
 int param_num = 6;
 double bin_scale = 1; 
 //--------------------------------------------------------
 TIter nextkey1(ifile->GetListOfKeys());
 TKey *key1;
 while ((key1 = (TKey*)nextkey1())){
	 TObject *obj1 = key1->ReadObj();
	 TH1F *hpx = (TH1F*)(obj1);
	 const char *name = hpx->GetName();
	 std::string s= name;
	 if (s.find("signal") < s.length() || s.find("DATA") < s.length()){//clone signal histograms
	   TH1F *clone1 = (TH1F*) hpx->Clone();
	   hpx->Write();
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
   
	 if (s.find("zjets") < s.length() || s.find("qcd") < s.length() || s.find("signal") < s.length() || s.find("data") < s.length() || s.find("DATA") < s.length() || s.find("stop") < s.length())//|| s.find("minus") < s.length()|| s.find("plus") < s.length())
	   continue;
	 cout<<name<<endl;
	 
  // if(s.find("wjets__pdf") > s.length()) continue;
	 
	 int xmin = hpx->GetXaxis()->GetXmin(); 
	 int xmax = hpx->GetXaxis()->GetXmax();
	 Int_t nbins = hpx->GetNbinsX();
	 int binw = (xmax-xmin)/nbins;

//-----------------------------------------------------

	 
	 TF1 *fitFn = new TF1("fit_new", "test_new", xmin+binw, xmax-binw, param_num);		  
/*		//finding stddev and mean for log(x) histogram
		TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
		for (int i = 0; i < nbins+2; i++){
		 int yaxis = hpx->GetBinContent(i);
		 double xaxis = std::log(xmin+i*binw);
		 h1->Fill(xaxis,yaxis);
		}*/
		
	 for (int i = 0; i<=nbins; i++){//remove negative bins
     if (hpx->GetBinContent(i) < 0)
   	   hpx->SetBinContent(i, 0);
   }
	 fitFn->SetParNames("Ngaus","Mean","sigma","sigma2","slope","n");
	 if(s.find("b1M__stop") < s.length()){
	 	 fitFn->SetParameters(10, 1200, 170, 0.35, 1.6,1.43);
	 }
	 if(s.find("b1M__top") < s.length()){
	 	 fitFn->SetParameters(15, 1100, 187, 0.29, .67,2.4);
	 }
	 if(s.find("b1M__wjets") < s.length()){
	 	 fitFn->SetParameters(86, 1200, 220, 0.3, -8,0.117);
	 }
	 if(s.find("b1T__stop") < s.length()){
	 	 fitFn->SetParameters(13, 1300, 240, 0.25, 2.3,2);
	 }
	 if(s.find("b1T__top") < s.length()){
	 	 fitFn->SetParameters(10, 1200, -219, 0.4, 1.7,0.45);
	 }
	 if(s.find("b1T__wjets") < s.length()){
	 	 fitFn->SetParameters(21, 1300, 230, 0.26, -1.2,-0.2);
	 }
	 if(s.find("b2M__stop") < s.length()){
	 	 fitFn->SetParameters(50, 1100, 170, 0.3, 12,5);
	 }
	 if(s.find("b2M__top") < s.length()){
	 	 fitFn->SetParameters(10, 1100, 170, 0.35, 1.4,2.5);
	 }
	 if(s.find("b2M__wjets") < s.length()){
	 	 fitFn->SetParameters(13, 1100, 170, 1.3, 1.5,0.15);
	 }	 
	 //fitFn->SetParNames("Ngaus","Mean","sigma","sigma2","lambda","a"); fitFn->SetParameters(10, 1000, 300, h1->GetStdDev(), 2,1/sqrt(1300));
   //fitFn->SetParNames("Ngaus","Mean","sigma","sigma2"); fitFn->SetParameters(42, 1100, 170,0.3);//,h1->GetStdDev());
   fitFn->SetLineColorAlpha(kBlack,0);fitFn->SetLineWidth(3);
	 hpx->Fit(fitFn,"R+");gStyle->SetOptFit(0000);
		 

	 TH1F *fit_hist = new TH1F(name, name, nbins, xmin, xmax); 
   fit_hist->SetBinContent(1,hpx->GetBinContent(1));
	 fit_hist->SetBinContent(nbins,hpx->GetBinContent(nbins));
	 for (int j = 2; j < nbins; j++){
			int xval = xmin+(j-.5)*binw;
			fit_hist->SetBinContent(j,fitFn->Eval(xval)); 
	 }
	 //fit_hist->Draw("same");
   tablefile<<name<<"\t"<<fitFn->GetChisquare()<<"/"<<fitFn->GetNDF()<<"\t"<<hpx->Integral()<<"\t"<<fitFn->Integral(xmin+binw,xmax-binw)/(binw)<<"\t";
   for(int i = 0; i < param_num; i++){
   	  tablefile<< fitFn->GetParameter(i) + fitFn->GetParError(i)<<"\t";
   }
   tablefile<<endl;
//----------------------------------------------------------------------  
	 nbins = bin_scale*hpx->GetNbinsX(); binw = (xmax-xmin)/nbins; 		 	 		
	 auto legend = new TLegend(.9, .9, .6, .6);legend->SetNColumns(2);
	 TF1 *fitFn_plus[param_num];
	 TF1 *fitFn_minus[param_num]; 
	 for(int i = 0; i < param_num; i++){
		  fitFn_plus[i] = new TF1("fitFn_plus", "test_new", xmin,xmax,param_num);
			fitFn_minus[i] = new TF1("fitFn_minus", "test_new", xmin,xmax,param_num);
			fitFn_plus[i]->SetParameters(fitFn->GetParameters());
			fitFn_minus[i]->SetParameters(fitFn->GetParameters());
			fitFn_plus[i]->SetParameter(i,fitFn->GetParameter(i) + fitFn->GetParError(i));
			fitFn_minus[i]->SetParameter(i,fitFn->GetParameter(i) - fitFn->GetParError(i));	
			fitFn_plus[i]->SetLineStyle(2);fitFn_plus[i]->SetLineColor(i+2);fitFn_plus[i]->Draw("same");
			fitFn_minus[i]->SetLineStyle(3);fitFn_minus[i]->SetLineColor(i+2);fitFn_minus[i]->Draw("same"); 
			std::string name_str_pl = fitFn->GetParName(i), name_str_mi = fitFn->GetParName(i);
      char* name_pl = const_cast<char*>(name_str_pl.append("__plus").c_str());
		 	char* name_mi = const_cast<char*>(name_str_mi.append("__minus").c_str());
		 	legend->AddEntry(fitFn_plus[i], name_pl, "l");
      legend->AddEntry(fitFn_minus[i], name_mi, "l");  
      	   
			if (s.find("plus") < s.length() || s.find("minus") < s.length())//only make param+- hist on nominal hist
			 	continue;
			 	std::string name_str_plus = name, name_str_minus = name;
			 	char* name_plus = const_cast<char*>(name_str_plus.append("__").append(fitFn->GetParName(i)).append("__plus").c_str());
		 	  char* name_minus = const_cast<char*>(name_str_minus.append("__").append(fitFn->GetParName(i)).append("__minus").c_str());	 
			 	TH1F *fit_hist_plus = new TH1F(name_plus, name_plus, nbins, xmin, xmax); 
			 	TH1F *fit_hist_minus = new TH1F(name_minus, name_minus, nbins, xmin, xmax); 
				fit_hist_plus->SetBinContent(1,hpx->GetBinContent(1)/bin_scale); 
				fit_hist_minus->SetBinContent(1,hpx->GetBinContent(1)/bin_scale); 	 		
				fit_hist_plus->SetBinContent(nbins,hpx->GetBinContent(nbins)/bin_scale); 
				fit_hist_minus->SetBinContent(nbins,hpx->GetBinContent(nbins)/bin_scale); 	
				for (int k = 2; k < nbins; k++){
					int xval = xmin+(k-.5)*binw;
					fit_hist_plus->SetBinContent(k,fitFn_plus[i]->Eval(xval)/bin_scale); 
				  fit_hist_minus->SetBinContent(k,fitFn_minus[i]->Eval(xval)/bin_scale); 
				}
				fit_hist_plus->Write(); fit_hist_minus->Write();
	 }				 		   
	 TH1F *fit_hist1 = new TH1F(name, name, nbins, xmin, xmax); 
	 fit_hist1->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fit_hist1->SetBinContent(nbins,hpx->GetBinContent(nbins/bin_scale));
	 for (int j = 2; j < nbins; j++){
			int xval = xmin+(j-.5)*binw;
			fit_hist1->SetBinContent(j,fitFn->Eval(xval)/bin_scale); 
	 }
	 fit_hist1->Write();
	 legend->Draw();
	 std::string name_str = name;
	 name_str.append(".png");
	 char* title = const_cast<char*>(name_str.c_str());//converting string to char
	 can->SaveAs(title);
 }//while	 
 
 //----------------------------------------------------------
 bin_scale = .5; 
 TIter nextkey0(ifile0->GetListOfKeys());
 TKey *key0;
 while ((key0 = (TKey*)nextkey0())){
   TObject *obj = key0->ReadObj();
   TH1F *hpx = (TH1F*)(obj);
   const char *name = hpx->GetName();
   std::string s = name;
   
	 if (s.find("stop") > s.length())
	   continue;
	 cout<<name<<endl;
	 
  // if(s.find("wjets__pdf") > s.length()) continue;
	 
	 int xmin = hpx->GetXaxis()->GetXmin(); 
	 int xmax = hpx->GetXaxis()->GetXmax();
	 Int_t nbins = hpx->GetNbinsX();
	 int binw = (xmax-xmin)/nbins;

//-----------------------------------------------------

	 
	 TF1 *fitFn = new TF1("fit_new", "test_new", xmin+binw, xmax-binw, param_num);		  
/*		//finding stddev and mean for log(x) histogram
		TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
		for (int i = 0; i < nbins+2; i++){
		 int yaxis = hpx->GetBinContent(i);
		 double xaxis = std::log(xmin+i*binw);
		 h1->Fill(xaxis,yaxis);
		}*/
		
	 for (int i = 0; i<=nbins; i++){//remove negative bins
     if (hpx->GetBinContent(i) < 0)
   	   hpx->SetBinContent(i, 0);
   }
	 fitFn->SetParNames("Ngaus","Mean","sigma","sigma2","slope","n");
	 if(s.find("b1M__stop") < s.length()){
	 	 fitFn->SetParameters(10, 1200, 170, 0.35, 1.6,1.43);
	 }
	 if(s.find("b1M__top") < s.length()){
	 	 fitFn->SetParameters(15, 1100, 187, 0.29, .67,2.4);
	 }
	 if(s.find("b1M__wjets") < s.length()){
	 	 fitFn->SetParameters(86, 1200, 220, 0.3, -8,0.117);
	 }
	 if(s.find("b1T__stop") < s.length()){
	 	 fitFn->SetParameters(13, 1300, 240, 0.25, 2.3,2);
	 }
	 if(s.find("b1T__top") < s.length()){
	 	 fitFn->SetParameters(10, 1200, -219, 0.4, 1.7,0.45);
	 }
	 if(s.find("b1T__wjets") < s.length()){
	 	 fitFn->SetParameters(21, 1300, 230, 0.26, -1.2,-0.2);
	 }
	 if(s.find("b2M__stop") < s.length()){
	 	 fitFn->SetParameters(50, 1100, 170, 0.3, 12,5);
	 }
	 if(s.find("b2M__top") < s.length()){
	 	 fitFn->SetParameters(10, 1100, 170, 0.35, 1.4,2.5);
	 }
	 if(s.find("b2M__wjets") < s.length()){
	 	 fitFn->SetParameters(13, 1100, 170, 1.3, 1.5,0.15);
	 }	 
	 //fitFn->SetParNames("Ngaus","Mean","sigma","sigma2","lambda","a"); fitFn->SetParameters(10, 1000, 300, h1->GetStdDev(), 2,1/sqrt(1300));
   //fitFn->SetParNames("Ngaus","Mean","sigma","sigma2"); fitFn->SetParameters(42, 1100, 170,0.3);//,h1->GetStdDev());
   fitFn->SetLineColorAlpha(kBlack,0);fitFn->SetLineWidth(3);
	 hpx->Fit(fitFn,"R+");gStyle->SetOptFit(0000);
		
	 TH1F *fit_hist = new TH1F(name, name, nbins, xmin, xmax); 
   fit_hist->SetBinContent(1,hpx->GetBinContent(1));
	 fit_hist->SetBinContent(nbins,hpx->GetBinContent(nbins));
	 for (int j = 2; j < nbins; j++){
			int xval = xmin+(j-.5)*binw;
			fit_hist->SetBinContent(j,fitFn->Eval(xval)); 
	 }
	 //fit_hist->Draw("same");
   tablefile<<name<<"\t"<<fitFn->GetChisquare()<<"/"<<fitFn->GetNDF()<<"\t"<<hpx->Integral()<<"\t"<<fitFn->Integral(xmin+binw,xmax-binw)/(binw)<<"\t";
   for(int i = 0; i < param_num; i++){
   	  tablefile<< fitFn->GetParameter(i) + fitFn->GetParError(i)<<"\t";
   }
   tablefile<<endl;
//----------------------------------------------------------------------  
	 nbins = bin_scale*hpx->GetNbinsX(); binw = (xmax-xmin)/nbins; 		 	 		
	 auto legend = new TLegend(.9, .9, .6, .6);legend->SetNColumns(2);
	 TF1 *fitFn_plus[param_num];
	 TF1 *fitFn_minus[param_num]; 
	 for(int i = 0; i < param_num; i++){
		  fitFn_plus[i] = new TF1("fitFn_plus", "test_new", xmin,xmax,param_num);
			fitFn_minus[i] = new TF1("fitFn_minus", "test_new", xmin,xmax,param_num);
			fitFn_plus[i]->SetParameters(fitFn->GetParameters());
			fitFn_minus[i]->SetParameters(fitFn->GetParameters());
			fitFn_plus[i]->SetParameter(i,fitFn->GetParameter(i) + fitFn->GetParError(i));
			fitFn_minus[i]->SetParameter(i,fitFn->GetParameter(i) - fitFn->GetParError(i));	
			fitFn_plus[i]->SetLineStyle(2);fitFn_plus[i]->SetLineColor(i+2);fitFn_plus[i]->Draw("same");
			fitFn_minus[i]->SetLineStyle(3);fitFn_minus[i]->SetLineColor(i+2);fitFn_minus[i]->Draw("same"); 
			std::string name_str_pl = fitFn->GetParName(i), name_str_mi = fitFn->GetParName(i);
      char* name_pl = const_cast<char*>(name_str_pl.append("__plus").c_str());
		 	char* name_mi = const_cast<char*>(name_str_mi.append("__minus").c_str());
		 	legend->AddEntry(fitFn_plus[i], name_pl, "l");
      legend->AddEntry(fitFn_minus[i], name_mi, "l");  
      	   
			if (s.find("plus") < s.length() || s.find("minus") < s.length())//only make param+- hist on nominal hist
			 	continue;
			 	std::string name_str_plus = name, name_str_minus = name;
			 	char* name_plus = const_cast<char*>(name_str_plus.append("__").append(fitFn->GetParName(i)).append("__plus").c_str());
		 	  char* name_minus = const_cast<char*>(name_str_minus.append("__").append(fitFn->GetParName(i)).append("__minus").c_str());	 
			 	TH1F *fit_hist_plus = new TH1F(name_plus, name_plus, nbins, xmin, xmax); 
			 	TH1F *fit_hist_minus = new TH1F(name_minus, name_minus, nbins, xmin, xmax); 
				fit_hist_plus->SetBinContent(1,hpx->GetBinContent(1)/bin_scale); 
				fit_hist_minus->SetBinContent(1,hpx->GetBinContent(1)/bin_scale); 	 		
				fit_hist_plus->SetBinContent(nbins,hpx->GetBinContent(nbins)/bin_scale); 
				fit_hist_minus->SetBinContent(nbins,hpx->GetBinContent(nbins)/bin_scale); 	
				for (int k = 2; k < nbins; k++){
					int xval = xmin+(k-.5)*binw;
					fit_hist_plus->SetBinContent(k,fitFn_plus[i]->Eval(xval)/bin_scale); 
				  fit_hist_minus->SetBinContent(k,fitFn_minus[i]->Eval(xval)/bin_scale); 
				}
				fit_hist_plus->Write(); fit_hist_minus->Write();
	 }				 		   
	 TH1F *fit_hist1 = new TH1F(name, name, nbins, xmin, xmax); 
	 fit_hist1->SetBinContent(1,hpx->GetBinContent(1)/bin_scale);
	 fit_hist1->SetBinContent(nbins,hpx->GetBinContent(nbins/bin_scale));
	 for (int j = 2; j < nbins; j++){
			int xval = xmin+(j-.5)*binw;
			fit_hist1->SetBinContent(j,fitFn->Eval(xval)/bin_scale); 
	 }
	 fit_hist1->Write();
	 legend->Draw();
	 std::string name_str = name;
	 name_str.append(".png");
	 char* title = const_cast<char*>(name_str.c_str());//converting string to char
	 can->SaveAs(title);
 }//while	 
} 
