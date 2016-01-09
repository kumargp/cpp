

//Materials.cc

#include "globals.hh"
#include "Materials.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Isotope.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

std::map <G4String, G4Material*> Materials::mList;

Materials::Materials()
{
}

Materials::~Materials()
{
}
   

void Materials::DefineMaterials()
{
    G4double mass, fractionmass;  // atomic mass
    G4double z, temperature;  // atomic number
    G4double density, pressure;  // density
	G4String name, symbol;              
	G4int ncomp, natoms;
    
    // Define elements
    G4double mH_times_Av =  1.01, mC_times_Av = 12.011;
    G4double mF_times_Av = 18.9984;
    
    G4Element* elH = new G4Element ("Hydrogen", "H", z = 1., mass= 1.01*g/mole);
    G4Element* elD  = new G4Element("Deuterium", "D" , z=1., mass =2.*g/mole);
    G4Element* elBe  = new G4Element("Beryllium", "Be" , z= 4., mass =8.*g/mole);
    G4Element* elC = new G4Element("Carbon", "C", z = 6., mass= 12.011*g/mole); 
    G4Element* elO = new G4Element("Oxygen", "O", z = 8., mass = 16.00*g/mole);
    G4Element* elF = new G4Element("Flourine", "F", z = 9., mass =18.9984*g/mole);
    G4Element* elNa = new G4Element("Sodium", "Na", z = 11., mass= 22.99*g/mole);
    G4Element* elMg = new G4Element("Magnesium", "Mg", z = 12., mass= 24.305*g/mole);
    G4Element* elAl = new G4Element("Aluminum","Al", z= 13., mass =27.*g/mole);
    G4Element* elSi  = new G4Element("Silicon","Si", z= 14., mass =28.*g/mole);
    //G4Element* elP = new G4Element("Phosphorus", "P", z = 15., mass= 30.974*g/mole); 
    G4Element* elS = new G4Element("Sulfur", "S", z = 16., mass= 32.064*g/mole); 
    //G4Element* elCl = new G4Element("Chlorine", "Cl", z = 17., mass= 35.453*g/mole); 
    G4Element* elK = new G4Element("Potassium", "K", z = 19., mass = 39.098*g/mole);
    G4Element* elCa = new G4Element("Calcium", "C", z=20.,mass=40.08*g/mole);
    G4Element* elMn = new G4Element("Manganese","Mn", z=25., mass =55.*g/mole);
    G4Element* elFe  = new G4Element("Iron", "Fe", z = 26., mass=55.85*g/mole);    
    //G4Element* elZn = new G4Element("Zinc", "Zn", z = 30., mass= 65.38*g/mole);
    G4Element* elRb = new G4Element("Rb", "Rb", z = 37., mass= 85.47 *g/mole);
    //G4Element* elSr = new G4Element("Sr", "Sr", z = 38., mass= 87.62 *g/mole);
    //G4Element* elZr = new G4Element("Zr", "Zr", z = 40., mass= 91.22 *g/mole);
    G4Element* elSb = new G4Element("Sb", "Sb", z = 51., mass= 121.76 *g/mole);
    G4Element* elCs = new G4Element("Cs","Cs", z = 55., mass= 132.905*g/mole );  
    //G4Element* elPb = new G4Element("Lead", "Pb", z = 82., mass= 207.19 *g/mole);

   //materials      
   // concrete as a material
    G4Material* cement = new G4Material(name="Cement", density=3.9*g/cm3, 11);
    cement->AddElement(elO, fractionmass=55.714*perCent);
    cement->AddElement(elFe, fractionmass=28.026*perCent);
    cement->AddElement(elH, fractionmass=10.293*perCent);
    cement->AddElement(elCa , fractionmass=3.09*perCent);
    cement->AddElement(elSi , fractionmass=1.929*perCent);
    cement->AddElement(elAl, fractionmass=0.55*perCent);
    cement->AddElement(elMg, fractionmass=0.236*perCent);
    cement->AddElement(elS, fractionmass=0.092*perCent);
    cement->AddElement(elMn, fractionmass=0.044*perCent);
    cement->AddElement(elK, fractionmass=0.014*perCent);
    cement->AddElement(elNa, fractionmass=0.012*perCent);
    mList[name] = cement;
 
   //define liquid hydrogen as a material
    G4Material* LH2 = new G4Material(name="LiquidHydrogen", density=67.8*mg/cm3, 
      ncomp=1, kStateLiquid, temperature = 20*kelvin, pressure=1.*atmosphere);
    LH2 ->AddElement(elH, natoms=1);
    mList[name] = LH2;
   

    // Water G4_WATER from nist
  
    G4Material* Steel = new G4Material(name="Steel",density = 7.8*g/cm3, 2);
    Steel->AddElement(elFe, fractionmass=98.*perCent);
    Steel->AddElement(elC, fractionmass=2.*perCent);
    mList[name] = Steel;   
  
    // define material for Be plugs - Be(90%)+D2O(10%)
    G4Material* D2O = new G4Material(name="D2O",density = 1.1*g/cm3, 2);
    D2O->AddElement(elD, natoms=2);
    D2O->AddElement(elO, natoms=1);
    mList[name] = D2O;
    
    G4Material* BeD2O = new G4Material(name="BeD2O",density = 1.85*g/cm3, 2);
    BeD2O->AddElement(elBe, fractionmass=90.*perCent);
    BeD2O->AddMaterial(D2O, fractionmass=10.*perCent);
    mList[name] =  BeD2O;
 
    G4Material* SteelD2O = new G4Material(name="SteelD2O",density = 7.8*g/cm3, 2);
    SteelD2O->AddMaterial(Steel, fractionmass=95.*perCent);
    SteelD2O->AddMaterial(D2O, fractionmass=5.*perCent);   
    mList[name] = SteelD2O;    
    //Elements
 /*
    G4Isotope* ih1 = new G4Isotope("Hydrogen",iz=1,n=1,mass= 1.01*g/mole);
    G4Isotope* ih2 = new G4Isotope("Deuterium",iz=1,n=2,mass= 2.01*g/mole);
    elH->AddIsotope(ih1,.999);
    elH->AddIsotope(ih2,.001);
  */
    //Materials
    //Liquid Xenon
    //G4Material* fLXeMat = new G4Material(name="LXe",z=54.,mass=131.29*g/mole, density=3.020*g/cm3);
	//mList[name] = fLXeMat;
	
    G4Material* LArMat = new G4Material(name="LAr",z=18., mass= 39.95*g/mole, density= 1.4*g/cm3);
	mList[name] = LArMat;
	
	//Vacuum
    G4Material* fVacuumMat = new G4Material(name="Vacuum",z=1.,mass=1.01*g/mole,
                    density=universe_mean_density,kStateGas,0.1*kelvin, 1.e-19*pascal);
	mList[name] = fVacuumMat;
	
    //Glass
    G4Material* fGlassMat = new G4Material(name="Glass", density=1.032*g/cm3,3);
    fGlassMat->AddElement(elSi,60.*perCent);
    fGlassMat->AddElement(elO,32.*perCent);
    fGlassMat->AddElement(elH,8.*perCent);
	mList[name] = fGlassMat;
 /*        "Si O_2"               -0.800       #. Atomic fraction
          "B_2 O_3"              -0.120       #. Atomic fraction
          "Na_2 O"               -0.080       #. Atomic fraction
*/	
   //BiAlkai
    G4Material* fBiAlkaiMat = new G4Material(name="BiAlkai", density=1.032*g/cm3, 4);
    fBiAlkaiMat->AddElement(elSb,70.*perCent);
    fBiAlkaiMat->AddElement(elRb,10.*perCent);
    fBiAlkaiMat->AddElement(elCs,10.*perCent);
    fBiAlkaiMat->AddElement(elK,10.*perCent);
    mList[name] = fBiAlkaiMat;
   
   //SBCs
    G4Material* fSBCs = new G4Material(name="SBCs", density=1.032*g/cm3,2);
    fSBCs->AddElement(elSb,50.*perCent);
    fSBCs->AddElement(elCs,50.*perCent);
    mList[name] = fSBCs;

    //PTFE  (C2F4)n
    G4Material* fPTFE = new G4Material(name="PTFE", density=2.2*g/cm3,2);
    fPTFE->AddElement(elF,66.6666*perCent);
    fPTFE->AddElement(elC,33.3333*perCent);
    mList[name] = fPTFE;
    
	/*
    G4Material* fEpoxy = new G4Material(name="Epoxy", density=1.3*g/cm3,3);
    fEpoxy->AddElement(elC,15.*perCent);
    fEpoxy->AddElement(elH,44.*perCent);
    fEpoxy->AddElement(elO,7.*perCent);
    mList[name] = fEpoxy;
    */


    
  /*"Epoxy"                      3     1.3            #. comp.no. dens   GMIX
          "Carbon"                -15.            #. Atomic proportions
          "Hydrogen"              -44.
          "Oxygen"                -7.0
  */

/*
  example  optical/LXe/
  G4double RefractiveIndexFiber[wlsnum]={ 1.60, 1.60, 1.60, 1.60};
  G 4double AbsFiber[wlsnum]={9.00*m,9.00*m,0.1*mm,0.1*mm}*;
  G4double EmissionFib[wlsnum]={1.0, 1.0, 0.0, 0.0};
  G4MaterialPropertiesTable* fiberProperty = new G4MaterialPropertiesTable();
  fiberProperty->AddProperty("RINDEX",wls_Energy,RefractiveIndexFiber,wlsnum);
  fiberProperty->AddProperty("WLSABSLENGTH"fPTFE,wls_Energy,AbsFiber,wlsnum);
  fiberProperty->AddProperty("WLSCOMPONENT",wls_Energy,EmissionFib,wlsnum);
  fiberProperty->AddConstProperty("WLSTIMECONSTANT", 0.5*ns);
  fPMMA->SetMaterialPropertiesTable(fiberProperty);
  
  G4double RefractiveIndexClad1[wlsnum]={ 1.49, 1.49, 1.49, 1.49};
  G4MaterialPropertiesTable* clad1Property = new G4MaterialPropertiesTable();
  clad1Property->AddProperty("RINDEX",wls_Energy,RefractiveIndexClad1,wlsnum);
  clad1Property->AddProperty("ABSLENGTH",wls_Energy,AbsFiber,wlsnum);
  fPethylene1->SetMaterialPropertiesTable(clad1Property);
	*/
	
                                              
//MPT->AddProperty("RAYLEIGH",ppckov,scattering,NUMENTRIES}; 

// Set the Birks Constant for the LAr scintillator
	//LArMat->GetIonisation()->SetBirksConstant(0.126*mm/MeV);//0.05

    //mH_times_Av =  1.01; mC_times_Av = 12.011;
    G4double numHperV = 4.82e+22, numCperV = 3.98e+22;
    G4double massRatioH = 
          numHperV*mH_times_Av/(numHperV*mH_times_Av + numCperV*mC_times_Av);
          //    = 4.82E+22*1.01/(4.82E+22*1.01+ 3.98E+22*12.011) = 0.092424853

    density = 0.874*g/cm3;
    G4Material* ej301 = new G4Material(name="EJ301", density, 2);
    ej301->AddElement(elH, massRatioH);//=0.092424853);
    ej301->AddElement(elC, 1-massRatioH);//=0.907575147);
    mList[name] = ej301;
    
    //EJ301 liquid scintillator. Get wavelength and amplitude from emissionm spectrum,
    //convert nm to eV.
    G4int num = 34;
    G4double abL = 100.*cm;
    G4double rInd = 1.505;
	
    //extracted from emission plot from 
   //http://www.eljentechnology.com/index.php/products/liquid-scintillators/71-ej-301
    G4double ej301_Energy[] = { 3.070707*eV, 3.034127*eV, 3.009901*eV, 2.993647*eV, 
                              2.979450*eV, 2.961659*eV, 2.933084*eV, 2.923077*eV, 
                               2.913138*eV, 2.900585*eV, 2.881076*eV, 2.872295*eV, 
                               2.865309*eV, 2.850575*eV, 2.818182*eV, 2.803927*eV, 
                               2.788166*eV, 2.775029*eV, 2.766070*eV, 2.753945*eV, 
                               2.744321*eV, 2.728431*eV, 2.714286*eV, 2.694881*eV, 
                               2.668932*eV, 2.644962*eV, 2.618505*eV, 2.593286*eV, 
                               2.558089*eV, 2.527219*eV, 2.493122*eV, 2.453528*eV, 
                               2.418270*eV, 2.387636*eV };
    G4double ej301_SCINT[] = {0.094680, 0.193301, 0.393052, 0.594903, 0.796774, 
                              0.917796, 0.982134, 0.994145, 0.981915, 0.909048, 
                              0.801758, 0.763274, 0.734912, 0.708480, 0.704060, 
                              0.691770, 0.667338, 0.636875, 0.598382, 0.553788, 
                              0.505183, 0.450439, 0.405815, 0.357081, 0.306237, 
                              0.275615, 0.236872, 0.208240, 0.171378, 0.140627, 
                              0.117896, 0.084965, 0.066225, 0.057646 };
 
    G4double ej301_RIND[]  = {rInd, rInd, rInd, rInd, rInd, rInd, rInd,
                             rInd, rInd, rInd, rInd, rInd, rInd, rInd,
                             rInd, rInd, rInd, rInd, rInd, rInd, rInd,
                             rInd, rInd, rInd, rInd, rInd, rInd, rInd,
                             rInd, rInd, rInd, rInd, rInd, rInd }; 
    G4double ej301_ABSORBL[] = { abL, abL, abL, abL, abL, abL, abL, abL, abL, abL,
                               abL, abL, abL, abL, abL, abL, abL, abL, abL, abL,
                               abL, abL, abL, abL, abL, abL, abL, abL, abL, abL,
                               abL, abL, abL, abL };
    G4MaterialPropertiesTable* ej301PropTab = new G4MaterialPropertiesTable();
    ej301PropTab->AddProperty("FASTCOMPONENT", ej301_Energy, ej301_SCINT, num);
    ej301PropTab->AddProperty("SLOWCOMPONENT", ej301_Energy, ej301_SCINT, num);
    ej301PropTab->AddProperty("RINDEX",        ej301_Energy, ej301_RIND,  num);
    ej301PropTab->AddProperty("ABSLENGTH",     ej301_Energy, ej301_ABSORBL,  num);
    ej301PropTab->AddConstProperty("SCINTILLATIONYIELD",12000./MeV);
    ej301PropTab->AddConstProperty("RESOLUTIONSCALE",1.0); 
    ej301PropTab->AddConstProperty("FASTTIMECONSTANT",3.2*ns);
    ej301PropTab->AddConstProperty("SLOWTIMECONSTANT",32.3*ns); /// 270 
    ej301PropTab->AddConstProperty("YIELDRATIO",0.78);
    ej301->SetMaterialPropertiesTable(ej301PropTab);
    ej301->GetIonisation()->SetBirksConstant(0.152*mm/MeV);//152from GLG4Sim, RAT
    //Birks const is used by G4EmSaturation
// http://www-geant4.kek.jp/lxr/source/processes/electromagnetic/utils/src/G4EmSaturation.cc#L83   
    
  //if void SetFiniteRiseTime(const G4bool state);
         // If set, the G4Scintillation process expects the user to have
         // set the constant material property FAST/SLOWSCINTILLATIONRISETIME.
  //in G4Scintillation.hh



    //Birks const: http://arxiv-web3.library.cornell.edu/pdf/1105.2100v1.pdf
    //http://wiki.opengatecollaboration.org/index.php/
     //                  Users_Guide_V6.2:Generating_and_tracking_optical_photons
	 //expect = E*SCINTILLATIONYIELD; std=RESOLUTIONSCALE*expect;
     //SCINTILLATIONYIELD = FWHM/2.35*sqrt(expect)
     //ABSORBL (average distance traveled by a photon before being absorbed by the medium)
     //In physics, the absorption length is the distance traveled by an optical 
     //photon into a material when the probability that the photon has not been 
     //absorbed has dropped to 1/e. 
}



G4bool Materials::fInitialized = false;


G4Material* Materials::GetMaterial(G4String matName)
{

	if(! fInitialized) 
	{
		DefineMaterials();
		fInitialized = true;
	}
	
	std::map <G4String, G4Material*>::iterator it;
	for(it = mList.begin(); it!=mList.end(); ++it)
	{
		if( (*it).first == matName )
		{
			return (*it).second;
		}
	}
	G4cout << "Materials::GetMaterial() WARNING: The material: "
	       << matName << " does not exist in the table. Return NULL pointer."
	       << G4endl;
	return 0;
}

