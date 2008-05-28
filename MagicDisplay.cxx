//////////////////////////////////////////////////////////////////////////////
/////  MagicDisplay.cxx        ANITA Event Canvas maker                  /////
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
#include "TButton.h"
#include "TGroupButton.h"

using namespace std;

MagicDisplay*  MagicDisplay::fgInstance = 0;
AnitaCanvasMaker *fCanMaker=0;
//Leave these as global variables for now

MagicDisplay::MagicDisplay()
{
  //Default constructor
   fRawEventPtr=0;
   fHeadPtr=0;
   fHkPtr=0;
   fUsefulEventPtr=0;
   fgInstance=this;
   fMainOption=MagicDisplayOption::kWavePhiVerticalOnly;
  
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
   fgInstance=this;
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
  int retVal=getEntry();
  if(retVal==0)
     refreshDisplay();   
}

int MagicDisplay::getEntry()
{

   if(fEntry<fEventTree->GetEntries())
      fEventTree->GetEntry(fEntry);
   else {
      std::cout << "No more entries in event tree" << endl;
      return -1;
   }
            
   if(fEntry<fHeadTree->GetEntries())
      fHeadTree->GetEntry(fEntry);
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

void MagicDisplay::refreshDisplay()
{
   if(!fMagicCanvas) {
      fMagicCanvas = new TCanvas("canMagic","canMagic",1200,800);
      fMagicCanvas->cd();
      drawButtons();
   }
   if(!fMagicMainPad) {
      fMagicCanvas->cd();
      fMagicMainPad= new TPad("canMagicMain","canMagicMain",0,0,1,0.9);
      fMagicMainPad->Draw();
      fMagicCanvas->Update();
   }
   //   fMagicMainPad->Clear();

           
  //This will need to change
   switch(fMainOption) {
   case MagicDisplayOption::kWavePhiVerticalOnly:
      fCanMaker->getVerticalCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      break;
   case MagicDisplayOption::kWavePhiHorizontalOnly:
      fCanMaker->getHorizontalCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      break;
   case MagicDisplayOption::kWavePhiCombined:
      fCanMaker->getCombinedCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      //      fCanMaker->setupPhiPadWithFrames(fMagicMainPad);
      break;
   default:
      fCanMaker->getVerticalCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      break;
   }

  fMagicCanvas->Update();
}

int MagicDisplay::displayNext()
{
   fEntry++;
   int retVal=getEntry();
   if(retVal==0) 
      refreshDisplay();   
   return retVal;  
}


int MagicDisplay::displayPrevious()
{
   if(fEntry>0)
      fEntry--;
   else 
      return -1;
   int retVal=getEntry();
   if(retVal==0) 
      refreshDisplay();   
   return retVal;  
}


void MagicDisplay::drawButtons() {
   TButton *butNext = new TButton("Next Event","MagicDisplay::Instance()->displayNext();",0.9,0.95,1,1);
   butNext->SetTextSize(0.5);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   TButton *butPrev = new TButton("Prev. Event","MagicDisplay::Instance()->displayPrevious();",0.9,0.90,1,0.95);
   butPrev->SetTextSize(0.5);
   butPrev->SetFillColor(kBlue-10);
   butPrev->Draw();

   fVertButton = new TButton("Vertical","MagicDisplay::Instance()->setMainCanvasOption(MagicDisplayOption::kWavePhiVerticalOnly); MagicDisplay::Instance()->refreshDisplay();",0,0.97,0.05,1);
   fVertButton->SetTextSize(0.4);
   fVertButton->SetFillColor(kGray+3);
   fVertButton->Draw();
   fHorizButton = new TButton("Horizontal","MagicDisplay::Instance()->setMainCanvasOption(MagicDisplayOption::kWavePhiHorizontalOnly); MagicDisplay::Instance()->refreshDisplay();",0,0.94,0.05,0.97);
   fHorizButton->SetTextSize(0.4);
   fHorizButton->SetFillColor(kGray);
   fHorizButton->Draw();
   fBothButton = new TButton("Both","MagicDisplay::Instance()->setMainCanvasOption(MagicDisplayOption::kWavePhiCombined); MagicDisplay::Instance()->refreshDisplay();",0,0.91,0.05,0.94);
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
