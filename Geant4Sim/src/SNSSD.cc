// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#include "SNSSD.hh"
#include "SNSHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4VProcess.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4OpticalPhoton.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VAnalysisManager.hh"
#include "G4LossTableManager.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4Electron.hh"
#include "G4Proton.hh"

//#include "TSpline.h"
//#include "TGraph.h"
#include "TMath.h"
#include "G4IonTable.hh"
#include "G4hZiegler1985Nuclear.hh"
#include "G4hZiegler1985p.hh"
#include "G4hIonEffChargeSquare.hh"
#include "G4hParametrisedLossModel.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4PSTARStopping.hh"//NIST electronic dE/dx for protons, up to 2 MeV
//#include "TF1.h"
#include "G4AtomicShells.hh"
#include "G4ParticleTable.hh"


#include <iomanip>
#include <iostream>
#include <vector>
#include "Analysis.hh"
#include "G4ios.hh"         

G4int SNSSD::fHCid = -1; //static
    
//------------------------------------------------------------------------   

SNSSD::SNSSD( const G4String& sdName, const G4String& hitsCollectionName )
: G4VSensitiveDetector(sdName),
fHitsCollection(NULL), fThresholdEDep(0.), fVerbose(0), proton(0)
{
	collectionName.insert(hitsCollectionName);
	//NOTE: don't get the collection ID here, but do it in initialize method
    
}

//------------------------------------------------------------------------

SNSSD::~SNSSD() 
{ 
}

//------------------------------------------------------------------------

void SNSSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection 
      = new SNSHitsCollection(SensitiveDetectorName, collectionName[0]); 
	
    if(fHCid < 0)
    {    
        fHCid = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    }
    hce->AddHitsCollection( fHCid, fHitsCollection );     
}

//-----------------------------------------------------------------------
//only SD vol hits
G4bool SNSSD::ProcessHits( G4Step* aStep, G4TouchableHistory* )
{  
	G4Track *track = aStep->GetTrack();
	if(track->GetDefinition() == G4OpticalPhoton::Definition()) 
		return false; //opticalphoton
		
	//G4bool fWriteAllSteps = false;
fVerbose = 0;

	G4double eDep = (aStep->GetTotalEnergyDeposit()
					- aStep->GetNonIonizingEnergyDeposit() )/MeV;
	if ( eDep == 0 ) return false;


	G4ParticleDefinition* particleType = track->GetDefinition();
	G4String particleName 	=  particleType->GetParticleName();
	G4double partKE			=  track->GetKineticEnergy();
	//G4double partKE		=  aStep->GetPreStepPoint()->GetKineticEnergy();
    //G4double charge = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
    //if (charge==0.) return true;
	// post-step point = for currentstepG4std::
	G4StepPoint* preStep = aStep->GetPreStepPoint();
	G4StepPoint* postStep = aStep->GetPostStepPoint();
	const G4VProcess *postProcStep = postStep->GetProcessDefinedStep();
	G4int nG4OpPhot = 0;
	G4double secTotKE = 0.;	
	G4double secTotEn = 0.;
	G4String postProc = "";
	G4String preProc = "";
	G4String photCreatorProc = "";
	G4String trackCreatorProc = "";
	
	if(postProcStep)
	{
		postProc = postProcStep->GetProcessName();
		if(fVerbose>2) G4cout << " SD:postprcess: " << postProc << G4endl;
	} else 	{ if(fVerbose>2) G4cout << " SD:postprcess: NONE "; 	}
	
	if(preStep->GetProcessDefinedStep())
	{
		preProc = preStep->GetProcessDefinedStep()->GetProcessName();
		if(fVerbose>2) G4cout << " SD:prestep.proc:" << preProc << " ";		
	}else {if(fVerbose>2) G4cout << " SD:prestep.proc: NONE ";}
	
	const G4VProcess* creatorProc = track->GetCreatorProcess();
	if(creatorProc)
	{
		trackCreatorProc = creatorProc->GetProcessName();
		if(fVerbose>2)G4cout<< " SD:track:creator.proc:" << trackCreatorProc << " ";
	}else { if(fVerbose>2) G4cout << " SD:track:creator.proc: NONE ";}
	
	
	//track->GetDynamicParticle()->GetTotalEnergy();

	const std::vector<const G4Track*>* secondaries = aStep->GetSecondaryInCurrentStep();
    if(fVerbose>2) G4cout << " SD:sec.loop.size: " <<secondaries->size() << G4endl; 
    
    G4bool flagPhot = true;
    
	if (secondaries->size()>0) 
	{
		for(std::size_t i=0; i<secondaries->size(); ++i) 
		{
			if(secondaries->at(i)->GetDefinition()
					== G4OpticalPhoton::OpticalPhotonDefinition())
			{
				nG4OpPhot++;
   				if(flagPhot  && fVerbose>2) 
                {
                    photCreatorProc = secondaries->at(i)->GetCreatorProcess()->GetProcessName();
					G4cout<< "SD::secPhoton: creator proc :" <<photCreatorProc << G4endl;
                    flagPhot = false;
                }
				//secondaries->at(i)->GetDynamicParticle()->GetKineticEnergy();
				//GetTotalEnergy();
			}
			else
			{
				secTotKE = secTotKE +  secondaries->at(i)->
							GetDynamicParticle()->GetKineticEnergy();
				secTotEn = secTotEn + secondaries->at(i)->GetDynamicParticle()
							->GetTotalEnergy();
				if(fVerbose>1) 
                    G4cout<< " part: " << secondaries->at(i)->GetDefinition()->GetParticleName()
                          << " KE: " << secondaries->at(i)->
                            GetDynamicParticle()->GetKineticEnergy()
                          << G4endl;
			}
		}//for
	}//secondaries  
	
	if(nG4OpPhot >1)
	{
		if(photCreatorProc != "Scintillation") 
			G4cout<< " SD:photon creator process not defined ! ";
	}	
	
	//calculate from interpolated data
	G4int nCalcPhot = CalculateLY(particleName, eDep);
	G4int glg4Nphot = (G4int) GLG4Scint(aStep);
	G4int g4MethodNphot = (G4int ) ( BirksAttenuation(aStep) * 12000.);
	
	
    G4TouchableHistory* touchable 	=
            (G4TouchableHistory*)(aStep->GetPostStepPoint()->GetTouchable());
    G4VPhysicalVolume* thePostPV 	= touchable->GetVolume();
    G4ThreeVector globalPos 		= postStep->GetPosition();
    G4ThreeVector localPos			=
			touchable->GetHistory()->GetTopTransform().TransformPoint(globalPos);
    //G4LogicalVolume* logVolume = thePostPV->GetLogicalVolume();      
	//G4ThreeVector position = preStep->GetPosition() + thePostStepPoint->GetPosition();
    // position = position/2.;
	SNSHit* newHit = new SNSHit(); //not for optG4std::icalphoton ?
	newHit->SetParticle(particleName);
	newHit->SetEdep(eDep);  
	newHit->SetTableNPhot(nCalcPhot);
    newHit->SetG4nPhot(nG4OpPhot);
    newHit->SetGLG4nPhot(glg4Nphot);
    newHit->SetG4MethodNPhot(g4MethodNphot);
	newHit->SetPostProc(postProc);
	newHit->SetPreProc(preProc);
	newHit->SetTrackCreatorProc(trackCreatorProc);	
	newHit->SetPartKE(partKE);
	newHit->SetSecTotKE(secTotKE);
	newHit->SetSecTotEn(secTotEn);
    newHit->SetTime(postStep->GetGlobalTime());	
    newHit->SetGlobalPos(globalPos);
	newHit->SetLocalPos(localPos);
    newHit->SetPhysVol(thePostPV);
	fHitsCollection->insert(newHit);

	G4int p = 5;
    G4int w = 10;
G4bool part = true;
//if(particleName != "proton") part = false;
fVerbose = 0;
    if(fVerbose && part)
    {
        //G4double trL = track->GetTrackLength()/um;
        //G4double stL = aStep->GetStepLength()/um;
        
        G4cout<< std::setw(w) << std::fixed << std::right << particleName
          << "   E: "
		  << std::setw(w) << std::fixed << std::right << std::setprecision(p) << eDep 
          << "  G4: " 
          << std::setw(4) << std::fixed << std::right << nG4OpPhot
          << "  table: "  
          << std::setw(4) << std::fixed << std::right << nCalcPhot
          << "  GLG4: "
          << std::setw(w) << std::fixed << std::right << std::setprecision(0)         
          << glg4Nphot
          << " G4method: "
          << std::setw(w) << std::fixed << std::right << std::setprecision(0)         
          << g4MethodNphot 
//           << "  stL: "                                
//           << std::setw(w) << std::fixed << std::right << std::setprecision(p)<<stL 
//           << " tr2st: "
//           << std::setw(w) << std::fixed << std::right << std::setprecision(1)<<trL/stL
//           << "  check: "
//           << std::setw(4) << std::fixed << std::right << CrossCheckLY(particleName, eDep)
//           << "  post: "  
//           << std::setw(w) << std::fixed << std::right 
//           << postProc
//           << " preProc: " << preProc
//           << "  cr: "
//           << std::setw(w) << std::fixed << std::right 
//           << trackCreatorProc 
//          << std::setw(w) << std::fixed << std::right 
//           << " pKE: "out
//           << std::setw(w) << std::fixed << std::right << std::setprecision(p) 
// 		  << partKE
//           << std::setw(w) << std::fixed << std::right 
//           << " secKE: " 
//           << std::setw(w) << std::fixed << std::right << std::setprecision(p) 
// 		  << secTotKE
//           << std::setw(w) << std::fixed << std::right << " secEn: " 
//           << std::setw(w) << std::fixed << std::right << std::setprecision(p) << secTotEn 
// 		<< " time: "  << postStep->GetGlobalTime()	
          << G4endl;
    }

	
	return true;
}


//----------------------------------------------------------------------------



G4double SNSSD::BirksAttenuation(const G4Step* aStep)
{
 //from G4source
 
 	G4Material* material = aStep->GetTrack()->GetMaterial();
 	G4double edep        = aStep->GetTotalEnergyDeposit();
 	G4double length      = aStep->GetStepLength();  
 	G4double niel        = aStep->GetNonIonizingEnergyDeposit();

 
 	if(edep <= 0.0) { return 0.0; }
 	G4double evis = edep;
	
	const G4ParticleDefinition* p = aStep->GetTrack()->GetParticleDefinition();
	const G4MaterialCutsCouple* couple = aStep->GetTrack()->GetMaterialCutsCouple();
	G4LossTableManager*   manager = G4LossTableManager::Instance();
	const G4ParticleDefinition* electron = G4Electron::Electron();
	G4double bfactor = material->GetIonisation()->GetBirksConstant(); //0.152*mm/MeV; 
	//FindBirksCoefficient(couple->GetMaterial());
 
 	if(bfactor > 0.0) { 
		G4int pdgCode = p->GetPDGEncoding();
	 	// atomic relaxations for gamma incident
	 	if(22 == pdgCode) {
			evis /= (1.0 + bfactor*edep/manager->GetRange(electron,edep,couple));
	 	} else { //energy loss
	 		// protections
	 		G4double nloss = niel;
	 		if(nloss < 0.0) nloss = 0.0;
	 		G4double eloss = edep - nloss;
	 
	    	// neutrons
	    	if(2112 == pdgCode || eloss < 0.0 || length <= 0.0) {
		    	nloss = edep;
		    	eloss = 0.0;
		 	}
		 
		 	// continues energy loss
		 	if(eloss > 0.0) { eloss /= (1.0 + bfactor*eloss/length); }
		  	// non-ionizing energy loss
		  	if(nloss > 0.0) {
				// compute mean mass ratio
				G4double curRatio 		= 0.0;
				G4double curChargeSq 	= 0.0;
				G4NistManager* nist = G4NistManager::Instance();	
				
				G4double norm = 0.0;
				const G4ElementVector* theElementVector = material->GetElementVector();
				const G4double* theAtomNumDensityVector = material->GetVecNbOfAtomsPerVolume();
				size_t nelm = material->GetNumberOfElements();
				for (size_t i=0; i<nelm; ++i) 
				{
					const G4Element* elm = (*theElementVector)[i];
					G4double Z = elm->GetZ();
					G4double w = Z*Z*theAtomNumDensityVector[i];
					curRatio += w/nist->GetAtomicMassAmu(G4int(Z));
					curChargeSq = Z*Z*w;
					norm += w;
				}
				curRatio *= proton_mass_c2/norm;
				curChargeSq /= norm;
				//G4cout << " ratio: "<< curRatio << " chargesq: " << curChargeSq;
	
				
				if(!proton) { proton = G4Proton::Proton(); }
		       	G4double escaled = nloss*curRatio;
				G4double range = 1.0;
				if(curChargeSq != 0)
		       		range = manager->GetRange(proton,escaled,couple)/curChargeSq; 
			   	nloss /= (1.0 + bfactor*nloss/range);
			}
			 
		evis = eloss + nloss;
		}
	}
	   
	return evis;
}

//--------------------------------------------------------------------------
G4double SNSSD::LinearInterp( G4double input, std::vector <G4double> dataX, 
                           std::vector <G4double>dataY, G4bool extrapolate=true)
{
    G4double low = -1.;
    G4double high = -1.;
    G4double res = -1.;
    G4double lowY = -1., highY = -1.; 
    G4double n = dataX.size()-1;
    
    //TODO:check if data sorted in ascending order, log scale .....
        
    
    //fVerbose = 2;    

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


//--------------------------------------------------------------------------
G4int SNSSD::CalculateLY(G4String particle, G4double inputE)
{
	//table for calculation
	const G4double tableEarr[] = {0.10, 0.13, 0.17, 0.20, 0.24, 0.30, 0.34, 0.40, 
		0.48, 0.60, 0.72, 0.84, 1.00, 1.30, 1.70, 2.00, 2.40, 3.00, 3.40, 4.00, 
		4.80, 6.00, 7.20, 8.40, 10.00, 13.00, 17.00, 20.00, 24.00,30.00,34.00,40.00};
        
	static const std::vector<G4double> tableE(tableEarr, tableEarr+sizeof(tableEarr)/
					sizeof(tableEarr[0]));
                    
	const G4double protonLYarr[] = { 0.00671, 0.00886, 0.01207, 0.01465, 0.01838, 
		0.0246, 0.029, 0.0365, 0.0483, 0.0678, 0.091, 0.1175, 0.1562, 0.2385, 
		0.366, 0.47250, 0.625, 0.866, 1.042, 1.327, 1.718, 2.31, 2.95, 3.62, 
		4.55, 6.36, 8.83, 10.8, 13.5, 17.7, 20.5, 24.8};

	static const std::vector<G4double> protonLY(protonLYarr, protonLYarr+
								sizeof(protonLYarr)/sizeof(protonLYarr[0]));
	
	const G4double carbonLYarr[] = {0.001038, 0.001270, 0.001573, 0.001788, 
        0.002076, 0.002506, 0.002793, 0.003191, 0.003676, 0.004361, 0.005023, 
        0.005686, 0.006569, 0.008128, 0.010157, 0.011647, 0.013634, 0.016615, 
        0.018713, 0.021859, 0.026054, 0.032347, 0.038750, 0.045154, 0.053986, 
		0.071346, 0.098808, 0.121440, 0.153456, 0.206448, 0.246192, 0.312432 };
		
	static const std::vector<G4double> carbonLY(carbonLYarr, carbonLYarr+
								sizeof(carbonLYarr)/sizeof(carbonLYarr[0]));	

	const G4double alphaLYarr[] = { 0.00164, 0.00209, 0.00272, 0.00320, 0.00386, 
        0.0049, 0.00564, 0.00675, 0.0083, 0.0108, 0.0135, 0.01656, 0.021, 0.0302, 
        0.0441, 0.0562, 0.075, 0.11, 0.1365, 0.1815, 0.2555, 0.407, 0.607, 0.87, 
        1.32, 2.35, 4.03, 5.44, 7.41, 10.42, 12.44, 15.5 };
	
	static const std::vector<G4double> alphaLY(alphaLYarr, alphaLYarr+
                        sizeof(alphaLYarr)/sizeof(alphaLYarr[0]));	
		
  	G4double resultY     = 0;
	G4double electronLY  = 12000.;
	G4double conversion  = 1.28*electronLY;
	G4double pulseHtLY   = 0;
    G4bool validParticle = false;
    G4bool extrapolate   = false;
    
extrapolate = true;
fVerbose = 0;

  	if(particle == "C12"|| particle == "Be9")
    {
        if( inputE >= tableE[0])
            pulseHtLY  = LinearInterp( inputE, tableE, carbonLY, extrapolate );
        else pulseHtLY = inputE * carbonLY[0]/tableE[0];
        
		resultY = conversion * pulseHtLY;
        validParticle = true;
    }
  	else if(particle == "alpha")
    {
        if( inputE >= tableE[0])
            pulseHtLY    = LinearInterp( inputE, tableE, alphaLY, extrapolate );
        else pulseHtLY = inputE * alphaLY[0]/tableE[0];   
        
		resultY = conversion * pulseHtLY;  
        validParticle = true;
    }
  	else if(particle == "proton" || particle =="deutron" || particle == "triton")
	{
        if( inputE >= tableE[0])
          pulseHtLY    = LinearInterp( inputE, tableE, protonLY, extrapolate );
        else pulseHtLY = inputE * protonLY[0]/tableE[0];
   //G4cout  << std::setw(9) << std::fixed << std::right << std::setprecision(6)<< pulseHtLY ; 
		resultY = conversion * pulseHtLY;
        validParticle = true;
	}
	else if(particle == "e-"|| particle == "e+" || particle == "gamma")
    {
		resultY = inputE * electronLY;
        validParticle = true;
    }        
  	else 
    { 
      if(fVerbose>1) G4cout << "*** Error in interpolation of particle : "
          << particle<< G4endl;
    }
    

    if( validParticle )
    {
        if(fVerbose > 2) 
        G4cout << std::setw(7) << std::fixed << std::right << particle  
               << std::setw(9) << std::fixed << std::right  
               << std::setprecision(5)<< pulseHtLY 
               << " *"<< std::setw(6)<< std::fixed << std::right 
               << std::setprecision(0) << conversion;
    }
    
 	return (int)resultY;
}

//-------------------------------------------------------------------------



//valence e- for the 92 natural elements, counting the d e- as core
//FIXME: for now d e- are never valence
const int ElVal[92]={
    1,2,//1st period
    1,2,3,4,5,6,7,8,//2nd period
    1,2,3,4,5,6,7,8,//3rd period
    1,2,2,2,2,2,2,2,2,2,2,2,3,4,5,6,7,8,//4th period
    1,2,2,2,2,2,2,2,2,2,2,2,3,4,5,6,7,8,//5th period
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,4,5,6,7,8,//6th period
    1,2,2,2,2,2//7th period
};

int ValEl(int Z)
{
  return ElVal[Z-1];
}

//ion effective charge used for QF according to D.-M. Mei, Z.-B. Yin, L.C. Stonehill and A. Hime, Astrop. Phys. 30 (2008) 12
Double_t Lambda(Double_t x, Double_t z1){
  return  .48*TMath::Power(1.-x, 2./3)/TMath::Power(z1, 1./3)/(1.-(1.-x)/7.);
}


Double_t ZEff(Double_t v1, Double_t z1, Double_t rho, Double_t a, Double_t ne)
{
  Double_t v0 = 2.18e6;           //Bhor velocity in m/s
  //Double_t pi = TMath::Pi();
  Double_t Na = TMath::Na();
  Double_t n = ne*Na*rho/a*1.e6;  // in /m3
  Double_t hbar = TMath::Hbar();
  Double_t me = 9.1093826E-31;
  Double_t vf = TMath::Power(3.*pi*pi*n, 1./3)*hbar/me;  //in m/s
  Double_t vr;
 
  Double_t Rv1vf = v1/vf;
  if (v1 <= vf){
    vr = 0.75*vf*(1.+2./3*Rv1vf*Rv1vf-1./15*Rv1vf*Rv1vf*Rv1vf*Rv1vf);
  }
  else
    vr = v1*(1.+0.2/Rv1vf/Rv1vf);

  Double_t yr = vr/v0/TMath::Power(z1, 2./3);

  Double_t q = 1.-1.00935*TMath::Exp(-0.92447*yr); //fun->Eval(yr);
  Double_t lambda = Lambda(q, z1);
  return z1*(q+0.5*(1.-q)*TMath::Log(1.+4.*lambda*lambda*vf*vf/v0/v0));
}


//---------------------------------------------------------------------------

G4double SNSSD::GLG4Scint( const G4Step* aStep)
{
    G4Track* aTrack             =   aStep->GetTrack();
    G4double TotalEnergyDeposit =   aStep->GetTotalEnergyDeposit();
    if (TotalEnergyDeposit <= 0.0 ) return 0;
    const G4Material* aMaterial = aTrack->GetMaterial();

    G4hParametrisedLossModel* ele_dEdx = new G4hParametrisedLossModel("ICRU_R49p");
    //to calculate Dongming et al. QF
    
    
    //from RAT GLG4Scint by Glenn Horton-Smith 
    //'energy' quenching factor using Birk's law (D.-M. Mei, Z.-B. Yin, L.C. Stonehill 
    //and A. Hime, Astrop. Phys. 30 (2008) 12)
    //finds E-dependent QF, unless the user provided an E-independent one and 
    //provided that we've got the QF data
    
    /*
          G4double CurrentEnergy=physicsEntry->QuenchingArray->GetPhotonEnergy();
          G4double qvalue1 = physicsEntry->QuenchingArray->GetProperty(CurrentEnergy);
          G4double qvalue2 = physicsEntry->QuenchingArray->GetProperty(PreviousEnergy);        
          G4double PreviousEnergy=CurrentEnergy;
          G4double VertexEn=aTrack.GetVertexKineticEnergy();
          G4double slope;
          //if the primary is below the energy range with a QF, then use QF of lowest energy
          if(VertexEn < CurrentEnergy)
              SetQuenchingFactor(qvalue1);
          else { //find 1st energy above primary, if available
              while((VertexEn > CurrentEnergy) && ++(*physicsEntry->QuenchingArray)){
                  PreviousEnergy=CurrentEnergy;
                  CurrentEnergy=physicsEntMeanNumPhotonsry->QuenchingArray->GetPhotonEnergy();
              }
              //if primary energy above range or in quenching array, use QF of last energy
            if(VertexEn >= CurrentEnergy)
                SetQuenchingFactor(qvalue1));
            else{//otherwise interpolates QF
                slope=(qvalue1- qvalue2) /(CurrentEnergy-PreviousEnergy);
                SetQuenchingFactor(slope*(VertexEn-PreviousEnergy)+qvalue2);
            }
          }
    }
    else{   */  


    //if no experimental QF dataset, try using arXiv:0712.2470
    G4double    DBirkFact  = 1.;
    //G4double quenchFactor  = 1.;
    G4double DMsConstant   = 0.00074;
    if(G4IonTable::IsIon(aTrack->GetDefinition()))
    {
        G4ParticleDefinition* parti = aTrack->GetDefinition();
        //first Lindhard's QF
        G4int Z            = parti->GetAtomicNumber();
        G4int A            = parti->GetAtomicMass();
        G4double Er        = aTrack->GetVertexKineticEnergy();
        G4double epsi      = 11500*Er/pow(Z,7./3);
        G4double QLind     = (epsi+.7*pow(epsi,0.6)+3*pow(epsi,.15))*.133
                            *pow(Z,2./3)/pow(A,.5);
        QLind              = QLind/(1+QLind);
      
        G4double DGdEdx       = 0;
     //determine the valence e-    
        G4double TotValElDens = 0;
        for(G4int ie = 0; ie < G4int(aMaterial->GetNumberOfElements()); ie++)
        {
          const G4Element* elem = (*(aMaterial->GetElementVector()))[ie];
          TotValElDens += ValEl(elem->GetZ())*aMaterial->GetVecNbOfAtomsPerVolume()[ie];
        }
        TotValElDens *= cm3/Avogadro;
        
        //ion's effective charge according to Astrop. Phys. 30 (2008) 12
        G4double ionmass    = parti->GetPDGMass();
        G4double beta       = TMath::Sqrt(Er*(Er+2.*ionmass))/(Er+ionmass);
        G4double DZeff      = ZEff(beta*c_light*s/m,Z,1.,1.,TotValElDens);
        if(DZeff > Z)DZeff = Z;
        
        //now dE/dx:
        DGdEdx  = DZeff*DZeff*(ele_dEdx->TheValue(parti,aMaterial,Er))
                    /(aMaterial->GetDensity()/g*cm2);
        DBirkFact = 1/(1 + DMsConstant*DGdEdx);
        //quenchFactor = QLind;
    }

    G4double  ScintillationYield = 12000./MeV; // check unit ??
    
     // figure out how many photons      

    G4double birksConstant = aMaterial->GetIonisation()->GetBirksConstant();
    G4double QuenchedTotalEnergyDeposit= TotalEnergyDeposit*DBirkFact;
    //Astrop. Phys. 30 (2008) 12 uses custom dE/dx, different from G4/Ziegler's
    G4double dE_dx = 0;
    if ( birksConstant != 0.0 )
      {
        dE_dx = TotalEnergyDeposit /  aStep->GetStepLength();
        QuenchedTotalEnergyDeposit/=
          (1.0 + birksConstant * dE_dx);          
      }
    //G4cout << " dE_dx: " << dE_dx;   
    G4double nPhot = ScintillationYield * QuenchedTotalEnergyDeposit;
    
    // randomize number of TRACKS (not photons)
    // this gets statistics right for number of PE after applying
    // boolean random choice to final absorbed track (change from
    // old method of applying binomial random choice to final absorbed
    // track, which did want poissonian number of photons divided
    // as evenly as possible into tracks)
    // Note for weight=1, there's no difference between tracks and photons.
   /* G4double MeanNumTracks= MeanNumPhotons/meanPhotonsPerSecondary
                         / RAT::PhotonThinning::GetFactor();
    
    G4double resolutionScale= physicsEntry->resolutionScale;
    G4int numSecondaries;
    if (MeanNumTracks > 12.0)
      numSecondaries=
        (G4int)(CLHEP::RandGauss::shoot(MeanNumTracks,
                        resolutionScale
                        * sqrt(MeanNumTracks)));
    else {
      if (resolutionScale > 1.0)
        MeanNumTracks = CLHEP::RandGauss::shoot
          (MeanNumTracks,
           sqrt( resolutionScale*resolutionScale-1.0 )*MeanNumTracks);
      numSecondaries=
        (G4int)( CLHEP::RandPoisson::shoot(MeanNumTracks) );
    }
  */
     
  return nPhot;//MeanNumPhotons;
}

//--------------------------------------------------------------------------
void SNSSD::EndOfEvent(G4HCofThisEvent*)
{
	if ( verboseLevel>1 ) { 
		G4int nofHits = fHitsCollection->entries();
		G4cout << "\n------>Hits Collection: in this event there are " << nofHits 
		<< " hits in the sensitive volume: " << G4endl;
		for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
	}
}

//-----------------------------------------------------------------------------

//If the option is false in SteppingAction::UserSteppingAction
/*
 * if(! fWriteAllSteps) 
 {  *
 Analysis* man = Analysis::GetInstance();
 if(man) man->WriteAllSteps(aStep);
 }
 
 
 store particles and KE for 1st step
 G4int StepNo = aStep->GetTrack()->GetCurrentStepNumber();
 if (secondaries->at(i)->GetParentID()>0) 
	 NOTE: dynamic particle has no position or geometrical info.
	 G4ProcessManager* pm
	 = aStep->GetTrack()->GetDefinition()->GetProcessManager();
 G4int nprocesses = pm->GetProcessListLength();
 G4ProcessVector* pv = pm->GetProcessList();
 
 
  G4AnalysisManager* g4man = G4AnalysisManager::Instance();
  g4man->FillH1(3, eDep);  //step energy  		
  g4man->FillNtupleDColumn(2,0, eDep);
  
 */