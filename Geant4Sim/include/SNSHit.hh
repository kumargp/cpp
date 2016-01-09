// 
//  This is part of Geant4 simulation code developed at UChicago, for simulating 
//  neutron background at SNS, and energy in CosI detector and scintillation detector.   
//  Author : Gopan Perumpilly , gopan.p@gmail.com
// 


#ifndef SNSHIT_HH
#define SNSHIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

class G4VPhysicalVolume;

class SNSHit : public G4VHit
{
public:
	SNSHit();
	SNSHit(const SNSHit&);
	virtual ~SNSHit();
	
	
public:
	// operators
	SNSHit& operator=(const SNSHit&);  //TODO implementation is not good
	G4int operator==(const SNSHit&) const; //TODO implementation is not good
	
	void* operator new(size_t);
	void  operator delete(void*);
	
	// methods from base class
	virtual void Draw() {}
	virtual void Print();

	//hits are all different. The per event summing is done in EventAction
	inline void SetParticle(G4String part) { fParticle = part; }
	inline G4String GetParticle() { return fParticle; }    

	inline void SetEdep(G4double edep) { fEdep = edep; }
	inline G4double GetEdep() { return fEdep; }    

	inline void SetEdepProt(G4double edep) { fEdepProt = edep; }
	inline G4double GetEdepProt() { return fEdepProt; }  

	inline void SetEdepC12(G4double edep) { fEdepC12 = edep; }
	inline G4double GetEdepC12() { return fEdepC12; } 
	
	inline void SetEdepElec(G4double edep) { fEdepElec = edep; }
	inline G4double GetEdepElec() { return fEdepElec; }  
	
	inline void SetEdepAlpha(G4double edep) { fEdepAlpha = edep; }
	inline G4double GetEdepAlpha() { return fEdepAlpha; }  
	
	inline void SetEdepBe(G4double edep) { fEdepBe = edep; }
	inline G4double GetEdepBe() { return fEdepBe; }  
	

	inline void SetTableNPhot(G4double num) {  fTableNPhot = num; }      
	inline G4int GetTableNPhot(){ return fTableNPhot;}        
	
	inline void SetG4nPhot(G4double num) { fG4nPhot = num; }
	inline G4int GetG4nPhot(){ return fG4nPhot ;}
	
	inline void SetGLG4nPhot(G4double num) { fGLG4nPhot = num; }
	inline G4int GetGLG4nPhot() {return fGLG4nPhot ;}
	
	inline void SetG4MethodNPhot(G4double num) { fG4MethodNPhot = num; }
	inline G4int GetG4MethodNPhot() { return fG4MethodNPhot ;}
	
	inline void SetPhysVol(G4VPhysicalVolume* physVol) { fPhysVol = physVol;}
	inline G4VPhysicalVolume* GetPhysVol() {return fPhysVol;}
	
	inline void SetGlobalPos(G4ThreeVector pos) { fGlobalPos = pos; }
	inline G4ThreeVector GetGlobalPos() { return fGlobalPos; }
	
	inline void SetLocalPos(G4ThreeVector pos) { fLocalPos = pos; }
	inline G4ThreeVector GetLocalPos() { return fLocalPos; }
	
	inline void SetTime(G4double time) { fTime = time; }
	inline G4double GetTime() { return fTime; }
	
	inline void SetPostProc(G4String postProc) { fPostProc = postProc; }
	inline G4String GetPostProc() { return fPostProc; }
	
	inline void SetPreProc(G4String preProc) { fPreProc = preProc; }
	inline G4String GetPreProc() { return fPreProc; }
	
	inline void SetTrackCreatorProc(G4String trackCreatorProc) 		
			{ fTrackCreatorProc = trackCreatorProc; }
	inline G4String GetTrackCreatorProc() { return fTrackCreatorProc; }
			
	inline void SetPartKE(G4double partKE) { fPartKE = partKE; }
	inline G4double GetPartKE() { return fPartKE; }
	
	inline void SetSecTotKE(G4double secTotKE) { fSecTotKE = secTotKE; }
	inline G4double GetSecTotKE() { return fSecTotKE; }
	
	inline void SetSecTotEn( G4double secTotEn) { fSecTotEn = secTotEn; }
	inline G4double GetSecTotEn() { return fSecTotEn; }		

	
private:
	G4String           fParticle;
	G4double           fEdep;
	G4double			fEdepProt;
	G4double			fEdepElec;
	G4double			fEdepC12;
	G4double			fEdepAlpha;
	G4double			fEdepBe;
	G4int              fTableNPhot;
	G4int               fG4nPhot;
	G4int               fGLG4nPhot;
	G4int               fG4MethodNPhot;
	G4ThreeVector      fGlobalPos;
	G4ThreeVector      fLocalPos;
	G4double           fTime;
	G4VPhysicalVolume* fPhysVol;
	G4String			fPostProc;
	G4String			fPreProc;
	G4String 			fTrackCreatorProc;
	G4double 			fPartKE;
	G4double 			fSecTotKE;
	G4double 			fSecTotEn;

};

//--------------------------------------------------------------------------

typedef G4THitsCollection<SNSHit> SNSHitsCollection;

extern G4ThreadLocal G4Allocator<SNSHit>* SNSHitAllocator;


//-------------------------------------------------------------------------

#endif