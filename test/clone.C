#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"

void clone(){
  TFile *ifile1 = new TFile("Electron/year2018/RunABCD_SF_TriggerLow.root","READ");
  TFile *ifile2 = new TFile("Electron/year2018/RunABCD_SF_TriggerHigh.root","READ");
  TFile *ofile = new TFile("ElectronTrigSF2018.root", "RECREATE");
  TGraphAsymmErrors *hpx1 = (TGraphAsymmErrors*)ifile1->Get("ScaleFactors"); TGraphAsymmErrors *hpx2 = (TGraphAsymmErrors*)ifile2->Get("ScaleFactors");
  hpx1->SetName("ScaleFactorsLow"); hpx1->Write();
  hpx2->SetName("ScaleFactorsHigh"); hpx2->Write();
}
  