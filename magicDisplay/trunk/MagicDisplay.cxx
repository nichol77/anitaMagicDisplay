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
#include "AnitaGpsCanvasMaker.h"
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
#include "G12Pos.h"
#include "G12Sat.h"
#include "Adu5Pat.h"
#include "Adu5Sat.h"
#include "Adu5Vtg.h"

//ROOT Includes
#include "TROOT.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeIndex.h"
#include "TButton.h"
#include "TGroupButton.h"
#include "TThread.h"
#include <TGClient.h>

using namespace std;

MagicDisplay*  MagicDisplay::fgInstance = 0;
AnitaCanvasMaker *fEventCanMaker=0;
AnitaRFCanvasMaker *fRFCanMaker=0;
AnitaGpsCanvasMaker *fGpsCanMaker=0;
MagicControlPanel *fControlPanel=0;
//Leave these as global variables for now

void MagicDisplay::zeroPointers() 
{
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
   
   //GPS stuff
   fGpsFile=0;
   fG12PosTree=0;
   fG12SatTree=0;
   fAdu5aPatTree=0;
   fAdu5aSatTree=0;
   fAdu5aVtgTree=0;
   fAdu5bPatTree=0;
   fAdu5bSatTree=0;
   fAdu5bVtgTree=0;
   fG12PosEntry=0;
   fG12SatEntry=0;
   fAdu5aPatEntry=0;
   fAdu5aSatEntry=0;
   fAdu5aVtgEntry=0;
   fAdu5bPatEntry=0;
   fAdu5bSatEntry=0;
   fAdu5bVtgEntry=0;
   fG12PosPtr=0;
   fG12SatPtr=0;
   fAdu5APatPtr=0;
   fAdu5ASatPtr=0;
   fAdu5AVtgPtr=0;
   fAdu5BPatPtr=0;
   fAdu5BSatPtr=0;
   fAdu5BVtgPtr=0;
   fGpsCanvas=0;
   fGpsMainPad=0;
   fGpsInfoPad=0;

   fMagicCanvas=0;
   fMagicEventInfoPad=0;
   fMagicMainPad=0;

   fTurfCanvas=0;
   fTurfMainPad=0;
   fTurfInfoPad=0;
   
   fSumTurfCanvas=0;
   fSumTurfMainPad=0;
   fSumTurfInfoPad=0;
   
   fSurfCanvas=0;
   fSurfMainPad=0;
   fSurfInfoPad=0;

   fAvgSurfCanvas=0;
   fAvgSurfMainPad=0;
   fAvgSurfInfoPad=0;

   fgInstance=this;
   fEventTree=0;
   fSurfHkTree=0;
   fTurfRateTree=0;
   fAvgSurfHkTree=0;
   fSumTurfRateTree=0;
   fEventPlaySleepMs=0;

   fWaveformFormat=MagicDisplayFormatOption::kWaveform;
   fCanvasLayout=MagicDisplayCanvasLayoutOption::kPhiVerticalOnly;

}

MagicDisplay::MagicDisplay()
{
  //Default constructor
  zeroPointers();
}

MagicDisplay::~MagicDisplay()
{
   //Default destructor
}


MagicDisplay::MagicDisplay(char *baseDir, int run, WaveCalType::WaveCalType_t calType)
{
  //Offline constructor
  zeroPointers();
  cout << "MagicDisplay::MagicDisplay(" << baseDir << " , " << run
       << ")" << endl;
  fCurrentRun=run;
  strncpy(fCurrentBaseDir,baseDir,179);
  fCalType=calType;
  
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
 
  fEventCanMaker=new AnitaCanvasMaker(this->fCalType);
  int retVal=this->getEventEntry();
  if(retVal==0)
      this->refreshEventDisplay();   
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
   fUsefulEventPtr = new UsefulAnitaEvent(fRawEventPtr,fCalType,fHeadPtr);  
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
   
   fEventCanMaker->setWaveformFormat(fWaveformFormat);
   fEventCanMaker->setCanvasLayout(fCanvasLayout);

   fEventCanMaker->getEventViewerCanvas(fUsefulEventPtr,fHeadPtr,fMagicMainPad);
   

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
   TButton *butReset = new TButton("Reset Avg","AnitaCanvasMaker::Instance()->resetAverage();",0.85,0.975,0.90,1);
   butReset->SetTextSize(0.5);
   butReset->SetFillColor(kViolet-10);
   butReset->Draw();


   fVertButton = new TButton("V","MagicDisplay::Instance()->setCanvasLayout(MagicDisplayCanvasLayoutOption::kPhiVerticalOnly); MagicDisplay::Instance()->refreshEventDisplay();",0,0.966,0.05,1);
   fVertButton->SetTextSize(0.5);
   fVertButton->SetFillColor(kGray+3);
   fVertButton->Draw();
   fHorizButton = new TButton("H","MagicDisplay::Instance()->setCanvasLayout(MagicDisplayCanvasLayoutOption::kPhiHorizontalOnly); MagicDisplay::Instance()->refreshEventDisplay();",0,0.933,0.05,0.966);
   fHorizButton->SetTextSize(0.5);
   fHorizButton->SetFillColor(kGray);
   fHorizButton->Draw();
   fBothButton = new TButton("V&H","MagicDisplay::Instance()->setCanvasLayout(MagicDisplayCanvasLayoutOption::kPhiCombined); MagicDisplay::Instance()->refreshEventDisplay();",0,0.90,0.05,0.933);
   fBothButton->SetTextSize(0.5);
   fBothButton->SetFillColor(kGray);
   fBothButton->Draw();
   fSurfButton = new TButton("SURF","MagicDisplay::Instance()->setCanvasLayout(MagicDisplayCanvasLayoutOption::kSurfOnly); MagicDisplay::Instance()->refreshEventDisplay();",0.05,0.966,0.1,1);
   fSurfButton->SetTextSize(0.5);
   fSurfButton->SetFillColor(kGray);
   fSurfButton->Draw();
   fPayloadButton = new TButton("Payload","MagicDisplay::Instance()->setCanvasLayout(MagicDisplayCanvasLayoutOption::kPayloadView); MagicDisplay::Instance()->refreshEventDisplay();",0.05,0.933,0.1,0.966);
   fPayloadButton->SetTextSize(0.5);
   fPayloadButton->SetFillColor(kGray);
   fPayloadButton->Draw();

   //NEW BUTTONS
   fWaveformButton = new TButton("Waveform","MagicDisplay::Instance()->setWaveformFormat(MagicDisplayFormatOption::kWaveform); MagicDisplay::Instance()->refreshEventDisplay();",0.1,0.975,0.2,1);
   fWaveformButton->SetTextSize(0.4);
   fWaveformButton->SetFillColor(kGray+3);
   fWaveformButton->Draw();
   fPowerButton = new TButton("FFT","MagicDisplay::Instance()->setWaveformFormat(MagicDisplayFormatOption::kFFT); MagicDisplay::Instance()->refreshEventDisplay();",0.1,0.95,0.2,0.975);
   fPowerButton->SetTextSize(0.4);
   fPowerButton->SetFillColor(kGray);
   fPowerButton->Draw();
   fHilbertButton = new TButton("Hilbert","MagicDisplay::Instance()->setWaveformFormat(MagicDisplayFormatOption::kHilbertEnvelope); MagicDisplay::Instance()->refreshEventDisplay();",0.1,0.925,0.2,0.95);
   fHilbertButton->SetTextSize(0.4);
   fHilbertButton->SetFillColor(kGray);
   fHilbertButton->Draw();
   fAverageFFTButton = new TButton("Average FFT","MagicDisplay::Instance()->setWaveformFormat(MagicDisplayFormatOption::kAveragedFFT); MagicDisplay::Instance()->refreshEventDisplay();",0.1,0.9,0.2,0.925);
   fAverageFFTButton->SetTextSize(0.4);
   fAverageFFTButton->SetFillColor(kGray);
   fAverageFFTButton->Draw();

   
}

void MagicDisplay::setCanvasLayout(MagicDisplayCanvasLayoutOption::MagicDisplayCanvasLayoutOption_t option)
{
   fCanvasLayout=option;
   switch(option) {
   case MagicDisplayCanvasLayoutOption::kSurfOnly:
      fVertButton->SetFillColor(kGray);
      fHorizButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray);
      fSurfButton->SetFillColor(kGray+3);
      fPayloadButton->SetFillColor(kGray);
      break;
   case MagicDisplayCanvasLayoutOption::kPhiVerticalOnly:
      fVertButton->SetFillColor(kGray+3);
      fHorizButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray);
      fSurfButton->SetFillColor(kGray);
      fPayloadButton->SetFillColor(kGray);
      break;
   case MagicDisplayCanvasLayoutOption::kPhiHorizontalOnly:
      fHorizButton->SetFillColor(kGray+3);
      fVertButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray);
      fSurfButton->SetFillColor(kGray);
      fPayloadButton->SetFillColor(kGray);
      break;
   case MagicDisplayCanvasLayoutOption::kPhiCombined:
      fHorizButton->SetFillColor(kGray);
      fVertButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray+3);
      fSurfButton->SetFillColor(kGray);
      fPayloadButton->SetFillColor(kGray);
      break;
   case MagicDisplayCanvasLayoutOption::kPayloadView:
      fHorizButton->SetFillColor(kGray);
      fVertButton->SetFillColor(kGray);
      fBothButton->SetFillColor(kGray);
      fSurfButton->SetFillColor(kGray);
      fPayloadButton->SetFillColor(kGray+3);
      break;
   default:
      break;
   }   
   fVertButton->Modified();
   fHorizButton->Modified();  
   fBothButton->Modified(); 
   fSurfButton->Modified(); 
   fPayloadButton->Modified();
}


void MagicDisplay::setWaveformFormat(MagicDisplayFormatOption::MagicDisplayFormatOption_t waveformOption)
{
  fWaveformFormat=waveformOption;
  if(waveformOption==MagicDisplayFormatOption::kWaveform) {
      //Turn on fft view off     
      fWaveformButton->SetFillColor(kGray+3);
      fPowerButton->SetFillColor(kGray);
      fHilbertButton->SetFillColor(kGray);
      fAverageFFTButton->SetFillColor(kGray);
      fWaveformButton->Modified();
      fPowerButton->Modified();
      fHilbertButton->Modified();
      fAverageFFTButton->Modified();
  }
  else if(waveformOption==MagicDisplayFormatOption::kFFT) {
      //Turn fft view on
      fWaveformButton->SetFillColor(kGray);
      fPowerButton->SetFillColor(kGray+3);
      fHilbertButton->SetFillColor(kGray);
      fAverageFFTButton->SetFillColor(kGray);
      fWaveformButton->Modified();
      fPowerButton->Modified();
      fHilbertButton->Modified();
      fAverageFFTButton->Modified();
   }
  else if(waveformOption==MagicDisplayFormatOption::kHilbertEnvelope) {
      //Turn fft view on
      fWaveformButton->SetFillColor(kGray);
      fPowerButton->SetFillColor(kGray);
      fHilbertButton->SetFillColor(kGray+3);
      fAverageFFTButton->SetFillColor(kGray);
      fWaveformButton->Modified();
      fPowerButton->Modified();
      fHilbertButton->Modified();
      fAverageFFTButton->Modified();
   }
  else if(waveformOption==MagicDisplayFormatOption::kAveragedFFT) {
      //Turn fft view on
      fWaveformButton->SetFillColor(kGray);
      fPowerButton->SetFillColor(kGray);
      fHilbertButton->SetFillColor(kGray);
      fAverageFFTButton->SetFillColor(kGray+3);
      fWaveformButton->Modified();
      fPowerButton->Modified();
      fHilbertButton->Modified();
      fAverageFFTButton->Modified();
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
  int retVal=this->getTurfEntry();
  if(retVal==0)
    this->refreshTurfDisplay();   
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
   TButton *butNext = new TButton("Next","MagicDisplay::Instance()->displayNextSurf();",0.9,0.95,1,1);
   butNext->SetTextSize(0.4);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   TButton *butPrev = new TButton("Prev.","MagicDisplay::Instance()->displayPreviousSurf();",0.9,0.90,1,0.95);
   butPrev->SetTextSize(0.4);
   butPrev->SetFillColor(kBlue-10);
   butPrev->Draw();


   TButton *butPlay = new TButton("Play","MagicDisplay::Instance()->startSurfHkPlaying();",0.8,0.95,0.9,1);
   butPlay->SetTextSize(0.5);
   butPlay->SetFillColor(kGreen-10);
   butPlay->Draw();

   TButton *butStop = new TButton("Stop","MagicDisplay::Instance()->stopSurfHkPlaying();",0.8,0.90,0.9,0.95);
   butStop->SetTextSize(0.5);
   butStop->SetFillColor(kRed-10);
   butStop->Draw();

   fSurfSurfViewButton = new TButton("SURF View","MagicDisplay::Instance()->setSurfHkDisplay(MagicDisplaySurfHkDisplay::kSurfView); MagicDisplay::Instance()->refreshSurfDisplay();",0,0.97,0.1,1);
   fSurfSurfViewButton->SetTextSize(0.4);
   fSurfSurfViewButton->SetFillColor(kGray+3);
   fSurfSurfViewButton->Draw();


   fSurfPhiViewButton = new TButton("PHI View","MagicDisplay::Instance()->setSurfHkDisplay(MagicDisplaySurfHkDisplay::kPhiView); MagicDisplay::Instance()->refreshSurfDisplay();",0,0.94,0.1,0.97);
   fSurfPhiViewButton->SetTextSize(0.4);
   fSurfPhiViewButton->SetFillColor(kGray);
   fSurfPhiViewButton->Draw();


   fSurfTimeViewButton = new TButton("Time View","MagicDisplay::Instance()->setSurfHkDisplay(MagicDisplaySurfHkDisplay::kTimeView); MagicDisplay::Instance()->refreshSurfDisplay();",0,0.91,0.1,0.94);
   fSurfTimeViewButton->SetTextSize(0.4);
   fSurfTimeViewButton->SetFillColor(kGray);
   fSurfTimeViewButton->Draw();

   fSurfAdcViewButton  = new TButton("ADC","MagicDisplay::Instance()->toggleSurfRfPowerView(0); MagicDisplay::Instance()->refreshSurfDisplay();",0.1,0.91,0.2,0.94);
   fSurfAdcViewButton->SetTextSize(0.4);
   fSurfAdcViewButton->SetFillColor(kGray);
   fSurfAdcViewButton->Draw();

   fSurfKelvinViewButton  = new TButton("T_inp","MagicDisplay::Instance()->toggleSurfRfPowerView(1); MagicDisplay::Instance()->refreshSurfDisplay();",0.1,0.94,0.15,0.97);
   fSurfKelvinViewButton->SetTextSize(0.4);
   fSurfKelvinViewButton->SetFillColor(kGray+3);
   fSurfKelvinViewButton->Draw();

   fSurfMeasuredKelvinViewButton  = new TButton("T_meas","MagicDisplay::Instance()->toggleSurfRfPowerView(2); MagicDisplay::Instance()->refreshSurfDisplay();",0.15,0.94,0.2,0.97);
   fSurfMeasuredKelvinViewButton->SetTextSize(0.4);
   fSurfMeasuredKelvinViewButton->SetFillColor(kGray);
   fSurfMeasuredKelvinViewButton->Draw();
   
   fSurfLogButton  = new TButton("Log Scale","MagicDisplay::Instance()->toggleSurfLogView(); MagicDisplay::Instance()->refreshSurfDisplay();",0.1,0.97,0.2,1);
   fSurfLogButton->SetTextSize(0.4);
   fSurfLogButton->SetFillColor(kGray);
   fSurfLogButton->Draw();


}


void MagicDisplay::toggleSurfLogView()
{
  Int_t nextState=0;
  if(fSurfLogButton->GetFillColor()==kGray) {
    nextState=1;
  }
  else {
    nextState=0;
  }
  //  std::cout << nextState << "\an";
  if(nextState) {
    fRFCanMaker->setSurfLogFlag(1);
    fSurfLogButton->SetFillColor(kGray+3);
    fSurfLogButton->Modified();
  }
  else {
    fRFCanMaker->setSurfLogFlag(0);
    fSurfLogButton->SetFillColor(kGray);
    fSurfLogButton->Modified();
  }
   refreshSurfDisplay();
}

void MagicDisplay::setSurfHkDisplay(MagicDisplaySurfHkDisplay::MagicDisplaySurfHkDisplay_t surfDisplay)
{
  fRFCanMaker->setSurfHkDisplay(surfDisplay);
  if(surfDisplay==MagicDisplaySurfHkDisplay::kSurfView) {
      //Turn on phi view off
      fSurfSurfViewButton->SetFillColor(kGray+3);
      fSurfPhiViewButton->SetFillColor(kGray);
      fSurfTimeViewButton->SetFillColor(kGray);
      fSurfSurfViewButton->Modified();
      fSurfPhiViewButton->Modified();
      fSurfTimeViewButton->Modified();
   }
  else if(surfDisplay==MagicDisplaySurfHkDisplay::kPhiView) {
      //Turn phi view on
      fSurfSurfViewButton->SetFillColor(kGray);
      fSurfPhiViewButton->SetFillColor(kGray+3);
      fSurfTimeViewButton->SetFillColor(kGray);
      fSurfSurfViewButton->Modified();
      fSurfPhiViewButton->Modified();
      fSurfTimeViewButton->Modified();
   }
  else if(surfDisplay==MagicDisplaySurfHkDisplay::kPhiView) {
      //Turn phi view on
      fSurfSurfViewButton->SetFillColor(kGray);
      fSurfPhiViewButton->SetFillColor(kGray);
      fSurfTimeViewButton->SetFillColor(kGray+3);
      fSurfSurfViewButton->Modified();
      fSurfPhiViewButton->Modified();
      fSurfTimeViewButton->Modified();
   }
 
}


void MagicDisplay::toggleSurfRfPowerView(Int_t kelvinView)
{
   
   if(kelvinView) {
      //Turn on 
     fRFCanMaker->fSurfKelvinView=kelvinView;
     if(kelvinView==1) {
       fSurfKelvinViewButton->SetFillColor(kGray+3);
       fSurfMeasuredKelvinViewButton->SetFillColor(kGray);
     }
     else if(kelvinView==2) {
       fSurfKelvinViewButton->SetFillColor(kGray);
       fSurfMeasuredKelvinViewButton->SetFillColor(kGray+3);
     }
     fSurfAdcViewButton->SetFillColor(kGray);
     fSurfKelvinViewButton->Modified();
     fSurfAdcViewButton->Modified();
     fSurfMeasuredKelvinViewButton->Modified();
   }
   else {
      //Turn phi view on
      fRFCanMaker->fSurfKelvinView=0;
      fSurfKelvinViewButton->SetFillColor(kGray);
      fSurfMeasuredKelvinViewButton->SetFillColor(kGray);
      fSurfAdcViewButton->SetFillColor(kGray+3);
      fSurfKelvinViewButton->Modified();
      fSurfAdcViewButton->Modified();
      fSurfMeasuredKelvinViewButton->Modified();
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
   TButton *butNext = new TButton("Next.","MagicDisplay::Instance()->displayNextAvgSurf();",0.9,0.95,1,1);
   butNext->SetTextSize(0.4);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   TButton *butPrev = new TButton("Prev.","MagicDisplay::Instance()->displayPreviousAvgSurf();",0.9,0.90,1,0.95);
   butPrev->SetTextSize(0.4);
   butPrev->SetFillColor(kBlue-10);
   butPrev->Draw();
   TButton *butPlay = new TButton("Play","MagicDisplay::Instance()->startAvgSurfHkPlaying();",0.85,0.95,0.9,1);
   butPlay->SetTextSize(0.4);
   butPlay->SetFillColor(kGreen-10);
   butPlay->Draw();
   TButton *butStop = new TButton("Stop","MagicDisplay::Instance()->stopAvgSurfHkPlaying();",0.85,0.90,0.9,0.95);
   butStop->SetTextSize(0.4);
   butStop->SetFillColor(kRed-10);
   butStop->Draw();

  

   fAvgSurfAdcViewButton  = new TButton("ADC","MagicDisplay::Instance()->toggleAvgSurfRfPowerView(0); MagicDisplay::Instance()->refreshAvgSurfDisplay();",0.1,0.91,0.2,0.94);
   fAvgSurfAdcViewButton->SetTextSize(0.4);
   fAvgSurfAdcViewButton->SetFillColor(kGray);
   fAvgSurfAdcViewButton->Draw();

   fAvgSurfKelvinViewButton  = new TButton("T_inp","MagicDisplay::Instance()->toggleAvgSurfRfPowerView(1); MagicDisplay::Instance()->refreshAvgSurfDisplay();",0.1,0.94,0.15,0.97);
   fAvgSurfKelvinViewButton->SetTextSize(0.4);
   fAvgSurfKelvinViewButton->SetFillColor(kGray+3);
   fAvgSurfKelvinViewButton->Draw();


   fAvgSurfMeasuredKelvinViewButton  = new TButton("T_meas","MagicDisplay::Instance()->toggleAvgSurfRfPowerView(2); MagicDisplay::Instance()->refreshAvgSurfDisplay();",0.15,0.94,0.2,0.97);
   fAvgSurfMeasuredKelvinViewButton->SetTextSize(0.4);
   fAvgSurfMeasuredKelvinViewButton->SetFillColor(kGray+3);
   fAvgSurfMeasuredKelvinViewButton->Draw();
   
   fAvgSurfLogButton  = new TButton("Log Scale","MagicDisplay::Instance()->toggleAvgSurfLogView(); MagicDisplay::Instance()->refreshAvgSurfDisplay();",0.1,0.97,0.2,1);
   fAvgSurfLogButton->SetTextSize(0.4);
   fAvgSurfLogButton->SetFillColor(kGray);
   fAvgSurfLogButton->Draw();



   fAvgSurfSurfViewButton = new TButton("SURF View","MagicDisplay::Instance()->setAvgSurfHkDisplay(MagicDisplaySurfHkDisplay::kSurfView); MagicDisplay::Instance()->refreshAvgSurfDisplay();",0,0.955,0.1,1);
   fAvgSurfSurfViewButton->SetTextSize(0.4);
   fAvgSurfSurfViewButton->SetFillColor(kGray+3);
   fAvgSurfSurfViewButton->Draw();


   fAvgSurfPhiViewButton = new TButton("PHI View","MagicDisplay::Instance()->setAvgSurfHkDisplay(MagicDisplaySurfHkDisplay::kPhiView); MagicDisplay::Instance()->refreshAvgSurfDisplay();",0,0.91,0.1,0.955);
   fAvgSurfPhiViewButton->SetTextSize(0.4);
   fAvgSurfPhiViewButton->SetFillColor(kGray);
   fAvgSurfPhiViewButton->Draw();

}


void MagicDisplay::toggleAvgSurfLogView()
{
  Int_t nextState=0;
  if(fAvgSurfLogButton->GetFillColor()==kGray) {
    nextState=1;
  }
  else {
    nextState=0;
  }
  //  std::cout << nextState << "\n";
  if(nextState) {
    fRFCanMaker->setAvgSurfLogFlag(1);
    fAvgSurfLogButton->SetFillColor(kGray+3);
    fAvgSurfLogButton->Modified();
  }
  else {
    fRFCanMaker->setAvgSurfLogFlag(0);
    fAvgSurfLogButton->SetFillColor(kGray);
    fAvgSurfLogButton->Modified();
  }
   refreshAvgSurfDisplay();
}

void MagicDisplay::setAvgSurfHkDisplay(MagicDisplaySurfHkDisplay::MagicDisplaySurfHkDisplay_t surfDisplay)
{
  
  fRFCanMaker->setAvgSurfHkDisplay(surfDisplay);
 
  if(surfDisplay==MagicDisplaySurfHkDisplay::kSurfView) {
      //Turn on phi view off
      fAvgSurfSurfViewButton->SetFillColor(kGray+3);
      fAvgSurfPhiViewButton->SetFillColor(kGray);
      fAvgSurfSurfViewButton->Modified();
      fAvgSurfPhiViewButton->Modified();
   }
  else if(surfDisplay==MagicDisplaySurfHkDisplay::kPhiView) {
      //Turn phi view on
      fAvgSurfSurfViewButton->SetFillColor(kGray);
      fAvgSurfPhiViewButton->SetFillColor(kGray+3);
      fAvgSurfSurfViewButton->Modified();
      fAvgSurfPhiViewButton->Modified();
   }
      
}


void MagicDisplay::toggleAvgSurfRfPowerView(Int_t kelvinView)
{
   
   if(kelvinView) {
      //Turn on 
     //     std::cout << "setting fAvgSurfKelvinView " << 1 << "\n";
      fRFCanMaker->fAvgSurfKelvinView=kelvinView;
      if(kelvinView==2) {
	fAvgSurfMeasuredKelvinViewButton->SetFillColor(kGray+3);
	fAvgSurfKelvinViewButton->SetFillColor(kGray);
      }
      else if(kelvinView==1){
	fAvgSurfMeasuredKelvinViewButton->SetFillColor(kGray);
	fAvgSurfKelvinViewButton->SetFillColor(kGray+3);
      }
	
      fAvgSurfAdcViewButton->SetFillColor(kGray);
      fAvgSurfKelvinViewButton->Modified();
      fAvgSurfAdcViewButton->Modified();
      fAvgSurfMeasuredKelvinViewButton->Modified();
   }
   else {
      //Turn phi view on
      fRFCanMaker->fAvgSurfKelvinView=0;
      fAvgSurfMeasuredKelvinViewButton->SetFillColor(kGray);
      fAvgSurfKelvinViewButton->SetFillColor(kGray);
      fAvgSurfAdcViewButton->SetFillColor(kGray+3);
      fAvgSurfKelvinViewButton->Modified();
      fAvgSurfAdcViewButton->Modified();
      fAvgSurfMeasuredKelvinViewButton->Modified();
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

void MagicDisplay::startSurfHkPlaying()
{
  fInSurfPlayMode=1;
  do {
    gSystem->ProcessEvents();
    if(!fInSurfPlayMode) break;
    if(fEventPlaySleepMs>0)
      gSystem->Sleep(fEventPlaySleepMs);
  }
  while(this->displayNextSurf()==0);  
}

void MagicDisplay::stopSurfHkPlaying()
{
  fInSurfPlayMode=0;
}


void MagicDisplay::startAvgSurfHkPlaying()
{
  fInAvgSurfPlayMode=1;
  do {
    gSystem->ProcessEvents();
    if(!fInAvgSurfPlayMode) break;
    if(fEventPlaySleepMs>0)
      gSystem->Sleep(fEventPlaySleepMs);
  }
  while(this->displayNextAvgSurf()==0);  
}

void MagicDisplay::stopAvgSurfHkPlaying()
{
  fInAvgSurfPlayMode=0;
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

int MagicDisplay::loadGpsTrees()
{
  char gpsName[FILENAME_MAX];
  sprintf(gpsName,"%s/run%d/gpsFile%d.root",fCurrentBaseDir,
	  fCurrentRun,fCurrentRun);
  fGpsFile = TFile::Open(gpsName);
  if(!fGpsFile) {
    cout << "Couldn't open: " << gpsName << "\n";
    return -1;
  }
  fG12PosTree = (TTree*) fGpsFile->Get("g12PosTree");
  if(!fG12PosTree) {
    cout << "Couldn't get g12PosTree\n";
  }
  else {
    fG12PosTree->SetBranchAddress("pos",&fG12PosPtr);
  }
  fG12SatTree = (TTree*) fGpsFile->Get("g12SatTree");
  if(!fG12SatTree) {
    cout << "Couldn't get g12SatTree\n";
  }
  else {
    fG12SatTree->SetBranchAddress("sat",&fG12SatPtr);
  }
  fAdu5aPatTree = (TTree*) fGpsFile->Get("adu5PatTree");
  if(!fAdu5aPatTree) {
    cout << "Couldn't get adu5aPatTree\n";
  }
  else {
    fAdu5aPatTree->SetBranchAddress("pat",&fAdu5APatPtr);
  }
  fAdu5aSatTree = (TTree*) fGpsFile->Get("adu5SatTree");
  if(!fAdu5aSatTree) {
    cout << "Couldn't get adu5aSatTree\n";
  }
  else {
    fAdu5aSatTree->SetBranchAddress("sat",&fAdu5ASatPtr);
    fAdu5aSatTree->BuildIndex("realTime");
    fAdu5aSatIndex=(TTreeIndex*)fAdu5aSatTree->GetTreeIndex();
  }
  fAdu5aVtgTree = (TTree*) fGpsFile->Get("adu5VtgTree");
  if(!fAdu5aVtgTree) {
    cout << "Couldn't get adu5aVtgTree\n";
  }
  else {
    fAdu5aVtgTree->SetBranchAddress("vtg",&fAdu5AVtgPtr);
  }
  fAdu5bPatTree = (TTree*) fGpsFile->Get("adu5bPatTree");
  if(!fAdu5bPatTree) {
    cout << "Couldn't get adu5bPatTree\n";
  }
  else {
    fAdu5bPatTree->SetBranchAddress("pat",&fAdu5BPatPtr);
  }
  fAdu5bSatTree = (TTree*) fGpsFile->Get("adu5bSatTree");
  if(!fAdu5bSatTree) {
    cout << "Couldn't get adu5bSatTree\n";
  }
  else {
    fAdu5bSatTree->SetBranchAddress("sat",&fAdu5BSatPtr);
    fAdu5bSatTree->BuildIndex("realTime");
    fAdu5bSatIndex=(TTreeIndex*)fAdu5bSatTree->GetTreeIndex();
  }
  fAdu5bVtgTree = (TTree*) fGpsFile->Get("adu5bVtgTree");
  if(!fAdu5bVtgTree) {
    cout << "Couldn't get adu5bVtgTree\n";
  }
  else {
    fAdu5bVtgTree->SetBranchAddress("vtg",&fAdu5BVtgPtr);
  }
  fG12PosEntry=0;
  fG12SatEntry=0;
  fAdu5aPatEntry=0;
  fAdu5aSatEntry=0;
  fAdu5aVtgEntry=0;
  fAdu5bPatEntry=0;
  fAdu5bSatEntry=0;
  fAdu5bVtgEntry=0;
  return 0;
}

void MagicDisplay::startGpsDisplay()
{
  if(!fGpsCanMaker)
    fGpsCanMaker=AnitaGpsCanvasMaker::Instance();
  int retVal=getGpsEntry();
  if(retVal==0)
    refreshGpsDisplay();


}

int MagicDisplay::displayNextGps()
{
  Int_t retVal=0;
  switch(fGpsCanMaker->getGpsDisplayOpt()) {
  case MagicDisplayGpsDisplay::kMapView:
    break;
  case MagicDisplayGpsDisplay::kSatView:
    fG12SatEntry++;
    if(fG12SatTree) {
      if(fG12SatEntry>=fG12SatTree->GetEntries())
	return -1;
    }
    getGpsEntry();
    if(fAdu5aSatIndex) {
      fAdu5aSatEntry=fAdu5aSatIndex->GetEntryNumberWithBestIndex(fG12SatPtr->realTime,0);
      if(fAdu5aSatEntry<0) fAdu5aSatEntry=0;
    }
    if(fAdu5bSatIndex) {
      fAdu5bSatEntry=fAdu5bSatIndex->GetEntryNumberWithBestIndex(fG12SatPtr->realTime,0);
      if(fAdu5bSatEntry<0) fAdu5aSatEntry=0;
    }
    break;
  case MagicDisplayGpsDisplay::kTimeView:
    break;
  default:
    break;
  }
  retVal=getGpsEntry();
  if(retVal==0)
    refreshGpsDisplay();
  return retVal;

}

int MagicDisplay::displayPreviousGps()
{
  Int_t retVal=0;
  switch(fGpsCanMaker->getGpsDisplayOpt()) {
  case MagicDisplayGpsDisplay::kMapView:
    break;
  case MagicDisplayGpsDisplay::kSatView:
    if(fG12SatEntry>0)
      fG12SatEntry--;
    if(fAdu5aSatEntry>0)
      fAdu5aSatEntry--;
    if(fAdu5bSatEntry>0)
      fAdu5bSatEntry--;
    break;
  case MagicDisplayGpsDisplay::kTimeView:
    break;
  default:
    break;
  }
  retVal=getGpsEntry();
  if(retVal==0)
    refreshGpsDisplay();
  return retVal;
}

void MagicDisplay::refreshGpsDisplay()
{
  if(!fGpsCanvas) {
      fGpsCanvas = new TCanvas("canMagicGps","canMagicGps",800,700);
      fGpsCanvas->cd();
      drawGpsButtons();
   }
   if(!fGpsMainPad) {
      fGpsCanvas->cd();
      fGpsMainPad= new TPad("canMagicGpsMain","canMagicGpsMain",0,0,1,0.9);
      fGpsMainPad->Draw();
      fGpsCanvas->Update();
   }
   if(!fGpsInfoPad) {
      fGpsCanvas->cd();
      fGpsInfoPad= new TPad("canMagicGpsInfo","canMagicGpsInfo",0.2,0.91,0.8,0.99);
      fGpsInfoPad->Draw();
      fGpsCanvas->Update();
   }  
  //This will need to change
   fGpsCanMaker->getGpsInfoCanvas(fAdu5ASatPtr,fAdu5BSatPtr,fG12SatPtr,
				  fAdu5APatPtr,fAdu5BPatPtr,fG12PosPtr,
				  fGpsInfoPad);
   fGpsCanMaker->getGpsMainCanvas(fAdu5ASatPtr,fAdu5BSatPtr,fG12SatPtr,
				fAdu5APatPtr,fAdu5BPatPtr,fG12PosPtr,
				fGpsMainPad);

   fGpsCanvas->Update();
   fGpsCanvas->Modified();

}

int MagicDisplay::getGpsEntry()
{
  if(!fGpsFile) {
    if(loadGpsTrees()<0) {
      cout << "Couldn't open GPS file\n";
      return -1;
    }
  }
  
  //Now have to decide which tree to move on in
  Int_t newData=0;
  if(fG12PosTree) {
    //    cout << "Got g12PosTree\n";
    if(fG12PosEntry<fG12PosTree->GetEntries()) {
      fG12PosTree->GetEntry(fG12PosEntry);
      newData++;
    }
    else {
      std::cout << "Now more entries in g12PosTree\n";
    }
  }
  if(fG12SatTree) {
    //    cout << "Got g12SatTree\n";
    if(fG12SatEntry<fG12SatTree->GetEntries()) {
      fG12SatTree->GetEntry(fG12SatEntry);
      newData++;
    }
    else {
      std::cout << "Now more entries in g12SatTree\n";
    }
  }
  if(fAdu5aSatTree) {
    //    cout << "Got adu5SatTree\n";
    if(fAdu5aSatEntry<fAdu5aSatTree->GetEntries()) {
      fAdu5aSatTree->GetEntry(fAdu5aSatEntry);
      newData++;
    }
    else {
      std::cout << "Now more entries in adu5aSatTree\n";
    }
  }
  if(fAdu5bSatTree) {
    //    cout << "Got adu5bSatTree\n";
    if(fAdu5bSatEntry<fAdu5bSatTree->GetEntries()) {
      fAdu5bSatTree->GetEntry(fAdu5bSatEntry);
      newData++;
    }
    else {
      std::cout << "Now more entries in adu5bSatTree\n";
    }
  }
  
  if(fAdu5aPatTree) {
    //    cout << "Got adu5PatTree\n";
    if(fAdu5aPatEntry<fAdu5aPatTree->GetEntries()) {
      fAdu5aPatTree->GetEntry(fAdu5aPatEntry);
      newData++;
    }
    else {
      std::cout << "Now more entries in adu5aPatTree\n";
    }
  }

  if(fAdu5bPatTree) {
    //    cout << "Got adu5bPatTree\n";
    if(fAdu5bPatEntry<fAdu5bPatTree->GetEntries()) {
      fAdu5bPatTree->GetEntry(fAdu5bPatEntry);
      newData++;
    }
    else {
      std::cout << "Now more entries in adu5bPatTree\n";
    }
  }
  if(fAdu5aVtgTree) {
    //    cout << "Got adu5VtgTree\n";
    if(fAdu5aVtgEntry<fAdu5aVtgTree->GetEntries()) {
      fAdu5aVtgTree->GetEntry(fAdu5aVtgEntry);
      newData++;
    }
    else {
    std::cout << "Now more entries in adu5aVtgTree\n";
    }
  }
  if(fAdu5bVtgTree) {
    //    cout << "Got adu5bVtgTree\n";
    if(fAdu5bVtgEntry<fAdu5bVtgTree->GetEntries()) {
      fAdu5bVtgTree->GetEntry(fAdu5bVtgEntry);
      newData++;
    }
    else {
      std::cout << "Now more entries in adu5bVtgTree\n";
    }
  }


  if(newData==0)
    return -1;
  return 0;
}

void MagicDisplay::setGpsView(MagicDisplayGpsDisplay::MagicDisplayGpsDisplay_t theDisplay)
{
  if(theDisplay==MagicDisplayGpsDisplay::kSatView) {
    fGpsCanMaker->setGpsDisplayOpt(theDisplay);
    fGpsSatViewButton->SetFillColor(kGray+3);
    fGpsMapViewButton->SetFillColor(kGray);
    fGpsSatViewButton->Modified();
    fGpsMapViewButton->Modified();
  }
  else if(theDisplay==MagicDisplayGpsDisplay::kMapView) {
    fGpsCanMaker->setGpsDisplayOpt(theDisplay);
    fGpsSatViewButton->SetFillColor(kGray);
    fGpsMapViewButton->SetFillColor(kGray+3);
    fGpsSatViewButton->Modified();
    fGpsMapViewButton->Modified();
  }
  refreshGpsDisplay();
}

void MagicDisplay::drawGpsButtons()
{
  TButton *butNext = new TButton("Next.","MagicDisplay::Instance()->displayNextGps();",0.9,0.95,1,1);
   butNext->SetTextSize(0.4);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   TButton *butPrev = new TButton("Prev.","MagicDisplay::Instance()->displayPreviousGps();",0.9,0.90,1,0.95);
   butPrev->SetTextSize(0.4);
   butPrev->SetFillColor(kBlue-10);
   butPrev->Draw();
   TButton *butPlay = new TButton("Play","MagicDisplay::Instance()->startGpsPlaying();",0.85,0.95,0.9,1);
   butPlay->SetTextSize(0.4);
   butPlay->SetFillColor(kGreen-10);
   butPlay->Draw();
   TButton *butStop = new TButton("Stop","MagicDisplay::Instance()->stopGpsPlaying();",0.85,0.90,0.9,0.95);
   butStop->SetTextSize(0.4);
   butStop->SetFillColor(kRed-10);
   butStop->Draw();

   fGpsSatViewButton = new TButton("Satellite","MagicDisplay::Instance()->setGpsView(MagicDisplayGpsDisplay::kSatView);",0.,0.95,0.1,1);
   fGpsSatViewButton->SetTextSize(0.4);
   fGpsSatViewButton->SetFillColor(kGray+3);
   fGpsSatViewButton->Draw();

   fGpsMapViewButton = new TButton("Map View","MagicDisplay::Instance()->setGpsView(MagicDisplayGpsDisplay::kMapView);",0.,0.9,0.1,0.95);
   fGpsMapViewButton->SetTextSize(0.4);
   fGpsMapViewButton->SetFillColor(kGray);
   fGpsMapViewButton->Draw();

}


void MagicDisplay::startGpsPlaying()
{
  fInGpsPlayMode=1;
  do {
    gSystem->ProcessEvents();
    if(!fInGpsPlayMode) break;
  }
  while(this->displayNextGps()==0);

} 
 
void MagicDisplay::stopGpsPlaying()
 {
   fInGpsPlayMode=0;
 }
