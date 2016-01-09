// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 

#include "SNSHit.hh"
#include "G4UnitsTable.hh" 
#include "G4VVisManager.hh" //
#include "G4VPhysicalVolume.hh" //
#include "G4Circle.hh" //
#include "G4Colour.hh" //
#include "G4VisAttributes.hh" //
#include "G4ios.hh"

#include <iomanip>

G4ThreadLocal G4Allocator<SNSHit>* SNSHitAllocator = 0;

//--------------------------------------------------------------------------

SNSHit::SNSHit()
: G4VHit(),
fParticle("NULL"), fEdep(0), fEdepProt(0), fEdepElec(0), fEdepC12(0), 
fEdepAlpha(0), fEdepBe(0), fTableNPhot(0), fG4nPhot(0), fGLG4nPhot(0), 
fG4MethodNPhot(0),fGlobalPos(0), fLocalPos(0),  
fTime(0.), fPhysVol(0), fPostProc("NULL"), fPreProc("NULL"), 
fTrackCreatorProc("NULL"), fPartKE(0), fSecTotKE(0), fSecTotEn(0)
{;}

//--------------------------------------------------------------------------

SNSHit::~SNSHit() {}

//--------------------------------------------------------------------------

SNSHit::SNSHit(const SNSHit& right)
: G4VHit()
{
    fParticle           = right.fParticle;
	fEdep       		= right.fEdep;
	fEdepProt			=	right.fEdepProt;
	fEdepElec			=	right.fEdepElec;
	fEdepC12			=	right.fEdepC12; 
	fEdepAlpha			=	right.fEdepAlpha;
	fEdepBe				=	right.fEdepBe;
    fTableNPhot  		= right.fTableNPhot;
    fG4nPhot            = right.fG4nPhot;
    fGLG4nPhot          = right.fGLG4nPhot ;
    fG4MethodNPhot      = right.fG4MethodNPhot;
    fGlobalPos  		= right.fGlobalPos;
    fLocalPos   		= right.fLocalPos;
    fTime       		= right.fTime;
	fPhysVol    		= right.fPhysVol; // TODO use clone ;
	fPostProc			= right.fPostProc;
	fPreProc			= right.fPreProc;
	fTrackCreatorProc	= right.fTrackCreatorProc;
	fPartKE				= right.fPartKE;
	fSecTotKE			= right.fSecTotKE;
	fSecTotEn 			= right.fSecTotEn;
}

//--------------------------------------------------------------------------

SNSHit& SNSHit::operator=(const SNSHit& right)
{
    fParticle           = right.fParticle;
    fEdep       		= right.fEdep;
	fEdepProt			=	right.fEdepProt;
	fEdepElec			=	right.fEdepElec;
	fEdepC12			=	right.fEdepC12; 
	fEdepAlpha			=	right.fEdepAlpha;
	fEdepBe				=	right.fEdepBe;
    fTableNPhot         = right.fTableNPhot;
    fG4nPhot            = right.fG4nPhot;
    fGLG4nPhot          = right.fGLG4nPhot ;
    fG4MethodNPhot      = right.fG4MethodNPhot;
    fGlobalPos  		= right.fGlobalPos;
    fLocalPos   		= right.fLocalPos;
    fTime       		= right.fTime;
	fPhysVol    		= right.fPhysVol;// TODO use clone ;
	fPostProc			= right.fPostProc;
	fPreProc			= right.fPreProc;
	fTrackCreatorProc	= right.fTrackCreatorProc;
	fPartKE				= right.fPartKE;
	fSecTotKE			= right.fSecTotKE;
	fSecTotEn 			= right.fSecTotEn;
	
	return *this;
}

//--------------------------------------------------------------------------

void* SNSHit::operator new(size_t)
{
	if(!SNSHitAllocator)
		SNSHitAllocator = new G4Allocator<SNSHit>;
	void *hit;
	hit = (void *) SNSHitAllocator->MallocSingle();
	return hit;
}

void SNSHit::operator delete(void *hit)
{
	if(!SNSHitAllocator)
		SNSHitAllocator = new G4Allocator<SNSHit>;
	SNSHitAllocator->FreeSingle((SNSHit*) hit);
}



//--------------------------------------------------------------------------

G4int SNSHit::operator==(const SNSHit& right) const
{
	return ( this == &right ) ? 1 : 0;
}

//--------------------------------------------------------------------------

void SNSHit::Print()
{
	G4cout
	<< "Edep: " 
	<< std::setw(7) << G4BestUnit(fEdep,"Energy")
    << std::setw(7) << fTableNPhot
	<< G4endl;
}