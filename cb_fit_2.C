//
// The "crystalball" function for ROOT 5.x (mimics ROOT 6.x).
//
// Create the "crystalball" TF1 somewhere in your source code using:
 //double xmin = 500., xmax = 3000.; // whatever you need
 //TF1 *crystalball = new TF1("crystalball", crystalball_function, xmin, xmax, 5);
 //crystalball->SetParNames("Constant", "Mean", "Sigma", "Alpha", "N");
 //crystalball->SetTitle("crystalball"); // not strictly necessary
//

#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"

// see math/mathcore/src/PdfFuncMathCore.cxx in ROOT 6.x
double crystalball_function(double x, double alpha, double n, double sigma, double mean) {
  // evaluate the crystal ball function
  //if (sigma < 0.)     return 0.;
  double z = (x - mean)/sigma; 
  if (alpha < 0) z = -z; 
  double abs_alpha = std::abs(alpha);
  //double C = n/abs_alpha * 1./(n-1.) * std::exp(-alpha*alpha/2.);
  //double D = std::sqrt(M_PI/2.)*(1.+TMath::Erf(abs_alpha/std::sqrt(2.)));
  //double N = 1./(sigma*(C+D));
  if (z  > -abs_alpha)
    return std::exp(- 0.5 * z * z);
  else {
   // double A = std::pow(n/abs_alpha,n) * std::exp(-0.5*abs_alpha*abs_alpha);
    double nDivAlpha = n/abs_alpha;
    double AA =  std::exp(-0.5*abs_alpha*abs_alpha);
    double B = nDivAlpha -abs_alpha;
    double arg = nDivAlpha/(B-z);
    return AA * std::pow(arg,n);
  }
}

double crystalball_function1(const double *x, const double *p) {
  // if ((!x) || (!p)) return 0.; // just a precaution
   //[Constant] * ROOT::Math::crystalball_function(x, [Alpha], [N], [Sigma], [Mean])
  return (p[0] * crystalball_function(x[0], p[3], p[4], p[2], p[1]));
}




void cb_fit_2(){
    // Create the "crystalball" TF1 somewhere in your source code using:
 double xmin = 0., xmax = 1000.; // whatever you need
 TF1 *crystalball = new TF1("crystalball", crystalball_function1, xmin, xmax, 5);
 crystalball->SetParNames("Constant", "Mean", "Sigma", "Alpha", "N");
 TFile *f = new TFile("templates-single.root");
 TH1F *hpx = (TH1F*)f->Get("ljets__wjets");
 crystalball->SetParameters(5000,hpx->GetMean(), hpx->GetRMS(), hpx->GetMean(), 2);
 int fitStatus = hpx->Fit("crystalball");
 //auto f1 = new TF1("f1", "crystalball", -10,4);
 //f1->SetParameters(5, 0, 1, 1, 1);
 //f1->Draw();
//
}