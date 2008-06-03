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

class TCanvas;
class TPad;
class RawAnitaHeader;
class PrettyAnitaHk;
class RawAnitaEvent;
class UsefulAnitaEvent;
class TurfRate;
class SurfHk;
class TButton;


class MagicDisplay 
{
 public:
  
  MagicDisplay(char *baseDir, int run);
  MagicDisplay();
  ~MagicDisplay();

  void startEventDisplay();
  int displayNextEvent();
  int displayPreviousEvent();
  void refreshEventDisplay();
  int getEventEntry();
  void drawEventButtons();
  void setMainCanvasOption(MagicDisplayOption::MagicDisplayOption_t option); 

  void startTurfDisplay();
  int displayNextTurf();
  int displayPreviousTurf();
  void refreshTurfDisplay();
  int getTurfEntry();
  void drawTurfButtons();
  void toggleTurfYScale();

  void startSurfDisplay();
  int displayNextSurf();
  int displayPreviousSurf();
  void refreshSurfDisplay();
  int getSurfEntry();
  void drawSurfButtons();
  void toggleSurfSurfView(Int_t surfView);

  //Instance generator
  static MagicDisplay*  Instance();
  

  //Here are the data managers
  TTree *fHeadTree;
  TChain *fEventTree;
  TTree *fPrettyHkTree;
  TTree *fTurfRateTree;
  TTree *fSurfHkTree;
  
  //And some useful info to keep track of what is where
  Long64_t fEventEntry;
  Long64_t fPrettyHkEntry;
  Long64_t fTurfRateEntry;
  Long64_t fSurfHkEntry;
  
  Int_t fCurrentRun;
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
   TCanvas *fSurfCanvas;
   TPad *fSurfMainPad;
   TPad *fSurfInfoPad;
   RawAnitaHeader *fHeadPtr;
   PrettyAnitaHk *fHkPtr;
   RawAnitaEvent *fRawEventPtr;
   UsefulAnitaEvent *fUsefulEventPtr;
   TurfRate *fTurfPtr;
   SurfHk *fSurfPtr;
   TButton *fVertButton;
   TButton *fHorizButton;
   TButton *fBothButton;
   TButton *fTurfYScaleButton;
   TButton *fSurfSurfViewButton;
   TButton *fSurfPhiViewButton;

};


#endif //MAGICDISPLAY_H
