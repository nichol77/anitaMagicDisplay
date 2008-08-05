//////////////////////////////////////////////////////////////////////////////
/////  MagicDisplay.h        MagicDisplay                                /////
/////                                                                    /////
/////  Description:                                                      /////
/////      The Marvellous ANITA Graphical Interface and Classy Display   /////
/////     (Magic Display) is a simple event display for ANITA. It's not  /////
/////     nearly as all singing and dancing as Ped's display, but it     /////
/////     does provide a convenient interface to the data and it has     /////
/////     some FFT etc. capabilities.                                    ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef MAGICDISPLAY_H
#define MAGICDISPLAY_H

//Includes
#include "TChain.h"
#include "MagicDisplayConventions.h"
#include "AnitaConventions.h"

class TCanvas;
class TPad;
class RawAnitaHeader;
class PrettyAnitaHk;
class RawAnitaEvent;
class UsefulAnitaEvent;
class TurfRate;
class SummedTurfRate;
class AveragedSurfHk;
class SurfHk;
class TButton;
class TTreeIndex;
class TFile;

class MagicDisplay 
{
 public:
  
  MagicDisplay(char *baseDir, int run, WaveCalType::WaveCalType_t calType=WaveCalType::kVoltageTime);
  MagicDisplay();
  ~MagicDisplay();

  //Control Panel Functions
  void startControlPanel();
  Int_t getCurrentRun()
  {return fCurrentRun;}
  UInt_t getCurrentEvent();

  void startEventPlaying();
  void startEventPlayingReverse();
  void stopEventPlaying();

  void closeCurrentRun();

  int loadEventTree();
  void startEventDisplay();
  int displayThisEvent(UInt_t eventNumber, UInt_t runNumber);
  int displayNextEvent();
  int displayPreviousEvent();
  void refreshEventDisplay();
  int getEventEntry();
  void drawEventButtons();
  void setMainCanvasOption(MagicDisplayOption::MagicDisplayOption_t option); 
  void toggleWaveformView(Int_t waveformView);

  int loadTurfTree();
  void startTurfDisplay();
  int displayNextTurf();
  int displayPreviousTurf();
  void refreshTurfDisplay();
  int getTurfEntry();
  void drawTurfButtons();
  void toggleTurfYScale();

  int loadSurfTree();
  void startSurfDisplay();
  int displayNextSurf();
  int displayPreviousSurf();
  void refreshSurfDisplay();
  int getSurfEntry();
  void drawSurfButtons();
  void toggleSurfSurfView(Int_t surfView);

  int loadAvgSurfTree();
  void startAvgSurfDisplay();
  int displayNextAvgSurf();
  int displayPreviousAvgSurf();
  void refreshAvgSurfDisplay();
  int getAvgSurfEntry();
  void drawAvgSurfButtons();
  void toggleAvgSurfSurfView(Int_t surfView);

  int loadSumTurfTree();
  void startSumTurfDisplay();
  int displayNextSumTurf();
  int displayPreviousSumTurf();
  void refreshSumTurfDisplay();
  int getSumTurfEntry();
  void drawSumTurfButtons();
  void toggleSumTurfYScale();

  //Instance generator
  static MagicDisplay*  Instance();
  

  TFile *fHeadFile;
  TFile *fEventFile;
  TFile *fTurfRateFile;
  TFile *fSumTurfRateFile;
  TFile *fSurfHkFile;
  TFile *fAvgSurfHkFile;

  //Here are the data managers
  TTree *fHeadTree;
  TChain *fEventTree;
  TTree *fPrettyHkTree;
  TTree *fTurfRateTree;
  TTree *fSurfHkTree;
  TTree *fSumTurfRateTree;
  TTree *fAvgSurfHkTree;

  //And some useful info to keep track of what is where
  Long64_t fEventEntry;
  Long64_t fPrettyHkEntry;
  Long64_t fTurfRateEntry;
  Long64_t fSurfHkEntry;
  Long64_t fSumTurfRateEntry;
  Long64_t fAvgSurfHkEntry;

  //And something to help with the indexing
  TTreeIndex *fHeadIndex;

  UInt_t fCurrentRun;
  Char_t fCurrentBaseDir[180];
  

 protected:
   static MagicDisplay *fgInstance;  
   // protect against multiple instances

 private:
   MagicDisplayOption::MagicDisplayOption_t fMainOption;
   TCanvas *fMagicCanvas; //The main canvas
   TPad *fMagicMainPad;
   TPad *fMagicEventInfoPad;

   TCanvas *fTurfCanvas;
   TPad *fTurfMainPad;
   TPad *fTurfInfoPad;

   TCanvas *fSumTurfCanvas;
   TPad *fSumTurfMainPad;
   TPad *fSumTurfInfoPad;

   TCanvas *fSurfCanvas;
   TPad *fSurfMainPad;
   TPad *fSurfInfoPad;

   TCanvas *fAvgSurfCanvas;
   TPad *fAvgSurfMainPad;
   TPad *fAvgSurfInfoPad;

   RawAnitaHeader *fHeadPtr;
   PrettyAnitaHk *fHkPtr;
   RawAnitaEvent *fRawEventPtr;
   UsefulAnitaEvent *fUsefulEventPtr;
   TurfRate *fTurfPtr;
   SurfHk *fSurfPtr;
   SummedTurfRate *fSumTurfPtr;
   AveragedSurfHk *fAvgSurfPtr;

   TButton *fVertButton;
   TButton *fHorizButton;
   TButton *fBothButton;
   TButton *fWaveformButton;
   TButton *fPowerButton;
   TButton *fSurfButton;

   TButton *fTurfYScaleButton;
   TButton *fSurfSurfViewButton;
   TButton *fSurfPhiViewButton;

   TButton *fSumTurfYScaleButton;
   TButton *fAvgSurfSurfViewButton;
   TButton *fAvgSurfPhiViewButton;

   Int_t fInEventPlayMode;
   Int_t fEventPlaySleepMs;

   WaveCalType::WaveCalType_t fCalType;

};


#endif //MAGICDISPLAY_H
