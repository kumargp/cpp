// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include <fstream>
#include <iomanip>

#include "Analysis.hh"
#include "G4VAnalysisManager.hh"
#include "G4AnalysisManagerState.hh" //
#include "globals.hh"
#include "G4UnitsTable.hh" //??
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4ios.hh"
#include "G4SteppingManager.hh" //??
#include "G4TrackingManager.hh"
#include "G4ThreeVector.hh"



//#ifdef G4ANALYSIS_USE
//#endif

Analysis* Analysis::fInstance = NULL;
std::ofstream* Analysis::textFile = NULL;

//----------------------------------------------------------------------------

Analysis::Analysis() 
: textFileName("hits.txt")
, histFileName("Root_file")
{

}

//---------------------------------------------------------------------------

Analysis::~Analysis()
{
	//if(fInstance) delete fInstance;
  	//if(textFile)  delete textFile;
  
}

//---------------------------------------------------------------------------

void Analysis::Book()
{
	// /analysis/setFileName name
//	if( !textFile ) 	textFile = new std::ofstream(textFileName);
	//if(textFile->is_open()) 
	//	(*textFile) << "EDep(MeV) ELight(MeV)  x(cm)    y(cm)    z(cm)" << G4endl << G4endl;  
	
	// Create analysis manager
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->SetFileName(histFileName);
  man->SetVerboseLevel(2);
  //man->SetActivation(true);     //enable inactivation of histograms


  // Open an output file
  G4cout << "Opening output file " << histFileName << " ... ";
  //man->SetHistoDirectoryName("Histo");
  //man->SetNtupleDirectoryName("NTuples");
  man->OpenFile(histFileName); //??? here ??
  man->SetFirstHistoId(1);
  man->SetFirstNtupleId(1);  
  //man->SetFirstNtupleColumnId(1);


  // Book ntuples, id =1
  man->CreateNtuple("evtPartEn", "Event Particle Energies for Hits"); //NTuple name, title
  man->CreateNtupleDColumn("eDep");
  man->CreateNtupleDColumn("eDepProt");
  man->CreateNtupleDColumn("eDepElec");
  man->CreateNtupleDColumn("eDepAlpha");
  man->CreateNtupleDColumn("eDepC12");
  man->CreateNtupleDColumn("eDepBe");
  man->CreateNtupleIColumn("nTableCalcPhot");
  man->CreateNtupleIColumn("nG4Phot");
  man->CreateNtupleIColumn("nGLG4Phot");
  man->CreateNtupleIColumn("nG4MethodPhot");  
  man->CreateNtupleIColumn("evtID");
  man->FinishNtuple();

  // ntuples, id =2
  man->CreateNtuple("evtProcEn", "SD Event Energy of Post Process"); //NTuple name, title
  man->CreateNtupleDColumn("eDep");
  man->CreateNtupleDColumn("nCapture");
  man->CreateNtupleDColumn("nInelastic");
  man->CreateNtupleDColumn("ionInelastic");
  man->CreateNtupleDColumn("compt");
  man->CreateNtupleDColumn("phot");
  man->CreateNtupleDColumn("eIoni");
  man->CreateNtupleDColumn("hIoni");
  man->CreateNtupleDColumn("ionIoni");
  man->CreateNtupleDColumn("eBrem");
  man->CreateNtupleDColumn("hadElastic");
  man->CreateNtupleDColumn("conv");
  man->CreateNtupleDColumn("msc");
  man->CreateNtupleDColumn("annihil");
  man->CreateNtupleDColumn("decay");
  man->CreateNtupleIColumn("evtID");
  man->FinishNtuple();
  
  // ntuples, id =3
  man->CreateNtuple("evtTrackCreatorEn", "SD Event Energy of Track Creator Process"); //NTuple name, title
  man->CreateNtupleDColumn("eDep");
  man->CreateNtupleDColumn("cr_nCapture");
  man->CreateNtupleDColumn("cr_nInelastic");
  man->CreateNtupleDColumn("cr_ionInelastic");
  man->CreateNtupleDColumn("cr_compt");
  man->CreateNtupleDColumn("cr_phot");
  man->CreateNtupleDColumn("cr_eIoni");
  man->CreateNtupleDColumn("cr_hIoni");
  man->CreateNtupleDColumn("cr_ionIoni");
  man->CreateNtupleDColumn("cr_eBrem");
  man->CreateNtupleDColumn("cr_hadElastic");
  man->CreateNtupleDColumn("cr_conv");
  man->CreateNtupleDColumn("cr_msc");
  man->CreateNtupleDColumn("cr_annihil");
  man->CreateNtupleDColumn("cr_decay");
  man->CreateNtupleIColumn("evtID");
  man->FinishNtuple();
  
  //ntuple id =4
  man->CreateNtuple("steps", "Step details"); 
  man->CreateNtupleDColumn("eDep");  
  man->CreateNtupleDColumn("Evt2TrackTime");
  man->CreateNtupleDColumn("EvtTime");
  man->CreateNtupleDColumn("TrackTime");
  man->CreateNtupleDColumn("E_carbon");
  man->CreateNtupleDColumn("E_alpha");
  man->CreateNtupleDColumn("E_proton");
  man->CreateNtupleDColumn("E_electron");
  man->CreateNtupleIColumn("stepNum");
  man->CreateNtupleIColumn("evtID");
  man->FinishNtuple();

    
  //ntuple id =5 //minimum filling instead of SNSdetections
  man->CreateNtuple("DetEdep", "eDep, wt and pos"); 
  man->CreateNtupleDColumn("eDep_mev");
  man->CreateNtupleIColumn("PDGcode");
  man->CreateNtupleDColumn("trackWt");
  man->CreateNtupleDColumn("preKE"); 
  man->CreateNtupleDColumn("postX_m");
  man->CreateNtupleDColumn("postY_m");
  man->CreateNtupleDColumn("postZ_m");
  man->CreateNtupleIColumn("evtID");
  man->CreateNtupleIColumn("trackID");
  man->CreateNtupleIColumn("parentID");
  man->CreateNtupleIColumn("stepNum");
  man->CreateNtupleDColumn("eDepNonI");
  man->CreateNtupleDColumn("gTime_ns");
  man->CreateNtupleIColumn("postProc");
  man->CreateNtupleIColumn("trkProc");  
  man->CreateNtupleDColumn("lTime_ns");
  man->CreateNtupleIColumn("postVol"); 
  man->FinishNtuple(); 

  
  //ntuple id 6 //minimum filling instead of SNSneutronFlux
  man->CreateNtuple("DetNFlux", "water, scint, MSDet etc"); 
  man->CreateNtupleDColumn("partKE_mev");
  man->CreateNtupleDColumn("trackWt");
  man->CreateNtupleDColumn("postX_m");
  man->CreateNtupleDColumn("postY_m");
  man->CreateNtupleDColumn("postZ_m");
  man->CreateNtupleIColumn("volumeID");
  man->FinishNtuple(); 
  
  
  //ntuple id 7 //minimum filling instead of SNSneutronFlux
  man->CreateNtuple("VolNFlux", "water, scint, MSDet etc"); 
  man->CreateNtupleDColumn("partKE_mev");
  man->CreateNtupleDColumn("trackWt");
  man->CreateNtupleDColumn("postX_m");
  man->CreateNtupleDColumn("postY_m");
  man->CreateNtupleDColumn("postZ_m");
  man->CreateNtupleIColumn("volumeID");
  man->FinishNtuple(); 

}


//NOTE: enable,  if (i%1000 == 1) ntuple->AutoSave("SaveSelf");

//----------------------------------------------------------------------------- 
/*
G4bool FillNtupleIColumn(G4int ntupleId, G4int columnId, G4int value);
G4bool FillNtupleFColumn(G4int ntupleId, G4int columnId, G4float value);
G4bool FillNtupleDColumn(G4int ntupleId, G4int columnId, G4double value);
G4bool FillNtupleSColumn(G4int ntupleId, G4int id, const G4String& value);
*/


void Analysis::FillNFluxOther(G4double partKE, G4double trackWt, G4double x, 
							G4double y , G4double z, G4int volumeID)
{
	G4AnalysisManager* man = G4AnalysisManager::Instance();
	man->FillNtupleDColumn(7,0,partKE);
	man->FillNtupleDColumn(7,1,trackWt);  
	man->FillNtupleDColumn(7,2,x);
	man->FillNtupleDColumn(7,3,y);
	man->FillNtupleDColumn(7,4,z);
	man->FillNtupleIColumn(7,5,volumeID);	
	man->AddNtupleRow(7);    
}     

//----------------------------------------------------------------------------- 

void Analysis::FillNFluxDet(G4double partKE, G4double trackWt, G4double x, 
							G4double y , G4double z, G4int volumeID)
{
	G4AnalysisManager* man = G4AnalysisManager::Instance();
	man->FillNtupleDColumn(6,0,partKE);
	man->FillNtupleDColumn(6,1,trackWt);  
	man->FillNtupleDColumn(6,2,x);
	man->FillNtupleDColumn(6,3,y);
	man->FillNtupleDColumn(6,4,z);
	man->FillNtupleIColumn(6,5,volumeID);	
	man->AddNtupleRow(6);    
}     

//---------------------------------------------------------------

void Analysis::FillEdep(G4double eDep, G4int pdgCode, G4double trackWt, G4double preKE, 
						G4double x, G4double y, G4double z, G4int evtID,  G4int trackid,    
						G4int parentID, G4int stepNum, G4double nonI, G4double gTime,
						G4int postProc, G4int trkProc, G4double lTime, G4int postVol)
// time or nonIonEdep
{
	G4AnalysisManager* man = G4AnalysisManager::Instance();
	man->FillNtupleDColumn(5,0,eDep);
	man->FillNtupleIColumn(5,1,pdgCode); 
	man->FillNtupleDColumn(5,2,trackWt);  
	man->FillNtupleDColumn(5,3,preKE); 	
	man->FillNtupleDColumn(5,4,x);
	man->FillNtupleDColumn(5,5,y);
	man->FillNtupleDColumn(5,6,z);     
	man->FillNtupleIColumn(5,7,evtID);		
	man->FillNtupleIColumn(5,8,trackid);		
	man->FillNtupleIColumn(5,9,parentID);		
	man->FillNtupleIColumn(5,10,stepNum);			
	man->FillNtupleDColumn(5,11,nonI); 	
	man->FillNtupleDColumn(5,12,gTime);
	man->FillNtupleIColumn(5,13,postProc);
	man->FillNtupleIColumn(5,14,trkProc);
	man->FillNtupleDColumn(5,15,lTime);
	man->FillNtupleIColumn(5,16,postVol);	
	man->AddNtupleRow(5);
} 


//-----------------------------------------------------------------------------

void Analysis::FillPerStep( G4double eDep, G4double evt2TrackTime, 
                  G4double evtTime, G4double trackTime, G4double ecarbon, 
                  G4double ealpha, G4double eproton, 
                  G4double eelectron, G4int stepNum, G4int evtID )
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(4,0, eDep);
    man->FillNtupleDColumn(4,1,evt2TrackTime);
    man->FillNtupleDColumn(4,2,evtTime); 
    man->FillNtupleDColumn(4,3,trackTime);
    man->FillNtupleDColumn(4,4,ecarbon);
    man->FillNtupleDColumn(4,5,ealpha);
    man->FillNtupleDColumn(4,6,eproton);
    man->FillNtupleDColumn(4,7,eelectron); 
    man->FillNtupleIColumn(4,8,stepNum); 
    man->FillNtupleIColumn(4,9,evtID); 
    man->AddNtupleRow(4);

} 


//-------------------------------------------------------------------------
//Per Event
void Analysis::FillCrProcessE(G4double  eDep, G4double nCapE, G4double nInelasticE,
            G4double inelasticE, G4double comptE, G4double photE, G4double eIoniE, 
            G4double hIoniE, G4double ionIoniE, G4double eBremE, G4double hadElasticE, 
            G4double convE, G4double mscE, G4double annihilE, G4double decayE, G4int evtID)
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(3,0,eDep);
    man->FillNtupleDColumn(3,1,nCapE);
    man->FillNtupleDColumn(3,2,nInelasticE);
    man->FillNtupleDColumn(3,3,inelasticE);
    man->FillNtupleDColumn(3,4,comptE);
    man->FillNtupleDColumn(3,5,photE); 
    man->FillNtupleDColumn(3,6,eIoniE);
    man->FillNtupleDColumn(3,7,hIoniE); 
    man->FillNtupleDColumn(3,8,ionIoniE); 
    man->FillNtupleDColumn(3,9,eBremE); 
    man->FillNtupleDColumn(3,10,hadElasticE); 
    man->FillNtupleDColumn(3,11,convE); 
    man->FillNtupleDColumn(3,12,mscE); 
    man->FillNtupleDColumn(3,13,annihilE); 
    man->FillNtupleDColumn(3,14,decayE);
    man->FillNtupleIColumn(3,15, evtID);
    man->AddNtupleRow(3);     
}
                          

//-----------------------------------------------------------------------------
//Per Event
void Analysis::FillProcessE(G4double  eDep, G4double nCapE, G4double nInelasticE,
            G4double inelasticE, G4double comptE, G4double photE, G4double eIoniE, 
            G4double hIoniE, G4double ionIoniE, G4double eBremE, G4double hadElasticE, 
            G4double convE, G4double mscE, G4double annihilE, G4double decayE, G4int evtID)
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(2,0, eDep);
    man->FillNtupleDColumn(2,1,nCapE);
    man->FillNtupleDColumn(2,2,nInelasticE);
    man->FillNtupleDColumn(2,3,inelasticE);
    man->FillNtupleDColumn(2,4,comptE);
    man->FillNtupleDColumn(2,5,photE); 
    man->FillNtupleDColumn(2,6,eIoniE);
    man->FillNtupleDColumn(2,7,hIoniE); 
    man->FillNtupleDColumn(2,8,ionIoniE); 
    man->FillNtupleDColumn(2,9,eBremE); 
    man->FillNtupleDColumn(2,10,hadElasticE); 
    man->FillNtupleDColumn(2,11,convE); 
    man->FillNtupleDColumn(2,12,mscE); 
    man->FillNtupleDColumn(2,13,annihilE); 
    man->FillNtupleDColumn(2,14,decayE);
    man->FillNtupleIColumn(2,15, evtID);
    man->AddNtupleRow(2);   
    
}

//-----------------------------------------------------------------------------
//Per Event
void Analysis::FillPartE( G4double eDep,G4double eDepProt, G4double eDepElec,
                             G4double eDepAlpha, G4double eDepC12, G4double eDepBe,
                            G4int tableNPhot, G4int g4nPhot, G4int glg4nPhot, 
                            G4int g4MethodNPhot, G4int evtID)
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();

    // Fill histograms
    man->FillH1(1,eDep);
    //man->FillH1(2,eDep,nOpPhot);

    //man->FillH2(1,nOpPhot,eDep);
    //G4bool FillH1(G4int id, G4double value, G4double weight = 1.0);
    
    man->FillNtupleDColumn(1,0,eDep);
    man->FillNtupleDColumn(1,1,eDepProt);
    man->FillNtupleDColumn(1,2,eDepElec);
    man->FillNtupleDColumn(1,3,eDepAlpha);
    man->FillNtupleDColumn(1,4,eDepC12);
    man->FillNtupleDColumn(1,5,eDepBe);
    man->FillNtupleIColumn(1,6,tableNPhot); 
    man->FillNtupleIColumn(1,7,g4nPhot);
    man->FillNtupleIColumn(1,8,glg4nPhot);
    man->FillNtupleIColumn(1,9,g4MethodNPhot);                       
    man->FillNtupleIColumn(1,10,evtID); 
    man->AddNtupleRow(1);
    
}

//-----------------------------------------------------------------

void Analysis::WriteAscii(  G4double eDep,G4double eDepProt, G4double eDepElec,
							G4double eDepAlpha, G4double eDepC12, G4double eDepBe,
							G4int tableNPhot, G4int g4nPhot, G4int glg4nPhot, 
                            G4int g4MethodNPhot, G4int evtID )
{
        // Write to file
    G4int w = 10;
    G4int p = 5;
    if(textFile->is_open()) 
	{
        (*textFile) << " E: "   << std::setw(w) << std::fixed << std::right 
                    << std::setprecision(p)     << eDep 
                    
                    << " tableNPhot: "<< std::setw(w) << std::fixed << std::right
                    << tableNPhot    
                    
                    << " g4nPhot: "<< std::setw(w) << std::fixed << std::right
                    << g4nPhot  
                    
                    << " glg4nPhot: "<< std::setw(w) << std::fixed << std::right
                    << glg4nPhot
                    
                    << " g4MethodNPhot: "<< std::setw(w) << std::fixed << std::right
                    << g4MethodNPhot   
                    
                    << " protE: "<< std::setw(w) << std::fixed << std::right
                    << eDepProt  
                    
                    << " ElecE: "<< std::setw(w) << std::fixed << std::right
                    << eDepElec  
                    
                    << " AlphaE: "<< std::setw(w) << std::fixed << std::right
                    << eDepAlpha  
                    
                    << " C12E: "<< std::setw(w) << std::fixed << std::right
                    << eDepC12  
                    
                    << " BeE: "<< std::setw(w) << std::fixed << std::right
                    << eDepBe           
                    
                    << " evt: " << std::setw(w) << std::fixed << std::right 
                    << evtID
                    << G4endl;
    }
      
}// writeText


//-----------------------------------------------------------------------------

void Analysis::Finish() 
{  
 if (fInstance) 
   {
      /*if(textFile->is_open())
      {
          textFile->close();
          delete textFile;
      }*/
      G4AnalysisManager* man = G4AnalysisManager::Instance();
      man->Write();
      man->CloseFile();
      // Complete clean-up
      delete G4AnalysisManager::Instance();

   }
}

//-----------------------------------------------------------------------------

Analysis* Analysis::GetInstance()
{
    if(! fInstance) 
		fInstance = new Analysis;
    return fInstance;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

void Analysis::WriteAllSteps(const G4Step* aStep)
{
	
	// G4AnalysisManager* man = G4AnalysisManager::Instance();
	// G4double xx = 0.;
	// G4double yy = 0.;
	// G4double zz = 0.;     
	
	
	/*
	 *    const DetectorConstruction* detectorConstruction
	 *      = static_cast<const DetectorConstruction*>
	 *        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	 */
	
	// get volume of the current aStep
	//G4LogicalVolume* volume = aStep->GetPreStepPoint()->GetTouchableHandle()
	//   						->GetVolume()->GetLogicalVolume();
	
	//G4double eDepStep = aStep->GetTotalEnergyDeposit();
	
	G4ThreeVector postPos = aStep->GetPostStepPoint()->GetPosition();
	G4String thePostPVname = 
	aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
	G4String thePostLogVname = 
	aStep->GetPostStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName();
	G4ThreeVector prePos = aStep->GetPreStepPoint()->GetPosition();
	G4String thePrePVname = 
	aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
	G4String thePreLogVname = 
	aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName();
	
	//G4Track* aTrack = aStep->GetTrack();
	//G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aTrack->GetTouchable());
	
	//G4int thePDGcharge = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
	//G4double aStepLength = aStep->GetStepLength();
	// Alive
	//if(theTrack-> GetTrackStatus() == fAlive) {return;} //????????????
	/* 
	 *  //Secondaries
	 *  	G4TrackVector* fSecondary = fpSteppingManager->GetfSecondary();
	 *     
	 *   for(size_t i=0; i<(*fSecondary).size(); i++)
	 *   { 
	 *      const G4ParticleDefinition* particleName = (*fSecondary)[i]->GetDefinition();     
	 * 
	 *      if (particleName == G4Gamma::Definition())
	 *      {
	 *        G4String process = (*fSecondary)[i]->GetCreatorProcess()->GetProcessName();  
	 *        if (process == "RadioactiveDecay")
	 *		{
	 *          G4double en = (*fSecondary)[i]->GetKineticEnergy();
	 *          analysisMan->FillHisto(en/MeV);
	 }//if process
	 }//if particleName
	 }//for
	 */
	
	 }
	 
	 
	 
	 //--------------------------------------------
	 /* 
	  *  
	  *  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	  *  analysisManager->SetActivation(true);
	  *  G4int histId = analysisManager->CreateH1(name, title, nbins, vmin, vmax);
	  *  analysisManager->SetH1Activation(histId, false);
	  *  
	  *  analysisManager->GetH1(1)
	  *  analysisManager->GetH1(1)->mean()
	  *  analysisManager->GetH1(1)->rms()
	  *  
	  *  G4Ntuple* ntuple = analysisManager->GetNtuple(ntupleId);
	  *  
	  */
	 
	 
	 /*  
	  / / *Define histograms start values  
	  // Initialization
	  // histograms
	  for (G4int k=0; k<MaxHisto; k++) fHistId[k] = 0;
	  
	  // Initialization ntuple
	  for (G4int k=0; k<MaxNtCol; k++) {
		  fNtColId[k] = 0;
		  }  
		  
		  const G4int kMaxHisto = 9;
		  const G4String id[] = {"0","1","2","3","4","5","6","7","8"};
		  const G4String title[] = 
		  { "dummy",                                    //0
		  "energy spectrum (%): e+ e-",               //1
		  "energy spectrum (%): nu_e anti_nu_e",      //2
		  "energy spectrum (%): gamma",               //3                  
		  "energy spectrum (%): alpha",               //4
		  "energy spectrum (%): ions",                //5
		  "total kinetic energy (Q)",                 //6                        
		  "momentum balance",                         //7
		  "total time of life of decay chain"         //8
		  };  
		  
		  // Default values (to be reset via /analysis/h1/set command)               
		  G4int nbins = 100;
		  G4double vmin = 0.;
		  G4double vmax = 100.;
		  
		  // Create all histograms as inactivated 
		  // as we have not yet set nbins, vmin, vmax
		  for (G4int k=0; k<kMaxHisto; k++) {
			  G4int ih = man->CreateH1(id[k], title[k], nbins, vmin, vmax);
			  man->SetH1Activation(ih, false);
			  }
		*/
	 
	 // Book 1D histograms
	 //man->CreateH1("ScintEDep","EDep (MeV)", 100,0.,20.);
	 //man->CreateH1("Weighted Edep","Edep*Scint_Photons per Event", 100,0,500);
	 //man->CreateH1("eDep_step","eDep per Step", 100,0,18.);
	 //man->CreateH1("PhotTime","Photon Global-LocalTrack time(ns)", 200,0.,2000.);  
	 //man->CreateH1("GlobTime","Photon Global Time(ns)", 200,0.,2000.);
	 //man->CreateH1("TrackTime","Photon Track(local) Time(ns)", 200,0.,2000.);  
	 
	 
	 // man->SetH1XAxisTitle(1, "E Deposited (MeV)");//G4int id, const G4String& title);
	 // man->SetH1YAxisTitle(1, "Events");//G4int id, const G4String& title);
	 /*man->SetH1XAxisTitle(2, "Num of Photons in each event");
	  * man->SetH1YAxisTitle(2, "Events");
	  * man->SetH1XAxisTitle(3, "edep per step (MeV)");
	  * man->SetH1YAxisTitle(3, "Events");
	  * man->SetH1XAxisTitle(4, "Photon time (from neutron event to phot track creation(ns)");
	  * man->SetH1YAxisTitle(4, "Events");
	  * man->SetH1XAxisTitle(5, "Global time (since neutron event creation)(ns)");
	  * man->SetH1YAxisTitle(5, "Events");
	  * man->SetH1XAxisTitle(6, "Photon Time (since Photon Track creation(ns)");
	  * man->SetH1YAxisTitle(6, "Events");
	  */
	 // Book 2D histograms
	 //man->CreateH2("E-Num", "E-num", 200,0.,15.,200,0,1000); 
	 //man->CreateH2("Num-Time", "Num-Time", 200,0,1000,200,0,2000.);  
	 
	 //man->SetH2XAxisTitle(1, "Energy Deposition (MeV)");
	 //man->SetH2YAxisTitle(1, "Number");
	 //man->SetH2XAxisTitle(2, "Time (Event-to-Track) of Photons(ns)");
	 //man->SetH2YAxisTitle(2, "Number");
	 