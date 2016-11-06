//.L poly_analysis.cxx
//To analyse the processes of the detected events in root file by tracing back upto 3rd step of the parent.
// Gopan Perumpilly  gopan.p@gmail.com

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>

#include <math.h>
#include <vector>
#include <string>
#include <map>
#include <utility>

#include <TROOT.h>
#include "TChain.h"
#include <TSystem.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TF1.h>
#include <TGraph.h>
#include <TApplication.h>
#include <TFile.h>
#include <TTree.h>
#include <TLegend.h>
#include "TBranch.h" 
#include "TBasket.h" 
#include "TKey.h"
#include "TBranch.h"
#include "TMath.h"
#include "TObject.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TRandom3.h"
#include "Math/DistSampler.h"




//for f in `ls *root` ; do sed -i "s/FileName\[128\]=.*/FileName\[128\]=\"${f}\"\;/" poly_analysis.cxx; 
//root.exe -blx << "DOC"
//.L poly_analysis.cxx
//poly_analysis()
//.q
//DOC
//done



using namespace std;


poly_analysis()
//doit(Char_t FileName[])
{	//comment this line for doit()
	Char_t FileName[128]="IS_8_12_ext_5_5_5_100k_996.root";

	Char_t file[128],OutputFile[128]="Poly_an_";
	strcpy (file,FileName);

	for (Int_t  ll=0; ll< strlen(file); ll++) {
		if( ! ( isalnum(file[ll])|| file[ll]=='.' ) ) file[ll]='_';
	} 


	file[strlen(file)-5]='\0';
	strcat (OutputFile, file);

	string fnamecopy(file);
	
	TChain* chain = new TChain("fTree");
	chain->Add(FileName);

	MGTMCEventSteps *evSteps;
	chain->SetBranchAddress("eventSteps",&evSteps);
	MGTMCEventHeader *evHeader;
	chain->SetBranchAddress("eventHeader",&evHeader);

	MGMCRun *mcrun;
	chain->SetBranchAddress("fMCRun", &mcrun);

	bool debug =false;
	double Etot=0;
	unsigned long int nentry, totEntries; 

	string fname(OutputFile);
	totEntries= chain->GetEntries();

	vector <unsigned int> trackarr;
	vector <unsigned int> parentarr;
	vector <int> pdgarr;
	vector <string> volarr;
	vector <string> procarr;
	vector <double> edeparr;
	vector <unsigned int> tracks;
	vector <unsigned int> sortSteps;
	vector <unsigned int> sortStepsFirst;
	vector <unsigned int> copy;
	vector <unsigned int> firstParents;
	vector <unsigned int> secondParents;
	vector <unsigned int> thirdParents;
	vector <unsigned int> proc_select;

	ofstream ofile;
	ofile.open(fname.c_str(),fstream::app);

	cout << "Events in chain "<< totEntries  << endl;

	ofile<<  "\ntotEvents in " << fnamecopy << " " << totEntries  << endl;
	for(nentry=0; nentry< totEntries ;nentry++){

		chain->GetEntry(nentry);	

		double entot = evHeader->GetTotalEnergy();
		Etot = 0;

		unsigned long int totSteps = evSteps->GetNSteps();
		if(entot==0 )continue;

		ofile<< "entry: " << nentry << " ===================================================================================="<<endl;
		if(debug)ofile<<"EventID: "<< evSteps->GetEventID()<< " totSteps " << totSteps << " : totE(keV) : " << entot*1000.0 << endl; 

		unsigned long int m=0, i=0, j=0, k=0, n=0, parent=0;
		bool add=true;

		trackarr.clear();
		parentarr.clear(); 
		pdgarr.clear(); 
		volarr.clear(); 
		procarr.clear(); 
		edeparr.clear(); 

		sortSteps.clear();
		sortStepsFirst.clear();
		tracks.clear();
		proc_select.clear();
		firstParents.clear();
		secondParents.clear();
		thirdParents.clear();

		for(long int nstep=0;nstep< totSteps;nstep++){

			MGTMCStepData *step = evSteps->GetStep(nstep);
			double edep = step->GetEdep();
			double ke = step->GetKineticE();
			int sensid= step->GetSensitiveVolumeID(); 
			string volname = step->GetPhysVolName();
			string process = step->GetProcessName();
			unsigned int parentid = step->GetParentTrackID();
			unsigned int trackid = step->GetTrackID();
			int particleid= step->GetParticleID();
			bool prestep = step->GetIsPreStep();

			if(debug && process=="ionIoni" && sensid>0)Etot += edep;

			trackarr.push_back(trackid);
			parentarr.push_back(parentid);
			pdgarr.push_back(particleid);
			volarr.push_back(volname);
			procarr.push_back(process);
			edeparr.push_back(edep);

			if(debug){
				if(prestep) ofile <<" -----PreStep : stepnum " << nstep << endl; 
				ofile<<"ID: "<< trackid << " :parent: "<< parentid << " :KE:" <<  ke*1000.0 <<  " : EDep " << edep*1000.0 << endl; 
				ofile <<"\t\t\t\t"<< particleid <<"  :: " << volname<< " : " << process<<   endl;
			}
			
			//select crystal events
			if( volname.find("ActiveCrystal") != string::npos && edep>0  && (particleid==22 ||particleid==11 ||particleid==-11) ) 
			{

				add=true;
				for(i=0;i<sortSteps.size();i++){ if(tracks[i]==trackarr[nstep]) {add=false; break;}  }
				if(add){
					sortSteps.push_back(nstep);
					tracks.push_back(trackarr[nstep]); 
					if(debug)ofile<< " add tracks " << tracks[sortSteps.size()-1]<<endl;
				}


				//if(debug){
				//	ofile<<"crys: ID: "<< trackid << " :parent: "<< parentid << " :KE:" <<ke*1000.0 <<  " : EDep " << edep*1000.0 << endl; 
				//	ofile <<"crys:\t\t\t\t"<< particleid <<"  :: " << volname<< " : " << process<<   endl;
				//}
			}
		}//nstep

		if(debug){ofile<< "numbers nSortSteps in ActiveCr: " << sortSteps.size()<<endl;}
		if(sortSteps.size()==0)
		{
			if(debug)ofile<< "No events in sortSteps of edep>0 in ActiveCrstal for part=11,-11,22 " <<endl;
			//continue;
		}

		
		for (m=0; m<sortSteps.size(); m++) // find first occurances
		{
			n = sortSteps[m];
			if(debug){ ofile<< "from crys: " << trackarr[n] << " " << parentarr[n] << " " << 
				pdgarr[n] << " " << volarr[n] << " " << procarr[n] << endl;}
			bool found = false;

			for (k=0; k<totSteps; k++)
			{
				if(found) break; //find only first
				if( trackarr[k] == trackarr[n])
				{
					sortStepsFirst.push_back(k);
					
					if(debug)ofile<< " add sortStepsFirst " << k << " " <<  trackarr[k] << " " << parentarr[k]<<endl;
					found = true;
				}
			}
		}

		if(debug){ofile<< "numbers  sortStepsFirst.size() in ActiveCr: " <<  sortStepsFirst.size() << endl;}
		if( sortStepsFirst.size()==0)
		{
			if(debug)ofile<< "Possible Error :No events sortStepsFirst.size() of edep>0 in ActiveCrstal ******" <<endl;
			//continue;
		}

		copy=sortStepsFirst;

		for (m=0; m<sortStepsFirst.size(); m++) // collect
		{
			n = sortStepsFirst[m];

			if(debug){ ofile<< "  : " << trackarr[n] << " " << parentarr[n] << " " << pdgarr[n] << " " << volarr[n] << " " << procarr[n] << endl;}

			for (k=0; k<sortStepsFirst.size(); k++)
			{
				//if(debug){ ofile<< " track of k "<<  trackarr[sortStepsFirst[k]] << " " <<  parentarr[sortStepsFirst[k]] << endl; }
				if(trackarr[n] == parentarr[sortStepsFirst[k]]) 
				{  
					if(debug) ofile<< " deleted " << trackarr[copy[k]] << endl;     
					copy[k] = 0;
				}
			}
		}

		for (m=0; m<sortStepsFirst.size(); m++)//find process and  parents if needed
		{
			if(copy[m]==0 ) continue; 
			if(debug)ofile<< " List : " << copy[m]<< ":: " << trackarr[copy[m]]<< " " << parentarr[copy[m]] << endl;

			n = sortStepsFirst[m];
			if(debug) ofile<< " selected process .... " << procarr[n]<<endl;

			if(procarr[n]=="nCapture" || procarr[n]=="nInelastic") { 
				proc_select.push_back(n);
			} //only first 
			else //else find its parent
			{
				for(k=0; k<totSteps; k++)
				{
					if( trackarr[k] == parentarr[n] ) 
					{
						add=true;
						for(i=0;i<firstParents.size();i++){ if(tracks[i]==trackarr[k]) {add=false; break;}  }
						if(add){
							tracks.push_back(trackarr[k]);
							firstParents.push_back(k);
							if(debug)ofile<< " ...add parent track " << trackarr[k] <<endl;
						}						
					}
				}
			}
		}

		long int track;
		bool foundSecond = false;
		for (m=0; m<firstParents.size(); m++)//second process and  parents if needed
		{
			n=firstParents[m];
			track = trackarr[n];
			if(debug)ofile<< " retrieving parent : " << track << endl;

			for (k=0; k<totSteps; k++)
			{
				//if parent does it later ?
				if(trackarr[k] == track)
				{
					if(debug) ofile<< " second: " << trackarr[k] << " " << parentarr[k] << " " << 
						pdgarr[k] << " "<< volarr[k] << " " << procarr[k] << endl;
					if (procarr[k]=="nCapture" || procarr[k]=="nInelastic")
					{
						proc_select.push_back(n);
						foundSecond = true;
						if(debug)ofile<< " second: FOUND " <<endl;
					}
					break; //checks only first
				}
			}
		}
		
		if(!foundSecond)//third round
		{
			for (m=0; m<firstParents.size(); m++)
			{
				n=firstParents[m];
				track = trackarr[n];
				if(debug)ofile<< " Second..retrieving parent : " << track << endl;
				for(k=0; k<totSteps; k++)
				{
					if( trackarr[k] == parentarr[n] ) 
					{
						add=true;
						for(i=0;i<secondParents.size();i++){ if(tracks[i]==trackarr[k]) {add=false; break;}  }
						if(add){
							tracks.push_back(trackarr[k]);
							secondParents.push_back(k);
							if(debug)ofile<< " ...add second parent track " << trackarr[k] <<endl;
						}						
					}
				}
			}
		}


		bool foundThird = false;
		for (m=0; m<secondParents.size(); m++)//second process and  parents if needed
		{
			n=secondParents[m];
			track = trackarr[n];
			if(debug)ofile<< " retrieving parent : " << track << endl;

			for (k=0; k<totSteps; k++)
			{
				//if parent does it later
				if(trackarr[k] == track)
				{
					if (procarr[k]=="nCapture" || procarr[k]=="nInelastic")
					{
						proc_select.push_back(n);
						foundThird = true;
						if(debug) ofile<< " second: " << trackarr[k] << " " << parentarr[k] << " " << 
							pdgarr[k] << " "<< volarr[k] << " " << procarr[k] << endl;
					}
					break; //check only first
				}
			}
		}

		//third parent
		if(!foundThird)//third round
		{
			for (m=0; m<secondParents.size(); m++)
			{
				n=secondParents[m];
				track = trackarr[n];
				if(debug)ofile<< " Second..retrieving parent : " << track << endl;
				for(k=0; k<totSteps; k++)
				{
					if( trackarr[k] == parentarr[n] ) 
					{
						add=true;
						for(i=0;i<thirdParents.size();i++){ if(tracks[i]==trackarr[k]) {add=false; break;}  }
						if(add){
							tracks.push_back(trackarr[k]);
							thirdParents.push_back(k);
							if(debug)ofile<< " ...add second parent track " << trackarr[k] <<endl;
						}						
					}
				}
			}
		}


		bool foundFourth = false;
		for (m=0; m<thirdParents.size(); m++)//second process and  parents if needed
		{
			n=thirdParents[m];
			track = trackarr[n];
			if(debug)ofile<< " retrieving parent : " << track << endl;

			for (k=0; k<totSteps; k++)
			{
				//if parent does it later
				if(trackarr[k] == track)
				{
					if (procarr[k]=="nCapture" || procarr[k]=="nInelastic")
					{
						proc_select.push_back(n);
						foundFourth = true;
						if(debug) ofile<< " second: " << trackarr[k] << " " << parentarr[k] << " " << 
							pdgarr[k] << " "<< volarr[k] << " " << procarr[k] << endl;
					}
					break; //check only first
				}
			}
		}


		bool foundIon = false;
		for(m=0; m<proc_select.size() ;m++)
		{
			k=proc_select[m];
			if(debug)ofile<< " k: " << k <<endl;
			//get ions 
			if(pdgarr[k] >3000) { foundIon=true; }
			else
			{
				for (i=0; i<totSteps; i++)
				{
					if( parentarr[i]==parentarr[k] && pdgarr[i] >3000 && (procarr[i]== "nCapture" || procarr[i]=="nInelastic"))
					{  
						proc_select.push_back(i);
						foundIon = true;
					}
				}
			}
		}

		if(debug && !foundIon && !foundSecond && !foundThird && firstParents.size() >0) ofile << " Didn't find process/ion ***** " << endl; 

 		if(debug){ ofile << "END=====proc_select.size() " << proc_select.size() << endl;}
		unsigned int t=0;
		bool noproc=true;
		for(m=0; m<proc_select.size() ;m++)
		{
			k = proc_select[m];
			if(debug) ofile<< " k: " << k <<endl;
			add=true;
			for(i=0;i<m;i++)
			{ 
				t= proc_select[i];
				if( parentarr[t]==parentarr[k] && pdgarr[t]==pdgarr[k] &&
					volarr[t]==volarr[k] && procarr[t]==procarr[k]) 
				{
					add = false; 
					break;
				} 
			}
			if(add)ofile <<  pdgarr[k] << "  " << volarr[k] << "  " << procarr[k] <<  
				  " Ek: " << entot*1000.0 << "   ev:t:p " << " "<< evSteps->GetEventID()<< " "<<
				  trackarr[k] << "  " << parentarr[k] << " "<< fnamecopy <<endl;
			noproc = false;
		}
	
		if(noproc) ofile << "EventID: "<< evSteps->GetEventID()<< " totSteps " << totSteps << "  Ek:\t" << entot*1000.0 << " \t\t" << fnamecopy<< endl; 

		if(debug)ofile<< "ionI energy (keV)" << Etot*1000.0 << "  totE " << entot*1000.0 << endl;
	}//events totEntries  nentry
	if(ofile)ofile.close();
	return 0;
}

