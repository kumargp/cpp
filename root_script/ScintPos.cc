
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
#include <TMath.h>
#include <TError.h>
#include <TRandom3.h>
#include <ctime>
#include <sys/time.h>
#include  <tuple> //c++11

//#include <boost/filesystem.hpp>
//#include "ScintEdepStructs.hh" //if c++11 is not there
//#include "spline.hpp" //doesn't work for the purpose



//NEEDs text input text file (NOT root) formatted in a specific way as defined below
// The file processing is split based on the keyword "evtIDseedsIndex", but *NOT* NFLUX
//NOTE don't replace the exe in cluster while jobs are running

//g++ -g -std=c++11  ScintPos.cc -o ScintPos `root-config --cflags --ldflags --libs` 
//  -Wl,--no-as-needed -lHist -lCore -lMathCore  


using namespace std;

int isNotAlphaNum(char ch)
{
	if (ch =='_') return 0;
	else 
		return ! isalnum(ch); //!isprint((unsigned)c); 
}


int main(int argc, char **argv)
{
	int 	verbose		= 1;
	double 	angleFull	= 180.;
	double 	refAngle 	= 0;
	double 	temp		= 0;
	
	string infileName("");
	if(argc>1)
	{
		infileName = argv[1];
	}

	if (argc >2)
	{
		string arg  = argv[2];  
		istringstream iss1(arg);
		if (iss1 >> temp && iss1.eof() && temp>0 ) 
		{
			angleFull	= temp;
		}
	}
	if (argc >3)
	{
		string arg2  = argv[3];  
		istringstream iss2(arg2);
		if (iss2 >> temp && iss2.eof() ) 
		{
			refAngle = temp;
		}
	}
	if(argc>4)
	{
		string arg3  = argv[4];  
		int ii;
		istringstream iss3(arg3);
		
		if (iss3 >> ii && iss3.eof() && ii>=0 ) 
		{
			verbose = ii;
		}
	}
	
	if(infileName.empty())
	{
		cout << "missing file" << endl;
		return 0;
	}
	
	gROOT->ProcessLine("gErrorIgnoreLevel = kFatal;");   
	
	
	TFile file(infileName.c_str());
	if (file.IsZombie())
	{
		cout << "Quitting: Invalid_Root_file: " << infileName << endl; 
		file.Close();
		return 0;
	}
	
	
	string rootFileName(infileName);
	size_t npos1 = rootFileName.find(".root");
	if(npos1 != string::npos) 
		rootFileName.replace(npos1,5,"");
	else 
	{
		size_t npos2 = rootFileName.find("_out");
		if(npos2 != string::npos) rootFileName.replace(npos2,4,"");
	}
	
	rootFileName.erase(remove_if( rootFileName.begin(), rootFileName.end(), 
								  isNotAlphaNum), rootFileName.end()); 
	
	ostringstream oss;
	if(argc >2 && angleFull>0) 
		oss << "_" << angleFull << "_" << refAngle;
	rootFileName = "Pos_" + rootFileName + oss.str() + ".root";
	
	TFile *rootFile = new TFile(rootFileName.c_str(), "RECREATE");	
	

	float evtID=0, origTrack=0, evtEdep =0, trackEdep=0;
	float edepQF=0, trackWt=0, xx=0, yy=0, preKE=0, edepDistTime=0;

	//string var("edep:edepQF:groupTotEdepQF:groupTotEdep:parentID:origTrack:evtID:");
	//var += "trackWt:gTime:xx_m:yy_m:preKE:distTime:partCode:postPrCode";
	
	TTree *tracksT = (TTree*)file.Get("Tracks");	
	tracksT->SetBranchAddress("evtID", &evtID);
	tracksT->SetBranchAddress("origTrack", &origTrack);
	tracksT->SetBranchAddress("edepQF", &edepQF);
	tracksT->SetBranchAddress("trackWt", &trackWt);	
	tracksT->SetBranchAddress("xx_m", &xx);
	tracksT->SetBranchAddress("yy_m", &yy);	
	tracksT->SetBranchAddress("preKE", &preKE);		
	
	//"edepQF_mev:edep_mev:origTrack:evtID:trKE:trackWt:distTime");
	TTree *edepT = (TTree*)file.Get("Edep");
	edepT->SetBranchAddress("evtID", &evtEdep);
	edepT->SetBranchAddress("origTrack", &trackEdep);
	edepT->SetBranchAddress("distTime", &edepDistTime);	

	TNtuple* edepRoot   = new TNtuple("Edep", "xy processed",
					"edepQF_mev:origTrack:evtID:trKE:distTime:trackWt");
	
	vector <tuple <int, int, double, double, double > > uniqTracksVec;
	vector <tuple <int, int, double > > nTracksVec;
	
	cout << "Edep_entries "  << edepT->GetEntries() << endl;
	for (Long64_t tn = 0; tn < edepT->GetEntries(); ++tn)
	{
		edepT->GetEntry(tn);
		nTracksVec.push_back(make_tuple( (int)evtEdep, (int)trackEdep, edepDistTime) );
		cout << "file:: " <<  evtEdep <<  trackEdep << edepDistTime << endl;
	}
	
	size_t hitTracksNum =0, totHits = 0, rejected = 0;
	double radius = 0, angle = 0, angleMin =0, angleMax =0;
	bool firstEntry = true;
	
	//NOTE use half angle
	double angleHalf = angleFull/2.;
	angleMin = refAngle - angleHalf;
	angleMax = refAngle + angleHalf;	
	
	if(angleMax >90) 
	{
		refAngle -= (angleMax - 90.);
		angleMin = refAngle - angleHalf;
		angleMax = 90.;	
	}
	if(angleMin <-90) 
	{
		refAngle -= (angleMin + 90.);
		angleMax = refAngle + angleHalf;
		angleMin = -90.;	
	}
	
	cout << "Angle :: " << angleMin << " " << angleMax 
		<< "  Symmetric about beam line(y axis) " << endl;
	
	cout << " Total entries " << tracksT->GetEntries() << endl;
	//for(size_t fileNum=0; fileNum < totFiles; ++ fileNum)
	//{
	//	uniqTracksVec.clear();
	//	hitTracksNum = 0;
		for (Long64_t tn = 0; tn < tracksT->GetEntries(); ++tn)
		{
			if(tn %50000 == 0)
				cout << "Entry: " << tn << endl;
			
			tracksT->GetEntry(tn);
			

			if(firstEntry)
			{
				firstEntry 	= false;
				radius 		= sqrt( xx*xx + yy *yy);
				cout << "Radius = " << radius << " m " <<  endl;		
			}	
			
			if(abs(yy) <=radius)
				angle =  asin(yy/radius)*180./TMath::Pi();
			else if(yy >radius)
				angle = 90.;
			else if( yy <-radius)
				angle = -90.;
			
			//cout << angle << " " << angleMin << " " << angleMax << " " << edepQF << endl;

			if( ! (angle >= angleMin &&  angle <= angleMax) )
			{
				rejected++;
				continue;
			}
			//find edep parent
			bool newOrigTrack = true;
			
			for(size_t n = 0; n < hitTracksNum; ++n)
			{	
				if(    get<0>(uniqTracksVec[n])==evtID
					&& get<1>(uniqTracksVec[n])==origTrack )
				{
					newOrigTrack = false;				
					get<2>(uniqTracksVec[n]) = get<2>(uniqTracksVec[n]) + edepQF;
					
					if(verbose >1) 
						cout << "\t\t Old_Track " 			<< setw (6)
						<< get<2>(uniqTracksVec[n]) 
						<< " " 		 	<< setw (10)		<< origTrack 		
						<< " "			<< setw (8)			<< evtID
						<< " "         	<< setw (8)         << preKE
						<< endl;	
						
						break;
				}//if		
			}//for
			
			
			//add
			if(newOrigTrack )
			{	
				hitTracksNum++;
				uniqTracksVec.push_back(make_tuple((int)evtID, (int)origTrack, 
											edepQF, preKE, trackWt)); 
				if(verbose >1) 
					cout << "************* New_Track " 			
					<< setw (6)		<< edepQF 
					<< " " 		 	<< setw (10)		<< origTrack 		
					<< " "			<< setw (8)			<< evtID
					<< " "         	<< setw (8)         << preKE
					<< endl;	
			}//new
		} //entries
		file.Close();
	
	
		
		for(size_t n = 0; n < hitTracksNum; ++n)
		{	
			double distTime = 0;
			
			// edep, edepQF, trackWt, origTrack, evt, preke, nC,nP,nge
			evtID		= get<0>(uniqTracksVec[n]);
			origTrack	= get<1>(uniqTracksVec[n]);
			edepQF		= get<2>(uniqTracksVec[n]);
			preKE		= get<3>(uniqTracksVec[n]);	
			trackWt		= get<4>(uniqTracksVec[n]);

			//get neutron time when it reaches the detector
			for(size_t nn = 0; nn < nTracksVec.size(); ++nn)
			{
				cout 	<< " time  " <<  get<2>( nTracksVec[nn]) << " " 
						<< get<0>(nTracksVec[nn]) << " " 
						<< get<1>(nTracksVec[nn]) <<  endl;
				
				if(get<0>(nTracksVec[nn])==evtID && get<1>(nTracksVec[nn])==origTrack )	
				{
					if(get<2>( nTracksVec[nn]) < 0.01)
						distTime	= get<2>( nTracksVec[nn]);
				}
			}
			
			
			edepRoot->Fill( edepQF, origTrack, evtID, preKE, distTime, trackWt);
			if(verbose >0)
			{
				cout 	<< "\t"     	<< setw(12) << edepQF			
				
				<<  " " 	<< setw(10) << preKE
				<<  " " 	<< setw(10) << evtID
				<<  " " 	<< setw(10) << distTime;
				
				cout.setf(ios::fixed);
				cout << setw(12) << setprecision(0) << origTrack
				<< setprecision(6) <<  endl;			
				cout.unsetf(ios_base::fixed);
			}	
		}
		
	//}	//while read file

	
	totHits += hitTracksNum;
	//}
	cout << " trackWt " << trackWt << endl;
	
	cout << "totalHits " <<  totHits << "  :: rejected  " << rejected << endl;

	edepRoot->Write();
	
	rootFile->Close();
	
	return 0;

}