#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "AnitaCanvasMaker.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TPaveLabel.h"
#include "TROOT.h"
#include "TMath.h"
#include <iostream>
#include <fstream>

void plotCanvas(char *baseDir,int run, int startEntry, int numEntries);

void plotCanvas()
{ 
   plotCanvas("/unix/anita2/testing/rootFiles",1028,1000,1);
}
  

int getNiceColour(int index)
{
    if(index>10) return index;
    Int_t niceColours[11]={kAzure+2,kRed+1,kGreen+1,kMagenta+1,kCyan+1,kOrange+2,kGreen-2,12,40,20,41};
    return niceColours[index];
}


void plotCanvas(char *baseDir,int run, int startEntry, int numEntries) {
  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  //  char hkName[FILENAME_MAX];
  sprintf(eventName,"%s/run%d/eventFile%d*.root",baseDir,run,run);
  sprintf(headerName,"%s/run%d/headFile%d.root",baseDir,run,run);
  //  sprintf(hkName,"/unix/anita2/testing/rootFiles/run%d/prettyHkFile%d.root",run,run);
  cout << eventName << endl << headerName << endl;
  RawAnitaEvent *event = 0;
  RawAnitaHeader *header =0;
  //  PrettyAnitaHk *hk = 0;
  
  TChain *eventChain = new TChain("eventTree");
  eventChain->Add(eventName);
  eventChain->SetBranchAddress("event",&event);

  TFile *fpHead = new TFile(headerName);
  if(!fpHead) {
    cout << "Couldn't open: " << headerName << "\n";
    return;
  }
  TTree *headTree = (TTree*) fpHead->Get("headTree");
  headTree->SetBranchAddress("header",&header);

  //  TFile *fpHk = new TFile(hkName);
  //  TTree *prettyHkTree = (TTree*) fpHk->Get("prettyHkTree");
  //  prettyHkTree->SetBranchAddress("hk",&hk);

  AnitaCanvasMaker *canMaker=AnitaCanvasMaker::Instance();

  for(int entry=startEntry;entry<startEntry+numEntries;entry++) {
    
    //Stupidly most do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    //    prettyHkTree->GetEntry(entry);
    
    
    UsefulAnitaEvent realEvent(event,WaveCalType::kJustUnwrap);
    cout << realEvent.eventNumber << " " << header->eventNumber << endl;
    //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
    TCanvas *canVert=(TCanvas*)canMaker->getVerticalCanvas(&realEvent,header);
    canVert->Update();
    gSystem->Sleep(1000);
  }
  //  canSurf->cd(10);
  //  sqaurey->Draw();
}
