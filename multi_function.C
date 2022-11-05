#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"


// This is a multi-function fit. 
// This has three Gaussians stitched together. 
// The parameters are user defined

void multi_function(){
    // Get the Histogram
    TCanvas *c1 = new TCanvas("c1","c1",700,500);
    TPad* thePad = (TPad*)c1->cd();
    TFile *f = new TFile("templates-single.root");
    TH1F *hpx = (TH1F*)f->Get("ljets__signal2100");
    
    // Find max bin
    Int_t MaxBin = hpx->GetMaximumBin();
    Int_t x,y,z;
    hpx->GetBinXYZ(MaxBin, x, y,z);
    Int_t BinWidth = hpx->GetBinWidth(MaxBin);
    Int_t q = (x * BinWidth) + 500; // Finds the middle of the peak
    printf("The peak is at (%d,%d)\n",q,y);
    printf("x is (%d,%d)\n",x,y);


    // Set the functions and the ranges
    // Originally had no function type on first three lines-works in ROOT, makes C++ mad
     TF1 *g1 = new TF1("m1", "gaus", 500, q - (q/8)); // ranges and functions can be changed, q is the midpoint
     TF1 *g2 = new TF1("m2", "gaus", q - (q/8), q + (q/8)); // q/8 is used because it works
     TF1 *g3 = new TF1("m3", "gaus", q + (q/8), 3000);
    // Total is the sum of the functions
     TF1 *total = new TF1("mtotal", "gaus(0)+gaus(3)",500, 3000); // Testing this I didn't include the last gaus, yikes
    // Originally had code to get histogram here

    //Define parameter array for the total function
    Double_t par[9];
   
    // Fit the function and add to list
    hpx->Fit(g1,"R");
    hpx->Fit(g2,"R+");
    hpx->Fit(g3, "R+");
   
    // Get parameters from the fit
    g1->GetParameters(&par[0]);
    g2->GetParameters(&par[3]);
    g3->GetParameters(&par[6]);
    
    // Use parameters on the sum
    total->SetParameters(par);
    total->SetLineColor(4);
    hpx->Fit(total, "R+");


    

}
