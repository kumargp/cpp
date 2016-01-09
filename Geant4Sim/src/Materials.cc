// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

//Materials.cc

#include "globals.hh"
#include "Materials.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Isotope.hh"
#include "G4NistManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


Materials::Materials()
{
	DefineMaterials();
}

Materials::~Materials()
{
}

Materials::Materials(const Materials&)
{;}

Materials& Materials::operator=(const Materials&)
{
	return *this;
}

void Materials::DefineMaterials()
{
	G4NistManager* nist = G4NistManager::Instance();
    G4double mass, fractionmass;  // atomic mass
    G4double z, temperature;  // atomic number
    G4double density, pressure;  // density
	G4String name, symbol;              
	G4int ncomp, natoms;
    
    // Define elements

  
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
    G4Element* elP = new G4Element("Phosphorus", "P", z = 15., mass= 30.974*g/mole); 
    G4Element* elS = new G4Element("Sulfur", "S", z = 16., mass= 32.064*g/mole); 
    //G4Element* elCl = new G4Element("Chlorine", "Cl", z = 17., mass= 35.453*g/mole); 
    G4Element* elK = new G4Element("Potassium", "K", z = 19., mass = 39.098*g/mole);
    G4Element* elCa = new G4Element("Calcium", "C", z=20.,mass=40.08*g/mole);
	G4Element* elTi = new G4Element("Titanium", "Ti", z=22, mass=47.867*g/mole);
    G4Element* elMn = new G4Element("Manganese","Mn", z=25., mass =55.*g/mole);
    G4Element* elFe  = new G4Element("Iron", "Fe", z = 26., mass=55.85*g/mole);    
    //G4Element* elZn = new G4Element("Zinc", "Zn", z = 30., mass= 65.38*g/mole);
    G4Element* elRb = new G4Element("Rb", "Rb", z = 37., mass= 85.47 *g/mole);
    //G4Element* elSr = new G4Element("Sr", "Sr", z = 38., mass= 87.62 *g/mole);
    //G4Element* elZr = new G4Element("Zr", "Zr", z = 40., mass= 91.22 *g/mole);
    G4Element* elSb = new G4Element("Sb", "Sb", z = 51., mass= 121.76 *g/mole);
	G4Element* elI = new G4Element("I", "I", z=53., mass=126.90447*g/mole);
    G4Element* elCs = new G4Element("Cs","Cs", z = 55., mass= 132.905*g/mole );  
    //G4Element* elPb = new G4Element("Lead", "Pb", z = 82., mass= 207.19 *g/mole);

 /*
    G4Isotope* ih1 = new G4Isotope("Hydrogen",iz=1,n=1,mass= 1.01*g/mole);
    G4Isotope* ih2 = new G4Isotope("Deuterium",iz=1,n=2,mass= 2.01*g/mole);
    elH->AddIsotope(ih1,.999);
    elH->AddIsotope(ih2,.001);
  */


   //materials      
   // concrete as a material
    G4Material* concrete = new G4Material(name="Concrete", density=2.4*g/cm3, 11);
    concrete->AddElement(elO, fractionmass=55.714*perCent);
    concrete->AddElement(elFe, fractionmass=28.026*perCent);
    concrete->AddElement(elH, fractionmass=10.293*perCent);
    concrete->AddElement(elCa , fractionmass=3.09*perCent);
	concrete->AddElement(elSi , fractionmass=1.929*perCent);
    concrete->AddElement(elAl, fractionmass=0.55*perCent);
    concrete->AddElement(elMg, fractionmass=0.236*perCent);
    concrete->AddElement(elS, fractionmass=0.092*perCent);
    concrete->AddElement(elMn, fractionmass=0.044*perCent);
    concrete->AddElement(elK, fractionmass=0.014*perCent);
	concrete->AddElement(elNa, fractionmass=0.012*perCent);
 
	
	//NOTE. for Region cut. concrete for outer Building.
	G4Material* concBld = new G4Material(name="HDConcrete", density=3.93*g/cm3, 11);
	concBld->AddElement(elO, fractionmass=55.714*perCent);
	concBld->AddElement(elFe, fractionmass=28.026*perCent);
	concBld->AddElement(elH, fractionmass=10.293*perCent);
	concBld->AddElement(elCa , fractionmass=3.09*perCent);
	concBld->AddElement(elSi , fractionmass=1.929*perCent);
	concBld->AddElement(elAl, fractionmass=0.55*perCent);
	concBld->AddElement(elMg, fractionmass=0.236*perCent);
	concBld->AddElement(elS, fractionmass=0.092*perCent);
	concBld->AddElement(elMn, fractionmass=0.044*perCent);
	concBld->AddElement(elK, fractionmass=0.014*perCent);
	concBld->AddElement(elNa, fractionmass=0.012*perCent);
	
	
	// dummy LightConcrete
	G4Material* lightCon = new G4Material(name="LightConcrete", density=0.1*g/cm3, 11);
	lightCon->AddElement(elO, fractionmass=55.714*perCent);
	lightCon->AddElement(elFe, fractionmass=28.026*perCent);
	lightCon->AddElement(elH, fractionmass=10.293*perCent);
	lightCon->AddElement(elCa , fractionmass=3.09*perCent);
	lightCon->AddElement(elSi , fractionmass=1.929*perCent);
	lightCon->AddElement(elAl, fractionmass=0.55*perCent);
	lightCon->AddElement(elMg, fractionmass=0.236*perCent);
	lightCon->AddElement(elS, fractionmass=0.092*perCent);
	lightCon->AddElement(elMn, fractionmass=0.044*perCent);
	lightCon->AddElement(elK, fractionmass=0.014*perCent);
	lightCon->AddElement(elNa, fractionmass=0.012*perCent);

	// dummy LightConcrete
	G4Material* testCon = new G4Material(name="TestConcrete", density=0.5*g/cm3, 11);
	testCon->AddElement(elO, fractionmass=55.714*perCent);
	testCon->AddElement(elFe, fractionmass=28.026*perCent);
	testCon->AddElement(elH, fractionmass=10.293*perCent);
	testCon->AddElement(elCa , fractionmass=3.09*perCent);
	testCon->AddElement(elSi , fractionmass=1.929*perCent);
	testCon->AddElement(elAl, fractionmass=0.55*perCent);
	testCon->AddElement(elMg, fractionmass=0.236*perCent);
	testCon->AddElement(elS, fractionmass=0.092*perCent);
	testCon->AddElement(elMn, fractionmass=0.044*perCent);
	testCon->AddElement(elK, fractionmass=0.014*perCent);
	testCon->AddElement(elNa, fractionmass=0.012*perCent);
	
	G4Material* SiO2 = new G4Material(name="SiO2",density = 2.65*g/cm3, 2);
	SiO2->AddElement(elSi, natoms=1);
	SiO2->AddElement(elO, natoms=2);

	G4Material* Al2O3 = new G4Material(name="Al2O3",density = 3.95*g/cm3, 2);
	Al2O3->AddElement(elAl, natoms=2);
	Al2O3->AddElement(elO, natoms=3);
	
	G4Material* K2O = new G4Material(name="K2O",density =2.35*g/cm3, 2);
	K2O->AddElement(elK, natoms=2);
	K2O->AddElement(elO, natoms=1);	

	G4Material* Na2O = new G4Material(name="Na2O",density =2.27*g/cm3, 2);
	Na2O->AddElement(elNa, natoms=2);
	Na2O->AddElement(elO, natoms=1);
	
	G4Material* CaO = new G4Material(name="CaO",density = 3.35*g/cm3, 2);
	CaO->AddElement(elCa, natoms=1);
	CaO->AddElement(elO, natoms=1);
	 
	G4Material* FeO = new G4Material(name="FeO",density =5.74*g/cm3, 2);
	FeO->AddElement(elFe, natoms=1);
	FeO->AddElement(elO, natoms=1);
	 
	G4Material* Fe2O3 = new G4Material(name="Fe2O3",density =5.24*g/cm3, 2);
	Fe2O3->AddElement(elFe, natoms=2);
	Fe2O3->AddElement(elO, natoms=3);

	G4Material* MgO = new G4Material(name="MgO",density = 3.58*g/cm3, 2);
	MgO->AddElement(elMg, natoms=1);
	MgO->AddElement(elO, natoms=1);
	 
	G4Material* TiO2 = new G4Material(name="TiO2",density = 4.23*g/cm3, 2);
	TiO2->AddElement(elTi, natoms=1);
	TiO2->AddElement(elO, natoms=2);
	 
	G4Material* P2O5 = new G4Material(name="P2O5",density = 2.39*g/cm3, 2);
	P2O5->AddElement(elP, natoms=2);
	P2O5->AddElement(elO, natoms=5);
	 
	G4Material* MnO = new G4Material(name="MnO",density = 5.03*g/cm3, 2);
	MnO->AddElement(elMn, natoms=1);
	MnO->AddElement(elO, natoms=1);
	
	//Wiki. Gneiss also similar in composition( road base stone)
	G4Material* roadStone = new G4Material(name="RoadStone", density=1.4*g/cm3, 11); //before Apr15 1.6
	roadStone->AddMaterial(SiO2, fractionmass=72.0*perCent);
	roadStone->AddMaterial(Al2O3, fractionmass=14.29*perCent);	
	roadStone->AddMaterial(K2O, fractionmass=4.12*perCent);	
	roadStone->AddMaterial(Na2O, fractionmass=3.69*perCent);
	roadStone->AddMaterial(CaO, fractionmass=1.82*perCent);
	roadStone->AddMaterial(FeO, fractionmass=1.68*perCent);
	roadStone->AddMaterial(Fe2O3, fractionmass=1.22*perCent);
	roadStone->AddMaterial(MgO, fractionmass=0.71*perCent);
	roadStone->AddMaterial(TiO2, fractionmass=0.3*perCent);
	roadStone->AddMaterial(P2O5, fractionmass=0.12*perCent);
	roadStone->AddMaterial(MnO, fractionmass=0.05*perCent);
	
	
	G4Material* roadStoneLayer = new G4Material(name="RoadStoneLayer", density=1.4*g/cm3, 11); //before Apr15 1.6
	roadStoneLayer->AddMaterial(SiO2, fractionmass=72.0*perCent);
	roadStoneLayer->AddMaterial(Al2O3, fractionmass=14.29*perCent);	
	roadStoneLayer->AddMaterial(K2O, fractionmass=4.12*perCent);	
	roadStoneLayer->AddMaterial(Na2O, fractionmass=3.69*perCent);
	roadStoneLayer->AddMaterial(CaO, fractionmass=1.82*perCent);
	roadStoneLayer->AddMaterial(FeO, fractionmass=1.68*perCent);
	roadStoneLayer->AddMaterial(Fe2O3, fractionmass=1.22*perCent);
	roadStoneLayer->AddMaterial(MgO, fractionmass=0.71*perCent);
	roadStoneLayer->AddMaterial(TiO2, fractionmass=0.3*perCent);
	roadStoneLayer->AddMaterial(P2O5, fractionmass=0.12*perCent);
	roadStoneLayer->AddMaterial(MnO, fractionmass=0.05*perCent);
	
	
	
	//stone. Usually it is made of granite-gneiss, trap rock, limestone or any combination of these materials.
	// http://www.braenstone.com/crushed-stone-vs-quarry-process-vs-stone-dust/#ixzz34UedHicu
	//granite(wiki):SiO2 72.04%;Al2O3 14.42%;K2O 4.12%;Na2O 3.69%;CaO 1.82%;
	//FeO 1.68%; Fe2O3 1.22%;MgO 0.71%; TiO2 0.30%; P2O5 0.12% ;MnO 0.05%
	

    G4Material* earth = new G4Material(name="Earth", density=1.4*g/cm3,9); //1.6
    earth->AddElement(elC, 11.33*perCent);
    earth->AddElement(elO, 63.02*perCent);
    earth->AddElement(elNa,0.6 *perCent);
    earth->AddElement(elMg, 0.65 *perCent);
    earth->AddElement(elAl, 5.81*perCent);
    earth->AddElement(elSi, 16.19*perCent);
    earth->AddElement(elK, 0.96*perCent);
    earth->AddElement(elCa, 0.29*perCent);
    earth->AddElement(elFe, 1.16*perCent);
	
   //define liquid hydrogen as a material
    G4Material* LH2 = new G4Material(name="LiquidHydrogen", density=67.8*mg/cm3, 
      ncomp=1, kStateLiquid, temperature = 20*kelvin, pressure=1.*atmosphere);
    LH2 ->AddElement(elH, natoms=1);
   
    // Water G4_WATER from nist
  
    G4Material* Steel = new G4Material(name="Steel",density = 7.8*g/cm3, 2);
    Steel->AddElement(elFe, fractionmass=98.*perCent);
    Steel->AddElement(elC, fractionmass=2.*perCent);  
    // define material for Be plugs - Be(90%)+D2O(10%)
    G4Material* D2O = new G4Material(name="D2O",density = 1.1*g/cm3, 2);
    D2O->AddElement(elD, natoms=2);
    D2O->AddElement(elO, natoms=1);
    
	G4Material* BeD2O = new G4Material(name="BeD2O",density = 1.814636*g/cm3, 2);
    BeD2O->AddElement(elBe, fractionmass=98.*perCent);
    BeD2O->AddMaterial(D2O, fractionmass=2.*perCent);
 
	G4Material* SteelD2O = new G4Material(name="SteelD2O",density =  6.9174*g/cm3, 2);
    SteelD2O->AddMaterial(Steel, fractionmass=85.*perCent);
    SteelD2O->AddMaterial(D2O, fractionmass=15.*perCent);   
	
    // PMMA C5H8O2 ( Acrylic )
    density = 1.19*g/cm3;
    G4Material* Acrylic = new G4Material(name="Acrylic", density, 3);
    Acrylic->AddElement(elC, 5);
    Acrylic->AddElement(elH, 8);
    Acrylic->AddElement(elO, 2);



	//IRP material Al6061
	G4Material* matAl = nist->FindOrBuildMaterial("G4_Al");
	G4Material* matMg = nist->FindOrBuildMaterial("G4_Mg");
	G4Material* matSi = nist->FindOrBuildMaterial("G4_Si");
	G4Material* matCu = nist->FindOrBuildMaterial("G4_Cu");
	G4Material* matCr = nist->FindOrBuildMaterial("G4_Cr");
	
	G4Material* Al6061 = new G4Material("Al6061", 2.713*g/cm3, 5);
	Al6061->AddMaterial(matAl, 97.92*perCent);
	Al6061->AddMaterial(matMg, 1.00*perCent);
	Al6061->AddMaterial(matSi, 0.60*perCent);
	Al6061->AddMaterial(matCu, 0.28*perCent);
	Al6061->AddMaterial(matCr, 0.20*perCent);

	
	
    //Materials
    //Liquid Xenon
    //G4Material* fLXeMat = new G4Material(name="LXe",z=54.,mass=131.29*g/mole, density=3.020*g/cm3);

	
    new G4Material(name="LAr",z=18., mass= 39.95*g/mole, density= 1.4*g/cm3);

	
	//Vacuum
    new G4Material(name="Vacuum",z=1.,mass=1.01*g/mole,
                    density=universe_mean_density,kStateGas,0.1*kelvin, 1.e-19*pascal);
	
    //Glass
    G4Material* fGlassMat = new G4Material(name="Glass", density=1.032*g/cm3,3);
    fGlassMat->AddElement(elSi,60.*perCent);
    fGlassMat->AddElement(elO,32.*perCent);
    fGlassMat->AddElement(elH,8.*perCent);

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

   
   //SBCs
    G4Material* fSBCs = new G4Material(name="SBCs", density=1.032*g/cm3,2);
    fSBCs->AddElement(elSb,50.*perCent);
    fSBCs->AddElement(elCs,50.*perCent);

	//CsI
	G4Material* csI = new G4Material(name="CsI", density=4.5*g/cm3,2);
	csI->AddElement(elCs,50.*perCent);
	csI->AddElement(elI,50.*perCent);
	
    //PTFE  (C2F4)n
    G4Material* fPTFE = new G4Material(name="PTFE", density=2.2*g/cm3,2);
    fPTFE->AddElement(elF,66.6666*perCent);
    fPTFE->AddElement(elC,33.3333*perCent);

    //HDPE
    G4Material* HDPE = new G4Material(name="HDPE", density=0.941*g/cm3,2);
    HDPE->AddElement(elH,66.6666*perCent);
    HDPE->AddElement(elC,33.3333*perCent);


    //Brick
    G4Material* brick = new G4Material(name="Brick", density=2.4*g/cm3,9);
    brick->AddElement(elC, 11.33*perCent);
    brick->AddElement(elO, 63.02*perCent);
    brick->AddElement(elNa,0.6 *perCent);
    brick->AddElement(elMg, 0.65 *perCent);
    brick->AddElement(elAl, 5.81*perCent);
    brick->AddElement(elSi, 16.19*perCent);
    brick->AddElement(elK, 0.96*perCent);
    brick->AddElement(elCa, 0.29*perCent);
    brick->AddElement(elFe, 1.16*perCent);

	/*
    G4Material* fEpoxy = new G4Material(name="Epoxy", density=1.3*g/cm3,3);
    fEpoxy->AddElement(elC,15.*perCent);
    fEpoxy->AddElement(elH,44.*perCent);
    fEpoxy->AddElement(elO,7.*perCent);

    */

    new G4Material("matPipe",29., 63.546*g/mole, 8.96*g/cm3);
    new G4Material("matCavePlate",29., 63.546*g/mole, 8.96*g/cm3);
    new G4Material("matFloorPlate",29., 63.546*g/mole, 8.96*g/cm3);

	//C7H8 matetrial without Scint light
	G4Material* matc7h8 = new G4Material(name="MatC7H8", density=0.87*g/cm3, ncomp=2);
	matc7h8->AddElement(elC, natoms=7);
	matc7h8->AddElement(elH, natoms=8);
	
	
	
	//C7H8 scintillator
	G4Material* scintc7h8 = new G4Material(name="C7H8", density=0.87*g/cm3, ncomp=2);
	scintc7h8->AddElement(elC, natoms=7);
	scintc7h8->AddElement(elH, natoms=8);
	//G4MaterialPropertiesTable* c7h8PropTab = new G4MaterialPropertiesTable();
	//TODO add properties ONLY if scint o/p is needed.
	

	
	
    //EJ301 liquid scintillator
    G4double mH_times_Av =  1.01; 
    G4double mC_times_Av = 12.011;
    G4double numHperV = 4.82e+22, numCperV = 3.98e+22;
    G4double massRatioH = 
          numHperV*mH_times_Av/(numHperV*mH_times_Av + numCperV*mC_times_Av);
          //    = 4.82E+22*1.01/(4.82E+22*1.01+ 3.98E+22*12.011) = 0.092424853

    density = 0.874*g/cm3;
    G4Material* ej301 = new G4Material(name="EJ301", density, 2);
    ej301->AddElement(elH, massRatioH);//=0.092424853);
    ej301->AddElement(elC, 1-massRatioH);//=0.907575147);
    
    //Get wavelength and amplitude from emissionm spectrum,
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
}


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

