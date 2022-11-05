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
    return constant * std::exp(-tmp *tmp*a/2)/(-(x-x0)*std::fabs(sigma) * std::sqrt(2 * M_PI));//40000-top, 100000-wjet, 51000-stop, 1129-zjets
  }
}

double signal_fit(double *x, double *par){//3 Gaussians stitched
	// Define parameters
	double N1 = par[0]; // First Gaussian
	double m1 = par[1]; // First Gaussian
	double s1 = par[2]; // First Gaussian
	double s2 = par[3]; // Second Gaussian
	double s3 = par[4]; // Third Gaussian
	double k = par[5]; // Determines Gaussian between one and two
	double l = par[6]; // Determines Gaussian between two and three
	//create functions
		double m2 = k-(k-m1)*pow(s2/s1,2);
		double N2 = N1*exp(-.5*pow(((k - m1)/s1),2)+0.5*pow(((k - m2)/s2),2));
		double m3 = l-(l-m2)*pow(s3/s2,2);
		double N3 = N2*exp(-.5*pow(((l - m2)/s2),2)+0.5*pow(((l - m3)/s3),2));
	if(x[0] <= k)
		return N1 * std::exp(-.5 * std::pow(((x[0] - m1)/s1),2));
	else if(x[0] >k && x[0] <= l){
		return N2 * std::exp(-.5 * std::pow(((x[0] - m2)/s2),2));
	}	
	else if(x[0] > l){
		return N3 * std::exp(-.5 * std::pow(((x[0] - m3)/s3),2));
	}
	else
		return 0;
}

/*double signal_fit(double *x, double *par){//2 Gaus and lognormal stitched
	// Define parameters
	double N1 = par[0]; // First Gaussian
	double m1 = par[1]; // First Gaussian
	double s1 = par[2]; // First Gaussian
	double s2 = par[3]; // Second Gaussian
	double k = par[4]; // Determines Gaussian between one and two
	double d = par[5]; // stitching point for lognormal and gaus
	double sigma = par[6]; 
	double a = par[7]; 
	//create functions
	 // double mean = k+abs(d);
		double m2 = k-(k-m1)*pow(s2/s1,2);
		double N2 = N1*exp(-.5*pow(((k - m1)/s1),2)+0.5*pow(((k - m2)/s2),2));

	if(x[0] <= k)
		return N1 * std::exp(-.5 * std::pow(((x[0] - m1)/s1),2));
	else if(x[0] >k && x[0] <= m2){
		return N2 * std::exp(-.5 * std::pow(((x[0] - m2)/s2),2));
	}	
	else if(x[0] > m2){
		double tmp = std::log(x[0]/m2)/sigma;
		return N2*std::exp(-tmp*tmp*a/2)/(x[0]/m2);
	}
	else
		return 0;
}*/

/*double signal_fit(double *x, double *par){//sum of 3 Gaussian
	// Define parameters
	double N1 = par[0]; // First Gaussian
	double m1 = par[1]; // First Gaussian
	double s1 = par[2]; // First Gaussian
	double N2 = par[3]; // Second Gaussian
	double m2 = par[4]; // Second Gaussian
	double s2 = par[5]; // Second Gaussian
	double N3 = par[6]; // Third Gaussian
	double m3 = par[7]; // Third Gaussian
	double s3 = par[8]; // Third Gaussian
	//create functions
	return N1*std::exp(-.5*std::pow(((x[0]-m1)/s1),2)) + N2*std::exp(-.5*std::pow(((x[0]-m2)/s2),2)) + N3*std::exp(-.5*std::pow(((x[0]-m3)/s3),2));
}*/

/*double signal_fit(double *x, double *par){//Gaus+lognormal
	// Define parameters
	double N1 = par[0]; // First Gaussian
	double m1 = par[1]; // First Gaussian
	double s1 = par[2]; // First Gaussian
	double Const = par[3]; 
	double mean = par[4];
	double sigma = par[5]; 
	double a = par[6]; 
	//create functions
  double tmp = (std::log(x[0]) - mean)/sigma;
	return N1*std::exp(-.5*std::pow(((x[0] - m1)/s1),2)) + Const*std::exp(-tmp*tmp*a/2)/(-x[0]*std::fabs(sigma)*std::sqrt(2*M_PI));
}*/
/*double signal_fit(double *x, double *par, double t){//2 Gaus and lognormal rigid stitch
	// Define parameters
	double N1 = par[0]; // First Gaussian
	double m1 = par[1]; // First Gaussian
	double s1 = par[2]; // First Gaussian
	double s2 = par[3]; // Second Gaussian
	double k = par[4]; // Determines Gaussian between one and two
	double sigma = par[5]; 
	double a = par[6]; 
	//create functions
		double m2 = k-(k-m1)*pow(s2/s1,2);
		double N2 = N1*exp(-.5*pow(((k - m1)/s1),2)+0.5*pow(((k - m2)/s2),2));
	if(x[0] <= k)
		return N1 * std::exp(-.5 * std::pow(((x[0] - m1)/s1),2));
	else if(x[0] >k && x[0] <= t){
		return N2 * std::exp(-.5 * std::pow(((x[0] - m2)/s2),2));
	}	
	else if(x[0] > t){
		double tmp = std::log(x[0]/m2)/sigma;
		return N2*std::exp(-tmp*tmp*a/2)/(x[0]/m2);
	}
	else
		return 0;
}*/
/*
double signal_fit(double *var, double *par){  
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
}*/
/*double signal_fit(double *x, double *par){//3 Gaussians stitched + gaus
	// Define parameters
	double N1 = par[0]; // First Gaussian
	double m1 = par[1]; // First Gaussian
	double s1 = par[2]; // First Gaussian
	double s2 = par[3]; // Second Gaussian
	double s3 = par[4]; // Third Gaussian
	double k = par[5]; // Determines Gaussian between one and two
	double l = par[6]; // Determines Gaussian between two and three
	double Ngaus = par[7], mean = par[8], sigma = par[9];
	//create functions
		double m2 = k-(k-m1)*pow(s2/s1,2);
		double N2 = N1*exp(-.5*pow(((k - m1)/s1),2)+0.5*pow(((k - m2)/s2),2));
		double m3 = l-(l-m2)*pow(s3/s2,2);
		double N3 = N2*exp(-.5*pow(((l - m2)/s2),2)+0.5*pow(((l - m3)/s3),2));
	if(x[0] <= k)
		return N1 * std::exp(-.5 * std::pow(((x[0] - m1)/s1),2)) + Ngaus * std::exp(-.5 * std::pow(((x[0] - mean)/sigma),2));
	else if(x[0] >k && x[0] <= l){
		return N2 * std::exp(-.5 * std::pow(((x[0] - m2)/s2),2)) + Ngaus * std::exp(-.5 * std::pow(((x[0] - mean)/sigma),2));
	}	
	else if(x[0] > l){
		return N3 * std::exp(-.5 * std::pow(((x[0] - m3)/s3),2)) + Ngaus * std::exp(-.5 * std::pow(((x[0] - mean)/sigma),2));
	}
	else
		return 0;
}*/
/*double signal_fit(double *x, double *par){//3 Gaussians stitched + LN
	// Define parameters
	double N1 = par[0]; // First Gaussian
	double m1 = par[1]; // First Gaussian
	double s1 = par[2]; // First Gaussian
	double s2 = par[3]; // Second Gaussian
	double s3 = par[4]; // Third Gaussian
	double k = par[5]; // Determines Gaussian between one and two
	double l = par[6]; // Determines Gaussian between two and three
	double sigma = par[7], mean= par[8], constant = par[9], a =par[10];
	//create functions
		double m2 = k-(k-m1)*pow(s2/s1,2);
		double N2 = N1*exp(-.5*pow(((k - m1)/s1),2)+0.5*pow(((k - m2)/s2),2));
		double m3 = l-(l-m2)*pow(s3/s2,2);
		double N3 = N2*exp(-.5*pow(((l - m2)/s2),2)+0.5*pow(((l - m3)/s3),2));
		double tmp = (std::log(x[0]) - mean)/sigma;

	if(x[0] <= k)
		return N1 * std::exp(-.5 * std::pow(((x[0] - m1)/s1),2)) + constant * std::exp(-tmp *tmp*a/2)/(-(x[0])*std::fabs(sigma) * std::sqrt(2 * M_PI));
	else if(x[0] >k && x[0] <= l)
		return N2 * std::exp(-.5 * std::pow(((x[0] - m2)/s2),2)) + constant * std::exp(-tmp *tmp*a/2)/(-(x[0])*std::fabs(sigma) * std::sqrt(2 * M_PI));
	else if(x[0] > l)
		return N3 * std::exp(-.5 * std::pow(((x[0] - m3)/s3),2)) + constant * std::exp(-tmp *tmp*a/2)/(-(x[0])*std::fabs(sigma) * std::sqrt(2 * M_PI));
	else
		return 0;
}*/
//--------------------------------------------------------------------------

void sig(){
 TCanvas *can= new TCanvas("can","can",700,500);
 TFile *ifile = new TFile("templates-3chan-Theta_rebin.root");
 TFile *ofile = new TFile("analytic_hist_3chan_sig.root", "RECREATE");
 ofstream tablefile;
 tablefile.open ("table_3gaus.txt");
 tablefile<<"HistName\t"<<"ChiSquare/NDF\t"<<"HistIntegral\t"<<"FitIntegral\t"<<"Ngaus"<<"\t"<<"Mean"<<"\t"<<"sigma"<<"\t"<<"sigma2"<<"\t"<<"lambda"<<"a"<<endl;
 int param_num = 7;
 int bin_scale = 1; 

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
	 if ( s.find("b1T__signal") > s.length())
	   continue;
	 cout<<name<<endl;
//-----------------------------------------------------
		//finding stddev and mean for log(x) histogram
		TH1F* h1 = new TH1F("h1", "h1 title", nbins, std::log(xmin), std::log(xmax));
		for (int i = 0; i < nbins+2; i++){
		 int yaxis = hpx->GetBinContent(i);
		 double xaxis = std::log(xmin+i*binw);
		 h1->Fill(xaxis,yaxis);
		}
		

		 TF1 *sigFit = new TF1("sig_fit", signal_fit, xmin, xmax, param_num);
		 sigFit->SetParameters(hpx->GetBinContent(hpx->GetMaximumBin()), hpx->GetMean(), hpx->GetRMS(), hpx->GetRMS(),hpx->GetRMS(),hpx->GetMean()-100,hpx->GetMean()+100); sigFit->SetParNames("N1", "Mean1", "Sigma1", "Sigma2", "Sigma3", "t1", "t2");//3 Gaus stitch
		 if( s.find("b1T__signal1800") < s.length()){
		 	sigFit->SetParameters(30, 1039, 65.2, 317.8,112.4,1000,1682);
		 }
		 if( s.find("b1T__signal1900") < s.length()){
		 	sigFit->SetParameters(30, 1139, 105.4, 326.5,114.9,1044,1787);
		 }		 
		 //sigFit->SetParameters(hpx->GetBinContent(hpx->GetMaximumBin()), hpx->GetMean(), hpx->GetRMS(), hpx->GetRMS(),hpx->GetMean()-100,100,log(hpx->GetRMS()),1.0); sigFit->SetParNames("N1", "Mean1", "Sigma1", "Sigma2", "t1",  "Mean", "Sigma","Slope");//2 Gaus and lognormal stitch
		 //sigFit->SetParameters(hpx->GetBinContent(hpx->GetMaximumBin()), hpx->GetMean(), hpx->GetRMS(), hpx->GetMaximumBin(), hpx->GetMean(), hpx->GetRMS(),hpx->GetMaximumBin(), hpx->GetMean(), hpx->GetRMS()); sigFit->SetParNames("N1", "Mean1", "Sigma1", "N2", "Mean2","Sigma2", "N3", "Mean3","Sigma3");//3 Gaus sum
		 //sigFit->SetParameters(hpx->GetBinContent(hpx->GetMaximumBin()), hpx->GetMean(), hpx->GetRMS(),1000,h1->GetMean(),h1->GetRMS(),1.0); sigFit->SetParNames("N1", "Mean1", "Sigma1", "Const","Mean", "Sigma","Slope");// Gaus+lognormal
		 //sigFit->SetParameters(hpx->GetBinContent(hpx->GetMaximumBin()), hpx->GetMean(), hpx->GetRMS(), hpx->GetRMS(),hpx->GetMean()-100,100,log(hpx->GetRMS()),1.0); sigFit->SetParNames("N1", "Mean1", "Sigma1", "Sigma2", "t1",  "Mean", "Sigma","Slope","t");//2 Gaus and lognormal rigid stitch
			//sigFit->SetParameters(hpx->GetBinContent(hpx->GetMaximumBin()), hpx->GetMean(), hpx->GetRMS(), hpx->GetRMS(),hpx->GetRMS(),hpx->GetMean()-100,hpx->GetMean()+100,10,1500,1000); sigFit->SetParNames("N1", "Mean1", "Sigma1", "Sigma2", "Sigma3", "t1", "t2", "Ngaus","Mean","Sigma");//3 Gaus stitch + gaus
			//sigFit->SetParameters(hpx->GetBinContent(hpx->GetMaximumBin()), hpx->GetMean(), hpx->GetRMS(), hpx->GetRMS(),hpx->GetRMS(),hpx->GetMean()-100,hpx->GetMean()+100,hpx->GetMean(),2,2,1000); sigFit->SetParNames("N1", "Mean1", "Sigma1", "Sigma2", "Sigma3", "t1", "t2","Sigma","Mean","const","a");//3 Gaus stitch + LN
		 hpx->Fit(sigFit,"R");
		  
   sigFit->SetLineColorAlpha(kBlack,0);sigFit->SetLineWidth(3);
	 hpx->Fit(sigFit,"R");gStyle->SetOptFit(1111);
	 
	 TH1F *fit_hist = new TH1F(name, name, nbins, xmin, xmax); 
   fit_hist->SetBinContent(1,hpx->GetBinContent(1));
	 fit_hist->SetBinContent(nbins,hpx->GetBinContent(nbins));
	 for (int j = 2; j < nbins; j++){
			int xval = xmin+(j-.5)*binw;
			fit_hist->SetBinContent(j,sigFit->Eval(xval)); 
	 }
	 //fit_hist->Draw("same");
   tablefile<<name<<"\t"<<sigFit->GetChisquare()/sigFit->GetNDF()<<"\t"<<hpx->Integral()<<"\t"<<sigFit->Integral(xmin,xmax)/(binw)<<"\t";
   for(int i = 0; i < param_num; i++){
   	  tablefile<< sigFit->GetParameter(i) + sigFit->GetParError(i)<<"\t";
   }
   tablefile<<endl;
//----------------------------------------------------------------------  
	 TF1 *sigFit_plus[param_num];
	 TF1 *sigFit_minus[param_num];
	 for(int i = 0; i < param_num; i++){
		  sigFit_plus[i] = new TF1("sigFit_plus", "signal_fit", xmin,xmax,param_num);
			sigFit_minus[i] = new TF1("sigFit_minus", "signal_fit", xmin,xmax,param_num);
			sigFit_plus[i]->SetParameters(sigFit->GetParameters());
			sigFit_minus[i]->SetParameters(sigFit->GetParameters());
			sigFit_plus[i]->SetParameter(i,sigFit->GetParameter(i) + sigFit->GetParError(i));
			sigFit_minus[i]->SetParameter(i,sigFit->GetParameter(i) - sigFit->GetParError(i));	
			sigFit_plus[i]->SetLineStyle(2);sigFit_plus[i]->SetLineColor(i+2);sigFit_plus[i]->Draw("same");
			sigFit_minus[i]->SetLineStyle(3);sigFit_minus[i]->SetLineColor(i+2);sigFit_minus[i]->Draw("same");   	   
	 }
	 std::string name_str = name;
	 name_str.append(".png");
	 char* title = const_cast<char*>(name_str.c_str());//converting string to char
	 //sigFit->SetTitle(title);
	 can->SaveAs(title);
//----------------------------------------------------------------------
 }//while	 
 ofile->Close(); tablefile.close();
 return 0;
}
