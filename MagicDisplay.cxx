//////////////////////////////////////////////////////////////////////////////
/////  MagicDisplay.cxx        ANITA Event Canvas make               /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for making pretty event canvases for ANITA-II            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include "MagicDisplay.h"
#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "WaveformGraph.h"
#include "AnitaCanvasMaker.h"

#include "TROOT.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"

using namespace std;

MagicDisplay*  MagicDisplay::fgInstance = 0;
AnitaCanvasMaker *fCanMaker=0;
//Leave these as global variables for now
RawAnitaEvent *rawEventPtr = 0;
RawAnitaHeader *headPtr =0;
PrettyAnitaHk *hkPtr =0;

MagicDisplay::MagicDisplay()
{
  //Default constructor
 
  
}

MagicDisplay::~MagicDisplay()
{
   //Default destructor
}


MagicDisplay::MagicDisplay(char *baseDir, int run)
{
  //Offline constructor
  cout << "MagicDisplay::MagicDisplay(" << baseDir << " , " << run
       << ")" << endl;
  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  //  char hkName[FILENAME_MAX];
  sprintf(eventName,"%s/run%d/eventFile%d*.root",baseDir,run,run);
  sprintf(headerName,"%s/run%d/headFile%d.root",baseDir,run,run);
  //  sprintf(hkName,"/unix/anita2/testing/rootFiles/run%d/prettyHkFile%d.root",run,run);
  //  cout << eventName << endl << headerName << endl;
  //  PrettyAnitaHk *hk = 0;
  
  fEventTree = new TChain("eventTree");
  fEventTree->Add(eventName);
  fEventTree->SetBranchAddress("event",&rawEventPtr);

  TFile *fpHead = new TFile(headerName);
  if(!fpHead) {
    cout << "Couldn't open: " << headerName << "\n";
    return;
  }
  fHeadTree = (TTree*) fpHead->Get("headTree");
  if(!fHeadTree) {
    cout << "Couldn't get headTree from " << headerName << endl;
    return;
  }
  fHeadTree->SetBranchAddress("header",&headPtr);
  fEntry=0;
  //  TFile *fpHk = new TFile(hkName);
  //  TTree *prettyHkTree = (TTree*) fpHk->Get("prettyHkTree");
  //  prettyHkTree->SetBranchAddress("hk",&hk);  
  startDisplay();
}


//______________________________________________________________________________
MagicDisplay*  MagicDisplay::Instance()
{
   //static function
   return (fgInstance) ? (MagicDisplay*) fgInstance : new MagicDisplay();
}

void MagicDisplay::startDisplay()
{
  fCanMaker=AnitaCanvasMaker::Instance();
  fEventTree->GetEntry(fEntry);
  fHeadTree->GetEntry(fEntry);
  //Need to make configurable at some point
  //This will also need to be modifed to make realEvent accessible outside here
  UsefulAnitaEvent realEvent(rawEventPtr,WaveCalType::kJustUnwrap);
  
  //This will need to change
  fMagicCanvas = new TCanvas("canMagic","canMagic",1200,800);
  fMagicMainPad = new TPad("canMagicMain","canMagicMain",0,0,1,0.9);
  fMagicMainPad->Draw();
  

  fCanMaker->getVerticalCanvas(&realEvent,headPtr,fMagicMainPad);
  fMagicCanvas->Update();
}
