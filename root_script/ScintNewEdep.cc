
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
#include "TMath.h"
#include <ctime>
#include <sys/time.h>
#include  <tuple> //c++11

//#include <boost/filesystem.hpp>
//#include "ScintEdepStructs.hh" //if c++11 is not there
//#include "spline.hpp" //doesn't work for the purpose



//NEEDs root file as input. If it is large >3M entries, submit as parts
//./ScintEdep <rootfile> part <partsize> <partnum>

//NOTE don't replace the exe in cluster while jobs are running

//g++ -g -Wall -std=c++11  ScintNewEdep.cc -o ScintEdep `root-config --cflags --ldflags --libs` 
//  -Wl,--no-as-needed -lHist -lCore -lMathCore  

// NOTE limit of int +-2147483647. check the largest id and use long if needed
	
using namespace std;


struct HitStruct;

string GetParticleName( int pdg);
string GetProcessName (int code);
void printHitData(string key, const vector < HitStruct >& hitData, size_t n );
void printHitShort(string key, const vector < HitStruct > & hitData, size_t n );
double  CalculateQF(string particle, double eDep);
double LinearInterp( double input, const vector <double>& dataX, 
					 const vector <double> & dataY, bool extrapolate);
double GetTimeDistribution();
void GetCodes (string name, string postPr, string trkPr, int* codes);
int isNotAlphaNum(char ch)
{
	if (ch =='_') return 0;
	else 
		return ! isalnum(ch); //!isprint((unsigned)c); 
}



//see tuple is fater or not
struct HitStruct
{
	int trackID;
	int origTrackID;		
	int parentID;
	int evtID;
	double eDep_mev;
	double eDepQF;
	double totEdepQF;		
	double trackWt;
	double gTime;
	double lTime;	
	double preKE;
	double origPreKE;
	double xx;
	double yy;
	string partName;
	string origName;
	string postProc;
	string trkProc;
	string elOrigTrkProc;
	string elOrigPart;
	string origTrkProc;
	string parentName;
	int pointer;
	
	
	
	HitStruct (int trackID, int origTrackID, int parentID, int evtID,
			   double eDep_mev, double eDepQF, double totEdepQF, double trackWt, 
			double gTime, double lTime, double preKE, double origPreKE,
			double xx, double yy, string partName, string origName, string postProc,  
			string trkProc, string elOrigTrkProc, string elOrigPart,  
			string origTrkProc, string parentName, int pointer)
	:trackID(trackID), origTrackID(origTrackID), parentID(parentID), 
	evtID(evtID),eDep_mev(eDep_mev), eDepQF(eDepQF), totEdepQF(totEdepQF),  
	trackWt(trackWt),gTime(gTime), lTime(lTime), preKE(preKE),origPreKE(origPreKE),
	xx(xx), yy(yy), partName(partName), origName(origName), 
	postProc(postProc), trkProc(trkProc), elOrigTrkProc(elOrigTrkProc), 
	elOrigPart(elOrigPart), origTrkProc(origTrkProc), parentName(parentName),
	pointer(pointer)
	{}
	~HitStruct() {}
	
	
	/*
	 *	 // keep it seperate from the POD
	 *	 struct compare
	 *	 {
	 *		inline bool operator() (const HitStruct& str1, const HitStruct& str2)
	 *			return (str1.hitTrackID < str2.hitTrackID);
		};
	*/
};

//--------------------------------------------------------------------------



int main(int argc, char **argv)
{
	bool test 				= false;
    bool timetest       	= false;
	int  verbose 			= 1;
	bool debugPeak 			= false;
	bool debugAll 			= false;
	bool uniqPrintDebug 	= false;	
	bool storeAllOrigNeutrons= true;
	bool runIfRootExist		= true;
	bool testWriteText		= false;
	
	double timeLimit 		= 1500.;   // nanosec
	bool skipIfTimeGTLimit 	= false;  //needs to split the event ?	
	
	bool splitRootFile  	= false;
	int  partRootNum		= 0;
	
	bool saveSkipSecElFromHadEl = true;
	bool skipSecElFromHadEl	= true; //Always 
	
	bool skipIfEGTotEdepGT30k = true; //Always. use skipSecElFromHadEl ?

	double elGTotalMin		= 0.03;  //30keV

	Long64_t rootFileLimit	= 1000000;
	
	//select basement detector region based on angle of the detection, on a circle
	//in basement starting from perpendicular to proton beam line and center on
	// the basement plane
	bool basementPosLimit = true; //Set the angleUpperSide and angleLowerSide
	double angleUpperSide = 30.; // degrees. symmetric to proton beam
	double angleLowerSide = -30.; //degrees. symmetric to proton beam

	if( angleUpperSide <= angleLowerSide)
		basementPosLimit = false;

	

	if(argc==1) 
	{ 
		//this is a mess !
		cout << "Usage: " << endl //http://www.chemie.fu-berlin.de/chemnet/use/info/libgpp/libgpp_39.html
			<< "./ScintEdep <filename|test|timetest> "
			<< "[ < elhad|noelhad|verbose|split|rootsize|part > "
			<< " <elhad|noelhad|verbose > "
			<< "<dball|udball|nodb|dbpeak|udbpeak>]"
			<< endl;		
		return 1;			
	}

	string infileName("");
	string testpart("proton");
	if(argc>1)
	{
		string firstArg	= argv[1];
		if( firstArg.compare("test")==0 )
		{
			test = true;
			if(argc>2)
			{
				testpart = argv[2];
			}
		}
        else if( firstArg.compare("timetest")==0 )
        {
            timetest = true;
        }
		else
		{
			//strcpy (infileName,argv[1]);
			int nextArg = 0;
			
			infileName = argv[1];
			if(argc>2)
			{
                string arg  = argv[2];  
				int ii;
				istringstream iss(arg);
				
				if (iss >> ii && iss.eof() && ii>=0 ) 
				{
					verbose = ii;
				}
				else if(arg == "noelhad") skipSecElFromHadEl = true;
				else if(arg == "elhad") skipSecElFromHadEl = false;
				else if(arg == "split" || arg == "rootsize" || arg == "part") 
				{
					if(argc>3)
					{
						string argNext  = argv[3]; 
						istringstream issNext(argNext);
						if (issNext >> nextArg && issNext.eof() && nextArg >=0 ) 
						{
							rootFileLimit = nextArg;
						}
						if(arg == "split") splitRootFile = true;					
					}
					if(argc>4)
					{
						if(arg == "part") 
						{
							string argNext  = argv[4];
							istringstream issNext(argNext);
							if (issNext >> nextArg && issNext.eof() && nextArg >0 ) 
								partRootNum = nextArg;
							
							cout 	<< " partRun:: rootFileLimit " << rootFileLimit 
									<< " partRootNum " << partRootNum  << endl;
						}
					}
				}
                else 
                {
                    cout << "2nd arg wrong !" << endl;
                    return 1;                       
                }                       
			}		
			if(argc>3 && nextArg ==0 )
			{
				string arg  = argv[3];  
				int ii;
				istringstream iss(arg);
				
				if (iss >> ii && iss.eof() && ii>=0 ) 
				{
					verbose = ii;
				}
				else if(arg == "noelhad") skipSecElFromHadEl = true;
				else if(arg == "elhad") skipSecElFromHadEl = false;
				else 
				{
					cout << "arg 3 wrong !" << endl;
					return 1;                       
				}       
			}
			if(argc>4 && nextArg ==0 )
			{//<dball|udball|nodb|dbpeak|udbpeak>
				string arg	= argv[4];	
				if(arg == "dball")
				{
					debugAll = true;	
					debugPeak=true;
					uniqPrintDebug = false;
				}
				else if(arg=="udball")
				{
					debugAll=true; 
					debugPeak=true;
					uniqPrintDebug = true;
				}								
				else if(arg == "dbpeak")
				{
					debugPeak=true;
					uniqPrintDebug = false;
					debugAll = false;
				}
				else if(arg=="udbpeak")
				{
					debugPeak=true;
					uniqPrintDebug = true;
					debugAll = false;
				}		
				else if(arg=="nodb")
				{
					debugAll	= false; 
					debugPeak	= false;
				}									
				else
				{
					cout << "4th arg wrong !" << endl;
					return 1;						
				}	
			}//5							

            if( ! ifstream(infileName) )
            {
                cout << "File not found ! " << endl;
                return 1;
            }       
		}//else
	}//1	
	
	time_t start = time(0);
	cout << "@@@Run_Begin: " << ctime(&start)<< endl;
	

	if(test)
	{
		double sampleArr[]= { 0.01, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 
            1, 2, 3, 4, 5,8, 10, 20, 30, 40, 50, 60, 100, 150, 200, 250,300,
            350, 400, 500, 700, 800, 1000, 1500, 2000, 3000,4000, 5000, 10000,50000}; 

        vector<double> sample(sampleArr, sampleArr+sizeof(sampleArr)/
			sizeof(sampleArr[0]));

        int ss =0;
        if(testpart == "C12") ss = 16;

		for( size_t i=ss; i<sample.size(); ++i)
		{
            double edep = sample[i];
			double qFactor = CalculateQF(testpart, edep);

            if(qFactor<0 && edep > 0.001) //1kev
                cout << "Qfactor error: edep:qf "<< edep    << " "  << qFactor 
                    << " record: " << i      << " "     << testpart << endl;
            if(qFactor <= 0 ) qFactor = 1.;
			cout << testpart << " " << edep << " " << qFactor 
                << "  EdepQF " << qFactor*edep << endl;
			
		}
		return 0;
	}	
    else if( timetest)
    {
        for( int i=0; i<100000; ++i)
            cout << "time "  << GetTimeDistribution() << endl;
        return 0;
    }

    
    gROOT->ProcessLine("gErrorIgnoreLevel = kFatal;");  
	
	TFile* inRootFp    = TFile::Open(infileName.c_str());
	if (inRootFp->IsZombie())
	{
		cout << "Not a valid RootFile ! " << infileName << endl; 
		inRootFp->Close();
		return 0;  // not 1 ?
	}

	
	double  eDep_mev=0, trackWt=0, xx = 0, yy = 0, preKE = 0, gTime = 0, lTime =0;
	int	 pdg =0, evtID = 0, trackID = 0, parentID = 0, postPrCode = 0, trkPrCode =0;
	
	//TChain* chain = new TChain("DetEdep"); //same evt, track
	TTree *inFileT = (TTree*)inRootFp->Get("DetEdep");		
	inFileT->SetBranchAddress("eDep_mev", &eDep_mev);	
	inFileT->SetBranchAddress("PDGcode", &pdg);		
	inFileT->SetBranchAddress("trackWt", &trackWt);
	inFileT->SetBranchAddress("preKE", &preKE);
	inFileT->SetBranchAddress("postX_m", &xx);
	inFileT->SetBranchAddress("postY_m", &yy);
	inFileT->SetBranchAddress("evtID", &evtID);
	inFileT->SetBranchAddress("trackID", &trackID);
	inFileT->SetBranchAddress("parentID", &parentID);
	inFileT->SetBranchAddress("gTime_ns", &gTime);
	inFileT->SetBranchAddress("postProc", &postPrCode);
	inFileT->SetBranchAddress("trkProc", &trkPrCode);
	inFileT->SetBranchAddress("lTime_ns", &lTime);
	//inFileT->SetBranchAddress("postZ_m", &zz);
	//inFileT->SetBranchAddress("partKE_mev", &partKE);
	//inFileT->SetBranchAddress("postVol", &postVol);
	//inFileT->SetBranchAddress("stepNum", &stepNum);
	//inFileT->SetBranchAddress("eDepNonI", &eDepNonI);	
	
	
	Long64_t totEntries = inFileT->GetEntries();
	cout << "total Entries " << totEntries << endl;
	
    //split root file
	//NOTE TODO writing root file is only in float , useless to compare int later
	if(splitRootFile && totEntries > rootFileLimit)
	{
		float stepNum, postVol;
		inFileT->SetBranchAddress("postVol", &postVol);
		inFileT->SetBranchAddress("stepNum", &stepNum);
		
		string splitFName(infileName);
		size_t npos1 = splitFName.find(".root");
		if(npos1 != string::npos) 
			splitFName.replace(npos1,splitFName.length()-npos1,"");
		
		splitFName.erase(remove_if( splitFName.begin(), splitFName.end(), 
									isNotAlphaNum), splitFName.end()); 
		
		
		int numOfFiles = totEntries/rootFileLimit;
		if(totEntries % rootFileLimit > 0.5 * rootFileLimit)
		{
			numOfFiles = ceil (totEntries/(double)rootFileLimit);
		}	
		
		Long64_t modFileLimit = totEntries/numOfFiles;
		
		string splitFNamePart("split.root");
		int fileNum = 0;
		Long64_t tIndex = 0, startInd = 0, endInd = 0;
		
		//TODO it writes in float type for all
		while(fileNum < numOfFiles && tIndex < totEntries)
		{
			ostringstream oss;
			oss << ( fileNum+1 ) ;
			splitFNamePart = "part_" + oss.str()+ splitFName  + ".root";			
			
			startInd = endInd;
			endInd   = endInd + modFileLimit;
			if(endInd > totEntries) 
				endInd = totEntries;
			++fileNum;

			
			TFile f(splitFNamePart.c_str());
			if (! f.IsZombie())
			{
				cout << "Quitting :RootFileExists .. : " << splitFNamePart << endl; 
				f.Close();
				continue;
			}
			f.Close();
			
		    //NOTE it writes in float type for all
			TFile *splitRootF = new TFile(splitFNamePart.c_str(), "RECREATE");
				
			string varSplit("eDep_mev:PDGcode:trackWt:preKE:postX_m:postY_m:evtID:trackID");
			varSplit += ":parentID:stepNum:gTime_ns:postProc:trkProc:lTime_ns:postVol";
			
			TNtuple* splitTracks = new TNtuple("DetEdep", "eDep, wt and pos", varSplit.c_str());

			cout << fileNum << " : " << startInd;
			for(tIndex = startInd; tIndex < endInd; ++tIndex)
			{
				inFileT->GetEntry(tIndex);
				splitTracks->Fill(eDep_mev,pdg,trackWt,preKE,xx,yy,evtID,trackID,
					parentID,stepNum,gTime,postPrCode,trkPrCode,lTime,postVol);
					
				if(tIndex == endInd-1 && tIndex < totEntries ) 
				{			
					if(pdg != 2112 || postPrCode !=1)//push last entry
						++endInd;
				}
			}
			
			cout <<  " " <<  endInd-1 << endl;
			
			splitTracks->Write();
			splitRootF->Close();
		}		
		return 0;
	}
	else if (splitRootFile && totEntries <= rootFileLimit)
	{
		cout << "totEntries <= rootFileLimit, quitting" << endl;
		return 0;
	}
	if(totEntries < rootFileLimit)
	{
		cout << "Warning: RootFileSize < limit specified" << endl;
	}


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

	rootFileName = "E_" + rootFileName + ".root";
 
	

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
	cout << "Processing_File ... " << infileName << endl; 


	vector < HitStruct > hitData;
	
	//make uniq parent vector //edep, edepQF, trackWt, origTrack, evt, preke, nC,nP,nge
	typedef tuple <double, double, double, int, int, int, double, double, double, double> MyTuple;
	vector < MyTuple > hitsTracksVec;
	
	vector <tuple <int, int, double, double> > uniqueNTracks;
	
	vector <tuple <int, int, int, int, int> >	elasticIonVec;
	


	
	vector < tuple <double, double, int, int, double, double, int, int> > hadSecElVec;
	size_t hadSecElNum = 0;

	
	vector 	<size_t > textPos;
	vector 	<tuple <int, int, int> > debugTP;

	double 	tempWt = 0, totEdepQF = 0, qFactor = 1;	
	string partName("NULL"), trkProc("NULL"), postProc("NULL"), origTrkProc("NULL");
	string secName("NULL"), tempName("NULL"), elOrigTrkProc("NULL"), origName("NULL");
	string elOrigPart("NULL");
	

	bool printOnce		= false;

	size_t skipEGTotEdepGT30k  = 0;
	size_t skipOtherPart= 0;
	size_t skipDiffTrkPr = 0;
	size_t totalHitData = 0;
	string line("");	
	
	size_t hitDataNum 		= 0;
	size_t hitTracksNum	= 0;
	int hitParentsAll	= 0;
	

	//https://stackoverflow.com/questions/9371238/why-is-reading-lines-from-stdin-much-slower-in-c-than-python?rq=1
	//cin.sync_with_stdio(false);
	//std::ios_base::sync_with_stdio(false)
	//while (getline(cin, input_line))

	
	//-------------------------------------------------------------------------
	//read root file
	if(totEntries > 3000000 && (partRootNum ==0 || rootFileLimit == 0 ))
	{
		cout << " size >3 M, should run in part mode " << endl;
		return 0;
	}
	
	Long64_t sIndex = 0, eIndex = totEntries;
	
	//if part run
	if(partRootNum > 0 && rootFileLimit > 0)
	{
		//Should use the same rootFileLimit in all runs of the same file
		
		int numOfFiles = totEntries/rootFileLimit;
		if(totEntries % rootFileLimit > 0.5 * rootFileLimit)
		{
			numOfFiles = ceil (totEntries/(double)rootFileLimit);
		}	
		
		if(partRootNum > numOfFiles)
		{
			cout << "Part specified > last part " << endl;
			return 0;
		}
		
		Long64_t modFileLimit = totEntries/numOfFiles;

		eIndex	= modFileLimit;
		if(eIndex > totEntries) 
			eIndex = totEntries;
		
		int thisPart = 1;
		bool foundPart = false;
		for (Long64_t tIndex = 0; tIndex< totEntries; ++tIndex)
		{
			inFileT->GetEntry(tIndex);
			
			if(tIndex == eIndex-1)
			{			
				if(pdg == 2112 && postPrCode ==1)
				{
					if(partRootNum == thisPart)
					{
						foundPart = true;	
						break;
					}
					else
					{
						sIndex = eIndex -1;
						eIndex += modFileLimit;
						++thisPart;
					}
				}
				else
					++eIndex;
			}			
		}


		if(foundPart || sIndex >0)
		{
			cout 	<< " Part run " <<  partRootNum << " ::size " << eIndex - sIndex
					<< " from " 	<<  sIndex	<< " to " 	  << eIndex 
					<< " of total " << totEntries << endl;
					
			ostringstream oss;
			oss << partRootNum << "_" << sIndex/1000 << "_" << eIndex/1000 << "_K";
			rootFileName = "E_part_" + oss.str() + "_" + rootFileName;	
		}
	}



	TFile *outRootFile = new TFile(rootFileName.c_str(), "RECREATE");
	
	TNtuple* edepRoot   = new TNtuple("Edep", "seperate tracks",
				"edepQF_mev:edep_mev:origTrack:evtID:trKE:trackWt:distTime");
	
	//fill all tracks, with totEdepQF as an entry for each origTrack. Max 15 entries	
	string var("edep:edepQF:groupTotEdepQF:parentID:origTrack:evtID:");
	var += "trackWt:gTime:lTime:xx_m:yy_m:preKE:distTime:partCode:postPrCode";
	
	
	TNtuple* allTracks   = new TNtuple("Tracks", 
	"en_mev; time_ns; part=n:1,C:2,p:3; proc=hadE:1,nIn:2,hIo:3,eIo:4", var.c_str());
	
	
	string var2("edep:edepQF:evtID:origTrack:preKE:trackWt:Pnum:Cnum");
	TNtuple* hadSecEl = new TNtuple("hadSecEl", "hadSecEl electrons",var2.c_str());
	
	TNtuple *nFluxAll = new TNtuple("NF","allN", "ke:track:evtID:procID");
	
	if(eIndex - sIndex >3000000 ) 
	{
		cout << " size >3 M, reduce part size " << endl;
		return 0;
	}
	
	if(eIndex > totEntries) 
		eIndex = totEntries;

	for (Long64_t tIndex = sIndex; tIndex< eIndex; ++tIndex)
	{
		//redundant, but to make sure 
		eDep_mev = preKE 	= 0.;
		evtID 	 = trackID 	= parentID  = pdg = -1;
		trackWt  = gTime  	= lTime  	= 0;
		xx 	     = yy     	= 0;
		
		inFileT->GetEntry(tIndex);

		//to make sure root file has the right type.
		if(trackID == -1 || parentID == -1 || evtID == -1)
		{
			cout << "Error getting trackID | parentID | evtID " << endl;
			return 0;
		}
				

		partName = GetParticleName(pdg);
		postProc = GetProcessName(postPrCode);
		trkProc  = GetProcessName(trkPrCode);
		
		if(partName == "C12") {}
		else if(partName[0]=='C' || partName[0]=='B' || partName[0]=='L'
			|| partName[0]=='N' )
			partName = "Carbonlike";
		//C12|neutron|proton|e-|gamma|e+|alpha|triton|He3|He5|Li|Be|B11|B12|C13|C11|B10|B9|C10|N12
		else if (partName == "alpha" || partName == "He3" || partName == "He5")
			partName = "alpha";
		
		hitData.push_back(HitStruct(trackID, trackID, parentID, evtID, eDep_mev,
			0, 0, trackWt, gTime, lTime, preKE, preKE, xx, yy, partName,
			partName, postProc, trkProc, "NULL", "NULL", trkProc, "NULL", hitDataNum));
		++hitDataNum;
		//elOrigTrkProc,lOrigPart, origTrkProc,parentName,pointer	

		if(testWriteText)
		{
			cout << "Det " << eDep_mev << " " << partName << " vol id vid " 
			<< postProc << " p|t " << trkProc << " wt " << trackWt << " ids "
			<< trackID << " st " << parentID << " " << evtID << " p 0 " 
			<< preKE   << " pke 0 pos  " <<  xx << " " << yy << " zz gt "
			<< gTime  << " lt " << lTime << " rest " << endl;
		}
		
		// Det  0 neutron BraneDetScintC7H8 -1 volID  neutronInelastic 
		// post|trkCr neutronInelastic  wt 8.882e-16 ::trk:st:par:ev 103608  
		// 5  103559  6837  |postKE_m 0  60.6  preKE|NonIE_kev 0 |post_xyz_m 
		// 18.6 3.85 -7.61 trGTns 171 trLTns 9.52  secSize: 6 secpart0 neutron 
		// secKE 17.7 secpart1 gamma secKE 5.72 secpart2 gamma secKE 0.829 
		
		
		
	} //end read root file	
	
		inRootFp->Close();
		
		//if(hitDataNum ==0 ) continue;
		//else 
			
		if( hitDataNum > 1000)
			cout << endl << "HitData size: "<< hitDataNum << endl << endl;
		
		totalHitData += hitDataNum;
		
		double edep=0, preke = 0;
		int  evt=0, parent=0, track=0 , origTrack = -1;
		
		string name("NULL"), trkpr("NULL"), postpr("NULL");

		//*******************updating records  ******************************		
		//for (vector < HitStruct > hitData::const_iterator iter = hitData.begin();
		//		iter != hitData.end(); ++iter)

		for (size_t index = 0; index< hitDataNum; ++index)
		{
			if(index>0 && index%50000 ==0 ) //index>0 to avoid it for each split pass 
				cout << "Processing particle: " << index << endl;
			
			if(verbose>3) 
				printHitData("I: ", hitData, index);

			if(hitData[index].postProc == "CoupledTransportation"
				|| hitData[index].postProc == "Transportation" )
				continue;
				
			
			//if neutron check for other steps
			double range1 = hitDataNum*0.01;
			double range2 = hitDataNum*0.005;
			if(range1 >2000) range1 = 2000;
			if(range2 >500) range2 = 500;
			size_t startInd = index - (size_t)range1;
			size_t endInd   = index + (size_t)range2;
			if(startInd < 0) startInd = 0;
			if(endInd  >hitDataNum) endInd = hitDataNum;

			bool searchFull = false;			
			bool foundNeut = false;
			
			//search around for other steps of neutron disabled, since it is rare,
			// most of them default to searchAll, wasting time.
			/*
			while( ! foundNeut)
			{			
				if(searchFull)
				{
					startInd = 0;
					endInd   = hitDataNum;
				} */
			for (size_t ii=startInd ; ii < endInd; ++ii)
			{
				
				if( hitData[index].partName == "neutron" && index != ii 
					&& hitData[ii].evtID 	== hitData[index].evtID
					&& hitData[ii].trackID 	== hitData[index].trackID 
					&& hitData[ii].parentID == hitData[index].parentID )
				{ 						
					if(hitData[ii].postProc == "CoupledTransportation"
						|| hitData[ii].postProc == "Transportation" )
					{					
						foundNeut = true;
						hitData[index].pointer 			= ii;
						hitData[index].origTrackID 		= hitData[ii].trackID;
						hitData[index].origName 		= hitData[ii].partName;
						hitData[index].parentName 		= hitData[ii].partName;
						hitData[index].origPreKE		= hitData[ii].origPreKE;
						hitData[index].origTrkProc		= hitData[ii].trkProc;
							
						if(verbose >3)
							cout	<< "\t => Match:: ind " << ii << " : " 
									<< hitData[index].origTrackID << endl;
						break;
					}
				}
			}
				//if(foundNeut || searchFull) break;
				//else
				//	searchFull = true;
			//}// end search neutron other steps
			
			if(foundNeut) continue;			
			
			range1 = hitDataNum*0.02;
			range2 = hitDataNum*0.005;
			if(range1 >5000) range1 = 5000;
			if(range2 >1000) range2 = 1000;
			startInd = index - (int)range1;
			endInd   = index + (int)range2;
			if(startInd < 0) startInd = 0;
			if(endInd  >hitDataNum) endInd = hitDataNum;	
			
			//search and set pointer	
			bool searchAround = true; //SET 
			
			size_t indRef = index;
			bool searchOuter = true;
			while (searchOuter)
			{			
				int point = hitData[indRef].pointer;
				if(point == -1 || indRef == (size_t)point )
				{
					bool search 	= true;
					searchFull 		= false;
					if(!searchAround) searchFull = true;
					
					while( search)
					{
						if(searchFull)
						{	
							startInd = 0;	
							endInd   = hitDataNum;	
						}				
						for (size_t ii=startInd ; ii < endInd; ++ii)
						{ 									
							
							if( indRef != ii && hitData[ii].evtID == hitData[index].evtID
								&& hitData[ii].trackID == hitData[indRef].parentID )
							{ //NOTE not checking secondary KE, goes to 1st track parent

								search = false;
								hitData[indRef].pointer = ii; //set again ?
								point = ii;
								break; //for loop. 
							}//if
						}//for
						if(search && ! searchFull) 
							searchFull = true;
						else if( searchFull)
							search = false;
						
					}//while search
				} //if
				
				if(point != -1 && indRef != (size_t) point)
				{
					string thiPostPr	= hitData[point].postProc;
					if( thiPostPr == "CoupledTransportation" || thiPostPr == "Transportation"  )
					searchOuter = false;
					
					//update records only here
					hitData[index].origTrackID = hitData[point].trackID;
					hitData[index].origName 	= hitData[point].partName;
					hitData[index].origPreKE	= hitData[point].preKE;
					hitData[index].origTrkProc	= hitData[point].trkProc; 
					hitData[index].parentName 	= hitData[point].partName;
					
					string elProc = hitData[index].elOrigTrkProc;
					if(hitData[index].partName == "e-" 
						&&! (  elProc == "hadElastic" || hitData[index].elOrigPart == "gamma"))
					{
						hitData[index].elOrigTrkProc = hitData[point].trkProc;	
						hitData[index].elOrigPart	 = hitData[point].partName;
					}
					indRef = point;
					
					if(verbose >3)
					{
						cout	<< "\t => Match:: ind " << point << endl;
						printHitShort(" \t\t =>:: ", hitData, index);
					}
				}
				else
					searchOuter = false;
				
			}//while	
		}//for updating
		//**********end of updating ***********
		
		
		//************make list of N tracks and add el gamma energies
		int   firstWtInd = -1;
		int wtCount = 0;
		string parentName("");		
		bool useSecElasticIonKE = false; //true ONLY for cross ckecking.	
		double edepQF = 0.;	
		double elEn =0, gammaEn = 0;
		bool newOrigTrack = true;
		

		size_t uniqueNTracksNum = 0;		
		uniqueNTracks.clear();
		
		size_t elasticIonVecNum = 0;
		elasticIonVec.clear();
		
		if(verbose >2) cout << endl<< endl<< "Sorting Edep ..Total records: " 
							<< hitDataNum << endl;

		for (size_t index = 0; index< hitDataNum; ++index)
		{
			if(verbose>0 && index%100000 ==0)
				cout << "Sorting N tracks "<< index << endl;
			
			if(hitData[index].pointer == -1) 
			{
				hitData[index].origTrackID = hitData[index].trackID;
			}
			
			name	= hitData[index].partName;
			
			if( name == "neutron" ) continue;
			
			evt				= hitData[index].evtID;
			edep 			= hitData[index].eDep_mev;
			origTrack		= hitData[index].origTrackID;
			trkpr			= hitData[index].trkProc;
			elOrigTrkProc	= hitData[index].elOrigTrkProc;
			
			elEn = gammaEn = 0.;
			if(name =="e-") elEn = edep;
			else if(name =="gamma")gammaEn = edep;	
		
			if(verbose>2)
				printHitData("II: ", hitData, index);
			
		
			//make vector of electron gamma total energies
			newOrigTrack = true;
			
			for(size_t n = 0; n < uniqueNTracksNum; ++n)
			{	
				if( get<0>(uniqueNTracks[n])==evt 
					&& get<1>(uniqueNTracks[n])==origTrack )			
				{
					newOrigTrack = false;
					if( !skipSecElFromHadEl || elOrigTrkProc != "hadElastic")
					{
						get<2>(uniqueNTracks[n]) = get<2>(uniqueNTracks[n]) + elEn;
					}
					else if(verbose >3 && skipSecElFromHadEl && name== "e-" 
						&& elOrigTrkProc == "hadElastic")
						cout << "skip::skipSecElFromHadEl" << endl;
						
						
					//if threshold is high, gamma edep insted of sec e-
					get<3>(uniqueNTracks[n]) = get<3>(uniqueNTracks[n])  + gammaEn;	

					
					if(verbose >3) 
						cout << "\t\t Old_origTrac "<<  name 	<< " " 
						<< setw(6)					<< origTrack 				 
						<< " " 						<< setw(6) 
						<< get<2>(uniqueNTracks[n]) 
						<< " " 						<< setw(6) 
						<< get<3>(uniqueNTracks[n]) 	<< " " 
						<< " elOrigTrkProc " 		<< elOrigTrkProc << endl;	
					
					break;
				}//if		
			}//for
			
			//add
			if(newOrigTrack && hitData[index].origName =="neutron" )
			{	
				uniqueNTracksNum++;
				uniqueNTracks.push_back(make_tuple(evt,origTrack, elEn, gammaEn));

				if(verbose >3) 
					cout << "\t\t New_origTrac "<<  name 		<< " " 
					<< setw(6)					<< origTrack 				 
					<< " " 						<< setw(6) 
					<< elEn						<< " " 						
					<< setw(6) 					<< gammaEn
					<< endl;	
			}//new				
					
				
			//make unique tracks of ions for using secKE later in place of 
			//edep by hIoni,eIoni etc. by the recoil ions. Wall effect will be reduced ?
			if( useSecElasticIonKE && trkpr == "hadElastic"
				&& (name =="proton" || name== "Carbonlike" 
				|| name == "C12"|| name == "alpha" )
			)
			{
				bool newTrack = true;
				trackID	   = hitData[index].trackID;
				parentID   = hitData[index].parentID;
				for(size_t nn = 0; nn < elasticIonVecNum; ++nn)
				{	
					if( get<2>(elasticIonVec[nn])==trackID 
						&& get<3>(elasticIonVec[nn])==parentID )			
					{
						newTrack = false;
					}
				}
				if(newOrigTrack || newTrack )
				{
					elasticIonVec.push_back(make_tuple(evt,origTrack, trackID,
										parentID, -1));
					++elasticIonVecNum;
					if(verbose >3) cout << "\tAdding to elasticIonVec" << endl; 
				}			
			}
		}//************end make list************************
		
		
		
		//**************sort out hit data ******************
		int trkWarn =-1;
		for (size_t index = 0; index< hitDataNum; ++index)
		{
			if(verbose>0 && index%100000 ==0)
				cout << "Making hit data " << index << endl;
			if(hitData[index].pointer == -1) 
			{
				hitData[index].origTrackID = hitData[index].trackID;
			}
			
			track			= hitData[index].trackID;
			parent		 	= hitData[index].parentID ;
			origTrack		= hitData[index].origTrackID;
			evt				= hitData[index].evtID;
			edep			= hitData[index].eDep_mev;
			trackWt			= hitData[index].trackWt;
			gTime			= hitData[index].gTime;
			lTime			= hitData[index].lTime;
			preke			= hitData[index].origPreKE;
			name			= hitData[index].partName;
			postpr			= hitData[index].postProc;
			trkpr			= hitData[index].trkProc;
			origTrkProc		= hitData[index].origTrkProc;
			parentName		= hitData[index].parentName;
			elOrigTrkProc	= hitData[index].elOrigTrkProc;
			elOrigPart		= hitData[index].elOrigPart;
			xx				= hitData[index].xx;
			yy				= hitData[index].yy;
			


			//select basement detector region based on angle of the detection,
			// on a circle in basement starting from perpendicular to proton 
			// beam line and center on the basement plane
			if(basementPosLimit)
			{
				double radiusSquare = xx*xx+yy*yy;
				double angleOfPoint = -999;
				
				if(radiusSquare > 0)
					angleOfPoint = asin( yy/sqrt(radiusSquare) )*180./TMath::Pi();
				else
				{
					std::cout 	<< " Error: Detection point has only radius " << radiusSquare
								<< " x: " << xx << " y: " << std::endl; 
				}
				
				if(angleOfPoint > angleUpperSide || angleOfPoint < angleLowerSide)
					continue;
			}
					

			if(index ==0) tempWt = trackWt;
			if(verbose>2)
				printHitData("III: ", hitData, index);

			//except N
			if( name== "neutron" ||  name== "e-" ||name== "gamma" ) //||edep < edepMinE) 
			{
				++skipOtherPart;
				continue;
			}
			
			if( trkpr != "hadElastic")
			{
				++skipDiffTrkPr;
				if(verbose >3) cout << "\t\t Diff_trkpr..... " << endl; 
				continue;
			}
			
			//time
			if( skipIfTimeGTLimit && gTime+lTime > timeLimit ) 
			{
				if(verbose >3) cout << "\t\t skipTimeLimit..... " << endl; 
				continue;	
			}
				
			bool discard 		= false;
			bool foundTrk		= false;

			elEn = gammaEn = 0;
			//see if total sec energy from EG is above limit
			if(!discard) 
			{
				for(size_t tn=0; tn < uniqueNTracksNum; ++tn)
				{				
					if( get<0>(uniqueNTracks[tn]) == evt
						&&  get<1>(uniqueNTracks[tn])==origTrack )
					{
						foundTrk = true;
						elEn   		= get<2>(uniqueNTracks[tn]);
						gammaEn    	= get<3>(uniqueNTracks[tn]);
						
						if(verbose>5)
						{
							cout << " EGselect " << evt 	<< " " 	<< origTrack 
								<< " " 			<< elEn  	<< "  " << gammaEn 
								<< endl;
						}
						if( (elEn + gammaEn) >= elGTotalMin ) 
						{
							if(skipIfEGTotEdepGT30k) //set true always
							{
								discard = true;
								++skipEGTotEdepGT30k;

								if(verbose >0 && trkWarn != origTrack) 
								{	
									cout << "En:eg>30k :ort " 
										<<  setw (10)	<<	origTrack <<  " evt "
										<<  setw (6)	<<	evt
										<< " e-,g,preke  "
										<<  fixed << setprecision(4) << elEn 
										<< "  "
										<<  fixed << setprecision(4) << gammaEn 
										<< "  "
										<<  fixed << setprecision(1) << preke 
										<< " trkpr "
										<<  setw (10)	   << trkpr 	
										<< " " << name		
										<< " ****APPLIES TO ALL IN THIS ORTRACK"
										<< setprecision(6) << endl;
								
									trkWarn = origTrack;
								}
								cout.unsetf(ios_base::fixed);
							}
						}

						break;
					}
				}
			}
			
			if( ! foundTrk ) 
			{
				if(verbose>1) 
					cout << "Error finding Track " << origTrack << " evt " << evt << endl;
				discard = true;
			}
			
			//search in hadEl proton vec			
			if( useSecElasticIonKE && !discard && trkpr == "hadElastic" &&
			(name =="proton"|| name== "Carbonlike" ||name == "C12"||name == "alpha"))
			{
				for(size_t nn=0; nn < elasticIonVecNum; ++nn)
				{				
					if( get<0>(elasticIonVec[nn]) == evt
						&& get<1>(elasticIonVec[nn])==origTrack
						&& get<2>(elasticIonVec[nn])==track
						&& get<3>(elasticIonVec[nn])==parent )
					{
						if(	get<4>(elasticIonVec[nn])!=1)
						{
							get<4>(elasticIonVec[nn]) = 1;
							edep 	= hitData[index].preKE;
							if(verbose >3) 
								cout << "\t Using  secKE=edep: " << edep << endl;							
						}
						else
						{
							discard = true;
							if(verbose >3) 
								cout << "\t\t Discarded by useSecElasticIonKE " << endl; 
						}
						break;
					}
				}
			} //useSecElasticIonKE
			
			
			//Don't discard before this step. 
			if(discard)
			{
				continue;		
			}
			
			
			//Applying quenching		
			edepQF 	    = 0;
			qFactor 	= 1.;
			
			if( name== "C12" || name == "Carbonlike" || name =="alpha" 
				|| name == "proton" || name == "deuteron" || name == "triton")
			{
				qFactor = CalculateQF(name, edep*1000.);
		
				if(qFactor <= 0 || qFactor >1.)
				{
					double oldQF = qFactor;	
					double energy = edep, delta = 0.001;
					int sign=1, trials=1;
					string qfstat(" => ");
					while(true)
					{
						energy += sign * trials * delta;
						
						if(energy>0)
							qFactor = CalculateQF(name, energy*1000.);
						
						if(qFactor > 0 || trials >20 )
						{
							qfstat	= " ::recalc=> ";
							break;
						}
						++trials;
						sign *= -1;					
					}
				
					if(qFactor <= 0 && 
						(name == "proton" || name == "deuteron" || name == "triton") )
					{
						qFactor = 0.15;
					}
					else if(qFactor <= 0 &&
						(name == "C12" || name == "Carbonlike" || name =="alpha"))
					{
						qFactor = 0.01;
					}
					if(verbose >1)
						cout << "Qfactor: ind:part:e:qf "<< index  << " "
							<< name << " " << " " << edep << " " << oldQF 
							<< qfstat << energy <<  " " << qFactor << endl;
				} //qFactor

			}
			
			else if(qFactor > 1. || qFactor<0)
				continue;			
			
			//find quenched energy
			edepQF	= edep*qFactor;	
			
			hitData[index].eDepQF = edepQF;
			
			//check trackwt
			wtCount ++;
			if( firstWtInd >-1  &&  abs(tempWt - trackWt) > tempWt/2. )
			{
				cout << "WARNING:TrackWt changes:: nums(old):old:new "<< wtCount 
					<< " " << tempWt << " " << trackWt << " at " << index<< endl;			
				wtCount = 0;
			}
			tempWt = trackWt;
			firstWtInd = 1;
			

			if(verbose >3)
				cout	<< " ... =>to_add " << endl;

			//reset pointer for filling use
			hitData[index].pointer = -2; //-2 means used
			
			//find edep parent
			newOrigTrack = true;

			for(size_t n = 0; n < hitTracksNum; ++n)
			{	
				if(    get<5>(hitsTracksVec[n])==evt 
					&& get<4>(hitsTracksVec[n])==origTrack )
				{
					newOrigTrack = false;
					get<0>(hitsTracksVec[n]) = get<0>(hitsTracksVec[n]) + edep;					
					get<1>(hitsTracksVec[n]) = get<1>(hitsTracksVec[n]) + edepQF;

					if(verbose >3) 
						cout << "\t\t Old_edep " << setw (6)
							<< get<0>(hitsTracksVec[n]) 
							<< " " 				<< setw (6) 
							<< get<1>(hitsTracksVec[n]) 
							<< "  " 		 	<< setw (6)		<< origTrack 		
							<< " "				<< setw (6)		<< trkpr
							<< " elEn "         << setw (6)     
							<<  get<7>(hitsTracksVec[n])
							<< " gammaEn "      << setw (6)     
							<< get<8>(hitsTracksVec[n])
							<< endl;	
							
					break;
				}//if		
			}//for
			

			//add
			if(newOrigTrack )
			{	
				hitTracksNum++;
				hitsTracksVec.push_back(make_tuple(edep, edepQF, trackWt, 
							parent, origTrack, evt, preke,  elEn, gammaEn, 0.));

				if(verbose >3)
					cout 
						<< " \t\t New_edep " << setw (6)		<< edep 
						<< " " 				 << setw (6)		<< edepQF 
						<< " track " 		 << setw (6)		<< origTrack 		
						<< " "				 << setw (6)		<< trkpr	
						<< " elEn "          << setw (6)        << elEn
						<< " gammaEn "       << setw (6)        << gammaEn
						<< "  " 			 << setw (6)		<< name 
						<< endl;	
			}//new
		}// *************end sort hit data *******************
		
		
		//************store all tracks ****************
		debugTP.clear();
		size_t debugVecSize= 0;
		
		hadSecElVec.clear();
		hadSecElNum		= 0;
		
		int pointer 	= 0;
		for(size_t index = 0; index< hitDataNum; ++index)
		{
			pointer			= hitData[index].pointer; //set as -2 if used in adding up
			name			= hitData[index].partName;
			edep			= hitData[index].eDep_mev;	
			parent		 	= hitData[index].parentID ;
			origTrack		= hitData[index].origTrackID;
			evt				= hitData[index].evtID;
			trackWt			= hitData[index].trackWt;
			gTime			= hitData[index].gTime;
			lTime			= hitData[index].lTime;
			xx				= hitData[index].xx;
			yy				= hitData[index].yy;
			preke			= hitData[index].origPreKE;
			postpr			= hitData[index].postProc;
			trkpr			= hitData[index].trkProc;
			origTrkProc		= hitData[index].origTrkProc;
			parentName		= hitData[index].parentName;
			elOrigTrkProc	= hitData[index].elOrigTrkProc;
			elOrigPart		= hitData[index].elOrigPart;
			edepQF 			= hitData[index].eDepQF;
			track			= hitData[index].trackID;
			
			
			//store all original Neutron KE
			if(storeAllOrigNeutrons && name== "neutron" &&
				(postpr== "CoupledTransportation" || postpr == "Transportation"))
			{
				//store neutron time when it reaches the detector
				for(size_t n = 0; n < hitTracksNum; ++n)
				{
					if(get<5>(hitsTracksVec[n])==evt && get<4>(hitsTracksVec[n])==origTrack)	
					{
						if(get<9>(hitsTracksVec[n]) < 0.01)
							get<9>(hitsTracksVec[n]) = gTime + GetTimeDistribution();
						
						if(verbose >3)
							cout << "trk,time " << origTrack    << " "      << gTime
								<< " "         << get<9>(hitsTracksVec[n]) << endl;	
					}
				}

				int idTrkpr = 0;
				if( trkpr == "ImportanceProcess") idTrkpr =1;
				else idTrkpr = 2;
				
				nFluxAll->Fill(preke, origTrack, evt, idTrkpr);
			}
			
			//storing saveSkipSecElFromHadEl 			
			if( skipSecElFromHadEl && saveSkipSecElFromHadEl 
				&& name== "e-" && elOrigTrkProc == "hadElastic")
			{
				
				qFactor = CalculateQF(elOrigPart, edep*1000.);
				double modEdep = edep;
				while(qFactor <= 0 || qFactor >1.)
				{
					modEdep += 0.001;
					qFactor = CalculateQF(elOrigPart, modEdep*1000.);
				}
				edepQF	= edep*qFactor;	
				
				array < int, 3> codes2 = {0,0,0}; 
				GetCodes(elOrigPart, "", "", codes2.data());
				
				int origProtNum = 0, origCNum = 0;
				int pCode = codes2[0];
				if(pCode == 2|| pCode ==4 || pCode == 7)
					origCNum = 1;
				else if(pCode == 3 || pCode == 5 || pCode == 6)
					origProtNum = 1;			
				
				bool newhadSecElTrack = true;
				for(size_t n = 0; n < hadSecElNum; ++n)
				{
					if(get<2>(hadSecElVec[n]) == evt && 
						get<3>(hadSecElVec[n]) == origTrack)
					{
						newhadSecElTrack = false;
						get<0>(hadSecElVec[n]) += edep;
						get<1>(hadSecElVec[n]) += edepQF;
						get<6>(hadSecElVec[n]) += origProtNum;
						get<7>(hadSecElVec[n]) += origCNum;
					}
				}
				
				if(newhadSecElTrack)
				{
					hadSecElVec.push_back(make_tuple( edep,edepQF,evt, origTrack, preke,
								trackWt, origProtNum, origCNum));
					++hadSecElNum;
				}
			}// end of storing saveSkipSecElFromHadEl

			//store only used records
			if(pointer !=-2) continue;
			
			//fill in root, and print debugs
			bool details = false;
			string key 	 = "Db:o "; 
			if( debugAll ) details 	= true;

			for(size_t n = 0; n < hitTracksNum; ++n)
			{
				if(get<5>(hitsTracksVec[n])==evt && get<4>(hitsTracksVec[n])==origTrack)
				{	
					totEdepQF	= get<1>(hitsTracksVec[n]);		
					
					if(debugPeak && totEdepQF>0.06 && totEdepQF<0.14)
					{
						details 	= true;
						key = "Db:p ";
					}
				}
			}
			
			if( verbose >4 || details)
			{
				bool printThis = true;
				if(uniqPrintDebug)
				{
					for (size_t dd=0; dd < debugVecSize; ++dd)
					{
						if( get<0>(debugTP[dd]) == track && get<1>(debugTP[dd]) == parent 
							&& get<2>(debugTP[dd]) == evt)
							printThis = false;
						else
						{
							debugTP.push_back(make_tuple( track, parent, evt));	
							debugVecSize++; //clear spoils size 
						}
					}
				}
				if(verbose >4 ||printThis)
				{
					printHitData(key, hitData, index);
				}
			}		
			
			//NOTE for each collision, apply time profile. Per neutron is done elsewhere
			double distTime = gTime + GetTimeDistribution();
			
			//partCode, postPrCode, trkPrCode
			array < int, 3> codes = {0,0,0}; 
			GetCodes(name,  postpr,  trkpr,  codes.data());

			if(verbose>5)
				cout    << " time_n_codes: "<< gTime        << " " 
						<< distTime         << " "          << codes[0]    
						<< " "              << codes[1]     << " " 
						<< codes[2]         << " "          << codes[3] 
						<< endl;

			allTracks->Fill(edep, edepQF, totEdepQF, parent, origTrack, evt, 
				trackWt, gTime, lTime, xx, yy, preke, distTime,codes[0], codes[1]);
			
		} //************************end store all tracks *********************
		
		
		//**********store sec el from hadElastic edep processes *************
		if( skipSecElFromHadEl && saveSkipSecElFromHadEl )
		{
			for(size_t nn=0; nn < hadSecElNum; ++nn)
			{
				edep 		= get<0>(hadSecElVec[nn]);
				edepQF 		= get<1>(hadSecElVec[nn]);
				evt			= get<2>(hadSecElVec[nn]);
				origTrack	= get<3>(hadSecElVec[nn]);
				preke		= get<4>(hadSecElVec[nn]);
				trackWt		= get<5>(hadSecElVec[nn]);
				int origProtNum	= get<6>(hadSecElVec[nn]);
				int origCNum	= get<7>(hadSecElVec[nn]);
				
				hadSecEl->Fill(edep,edepQF,evt, origTrack, preke,
								trackWt, origProtNum, origCNum);
				if(verbose>4)
					cout << "saveSecElFromHadEl " << edep << " "  << edepQF
						<< " " << origTrack << " " << preke << endl;
			}
		}// *****************end of saveSkipSecElFromHadEl  *****************
		
		
		
		// ***********RESULT ******************
		hitParentsAll += hitTracksNum;
		
		//Write edep
		if(verbose >2) cout  << "Edep for unique Neutrons: " << endl;
		if(verbose >0 && ! printOnce)
		{
			printOnce = true;
			if(hitParentsAll >0)
			{
				cout  << endl
				<< "\tedep\tedepQF   preke  elEn(kev)  gammaEn(kev) origTrack "
				<< "  parent(IA)  evt neutgTime" << endl 
				<< "\t-----------------------------------------------"
				<< 	"------------------------------------------" << endl;
			}
		}

		for(size_t n = 0; n < hitTracksNum; ++n)
		{	
			double neutgTime;
			
			// edep, edepQF, trackWt, origTrack, evt, preke, nC,nP,nge
			edep		= get<0>(hitsTracksVec[n]);
			edepQF		= get<1>(hitsTracksVec[n]);
			trackWt		= get<2>(hitsTracksVec[n]);
			origTrack	= get<3>(hitsTracksVec[n]);	
			parent		= get<4>(hitsTracksVec[n]);
			evt			= get<5>(hitsTracksVec[n]);
			preke		= get<6>(hitsTracksVec[n]);	
			elEn		= get<7>(hitsTracksVec[n]);
			gammaEn		= get<8>(hitsTracksVec[n]);			
			neutgTime	= get<9>(hitsTracksVec[n]);
			
			edepRoot->Fill(edepQF, edep, origTrack, evt, preke, trackWt, neutgTime);


			if(verbose >0)
			{
				cout 		<<  fixed << setprecision(2) << setw(10) << edep			
				<<  " "  	<<  fixed << setprecision(4) << setw(10) << edepQF
				<<  " " 	<<  setprecision(1) 		 << setw(10) << preke
				<<  " " 	<<  fixed << setprecision(1) << setw(6)  << elEn*1000.
				<<  " " 	<<  fixed << setprecision(1) << setw(6)  << gammaEn*1000.
				<<  " " 	<<  setw (10)	<<	parent
				<<  " " 	<<  setw (10)	<<	origTrack			
				<<  " " 	<<  setw (8)	<< 	evt 
				<<  " " 	<<  setw (8)	<< neutgTime
				<<  endl;	
			}	
		}
		cout.unsetf(ios_base::fixed);
		cout << setprecision(6)<< endl;

		//processSet 	= false;
		//processDone = true;
	//}	//while read file
	//inRootFp.close();
	//************** end result ********************************


	if(verbose >0 )
	{
		if(hitParentsAll >0)
		{
			cout 
			<< "\t-----------------------------------------------"
			<< 	"------------------------------------------" << endl 
			<< "\tedep\tedepQF   preke  elEn(kev)  gammaEn(kev) origTrack "
			<< "  parent(IA)  evt neutgTime" << endl ;
			
			cout << "debug:: edep  edepQF  track  parent origTrack  evt  postProc "
                <<	"trackPr origTrkProc parentTrPr   gTime  lTime parentname"
                << endl;
				
			cout << endl<< "TrackWt: " << trackWt << endl;
		}
		std::cout 	<< endl	
					<< "Size (Total_Number_of_Neutron_Tracks) " << hitParentsAll 
					<< std::endl;
		
		std::cout 	<< "Total_records: "  << totalHitData  << "  Discarded::EG>30k: "
					<< skipEGTotEdepGT30k << " NonIons: "  << skipOtherPart 
					<< " DiffTrkPr: "	  << skipDiffTrkPr 
					<<  std::endl;

		std::cout 	<< "Flags:: " 				<< " skipIfEGTotEdepGT30k: " 
					<< skipIfEGTotEdepGT30k 
					<< " skipSecElFromHadEl: " 	<< skipSecElFromHadEl 
					<< std::endl;
				
		std::cout << " basementDetectorPosLimit: " 		<< basementPosLimit
				  << " angle from " 					<< angleLowerSide 
				  << " to " 							<< angleUpperSide
				  << std:: endl;

	}

	nFluxAll->Write();
	hadSecEl->Write();
	allTracks->Write();
	edepRoot->Write();

	outRootFile->Close();
	
	
	time_t end = time(0);
	time_t dur = end - start;
	cout << "\n@@@Run_duration " << dur/60. << " min @ " << ctime(&end)<< endl; 
	cout << "-----------------------------------------------------" << endl << endl;

	return 0;
}


/*
 // Nuclear PDG : 10-digit numbers +-100ZZZAAAI. Isomer level I=0 ground
// Hyper-Nucleus +-10LZZZAAAI. L = nlambda
*/		

//---------------------------------------------------------------------------

//--------------------------------------------------------------------------
double  CalculateQF(string particle, double eDepKev)
{
	
	//digitized
	   
	double carbonKeVnrArr[] = {14.309, 18.466, 29.112, 41.167, 51.222, 64.903, 
		85.289, 112.08, 144.62, 183.25, 226.30, 279.99, 351.56, 433.43, 525.80, 
		628.97, 789.74, 1017.0, 1299.1, 1720.7, 2259.1, 3072.0, 4068.7, 5436.5, 
		7075.1, 8658.7, 10976, 12991, 15871, 18698, 22750, 25951, 32325, 38738 };
		
	vector<double> carbonKeVnr(carbonKeVnrArr, carbonKeVnrArr+sizeof(carbonKeVnrArr)/
			sizeof(carbonKeVnrArr[0]));	
	
	double carbonQFArr[] = {0.11192, 0.087806, 0.058816, 0.043206, 0.035479, 
		0.028326, 0.022299, 0.017554, 0.014825, 0.012521, 0.011534, 0.010026, 0.0090034, 
		0.0083254, 0.0078312, 0.0073306, 0.0067126, 0.0062677, 0.0060476, 0.0056882, 
		0.0054232, 0.0053487, 0.0052754, 0.0052740, 0.0052728, 0.0053077, 0.0054525, 
		0.0056763, 0.0059530, 0.0062614, 0.0066330, 0.0070047, 0.0076456, 0.0082892};
		
			
	vector<double> carbonQF(carbonQFArr, carbonQFArr+sizeof(carbonQFArr)/
			sizeof(carbonQFArr[0]));				
			
	double protonKeVnrArr[] = {	
		0.022620, 0.026389, 0.03, 0.034117, 0.0382, 0.040838, 0.046674, 0.05146, 
		0.0588, 0.07411, 0.082553, 0.094836, 0.11236, 0.13728, 0.1577, 0.1927, 
		0.2391, 0.3107, 0.40379, 0.54117, 0.6819, 0.87263, 1.134, 1.3856, 1.8286, 
		2.4888, 3.2342, 4.1387, 5.4620, 6.8826, 9.2242, 11.623, 15.820, 20.244, 
		26.308, 34.719, 45.819, 61.407, 76.195, 103.70, 130.68, 152.45, 
		201.19, 261.46, 339.78, 462.45, 639.18, 869.94,	1000.0, 1068.506, 
		1165.9, 1219.919, 1362.362, 1492.0, 1590.153, 1856.031, 2126.7, 2528.59,
		2951.37, 3126.4, 4667.4, 7411.0, 10895, 16265, 25430};
		
	double protonQFarr[] = { 
		0.041089, 0.049818, 0.058493, 0.070072, 0.079546, 0.084958, 0.095519, 0.10425, 
		0.11581, 0.13510, 0.14662, 0.16145, 0.17691, 0.19107, 0.20439, 0.21759,
		0.23275, 0.24658, 0.25748, 0.2637, 0.2624, 0.25616, 0.24765, 0.2371, 0.2259,  
		0.21118, 0.19739, 0.18539, 0.16997, 0.1558, 0.14288, 0.1316, 0.11838, 0.1085,
		0.099036, 0.087368, 0.0801, 0.07344, 0.06931, 0.067, 0.0683, 0.07064,
		0.07703, 0.083597, 0.091161, 0.10232, 0.12110, 0.14195, 0.1545, 0.1632,
		0.17210, 0.1736, 0.1873, 0.19884, 0.2048, 0.2240, 0.23307, 0.2518,
		0.2735, 0.27717, 0.32645, 0.40346, 0.48678, 0.56510, 0.64972};
		
		
	vector<double> protonKeVnr(protonKeVnrArr, protonKeVnrArr+sizeof(protonKeVnrArr)/
			sizeof(protonKeVnrArr[0]));
	vector<double> protonQF(protonQFarr, protonQFarr+sizeof(protonQFarr)/
		sizeof(protonQFarr[0]));
		
	
	double resultY     	= 0;
	//double electronLY  	= 12000.;
	double conversion  	= 1.;//1.28*electronLY;
	double qFactor   	= 0;
	bool validParticle 	= false;
	//double ypval		= 0;
	
	int fVerbose 		= 1;
	
	
	if(particle =="C12" || particle=="Carbonlike" || particle == "alpha")
	{
		//spline_quadratic_val ( sizeof(carbonKeVnrArr)/sizeof(carbonKeVnrArr[0]),
		//				carbonKeVnrArr, carbonQFArr, eDepKev, &qFactor, &ypval );
		
		qFactor    = LinearInterp( eDepKev, carbonKeVnr, carbonQF, true );
		validParticle = true;
	}
	else if(particle == "proton" || particle=="deuteron" || particle=="triton")
	{
		//spline_quadratic_val (sizeof(protonKeVnrArr)/sizeof(protonKeVnrArr[0]),
		//	protonKeVnrArr, protonQFarr, eDepKev, &qFactor, &ypval );
		
		qFactor    = LinearInterp( eDepKev, protonKeVnr, protonQF, true );

		validParticle = true;
	}
	else 
	{ 
		if(fVerbose>1) cout << "*** Unknown particle in nPhot interpolation"
			<< particle<< endl;
	}
	
	resultY = conversion * qFactor;
	
	if( validParticle )
	{
		if(fVerbose > 2) 
			cout << setw(7) << fixed << right << particle  
			<< setw(9) << fixed << right  
			<< setprecision(5)<< qFactor 
			<< " * "<< setw(6)<< fixed << right 
			<< setprecision(0) << conversion << endl;
	}
	
	return resultY;
}



//--------------------------------------------------------------------------
double LinearInterp( double input, const vector <double>& dataX, 
					   const vector <double>& dataY, bool extrapolate=true)
{
	double low	= -1.;
	double high = -1.;
	double res 	= -1.;
	double lowY = -1.;
	double highY= -1.; 
	double n 	= dataX.size()-1;
	
	//TODO:check if data sorted in ascending order, log scale .....
	
	int fVerbose = 0;
	
	if(input <  dataX[0])
	{
		if(extrapolate)
		{
			double slope1 = (dataY[1]-dataY[0])/(dataX[1]-dataX[0]); 
			double slope2  = (dataY[2]-dataY[1])/(dataX[2]-dataX[1]) ;
			res = dataY[0] - (slope1 + slope2)/2. * (dataX[0] - input);
		}
		else res = dataY[0];
	}
	else if (input > dataX[n])
	{
		if(extrapolate)
		{ 
			double slope1 = (dataY[n]-dataY[n-1])/(dataX[n]-dataX[n-1]); 
			double slope2  = (dataY[n-1]-dataY[n-2])/(dataX[n-1]-dataX[n-2]) ;
			res = dataY[n] + (slope1 + slope2)/2. * (input - dataX[n]);
		}
		else res = dataY[n];
	}
	else //if within range
	{
		for (size_t i=1; i< dataX.size(); ++i)
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
			if(fVerbose>1) cout << "Error in interpolation: energy range"<< endl;
		}
	}
	
	return res;
}

//------------------------------------------------------------------------------//

double GetTimeDistribution()
{
    static TH1F* timeHist;
	static TRandom3* gRandom = NULL;

    if(!timeHist)
    {
        /*vector <double> timeRef =  {0.0000, 0.50556, 24.795, 48.994, 105.05, 129.41, 
            169.57, 194.07, 194.76, 219.32, 243.98, 268.61, 316.85, 356.71, 388.23, 
            450.67, 521.39, 583.74, 630.19, 676.78, 722.80, 762.27, 1032.1, 1286.0};

        vector <double> intensity = {0.0000, 0.0033598, 0.0097740, 0.014967, 0.021686, 
            0.029017, 0.035431, 0.044594, 0.053757, 0.063837, 0.075138, 0.086134,
            0.094381, 0.096824, 0.093770, 0.079720, 0.070251, 0.054979, 0.039402, 
            0.025657, 0.0042761, 0.0015272, 0.00091632, 0.00061088};
        */

        vector <double> timeRef =  {0, 1, 9, 10, 12, 19, 20, 21, 25, 27, 34, 36, 
            42, 50, 59, 70, 77, 87, 102, 112, 121, 128, 142, 151, 162, 171, 176, 
            183, 184, 190, 193, 195, 204, 211, 220, 227, 233, 243, 252, 262, 274, 
            282, 290, 296, 307, 321, 335, 348, 367, 379, 395, 410, 423, 435, 453, 
            469, 487, 504, 525, 540, 559, 578, 593, 609, 625, 643, 656, 670, 677, 
            682, 686, 692, 698, 702, 707, 714, 719, 725, 732, 738, 742, 745, 750, 
            754, 759, 763, 775, 796, 812, 834, 849, 868, 886, 908, 927, 949, 974, 
            992, 1015, 1035, 1058, 1082, 1100, 1117, 1135, 1157, 1176, 1195, 1215, 
            1235, 1256, 1277, 1286};

        vector <double> intensity = {0.0, 0.0037798, 0.0053419, 0.0047430, 
            0.0060819, 0.0067033, 0.0083212, 0.0075498, 0.009774, 0.0103296, 
            0.0113851, 0.0122801, 0.0131030, 0.0149179, 0.0162655, 0.0171689, 
            0.0185185, 0.0195470, 0.0210835, 0.0237458, 0.0267442, 0.0291601, 
            0.0307796, 0.0328422, 0.0342935, 0.0358089, 0.0377976, 0.0412120, 
            0.0448538, 0.0486419, 0.0499745, 0.0537090, 0.0566919, 0.0598405, 
            0.0631640, 0.0672754, 0.0716544, 0.0749557, 0.0784091, 0.0827638, 
            0.0865769, 0.0889486, 0.0905658, 0.0913853, 0.0922123, 0.0955957, 
            0.0964607, 0.0973336, 0.0955957, 0.0947384, 0.0938887, 0.0889486, 
            0.0865769, 0.0827638, 0.0798346, 0.0770090, 0.0756339, 0.0729571, 
            0.0691183, 0.0660741, 0.0609284, 0.0561835, 0.0508830, 0.0452597, 
            0.0402579, 0.0342317, 0.0301756, 0.0273287, 0.0232379, 0.0199382, 
            0.0174181, 0.0137810, 0.0117181, 0.0091057, 0.0076044, 0.0057515, 
            0.0048467, 0.0039044, 0.0032314, 0.0028230, 0.0025566, 0.0022947, 
            0.0021544, 0.0018992, 0.0017513, 0.0015437, 0.0015162, 0.0014364, 
            0.0013981, 0.0013731, 0.0013127, 0.0012436, 0.0012104, 0.0011782, 
            0.0011467, 0.0010574, 0.0010386, 0.0010018, 0.0009663, 0.0009155, 
            0.0008911, 0.0008596, 0.0008217, 0.0007855, 0.0007855, 0.0007375, 
            0.0007243, 0.0007050, 0.0006924, 0.0006619, 0.0006443, 0.0006271, 
            0.00061088 };
			
		if(gRandom) delete gRandom;
		gRandom = new TRandom3(0);		  

        timeHist = new TH1F("Times","Times",1286,0,1286);
        
        for (unsigned int i = 0; i< timeRef.size(); i++){
            timeHist->SetBinContent((int)timeRef[i], intensity[i]);
        }
    }	
    
	gRandom->SetSeed(0);
	
    return timeHist->GetRandom();
}

//------------------------------------------------------------------------------//

//partCode, postPrCode, trkPrCode, parentCode
//TODO this coding doesn't agree with the Geant4 steppingAction coding, and its decode below
void GetCodes (string name, string postpr, string trkpr, int codes[])
{

	if(name == "neutron") codes[0] = 1;
	else if(name == "C12") codes[0] = 2;	
	else if(name == "proton") codes[0] = 3;
	else if(name == "Carbonlike") codes[0] = 4;
	else if(name == "deuteron") codes[0] = 5;
	else if(name == "triton") codes[0] = 6;
	else if(name == "alpha") codes[0] = 7;
	else if(name == "gamma") codes[0] = 8;
	else if(name == "e+") codes[0] = 9;
	else if(name == "e-") codes[0] = 10;
	else  codes[0] = 11;	

	
	if(postpr == "hadElastic") codes[1] = 1;
	else if(postpr == "neutronInelastic") codes[1] = 2;	
	else if(postpr == "hIoni") codes[1] = 3;
	else if(postpr == "eIoni") codes[1] = 4;
	else if(postpr == "annihil") codes[1] = 5;
	else if(postpr == "compt") codes[1] = 6;
	else if(postpr == "conv") codes[1] = 7;
	else if(postpr == "Decay") codes[1] = 8;
	else if(postpr == "eBrem") codes[1] = 9;
	else if(postpr == "ionIoni") codes[1] = 10;
	else if(postpr == "msc") codes[1] = 11;
	else if(postpr == "nCapture") codes[1] = 12;
	else if(postpr == "phot") codes[1] = 13;
	else if(postpr == "protonInelastic") codes[1] = 14;
	else if(postpr == "CoupledTransportation") codes[1] = 15;	
	else if(postpr == "Transportation") codes[1] = 16;

	
	if(trkpr == "hadElastic") codes[2] = 1;
	else if(trkpr == "neutronInelastic") codes[2] = 2;	
	else if(trkpr == "hIoni") codes[2] = 3;
	else if(trkpr == "eIoni") codes[2] = 4;
	else if(trkpr == "annihil") codes[2] = 5;
	else if(trkpr == "compt") codes[2] = 6;
	else if(trkpr == "conv") codes[2] = 7;
	else if(trkpr == "Decay") codes[2] = 8;
	else if(trkpr == "eBrem") codes[2] = 9;
	else if(trkpr == "ionIoni") codes[2] = 10;
	else if(trkpr == "msc") codes[2] = 11;
	else if(trkpr == "nCapture") codes[2] = 12;
	else if(trkpr == "phot") codes[2] = 13;
	else if(trkpr == "protonInelastic") codes[2] = 14;
	else if(trkpr == "CoupledTransportation") codes[2] = 15;	
	else if(trkpr == "Transportation") codes[2] = 16;

}

//-------------------------------------------------------------------------

string GetParticleName( int pdg)
{
	//C12|neutron|proton|e-|gamma|e+|alpha|triton|He3|He5|Li|Be|B11|B12|C13|C11|B10|B9|C10|N12
	// 100ZZZAAAI
	string name("NULL");
	if(pdg == 11) name = "e-";
	else if(pdg == -11) name = "e+";
	else if(pdg == 22 ) name = "gamma";
	else if(pdg ==2112) name = "neutron";
	else if(pdg == 2212) name = "proton";
	//else if(pdgAtNum ==100001)name ="
	else if(pdg > 1000020000 &&  pdg < 1000039999)name = "alpha";
	else if(pdg >= 1000040000 )name = "Carbonlike";
	
	return name;
			
}

//-----------------------------------------------------------------------

string GetProcessName (int code)
{
	string procName("NULL");
	if (code == 1) procName = "CoupledTransportation";  //"Transportation"
	else if (code == 2 ) procName = "hadElastic";
	else if (code == 3 ) procName = "ImportanceProcess";	
	else if (code == 4 ) procName = "neutronInelastic";	
	else if (code == 5 ) procName = "nCapture";
	else if (code == 25 ) procName = "alphaInelastic";
	else if (code == 6 ) procName = "protonInelastic";
	else if (code == 7 ) procName = "ionInelastic";	
	else if (code == 8 ) procName = "ionIoni";
	else if (code == 9 ) procName = "eBrem";
	else if (code == 10 ) procName = "compt";
	else if (code == 11 ) procName = "hIoni";
	else if (code == 12) procName = "eIoni";		
	else if (code == 13 ) procName = "nKiller";
	else if (code == 14 ) procName = "msc";
	else if (code == 15 ) procName = "hBrems";
	else if (code == 16 ) procName = "annihil";
	else if (code == 17 ) procName = "conv";
	else if (code == 18 ) procName = "photonNuclear";		
	else if (code == 19 ) procName = "hPairProd";
	else if (code == 20 ) procName = "CoulombScat";
	else if (code == 21 ) procName = "electronNuclear";
	else if (code == 22 ) procName = "phot";
	else if (code == 23 ) procName = "dInelastic";
	else if (code == 24 ) procName = "Decay";
	
	return procName;
}
//--------------------------------------------------------------------------
void printHitData(string key, const vector < HitStruct >& hitData, size_t n )
{			
	if(n %10 ==0)
		cout << " index ::\tpart  trk   parent\tedep :  origtrack "
		<< "postProc   preKE\ttrkproc : orgTrkProc\t parent " 
		<< "origPart elOrigPr elOrigPart ptr" << endl
		<< "-----------------------------------------------------------"
		<<"------------------------------------------------------------"
		<< endl;
	
	cout << key <<  n	 			<< " :: " 	<<  setw (8)
		<< hitData[n].partName 			<< " "		<<  setw (8)
		<< hitData[n].trackID 			<< " " 		<<  setw (8)
		<< hitData[n].parentID 			<< " " 		<<  setw(8) 
		<< setprecision(3) 
		<< hitData[n].eDep_mev			<< " : "	<<  setw (8) 
		<< hitData[n].origTrackID  		<< " " 		<<  setw (8) 
		<< hitData[n].postProc 			<< " " 		<<  setw (8) 
		<< setprecision(2)
		<< hitData[n].preKE 			<< " " 		<<  setw (8) 
		<< hitData[n].trkProc 			<< " : " 	<<  setw (8) 
		<< hitData[n].origTrkProc		<< " "		<<  setw (6) 
		<< hitData[n].parentName		<<  " "		<<  setw (8) 				
		<< hitData[n].origName			<<  " "		<<  setw (8) 
		<< hitData[n].elOrigTrkProc		<<  " "		<<  setw (8) 
		<< hitData[n].elOrigPart		<<  " "		<<  setw (8) 
		<< hitData[n].pointer			//<<  " "		<<  setw (8) 
		//<< hitData[n].evtID			
		<< setprecision(6) 
		<<  endl;
	
}
//-------------------------------------------------------------------------
void printHitShort(string key, const vector < HitStruct >& hitData, size_t n )
{

	cout << key	 						<< setw (8) 
		<< hitData[n].origTrackID  		<< " ort,pr " 		<<  setw (8) 
		<< hitData[n].origTrkProc		<< " "		<<  setw (6) 			
		<< hitData[n].origName			<<  " orn,el "		<<  setw (8) 
		<< hitData[n].elOrigTrkProc		<<  " "		<<  setw (8) 
		<< hitData[n].elOrigPart		<<  " elp,p "		<<  setw (8) 
		<< hitData[n].pointer			<<  endl;
	
}

/*The recoiling protons transfer energy to the system through excitation and ionization of the organic molecules. 
 NOTE
 G *eant4 may be doing wrong, hIoni instead of scintillation ?.
 if proton , elOrigTrkProc ==hadElastic use sec KE ?
	 Just take the ion KE - the KE of subsequent neutronInelastic (Wall effect)
	 
	 if e- cut is high ?, edep by e- will be missing, since e- was not created,
		 but proton or gamma deposit the energy as NonI. 
		 similarly, if particle is killed, this e- edep energy is missing.
		 */			
