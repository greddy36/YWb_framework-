#include <iostream>
#include "TFile.h"
#include "TString.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TProfile2D.h"
#include <algorithm>
#include <cstring>
#include <string>
#include <map>
#include <cmath>
#include <set>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <vector>
#include "TString.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TMath.h"
#include "TRandom3.h"
#include"TVector.h"
void print(const char* ext = ".root"){
  const char* inDir;
  for(Int_t k=1; k<=3;k++){
    if(k==1){inDir = ".";}
	char* dir = gSystem->ExpandPathName(inDir);
	void* dirp = gSystem->OpenDirectory(dir);
	const char* entry;
	const char* filename[100];
	TString str; Int_t n = 0;
		while((entry = (char*)gSystem->GetDirEntry(dirp))){
	  		str = entry;
	  		if(str.EndsWith(ext)){
				filename[n++] = gSystem->ConcatFileName(dir, entry);
	  		}
		}
	Printf(" Selection: %s", "EvtWt*( nBTagMed_DeepFLV >=1  && ST>500 && DR_LepleadJet > 2.0  && HT_Full_pu > 500 && abs(DPHI_LepMet) < 1.0 && bVsW_ratio < 2 && (nVetoElectrons +  nVetoMuons) == 1 && LeadJet_btag >=1 && Lepton_pt > 40 && Lepton_eta < 2.1 && LeadJet_pt > 200 && abs(LeadJet_eta) < 2.4)");
	for (Int_t i = 0; i < n; i++){
		TFile *input_file = new TFile(filename[i]);
		TTree *tree = (TTree*)input_file->Get("nominal");
		Int_t M = tree->GetEntries("EvtWt*( nBTagMed_DeepFLV >=1  && ST>500 && DR_LepleadJet > 2.0  && HT_Full_pu > 500 && abs(DPHI_LepMet) < 1.0 && bVsW_ratio < 2 && (nVetoElectrons +  nVetoMuons) == 1 && LeadJet_btag >=1 && Lepton_pt > 40 && Lepton_eta < 2.1 && LeadJet_pt > 200 && abs(LeadJet_eta) < 2.4)");
		//Int_t N = tree->Draw("Lepton_pt","EvtWt*(LeadJet_btag >=1 && DR_LepClosestJet > 0.5 &&  DR_LepClosestJet < 1.5 && MT > 0 && nBTagMed_DeepFLV >=2 && ST> 300 && HT_Full_pu > 500 && (nVetoElectrons +  nVetoMuons) == 1)");
		Printf("file -> (%s) = %i", filename[i] ,M);
	}
  }
}			
