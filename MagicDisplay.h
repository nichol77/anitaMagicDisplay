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

class TCanvas;

class MagicDisplay 
{
 public:
  
  MagicDisplay(char *baseDir, int run);
  MagicDisplay();
  ~MagicDisplay();

  void startDisplay();
  
  //Instance generator
  static MagicDisplay*  Instance();
  

  //Here are the data managers
  TTree *fHeadTree;
  TChain *fEventTree;
  TTree *fPrettyHkTree;
  
  //And some useful info to keep track of what is where
  Long64_t fEntry;
  

 protected:
   static MagicDisplay *fgInstance;  
   // protect against multiple instances

 private:
   TCanvas *fMagicCanvas; //The main canvas
   TPad *fMagicMainPad;

};


#endif //MAGICDISPLAY_H
