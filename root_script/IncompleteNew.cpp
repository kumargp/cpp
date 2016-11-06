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
//#include <stdio.h>
//g++ code.cpp `root-config --cflags --ldflags --libs`
//text input (simulation out) files accepted having the specific format as the code needs

using namespace std;
int main(int argc, char *argv[])
//void process(Char_t inFileName[])
{
   //gROOT->Reset();

    Char_t infileName[500],rootFileName[500]="Stopped_";

    if(argc>1) strcpy (infileName,argv[1]);
    else { cout << "Needs input file" << endl; return 1;}
    bool v = false;    
    string tt("test");
    if(argc>2 &&  tt.compare(argv[2])==0 ) v=true;

    for (Int_t  ll=0; ll< strlen(infileName); ll++) {
        if( ! ( isalnum(infileName[ll])|| infileName[ll]=='.' )) infileName[ll]='_';
    } 

    //infileName[strlen(infileName)-4]='\0';
    strcat (rootFileName, infileName);
	strcat (rootFileName, ".root");
	
	
	TFile *rootFile = new TFile(rootFileName, "RECREATE");
	
	
	Double_t  en, trackWt, X, Y, Z, dd =0.,preke,partke;
	Int_t volID, pdg, evtID, detID, nCols=20; 
	string postVol(""); 

	
	TNtuple *evtPartEn  = new TNtuple("evtPartEn", "Event Particle Energies for Hits",
									  "eDep:eDepProt:eDepElec:eDepAlpha:eDepC12:eDepBe:nTableCalcPhot:nG4Phot:nGLG4Phot:nG4MethodPhot:evtID");
	TNtuple *evtProcEn  = new TNtuple("evtProcEn", "SD Event Energy of Post Process",
									  "eDep:nCapture:nInelastic:ionInelastic:compt:phot:eIoni:hIoni:ionIoni:eBrem:hadElastic:conv:msc:annihil:decay:evtID");
	TNtuple *evtTrack   = new TNtuple("evtTrackCreatorEn", 
									  "SD Event Energy of Track Creator Process",
								   "eDep:cr_nCapture:cr_nInelastic:cr_ionInelastic:cr_compt:cr_phot:cr_eIoni:cr_hIoni:cr_ionIoni:cr_eBrem:cr_hadElastic:cr_conv:cr_msc:cr_annihil:cr_decay:evtID");
	TNtuple *steps      = new TNtuple("steps", "Step details",
									  "eDep:Evt2TrackTime:EvtTime:TrackTime:E_carbon:E_alpha:E_proton:E_electron:stepNum:evtID"); 
	TNtuple *snsNflux   = new TNtuple("SNSneutronFlux", 
									  "crossing in:Volumes 1(CsI),2(Pb),3(Poly),4(water),5(mono)",
									  "VolumeID:preKE_mev:partKE_mev:PDGcode:trackWt:trackX_m:trackY_m:trackZ_m:evtID:trackID:parentTrackID:stepNum:time_us"); 
	TNtuple *snsDet     = new TNtuple("SNSdetections", "eDep, pos, momentum and ke",
									  "eDep_mev:preKE_mev:partKE_mev:PDGcode:trackWt:postX_m:postY_m:postZ_m:evtID:trackID:parentTrackID:stepNum:time_us"); 

	TNtuple *mstep = new TNtuple("MStep","MS det Nflux",
								 "partKE_mev:trackWt:trackX_m:trackY_m:trackZ_m:volumeID");
	
	TNtuple *nflux = new TNtuple("SNSNFlux","nflux in detector",
								 "partKE_mev:trackWt:trackX_m:trackY_m:trackZ_m:detID:evtID");
	TNtuple *wallRoof = new TNtuple("WallRoof","Wall and Roof Detector NFlux", 
								 "partKE_mev:trackWt:trackX_m:trackY_m:trackZ_m:volumeID");
	TNtuple *edep = new TNtuple("SNSEdep", "eDep, wt and pos",
								"eDep_mev:PDGcode:trackWt:postX_m:postY_m:postZ_m");
	TNtuple *layerFlux = new TNtuple("SNSLayerFlux",
									 "crossing shield layers(base id=1, surf id =2",
									 "partKE_mev:trackWt:trackX_m:trackY_m:trackZ_m:layerID");
	
	ifstream infile;
	infile.open(infileName);
	string line;
	int ndet = 0;

	while(true )
	{
		if( infile.eof() ) break;
		//volID = evtID  = 0;
		detID = evtID = 0;
		en =  trackWt = X = Y = Z = 0.;
		postVol = "";
		string keywordString("");
		int spos=0;
		string aa("");
		
		
		//dd =0.;
		getline(infile,line);
		stringstream ss(line);
		stringstream ss2(line);
		
		bool flux   	= false;
		bool layer  	= false;
		bool det    	= false;
		bool mstepflux 	= false;
		if(line.empty()) continue;
		/*
		if (line.compare(spos,5,"NFLUX") == 0) flux = true;
		else if (line.compare(spos,9,"layerFLUX") == 0) layer = true;
		else if (line.compare(spos,3,"Det") == 0) det = true;
		else if (line.compare(spos,5,"MSDET")==0) mstepflux = true;
		else continue;
		*/
		ss2 >> keywordString >> aa;
		if (keywordString.compare("NFLUX") == 0) flux = true;
		else if (keywordString.compare("layerFLUX") == 0) layer = true;
		else if (keywordString.compare("Det") == 0) det = true;
		else if (keywordString.compare("MSDET")==0) mstepflux = true;
		else continue;
		
		
		//check if line is incomplete ?

		if(flux)
		{
			if(v) cout << line << endl;
			for (int nn=0; nn<nCols; nn++) 
			{
				//NFLUX  5.59 2.71e-20 wt|post_xyz_m -4.95 19.4 -6.3 \
				//neutron TunWallDet postvol|volID 8 evtID 809 
				// NFLUX 19.7 1.42e-14 wt|post_xyz_m -1.07 -23.5 -7.15 id 1
				if(nn== 1) {ss >> en; if(v)cout << " en = " << en <<endl ;}
				else if (nn== 2){ ss >> trackWt; if(v)cout << " trkwt " <<  trackWt <<endl ;}
				else if (nn== 4){ ss >> X; if(v)cout << " X " << X <<endl ;}
				else if (nn== 5){ ss >> Y; if(v)cout << " Y "<<  Y <<endl ;}
				else if (nn== 6){ ss >> Z; if(v)cout << " Z " << Z <<endl ;}
				else if (nn==8 ) ss >> postVol; 
				else if (nn== 10){ ss >> detID; if(v)cout << " detID "<< detID <<endl ;}
				else if (nn== 12){ ss >> evtID; if(v)cout << " evtID "<< evtID <<endl ;}
				else ss >> aa;
				
			} 
			if(postVol.compare("BraneDetScintC7H8")==0 || postVol.compare("DetWaterShield")==0 ) 
			    nflux->Fill(en,trackWt,X,Y,Z,detID,evtID);
			else if(postVol.compare("TunWallDet")==0 || postVol.compare("BaseRoofDet")==0  
			        || postVol.compare("HdcLayer")==0 )
			    wallRoof->Fill(en,trackWt,X,Y,Z,detID,evtID);
            else 
                cout<< "postVol not matching" << endl;
		}
		else if(layer)
		{
			if(v) cout << "\n>>> layerFLUX \n "<< line << endl;
			for (int nn=0; nn< nCols; nn++) 
			{
				
				// NFLUX 19.7 1.42e-14 wt|post_xyz_m -1.07 -23.5 -7.15 id 1
				if(nn== 1) {ss >> en; if(v)cout << " en = " << en <<endl ;}
				else if (nn== 2){ ss >> trackWt; if(v)cout << " trkwt " <<  trackWt <<endl ;}
				else if (nn== 8){ ss >> detID; if(v)cout << " detID "<< detID <<endl ;}
				else if (nn== 4){ ss >> X; if(v)cout << " X " << X <<endl ;}
				else if (nn== 5){ ss >> Y; if(v)cout << " Y "<<  Y <<endl ;}
				else if (nn== 6){ ss >> Z; if(v)cout << " Z " << Z <<endl ;}
				else ss >> aa;
				
			} 
			layerFlux->Fill(en,trackWt,X,Y,Z,detID);
		}
		else if(mstepflux)
		{
			if(v) cout << "\n>>> MSDET \n "<< line << endl;
			for (int nn=0; nn< nCols; nn++) 
			{
				
				// NFLUX 19.7 1.42e-14 wt|post_xyz_m -1.07 -23.5 -7.15 id 1
				if(nn== 1) {ss >> en; if(v)cout << " en = " << en <<endl ;}
				else if (nn== 2){ ss >> trackWt; if(v)cout << " trkwt " <<  trackWt <<endl ;}
				else if (nn== 4){ ss >> X; if(v)cout << " X " << X <<endl ;}
				else if (nn== 5){ ss >> Y; if(v)cout << " Y "<<  Y <<endl ;}
				else if (nn== 6){ ss >> Z; if(v)cout << " Z " << Z <<endl ;}
				else if (nn== 10){ ss >> detID; if(v)cout << " volID "<< detID <<endl ;}				
				else ss >> aa;
				
			} 
			mstep->Fill(en,trackWt,X,Y,Z,detID);	
		}
		else if(det)
		{
			if(v)cout << "\n>>>>Det\n "<< line << endl;
			for (int nn=0; nn<8; nn++) 
			{
				// Det 0.00326 edep_mev|wt 1.42e-14 post_xyz_m -1.45 -23.2 -7.65
				if(nn== 1) {ss >> en; if(v)cout << en <<endl ;}
				else if (nn== 3){ ss >> trackWt;if(v)cout << trackWt <<endl ;}
				else if (nn== 5){ ss >> X; if(v)cout << X <<endl ;}
				else if (nn== 6){ ss >> Y;if(v)cout << Y <<endl ;}
				else if (nn== 7){ ss >> Z;if(v)cout << Z <<endl ;}
				else ss >> aa;
				
			}
			//edep->Fill( en,preke,partke,pdg,trackWt,X,Y,Z,evtID);
			edep->Fill( en,0,trackWt,X,Y,Z);
			ndet++;
		}
	}
	cout << "ndet " <<ndet << " for file " << argv[1] << endl;
	infile.close();
	
	
	//nflux->Print();
	//nflux->Draw("z : sqrt(x*x + y*y)");
	//cout << "Press any key to continue";
	//getchar();
	evtPartEn->Write();
	evtProcEn->Write();
	evtTrack->Write(); 
	steps->Write();
	snsNflux->Write();
	snsDet->Write();
	nflux->Write();
	mstep->Write();
	wallRoof->Write();
	edep->Write();
	layerFlux->Write();
	rootFile->Close();
	return 0;
}
