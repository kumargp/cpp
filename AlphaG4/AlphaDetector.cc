//AlphaDetector

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4VSolid.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"

#include "gerdageometry/GEGeometrySD.hh"
#include "database/MJDatabase.hh"
#include "database/MJDatabaseMaterial.hh"
#include "materials/MJMaterial.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4IStore.hh"
#include "G4GeometrySampler.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"
#include "G4UnitsTable.hh"

#include "G4TriangularFacet.hh"
#include "G4TessellatedSolid.hh"
#include "G4QuadrangularFacet.hh"

#include "geometry/MGGeometryGlobals.hh"
#include "io/MGLogger.hh"

#include "geometry/AlphaDetector.hh"
#include "geometry/AlphaMessenger.hh"

#include "TRandom3.h"
//---------------------------------------------------------------------------//

AlphaDetector::AlphaDetector( G4String serNum ):
  MGGeometryDetector( serNum ),
  fDetHeight(0),  fDetRadius(0),   fSampleHeight(0),   fSampleRadius(0),
  fSampleLength(0),  fSampleWidth(0), fGroove(0),fSpot(0), fDeadThick(0 ),
  fDetBoundThick(0), fSlot(0), fGap(0), fVacPressure(0), fDetMaterial(0 ),
  fDetMaterialName( "G4_Si" ),  fSampleMaterial(0),
  fSampleMaterialName( "G4_Pb" ),   fChamberMaterial(0),
  fChamberMaterialName("G4_Galactic"),   fDeadLMaterial(0),
  fDeadLMaterialName(""),   fDetVisAtt(0), fDLayerVisAtt(0),
  fSampleVisAtt(0), fBoundaryVisAtt(0)
{
  fMessenger = new AlphaMessenger(this);
}

//--------------------------------------------------------------------

AlphaDetector::AlphaDetector(const AlphaDetector & rhs):
  MGGeometryDetector(""),
  fDetHeight( rhs.fDetHeight ),
  fDetRadius( rhs.fDetRadius ),
  fSampleHeight( rhs.fSampleHeight ),
  fSampleRadius( rhs.fSampleRadius ),
  fSampleLength( rhs.fSampleLength),
  fSampleWidth( rhs.fSampleWidth),
  fGroove(rhs.fGroove),
  fSpot(rhs.fSpot),
  fDeadThick ( rhs.fDeadThick ),
  fDetBoundThick (rhs.fDetBoundThick),
  fSlot( rhs.fSlot ),
  fGap( rhs.fGap),
  fVacPressure (rhs.fVacPressure),
  fDetMaterial( rhs.fDetMaterial ),
  fDetMaterialName( rhs.fDetMaterialName ),
  fSampleMaterial( rhs.fSampleMaterial ),
  fSampleMaterialName( rhs.fSampleMaterialName ),
  fDeadLMaterial(rhs.fDeadLMaterial),
  fDeadLMaterialName(rhs.fDeadLMaterialName)
{
  theDetectorLogical = rhs.theDetectorLogical;
}

//--------------------------------------------------------------------

AlphaDetector::~AlphaDetector()
{
 delete fMessenger;
 delete fDetVisAtt;
 delete fDLayerVisAtt;
 delete fSampleVisAtt;
 delete fBoundaryVisAtt;
}

//--------------------------------------------------------------------


void AlphaDetector::ConstructDetector()
{
  if( theDetectorLogical ) return;

  if (!( (fSampleLength || fSampleRadius ) && fDetHeight && fDetRadius
     && fSampleHeight && fDeadThick && fSlot )){
      MGLog(error) << " Dimensions not set " << endlog;
  }



  G4double z, a, density, temperature, fractionmass;
  G4int    ncomponents, natoms;
  G4String name,symbol;


  G4Element* H = new G4Element(name="Hydrogen",symbol="H",z=1.,a=1.00794*g/mole);
  G4Element* C = new G4Element(name="Carbon",symbol="C",z=6.,a=12.011*g/mole);
  G4Element* O = new G4Element(name="Oxygen",symbol="O",z=8.,a=16.00*g/mole);
  G4Element* Si = new G4Element(name="Silicon",symbol="Si",z=14.,a=28.09*g/mole);
  G4Element* Mn = new G4Element(name="Manganese",symbol="Mn",z=25.,a=54.93805*g/mole);
  G4Element* Fe = new G4Element(name="Iron",symbol="Fe",z=26.,a=55.845*g/mole);
  G4Element* Ni = new G4Element(name="Nickel",symbol="Ni",z=28.,a=58.6934*g/mole);
  G4Element* Cr = new G4Element(name="Chromium",symbol="Cr",z=24.,a=51.9961*g/mole);
  G4Element* N  = new G4Element("Nitrogen", "N", z= 7., a= 14.01*g/mole);
  G4Element* Ar = new G4Element("Argon", "Ar",z=18., a= 39.948*g/mole);
  G4Element* B = new G4Element("Boron", "B",z=5., a=10.811*g/mole);
  G4Element* Al  = new G4Element("Aluminium" ,"Al", z=13., a=26.98*g/mole);
  G4Element* Na = new G4Element("Sodium"   ,"Na", z=11., a=22.99*g/mole);
  G4Element* K = new G4Element("Potassium","K",z=19.,a=39.098*g/mole);

  density = 1.7836*mg/cm3 ;
  G4Material* Argon = new G4Material(name="Argon"  , density, ncomponents=1);
  Argon->AddElement(Ar, 1);

  density = 1.25053*mg/cm3 ;
  G4Material* Nitrogen = new G4Material(name="N2"  , density, ncomponents=1);
  Nitrogen->AddElement(N, 2);

  density = 1.4289*mg/cm3 ;
  G4Material* Oxygen = new G4Material(name="O2"  , density, ncomponents=1);
  Oxygen->AddElement(O, 2);



  //density = universe_mean_density;

  //geant4/examples/advanced/purging_magnet/src/PurgMagDetectorConstruction.cc
  //density  = 1.2928*mg/cm3 ;       	// STP
  temperature = 293.15*kelvin; 		// STP_Temperature = 273.15*kelvin;
  //1 torr = 0.00131578947 atm; 20 mtorr = 20* 1.0e-3 *0.00131578947* atm
  //20 mtorr = 20* 1.0e-3 *0.001332* bar = 2.664e-5*bar
  //pressure for 20 mtorr =2.66644 Pa

  fVacPressure = 2.63157894e-5*STP_Pressure;//STP_Pressure    = 1.*atmosphere;

 //Density = 2.66644 / (287.05 * (20 + 273.15)) = 0.000031687 kg/m3
  density=0.000031687*kg/m3;


  G4Material* PumpedVacuum = new G4Material(name="PumpedVacuum", density,ncomponents=3,
                                   kStateGas,temperature, fVacPressure);
  //G4Material* PumpedVacuum = new G4Material("PumpedVacuum" , density, ncomponents=3);
  PumpedVacuum->AddMaterial( Nitrogen, fractionmass = 0.7557 ) ;
  PumpedVacuum->AddMaterial( Oxygen,   fractionmass = 0.2315 ) ;
  PumpedVacuum->AddMaterial( Argon,    fractionmass = 0.0128 ) ;
 /*


  G4Material* Air = new G4Material("Air"  , density= 1.290*mg/cm3, ncomponents=2);
  Air->AddElement(N, fractionmass=0.7);
  Air->AddElement(O, fractionmass=0.3);
  density     = 1.e-5*g/cm3;//2.376e-15*g/cm3;
  pressure    = 2.e-2*bar;//2.0e-7*bar;
  temperature = STP_Temperature;  // 300*kelvin;
  G4Material* vac=new G4Material("TechVacuum",density,1,kStateGas,temperature,pressure);
  vac->AddMaterial( Air, fractionmass=1. );

  density     = universe_mean_density;  // =1.e-25*g/cm3  from PhysicalConstants.h
  pressure    = 3.e-18*pascal;
  temperature = 2.73*kelvin;
  G4Material* vacuum =
    new G4Material("Galactic",z= 1,a= 1.008*g/mole,density,
		   kStateGas,temperature,pressure);
*/



 // Heat resistance boro silicate glass
 //80.8% SiO2 + 13% B2O3 + 2% Na2O + 2% K2O + 2.2% Al2O3. density = 2.2145 g/cm3;

  density = 2.648*g/cm3 ;
  G4Material* SiO2 = new G4Material(name="SiO2"  , density, ncomponents=2);
  SiO2->AddElement(Si, 1);
  SiO2->AddElement(O, 2);


  density = 2.46*g/cm3;
  G4Material* B2O3 = new G4Material(name="B2O3", density, ncomponents=2);
  B2O3->AddElement(B, natoms=2);
  B2O3->AddElement(O , natoms=3);

  density = 2.27*g/cm3;
  G4Material* Na2O = new G4Material(name="Na2O", density, ncomponents=2);
  Na2O->AddElement(Na, natoms=2);
  Na2O->AddElement(O , natoms=1);

  density = 2.35*g/cm3;
  G4Material* K2O = new G4Material(name="K2O", density, ncomponents=2);
  K2O->AddElement(K, natoms=2);
  K2O->AddElement(O , natoms=1);

  density = 4.0*g/cm3;
  G4Material* Al2O3 = new G4Material(name="Al2O3", density, ncomponents=2);
  Al2O3->AddElement(Al, natoms=2);
  Al2O3->AddElement(O , natoms=3);

  density = 2.215*g/cm3;
  G4Material* Glass = new G4Material(name="Glass", density, ncomponents=5);
  Glass->AddMaterial(SiO2, fractionmass=80.8*perCent);
  Glass->AddMaterial(B2O3, fractionmass=13.*perCent);
  Glass->AddMaterial(Na2O, fractionmass=2.*perCent);
  Glass->AddMaterial(K2O,  fractionmass=2.*perCent);
  Glass->AddMaterial(Al2O3, fractionmass=2.2*perCent);



  //stainless steel
  G4Material* ssteel = new G4Material(name="SSteel",density=7.9*g/cm3,ncomponents=5);
  ssteel->AddElement(Si,fractionmass= 0.01);
  ssteel->AddElement(Cr,fractionmass= 0.20);
  ssteel->AddElement(Mn,fractionmass= 0.02);
  ssteel->AddElement(Fe,fractionmass= 0.67);
  ssteel->AddElement(Ni,fractionmass= 0.10);

   // Acrylic
  G4Material* Acrylic= new G4Material(name="Acrylic",density=1.15*g/cm3, ncomponents=3);
  Acrylic->AddElement(C,natoms=5);
  Acrylic->AddElement(H,natoms=8);
  Acrylic->AddElement(O,natoms=2);


  G4NistManager* man = G4NistManager::Instance();
  fDetMaterial = man->FindOrBuildMaterial(fDetMaterialName);

  if( fSampleMaterialName == "SSteel" ) fSampleMaterial = ssteel;
  else if(fSampleMaterialName == "Acrylic" ) fSampleMaterial = Acrylic;
  //else if(fSampleMaterialName == "Glass" ) fSampleMaterial = Glass;
  else
    fSampleMaterial = man->FindOrBuildMaterial(fSampleMaterialName);


  if( fChamberMaterialName == "PumpedVacuum" ) fChamberMaterial = PumpedVacuum;
  else
    fChamberMaterial = man->FindOrBuildMaterial(fChamberMaterialName);

  if(fDeadLMaterialName == "") fDeadLMaterialName = fDetMaterialName;
  fDeadLMaterial = man->FindOrBuildMaterial(fDeadLMaterialName);

  //fSampleMaterial = G4Material::GetMaterial( fSampleMaterialName);
  //fDetMaterial = G4Material::GetMaterial( fDetMaterialName);
  //fChamberMaterial = G4Material::GetMaterial(fChamberMaterialName);




  if( !(fSampleMaterial && fDetMaterial && fChamberMaterial )) {
    MGLog(error) << "NULL Pointer for material: "
       <<  fDetMaterialName<< " and/or " << fSampleMaterialName
       << " and/or " << fChamberMaterialName << endlog;
    Dump();
    MGLog(fatal) << endlog;
  }


  //1/16 is only boundary, not covering the detector on front.
 // fGap = (-1/16+3/32.0 + fSlot* 5/32.0) *2.54*cm  - fSampleHeight;
  fGap = (3/32.0 + fSlot* 5/32.0) *2.54*cm  - fSampleHeight;

 // Chamber
  G4double worldSize =10*cm;
  fChamberBox = new G4Box("chamberBox",worldSize/2.0,worldSize/2.0,worldSize/2.0);
  theDetectorLogical = new G4LogicalVolume(fChamberBox,  fChamberMaterial,  "theDetectorLogical");
  fSolidVec.push_back(fChamberBox);
  fLogicalVolumeVec.push_back(theDetectorLogical);





  //
  // ADetector
  //
  G4VSolid* detVol = new G4Tubs("ADetector",0*cm,fDetRadius, fDetHeight/2.0, 0*deg, 360.0*deg );
  fSolidVec.push_back(detVol );

  G4LogicalVolume *detLog = new G4LogicalVolume(detVol,
		fDetMaterial, "activeDetLog", 0, 0, 0, true);

  fLogicalVolumeVec.push_back(detLog);
  G4ThreeVector posDet = G4ThreeVector(0,0,fGap/2.0 + fDeadThick + fDetHeight/2.0);

  G4VPhysicalVolume* detPhys = new G4PVPlacement(0,	//no rotation
  			   posDet,		//at
                           detLog,			//logical volume
			   "activeDetector",		//name
                           theDetectorLogical,	       	//mother  volume
                           false,			//no boolean operation
                           0,				//copy number
			true);				//overlap

  fPhysicalVolumeVec.push_back(detPhys);



  //
  // detBoundary, including that projecting on front that is 1/16"
  //

  G4VSolid* detBoundVol = new G4Tubs("detBoundVol",fDetRadius, fDetRadius+fDetBoundThick,
				     fDetHeight/2.0 + 1/16*2.54*cm, 0*deg, 360.0*deg );

  fSolidVec.push_back(detBoundVol );

  G4LogicalVolume *detBoundLog = new G4LogicalVolume(detBoundVol,
		fDetMaterial, "detBoundLog", 0, 0, 0, true);

  fLogicalVolumeVec.push_back(detBoundLog);

  G4VPhysicalVolume* detBoundPhys = new G4PVPlacement(0,	//no rotation
  			   posDet,		//at
                           detBoundLog,			//logical volume
			   "detBoundary",		//name
                           theDetectorLogical,	       	//mother  volume
                           false,			//no boolean operation
                           0,				//copy number
			true);				//overlap

  fPhysicalVolumeVec.push_back(detBoundPhys);



  //
  //dead layer
  //

  G4ThreeVector posDead = G4ThreeVector(0,0,fGap/2.0 + fDeadThick/2.0);

  G4VSolid* deadVol = new G4Tubs("deadVol", 0.,
		      fDetRadius, fDeadThick/2.0, 0*deg, 360.0*deg );

  fSolidVec.push_back( deadVol );

  G4LogicalVolume* deadLog = new G4LogicalVolume(deadVol, //shape
                               fDeadLMaterial,		//material
                              "DeadLog");		//name

  fLogicalVolumeVec.push_back(deadLog);

  G4VPhysicalVolume* deadPhys = new G4PVPlacement(0,	//no rotation
  			   posDead,		//at
                           deadLog,			//logical volume
			   "DeadLayer",		//name
                           theDetectorLogical, 		//mother  volume
                           false,			//no boolean operation
                           0,
			true);
  fPhysicalVolumeVec.push_back(deadPhys);


/*
  //
  // deadBoundary, it is included in the detboundary
  G4VSolid* deadBoundVol = new G4Tubs("deadBoundVol",fDetRadius, fDetRadius+fDetBoundThick,
				     fDeadThick/2.0, 0*deg, 360.0*deg );
  fSolidVec.push_back(deadBoundVol );
  G4LogicalVolume *deadBoundLog = new G4LogicalVolume(deadBoundVol,
		fDetMaterial, "deadBoundLog", 0, 0, 0, true);
  fLogicalVolumeVec.push_back(deadBoundLog);
  G4VPhysicalVolume* deadBoundPhys = new G4PVPlacement(0,  posDead, deadBoundLog,
  "deadBoundary", theDetectorLogical, false, 0,true);
  fPhysicalVolumeVec.push_back(deadBoundPhys);

*/


  //
  // Sample
  //
  G4ThreeVector posSample = G4ThreeVector(0,0,-fGap/2.0 - fSampleHeight/2.0);

  G4VSolid* sampleVol;
  if( fSampleLength ) {
    if( !fSampleWidth ) fSampleWidth = fSampleLength;
    sampleVol = new G4Box("sampleVol", fSampleLength/2.0, fSampleWidth/2.0,
				    fSampleHeight/2.0);
    fSampleRadius=0;
  }
  else if( fSampleRadius ){
    sampleVol = new G4Tubs("sampleVol", 0,
		      fSampleRadius, fSampleHeight/2.0, 0*deg, 360.0*deg );
  }
  G4LogicalVolume* sampleLog;


//surface Roughness shapes for subtraction on top surface of sample
  if(fGroove>0 || fSpot>0){
  //TRandom3 ran(0);
  //G4double rand = ran.Uniform(0,1 );
    G4VSolid* groove;
    G4VSolid* spot;

    G4double grooveRad = fGroove; //0.01*mm; //range
    G4double shiftZ = fSampleHeight/2.0;
    G4double sep = 2.0*2.0*grooveRad;
    G4int numHalfGrooves=0;
    if(sep>0) numHalfGrooves= (int) fSampleWidth/sep;

    G4double sepSpot= 2.0*fSpot;
    G4int numHalfSpots=0;
    if(sepSpot>0)numHalfSpots= (int) fSampleWidth/sepSpot;
    G4SubtractionSolid* sampleVolSub;

    G4cout << " numGrooves: " << 2*numHalfGrooves << G4endl;
    G4cout << " numSpots: " << 2*numHalfSpots << G4endl;

    if(fGroove>0){

      groove = new G4Tubs("groove", 0, grooveRad, fSampleWidth, 0*deg, 360.0*deg );
      G4RotationMatrix* rot = new G4RotationMatrix();
      rot->rotateX(90*deg);//pi/2);
      sampleVolSub = new G4SubtractionSolid("sampleVol", sampleVol,
		groove, rot, G4ThreeVector(0, 0,shiftZ));

      for(G4int num=-numHalfGrooves; num < numHalfGrooves; num++)
      {
	sampleVolSub = new G4SubtractionSolid("sampleVolSub", sampleVolSub,
	groove, rot, G4ThreeVector(num*sep, 0,shiftZ));
      }
    }
    if(fSpot>0){
      spot = new G4Cons("spot", 0, fSpot/2.0, 0, fSpot, fSpot/2.0, 0*deg, 360.0*deg );
      sampleVolSub = new G4SubtractionSolid("sampleVol", sampleVol,
		spot, 0, G4ThreeVector(0, 0,fSampleHeight/2.0) );
      for(G4int numX=-numHalfSpots; numX < numHalfSpots; numX++)
      {
	for(G4int numY=-numHalfSpots; numY < numHalfSpots; numY++)
	{
	  sampleVolSub = new G4SubtractionSolid("sampleVolSub", sampleVolSub,
		spot, 0, G4ThreeVector(numX*sepSpot, numY*sepSpot,fSampleHeight/2.0));
	}
      }
    }

    fSolidVec.push_back(sampleVolSub );

    sampleLog = new G4LogicalVolume(sampleVolSub,	//shape
                               fSampleMaterial,		//material
                              "SampleLog");		//name

  }else {
    fSolidVec.push_back(sampleVol);
    sampleLog = new G4LogicalVolume(sampleVol,	//shape
                               fSampleMaterial,		//material
                              "SampleLog");		//name
  }
  fLogicalVolumeVec.push_back(sampleLog);

  G4VPhysicalVolume* samplePhys = new G4PVPlacement(0,	//no rotation
  	posSample, sampleLog, "Sample", theDetectorLogical, false, 0, true);
  fPhysicalVolumeVec.push_back(samplePhys);


/*
  //texture
//fTesselated, fTessNum
//if(fTesselated ) {
  G4TessellatedSolid* texture = new G4TessellatedSolid("Texture");
  G4int num = 1;//fTessNum;
  G4double d = 1*mm;

  for(G4int i =0; i<num; i++){
   // if(fSampleLength){
      d = d*(i+1.0);
      G4ThreeVector v1(0,     0,    -10*mm );
      G4ThreeVector v2(d,   -d,    -d);
      G4ThreeVector v3(d, d,    d);

    G4TriangularFacet *facet = new G4TriangularFacet (v1, v3, v2, RELATIVE);//ABSOLUTE);
    texture->AddFacet((G4VFacet*) facet);
  }
  texture->SetSolidClosed(true);
  fSolidVec.push_back(texture );

  G4LogicalVolume* textureLog = new G4LogicalVolume(texture,fSampleMaterial,"TextureLog");
  fLogicalVolumeVec.push_back(textureLog);

  G4ThreeVector posTexture = G4ThreeVector(0.,0.,-fGap/2.0);
  G4VPhysicalVolume* texturePhys = new G4PVPlacement(0,	//no rotation
  	posTexture,  textureLog, "Texture",theDetectorLogical, false, 0, true);
  fPhysicalVolumeVec.push_back(texturePhys);
*/
//}//tess

//      G4TriangularFacet (const G4ThreeVector Pt0, const G4ThreeVector vt1,
//                         const G4ThreeVector vt2, G4FacetVertexType);
//
//   takes 4 parameters to define the three vertices:
//      1) G4FacetvertexType = "ABSOLUTE": in this case Pt0, vt1 and vt2 are
//         the 3 vertices in anti-clockwise order looking from the outsider.
//      2) G4FacetvertexType = "RELATIVE": in this case the first vertex is Pt0,
//         the second vertex is Pt0+vt1 and the third vertex is Pt0+vt2, all
//         in anti-clockwise order when looking from the outsider.

  //
  //color
  //
  //// lWorld->SetVisAttributes (G4VisAttributes::Invisible);

  fDetVisAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  fDetVisAtt->SetVisibility(true);
  detLog->SetVisAttributes(fDetVisAtt);

  fDLayerVisAtt= new G4VisAttributes(G4Colour(0.0,1.0,0.0));
  fDLayerVisAtt->SetVisibility(true);
  deadLog->SetVisAttributes(fDLayerVisAtt);

  fSampleVisAtt= new G4VisAttributes(G4Colour(0.0,1.0,1.0));
  fSampleVisAtt->SetVisibility(true);
  sampleLog->SetVisAttributes(fSampleVisAtt);


  fBoundaryVisAtt= new G4VisAttributes(G4Colour(0.5,0.5,0.5));
  fBoundaryVisAtt->SetVisibility(true);
  //deadBoundLog->SetVisAttributes(fBoundaryVisAtt);
  detBoundLog->SetVisAttributes(fBoundaryVisAtt);

  // added for stand-alone detector:
  SetDetectorName( GetSerialNumber() );
  Dump();
}

//--------------------------------------------------------------------

void AlphaDetector::Dump()
{
  G4cout << "Settings for AlphaDetector:" << G4endl;
  G4cout << "----------------------------------------" << G4endl;
  G4cout << "dead Layer thickness " << fDeadThick/angstrom << " Angstrom"<< G4endl;
  G4cout << "Det Height: " << fDetHeight/mm << " mm" << G4endl;
  G4cout << "Det Radius: " << fDetRadius/mm << " mm" <<G4endl;
  G4cout << "Sample Height: " << fSampleHeight/mm << " mm" <<G4endl;

  if(fSampleRadius!=0)G4cout << "Sample Radius: "<< fSampleRadius/mm << " mm"<<G4endl;
  else G4cout << "Sample Length: " << fSampleLength/mm << " mm" <<G4endl;

  G4cout << "Slot Number: " << fSlot << G4endl;
  G4cout << "Seperation: " << fGap/mm << " mm" <<G4endl;
  G4cout << "Sample Material: " <<  fSampleMaterialName << G4endl;
  G4cout << "Det Material: " <<  fDetMaterialName << G4endl;
  G4cout << "Chamber Material: " <<  fChamberMaterialName << "  "
	 << fVacPressure/STP_Pressure << "  atm" << G4endl;
  G4cout << "DeadLayer Material: " <<  fDeadLMaterialName << G4endl;

  G4cout << "----------------------------------------" << G4endl;
}
