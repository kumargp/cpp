// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

//#ifdef G4MULTITHREADED
//#include "G4MTRunManager.hh"
//#else
#include "G4RunManager.hh"
//#endif

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "G4UImanager.hh"
#include "QBBC.hh"
#include "QGSP_BIC.hh"
#include "QGSP_BIC_HP.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BERT_HP.hh" //no elastic scatt ??
#include "FTFP_BERT.hh"
#include "FTFP_BERT_HP.hh"
#include "FTFP_INCLXX.hh"
#include "QGSP_INCLXX.hh"
#include "QGSP_INCLXX_HP.hh"
#include "FTFP_INCLXX_HP.hh"
#include "Shielding.hh"
#include "PhysicsList.hh"
#include "SNSParallelTargetDetector.hh"

#include "G4VModularPhysicsList.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4NeutronTrackingCut.hh"

#include "G4ImportanceBiasing.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4GeometrySampler.hh"
#include "G4GeometryManager.hh"
#include "G4IStore.hh"
#include "G4VWeightWindowStore.hh"
#include "G4WeightWindowAlgorithm.hh"
#include "G4RunManagerKernel.hh"

#include "MaxTimeCuts.hh"
#include "G4NeutronTrackingCut.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

//needs Geant4.10

int main(int argc, char **argv)
{
	
    G4Random::setTheEngine(new CLHEP::RanecuEngine());
    //G4Random needed for MT instead of CLHEP::HepRandom

	G4RunManager* runManager = new G4RunManager;
	
	G4RunManagerKernel::GetRunManagerKernel()->SetVerboseLevel(2);
	
	DetectorConstruction* detector = new DetectorConstruction;
    runManager->SetUserInitialization( detector);

    //http://geant4.in2p3.fr/IMG/pdf_PhysicsLists.pdf
    //http://geant4.web.cern.ch/geant4/support/proc_mod_catalog/physics_lists/useCases.shtml
    //http://m.iopscience.iop.org/1742-6596/219/3/032043/pdf/1742-6596_219_3_032043.pdf
	//http://hypernews.slac.stanford.edu/HyperNews/geant4/get/phys-list/572/1.html
    //QGSP_BIC_HP;QGSP_BERT_HP ;QBBC; FTFP_BERT; FTFP_BERT_HP;FTF_BIC;FTFP_INCLXX ;FTFP_INCLXX_HP; 
    //Shielding, FTFP_INCLXX_HP (energy con problem), NuBeam; 

    //By default, rare decay modes of charged pions not added: pi+ -> e+ + v ; pi- -> e- + vbar
    //T. J. Roberts, “Analysis of Geant4 Physics Processes for μ– Capture at Rest”,
    //http://mu2e-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=1119


	G4VModularPhysicsList* physicsList = new  QGSP_BERT_HP; //FTFP_INCLXX; //_HP; //Shielding; //  
    physicsList->RegisterPhysics( new G4NeutronTrackingCut(2) ); //no command for _HP
    //physicsList->RegisterPhysics(new G4RadioactiveDecayPhysics()); //not needed for shielding
	
    G4bool scint_opt = false;	
	if(scint_opt)
	{
		 G4OpticalPhysics* optical = new G4OpticalPhysics();
		 optical->SetScintillationByParticleType(true);
		 physicsList->RegisterPhysics(optical);
		 physicsList->RegisterPhysics(new G4RadioactiveDecayPhysics());
	}
	

    //use exampleRE06.cc 
    G4bool useParallel = false;
	SNSParallelTargetDetector* pdet = NULL;
    G4GeometrySampler* pgsN = NULL;
    if (argc >2 && (std::string(argv[2]) == "par" || std::string(argv[2]) == "parallel")) 
    {
		useParallel = true;
        G4String parallelName("SNSParallelWorld");
        pdet = new SNSParallelTargetDetector(parallelName);

        detector->RegisterParallelWorld(pdet);
    
        pgsN = new G4GeometrySampler(pdet->GetWorldVolume(),"neutron");
        pgsN->SetParallel(true);
        physicsList->RegisterPhysics(new G4ImportanceBiasing(pgsN,parallelName));//biasing
        physicsList->RegisterPhysics(new G4ParallelWorldPhysics(parallelName)); //scoring
    }
	physicsList->RegisterPhysics(new G4StepLimiterPhysics);//force step in vol
	//physicsList->RegisterPhysics(new SpecialCutsBuilder());//n cut time
	//physicsList->RegisterPhysics(new MaxTimeCuts());
	////physicsList->RegisterPhysics( new G4NeutronTrackingCut(1) );//see command
	runManager->SetUserInitialization(physicsList);
    
    
    //runManager->SetUserInitialization(new PhysicsList);	
    //Only 1 physics list should be instantiated.
	runManager->SetUserInitialization(new ActionInitialization(detector));
    
    runManager->Initialize();
	//initialization needed for imp. sampling, but for det. messenger, it mut be in macro.
    //so, need to move the physics into a seperate class
	if(pdet) pdet->CreateImportanceStore();//importance sampling

    #ifdef G4VIS_USE
        G4VisManager* visManager = new G4VisExecutive;
        visManager->Initialize();
    #endif

    G4UImanager* UImanager = G4UImanager::GetUIpointer();
	//UImanager->ApplyCommand("/tracking/verbose 1");

	  if(argc==1)
    // Define (G)UI terminal for interactive mode  
    { 
    #ifdef G4UI_USE
        G4UIExecutive * ui = new G4UIExecutive(argc, argv);
        //UImanager->ApplyCommand("/control/execute vis.mac");    
        ui->SessionStart();
        delete ui;
    #endif
    }
    else
    // Batch mode
    { 
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command+fileName);
    }

    // User actions, physics_list and detector_description are
    // owned and deleted by the run manager.
    
    // open geometry for clean biasing stores clean-up
    if(useParallel && pgsN)
    {
        G4GeometryManager::GetInstance()->OpenGeometry();
        pgsN->ClearSampling();
    }


    #ifdef G4VIS_USE
        delete visManager;
    #endif
    delete runManager;
    
    return 0;
}

/*
http://hypernews.slac.stanford.edu/HyperNews/geant4/get/hadronprocess/1403/4/1/1/1.html
to avoid RDM warnings. This class is registered as a G4UserStackingAction.

G4ClassificationOfNewTrack BLCMDrdecaysource::ClassifyNewTrack(const G4Track *track)
{
    //...
    if(track->GetParticleDefinition()->IsGeneralIon()) {
        double lifetime = track->GetParticleDefinition()->GetIonLifeTime();
        //@ workaround for HAD_RDM_011: lifetime = -1001 means
        //@ very short lived and not in the table.
        if(fabs(lifetime - -1001.0) < 0.1) {
            static bool once=true;
            if(once) G4Exception("rdecaysource","Workaround for HAD_RDM_011",JustWarning,"Will be fixed in Geant4.10.1");
            once = false;
            const_cast<G4ParticleDefinition*>(track->GetParticleDefinition())->SetPDGLifeTime(DBL_MIN);
        }
    }
    //... 
    return fUrgent;
}
*/