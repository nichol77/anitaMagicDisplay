//////////////////////////////////////////////////////////////////////////////
/////  MagicDisplay.cxx       Magic Display                              /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The main Magic Display class displaying data for ANITA-II      /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
/*! \mainpage Magic Display
 *
 * \section intro_sec Introduction
 *
 * Here is some Doxygen generated documentation for the Marvellous ANITA Graphical Interface and Classy Display (Magic Display).
 *
 * \section prereq_sec Prerequisites
 *
 * -# <A HREF="http://root.cern.ch">ROOT</A>
 * -# <A HREF="http://www.fftw.org/">FFTW 3 -- Fastest Fourier Transform in the West</a>
 * -# <A HREF="http://www.hep.ucl.ac.uk/uhen/anita/libRootFftwWrapper">libRootFftwWrapper -- a ROOT wrapper for FFTW 3</a>
 * -# <A HREF="http://www.hep.ucl.ac.uk/uhen/anita/eventReader">ANITA-II Event Reader</a>
 * 
 * \section install_sec Installation
 * -# Ensure all the prerequisites are installed and in LD_LIBRARY_PATH or system paths.
 * -# Checkout the code from the SVN repository, eg.: <BR><PRE>svn co https://delos.mps.ohio-state.edu/anitaGround/magicDisplay/trunk mySillyNamedMagicDisplayDir</PRE>
 * -# Define the ANITA_UTIL_INSTALL_DIR to point to the location you want the library installed (the library files will end up in (ANITA_UTIL_INSTALL_DIR)/lib and the header files in (ANITA_UTIL_INSTALL_DIR)/include).
 * -# Do <PRE>make</PRE><PRE>make install</PRE>
 * \section manual_sec Manual
 * If you are averse to reading web pages (and who wouldn't be) you can download a <a href="manual/magicDisplay.pdf">pdf copy of the reference material</a> but be warned it won't be a thrilling read as it was written by a computer program.
 */



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
  fInEventPlayMode=0;
  fEventPlaySleepMs=0;
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
   fHeadFile=0;
   fEventFile=0;
   fTurfRateFile=0;
   fSumTurfRateFile=0;
   fSurfHkFile=0;
   fAvgSurfHkFile=0;
   fCalType=WaveCalType::kVoltageTime;
}

MagicDisplay::~MagicDisplay()
{
   //Default destructor
}


MagicDisplay::MagicDisplay(char *baseDir, int run, WaveCalType::WaveCalType_t calType)
{
  //Offline constructor
   fHeadFile=0;
   fEventFile=0;
   fTurfRateFile=0;
   fSumTurfRateFile=0;
   fSurfHkFile=0;
   fAvgSurfHkFile=0;
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
   fCalType=calType;




  //  char hkName[FILENAME_MAX];

  //  sprintf(hkName,"/unix/anita2/testing/rootFiles/run%d/prettyHkFile%d.root",run,run);
  //  TFile *fpHk = TFile::Open(hkName);
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
  fEventCanMaker=new AnitaCanvasMaker(fCalType);
  int retVal=getEventEntry();
  if(retVal==0)
      refreshEventDisplay();   
}

int MagicDisplay::getEventEntry()
{

  if(!fEventTree) {
    if(loadEventTree()<0) {
      std::cout << "Couldn't open event file\n";
      return -1;
    }
  }
  if(fEventEntry<fEventTree->GetEntries())
    fEventTree->GetEntry(fEventEntry);
  else {
    std::cout << "No more entries in event tree " << fEventEntry << "\t" << fEventTree->GetEntries() << endl;
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
   fUsefulEventPtr = new UsefulAnitaEvent(fRawEventPtr,fCalType);  
  //Need to make configurable at some point
  //This will also need to be modifed to make realEvent accessible outside here
   return 0;
}


void MagicDisplay::closeCurrentRun()
{
  
  if(fHeadFile)
    fHeadFile->Close();
  if(fEventFile)
    fEventFile->Close();
  if(fTurfRateFile)
    fTurfRateFile->Close();
  if(fSumTurfRateFile)
    fSumTurfRateFile->Close();
  if(fSurfHkFile)
    fSurfHkFile->Close();
  if(fAvgSurfHkFile)
    fAvgSurfHkFile->Close();

  fHeadFile=0;
  fEventFile=0;
  fTurfRateFile=0;
  fSumTurfRateFile=0;
  fSurfHkFile=0;
  fAvgSurfHkFile=0;

  fHeadTree=0;
  fEventTree=0;
  fTurfRateTree=0;
  fSumTurfRateTree=0;
  fSurfHkTree=0;
  fAvgSurfHkTree=0;
}


int MagicDisplay::loadEventTree()
{       
  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  sprintf(eventName,"%s/run%d/eventFile%d.root",fCurrentBaseDir,fCurrentRun,fCurrentRun);
  sprintf(headerName,"%s/run%d/headFile%d.root",fCurrentBaseDir,fCurrentRun,fCurrentRun);
  fEventTree = new TChain("eventTree");
  fEventTree->Add(eventName);

  for(int extra=1;extra<100;extra++) {
    sprintf(eventName,"%s/run%d/eventFile%d_%d.root",fCurrentBaseDir,fCurrentRun,fCurrentRun,extra);
    TFile *fpTest = TFile::Open(eventName);
    if(!fpTest) 
      break;
    else {
      delete fpTest;
      fEventTree->Add(eventName);
    }
  }


  fEventTree->SetBranchAddress("event",&fRawEventPtr);
      
  if(fEventTree->GetEntries()<1) {
    cout << "Couldn't open: " << eventName << "\n";
    return -1;
  }

  fHeadFile = TFile::Open(headerName);
  if(!fHeadFile) {
    cout << "Couldn't open: " << headerName << "\n";
    return -1;
  }
  fHeadTree = (TTree*) fHeadFile->Get("headTree");
  if(!fHeadTree) {
    cout << "Couldn't get headTree from " << headerName << endl;
    return -1;
  }
  fHeadTree->SetBranchAddress("header",&fHeadPtr);
  fEventEntry=0;
  fHeadTree->BuildIndex("eventNumber");
  //      fHeadIndex = (TTreeIndex*) fHeadTree->GetTreeIndex();
  std::cerr << fEventTree << "\t" << fHeadTree << "\n";
  std::cerr << fHeadTree->GetEntries() << "\t"
	    << fEventTree->GetEntries() << "\n";
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
  
   fEventCanMaker->getEventInfoCanvas(fUsefulEventPtr,fHeadPtr,fMagicEventInfoPad);
   switch(fMainOption) {
   case MagicDisplayOption::kWavePhiVerticalOnly:
     fEventCanMaker->fPolView=0;
      fEventCanMaker->getEventViewerCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      break;
   case MagicDisplayOption::kWavePhiHorizontalOnly:
     fEventCanMaker->fPolView=1;
      fEventCanMaker->getEventViewerCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      break;
   case MagicDisplayOption::kWavePhiCombined:
     fEventCanMaker->fPolView=2;
      fEventCanMaker->getEventViewerCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      //      fEventCanMaker->setupPhiPadWithFrames(fMagicMainPad);
      break;
   case MagicDisplayOption::kWaveSurfOnly:
     fEventCanMaker->fPolView=3;
     fEventCanMaker->getEventViewerCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
     break;
   default:
     fEventCanMaker->getEventViewerCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
     //fEventCanMaker->getVerticalCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
      break;
   }

  fMagicCanvas->Update();
}

int MagicDisplay::displayNextEvent()
{
   fEventEntry++;
   int retVal=getEventEntry();
     fEventCanMaker->fNewEvent=1;
   if(retVal==0) {
      refreshEventDisplay(); 
   }
   else fEventEntry--;
   return retVal;  
}


int MagicDisplay::displayFirstEvent()
{
  fEventEntry=0;
   int retVal=getEventEntry();
     fEventCanMaker->fNewEvent=1;
   if(retVal==0) {
      refreshEventDisplay(); 
   }
   else fEventEntry--;
   return retVal;  
}


int MagicDisplay::displayLastEvent()
{
  //  fEventTree->Refresh();
  //  fHeadTree->Refresh();
  

  Long64_t headEnts=fHeadTree->GetEntries();
  Long64_t eventEnts=fEventTree->GetEntries();

  fEventEntry=headEnts-1;
  if(eventEnts<headEnts)
    fEventEntry=eventEnts-1;
   int retVal=getEventEntry();
     fEventCanMaker->fNewEvent=1;
   if(retVal==0) {
      refreshEventDisplay(); 
   }
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
     fEventCanMaker->fNewEvent=1;
   if(retVal==0) {
      refreshEventDisplay(); 
   }  
   return retVal;  
}


int MagicDisplay::displayThisEvent(UInt_t eventNumber, UInt_t runNumber)
{
  cout << "displayThisEvent: " << eventNumber << "\t" << runNumber <<endl;  
  
  if(fCurrentRun != runNumber) {
    cout << "\t" << fCurrentRun << "\t" << runNumber << endl;
    closeCurrentRun();
    fCurrentRun=runNumber;
  }

  if(!fEventCanMaker) startEventDisplay();
  

  if(eventNumber==0) {
    fEventEntry=0;
  }
  else {
    fEventEntry=fHeadTree->GetEntryNumberWithIndex(eventNumber);
    if(fEventEntry<0) 
      return -1;      
  }
  cout << "fEventEntry: " << fEventEntry << endl;
  int retVal=getEventEntry();
  if(retVal==0) 
    refreshEventDisplay(); 
  else {
    cout << "retVal: " << retVal << endl;
  }
  return retVal;  
}

void MagicDisplay::drawEventButtons() {
   TButton *butNext = new TButton("Next ","MagicDisplay::Instance()->displayNextEvent();",0.95,0.975,1,1);
   butNext->SetTextSize(0.5);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   TButton *butPrev = new TButton("Prev.","MagicDisplay::Instance()->displayPreviousEvent();",0.95,0.95,1,0.975);
   butPrev->SetTextSize(0.5);
   butPrev->SetFillColor(kBlue-10);
   butPrev->Draw();
   TButton *butFirst = new TButton("First ","MagicDisplay::Instance()->displayFirstEvent();",0.95,0.925,1,0.95);
   butFirst->SetTextSize(0.5);
   butFirst->SetFillColor(kOrange+10);
   butFirst->Draw();
   TButton *butLast = new TButton("Last.","MagicDisplay::Instance()->displayLastEvent();",0.95,0.90,1,0.925);
   butLast->SetTextSize(0.5);
   butLast->SetFillColor(kViolet-10);
   butLast->Draw();
   

   TButton *butPlay = new TButton("Play","MagicDisplay::Instance()->startEventPlaying();",0.9,0.97,0.95,1);
   butPlay->SetTextSize(0.5);
   butPlay->SetFillColor(kGreen-10);
   butPlay->Draw();
   TButton *butPlayRev = new TButton("Rev ","MagicDisplay::Instance()->startEventPlayingReverse();",0.9,0.94,0.95,0.97);
   butPlayRev->SetTextSize(0.5);
   butPlayRev->SetFillColor(kBlue-10);
   butPlayRev->Draw();
   TButton *butStop = new TButton("Stop","MagicDisplay::Instance()->stopEventPlaying();",0.90,0.90,0.95,0.94);
   butStop->SetTextSize(0.5);
   butStop->SetFillColor(kRed-10);
   butStop->Draw();

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

   //NEW BUTTONS
   fWaveformButton = new TButton("Waveform View","MagicDisplay::Instance()->toggleWaveformView(1); MagicDisplay::Instance()->refreshEventDisplay();",0.05,0.95,0.14,1);
   fWaveformButton->SetTextSize(0.4);
   fWaveformButton->SetFillColor(kGray+3);
   fWaveformButton->Draw();
   fPowerButton = new TButton("FFT View","MagicDisplay::Instance()->toggleWaveformView(0); MagicDisplay::Instance()->refreshEventDisplay();",0.05,0.9,0.14,0.95);
   fPowerButton->SetTextSize(0.4);
   fPowerButton->SetFillColor(kGray);
   fPowerButton->Draw();

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


void MagicDisplay::toggleWaveformView(Int_t surfView)
{
   
   if(surfView) {
      //Turn on fft view off
      fEventCanMaker->fPowerSpecView=0;
      fWaveformButton->SetFillColor(kGray+3);
      fPowerButton->SetFillColor(kGray);
      fWaveformButton->Modified();
      fPowerButton->Modified();
   }
   else {
      //Turn fft view on
      fEventCanMaker->fPowerSpecView=1;
      fWaveformButton->SetFillColor(kGray);
      fPowerButton->SetFillColor(kGray+3);
      fWaveformButton->Modified();
      fPowerButton->Modified();
   }
      
}

int MagicDisplay::loadTurfTree() 
{
  char turfName[FILENAME_MAX];
  sprintf(turfName,"%s/run%d/turfRateFile%d.root",fCurrentBaseDir,
	  fCurrentRun,fCurrentRun);
  fTurfRateFile = TFile::Open(turfName);
  if(!fTurfRateFile) {
    cout << "Couldn't open: " << turfName << "\n";
    return -1;
  }
  fTurfRateTree = (TTree*) fTurfRateFile->Get("turfRateTree");
  if(!fTurfRateTree) {
    cout << "Couldn't get turfRateTree from " << turfName << endl;
    return -1;
  }
  if(fTurfRateTree->GetEntries()>0) {
    fTurfRateTree->SetBranchAddress("turf",&fTurfPtr);
    fTurfRateEntry=0;
  }
  else {
    delete fTurfRateTree;
    fTurfRateFile->Close();
    fTurfRateFile=0;
    fTurfRateTree=0;
  }
  return 0;
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
  if(!fTurfRateTree) {
    if(loadTurfTree()<0) 
      return -1;
  }
  if(!fTurfRateTree) {
    std::cout << "No Turf Rates to play with\n";
    return -1;
  }
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

int MagicDisplay::loadSurfTree()
{
 char surfName[FILENAME_MAX];
 sprintf(surfName,"%s/run%d/surfHkFile%d.root",fCurrentBaseDir,
	 fCurrentRun,fCurrentRun);
 fSurfHkFile = TFile::Open(surfName);
 if(!fSurfHkFile) {
   cout << "Couldn't open: " << surfName << "\n";
   return -1;
 }
 fSurfHkTree = (TTree*) fSurfHkFile->Get("surfHkTree");
 if(!fSurfHkTree) {
   cout << "Couldn't get surfHkTree from " << surfName << endl;
   return -1;
 }
 fSurfHkTree->SetBranchAddress("surf",&fSurfPtr);
 fSurfHkEntry=0;
 return 0;
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
  if(!fSurfHkTree) {
    if(loadSurfTree()<0) {
      cout << "Couldn't open SURF hk tree\n";
      return -1;
    }
  }
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



int MagicDisplay::loadAvgSurfTree()
{
  char surfName[FILENAME_MAX];
  sprintf(surfName,"%s/run%d/avgSurfHkFile%d.root",fCurrentBaseDir,
	  fCurrentRun,fCurrentRun);
  fSurfHkFile = TFile::Open(surfName);
  if(!fSurfHkFile) {
    cout << "Couldn't open: " << surfName << "\n";
    return -1;
  }
  fAvgSurfHkTree = (TTree*) fSurfHkFile->Get("avgSurfHkTree");
  if(!fAvgSurfHkTree) {
    cout << "Couldn't get avgSurfHkTree from " << surfName << endl;
    return -1;
  }
  fAvgSurfHkTree->SetBranchAddress("avgsurf",&fAvgSurfPtr);
  fAvgSurfHkEntry=0;
  return 0;
}

void MagicDisplay::startAvgSurfDisplay()
{
  
   if(!fRFCanMaker)
      fRFCanMaker=AnitaRFCanvasMaker::Instance();
   int retVal=getAvgSurfEntry();
   if(retVal==0)
      refreshAvgSurfDisplay();   
}

int MagicDisplay::getAvgSurfEntry() 
{
  if(!fAvgSurfHkTree) {
    int retVal=loadAvgSurfTree();
    if(retVal<0) {
      std::cout << "Couldn't load average surf tree\n";
      return retVal;
    }
  }
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

int MagicDisplay::loadSumTurfTree()
{

  char sumTurfName[FILENAME_MAX];
  sprintf(sumTurfName,"%s/run%d/sumTurfRateFile%d.root",fCurrentBaseDir,
	  fCurrentRun,fCurrentRun);
  fSumTurfRateFile = TFile::Open(sumTurfName);
  if(!fSumTurfRateFile) {
    cout << "Couldn't open: " << sumTurfName << "\n";
    return -1;
  }
  fSumTurfRateTree = (TTree*) fSumTurfRateFile->Get("sumTurfRateTree");
  if(!fSumTurfRateTree) {
    cout << "Couldn't get sumTurfRateTree from " << sumTurfName << endl;
    return -1;
  }
  fSumTurfRateTree->SetBranchAddress("sumturf",&fSumTurfPtr);
  fSumTurfRateEntry=0;
  return 0;
}


void MagicDisplay::startSumTurfDisplay()
{
 
   

   fRFCanMaker=AnitaRFCanvasMaker::Instance();
   int retVal=getSumTurfEntry();
   if(retVal==0)
      refreshSumTurfDisplay();   
}

int MagicDisplay::getSumTurfEntry() 
{
  if(!fSumTurfRateTree) {
    if(loadSumTurfTree()<0)
      return -1;
  }
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

void MagicDisplay::startEventPlaying()
{
  fInEventPlayMode=1;
  do {
    gSystem->ProcessEvents();
    if(!fInEventPlayMode) break;
    if(fEventPlaySleepMs>0)
      gSystem->Sleep(fEventPlaySleepMs);
  }
  while(this->displayNextEvent()==0);
  if(fInEventPlayMode) {
    Long64_t headEntries=fHeadTree->GetEntries();
    //    fHeadTree->Refresh();
    //fEventTree->Refresh();
    if(fHeadTree->GetEntries()!=headEntries) {
      std::cout << headEntries << "\t" << fHeadTree->GetEntries() << "\n";
      startEventPlaying();
    }
  }
  
  
}


void MagicDisplay::startEventPlayingReverse()
{
  fInEventPlayMode=1;
  do {
    gSystem->ProcessEvents();
    if(!fInEventPlayMode) break;
    if(fEventPlaySleepMs>0)
      gSystem->Sleep(fEventPlaySleepMs);
  }
  while(this->displayPreviousEvent()==0);
}

void MagicDisplay::stopEventPlaying() 
{
  fInEventPlayMode=0;
}


/*
void MagicDisplay::startSummaryDisplay()
{
  char headerName[FILENAME_MAX];
   char eventName[FILENAME_MAX];
   sprintf(headerName,"%s/run%d/headFile%d.root",fCurrentBaseDir,fCurrentRun,fCurrentRun);
   sprintf(eventName,"%s/run%d/eventFile%d*.root",fCurrentBaseDir,fCurrentRun,fCurrentRun);

  fEventTree = new TChain("eventTree");
  fEventTree->Add(eventName);
  fEventTree->SetBranchAddress("event",&fSummaryEventPtr);

  TFile *fHeadFile = TFile::Open(headerName);
  if(!fHeadFile) {
     cout << "Couldn't open: " << headerName << "\n";
     return;
  }
  fHeadTree = (TTree*) fHeadFile->Get("headTree");
  if(!fHeadTree) {
     cout << "Couldn't get headTree from " << headerName << endl;
     return;
  }

  fHeadTree->SetBranchAddress("header",&fSummaryHeadPtr);



  }
*/
