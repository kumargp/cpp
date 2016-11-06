
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstring>
//#include <tuple>  // -std=gnu++11

#include "TFile.h"
#include "TH1.h"
#include "TNtuple.h"
#include "TChain.h"
//#include <stdio.h>
//g++ file.cc `root-config --cflags --ldflags --libs` -o exe

using namespace std;

G4double LinearInterp( G4double input, std::vector <G4double> dataX, 
					   std::vector <G4double>dataY, G4bool extrapolate=true);
G4int  CalculateLY(G4String particle, G4double eDep);


//process(Char_t infileName[])
int main(int argc, char **argv)
{

    bool plotTrack = true, plotLayer = true, plotNflux = true, plotEdep = true;
    Char_t infileName[500],rootFileName[500]="Analysis_";

    if(argc>1) strcpy (infileName,argv[1]);
    else { cout << "Needs input file" << endl; return 1;}

    TFile* infp = new TFile(infileName);
    TTree *SNSNFlux = (TTree*)infp->Get("SNSNFlux");
    TTree *SNSEdep = (TTree*)infp->Get("SNSEdep");
    TTree *SNSLayerFlux = (TTree*)infp->Get("SNSLayerFlux");

    for (Int_t  ll=0; ll< strlen(infileName); ll++) {
        if( ! ( isalnum(infileName[ll])|| infileName[ll]=='.' )) infileName[ll]='_';
    } 

    infileName[strlen(infileName)-5]='\0';
    strcat (rootFileName, infileName);
    strcat (rootFileName, ".root");

    TFile *rootFile = new TFile(rootFileName, "RECREATE");

    Double_t  en, trackWt, X, Y, Z, dd =0.,preke,partke;
    Int_t volID, pdg,evtID, detID;


    SNSNFlux->SetBranchAddress("partKE_mev",&partke);
    SNSNFlux->SetBranchAddress("trackWt",&trackWt);

    SNSEdep->SetBranchAddress("eDep_mev",&en);
    SNSEdep->SetBranchAddress("trackWt",&trackWt);

    SNSLayerFlux->SetBranchAddress("partKE_mev",&partke);
    SNSLayerFlux->SetBranchAddress("trackWt",&trackWt);


plotTrack = false;
    if(plotTrack)
    {
        TH1F *wtN = new TH1F("trackWtN", "trackWtN", 200,1.0e-25,1);
        TH1F *wtE = new TH1F("trackWtE", "trackWtE", 200,1.0e-25,1);

        SNSNFlux->Draw("trackWt >>trackWtN");
        wtN->Write();

        SNSEdep->Draw("trackWt >> trackWtE");
        wtE->Write();
    }

plotLayer = true;
    if(plotLayer)
    {
        TH1F *layer = new TH1F("LF", "Layer Flux", 300,0,300);
        //SNSLayerFlux->Draw("partKE_mev >> LF","trackWt/100000000*6.56E15*1/5130461*(layerID==1)");
        //SNSLayerFlux->Draw("partKE_mev >> LF","trackWt/10000000*6.56E15*1/2198000*(layerID==2)", "SAME");
        SNSLayerFlux->Draw("partKE_mev >> LF","trackWt/10000000*6.56E15*1/1884000*(layerID==3)");//, "SAME");
        SNSLayerFlux->Draw("partKE_mev","trackWt/10000000*6.56E15*1/1099000*(layerID==4)", "SAME");
        SNSLayerFlux->Draw("partKE_mev","trackWt/10000000*6.56E15*1/2826000*(layerID==5)", "SAME");
        SNSLayerFlux->Draw("partKE_mev","trackWt/10000000*6.56E15*1/7714352*(layerID==6)", "SAME");
        SNSLayerFlux->Draw("partKE_mev","trackWt/10000000*6.56E15*1/11258784*(layerID==8)", "SAME");
        SNSLayerFlux->Draw("partKE_mev","trackWt/10000000*6.56E15*1/10320552*(layerID==7)", "SAME");
        //layer->SetStats(0);
        layer->Write();
    }


plotNflux = true;
    if(plotNflux)
    {

        TH1F *nflux = new TH1F("NF", "Neutron Flux", 300,0,300);
        SNSNFlux->Draw("partKE_mev >> NF",
        "trackWt/100000000*6.56E15*1/35308*(asin(trackY_m/23.8)*180./TMath::Pi()<-35\
        && asin(trackY_m/23.8)*180./TMath::Pi()>-45)*1/2.");

        //TH1F *nflux2 = new TH1F("NF2", "Neutron Flux", 300,0,300);
        SNSNFlux->Draw("partKE_mev>>NF2","trackWt/100000000*6.56E15*1/35308*1/36");
        //nflux2->Draw("same");
        //nflux->SetStats(0);
        nflux->Write();
    }

plotEdep = false;
    if(plotEdep)
    {
        TH1F *edep = new TH1F("ED", "Energy Dep", 300,0,300);
        SNSEdep->Draw("eDep_mev*1000>>edep","trackWt/100000000*\
        (asin(postY_m/sqrt(postX_m*postX_m+postY_m*postY_m))*180./TMath::Pi()<-35\
        && asin(postY_m/sqrt(postX_m*postX_m+postY_m*postY_m))*180./TMath::Pi()>-45) *1/2.");
        //edep->SetStats(0);
        edep->Write();
    }

/*    TLegend myLegend(0.1, 0.2, 0.5, 0.5, "myLegend")
myLegend.SetTextSize(0.04);
//AddEntry(): first arg must be pointer
myLegend.AddEntry(&h2, "after cuts", "l");
myLegend.AddEntry(&h1, "before cuts", "l");
myLegend.Draw();

TPaveText *myText = new TPaveText(0.2,0.7,0.4,0.85, “NDC”);
//NDC sets coords relative to pad
myText->SetTextSize(0.04);
myText->SetFillColor(0); //white background
myText->SetTextAlign(12);
myTextEntry = myText->AddText(“Here’s some text.”);
myText->Draw();
*/

    rootFile->Close();
    return 0;
}



//---------------------------------------------------------------------------

//--------------------------------------------------------------------------
G4int  CalculateLY(G4String particle, G4double eDep)
{
	//table for calculation
	const G4double carbonKeVnrArr[] = {10, 50, 100, 150, 200, 300, 400, 500, 600, 800,
		1000, 1500, 2000, 3000, 4000, 5000, 7000, 9000, 10000, 13000, 16000,
		20000,25000, 30000, 40000, 50000};
		
		static const std::vector<G4double> carbonKeVnr(carbonKeVnrArr, carbonKeVnrArr+sizeof(carbonKeVnrArr)/
		sizeof(carbonKeVnrArr[0]));
		
		const G4double carbonQFArr[] = {0.66943, 0.04508, 0.01811, 0.01238,
			0.01022,0.00855, 0.00791, 0.00759, 0.00742, 0.00724, 0.00714, 0.00705, 
			0.00701, 0.00698, 0.00697, 0.00697, 0.00697, 0.00696, 0.00696, 0.00696,
			0.00696, 0.00696, 0.00696, 0.00696, 0.00696, 0.00696};
			
			
			static const std::vector<G4double> carbonQF(carbonQFArr, carbonQFArr+
			sizeof(carbonQFArr)/sizeof(carbonQFArr[0]));	
			
			const G4double protonKeVnrArr[] = { 0.034327, 0.065466, 0.10650, 0.15317, 0.20933,
				0.26620, 0.35545, 0.43858, 1.2313, 	11.091,	23.050, 56.096, 
				75 , 100, 150 , 200, 300, 400, 500, 600, 800, 1000, 1200, 1500, 2000, 3000,      
				4000, 5000, 7000, 10000, 15000, 20000, 25000, 30000, 40000, 50000 };			
				
				static const std::vector<G4double> protonKeVnr(protonKeVnrArr, protonKeVnrArr+sizeof(protonKeVnrArr)/
				sizeof(protonKeVnrArr[0]));
				
				const G4double protonQFarr[] = { 0.086844, 0.11630, 0.12379, 0.15150,
					0.16085, 0.20899, 0.14971, 0.20547, 0.19841, 0.11543, 0.092859, 0.078106,
					0.05293, 0.05827, 0.06811, 0.77126, 0.09343, 0.10807, 0.12146, 0.13385,
					0.15623, 0.17611, 0.19401, 0.21792, 0.25182, 0.30449, 0.34424, 0.37568,
					0.42282, 0.47071, 0.52036, 0.55158, 0.57329, 0.58935, 0.61173, 0.62271};
					
					static const std::vector<G4double> protonQF(protonQFarr, protonQFarr+
					sizeof(protonQFarr)/sizeof(protonQFarr[0]));
					
					
					
					G4double resultY     = 0;
					G4double electronLY  = 1.; // 12000.;
					G4double conversion  = 1.*electronLY;// 1.28*electronLY;
					G4double qFactor   = 0;
					G4bool validParticle = false;
					G4bool extrapolate   = false;
					
					extrapolate = true;
					G4int fVerbose = 0;
					
					G4double inputE = eDep/keV;  //NOTE
					
					if(particle == "C12"|| particle == "C13")
					{
						if( inputE >= carbonKeVnr[0])
							qFactor  = LinearInterp( inputE, carbonKeVnr, carbonQF, extrapolate );
						else qFactor = inputE * carbonQF[0]/carbonKeVnr[0];
						
						resultY = conversion * qFactor;
						validParticle = true;
					}
					else if(particle == "proton" || particle =="deuteron" )
					{
						if( inputE >= protonKeVnr[0])
							qFactor    = LinearInterp( inputE, protonKeVnr, protonQF, extrapolate );
						else qFactor = inputE * protonQF[0]/protonKeVnr[0];
						//G4cout  << std::setw(9) << std::fixed << std::right << std::setprecision(6)<< qFactor ; 
						resultY = conversion * qFactor;
						validParticle = true;
					}
					else 
					{ 
						if(fVerbose>1) G4cout << "*** Unknown particle in nPhot interpolation"
							<< particle<< G4endl;
					}
					
					
					if( validParticle )
					{
						if(fVerbose > 2) 
							G4cout << std::setw(7) << std::fixed << std::right << particle  
							<< std::setw(9) << std::fixed << std::right  
							<< std::setprecision(5)<< qFactor 
							<< " *"<< std::setw(6)<< std::fixed << std::right 
							<< std::setprecision(0) << conversion;
					}
					
					return (int)resultY;
}



//--------------------------------------------------------------------------
G4double LinearInterp( G4double input, std::vector <G4double> dataX, 
					   std::vector <G4double>dataY, G4bool extrapolate=true)
{
	G4double low = -1.;
	G4double high = -1.;
	G4double res = -1.;
	G4double lowY = -1., highY = -1.; 
	G4double n = dataX.size()-1;
	
	//TODO:check if data sorted in ascending order, log scale .....
	
	G4int fVerbose = 0;
	
	if(input <  dataX[0])
	{
		if(extrapolate)
		{
			G4double slope1 = (dataY[1]-dataY[0])/(dataX[1]-dataX[0]); 
			G4double slope2  = (dataY[2]-dataY[1])/(dataX[2]-dataX[1]) ;
			res = dataY[0] - (slope1 + slope2)/2. * (dataX[0] - input);
		}
		else res = dataY[0];
	}
	else if (input > dataX[n])
	{
		if(extrapolate)
		{ 
			G4double slope1 = (dataY[n]-dataY[n-1])/(dataX[n]-dataX[n-1]); 
			G4double slope2  = (dataY[n-1]-dataY[n-2])/(dataX[n-1]-dataX[n-2]) ;
			res = dataY[n] + (slope1 + slope2)/2. * (input - dataX[n]);
		}
		else res = dataY[n];
	}
	else //if within range
	{
		for (size_t i=1; i< dataX.size(); i++)
		{
			if(input < dataX[i] && input > dataX[i-1])
			{
				low     = dataX[i-1];
				high    = dataX[i];
				lowY    = dataY[i-1];
				highY   = dataY[i];
				break;
			}
		}
		//linear
		if(low>-1 && high >-1)
		{
			res = lowY + (highY-lowY)*(input-low)/(high-low);
		}
		else 
		{
			if(fVerbose>1) G4cout << "Error in interpolation: energy range"<< G4endl;
		}
	}
	
	return res;
}


