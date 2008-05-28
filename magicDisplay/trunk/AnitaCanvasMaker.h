//////////////////////////////////////////////////////////////////////////////
/////  AnitaCanvasMaker.h        ANITA Canvas Maker                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////                                                                    ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ANITACANVASMAKER_H
#define ANITACANVASMAKER_H

//Includes
#include <iostream>
#include <TObject.h>
#include <TMath.h>
#include <TVector3.h>

class UsefulAnitaEvent;
class RawAnitaHeader;
class TPad;

class AnitaCanvasMaker 
{
 public:
  Int_t fSetVoltLimits;
  Double_t fMinVoltLimit;
  Double_t fMaxVoltLimit;

  TPad *getHorizontalCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getVerticalCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getSurfChanCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getCombinedCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);


  AnitaCanvasMaker();
  ~AnitaCanvasMaker();

  //Instance generator
  static AnitaCanvasMaker*  Instance();
  

 protected:
   static AnitaCanvasMaker *fgInstance;  
   // protect against multiple instances


};


#endif //ANITACANVASMAKER_H
