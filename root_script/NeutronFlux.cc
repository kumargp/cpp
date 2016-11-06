
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <array>
#include <algorithm>
#include <functional>
#include <cassert>

#include <vector>
#include <TSystem.h>
#include <TFile.h>
#include <TH1.h>
#include <TNtuple.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TFrame.h>
#include <TROOT.h>
#include <TError.h>
#include <TRandom3.h>
#include <ctime>
#include <sys/time.h>
#include  <tuple> //c++11

//g++ -g -Wall -std=c++11  ScintNewEdep.cc -o ScintEdep `root-config --cflags --ldflags --libs` 
//  -Wl,--no-as-needed -lHist -lCore -lMathCore  

// NOTE limit of int +-2147483647. check the largest id and use long if needed
	
using namespace std;

int isNotAlphaNum(char ch)
{
	if (ch =='_') return 0;
	else 
		return ! isalnum(ch); //!isprint((unsigned)c); 
}


int main(int argc, char **argv)
{
	string infileName("");
		
	bool runIfRootExist = true;
	
	
	if(argc==1) 
	{ 
		cout << "To plot NFlux. Usage: ./exe rootfile "
			<< endl;		
		return 1;			
	}
	else if(argc ==2)
	{
		infileName = argv[1];
	}
	else
		cout << "As of now, only one filename is used !. " << endl;
	
	   
    gROOT->ProcessLine("gErrorIgnoreLevel = kFatal;");  
	
	TFile* inRootFp    = TFile::Open(infileName.c_str());
	if (inRootFp->IsZombie())
	{
		cout << "Not a valid RootFile ! " << infileName << endl; 
		inRootFp->Close();
		return 0;  // not 1 ?
	}

	
	double partKE=0, trackWt=0, xx = 0, yy = 0, zz= 0;
	int	 volumeID =0;
	  
  
	//TChain* chain = new TChain("DetEdep"); //same evt, track
	TTree *inFileT = (TTree*)inRootFp->Get("DetNFlux");	
	
	inFileT->SetBranchAddress("partKE_mev", &partKE);
	inFileT->SetBranchAddress("trackWt", &trackWt);
	inFileT->SetBranchAddress("postX_m", &xx);
	inFileT->SetBranchAddress("postY_m", &yy);
	inFileT->SetBranchAddress("postZ_m", &zz);
	inFileT->SetBranchAddress("volumeID", &volumeID);
	

	string rootFileName(infileName);
	size_t npos1 = rootFileName.find(".root");
	if(npos1 != string::npos) 
		rootFileName.replace(npos1,rootFileName.length()-npos1,"");
	else 
	{
		size_t npos2 = rootFileName.find("_out");
		if(npos2 != string::npos) rootFileName.replace(npos2,4,"");
	}
	
	rootFileName.erase(remove_if( rootFileName.begin(), rootFileName.end(), 
								  isNotAlphaNum), rootFileName.end()); 

	rootFileName = "NF_" + rootFileName + ".root";
 
	

	if( ! runIfRootExist)
	{
		TFile f(rootFileName.c_str());
		if (! f.IsZombie())
		{
			cout << "Quitting :Good RootFileExists .. : " << rootFileName << endl; 
			f.Close();
			return 0;
		}
		f.Close();
	}
	
	
	TNtuple *nFlux = new TNtuple("DetNFlux","allN", 
								 "partKE_mev:trackWt:postX_m:postY_m:postZ_m:volumeID");
		
	TFile *outRootFile = new TFile(rootFileName.c_str(), "RECREATE");	

	Long64_t wrongVolID = 0;
	
	for (Long64_t tIndex = 0; tIndex< inFileT->GetEntries(); ++tIndex)
	{
		inFileT->GetEntry(tIndex);	

		if(volumeID == 5)
		{
			nFlux->Fill(partKE, trackWt, xx, yy, zz, volumeID);
		}
		else
		{
			++wrongVolID;
		}
	}
	
	if(wrongVolID >0)
	{
		cout << "Wrong volumeID : " << wrongVolID << endl;
	}
	
	nFlux->Write();
	
	outRootFile->Close();
	
	return 0;

}
	