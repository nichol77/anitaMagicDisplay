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
  Double_t fMinClockVoltLimit;
  Double_t fMaxClockVoltLimit;
  Double_t fMinTimeLimit;
  Double_t fMaxTimeLimit;
  Int_t fLastView;
  

  TPad *getHorizontalCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getVerticalCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getCombinedCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getSurfChanCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  
   TPad *getHorizontalPhiCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getVerticalPhiCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getSurfChanPhiCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getCombinedPhiCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);


  TPad *getEventInfoCanvas(RawAnitaHeader *hdPtr, TPad *useCan=0);
  
  void setupPhiPadWithFrames(TPad *plotPad);
  void setupSurfPadWithFrames(TPad *plotPad);
  void deleteTGraphsFromPad(TPad *paddy);

  AnitaCanvasMaker();
  ~AnitaCanvasMaker();

  //Instance generator
  static AnitaCanvasMaker*  Instance();
  

 protected:
   static AnitaCanvasMaker *fgInstance;  
   // protect against multiple instances


};


#endif //ANITACANVASMAKER_H
