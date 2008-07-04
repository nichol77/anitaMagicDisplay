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
#include "MagicControlPanel.h"

//Event Reader Includes
#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "TurfRate.h"
#include "SurfHk.h"
#include "SummedTurfRate.h"
#include "AveragedSurfHk.h"

//ROOT Includes
#include "TROOT.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TButton.h"
#include "TGroupButton.h"
#include <TGClient.h>

using namespace std;

MagicDisplay*  MagicDisplay::fgInstance = 0;
AnitaCanvasMaker *fEventCanMaker=0;
AnitaRFCanvasMaker *fRFCanMaker=0;
MagicControlPanel *fControlPanel=0;
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
   fAvgSurfPtr=0;
   fSumTurfPtr=0;
   fgInstance=this;
   fMainOption=MagicDisplayOption::kWavePhiVerticalOnly;
   fMagicCanvas=0;
   fMagicEventInfoPad=0;
   fMagicMainPad=0;
   fCurrentRun=0;
   fEventTree=0;
   fSurfHkTree=0;
   fTurfRateTree=0;
   fAvgSurfHkTree=0;
   fSumTurfRateTree=0;
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
   fAvgSurfPtr=0;
   fSumTurfPtr=0;
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
   fCurrentRun=run;
   strncpy(fCurrentBaseDir,baseDir,179);
   fEventTree=0;
   fSurfHkTree=0;
   fTurfRateTree=0;
   fAvgSurfHkTree=0;
   fSumTurfRateTree=0;




  //  char hkName[FILENAME_MAX];

  //  sprintf(hkName,"/unix/anita2/testing/rootFiles/run%d/prettyHkFile%d.root",run,run);
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

void MagicDisplay::startControlPanel() 
{
  //  fControlPanel=MagicControlPanel::Instance();
  //  fControlPanel->DrawControlPanel();
  new MagicControlPanel(gClient->GetRoot(), 400, 200);
}


void MagicDisplay::startEventDisplay()
{
  if(!fEventTree) loadEventTree(); 
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
   fUsefulEventPtr = new UsefulAnitaEvent(fRawEventPtr,WaveCalType::kVoltageTime);  
  //Need to make configurable at some point
  //This will also need to be modifed to make realEvent accessible outside here
   return 0;
}

void MagicDisplay::loadEventTree()
{       
  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  sprintf(eventName,"%s/run%d/eventFile%d*.root",fCurrentBaseDir,fCurrentRun,fCurrentRun);
  sprintf(headerName,"%s/run%d/headFile%d.root",fCurrentBaseDir,fCurrentRun,fCurrentRun);
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
  fHeadTree->BuildIndex("eventNumber");
  //      fHeadIndex = (TTreeIndex*) fHeadTree->GetTreeIndex();
  std::cerr << fEventTree << "\t" << fHeadTree << "\n";
  std::cerr << fHeadTree->GetEntries() << "\t"
	    << fEventTree->GetEntries() << "\n";
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
   case MagicDisplayOption::kWaveSurfOnly:
     fEventCanMaker->getSurfChanCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
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


int MagicDisplay::displayThisEvent(UInt_t eventNumber)
{
  if(!fHeadTree) startEventDisplay();
  if(eventNumber==0) {
    fEventEntry=0;
  }
  else {
    fEventEntry=fHeadTree->GetEntryWithIndex(eventNumber);
    if(fEventEntry<0) 
      return -1;      
  }
  int retVal=getEventEntry();
  if(retVal==0) 
    refreshEventDisplay(); 
  else fEventEntry--;
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

   fVertButton = new TButton("Vertical","MagicDisplay::Instance()->setMainCanvasOption(MagicDisplayOption::kWavePhiVerticalOnly); MagicDisplay::Instance()->refreshEventDisplay();",0,0.975,0.05,1);
   fVertButton->SetTextSize(0.4);
   fVertButton->SetFillColor(kGray+3);
   fVertButton->Draw();
   fHorizButton = new TButton("Horizontal","MagicDisplay::Instance()->setMainCanvasOption(MagicDisplayOption::kWavePhiHorizontalOnly); MagicDisplay::Instance()->refreshEventDisplay();",0,0.95,0.05,0.975);
   fHorizButton->SetTextSize(0.4);
   fHorizButton->SetFillColor(kGray);
   fHorizButton->Draw();
   fBothButton = new TButton("Both","MagicDisplay::Instance()->setMainCanvasOption(MagicDisplayOption::kWavePhiCombined); MagicDisplay::Instance()->refreshEventDisplay();",0,0.925,0.05,0.95);
   fBothButton->SetTextSize(0.4);
   fBothButton->SetFillColor(kGray);
   fBothButton->Draw();
   fSurfButton = new TButton("Surf","MagicDisplay::Instance()->setMainCanvasOption(MagicDisplayOption::kWaveSurfOnly); MagicDisplay::Instance()->refreshEventDisplay();",0,0.9,0.05,0.925);
   fSurfButton->SetTextSize(0.4);
   fSurfButton->SetFillColor(kGray);
   fSurfButton->Draw();
   
}

void MagicDisplay::setMainCanvasOption(MagicDisplayOption::MagicDisplayOption_t option)
{
   fMainOption=option;
   switch(option) {
   case MagicDisplayOption::kWaveSurfOnly:
      fVertButton->SetFillColor(kGray);
      fHorizButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray);
      fSurfButton->SetFillColor(kGray+3);
      break;
   case MagicDisplayOption::kWavePhiVerticalOnly:
   case MagicDisplayOption::kPowerPhiVerticalOnly:
      fVertButton->SetFillColor(kGray+3);
      fHorizButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray);
      fSurfButton->SetFillColor(kGray);
      break;
   case MagicDisplayOption::kWavePhiHorizontalOnly:
   case MagicDisplayOption::kPowerPhiHorizontalOnly:
      fHorizButton->SetFillColor(kGray+3);
      fVertButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray);
      fSurfButton->SetFillColor(kGray);
      break;
   case MagicDisplayOption::kWavePhiCombined:
   case MagicDisplayOption::kPowerPhiCombined:
      fHorizButton->SetFillColor(kGray);
      fVertButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray+3);
      fSurfButton->SetFillColor(kGray);
      break;
   default:
      break;
   }   
   fVertButton->Modified();
   fHorizButton->Modified();  
   fBothButton->Modified(); 
   fSurfButton->Modified();
}



void MagicDisplay::startTurfDisplay()
{
   if(!fTurfRateTree) {
      char turfName[FILENAME_MAX];
      sprintf(turfName,"%s/run%d/turfRateFile%d.root",fCurrentBaseDir,
	      fCurrentRun,fCurrentRun);
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
   }
   

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
   
   fTurfYScaleButton = new TButton("Fix Scale","MagicDisplay::Instance()->toggleTurfYScale(); MagicDisplay::Instance()->refreshTurfDisplay();",0,0.96,0.1,1);
   fTurfYScaleButton->SetTextSize(0.4);
   fTurfYScaleButton->SetFillColor(kGray);
   fTurfYScaleButton->Draw();
}

void MagicDisplay::toggleTurfYScale()
{
   if(fTurfYScaleButton->GetFillColor()==kGray) {
      //Turn on fixed y scale
      fRFCanMaker->fFixTurfYScale=1;
      fTurfYScaleButton->SetFillColor(kGray+3);
      fTurfYScaleButton->Modified();
   }
   else {
      //Turn off fixed y scale
      fRFCanMaker->fFixTurfYScale=0;
      fTurfYScaleButton->SetFillColor(kGray);
      fTurfYScaleButton->Modified();
   }
      
}


void MagicDisplay::startSurfDisplay()
{
   if(!fSurfHkTree) {

      char surfName[FILENAME_MAX];
      sprintf(surfName,"%s/run%d/surfHkFile%d.root",fCurrentBaseDir,
	      fCurrentRun,fCurrentRun);
      TFile *fpSurf = new TFile(surfName);
      if(!fpSurf) {
	 cout << "Couldn't open: " << surfName << "\n";
	 return;
      }
      fSurfHkTree = (TTree*) fpSurf->Get("surfHkTree");
      if(!fSurfHkTree) {
     cout << "Couldn't get surfHkTree from " << surfName << endl;
     return;
      }
      fSurfHkTree->SetBranchAddress("surf",&fSurfPtr);
      fSurfHkEntry=0;
   }


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
      std::cout << "No more entries in surfHkTree" << endl;
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

   fSurfSurfViewButton = new TButton("SURF View","MagicDisplay::Instance()->toggleSurfSurfView(1); MagicDisplay::Instance()->refreshSurfDisplay();",0,0.96,0.1,1);
   fSurfSurfViewButton->SetTextSize(0.4);
   fSurfSurfViewButton->SetFillColor(kGray+3);
   fSurfSurfViewButton->Draw();


   fSurfPhiViewButton = new TButton("PHI View","MagicDisplay::Instance()->toggleSurfSurfView(0); MagicDisplay::Instance()->refreshSurfDisplay();",0,0.92,0.1,0.96);
   fSurfPhiViewButton->SetTextSize(0.4);
   fSurfPhiViewButton->SetFillColor(kGray);
   fSurfPhiViewButton->Draw();

}


void MagicDisplay::toggleSurfSurfView(Int_t surfView)
{
   
   if(surfView) {
      //Turn on phi view off
      fRFCanMaker->fSurfPhiView=0;
      fSurfSurfViewButton->SetFillColor(kGray+3);
      fSurfPhiViewButton->SetFillColor(kGray);
      fSurfSurfViewButton->Modified();
      fSurfPhiViewButton->Modified();
   }
   else {
      //Turn phi view on
      fRFCanMaker->fSurfPhiView=1;
      fSurfSurfViewButton->SetFillColor(kGray);
      fSurfPhiViewButton->SetFillColor(kGray+3);
      fSurfSurfViewButton->Modified();
      fSurfPhiViewButton->Modified();
   }
      
}



void MagicDisplay::startAvgSurfDisplay()
{
   if(!fAvgSurfHkTree) {

      char surfName[FILENAME_MAX];
      sprintf(surfName,"%s/run%d/avgSurfHkFile%d.root",fCurrentBaseDir,
	      fCurrentRun,fCurrentRun);
      TFile *fpAvgSurf = new TFile(surfName);
      if(!fpAvgSurf) {
	 cout << "Couldn't open: " << surfName << "\n";
	 return;
      }
      fAvgSurfHkTree = (TTree*) fpAvgSurf->Get("avgSurfHkTree");
      if(!fAvgSurfHkTree) {
     cout << "Couldn't get avgSurfHkTree from " << surfName << endl;
     return;
      }
      fAvgSurfHkTree->SetBranchAddress("avgsurf",&fAvgSurfPtr);
      fAvgSurfHkEntry=0;
   }


   if(!fRFCanMaker)
      fRFCanMaker=AnitaRFCanvasMaker::Instance();
   int retVal=getAvgSurfEntry();
   if(retVal==0)
      refreshAvgSurfDisplay();   
}

int MagicDisplay::getAvgSurfEntry() 
{
   //   std::cerr << 
   if(fAvgSurfHkEntry<fAvgSurfHkTree->GetEntries())
      fAvgSurfHkTree->GetEntry(fAvgSurfHkEntry);
   else {
      std::cout << "No more entries in avgSurfHkTree" << endl;
      return -1;
   }
   //   std::cout << fAvgSurfHkEntry << "\t" << fAvgSurfPtr->realTime 
   //	     << "\t" << fAvgSurfPtr->ppsNum << std::endl;

   return 0;
}

int MagicDisplay::displayNextAvgSurf()
{
   fAvgSurfHkEntry++;
   int retVal=getAvgSurfEntry();
   if(retVal==0) 
      refreshAvgSurfDisplay();  
   else fAvgSurfHkEntry--;
   return retVal;  
}


int MagicDisplay::displayPreviousAvgSurf()
{
   if(fAvgSurfHkEntry>0)
      fAvgSurfHkEntry--;
   else 
      return -1;
   int retVal=getAvgSurfEntry();
   if(retVal==0) 
      refreshAvgSurfDisplay();   
   return retVal;     
}

void MagicDisplay::refreshAvgSurfDisplay()
{
   if(!fAvgSurfCanvas) {
      fAvgSurfCanvas = new TCanvas("canMagicAvgSurf","canMagicAvgSurf",800,800);
      fAvgSurfCanvas->cd();
      drawAvgSurfButtons();
   }
   if(!fAvgSurfMainPad) {
      fAvgSurfCanvas->cd();
      fAvgSurfMainPad= new TPad("canMagicAvgSurfMain","canMagicAvgSurfMain",0,0,1,0.9);
      fAvgSurfMainPad->Draw();
      fAvgSurfCanvas->Update();
   }
   if(!fAvgSurfInfoPad) {
      fAvgSurfCanvas->cd();
      fAvgSurfInfoPad= new TPad("canMagicAvgSurfInfo","canMagicAvgSurfInfo",0.2,0.91,0.8,0.99);
      fAvgSurfInfoPad->Draw();
      fAvgSurfCanvas->Update();
   }  
  //This will need to change
   fRFCanMaker->getAvgSurfInfoCanvas(fAvgSurfPtr,fAvgSurfInfoPad);
   fRFCanMaker->getAvgSurfHkCanvas(fAvgSurfPtr,fAvgSurfMainPad);

}

void MagicDisplay::drawAvgSurfButtons()
{
   TButton *butNext = new TButton("Next AvgSurfHk","MagicDisplay::Instance()->displayNextAvgSurf();",0.85,0.95,1,1);
   butNext->SetTextSize(0.4);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   TButton *butPrev = new TButton("Prev. AvgSurfHk","MagicDisplay::Instance()->displayPreviousAvgSurf();",0.85,0.90,1,0.95);
   butPrev->SetTextSize(0.4);
   butPrev->SetFillColor(kBlue-10);
   butPrev->Draw();

   fAvgSurfSurfViewButton = new TButton("SURF View","MagicDisplay::Instance()->toggleAvgSurfSurfView(1); MagicDisplay::Instance()->refreshAvgSurfDisplay();",0,0.96,0.1,1);
   fAvgSurfSurfViewButton->SetTextSize(0.4);
   fAvgSurfSurfViewButton->SetFillColor(kGray+3);
   fAvgSurfSurfViewButton->Draw();


   fAvgSurfPhiViewButton = new TButton("PHI View","MagicDisplay::Instance()->toggleAvgSurfSurfView(0); MagicDisplay::Instance()->refreshAvgSurfDisplay();",0,0.92,0.1,0.96);
   fAvgSurfPhiViewButton->SetTextSize(0.4);
   fAvgSurfPhiViewButton->SetFillColor(kGray);
   fAvgSurfPhiViewButton->Draw();

}


void MagicDisplay::toggleAvgSurfSurfView(Int_t surfView)
{
   
   if(surfView) {
      //Turn on phi view off
      fRFCanMaker->fAvgSurfPhiView=0;
      fAvgSurfSurfViewButton->SetFillColor(kGray+3);
      fAvgSurfPhiViewButton->SetFillColor(kGray);
      fAvgSurfSurfViewButton->Modified();
      fAvgSurfPhiViewButton->Modified();
   }
   else {
      //Turn phi view on
      fRFCanMaker->fAvgSurfPhiView=1;
      fAvgSurfSurfViewButton->SetFillColor(kGray);
      fAvgSurfPhiViewButton->SetFillColor(kGray+3);
      fAvgSurfSurfViewButton->Modified();
      fAvgSurfPhiViewButton->Modified();
   }
      
}



void MagicDisplay::startSumTurfDisplay()
{
   if(!fSumTurfRateTree) {
      char sumTurfName[FILENAME_MAX];
      sprintf(sumTurfName,"%s/run%d/sumTurfRateFile%d.root",fCurrentBaseDir,
	      fCurrentRun,fCurrentRun);
      TFile *fpSumTurf = new TFile(sumTurfName);
      if(!fpSumTurf) {
	 cout << "Couldn't open: " << sumTurfName << "\n";
	 return;
      }
      fSumTurfRateTree = (TTree*) fpSumTurf->Get("sumTurfRateTree");
      if(!fSumTurfRateTree) {
	 cout << "Couldn't get sumTurfRateTree from " << sumTurfName << endl;
	 return;
      }
      fSumTurfRateTree->SetBranchAddress("sumturf",&fSumTurfPtr);
      fSumTurfRateEntry=0;
   }
   

   fRFCanMaker=AnitaRFCanvasMaker::Instance();
   int retVal=getSumTurfEntry();
   if(retVal==0)
      refreshSumTurfDisplay();   
}

int MagicDisplay::getSumTurfEntry() 
{
   if(fSumTurfRateEntry<fSumTurfRateTree->GetEntries())
      fSumTurfRateTree->GetEntry(fSumTurfRateEntry);
   else {
      std::cout << "No more entries in sumTurf rate tree" << endl;
      return -1;
   }
   //   std::cout << fSumTurfRateEntry << "\t" << fSumTurfPtr->realTime 
   //	     << "\t" << fSumTurfPtr->ppsNum << std::endl;

   return 0;
}

int MagicDisplay::displayNextSumTurf()
{
   fSumTurfRateEntry++;
   int retVal=getSumTurfEntry();
   if(retVal==0) 
      refreshSumTurfDisplay();  
   else fSumTurfRateEntry--;
   return retVal;  
}


int MagicDisplay::displayPreviousSumTurf()
{
   if(fSumTurfRateEntry>0)
      fSumTurfRateEntry--;
   else 
      return -1;
   int retVal=getSumTurfEntry();
   if(retVal==0) 
      refreshSumTurfDisplay();   
   return retVal;     
}

void MagicDisplay::refreshSumTurfDisplay()
{
   if(!fSumTurfCanvas) {
      fSumTurfCanvas = new TCanvas("canMagicSumTurf","canMagicSumTurf",800,800);
      fSumTurfCanvas->cd();
      drawSumTurfButtons();
   }
   if(!fSumTurfMainPad) {
      fSumTurfCanvas->cd();
      fSumTurfMainPad= new TPad("canMagicSumTurfMain","canMagicSumTurfMain",0,0,1,0.9);
      fSumTurfMainPad->Draw();
      fSumTurfCanvas->Update();
   }
   if(!fSumTurfInfoPad) {
      fSumTurfCanvas->cd();
      fSumTurfInfoPad= new TPad("canMagicSumTurfInfo","canMagicSumTurfInfo",0.2,0.91,0.8,0.99);
      fSumTurfInfoPad->Draw();
      fSumTurfCanvas->Update();
   }  
  //This will need to change
   fRFCanMaker->getSumTurfInfoCanvas(fSumTurfPtr,fSumTurfInfoPad);
   fRFCanMaker->getSumTurfRateCanvas(fSumTurfPtr,fSumTurfMainPad);

}

void MagicDisplay::drawSumTurfButtons()
{
   TButton *butNext = new TButton("Next SumTurf","MagicDisplay::Instance()->displayNextSumTurf();",0.85,0.95,1,1);
   butNext->SetTextSize(0.5);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   TButton *butPrev = new TButton("Prev. SumTurf","MagicDisplay::Instance()->displayPreviousSumTurf();",0.85,0.90,1,0.95);
   butPrev->SetTextSize(0.5);
   butPrev->SetFillColor(kBlue-10);
   butPrev->Draw();
   
   fSumTurfYScaleButton = new TButton("Fix Scale","MagicDisplay::Instance()->toggleSumTurfYScale(); MagicDisplay::Instance()->refreshSumTurfDisplay();",0,0.96,0.1,1);
   fSumTurfYScaleButton->SetTextSize(0.4);
   fSumTurfYScaleButton->SetFillColor(kGray);
   fSumTurfYScaleButton->Draw();
}

void MagicDisplay::toggleSumTurfYScale()
{
   if(fSumTurfYScaleButton->GetFillColor()==kGray) {
      //Turn on fixed y scale
      fRFCanMaker->fFixSumTurfYScale=1;
      fSumTurfYScaleButton->SetFillColor(kGray+3);
      fSumTurfYScaleButton->Modified();
   }
   else {
      //Turn off fixed y scale
      fRFCanMaker->fFixSumTurfYScale=0;
      fSumTurfYScaleButton->SetFillColor(kGray);
      fSumTurfYScaleButton->Modified();
   }
      
}

UInt_t MagicDisplay::getCurrentEvent()
{
  if(fHeadPtr) return fHeadPtr->eventNumber; 
  return 0;
}
