#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"

void compare(){
 TCanvas *can= new TCanvas("can","can",700,500); gStyle->SetOptStat(0); 
 TLegend *leg = new TLegend(0.2, 0.2, .8, .8);
 TFile *ifile = new TFile("/home/guru/Fitting/test/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8_2016.root");TFile *ifile1 = new TFile("/home/guru/Fitting/test/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8_2016_1-3btag.root"); 

// TFile *ifile = new TFile("/home/guru/Fitting/test/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8_2016.root");
// TFile *ifile2016 = new TFile("/home/guru/Fitting/test/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8_2016_3.root");
 //TFile *ifile = new TFile("/home/guru/Fitting/test/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8_2016.root"); TFile *ifile2016 = new TFile("/home/guru/Fitting/test/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8_2016_bTagEff_new.root");

 TTree *tree = (TTree*)ifile->Get("Skim");TTree *tree1 = (TTree*)ifile1->Get("nominal");
 const char *var[] =  {"MET"};
 for(int i = 0; i < sizeof var/sizeof var[0]; i++){
	 //tree->Draw("EvtWt >> h1(10,0,5)");
	 //tree2016->Draw("ST");
	 tree->Draw("MT >> h1(100,0,200)");
	 tree1->Draw("MT","","same");((TH1F*)(gPad->GetListOfPrimitives()->At(1)))->SetLineColor(2);((TH1F*)(gPad->GetListOfPrimitives()->At(1)))->SetLineWidth(2);((TH1F*)(gPad->GetListOfPrimitives()->At(0)))->SetLineWidth(2);//htemp->SetLineColor(2); // cout<<var[i]<<endl; 
	 leg->AddEntry((TH1F*)(gPad->GetListOfPrimitives()->At(0)), "Before"); leg->AddEntry((TH1F*)(gPad->GetListOfPrimitives()->At(1)), "After");
	 leg->Draw();
	 std::string s = var[i]; s.append("2016.png");
	 char* title = const_cast<char*>(s.c_str());//converting string to char
	 can->SaveAs(title);
 }

 	
/* const char *hist_name = "Ele_WJets_WPt_nominal";
 TH1F *hpx = new TH1F; TH1F *hpx1 = new TH1F;
 TIter nextkey(ifile->GetListOfKeys()); 
 TKey *key;
 while ((key = (TKey*)nextkey())){
	 TObject *obj = key->ReadObj(); 
	 hpx = (TH1F*)(obj); 
	 const char *name = hpx->GetName();
	 hpx1 = (TH1F*)ifile2016->Get(name);
	 std::string s = name;
	 if (s.find(hist_name) < s.length()){
	   hpx->Draw("hist"); hpx->SetLineColor(4);//blue
	   hpx->SetTitle(name);
	   hpx1->Draw("same hist"); hpx1->SetLineColor(2);//red
	   cout<<name<<endl;
	   s.append(".png");
	   char* title = const_cast<char*>(s.c_str());//converting string to char
	   can->SaveAs(title);
	 }
 }*/
}
