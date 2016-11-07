/*
  Parallel geometries for MJD.
  Cavern Air, Poly, Pb and cu shields.
  For use with Geant4, ROOT, MaGe framework

  Gopan Perumpilly
*/


#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4ThreeVector.hh"

#include "G4PVPlacement.hh"
#include "G4ios.hh"
#include <sstream>


#include "io/MGLogger.hh"
#include "mjdemonstrator/MJDemoParallelWorld.hh"
#include "mjdemonstrator/MJDemoParallelWorldMessenger.hh"

MJDemoParallelWorld::MJDemoParallelWorld() :
  MGGeometryParallelWorld(),
  fMessenger(0),
  fWorldVolume(NULL),
  fNParallelVolumesShield(30),
  fNParallelVolumesAir(5),
  fNParallelVolumesPoly(5)
{
  fMessenger = new MJDemoParallelWorldMessenger(this);
}

MJDemoParallelWorld::~MJDemoParallelWorld()
{
  delete fMessenger;
}

void MJDemoParallelWorld::Construct()
{
  fWorldVolume = GetWorld();
  MGLog(routine) << "Constructing Parallel MJDemo Worlds" << endlog;

  G4LogicalVolume* parallelWorldLogical = fWorldVolume->GetLogicalVolume();

  double xMin,yMin,zMin,xMax,yMax,zMax,stepX,stepY,stepZ, x, y, z;
  size_t isNum=0, isn=0;


  /*
  The dimensions have been modified for avoiding crossing of volumes.
  See the MJGeometryDemonstrator.cc and set the innerBox dimension of the cavern
  and use it as the max Z of the cavern air below.
  The min size odf air is a reasonable approximation so as to avoid the shadow Pb.
  For the subtraction dim see the cavern subtraction solid in the
  MJGeometryDemonstrator.cc.
  */

 // outer dimensions of  Cavern Air
  xMax = 7.1628*m;
  yMax = 20.5232*m;
  zMax = 2.0*m;  //modified
 //inner
  xMin = 2.5*m;
  yMin = 2.5*m;
  zMin = 1.524*m;


  double xSub = 4.6478*m, ySub = 16.5611*m, zSub=0;//, zDiff = zMax-zMin;

  //size_t fNParallelVolumesAir = 10;

  stepX = (xMax-xMin)/(fNParallelVolumesAir-1);
  stepY = (yMax-yMin)/(fNParallelVolumesAir-1);
  stepZ = (zMax-zMin)/(fNParallelVolumesAir-1);

  MGLog(routine) << "parallel Cavern volume spacing [cm]: x=" << stepX/cm
    << ", y=" << stepY/cm <<   ", z=" << stepZ/cm << endlog;

  G4VisAttributes* MagVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,1.0));

  for (int i = 0; i <fNParallelVolumesAir; i++){

    ostringstream parallelAirName;
    parallelAirName << "parallelAir"<< i;

    ostringstream parallelAirLogicalName;
    parallelAirLogicalName<<  "parallelLogicalAir"<< i;
    G4Box* cavernAir  = new G4Box(parallelAirName.str(), xMax-i*stepX,yMax-i*stepY,zMax-i*stepZ);

    G4LogicalVolume* parallelLogicalAir = new G4LogicalVolume(cavernAir, 0,
					parallelAirLogicalName.str(),0,0,0);

    parallelLogicalAir->SetVisAttributes(MagVisAtt);//blackVisAtt);

    MGLog(routine)
      << "Created Parallel Cavern Air Logical" << parallelAirLogicalName.str() <<  endlog;

    ostringstream parallelAirPhysicalName;
    parallelAirPhysicalName << "parallelAirPhysical" << i;

    double frac = 1./(fNParallelVolumesAir-1.0);

    //x=xSub *(1- frac*i), y=-ySub *(1- frac*i),z=zDiff *(1- frac*i);


    //for nested
    if(i==0){
      x=xSub;
      y=-ySub;
      z=zSub;
    }
    else {
     x=-xSub *frac;
     y=ySub *frac;
     z=zSub;
    }

    MGLog(routine)<<" translations:  " << x/cm<< " "<<  y/cm<< " " << z/cm << endl;
    G4ThreeVector trans(x,y,z);

    G4VPhysicalVolume* parallelAirPhysicalVolume = new G4PVPlacement(0, trans, parallelLogicalAir,
				parallelAirPhysicalName.str(),parallelWorldLogical, false, 0);


    SetImportanceValueForRegion(parallelAirPhysicalVolume,  pow(2.0, i+1.0) );
    isn = i+1;
    // the next volume will be nested in this volume
    parallelWorldLogical = parallelLogicalAir;
  }//for  cavern
  isNum= isn;


//Poly shield

  /*
    set the dimensions in MJDemoRadShieldPoly.cc. Modifiy it as needed.
  */

  // inner dimensions
  xMin = 43.0*25.4*mm;
  yMin = 35.0*25.4*mm;
  zMin = 36.0*25.4*mm;

  // outer dimensions
  xMax = 45.0*25.4*mm;
  yMax = 37.0*25.4*mm;
  zMax = 38.0*25.4*mm;


  //size_t fNParallelVolumesPoly = 5;

  stepX = (xMax-xMin)/(fNParallelVolumesPoly-1);
  stepY = (yMax-yMin)/(fNParallelVolumesPoly-1);
  stepZ = (zMax-zMin)/(fNParallelVolumesPoly-1);

  MGLog(routine) << "parallel poly spacing [cm]: x=" << stepX/cm
    << ", y=" << stepY/cm << ", z=" << stepZ/cm << endlog;

  x = 0.0, y = 0.0, z = 0.0;
  G4ThreeVector orig(x, y, z);

  for (int i = 0; i <fNParallelVolumesPoly; i++){

    ostringstream parallelPolyShieldName;
    parallelPolyShieldName << "parallelPolyShield"<< i;

    ostringstream parallelPolyLogicalName;
    parallelPolyLogicalName<<  "parallelLogicalPoly"<< i;

    G4Box* polyShield = new G4Box(parallelPolyShieldName.str(), xMax-i*stepX,
			    yMax-i*stepY,zMax-i*stepZ);

    G4LogicalVolume* parallelPolyLogical = new G4LogicalVolume(polyShield, 0,
					parallelPolyLogicalName.str(),0,0,0);

    G4VisAttributes* blackVisAtt = new G4VisAttributes(G4Colour(0.3, 0.3, 0.3));
    parallelPolyLogical->SetVisAttributes(blackVisAtt);

    MGLog(routine)
      << "Created Parallel Poly Shield Logical"
      << parallelPolyLogicalName.str() <<  endlog;

    ostringstream parallelPolyPhysicalName;
    parallelPolyPhysicalName << "parallelPolyPhysical" << i;

    G4VPhysicalVolume* parallelPolyPhysicalVolume = new G4PVPlacement(0,orig,
    parallelPolyLogical, parallelPolyPhysicalName.str(),parallelWorldLogical, false, 0);

    SetImportanceValueForRegion(parallelPolyPhysicalVolume,  pow(2.0, isNum+i+1.0) );
    isn = isNum+i+1;
    // the next volume will be nested in this volume
    parallelWorldLogical = parallelPolyLogical;
  }//for
  isNum= isn;



// Shields

  //Cu shield dimensions.
  // inner dimensions of Cu
  xMin = 16.031 * 25.4*mm;
  yMin = 7.906*25.4*mm;
  zMin = 9.954*25.4*mm;

  //Pb shield
  // inner dimensions of Pb
  //xMin = 508.0254*mm;
  //yMin = 330.2254*mm;
  //zMin = 355.6254*mm;

  // outer dimensions of Pb
  xMax = 38.0 * 25.4*mm;
  yMax = 30.0 * 25.4*mm;
  zMax = 32.0 * 25.4*mm;


  //size_t fNParallelVolumesShield = 30;

  stepX = (xMax-xMin)/(fNParallelVolumesShield-1);
  stepY = (yMax-yMin)/(fNParallelVolumesShield-1);
  stepZ = (zMax-zMin)/(fNParallelVolumesShield-1);

  MGLog(routine) << "parallel inner shield spacing [cm]: x=" << stepX/cm
    << ", y=" << stepY/cm << ", z=" << stepZ/cm << endlog;

  x = 0.0, y = 0.0, z = 0.0;
  G4ThreeVector origin(x, y, z);

  for (int i = 0; i <fNParallelVolumesShield; i++){

    ostringstream parallelLeadShieldName;
    parallelLeadShieldName << "parallelShield"<< i;

    ostringstream parallelLogicalName;
    parallelLogicalName<<  "parallelLogical"<< i;

    G4Box* leadShield = new G4Box(parallelLeadShieldName.str(), xMax-i*stepX,
			    yMax-i*stepY,zMax-i*stepZ);

    G4LogicalVolume* parallelLogical = new G4LogicalVolume(leadShield, 0,
					parallelLogicalName.str(),0,0,0);

    G4VisAttributes* blackVisAtt = new G4VisAttributes(G4Colour(0.3, 0.3, 0.3)); // black
    //blackVisAtt->SetForceWireframe( false );
    parallelLogical->SetVisAttributes(blackVisAtt);

    MGLog(routine)
      << "Created Parallel Shield Logical"
      << parallelLogicalName.str() <<  endlog;

    ostringstream parallelPhysicalName;
    parallelPhysicalName << "parallelShieldPhysical" << i;

    G4VPhysicalVolume* parallelPhysicalVolume = new G4PVPlacement(0,origin, parallelLogical,
				parallelPhysicalName.str(),parallelWorldLogical, false, 0);

    SetImportanceValueForRegion(parallelPhysicalVolume,  pow(2.0, isNum+i+1.0) );
    isn = isNum+i+1;
    // the next volume will be nested in this volume
    parallelWorldLogical = parallelLogical;
  }//for
  isNum= isn; // if needed further
}
