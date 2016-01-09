
//SteppingAction.cc


#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "Analysis.hh"
#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4ios.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SteppingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VParticleChange.hh"
#include "G4StepStatus.hh"
#include "G4VSolid.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"
#include "G4GeometryTolerance.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4VAnalysisManager.hh"
#include "G4DynamicParticle.hh"
//#include "G4PhysicsTable.hh"
//#include "G4ElementTable.hh"
//#include "G4NeutronHPData.hh"
//#include "G4VCrossSectionDataSet.hh"
#include "G4IStore.hh"
//#include <regex>
#include <string>
//===========================================================================

SteppingAction::SteppingAction(DetectorConstruction* det, EventAction* evt)
: G4UserSteppingAction()
, fEventAction(evt)
, fDetector(det)
, fSDvolume(NULL)
, fWriteAllSteps(false)
, fScintillationCounter(0)
, fCerenkovCounter(0)
, fEventID(-1)
, fSteppingVerbose(0)
{
  //get a messenger for fWriteAllSteps.
}

//===========================================================================

SteppingAction::~SteppingAction()
{}

//===========================================================================

void SteppingAction::UserSteppingAction(const G4Step* step)
{
	
	G4bool skipStepping = false; //set true if only SD is needed
	if ( skipStepping )return;

    G4Track* track = step->GetTrack();	
    G4String postVol    = "NIL";
    G4String preVol     = "NIL";
    G4String partName   = "NIL";
    G4int    pdgCode    = -999;
    G4String proc       = "NIL";
    G4double time       = -1;
    G4String postProc   = "NIL";
    G4String preProc    = "NIL";
    G4String trackProc  = "NIL";
    G4String trkNextVol = "NIL";
	G4double preKE = 0;


	if(step->GetPostStepPoint()->GetPhysicalVolume())
		postVol  = step->GetPostStepPoint()->GetPhysicalVolume()->GetName(); 
        //step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName();

	if(step->GetPreStepPoint()->GetPhysicalVolume())
		preVol  =  step->GetPreStepPoint()->GetPhysicalVolume()->GetName(); 

    if(track->GetDynamicParticle()->GetDefinition()) // GetParticleDefinition
    {
        partName = track->GetDynamicParticle()->GetDefinition()->GetParticleName();
    }
    if(step->GetPreStepPoint())
    {
        preKE = step->GetPreStepPoint()->GetKineticEnergy();
    }

	G4bool skipVolume = true;

	if(skipVolume)
	{
        if(!(postVol=="SNSDetectorCsI"|| postVol=="DetPolyShield"|| postVol=="DetLeadShield" 
            ||preVol=="SNSDetectorCsI"|| preVol=="DetPolyShield"|| preVol=="DetLeadShield"
            )) return;
        if( postVol != "SNSDetectorCsI" && partName !="neutron" )   return;
        //skip if not neutron in other volumes
    }

    if(step->GetTotalEnergyDeposit()<0.1*keV && partName !="neutron") return;  


    G4bool skipParticles = true; // all particles ?
    if(skipParticles && step->GetTrack()->GetDynamicParticle()->GetDefinition())
    {
        if( partName == "nu_e"|| partName =="nu_mu"|| partName == "anti_nu_mu" 
            ||partName == "anti_nu_e" || partName == "mu+" || partName =="pi+" 
            || partName =="pi-") // skipped previously e- e+, gamma
                return;
    }

    Analysis* man = Analysis::GetInstance();
    G4StepPoint* pre  = step->GetPreStepPoint();



    if(step->GetPostStepPoint()->GetProcessDefinedStep())
        proc =  step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

    if(step->GetPostStepPoint())
		time = track->GetGlobalTime(); //step->GetPostStepPoint()->GetGlobalTime();//
    if(track->GetDynamicParticle()->GetDefinition()) // GetParticleDefinition
    {
        pdgCode  = track->GetDynamicParticle()->GetDefinition()->GetPDGEncoding();
    }

    if( step->GetPostStepPoint()->GetProcessDefinedStep())
        postProc = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

    if( step->GetPreStepPoint()->GetProcessDefinedStep())
        preProc = step->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName();

    if(track->GetCreatorProcess())
        trackProc =  track->GetCreatorProcess()->GetProcessName();

	G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();  

	
	//############################################
	//In SNS Detector
 
	G4bool snsDetectorCsI = true;  // detector
	G4bool verbDetCsIInfo = false; 	//text output
    G4int volumeID = -1;    
	if( snsDetectorCsI && (postVol=="SNSDetectorCsI" 
	|| postVol=="DetPolyShield" || postVol=="DetLeadShield") )// && partName == "neutron"))
	{
		//if(	!(partName == "gamma"|| partName == "e-"))
		//{
			G4double trackKE = track->GetKineticEnergy();
			G4int stepNum   = track->GetCurrentStepNumber();
            G4int evtid     = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
            G4int trackid   = track->GetTrackID();
			G4int parentID  = track->GetParentID();

			G4double xx = track->GetPosition().x(); //trackpos= postpos
			G4double yy = track->GetPosition().y();
			G4double zz = track->GetPosition().z();
			G4double postX = step->GetPostStepPoint()->GetPosition().x();
			G4double postY = step->GetPostStepPoint()->GetPosition().y();				
			G4double postZ = step->GetPostStepPoint()->GetPosition().z();
			G4double weight = track->GetWeight() ;//step->GetPreStepPoint()->GetWeight();
			G4double eDep =0;
			
            if(partName == "neutron")// && step->GetPostStepPoint()->GetStepStatus()==fGeomBoundary ) 
            {//fGeomBoundary =1, from G4StepStatus.hh.  use poststep, not pre

                if(postVol=="SNSDetectorCsI" && preVol != "SNSDetectorCsI" ) volumeID = 1;
                else if( postVol == "DetLeadShield" &&  !(preVol == "SNSDetectorCsI" 
                        || preVol == "DetLeadShield"))  volumeID = 2;
                else if( postVol == "DetPolyShield"&&  !(preVol == "SNSDetectorCsI" 
                        || preVol == "DetPolyShield" || preVol == "DetLeadShield"))volumeID = 3;
                //else if( postVol == "DetWaterShield"&&  !(preVol == "SNSDetectorCsI" 
                //   || preVol == "DetLeadShield" ||preVol == "DetPolyShield" ))volumeID = 4;    
                //else if( preVol == "MonolithSteelCyl")volumeID = 5;//NOTE
                //xx = (aTrack->GetVertexPosition()).x();
                //yy = (aTrack->GetVertexPosition()).y();
                //zz = (aTrack->GetVertexPosition()).z();
				
				G4double preX = step->GetPreStepPoint()->GetPosition().x();
				G4double preY = step->GetPreStepPoint()->GetPosition().y();				
				G4double preZ = step->GetPreStepPoint()->GetPosition().z();
				//pass without interaction
                if(step->GetPostStepPoint()->GetStepStatus()==fGeomBoundary &&  
                    preVol == "DetLeadShield" && postVol == "DetLeadShield")
                {   
                    volumeID = 1;
                    G4cout  << " Passed vol1 ...... " << G4endl;
                }

                if(track->GetNextVolume() == 0) volumeID = 0;

                if(volumeID > -1)
					man->FillSNSneutronFlux(volumeID, preKE/MeV , trackKE/MeV, pdgCode, weight, xx/m,  
                    yy/m, zz/m, evtid, trackid, parentID, stepNum, time/microsecond); 
                

				if(verbDetCsIInfo && volumeID > -1)
				{
                    G4cout  << " NFLUX "
					<< std::setprecision(3) << preKE/MeV 
					<< " preKE(mev):postKE " << std::setprecision(3) << trackKE/MeV    
                    << " " << partName << " eDep "
                    << std::setprecision(3) << eDep/MeV <<  " mev|pre:post " <<  preVol 
                    <<  " " << postVol << " volID "<< volumeID << " " << weight 
                    << " wt|evt:track:step " << evtid << " "<< trackid << " " << stepNum
                    << "  " << preProc << " preproc|postproc " << postProc << " "
                    << track->GetCreatorProcess()->GetProcessName()
                    <<  " create_proc|in " << track->GetLogicalVolumeAtVertex()->GetName()
					<< "  withKE "<< track->GetVertexKineticEnergy()/MeV
                    //<< " track_xyz_m " << std::setprecision(3) << xx/m << " " << std::setprecision(3) 
                    //<< yy/m << " " << std::setprecision(3) <<  zz/m
                    << " post_xyz_m " << std::setprecision(3) << postX/m << " " << std::setprecision(3) 
					<< postY/m << " " << std::setprecision(3) << postZ/m
					<< " pre_xyz_m " << std::setprecision(3) << preX/m << " " << std::setprecision(3) 
					<< preY/m << " " << std::setprecision(3) << preZ/m
                    << " postStatus " << step->GetPostStepPoint()->GetStepStatus()
                    << G4endl;
				}
			
				G4bool shieldSteps = false;
                if( shieldSteps && (postVol== "DetPolyShield"|| postVol=="DetLeadShield" ||
					postVol=="SNSDetectorCsI" ) &&  postVol != preVol ) 
                {
                    G4cout  << " ShieldN "
                    << std::setprecision(3) << preKE/MeV 
                    << " preKE(mev):postKE " << std::setprecision(3) << trackKE/MeV    
                    << " " << partName << " eDep "
                    << std::setprecision(3) << eDep/MeV <<  " mev|pre:post " <<  preVol 
                    <<  " " << postVol << " volID "<< volumeID << " " << weight 
                    << " wt|evt:track:step " << evtid << " "<< trackid << " " << stepNum
                    << "  " << preProc << " preproc|postproc " << postProc
                    << "  " << track->GetCreatorProcess()->GetProcessName()
                    <<  " create_proc|in " << track->GetLogicalVolumeAtVertex()->GetName()
                    << "  withKE "<< track->GetVertexKineticEnergy()/MeV
                    << " xyz_m " << std::setprecision(3) << postX/m << " " << std::setprecision(3) 
                    << postY/m << " " << std::setprecision(3) << postZ/m
                    << " postStatus " << step->GetPostStepPoint()->GetStepStatus()
                    << G4endl;
                }	
            }//nuetron nFlux vol


            //Detections
			eDep = step->GetTotalEnergyDeposit();	
			if(postVol=="SNSDetectorCsI")
			{
				man->FillSNSDetection(eDep/MeV, preKE/MeV , trackKE/MeV, pdgCode, weight, postX/m, 
				postY/m, postZ/m, evtid, trackid, parentID, stepNum, time/microsecond);
				//proc, vol

                if(verbDetCsIInfo)
                {
					G4cout  << " Det "			
                    << std::setprecision(3) << preKE/MeV 
                    << " preKE(mev):postKE " << std::setprecision(3) << trackKE/MeV    
                    << " " << partName << " eDep "
                    << std::setprecision(3) << eDep/MeV <<  " mev|pre:post " <<  preVol 
                    <<  " " << postVol << " volID "<< volumeID << " " << weight 
                    << " wt|evt:track:step " << evtid << " "<< trackid << " " << stepNum
                    << "  " << preProc << " preproc|postproc " << postProc 
                    << "  " << track->GetCreatorProcess()->GetProcessName()
                    <<  " create_proc:in " << track->GetLogicalVolumeAtVertex()->GetName()
                    << "  withKE "<< track->GetVertexKineticEnergy()/MeV
                    << " post_xyz_m " << std::setprecision(3) << postX/m 
                    << " " << std::setprecision(3) 
                    << postY/m << " " << std::setprecision(3) <<  postZ/m
                    << " postStatus " << step->GetPostStepPoint()->GetStepStatus()
                    << G4endl;
                }
            }//if CsI
	}//if vol det

	
    
    //###############################################
    //NFLUX at BasementLayer
G4bool nFluxBaseLayer = true;    
    if(nFluxBaseLayer && partName == "neutron" &&
        preVol == "BasementIn" && postVol == "BasementLayer")
    {
        G4double trackKE = track->GetKineticEnergy();
        G4int evtid     = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
        G4int trackid   = track->GetTrackID();
        G4double xx     = track->GetPosition().x(); //trackpos= postpos
        G4double yy     = track->GetPosition().y();
        G4double zz     = track->GetPosition().z();
        G4double weight = track->GetWeight() ; 

        G4cout  << " BaseFLUX "
            << std::setprecision(3) << trackKE/MeV    
            << " " <<  weight << " " << partName << " "<< preVol <<  " " << postVol 
            << " " << preProc << " " << postProc << " " << evtid << " " << trackid 
            << track->GetCreatorProcess()->GetProcessName()
            << " cr_pr|in " << track->GetLogicalVolumeAtVertex()->GetName()
            << " KE "<< track->GetVertexKineticEnergy()/MeV
            << " xyz_m " << std::setprecision(3) << xx/m << " " 
            << std::setprecision(3) 
            <<  yy/m << " " << std::setprecision(3) << zz/m
            << " st " << step->GetPostStepPoint()->GetStepStatus()
            << G4endl;
    }

	////############################################
	//NEUTRONS FLUX at Target and MonolithSteel

    G4bool HgNflux = false;
    if(HgNflux && partName =="neutron")
    {
        if(postVol != "HgTarget" && preVol == "HgTarget" )
        G4cout << "NFLUX_HgTargetOUT " << partName << " preKE "<< std::setprecision(3)
               << preKE/MeV << " " << preVol << " pre:post " << postVol << G4endl;

        if(preVol != "HgTarget" && postVol == "HgTarget" )
        G4cout << "NFLUX_HgTargetIN " << partName << " preKE "<< std::setprecision(3)
               << preKE/MeV << " " << preVol << " pre:post " << postVol << G4endl;
    }

    G4bool monoNflux = false;

    if(monoNflux && partName =="neutron")
    {
        G4double monOuterRad = 5.*m;
        G4double monTopZ = 8.*m;
        G4double monBotZ = -4.5*m;
        if(postVol != "MonolithSteelCyl" && preVol == "MonolithSteelCyl" )
        {
            G4double radius = std::sqrt((track->GetPosition().x()) *  
                    (track->GetPosition().x()) + (track->GetPosition().y()) *
                    (track->GetPosition().y()) ); 
            G4double prerad = std::sqrt((step->GetPreStepPoint()->GetPosition().x()) *  
                    (step->GetPreStepPoint()->GetPosition().x()) + 
                    (step->GetPreStepPoint()->GetPosition().y()) *
                    (step->GetPreStepPoint()->GetPosition().y()) ); 
            G4double postrad = std::sqrt((step->GetPostStepPoint()->GetPosition().x()) *  
                    (step->GetPostStepPoint()->GetPosition().x()) + 
                    (step->GetPostStepPoint()->GetPosition().y()) *
                    (step->GetPostStepPoint()->GetPosition().y()) ); 

            G4double zpos = step->GetPostStepPoint()->GetPosition().z();
            if(radius > monOuterRad - kCarTolerance || (zpos > monTopZ-kCarTolerance
                || zpos < monBotZ+kCarTolerance) )
            {
                G4cout << "NFLUX_monoOUT " << partName << " preKE "
                       << std::setprecision(3)<< preKE/MeV << " postKE "
                       << std::setprecision(3) << step->GetPostStepPoint()->GetKineticEnergy()
                       << " "<< preVol<< " pre:post " << postVol  << " trackrad " << radius 
                       << " prerad " << prerad << " postrad " << postrad 
                       << " stat.pre:post " << step->GetPreStepPoint()->GetStepStatus()
                       << "  " << step->GetPostStepPoint()->GetStepStatus()
                       << " postxyz " << std::setprecision(3) 
                       << step->GetPostStepPoint()->GetPosition().x() 
                       << "  "  << std::setprecision(3) 
                       << step->GetPostStepPoint()->GetPosition().y()
                       << " " << std::setprecision(3) 
                       << step->GetPostStepPoint()->GetPosition().z()
                       << " prexyz " << std::setprecision(3) 
                       << step->GetPreStepPoint()->GetPosition().x() 
                       << "  "  << std::setprecision(3) 
                       << step->GetPreStepPoint()->GetPosition().y()
                       << " " << std::setprecision(3) 
                       << step->GetPreStepPoint()->GetPosition().z()
                       << G4endl;
            }
        }
    }

    //#######################################
    //COUNT NEUTRONS produced by proton
    G4bool  verbNeutCount  = false; 
    if(verbNeutCount)
    {
        static G4int numNeutPerEvt = 0;
        static G4int staticEvtId = 0;
        static G4int beamSec     = 0;
        G4double    neutronsKE   = 0;
        G4int       numNthisStep = 0;
        G4String    thisSecPart  = "";
        G4String    thisProc     = "";
        G4bool      pFlag        = true;
        G4bool      isBeamProt   = (track->GetParentID()==0) ? true:false;
        //if(track->GetParentID() == 0)//parent is primary proton
        //{
        const std::vector<const G4Track*>* secVec=step->GetSecondaryInCurrentStep();
        //G4int thisPDG = (*secVec)[secN]->GetDynamicParticle()->GetPDGcode();

        if(postVol == "HgTarget")
        {
            for(size_t secN = 0; secN < secVec->size(); secN++)
            {
                thisSecPart = (*secVec)[secN]->GetDynamicParticle()->GetDefinition()->GetParticleName();
                if(thisSecPart == "neutron"|| thisSecPart == "proton")
                {

                    if(pFlag)
                    {
                        G4cout << "particle: "<<partName << " ke(mev) "<< std::setprecision(2) 
                            << preKE/MeV  << " Nsec " << secVec->size()
                            << " postVol " << postVol ;
                        if(isBeamProt){ G4cout  << "  :primary ";} 
                        G4cout << G4endl;
                        pFlag = false;
                    }
                    const G4VProcess* creator = (*secVec)[secN]->GetCreatorProcess();
                    if(creator) thisProc = creator->GetProcessName();

                    G4double keThisN = (*secVec)[secN]->GetDynamicParticle()->GetKineticEnergy();

                    G4cout  << " \t sec: " << thisSecPart<< " ke: " << std::setprecision(2)
                            << keThisN/MeV << " " << thisProc;
                    if(isBeamProt){ G4cout  << " \t firstImpact "; }
                    G4cout << G4endl;

                    if(thisSecPart == "neutron")
                    {
                        numNthisStep++;
                        if(isBeamProt)beamSec++; 
                        neutronsKE = neutronsKE +  keThisN;
                    }
                }//n
                //else G4cout  <<"other : " << thisSecPart << G4endl;
            }//for
        }//if Target
       // }//primary
        G4int thisEvtId = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

        if(thisEvtId == staticEvtId) numNeutPerEvt += numNthisStep;
        else 
        {
            if(numNeutPerEvt>0 )
                G4cout << "####Evt " <<  thisEvtId <<" numN  " << numNeutPerEvt
                       << "  firstImpact " << beamSec << G4endl;
            numNeutPerEvt = numNthisStep;
            beamSec       = 0;//only first time needed
        }
        staticEvtId = thisEvtId;
    }//if countN
    //########## End of Neutron Count


    //NEUTRONS FLUX escaping to outofWorld
	G4bool nFluxOutOfWorld = false;
    if(nFluxOutOfWorld)
    {
		static  G4String preVolStatic = "";
		static  G4double preX  = -1;
		static  G4double preY  = -1;
		static  G4double preZ  = -1;
		static  G4double prePx = -1;
		static  G4double prePy = -1;
		static  G4double prePz = -1;
		static  G4double preKe = -1;
		static  G4bool  preExists = false;

		if(track->GetCurrentStepNumber() <=1) preExists = false;
        preVolStatic  = preVol;

		G4bool verb2 = false;
		if(verb2)
		{
			if(track->GetNextVolume())  
			G4cout << "# " << track->GetCurrentStepNumber() 
                   << postVol
			       << "   " << track->GetVolume()->GetName()
			//<< " preVol "  << preVol  
				//<< " pos: "         <<  track->GetPosition().x() //present or post
				//<< " "             <<  track->GetPosition().y()
				//<< " "             <<  track->GetPosition().z() 
				<< "  prexist " << preExists
				<< G4endl;
		}

		G4bool takeIt = (track->GetCurrentStepNumber() ==0 && track->GetTrackID()==1) ? false:true;  
        if( preVolStatic== "SNSTargetDetector") takeIt = false;
		if( preVolStatic== "World") takeIt = false;

		if( takeIt && pre && pdgCode==2112 &&
			( postVol=="SNSTargetDetector"||	postVol=="World"||
			  postVol=="OutOfWorld" )  )
		{
			preExists   = true;
			preX    = pre->GetPosition().x();
			preY    = pre->GetPosition().y();
			preZ    = pre->GetPosition().z(); // track->GetPosition().z()
			prePx   = pre->GetMomentumDirection().x();//track->GetMomentumDirection().x()
			prePy   = pre->GetMomentumDirection().y();
			prePz   = pre->GetMomentumDirection().z();
			preKe   = pre->GetKineticEnergy();
		}
	   //TODO reset when out of scope			
		if(  track->GetNextVolume() == 0 || postVol =="OutOfWorld" 	|| postVol=="World" )
		{
			if(preExists && pdgCode ==2112 ) //neutron
			{
				RunAction::CountParticles(); 
				G4bool verb = false;
				if(verb)
				{
					G4cout 
					//<< " :PART: " 
					<< " "          << partName
					<< "  "         << preVolStatic
					<< "  "         << preX
					<< "  "         << preY
					<< "  "         << preZ
					<<  " current " << track->GetPosition().x()  
					<< "  "         << track->GetPosition().y()
					<< "  "         << track->GetPosition().z()
					<< " Mom: "     << prePx   
					<< " "          << prePy 
					<< " "          << prePz
					<< " ke "       << preKe/MeV
					//<< " time " << track->GetGlobalTime()
					//<< " :proc: "   << postProc
					//<< " :prestKE: "  << pre->GetKineticEnergy()/MeV  
					//<< " :poststKE: " << pre->GetKineticEnergy()/MeV	
					//<< " trackL " << track->GetTrackLength()
					//<< " stepL " << track->GetStepLength()
					
					//VERTEX (,where this track was created) information 
					//<< " MeV.VertMom: " << track->GetVertexMomentumDirection().x()
					//<< " " << track->GetVertexMomentumDirection().y()
					//<< " " << track->GetVertexMomentumDirection().z()
					//<< "ke " << track->GetVertexKineticEnergy()/MeV
					//<< "   "<< track->GetLogicalVolumeAtVertex()->GetName()
					//<< "  VertPOS: "<< track->GetVertexPosition().x()
					//<< "  "<< track->GetVertexPosition().y()
					//<< "  "<< track->GetVertexPosition().z()
					//<< "   "<< track->GetCreatorProcess()->GetProcessName() //NOTE: check before
					//<< " model "<< track->GetCreatorModelName()
					<< G4endl;
				} //if verb
				//fill in root file
                man->FillSNSneutronFlux(0, preKE/MeV, preKe/MeV, pdgCode,  0, track->GetPosition().x()/m, 
                    track->GetPosition().y()/m, track->GetPosition().z()/m, 0, 0, 0, 0, 0);//, prePx, prePy, prePz );

			}		
		}
	} //END of FIND NEUTRONS escaping to outofWorld





	//##############################################
    //SCINTILLATOR DETECTOR analysis process
    //

	G4bool verbScint = false;
    //set verbScint if secondary size>0
	//if( step->GetSecondaryInCurrentStep()->size() )  verbScint=true;	
        
	if(verbScint)
	{
		G4ParticleDefinition* particleType = track->GetDefinition();
		G4double edep = step->GetTotalEnergyDeposit()/MeV; 
        G4StepPoint* preStep  = step->GetPreStepPoint();
        G4StepPoint* postStep = step->GetPostStepPoint();
		//NOTE prestep and postep may not exist in some cases !

		//const G4VProcess* postDefStep = postStep->GetProcessDefinedStep();
        G4cout << " eDep " << std::setprecision(4) << edep;
		if( postProc)
		{
			G4cout << " post:" << postProc << " ";		
		}else {  G4cout << " postproc: NONE ";}

		if( preProc)
		{
			G4cout << " pre:" << preProc << " ";		
		}else { G4cout << " preproc: NONE ";}

		if(trackProc){ 
			G4cout<< " trackProc:" <<trackProc << " ";
		}else { G4cout << " trackProc: NONE ";}
		


        //if(step->GetTrack()->GetParentID()==0)
        //fpSteppingManager available only in SteppingAction class
        G4TrackVector* fSecondary=fpSteppingManager->GetfSecondary();
        G4int nSecThisStep = fpSteppingManager->GetfN2ndariesAtRestDoIt()
          + fpSteppingManager->GetfN2ndariesAlongStepDoIt()
          + fpSteppingManager->GetfN2ndariesPostStepDoIt();
       //const std::vector<const G4Track*>* fSecondary=step->GetSecondaryInCurrentStep();
		
		size_t secSize = fSecondary->size();// total of all steaps so far.
        //if( secSize > 0)
		G4cout  << "secSize: " << secSize-nSecThisStep ;
		
        G4double totalKE = 0;
        G4double totalEn = track->GetTotalEnergy();
        // track->GetDynamicParticle()->GetTotalEnergy();

        if(postStep)
		    totalKE = postStep->GetKineticEnergy(); 
		
        size_t nOptPhot = 0;
        G4bool printPhot  = true;		
        if( nSecThisStep>0 ) //nSecThisStep = that of this step
        {
          for(size_t lp1 = secSize-nSecThisStep; lp1 < secSize; lp1++)
          {
			  
			  //(*fSecondary)[lp1]->GetDynamicParticle()->GetKineticEnergy();
			  //GetTotalEnergy();
//               G4String secPart = (*fSecondary)[lp1]->GetDefinition()->GetParticleName();
//               if(secPart =="neutron" || secPart == "proton" || secPart == "deutron"
//                 ||secPart == "alpha" || secPart == "gamma"  || secPart=="e-"||secPart=="e+")
//               {
                totalKE = totalKE +  (*fSecondary)[lp1]->
                                      GetDynamicParticle()->GetKineticEnergy();
                totalEn = totalEn + (*fSecondary)[lp1]->GetTotalEnergy();
             // }
              //if(postProc ~ /neutronInelastic|nCapture/ )
              //  G4cout<< " part: " << (*fSecondary)[lp1]->GetDefinition()->GetParticleName()
              //	<< " KE: " << (*fSecondary)[lp1]->GetDynamicParticle()->GetKineticEnergy()
              //  << G4endl;
			  
			  
			  
			 //optical photon 
            if(particleType == G4OpticalPhoton::Definition()) 
			{
				nOptPhot++;
				if(printPhot)
                {
                  G4cout << " Op.photon "; 
                  const G4VProcess* creator=(*fSecondary)[lp1]->GetCreatorProcess();
                  if(creator)
                  {
                    G4cout<<" sec.proc: "<< creator->GetProcessName();
                    printPhot= false;
                  }
                  G4cout << " " << G4endl;
                        
               }//print
            }//if phot
            
          }//for
        }//if secSize
        G4cout<<" nOptPhot: "<< G4endl;       

		G4bool verb4 = false;
		if(verb4)
		{
            G4double  preTotE = -1;
            if(preStep)
            {
                preKE   = preStep->GetKineticEnergy()/MeV ;
                preTotE = preStep->GetTotalEnergy()/MeV;
            }
            G4double postKE = -1, postTotE = -1;
            if(postStep)
            {
                postKE   = postStep->GetKineticEnergy()/MeV;
                postTotE =  postStep->GetTotalEnergy()/MeV;
            }

			G4cout 
			<< partName
			<< " trackID: " << track->GetTrackID()  
			<< " :prestKE: " << std::setprecision(4)<< preKE  
			<< " :poststKE: " << std::setprecision(4) << postKE
			<< " :preTotEn: " << std::setprecision(4)<< preTotE
			<< " :postTotEn: " << std::setprecision(4)<< postTotE
			//<< " :TotEn: " << std::setprecision(4) << totalEn/MeV		
			//<< " :totalKE: " << std::setprecision(4) << totalKE/MeV			
			<< " :trackKE: " << std::setprecision(4) << track->GetKineticEnergy() /MeV
			<< " :eDep: " << std::setprecision(4) << edep 
			<< " :-deltaE: " << std::setprecision(4)<< -step->GetDeltaEnergy()/MeV 
			//<< " :-deltaE-edep: "<< std::setprecision(4)<< -step->GetDeltaEnergy()/MeV-edep
			//<< " :niel: " << std::setprecision(4)<< step->GetNonIonizingEnergyDeposit()/MeV
			//<< " :proc: "<<  postStep->GetProcessDefinedStep()->GetProcessName()
			<< G4endl;
		}
        if( nSecThisStep>0 ) G4cout<<" EndProcSearchKeyWord "<< G4endl;  //don't delete this, just comment it 
    }//verbose    
    //####################################
    
}//end of userSteppingAction

//---------------------------------------------------------------------------

G4int SteppingAction::IsCylSurfaceFlux(const G4Step* aStep, G4double fRadius, G4double fHalfHt)
{  //from G4PSCylinderSurfaceFlux        //returns 1(in), 2(out), or -1
	
	G4StepPoint* preStep = aStep->GetPreStepPoint(); //?????
	G4VPhysicalVolume* physVol = preStep->GetPhysicalVolume();
	G4VPVParameterisation* physParam = physVol->GetParameterisation();
	//G4VSolid* solid = NULL;
	if(physParam)
	{ // for parameterized volume
		//G4int idx = ((G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable()))
		//->GetReplicaNumber(indexDepth);//TODO get indexdepth and uncomment these
		//solid = physParam->ComputeSolid(idx, physVol);
		//solid->ComputeDimensions(physParam,idx,physVol);
	}
	else
	{ // for ordinary volume
	//	solid = physVol->GetLogicalVolume()->GetSolid();
		//G4cout << " logvol " << physVol->GetLogicalVolume()->GetName() << G4endl;
		//NOTE this is wrong
	}
	
	//G4Tubs* tubsSolid = (G4Tubs*)(solid);  //NOTE solid dimensions are all 0
	//now it is bypassed by passing in ht and rad. TODO correct it
	
	G4TouchableHandle theTouchable = aStep->GetPreStepPoint()->GetTouchableHandle();
	G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();  
	
	if (aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary )
	{
		// Entering Geometry. Prestep point is at the Outer surface. 
		G4ThreeVector stppos1= aStep->GetPreStepPoint()->GetPosition();
		G4ThreeVector localpos1 = 
		theTouchable->GetHistory()->GetTopTransform().TransformPoint(stppos1);
		//if ( std::fabs(localpos1.z()) > tubsSolid->GetZHalfLength()) return -1;			
		if ( std::fabs(localpos1.z()) > (fHalfHt+ kCarTolerance))//tubsSolid->GetZHalfLength()
		{//G4cout << " preGeom:-1:z " << localpos1.z() << " halfz " << tubsSolid->GetZHalfLength() 
		 //      <<" tol " << kCarTolerance << G4endl; // NOTE solid dimensions are all 0
			return -1; 												
		}
		G4double localR2 = localpos1.x()*localpos1.x()+localpos1.y()*localpos1.y();
		G4double outerRad = fRadius;//tubsSolid->GetOuterRadius();// GetInnerRadius();//NOTE
		if( (localR2 > (outerRad-kCarTolerance)*(outerRad-kCarTolerance)
			&& localR2 < (outerRad+kCarTolerance)*(outerRad+kCarTolerance))
			||
			( std::fabs( localpos1.z())< (fHalfHt +kCarTolerance)//tubsSolid->GetZHalfLength() 
			&&	localR2 < (outerRad+kCarTolerance)*(outerRad+kCarTolerance) )
		){
			//G4cout << " preGeom:1 " << G4endl;
			return 1;//fFlux_In;
		}
	}
	
	if (aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary )
	{
		// Exiting Geometry. Post step point is at the outer surface.
		G4ThreeVector stppos2= aStep->GetPostStepPoint()->GetPosition();
		G4ThreeVector localpos2 = 
		theTouchable->GetHistory()->GetTopTransform().TransformPoint(stppos2);
		//if ( std::fabs(localpos2.z()) > tubsSolid->GetZHalfLength() )	return -1;
		if ( std::fabs(localpos2.z()) > (fHalfHt+ kCarTolerance))//tubsSolid->GetZHalfLength() 
		{
			return -1;
		}
		G4double localR2 = localpos2.x()*localpos2.x()+localpos2.y()*localpos2.y();
		G4double outerRad = fRadius;//tubsSolid->GetOuterRadius();//GetInnerRadius();//NOTE
		if( (localR2 > (outerRad-kCarTolerance)*(outerRad-kCarTolerance)
			&& localR2 < (outerRad+kCarTolerance)*(outerRad+kCarTolerance))
			||
			( std::fabs( localpos2.z())< ( fHalfHt+kCarTolerance)// tubsSolid->GetZHalfLength()
			&&	localR2 < (outerRad+kCarTolerance)*(outerRad+kCarTolerance) )
		){
			//G4cout << " postGeom:2 " << G4endl;
			return 2;//fFlux_Out;
		}
	}
	
	return -1;
}

//---------------------------------------------------------------------------
G4int SteppingAction::IsBoxSurfaceFlux(const G4Step* aStep) //returns 1(in), 2(out), or -1
{  //G4PSFlatSurfaceFlux
	
	G4StepPoint* preStep = aStep->GetPreStepPoint();
	
	G4VPhysicalVolume* physVol = preStep->GetPhysicalVolume();
	G4VPVParameterisation* physParam = physVol->GetParameterisation();
	G4VSolid * solid = 0;
	if(physParam)
	{ // for parameterized volume
	//G4int idx = ((G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable()))
	//->GetReplicaNumber(indexDepth);//TODO find indexDepth and use these
	//solid = physParam->ComputeSolid(idx, physVol);
	//solid->ComputeDimensions(physParam,idx,physVol);
	}
	else
	{ // for ordinary volume
		solid = physVol->GetLogicalVolume()->GetSolid();
	}
	
	G4Box* boxSolid = (G4Box*)(solid);
	
	G4TouchableHandle theTouchable = 
	aStep->GetPreStepPoint()->GetTouchableHandle();
	G4double kCarTolerance=G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
	
	if (aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary ){
		// Entering Geometry
		G4ThreeVector stppos1= aStep->GetPreStepPoint()->GetPosition();
		G4ThreeVector localpos1 = 
		theTouchable->GetHistory()->GetTopTransform().TransformPoint(stppos1);
		if(std::fabs( localpos1.z() + boxSolid->GetZHalfLength())<kCarTolerance 
			||std::fabs( localpos1.z() - boxSolid->GetZHalfLength())<kCarTolerance
			||std::fabs( localpos1.y() + boxSolid->GetYHalfLength())<kCarTolerance 
			||std::fabs( localpos1.y() - boxSolid->GetYHalfLength())<kCarTolerance 
			||std::fabs( localpos1.x() + boxSolid->GetXHalfLength())<kCarTolerance 
			||std::fabs( localpos1.x() - boxSolid->GetXHalfLength())<kCarTolerance 
		){
			return 1;//fFlux_In; _InOut =0 		
		}
		
	}
	
	if (aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary ){
		// Exiting Geometry
		G4ThreeVector stppos2= aStep->GetPostStepPoint()->GetPosition();
		G4ThreeVector localpos2 = 
		theTouchable->GetHistory()->GetTopTransform().TransformPoint(stppos2);
		if(std::fabs( localpos2.z() + boxSolid->GetZHalfLength())<kCarTolerance 
			||std::fabs( localpos2.z() - boxSolid->GetZHalfLength())<kCarTolerance
			||std::fabs( localpos2.y() + boxSolid->GetYHalfLength())<kCarTolerance 
			||std::fabs( localpos2.y() - boxSolid->GetYHalfLength())<kCarTolerance 
			||std::fabs( localpos2.x() + boxSolid->GetXHalfLength())<kCarTolerance 
			||std::fabs( localpos2.x() - boxSolid->GetXHalfLength())<kCarTolerance 			
		){
			return 2;//fFlux_Out;
		}
	}
	
	return -1;
}

//----------------------------------------------------------------------------
    //track->GetOriginTouchableHandle()
    //track->GetVolume()->GetName() //current vol = pre ?
    //track->GetNextVolume()->GetName() //postvol
    //G4VPhysicalVolume* postPV = step->GetPostStepPoint()->GetPhysicalVolume();
    //G4double postImp = G4IStore::GetInstance()->GetImportance(postPV,0); 
    //step->GetPostStepPoint()->GetTouchable()->GetReplicaNumber());    
 /*
       if(strcmp(physVol->GetName(),"ModulePhysical")== 0
        ........
    
       //FCALSteppingAction
	   // Get the primary particle
	   if(TrackID==1 && ParentID==0 && (aTrack->GetCurrentStepNumber()) == 1)
	   {
		   PrimaryVertex    = aTrack->GetVertexPosition(); 
		   PrimaryDirection = aTrack->GetVertexMomentumDirection();
		   PrimaryVertex.x() .....
		}
		xx = (aTrack->GetVertexPosition()).x();
	
	   
	   //Area of Cylinder. G4PSCylinderSurfaceCurrent
	   G4StepPoint* preStep = aStep->GetPreStepPoint();
	   G4VPhysicalVolume* physVol = preStep->GetPhysicalVolume();
	   G4VPVParameterisation* physParam = physVol->GetParameterisation();
	   G4VSolid * solid = 0;
	   if(physParam)
	   { // for parameterized volume
	   G4int idx = ((G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable()))
	   ->GetReplicaNumber(indexDepth);
	   solid = physParam->ComputeSolid(idx, physVol);
	   solid->ComputeDimensions(physParam,idx,physVol);
	   }
	   else
	   { // for ordinary volume
	   solid = physVol->GetLogicalVolume()->GetSolid();
	   }
	   G4Tubs* tubsSolid = (G4Tubs*)(solid);
	   G4double square = 2.*tubsSolid->GetZHalfLength() *tubsSolid->GetInnerRadius()* tubsSolid->GetDeltaPhiAngle()/radian;
	   
	   
	   
	   
	//G4RunManager* runManager = G4RunManager::GetRunManager();
	//G4int evtID = runManager->GetCurrentEvent()->GetEventID();
	//const G4Material* theMat = preStep->GetMaterial();
	//G4double tableEloss = G4EnergyLossTables::GetDEDX(currentParticle, preStep, theMat);
	//G4cout << " stepniel: "<< step->GetNonIonizingEnergyDeposit()/eV;
	
   
 if(track->GetTrackID() ==1 && track->GetCurrentStepNumber()==1)   
 G4cout << " stepAct:: stepN:Momentum= "
        << track->GetCurrentStepNumber()
        << " "<< track->GetMomentumDirection().x()   
        << " " <<  track->GetMomentumDirection().y() 
        << " " <<  track->GetMomentumDirection().z()
        << G4endl;


    //from G4NeutronHPInelasticData    to get cross section
    G4bool printCS = true;
    if(printCS)
    {
      const G4DynamicParticle*  dynPart = track->GetDynamicParticle();
      G4Material* material = preStep->GetMaterial();
      G4double aT = material->GetTemperature();
      G4Element* elC = G4Element::GetElement("Carbon");
      //const G4Element* elH = G4Element::GetElement("Hydrogen");
      G4int index = elC->GetIndex();
  
      G4NeutronHPInelasticData* csData = new G4NeutronHPInelasticData; //??
     // G4double xsC = csData->GetCrossSection(dynPart, elC, aT);
      
     G4bool outOfRange=false;
      G4PhysicsTable * theCrossSections = new G4PhysicsTable;
      //G4double ke = track->GetDynamicParticle()->GetKineticEnergy();
      for (int n =0; n<2; n++)
      {
        G4double ke = n*0.1*MeV;
        G4double xsC = (*((*theCrossSections)(index))).GetValue(ke, outOfRange);
        
        G4cout << " xs_C:H " << xsC << G4endl;      
      }  
    }//if printCS
  
     // do scintillation photons, and also re-emission
    {
      // invoke scintillation process
      G4VParticleChange * pParticleChange
    = GLG4Scint::GenericPostPostStepDoIt(aStep);
      // were any secondaries defined?
      G4int iSecondary= pParticleChange->GetNumberOfSecondaries();
      if (iSecondary > 0)
    {
      // add secondaries to the list
      while ( (iSecondary--) > 0 )
        {
          G4Track * tempSecondaryTrack
        = pParticleChange->GetSecondary(iSecondary);
          fpSteppingManager->GetfSecondary()
        ->push_back( tempSecondaryTrack );
        }
    }
      // clear ParticleChange
      pParticleChange->Clear();
    }
   
   
   //distribute eDep point bet pre and post step:
   G4double edep = aStep->GetTotalEnergyDeposit();
   G4ThreeVector prePoint  = aStep->GetPreStepPoint() ->GetPosition();//CHECK
   G4ThreeVector postPoint = aStep->GetPostStepPoint()->GetPosition();//CHECK
   G4ThreeVector point = prePoint + G4UniformRand()*(postPoint - prePoint);
   
   
   
   
   

   if(fWriteAllSteps)
   {
	   if(man) man->WriteAllSteps(step);
   }	
   
   G4AnalysisManager* g4man = G4AnalysisManager::Instance();
   //FillPerstep
   if(track->GetDefinition() == G4OpticalPhoton::Definition())//see above
	{
		if( stepNum == 1) //first step
		{
			if(verbose) G4cout << " step:photstepNum: " << stepNum << " ";
			//fill
			//Global time (time since the event was created)
			//Local time (time since the track was created)//GetLocalTime
			//Proper time (time in its rest frame since the track was created )
			G4double gTime = postStep->GetGlobalTime()/nanosecond;//CHECK
			G4double lTime = postStep->GetLocalTime()/nanosecond;	//CHECK
			//G4double pTime = postStep->GetProperTime()/nanosecond;
			if(verbose) G4cout << ": " << gTime-lTime << " ";
			
			 g4man->FillH1(4, gTime-lTime);   
			 g4man->FillH1(5, gTime);   
			 g4man->FillH1(6, lTime);  	
			 		
			 //if(man) man->FillPerStep(edep,gTime-lTime,gTime, lTime,0.,0.,0.,0.,
			 //	stepNum, evtID); 
			 // ecarbon, ealpha, eproton,eelectron,stepNum, evtID);	
			 	
			 g4man->FillNtupleDColumn(2,0,edep);
			 g4man->FillNtupleDColumn(2,1,gTime-lTime);
			 g4man->FillNtupleDColumn(2,2,gTime); 
			 g4man->FillNtupleDColumn(2,3,lTime);
			 g4man->AddNtupleRow(2);
			
			}
			}
			   

	if(postStep->GetPhysicalVolume() && verbose)
	{
		G4cout << "Poststep.vol: "<< postStep->GetPhysicalVolume()->GetName() << " ";//CHECK
	}
	if (preStep->GetPhysicalVolume() && verbose)
	{
		G4cout << "Prestep.vol: "<< preStep->GetPhysicalVolume()->GetName() << " ";//CHECK
	}   

	if( track->GetNextVolume() != 0  && verbose) { 
		G4cout << "track:nextvol:" << track->GetNextVolume()->GetName() << " ";//CHECK
	}

   
 	//G4double partEnergy =preStep->GetKineticEnergy();
	//trackInformation->SetPosition(track->GetPosition());//CHECK
	//G4double x  = theTrack->GetPosition().x();
	G4ThreeVector pos = theTrack->GetPosition();
	G4int parentID = track->GetParentID();
	
	
	//WLSSteppingAction::UserSteppingAction
	//extended/optical/LXe/src/LXeSteppingAction.cc
	
	G4VPhysicalVolume* thePrePV  = preStep->GetPhysicalVolume();//CHECK
	G4VPhysicalVolume* thePostPV = postStep->GetPhysicalVolume();//CHECK
	
	
	G4VPhysicalVolume* volume  //prestep volume ??
				= preStep->GetTouchableHandle()->GetVolume();//CHECK
    G4LogicalVolume* lVol  //prestep volume ??
			= preStep->GetTouchableHandle()->GetVolume()->GetLogicalVolume();//CHECK
   
    
    if (!fSDvolume) { 
      
      //const DetectorConstruction* detectorConstruction
      //  = static_cast<const DetectorConstruction*>
      //  (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
      //fSDvolume = detectorConstruction->GetSDvolume();   
      
      fSDvolume = fDetector->GetSDvolume();      
    }
    if (volume != fSDvolume) return;
    
    fEventAction->AddStepEdep(edep);
    
    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

     
     G4String ParticleName = track->GetDefinition()->GetParticleName();//CHECK
     //if (ParticleName == "opticalphoton") return;
     
	
	const G4VProcess* process = postStep->GetProcessDefinedStep();
    G4String procName = postStep->GetProcessDefinedStep()->GetProcessName();
  
  	// Retrieve the status of the photon
  	G4OpBoundaryProcessStatus theStatus = Undefined;  
  	G4ProcessManager* OpManager =
		G4OpticalPhoton::OpticalPhoton()->GetProcessManager();
  
  	if (OpManager) {
		G4int MAXofPostStepLoops =
	  		OpManager->GetPostStepProcessVector()->entries();
	  	G4ProcessVector* fPostStepDoItVector =
	  		OpManager->GetPostStepProcessVector(typeDoIt);
	  
	  	for ( G4int i=0; i<MAXofPostStepLoops; i++) {
		  	G4VProcess* fCurrentProcess = (*fPostStepDoItVector)[i];
		  	fOpProcess = dynamic_cast<G4OpBoundaryProcess*>(fCurrentProcess);
		  	if (fOpProcess) { theStatus = fOpProcess->GetStatus(); break;}
	  	}
  	}
  
  
	//if theStatus =...
  	if ( thePostPVname == "SNSDet" ) {
		G4SDManager* SDman = G4SDManager::GetSDMpointer();
	  	G4String SDname="SNS/Scintillator";
	  	SNSSD* snsSD = (SNSSD*)SDman->FindSensitiveDetector(SDname);
	  
	  	//if (mppcSD) snsSD->ProcessHits_constStep(step,NULL);
	  	//theTrack->SetTrackStatus(fStopAndKill);
  }
	  
	G4RunManager* theRunManager = G4RunManager::GetRunManager();
 	runID = theRunManager->GetCurrentRun()->GetRunID();
 	evtID = theRunManager->GetCurrentEvent()->GetEventID();      
*/ 


    /*
     G4bool SteppingAction::IsPassed(G4Step* aStep){ //G4PSPassageCellCurrent.cc
        G4bool Passed = FALSE;
        G4bool isEnter = aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary;
        G4bool isExit  = aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary;
        G4int  trkid  = aStep->GetTrack()->GetTrackID();
        if ( isEnter &&isExit ){         // Passed at one step
            Passed = TRUE;                 
        }else if ( isEnter ){            // Enter a new geometry
            fCurrentTrkID = trkid;         // Resetting the current track.
        }else if ( isExit ){             // Exit a current geometry
            if ( fCurrentTrkID == trkid ) {
                Passed = TRUE;               // if the track is same as entered.
            }
        }else{                           // Inside geometry
            if ( fCurrentTrkID == trkid ){ // Adding the track length to current one ,
                }
        }
        return Passed;
    }
    */

	//WLSSteppingAction::UserSteppingAction
	//extended/optical/LXe/src/LXeSteppingAction.cc
 //electromagnetic/TestEm17/src/RunAction.cc , process storing, theoretical ..
 
 
        
 //data can be stored in EventAction, RunData (RunAction), and/or Analysis class
 //Best is EventAction.
 
  //Do the process in RunData
  //RunData* runData  = static_cast<RunData*>(
  //    G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  //OR store data in EventAction class

    ////G4RunManager::AbortEvent(); //to stop this event

    //NOTE: always check befoe using the following :
    //track->GetNextVolume() ;  track->GetDynamicParticle()->GetDefinition()
    //step->GetPostStepPoint()->GetPhysicalVolume()
    //step->GetPreStepPoint()->GetPhysicalVolume()
    //step->GetPostStepPoint()->GetProcessDefinedStep()
    //track.GetCreatorProcess()
    //if(track->GetParticleDefinition() == G4OpticalPhoton::Definition()) return;   