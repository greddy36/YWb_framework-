#include "TMath.h"
#include <cmath>
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
void add_wqcdy(){
	TFile *ifile = new TFile("templates-6chan-HC_qcdy.root");
	TFile *ofile = new TFile("templates-6chan-HC_wqcdy.root", "RECREATE");

	TIter nextkey(ifile->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)nextkey())){
  	TObject *obj = key->ReadObj();
		TH1F *hpx = (TH1F*)(obj);
		const char *name = hpx->GetName();
		std::string s = name;
		
		if (s.find("zjets") > s.length() && s.find("qcd") > s.length()){//clone histograms that aren't zjet or qcd. Clones wjets
			TH1F *clone = (TH1F*) hpx->Clone();
			hpx->Write();
		}
		else if (s.find("qcd") < s.length()){
			TIter nextkey1(ifile->GetListOfKeys());
			TKey *key1;
			while ((key1 = (TKey*)nextkey1())){
				TObject *obj1 = key1->ReadObj();
				TH1F *hpx1 = (TH1F*)(obj1);
				const char *name1 = hpx1->GetName();
				std::string s1 = name1;
				if (s1.find("wjets") < s1.length()){
					if (s1.compare(5,s1.length()-5, s,3,s.length()-3)==0){//checking channels are similar
					  hpx->Add(hpx1); hpx->Write(); cout<<name<<"\t"<<name1<<endl;} 
					else continue;
				}//if

			}//while
			//hpx->Write();
		}//elseif
	}//while
}

/*				if (s1.find("wjets") < s1.length()){
					if (s.find("us")<s.length() && s1.find("us")<s1.length() && s1.compare(0,5, s,0,5)==0 && s1.compare(12,s1.length()-12, s,10,s.length()-10)==0){//checking channels are similar
					  hpx->Add(hpx1); hpx->Write(); cout<<name<<"\t"<<name1<<endl;} 
					else if(s.length()==10 && s1.length()==12 && s1.compare(0,5, s,0,5)==0){hpx->Add(hpx1); hpx->Write();cout<<name<<"\t"<<name1<<endl;}//for 6 chan
					else if(s.length()==8 && s1.length()==10 && s1.compare(0,5, s,0,5)==0){hpx->Add(hpx1); hpx->Write(); cout<<name<<"\t"<<name1<<endl;}//for 3chan
					else continue;
				}//if

			}//while
			//hpx->Write();
		}//elseif
	}//while
}
*/