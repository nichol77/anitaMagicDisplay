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
#include "AnitaConventions.h"

class UsefulAnitaEvent;
class RawAnitaHeader;
class TPad;

class AnitaCanvasMaker 
{
 public:
  Int_t fSetVoltLimits;
  Double_t fMinVoltLimit;
  Double_t fMaxVoltLimit;
  Double_t fMinVertVoltLimit;
  Double_t fMaxVertVoltLimit;
  Int_t fAutoScale;
  Double_t fMinClockVoltLimit;
  Double_t fMaxClockVoltLimit;
  Double_t fMinTimeLimit;
  Double_t fMaxTimeLimit;
  Double_t fMinPowerLimit;
  Double_t fMaxPowerLimit;
  Double_t fMinFreqLimit;
  Double_t fMaxFreqLimit;
  Int_t fPolView;
  Int_t fPowerSpecView;
  Int_t fRedoEventCanvas;
  //Int_t fRedoSurfCanvas;
  Int_t fLastView;
  Int_t fNewEvent;
  

  TPad *getEventViewerCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getHorizontalCanvas(RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getVerticalCanvas(RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getCombinedCanvas(RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getSurfChanCanvas(RawAnitaHeader *hdPtr, TPad *useCan=0);
  
  TPad *quickGetEventViewerCanvasForWebPlottter(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getVerticalCanvasForWebPlotter(RawAnitaHeader *hdPtr, TPad *useCan=0);
  
  
  //   TPad *getHorizontalPhiCanvas(RawAnitaHeader *hdPtr, TPad *useCan=0);
  //  TPad *getVerticalPhiCanvas(RawAnitaHeader *hdPtr, TPad *useCan=0);
  //  TPad *getSurfChanPhiCanvas(RawAnitaHeader *hdPtr, TPad *useCan=0);
  //  TPad *getCombinedPhiCanvas(RawAnitaHeader *hdPtr, TPad *useCan=0);


  TPad *getEventInfoCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  
  void setupPhiPadWithFrames(TPad *plotPad);
  void setupSurfPadWithFrames(TPad *plotPad);
  //void deleteTGraphsFromPad(TPad *paddy);
  void deleteTGraphsFromPad(TPad *paddy,int surf,int chan);
  void deleteTGraphsFromPad(TPad *paddy,int surf,int chan,int chan2);

  AnitaCanvasMaker(WaveCalType::WaveCalType_t calType=WaveCalType::kVoltageTime);
  ~AnitaCanvasMaker();

  //Instance generator
  static AnitaCanvasMaker*  Instance();
  

  WaveCalType::WaveCalType_t fCalType;

 protected:
   static AnitaCanvasMaker *fgInstance;  
   // protect against multiple instances


};


#endif //ANITACANVASMAKER_H
