#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"

void compare1(){
	TFile *ifile = new TFile("/home/guru/Fitting/nuisance_hist_6chan_HC_wqcdy_5par.root");
	TCanvas *can= new TCanvas("can","can",700,500); gStyle->SetOptStat(0); 
	
	std::string sys[14] = {"eleId","eleReco","eleTrig","muId","muIso","muTrig","jes","jer","pu","hf","lf","stsf","pdf","scalewjets"};
	for(int i=0; i<15; i++){
		TLegend *leg = new TLegend(0.66, 0.72, .87, .87);
		std::string name = "wjets__b2M";
		std::string name_up, name_down;
		name_up = name+"__"+sys[i]+"Up"; 
		name_down = name+"__"+sys[i]+"Down";
		
		char* hist_nom = const_cast<char*>(name.c_str());
		char* hist_up = const_cast<char*>(name_up.c_str());//converting string to char
		char* hist_down = const_cast<char*>(name_down.c_str());
		cout<< hist_nom <<","<< hist_up <<endl;
		
		TH1D *nom = (TH1D*)ifile->Get(hist_nom); nom->Rebin();
		TH1D *up = (TH1D*)ifile->Get(hist_up); up->Rebin();
		TH1D *down = (TH1D*)ifile->Get(hist_down); down->Rebin();
		
		up->SetTitle(""); up->SetXTitle("VLQ Mass (GeV)");
		up->Draw("hist"); up->SetLineColor(2); up->SetLineStyle(1); up->SetLineWidth(1); up->SetFillColor(0);
		down->Draw("same hist"); down->SetLineColor(4); down->SetLineStyle(1); down->SetLineWidth(1); down->SetFillColor(0);
		nom->Draw("same hist"); nom->SetLineStyle(1); nom->SetLineWidth(1); nom->SetFillColor(0);
		
		std::string leg_up = sys[i]+"Up", leg_down = sys[i]+"Down"; 
		leg->AddEntry(nom, "Nominal");
		leg->AddEntry(up, const_cast<char*>(leg_up.c_str()));
		leg->AddEntry(down, const_cast<char*>(leg_down.c_str()));
		leg->Draw();
		
		name = name+"__"+sys[i]+".png";		 
		can->SaveAs(const_cast<char*>(name.c_str()));
		nom->Delete();
	}
	ifile->Close();
}
