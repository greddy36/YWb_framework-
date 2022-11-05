#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"


// This is a multi-function fit. 
// This is a gaussian stitched together with an exponential

void multi_fit_jets(){
    // Get the Histogram
    TCanvas *c1 = new TCanvas("c1","c1",700,500);
    TPad* thePad = (TPad*)c1->cd();
    TFile *f = new TFile("templates-single.root");
    TH1F *hpx = (TH1F*)f->Get("ljets__zjets");

    // Set the functions and the ranges
    // Originally had no function type on first three lines-works in ROOT, makes C++ mad
     TF1 *g1 = new TF1("m1", "gaus", 500, 1400); // ranges and functions can be changed, q is the midpoint
     TF1 *g2 = new TF1("m2", "expo", 1400, 3000); // ranges because they work
    //  TF1 *g3 = new TF1("m3", "gaus", q + (q/8), 3000);
    // Total is the sum of the functions
     TF1 *g3 = new TF1("m2", "12*TMath::Landau(x,[0],[1],0)",500,3000);
     TF1 *total = new TF1("mtotal", "gaus(0)+expo(3)",500, 3000); // Testing this I didn't include the last gaus, yikes

    //Define parameter array for the total function
    Double_t par[5];
   
    // Fit the function and add to list
    hpx->Fit(g1,"R");
    hpx->Fit(g2,"R+");
    //hpx->Fit(g3, "R+");
   
    // Get parameters from the fit
    g1->GetParameters(&par[0]);
    g2->GetParameters(&par[3]);
     //g3->GetParameters(&par[0]);
    
    // Use parameters on the sum
    total->SetParameters(par);
    total->SetLineColor(4);
     hpx->Fit(total, "R+");


   

}
