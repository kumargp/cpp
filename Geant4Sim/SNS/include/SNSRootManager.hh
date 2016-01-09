#ifndef SNSRootManager_h
#define SNSRootManager_h 1

#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TList.h>

using namespace std;

class SNSRootManager
{
public:
  SNSRootManager(string fout="sns_out.root");
  virtual ~SNSRootManager();     

  // methods
  void Init();
  void Save();

  // methods for ROOT objects
  void FillHisto(string hname, Double_t x);
  void FillHisto(string hname, Double_t x, Double_t y);
  void FillHisto(string hname, Double_t x, Double_t y, Double_t z);
  void FillHisto(string hname, Int_t x);
  void FillHisto(string hname, Int_t x, Int_t y);
  void FillHisto(string hname, Int_t x, Int_t y, Int_t z);
  void FillHisto(string hname, Float_t x);
  void FillHisto(string hname, Float_t x, Float_t y);
  void FillHisto(string hname, Float_t x, Float_t y, Float_t z);
  void SetHistoBin(Int_t nbin, string hname,Double_t x);
  void GetHistoBin(Int_t nbin, string hname,Double_t &x);
  
private:
	SNSRootManager (const SNSRootManager &);
	SNSRootManager& operator= (const SNSRootManager &);	
  static  SNSRootManager* fgInstance; //Singleton instance
  
  // data members
  TH1F                 *fHist;
  TList                *fHlist;
  string               fOutFileName;

public:
  static SNSRootManager* Instance() {return fgInstance;}

};

#endif

   

