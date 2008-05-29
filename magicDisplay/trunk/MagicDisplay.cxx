//////////////////////////////////////////////////////////////////////////////
/////  MagicDisplay.cxx        ANITA Event Canvas maker                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for making pretty event canvases for ANITA-II            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
//System includes
#include <fstream>
#include <iostream>

//Magic Display Includes
#include "MagicDisplay.h"
#include "WaveformGraph.h"
#include "AnitaCanvasMaker.h"
#include "AnitaRFCanvasMaker.h"

//Event Reader Includes
#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "TurfRate.h"
#include "SurfHk.h"

//ROOT Includes
#include "TROOT.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TButton.h"
#include "TGroupButton.h"

using namespace std;

MagicDisplay*  MagicDisplay::fgInstance = 0;
AnitaCanvasMaker *fEventCanMaker=0;
AnitaRFCanvasMaker *fRFCanMaker=0;
//Leave these as global variables for now

MagicDisplay::MagicDisplay()
{
  //Default constructor
   fRawEventPtr=0;
   fHeadPtr=0;
   fHkPtr=0;
   fUsefulEventPtr=0;
   fSurfPtr=0;
   fTurfPtr=0;
   fgInstance=this;
   fMainOption=MagicDisplayOption::kWavePhiVerticalOnly;
   fMagicCanvas=0;
   fMagicEventInfoPad=0;
   fMagicMainPad=0;
  
}

MagicDisplay::~MagicDisplay()
{
   //Default destructor
}


MagicDisplay::MagicDisplay(char *baseDir, int run)
{
  //Offline constructor
   fRawEventPtr=0;
   fHeadPtr=0;
   fHkPtr=0;
   fUsefulEventPtr=0;
   fTurfRateTree=0;
   fTurfPtr=0;
   fSurfPtr=0;
   fSurfHkEntry=0;
   fSurfHkTree=0;
   fgInstance=this;
   fMainOption=MagicDisplayOption::kWavePhiVerticalOnly;
   fMagicCanvas=0;
   fMagicEventInfoPad=0;
   fMagicMainPad=0;
   cout << "MagicDisplay::MagicDisplay(" << baseDir << " , " << run
       << ")" << endl;
  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  char turfName[FILENAME_MAX];
  char surfName[FILENAME_MAX];
  //  char hkName[FILENAME_MAX];
  sprintf(eventName,"%s/run%d/eventFile%d*.root",baseDir,run,run);
  sprintf(headerName,"%s/run%d/headFile%d.root",baseDir,run,run);
  sprintf(turfName,"%s/run%d/turfRateFile%d.root",baseDir,run,run);
  sprintf(surfName,"%s/run%d/surfHkFile%d.root",baseDir,run,run);
  //  sprintf(hkName,"/unix/anita2/testing/rootFiles/run%d/prettyHkFile%d.root",run,run);
  //  cout << eventName << endl << headerName << endl;
  //  PrettyAnitaHk *hk = 0;
  
  fEventTree = new TChain("eventTree");
  fEventTree->Add(eventName);
  fEventTree->SetBranchAddress("event",&fRawEventPtr);

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
  fHeadTree->SetBranchAddress("header",&fHeadPtr);
  fEventEntry=0;

  TFile *fpTurf = new TFile(turfName);
  if(!fpTurf) {
     cout << "Couldn't open: " << turfName << "\n";
     return;
  }
  fTurfRateTree = (TTree*) fpTurf->Get("turfRateTree");
  if(!fTurfRateTree) {
     cout << "Couldn't get turfRateTree from " << turfName << endl;
     return;
  }
  fTurfRateTree->SetBranchAddress("turf",&fTurfPtr);
  fTurfRateEntry=0;


  TFile *fpSurf = new TFile(surfName);
  if(!fpSurf) {
     cout << "Couldn't open: " << surfName << "\n";
     return;
  }
  fSurfHkTree = (TTree*) fpSurf->Get("surfHkTree");
  if(!fSurfHkTree) {
     cout << "Couldn't get surfRateTree from " << surfName << endl;
     return;
  }
  fSurfHkTree->SetBranchAddress("surf",&fSurfPtr);
  fSurfHkEntry=0;

  //  TFile *fpHk = new TFile(hkName);
  //  TTree *prettyHkTree = (TTree*) fpHk->Get("prettyHkTree");
  //  prettyHkTree->SetBranchAddress("hk",&hk);  
  //  startEventDisplay();
}


//______________________________________________________________________________
MagicDisplay*  MagicDisplay::Instance()
{
   //static function
   return (fgInstance) ? (MagicDisplay*) fgInstance : new MagicDisplay();
}

void MagicDisplay::startEventDisplay()
{
  fEventCanMaker=AnitaCanvasMaker::Instance();
  int retVal=getEventEntry();
  if(retVal==0)
     refreshEventDisplay();   
}

int MagicDisplay::getEventEntry()
{

   if(fEventEntry<fEventTree->GetEntries())
      fEventTree->GetEntry(fEventEntry);
   else {
      std::cout << "No more entries in event tree" << endl;
      return -1;
   }
            
   if(fEventEntry<fHeadTree->GetEntries())
      fHeadTree->GetEntry(fEventEntry);
   else {
      std::cout << "No more entries in header tree" << endl;
      return -1;
   }
   if(fUsefulEventPtr)
      delete fUsefulEventPtr;
   fUsefulEventPtr = new UsefulAnitaEvent(fRawEventPtr,WaveCalType::kJustUnwrap);  
  //Need to make configurable at some point
  //This will also need to be modifed to make realEvent accessible outside here
   return 0;
}

void MagicDisplay::refreshEventDisplay()
{
   if(!fMagicCanvas) {
      fMagicCanvas = new TCanvas("canMagic","canMagic",1200,800);
      fMagicCanvas->cd();
      drawEventButtons();
   }
   if(!fMagicMainPad) {
      fMagicCanvas->cd();
      fMagicMainPad= new TPad("canMagicMain","canMagicMain",0,0,1,0.9);
      fMagicMainPad->Draw();
      fMagicCanvas->Update();
   }
   if(!fMagicEventInfoPad) {
      fMagicCanvas->cd();
      fMagicEventInfoPad= new TPad("canMagicEventInfo","canMagicEventInfo",0.2,0.91,0.8,0.99);
      fMagicEventInfoPad->Draw();
      fMagicCanvas->Update();
   }  
   //   fMagicMainPad->Clear();

           
  //This will need to change
   fEventCanMaker->getEventInfoCanvas(fHeadPtr,fMagicEventInfoPad);
   switch(fMainOption) {
   case MagicDisplayOption::kWavePhiVerticalOnly:
      fEventCanMaker->getVerticalCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      break;
   case MagicDisplayOption::kWavePhiHorizontalOnly:
      fEventCanMaker->getHorizontalCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      break;
   case MagicDisplayOption::kWavePhiCombined:
      fEventCanMaker->getCombinedCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      //      fEventCanMaker->setupPhiPadWithFrames(fMagicMainPad);
      break;
   default:
      fEventCanMaker->getVerticalCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      break;
   }

  fMagicCanvas->Update();
}

int MagicDisplay::displayNextEvent()
{
   fEventEntry++;
   int retVal=getEventEntry();
   if(retVal==0) 
      refreshEventDisplay(); 
   else fEventEntry--;
   return retVal;  
}


int MagicDisplay::displayPreviousEvent()
{
   if(fEventEntry>0)
      fEventEntry--;
   else 
      return -1;
   int retVal=getEventEntry();
   if(retVal==0) 
      refreshEventDisplay();   
   return retVal;  
}


void MagicDisplay::drawEventButtons() {
   TButton *butNext = new TButton("Next Event","MagicDisplay::Instance()->displayNextEvent();",0.9,0.95,1,1);
   butNext->SetTextSize(0.5);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   TButton *butPrev = new TButton("Prev. Event","MagicDisplay::Instance()->displayPreviousEvent();",0.9,0.90,1,0.95);
   butPrev->SetTextSize(0.5);
   butPrev->SetFillColor(kBlue-10);
   butPrev->Draw();

   fVertButton = new TButton("Vertical","MagicDisplay::Instance()->setMainCanvasOption(MagicDisplayOption::kWavePhiVerticalOnly); MagicDisplay::Instance()->refreshEventDisplay();",0,0.97,0.05,1);
   fVertButton->SetTextSize(0.4);
   fVertButton->SetFillColor(kGray+3);
   fVertButton->Draw();
   fHorizButton = new TButton("Horizontal","MagicDisplay::Instance()->setMainCanvasOption(MagicDisplayOption::kWavePhiHorizontalOnly); MagicDisplay::Instance()->refreshEventDisplay();",0,0.94,0.05,0.97);
   fHorizButton->SetTextSize(0.4);
   fHorizButton->SetFillColor(kGray);
   fHorizButton->Draw();
   fBothButton = new TButton("Both","MagicDisplay::Instance()->setMainCanvasOption(MagicDisplayOption::kWavePhiCombined); MagicDisplay::Instance()->refreshEventDisplay();",0,0.91,0.05,0.94);
   fBothButton->SetTextSize(0.4);
   fBothButton->SetFillColor(kGray);
   fBothButton->Draw();
   
}

void MagicDisplay::setMainCanvasOption(MagicDisplayOption::MagicDisplayOption_t option)
{
   fMainOption=option;
   switch(option) {
   case MagicDisplayOption::kWavePhiVerticalOnly:
   case MagicDisplayOption::kPowerPhiVerticalOnly:
      fVertButton->SetFillColor(kGray+3);
      fHorizButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray);
      break;
   case MagicDisplayOption::kWavePhiHorizontalOnly:
   case MagicDisplayOption::kPowerPhiHorizontalOnly:
      fHorizButton->SetFillColor(kGray+3);
      fVertButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray);
      break;
   case MagicDisplayOption::kWavePhiCombined:
   case MagicDisplayOption::kPowerPhiCombined:
      fHorizButton->SetFillColor(kGray);
      fVertButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray+3);
      break;
   default:
      break;
   }   
   fVertButton->Modified();
   fHorizButton->Modified();  
   fBothButton->Modified();  
}



void MagicDisplay::startTurfDisplay()
{
   fRFCanMaker=AnitaRFCanvasMaker::Instance();
   int retVal=getTurfEntry();
   if(retVal==0)
      refreshTurfDisplay();   
}

int MagicDisplay::getTurfEntry() 
{
   if(fTurfRateEntry<fTurfRateTree->GetEntries())
      fTurfRateTree->GetEntry(fTurfRateEntry);
   else {
      std::cout << "No more entries in turf rate tree" << endl;
      return -1;
   }
   //   std::cout << fTurfRateEntry << "\t" << fTurfPtr->realTime 
   //	     << "\t" << fTurfPtr->ppsNum << std::endl;

   return 0;
}

int MagicDisplay::displayNextTurf()
{
   fTurfRateEntry++;
   int retVal=getTurfEntry();
   if(retVal==0) 
      refreshTurfDisplay();  
   else fTurfRateEntry--;
   return retVal;  
}


int MagicDisplay::displayPreviousTurf()
{
   if(fTurfRateEntry>0)
      fTurfRateEntry--;
   else 
      return -1;
   int retVal=getTurfEntry();
   if(retVal==0) 
      refreshTurfDisplay();   
   return retVal;     
}

void MagicDisplay::refreshTurfDisplay()
{
   if(!fTurfCanvas) {
      fTurfCanvas = new TCanvas("canMagicTurf","canMagicTurf",800,800);
      fTurfCanvas->cd();
      drawTurfButtons();
   }
   if(!fTurfMainPad) {
      fTurfCanvas->cd();
      fTurfMainPad= new TPad("canMagicTurfMain","canMagicTurfMain",0,0,1,0.9);
      fTurfMainPad->Draw();
      fTurfCanvas->Update();
   }
   if(!fTurfInfoPad) {
      fTurfCanvas->cd();
      fTurfInfoPad= new TPad("canMagicTurfInfo","canMagicTurfInfo",0.2,0.91,0.8,0.99);
      fTurfInfoPad->Draw();
      fTurfCanvas->Update();
   }  
  //This will need to change
   fRFCanMaker->getTurfInfoCanvas(fTurfPtr,fTurfInfoPad);
   fRFCanMaker->getTurfRateCanvas(fTurfPtr,fTurfMainPad);

}

void MagicDisplay::drawTurfButtons()
{
   TButton *butNext = new TButton("Next Turf","MagicDisplay::Instance()->displayNextTurf();",0.85,0.95,1,1);
   butNext->SetTextSize(0.5);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   TButton *butPrev = new TButton("Prev. Turf","MagicDisplay::Instance()->displayPreviousTurf();",0.85,0.90,1,0.95);
   butPrev->SetTextSize(0.5);
   butPrev->SetFillColor(kBlue-10);
   butPrev->Draw();

}


void MagicDisplay::startSurfDisplay()
{
   if(!fRFCanMaker)
      fRFCanMaker=AnitaRFCanvasMaker::Instance();
   int retVal=getSurfEntry();
   if(retVal==0)
      refreshSurfDisplay();   
}

int MagicDisplay::getSurfEntry() 
{
   //   std::cerr << 
   if(fSurfHkEntry<fSurfHkTree->GetEntries())
      fSurfHkTree->GetEntry(fSurfHkEntry);
   else {
      std::cout << "No more entries in surf rate tree" << endl;
      return -1;
   }
   //   std::cout << fSurfHkEntry << "\t" << fSurfPtr->realTime 
   //	     << "\t" << fSurfPtr->ppsNum << std::endl;

   return 0;
}

int MagicDisplay::displayNextSurf()
{
   fSurfHkEntry++;
   int retVal=getSurfEntry();
   if(retVal==0) 
      refreshSurfDisplay();  
   else fSurfHkEntry--;
   return retVal;  
}


int MagicDisplay::displayPreviousSurf()
{
   if(fSurfHkEntry>0)
      fSurfHkEntry--;
   else 
      return -1;
   int retVal=getSurfEntry();
   if(retVal==0) 
      refreshSurfDisplay();   
   return retVal;     
}

void MagicDisplay::refreshSurfDisplay()
{
   if(!fSurfCanvas) {
      fSurfCanvas = new TCanvas("canMagicSurf","canMagicSurf",800,800);
      fSurfCanvas->cd();
      drawSurfButtons();
   }
   if(!fSurfMainPad) {
      fSurfCanvas->cd();
      fSurfMainPad= new TPad("canMagicSurfMain","canMagicSurfMain",0,0,1,0.9);
      fSurfMainPad->Draw();
      fSurfCanvas->Update();
   }
   if(!fSurfInfoPad) {
      fSurfCanvas->cd();
      fSurfInfoPad= new TPad("canMagicSurfInfo","canMagicSurfInfo",0.2,0.91,0.8,0.99);
      fSurfInfoPad->Draw();
      fSurfCanvas->Update();
   }  
  //This will need to change
   fRFCanMaker->getSurfInfoCanvas(fSurfPtr,fSurfInfoPad);
   fRFCanMaker->getSurfHkCanvas(fSurfPtr,fSurfMainPad);

}

void MagicDisplay::drawSurfButtons()
{
   TButton *butNext = new TButton("Next SurfHk","MagicDisplay::Instance()->displayNextSurf();",0.85,0.95,1,1);
   butNext->SetTextSize(0.5);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   TButton *butPrev = new TButton("Prev. SurfHk","MagicDisplay::Instance()->displayPreviousSurf();",0.85,0.90,1,0.95);
   butPrev->SetTextSize(0.5);
   butPrev->SetFillColor(kBlue-10);
   butPrev->Draw();

}
