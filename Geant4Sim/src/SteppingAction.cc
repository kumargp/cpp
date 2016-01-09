// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


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
#include "G4GeometryTolerance.hh"
//#include "G4PhysicsTable.hh"
//#include "G4ElementTable.hh"
//#include "G4NeutronHPData.hh"
//#include "G4VCrossSectionDataSet.hh"
#include "G4IStore.hh"
//#include <regex>
#include <string>
#include "G4HadronicProcessType.hh"

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
//get a messenger 
}

//===========================================================================

SteppingAction::~SteppingAction()
{}

//===========================================================================

void SteppingAction::UserSteppingAction(const G4Step* step)
{

	enum detils {detail=true, nodetail=false};

	G4bool skipStepping = false; //set true if only SD is needed
	if ( skipStepping )return;


	//Settings
	G4double neutThreshKill  =  30.0*keV; 	
	G4double neutInnerVolKill= 900.0*keV;
	G4bool stepKill          = true; //NOTE	

	G4bool tunnelWallDet    = true;		

	G4bool detectorEdep     = true; //DETEDEP  
	G4bool neutronFlux		= true;
	
	G4bool verbDetNE   		= false;  //VERBNFLUX
	G4bool verbDetections	= false; //VERBDETECTION	
	
	G4bool stepPosition 	= false; // = gps verbose 1	
	G4bool printEvtSeeds	= false;
	G4bool  testDetector1   = false;  //NOTE

	G4bool baseLayerNRoofStone	= false;    

	G4bool  verbNeutTimes   = false; //for time distr of neutrons 
	G4int	evtIDRef		= 1234; //NOTE
	G4bool surfaceDet       = false; 
	G4bool verbSurfaceDet   = false;

	G4double minCutoffNeutKE	= 0.5*keV; // store N ke in root 

	//G4bool verbLayerFlux   = false; //VERBLAYER	


	//----------------------------------------------------------------------
	//NOTE flood with text output
	G4bool testAllStepsAllVolumes = false; //NOTE all texts 
	if(testAllStepsAllVolumes)
	{
		VerbMore(step, "TESTALL",detail);		
	}


	G4String postVol    = "NIL";
	if(step->GetPostStepPoint()->GetPhysicalVolume())
		postVol  = step->GetPostStepPoint()->GetPhysicalVolume()->GetName(); 
	//post->GetTouchableHandle()->GetVolume()->GetName();

	G4String preVol     	= "NIL";
	G4String partName   	= "NIL";
	G4Track* track 			= step->GetTrack();	
	G4StepPoint* pre    	= step->GetPreStepPoint();
	G4StepPoint* post   	= step->GetPostStepPoint();

	if(pre->GetPhysicalVolume())
		preVol  =  pre->GetPhysicalVolume()->GetName(); 

	if(track->GetDynamicParticle()->GetDefinition()) // GetParticleDefinition
	{
		partName = track->GetDynamicParticle()->GetDefinition()->GetParticleName();
	}


	if( stepPosition)
	{
		if(partName == "proton" && track->GetParentID()==0 
				&&track->GetTrackID()==1 && track->GetCurrentStepNumber()==1 )
			G4cout  << " P::evt:xyz " 	<< 
					G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID() 
					<< " "				<< track->GetPosition().x()/m
					<< " " 				<< track->GetPosition().y()/m 
					<< " " 				<< track->GetPosition().z()/m 
					<< G4endl;
	}
	
	G4int evtID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
	
	//print seed
	if(printEvtSeeds)
	{
		static G4long lastPrintEvtID = -1;		
		if( lastPrintEvtID != evtID) // && radius1 >= 0
		{
			G4cout << EventAction::GetRandString() << G4endl;
			lastPrintEvtID = evtID;
		}
	}
	
	//-----------------------------------------------------------------------
	if( stepKill && partName == "neutron" )
	{
		G4bool killInThisVolume = false;
		
		if( track->GetKineticEnergy()< neutThreshKill 
			&& !(
				postVol=="BraneDetScintC7H8" || postVol=="SNSDetectorCsI"
				|| postVol=="DetWaterShield" || postVol=="DetLeadShield"
				|| postVol=="DetCdShield"    
				//|| postVol=="TunWall"      	 || postVol=="BasementLayer"
				//|| postVol=="BasementRoofIn" 	 || postVol=="BasementRoofStone" 
				//|| postVol=="BasementRoofTop"  
				) )
				killInThisVolume = true;

		else if( track->GetKineticEnergy() < neutInnerVolKill
			&& (
				postVol== "SurfaceHdcInnerShield"
			|| postVol== "BasementIn" 		   || postVol== "InstrFloor"
			|| postVol== "MonolithSteelCyl"	   || postVol== "OuterSteelCylinder"
			|| postVol== "InnerBottomSteelD2O" || postVol== "InnerUpperSteelD2"
			|| postVol== "BePlugBottom"		   || postVol== "UpperBePlug"
			|| postVol== "InnerRefPlug"		   || postVol== "WaterModerator"
			|| postVol== "LiquidHydr3"		   || postVol== "LiquidHydr2"
			|| postVol== "LiquidHydr1"		   || postVol== "HgSteelBox"
			|| postVol== "HgTarget"			   || postVol== "MonoBase"
			) )
			killInThisVolume = true;

		
		if(killInThisVolume)
		{
			track->SetTrackStatus(fStopAndKill); //NOTE fKillTrackAndSecondaries
			return;
		}
	}

	//------------------------------------------------------------------------
	//IMPORTANT
	
	G4bool killMuNuPi = true;
	if(killMuNuPi && step->GetTrack()->GetDynamicParticle()->GetDefinition())
	{
		if( partName == "nu_e" || partName == "nu_mu" || partName == "anti_nu_mu" 
			||partName == "anti_nu_e" || partName == "nu_tau" || partName == "anti_nu_tau")
			
			track->SetTrackStatus(fStopAndKill);
	}
	
	//------------------------------------------------------------------------
	
	
	//Detections
	if( detectorEdep && ( postVol=="SNSDetectorCsI" ||
		postVol=="BraneDetScintC7H8" ) )
	{
		CallFillDetection(step);
		
		if(verbDetections)
		{
			VerbMore(step, "Det", false);
		}
		
	}//if CsI



	//-------------------------------------------------------------------------


	//Neutron Flux
	
	G4bool fillThisN 	= false;	
	if( neutronFlux && partName == "neutron" )
	{	
		G4double trackKE 	= track->GetKineticEnergy();
		G4bool boundary 	= false;
		if( post->GetStepStatus()==fGeomBoundary ) boundary = true;
		//NOTE : entering point is boundary, but not exiting point
		//pre boundary is good for second previous point in a volume, not first. G4StepStatus.hh

		if(trackKE > minCutoffNeutKE && postVol == "DetWaterShield" 
			&& preVol != "DetWaterShield" ) // && boundary )
		{
			CallFillNFlux(step, true); // NOTE only foe det (true )
				
			//print random seed		
			if(verbDetNE)
				VerbMinimum(step,"NFLUX", false);
				
			static G4int lastHitEvtID = -1;
			if(lastHitEvtID != evtID)
			{
				G4cout <<  "Hit" <<EventAction::GetRandString() << G4endl;
				lastHitEvtID = evtID;
			}
		}
		
		else if(trackKE > minCutoffNeutKE && postVol=="SNSDetectorCsI" 
			 && preVol != "SNSDetectorCsI" )//&& boundary)
			fillThisN = true;
		
		else if(trackKE > minCutoffNeutKE && postVol == "BraneDetScintC7H8"// && boundary 
			&& preVol != "BraneDetScintC7H8")
			fillThisN = true;

		else if(baseLayerNRoofStone && trackKE > minCutoffNeutKE  
			&& postVol=="BasementLayer" && (preVol=="BasementIn" || preVol=="InstrFloor") )
			fillThisN = true;

		else if(baseLayerNRoofStone && trackKE > minCutoffNeutKE   
			&& postVol=="BasementRoofTop" &&	preVol=="InstrFloor" ) 
			fillThisN = true;
		
		else if(tunnelWallDet && trackKE > minCutoffNeutKE 
			&& postVol=="TunWallDet" && boundary ) 
			fillThisN = true;	

	}////nuetron nFlux


	//Neutron Flux store or write text o/p	
	if(fillThisN)
	{
		CallFillNFlux(step, false); // false=other than DetWaterShield
		
		if(verbDetNE)
			VerbMinimum(step,"NFLUX", false);
	}

	//------------------------------------------------------------------------
    //code below in this method is not important, it is for as needed.
	//------------------------------------------------------------------------

	//NOTE stepkill above
	if(testDetector1)
	{		
		G4bool printIn = false;		

		if( printIn && partName=="neutron" 
			&& step->GetPostStepPoint()->GetKineticEnergy()/MeV >100.*keV )
		{
			if(preVol=="VolOne" && postVol=="VolTwo" )
					//if( postRad > outer+kCarTolerance)  postX = postY = postZ = outer;
				VerbMore(step, "TEST", false);
			
			else if(preVol=="VolTwo" && postVol=="VolThree" )
				VerbMore(step, "TEST", false);
		}

		if( detectorEdep && postVol=="FluxRunDet" )
		{
			CallFillDetection(step);
			
			if(verbDetections)
				VerbMore(step, "Det", false);			
		}
			
		else if (postVol == "DetWaterShield" && preVol != "DetWaterShield") 
		{
			CallFillNFlux(step, true); // NOTE only foe det (true )
					
			if(verbDetNE)
				VerbMinimum(step,"NFLUX", false);
		}

	} //end of testDetector


	//---------------------------------------------------------------------

	//SurfaceDet for neutron flux comparison
	// surfaceDet = true; above
	if( surfaceDet && partName.compare("neutron")==0 
		&& pre->GetStepStatus()==fGeomBoundary
		&& postVol.compare("SurfaceDetector")==0 )
	{
		if(	track->GetKineticEnergy() > minCutoffNeutKE )
			CallFillNFlux(step, false);

		if(verbSurfaceDet)
			VerbMinimum(step,"SURF", false);
	}//surfaceDet



	//--------------------------------------------------------------------
	//NFLUX at Layer
	// verbSurfaceLayer verbLayerFlux set above
	G4bool verbSurfaceLayer = false;
	G4bool verbMonoBaseLayer= false;

	G4int layerID = 0;
	if(verbSurfaceLayer &&  partName.compare("neutron")==0 &&
		preVol.compare("SurfaceHdcInnerShield")==0 && postVol.compare("SurfaceHdcLayer")==0 )
	{
		layerID	  = 2;
	}

	if(verbMonoBaseLayer &&  partName.compare("neutron")==0)
	{		
		if(preVol.compare("MonolithSteelCyl")==0 && postVol.compare("MonoBase")==0 )
		{
			layerID   = 3; 
		}	
		if(preVol.compare("MonoBase")==0 &&  postVol.compare("BasementIn")==0 ) //see return statement
		{
			layerID   = 5; 
		}
	} 
	if(verbSurfaceLayer &&  partName.compare("neutron")==0 &&		
		preVol.compare("MonolithSteelCyl")==0 && ( postVol.compare("SurfaceHdcInnerShield")==0 ||
		postVol.compare("OuterProtonBoxHDBox1")==0 || postVol.compare("OuterProtonBoxSteelBox")==0 ||
		postVol.compare("OuterProtonBoxConcrete")==0 ))
	{
		layerID   = 4;
	}	


	if((verbSurfaceLayer || verbMonoBaseLayer) && layerID>0)
	{
		VerbMinimum(step,"layerFLUX", false);
	}

		

	////############################################
	//NEUTRONS FLUX at Target and MonolithSteel

	G4bool HgNflux = false;
	if(HgNflux && partName=="neutron")
	{//TODO use pre->GetStepStatus()==fGeomBoundary
		G4double preKE 		 = 0;
		if(pre)
			preKE = pre->GetKineticEnergy();
		if(postVol != "HgTarget" && preVol =="HgTarget")
		G4cout << "NFLUX_HgTargetOUT " << partName << " preKE "<< std::setprecision(3)
				<< preKE/MeV << " " << preVol << " pre:post " << postVol << G4endl;

		if(preVol != "HgTarget" && postVol == "HgTarget")
		G4cout << "NFLUX_HgTargetIN " << partName << " preKE "<< std::setprecision(3)
				<< preKE/MeV << " " << preVol << " pre:post " << postVol << G4endl;
	}

	G4bool monoNflux = false;

	if(monoNflux && partName=="neutron")
	{
		G4double monOuterRad = 5.*m;
		G4double monTopZ = 8.*m;
		G4double monBotZ = -4.5*m;
		if(postVol != "MonolithSteelCyl" && preVol == "MonolithSteelCyl")
		{
			G4double radius = std::sqrt((track->GetPosition().x()) *  
					(track->GetPosition().x()) + (track->GetPosition().y()) *
					(track->GetPosition().y()) ); 
			G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance(); 
			G4double zpos = post->GetPosition().z();
			if(radius > monOuterRad - kCarTolerance || (zpos > monTopZ-kCarTolerance
				|| zpos < monBotZ+kCarTolerance) )
			{
				G4cout << "\t Trackrad " << radius/mm << G4endl; 
				VerbMore(step, "NFLUX_monoOUT", detail); 
			}
		}
	}

	//############################################
	//Neutron production times
	//postVol=="BraneDetScintC7H8"
	//if(verbNeutTimes && G4RunManager::GetRunManager()->GetCurrentEvent()
		//->GetEventID() == evtIDRef) VerbMore(step, "", 0);

	if(verbNeutTimes && evtID == evtIDRef)
	{
		G4bool secNeutron = false;
		G4double preKE = 0;
		G4String  thisSecPart("NULL"), thisProc("NULL");
		//if(partName == "proton" )// postVol =="HgTarget")
		//{
			if(pre)
				preKE = pre->GetKineticEnergy();
			
			G4double xx = step->GetPostStepPoint()->GetPosition().x() ;
			G4double yy = step->GetPostStepPoint()->GetPosition().y() ;
			G4double zz = step->GetPostStepPoint()->GetPosition().z() ;

			const std::vector<const G4Track*>* secVec = step->GetSecondaryInCurrentStep();

			if(secVec->size() >0)
				G4cout 	<< postVol
					<< " primary: "	   	<< partName      
					<< " ke "		    << std::setprecision(3)
					<< " evt "          << evtID 
					<< " "
					<< preKE/MeV 	    << " pos "
					<< std::setprecision(3) << xx/m     << " "
					<< std::setprecision(3) << yy/m     << " "
					<< std::setprecision(3) << zz/m
					<< " :: sec:size " 	<< secVec->size();
					
			for(size_t secN = 0; secN < secVec->size(); secN++)
			{
				thisSecPart = (*secVec)[secN]->GetDynamicParticle()
									->GetDefinition()->GetParticleName();
				if(thisSecPart == "neutron")
				{
					secNeutron = true;
					double gTime = (*secVec)[secN]->GetGlobalTime();
					const G4VProcess* creator = (*secVec)[secN]->GetCreatorProcess();
					if(creator) thisProc = creator->GetProcessName();
					
					G4double keThisN = (*secVec)[secN]->GetDynamicParticle()->GetKineticEnergy();
					
					G4cout  << " ::n,ke,time " << secN << " "  
							<< keThisN/MeV << " "<<  gTime/nanosecond; 					
				}
			}
			if(secNeutron)
				G4cout  << " seconadryNeutron ";

			if(secVec->size() >0) G4cout  << G4endl;
		//}
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

		if(postVol =="HgTarget")
		{
			for(size_t secN = 0; secN < secVec->size(); secN++)
			{
				thisSecPart = (*secVec)[secN]->GetDynamicParticle()->GetDefinition()->GetParticleName();
				if(thisSecPart == "neutron" || thisSecPart == "proton")
				{

					if(pFlag)
					{
						G4double preKE 	= 0;				
						
						if(pre)
							preKE = pre->GetKineticEnergy();
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

	//##############################################
	//SCINTILLATOR DETECTOR analysis process
	//

	G4bool verbScint = false;
	//set verbScint if secondary size>0
	//if( step->GetSecondaryInCurrentStep()->size() )  verbScint=true;	
		
	if(verbScint)
	{
		G4ParticleDefinition* particleType = track->GetDefinition();
		G4double edep = step->GetTotalEnergyDeposit(); 
		G4StepPoint* postStep = post;
		G4String preProc    = "NIL";
		if( pre->GetProcessDefinedStep())
			preProc = pre->GetProcessDefinedStep()->GetProcessName();
		
		G4String postProc   = "NIL";
		if( post->GetProcessDefinedStep())
			postProc = post->GetProcessDefinedStep()->GetProcessName();
		
		//NOTE prestep and postep may not exist in some cases !

		//const G4VProcess* postDefStep = postStep->GetProcessDefinedStep();
		G4cout << " eDep " << std::setprecision(4) << edep/MeV;
		if( postProc)
		{
			G4cout << " post:" << postProc << " ";		
		}else {  G4cout << " postproc: NONE ";}

		if( preProc)
		{
			G4cout << " pre:" << preProc << " ";		
		}else { G4cout << " preproc: NONE ";}

		G4String trackProc  = "NIL";
		if(track->GetCreatorProcess())
			trackProc =  track->GetCreatorProcess()->GetProcessName();
		
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

		G4bool verb4 = false;
		if(verb4)
		{
			VerbMore(step, "nOptPhot", false);
		}
		if( nSecThisStep>0 ) G4cout<<" EndProcSearchKeyWord "<< G4endl;  //don't delete this, just comment it 
	}//verbose    
	//####################################

}//end of userSteppingAction

//===========================================================================


void SteppingAction::CallFillDetection(const G4Step* step)
{

	Analysis* man   	= Analysis::GetInstance();

	G4double weight 	= 2;
	G4double postX  	= 0;
	G4double postY  	= 0;
	G4double postZ  	= 0;
	G4double preKE  	= 0; 	
	G4int postProcCode 	= 0;
	G4int trkProcCode	= 0;
	G4int postVolCode 	= 0;
	G4int pdgCode   	= 0;

	GetNParams(step, preKE, weight, postX, postY, postZ, postVolCode,
				trkProcCode, postProcCode, pdgCode);

	G4Track* track 	= step->GetTrack();
	G4double eDep   = step->GetTotalEnergyDeposit();	
	G4int evtID     = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
	G4int trkID     = track->GetTrackID();	
	G4int parentID  = track->GetParentID();
	G4int stepNum   = track->GetCurrentStepNumber();
	G4double nonI   = step->GetNonIonizingEnergyDeposit()/MeV;
	G4double gTime	= track->GetGlobalTime();
	G4double lTime  = track->GetLocalTime();

	//std::array < G4double, 17 > data = {};
	man->FillEdep(eDep/MeV, pdgCode, weight, preKE/MeV,  
			postX/m, postY/m, postZ/m, evtID, trkID, parentID, stepNum, nonI/MeV, 
				gTime/nanosecond, postProcCode, trkProcCode, lTime, postVolCode);

}

//=========================================================================

void SteppingAction::CallFillNFlux(const G4Step* step, bool det)
{
	Analysis* man   	= Analysis::GetInstance();

	G4double weight 	= 2;
	G4double postX  	= 0;
	G4double postY  	= 0;
	G4double postZ  	= 0;
	G4double preKE  	= 0; 	
	G4int postProcCode 	= 0;
	G4int trkProcCode	= 0;
	G4int postVolCode 	= 0;
	G4int pdgCode   	= 0;

	GetNParams(step, preKE, weight, postX, postY, postZ, postVolCode,
				trkProcCode, postProcCode, pdgCode);

	if( (det && postVolCode != 5 ) || (!det && postVolCode == 5))
		std::cout << " ERROR matching volumeID for NFLUX " << std::endl;

	if( det && pdgCode == 2112 && postVolCode == 5) // DetWaterShield 
		man->FillNFluxDet(preKE/MeV, weight, postX/m, postY/m, postZ/m, postVolCode); 

	else if( !det && pdgCode == 2112 && postVolCode != 5) // NOT DetWaterShield 
		man->FillNFluxOther(preKE/MeV, weight, postX/m, postY/m, postZ/m, postVolCode); 	
}


//========================================================================
void SteppingAction::GetNParams(const G4Step* step, G4double& preKE, G4double& weight, 
		G4double& postX, G4double& postY, G4double& postZ, G4int& postVolCode,
		G4int& trkProcCode, G4int& postProcCode, G4int& pdgCode)
{
	G4Track* track 	= step->GetTrack();
	G4StepPoint* post= step->GetPostStepPoint();

	weight = track->GetWeight();
	postX  = post->GetPosition().x();
	postY  = post->GetPosition().y();
	postZ  = post->GetPosition().z();
	preKE  = step->GetPreStepPoint()->GetKineticEnergy(); 
	pdgCode= step->GetTrack()->GetDynamicParticle()->GetPDGcode();

	G4String postProc("NULL");
	G4String postVol("NULL");	
	G4String trkProc("NULL");

	if( post->GetProcessDefinedStep())
		postProc	= post->GetProcessDefinedStep()->GetProcessName();

	if(track->GetCreatorProcess())
		trkProc = track->GetCreatorProcess()->GetProcessName();

	if(post->GetPhysicalVolume())
		postVol  = post->GetPhysicalVolume()->GetName(); 

	GetProcVolIDs(postProcCode, postVolCode, postProc, postVol);

	GetProcVolIDs(trkProcCode, postVolCode, trkProc, postVol);// better than NULL
}

//=========================================================================
void SteppingAction::GetProcVolIDs( G4int& procCode, G4int& volID,
		const G4String procName, const G4String volName)
{	

	if (procName == "CoupledTransportation" ||  procName =="Transportation")
		procCode = 1;
	else if (procName == "hadElastic") 	procCode=2;
	else if (procName == "ImportanceProcess" ) procCode=3;	
	else if (procName == "neutronInelastic") procCode = 4;	
	else if (procName == "nCapture") procCode=5;
	else if (procName == "alphaInelastic") procCode=25; //was 5
	else if (procName == "protonInelastic") procCode=6;
	else if (procName == "ionInelastic") procCode=7;	
	else if (procName == "ionIoni") procCode=8;
	else if (procName == "eBrem") procCode=9;
	else if (procName == "compt") procCode=10;
	else if (procName == "hIoni") procCode=11;
	else if (procName == "eIoni") procCode=12;		
	else if (procName == "nKiller") procCode=13;
	else if (procName == "msc") procCode=14;
	else if (procName == "hBrems") procCode=15;
	else if (procName == "annihil") procCode=16;
	else if (procName == "conv") procCode=17;
	else if (procName == "photonNuclear") procCode=18;		
	else if (procName == "hPairProd") procCode=19;
	else if (procName == "CoulombScat") procCode=20;
	else if (procName == "electronNuclear") procCode=21;
	else if (procName == "phot") procCode=22;
	else if (procName == "dInelastic")  procCode=23;
	else if (procName == "Decay") procCode=24;
	else procCode = 0;

	if(volName == "NULL" ) 
		return;


	if ( volName== "SNSTargetDetector")  volID = 1;
	else if( volName=="BraneDetScintC7H8" ) volID = 2;
	else if ( volName== "DetLSAcrlic" ) volID = 3;
	else if ( volName== "DetLSHDPE") volID = 4;
	else if ( volName=="DetWaterShield" ) volID = 5;
	else if ( volName=="DetLeadShield" ) volID =6;
	else if ( volName=="DetCdShield" ) volID = 7;
	else if( volName=="SNSDetectorCsI" ) volID = 8;
			
	else if ( volName == "TunWallDet") volID = 20;	
	else if ( volName == "BasementLayer")volID = 21;
	else if ( volName == "BasementRoofStone")volID = 22;	
			
	else if (volName== "SurfaceHdcInnerShield") volID = 40;
	else if (volName== "BasementIn") volID = 41;
	else if (volName== "InstrFloor") volID = 42;
	else if (volName== "MonolithSteelCyl") volID = 43;
	else if (volName== "OuterSteelCylinder") volID = 44;
	else if (volName== "InnerBottomSteelD2O") volID = 45;
	else if (volName== "InnerUpperSteelD2") volID = 46;
	else if (volName== "BePlugBottom") volID = 47;	   
	else if (volName== "UpperBePlug") volID = 48;
	else if (volName== "InnerRefPlug") volID = 49;	   
	else if (volName== "WaterModerator") volID = 50;
	else if (volName== "LiquidHydr3") volID = 51;
	else if (volName== "LiquidHydr2") volID = 52;
	else if (volName== "LiquidHydr1") volID = 53;		   
	else if (volName== "HgSteelBox") volID = 54;
	else if (volName== "HgTarget"	) volID = 55;		  
	else if (volName== "MonoBase") volID = 56;	

	else if(volName== "FluxRunDet" ) volID = 90;
	else if (volName =="VolTwo" )volID = 91;
	else if (volName=="VolThree")volID = 92;
	else if ("FluxRunAcrlic")volID = 93;
	else if ("FluxRunHdpe")volID = 94;

	else if ( volName == "MultiStepDet1") volID = 95;
	else if ( volName == "MultiStepDet2") volID = 96;
	else if ( volName == "MultiStepDet3") volID = 97;
	else volID = 0;

}

//==========================================================================
void SteppingAction::VerbMinimum(const G4Step* step, G4String key="Edep",
								 G4bool part)
{
	G4Track* track 		= step->GetTrack();

	G4double eDep       = step->GetTotalEnergyDeposit() ;
	//- step->GetNonIonizingEnergyDeposit();		

	G4StepPoint* post   = step->GetPostStepPoint();

	G4String postVol("NULL");
	G4String partName("NULL");

	if(step->GetPostStepPoint()->GetPhysicalVolume())
		postVol  = step->GetPostStepPoint()->GetPhysicalVolume()->GetName(); 

	if(track->GetDynamicParticle()->GetDefinition())
	{
		partName = track->GetDynamicParticle()->GetDefinition()->GetParticleName();
	}

	G4String postProc("NULL");
	if( post->GetProcessDefinedStep())
		postProc 	= post->GetProcessDefinedStep()->GetProcessName();

	G4String trackProc("NULL");	
	if(track->GetCreatorProcess())
		trackProc 	= track->GetCreatorProcess()->GetProcessName();

	G4int postProcCode = 0;
	G4int volumeID =  0;
	GetProcVolIDs(postProcCode, volumeID, postProc, postVol);
	
	G4cout  << key	
	<< std::setprecision(4) << "  "			<< eDep/MeV			<< " " 
	<< partName 			<< " " 			<< postVol			<< " "			
	<< volumeID				<< " " 			
	<< step->GetPreStepPoint()->GetKineticEnergy()/MeV 			<< " " 	
	<< postProc     		<< " post|trkCr " 		
	<< trackProc 			<< "  wt " 
	<< track->GetWeight();

	if(!part) G4cout << G4endl;
}

//Det  0 neutron BraneDetScintC7H8  5 volID|evtID CoupledTransportation 
// postproc|trkCrproc neutronInelastic wt 8.88e-16  ::trk:step:parent:evt 593881 
// 7  593658  6826 |postKE_m 34.4 34.4  preKE|NonIE_kev 0  |post_xyz_m 6.17 -17.9 -7.57  secSize: 0
//============================================================================

void SteppingAction::VerbMore(const G4Step* step, G4String key="NFLUX", 
							G4bool detailed)
{
	VerbMinimum(step,key, true);
	G4Track* track      = step->GetTrack();  
	G4StepPoint* post   = step->GetPostStepPoint();

	G4StepPoint* pre= step->GetPreStepPoint();
	G4int parentID  = track->GetParentID();

	G4int stepNum 	= track->GetCurrentStepNumber();
	G4double postKe	= post->GetKineticEnergy();

	G4String preVol("NULL");
	if(step->GetPreStepPoint()->GetPhysicalVolume())
		preVol  	=  step->GetPreStepPoint()->GetPhysicalVolume()->GetName(); 

	G4cout        
	<< " ::trk:st:par:ev "		<< track->GetTrackID() 
	<< "  " 					<< stepNum 						
	<< "  "						<< parentID					
	<< "  "
	<< G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID()
	<< "  |postKE_m "     		<< std::setprecision(3) 
	<< postKe/MeV				<< "  "		
	<< std::setprecision(3)		<< pre->GetKineticEnergy()/MeV 
	<< "  preKE|NonIE_kev "     		<< std::setprecision(3) 	
	<< step->GetNonIonizingEnergyDeposit()/keV		
	<< " |post_xyz_m " 
	<< std::setprecision(3) 	<< post->GetPosition().x()/m 	
	<< " " 						<< std::setprecision(3) << post->GetPosition().y()/m 	
	<< " "						<< std::setprecision(3) << post->GetPosition().z()/m 
	<< " trGTns "				<< track->GetGlobalTime()/nanosecond //since evt
	<< " trLTns "				<< track->GetLocalTime()/nanosecond // since track created
	<< " preVol "				<<  preVol					
	<< " "; 				

	const std::vector<const G4Track*>* secVec=step->GetSecondaryInCurrentStep();
	//G4int thisPDG = (*secVec)[secN]->GetDynamicParticle()->GetPDGcode();
	G4cout << " secSize: " << secVec->size();
	for(size_t secN = 0; secN < secVec->size(); secN++)
	{
		G4cout << " secpart" << secN << " " 
		<< (*secVec)[secN]->GetDynamicParticle()->GetDefinition()->GetParticleName();
		
		//const G4VProcess* creator = (*secVec)[secN]->GetCreatorProcess();
		//if(creator) G4cout << " secCrProc " << creator->GetProcessName();
		//(*secondary)[lp]->GetGlobalTime() //GetLocalTime()
		G4cout << " secKE " <<  (*secVec)[secN]->GetDynamicParticle()->GetKineticEnergy()/MeV ;
	}

	G4cout << G4endl;



	if(detailed)
	{			
		G4String preProc("NULL");
		if( pre->GetProcessDefinedStep())
			preProc 	= pre->GetProcessDefinedStep()->GetProcessName();
		
		G4cout << key << "More preproc "
		<< preProc			   		<< " "  
		<< std::setprecision(3)		<< pre->GetKineticEnergy()/MeV 
		<< " preKE_m|Pxyz: "
		<< std::setprecision(3) << post->GetMomentumDirection().x() << " "
		<< std::setprecision(3) << post->GetMomentumDirection().y() << " "
		<< std::setprecision(3) << post->GetMomentumDirection().z() << " "			
		<<  " :vertEXVol "   << track->GetLogicalVolumeAtVertex()->GetName()
		<< "  :vertKE "            << track->GetVertexKineticEnergy()/MeV
		<< " :postStat "         	<< post->GetStepStatus()
		<< " pre_xyz_m "            
		<< std::setprecision(3) 	<< pre->GetPosition().x()/m << " " 
		<< std::setprecision(3) 	<< pre->GetPosition().y()/m << " "
		<< std::setprecision(3)     << pre->GetPosition().z()/m
		<< G4endl; 
	}	   

}

//--------------------------------------------------------------------------
G4int SteppingAction::CalculateLY(G4String particle, G4double eDep)
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
G4double SteppingAction::LinearInterp( G4double input, std::vector <G4double> dataX, 
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
/*At the end of each step, according to the processes involved, the state of a track may be changed 
in UserSteppingAction. Geant4 kernel won’t set the last 3, but the user can set.
G4Track* theTrack = theStep->GetTrack();
if(…) theTrack->SetTrackStatus(fSuspend);
fAlive : Continue the tracking. 
fStopButAlive: The track has come to zero kinetic energy, but still AtRest process to occur.
fStopAndKill: The track has lost its identity because it has decayed, interacted or gone beyond the world boundary.
Secondaries will be pushed to the stack.
fKillTrackAndSecondaries :Kill the current track and also associated secondaries.
fSuspend :Suspend processing of the current track and push it and its secondaries to the stack.
fPostponeToNextEvent:Postpone processing of the current track to the next event. 
Secondaries are still being processed within the current event.
If you want to identify the first step in a volume, pick fGeomBoudary status in PreStepPoint. 
If you want to identify a step getting out of a volume, pick fGeomBoundary status in PostStepPoint



G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
track->GetOriginTouchableHandle()
track->GetVolume()->GetName() //current vol = step->GetPreStepPoint() ?
track->GetNextVolume()->GetName() //postvol
G4VPhysicalVolume* postPV = step->GetPostStepPoint()->GetPhysicalVolume();
G4double postImp = G4IStore::GetInstance()->GetImportance(postPV,0); 
step->GetPostStepPoint()->GetTouchable()->GetReplicaNumber());    

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


WLSSteppingAction::UserSteppingAction
extended/optical/LXe/src/LXeSteppingAction.cc
electromagnetic/TestEm17/src/RunAction.cc , process storing, theoretical ..


	
//data can be stored in EventAction, RunData (RunAction), and/or Analysis class
//Best is EventAction.

//Do the process in RunData
RunData* runData  = static_cast<RunData*>(
	G4RunManager::GetRunManager()->GetNonConstCurrentRun());
OR store data in EventAction class

//G4RunManager::AbortEvent(); //to stop this event

//NOTE: always check befoe using the following :
track->GetNextVolume() ;  track->GetDynamicParticle()->GetDefinition()
step->GetPostStepPoint()->GetPhysicalVolume()
step->GetPreStepPoint()->GetPhysicalVolume()
step->GetPostStepPoint()->GetProcessDefinedStep()
track.GetCreatorProcess()
if(track->GetParticleDefinition() == G4OpticalPhoton::Definition()) return;   


else if( postVol == "DetWaterShield"&&  !(preVol == "SNSDetectorCsI" 
	|| preVol == "DetLeadShield" ||preVol == "DetWaterShield" ))volumeID = 4;    
else if( preVol == "MonolithSteelCyl")volumeID = 5;//NOTE
xx = (aTrack->GetVertexPosition()).x();
yy = (aTrack->GetVertexPosition()).y();
zz = (aTrack->GetVertexPosition()).z();

<< " time " << track->GetGlobalTime()
<< " :proc: "   << postProc
<< " :prestKE: "  << pre->GetKineticEnergy()/MeV  
<< " :poststKE: " << pre->GetKineticEnergy()/MeV  
<< " trackL " << track->GetTrackLength()
<< " stepL " << track->GetStepLength()

VERTEX (,where this track was created) information 
<< " MeV.VertMom: " << track->GetVertexMomentumDirection().x()
<< " " << track->GetVertexMomentumDirection().y()
<< " " << track->GetVertexMomentumDirection().z()
<< "ke " << track->GetVertexKineticEnergy()/MeV
<< "   "<< track->GetLogicalVolumeAtVertex()->GetName()
<< "  VertPOS: "<< track->GetVertexPosition().x()
<< "  "<< track->GetVertexPosition().y()
<< "  "<< track->GetVertexPosition().z()
<< "   "<< track->GetCreatorProcess()->GetProcessName() //NOTE: check before
<< " model "<< track->GetCreatorModelName()



G4bool posAbort         = false;
G4int posAbortLimit     = 10000;
if(posAbort)
{
	static G4int farNum = 0;
	G4double radPos     = 19.*m;//23.*m;
	G4double tx         = track->GetPosition().x();
	G4double ty         = track->GetPosition().y();
	G4double radius1    = std::sqrt(tx*tx+ ty*ty);
	
	if( partName.compare("neutron")==0 && radius1 > 3/4.*radPos && 
		((tx<0 && asin(ty/radPos)*180./pi > -45.) || ( tx>0 && asin(ty/radPos)*180./pi > 45.)) )
	{
		farNum++;
		if(farNum > posAbortLimit)
		{   
			G4cout << "far from 10 degrees. Aborting.. " << G4endl;
			std::abort();
		}
	}
}




//secondaries ONLY if CUT is low enough. Use HP physics ?
//G4HadronicProcessType.hh , G4ProcessType.h
//post->GetProcessDefinedStep()->GetProcessType() == fHadronic or fParallel
if(step->GetSecondary() != 0 && step->GetSecondary()->size() != 0)
{
std::vector<G4Track*>::const_iterator it;
for (it=step->GetSecondary()->begin(); it!=step->GetSecondary()->end(); it++) 
{
if( (*it)->GetCreatorProcess() &&
	(*it)->GetCreatorProcess()->GetProcessSubType() == fHadronElastic){

G4String secPart = (*it)->GetDynamicParticle()->GetDefinition()->GetParticleName();
secKE = (*it)->GetDynamicParticle()->GetKineticEnergy();


G4TouchableHistory* theTouchable =  (G4TouchableHistory*)(step->GetTrack()->GetTouchable());
G4VPhysicalVolume* physVol = theTouchable->GetVolume();
postVol  =	physVol->GetName();

*/