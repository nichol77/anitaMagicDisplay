//////////////////////////////////////////////////////////////////////////////
/////  AnitaRFCanvasMaker.h        ANITA Canvas Maker                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////                                                                    ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ANITARFCANVASMAKER_H
#define ANITARFCANVASMAKER_H

//Includes
#include <iostream>
#include <TObject.h>
#include <TMath.h>
#include <TVector3.h>

class SurfHk;
class AveragedSurfHk;
class TurfRate;
class SummedTurfRate;
class TPad;

class AnitaRFCanvasMaker 
{
 public:
  AnitaRFCanvasMaker();
  ~AnitaRFCanvasMaker();

  //Instance generator
  static AnitaRFCanvasMaker*  Instance();
  TPad *getTurfRateCanvas(TurfRate *turfPtr, TPad *useCan=0);
  TPad *getTurfInfoCanvas(TurfRate *turfPtr,TPad *useCan);
  TPad *getSumTurfRateCanvas(SummedTurfRate *sumTurfPtr, TPad *useCan=0);
  TPad *getSumTurfInfoCanvas(SummedTurfRate *sumTurfPtr, TPad *useCan);
  TPad *getSurfHkCanvas(SurfHk *surfPtr, TPad *useCan=0);
  TPad *getSurfInfoCanvas(SurfHk *surfPtr,TPad *useCan);
  TPad *getAvgSurfHkCanvas(AveragedSurfHk *avgSurfPtr, TPad *useCan=0);
  TPad *getAvgSurfInfoCanvas(AveragedSurfHk *avgSurfPtr,TPad *useCan);

  void setLogFlag(Int_t logFlag)
     {fUseLog=logFlag;}

  Int_t fTurfL1Max;
  Int_t fTurfL1Min;
  Int_t fTurfL2Max;
  Int_t fTurfL2Min;
  Int_t fTurfL3Max;
  Int_t fTurfL3Min;
  Int_t fSumTurfL1Max;
  Int_t fSumTurfL1Min;
  Int_t fSumTurfL2Max;
  Int_t fSumTurfL2Min;
  Int_t fSumTurfL3Max;
  Int_t fSumTurfL3Min;
  Int_t fUseLog;
  Int_t fFixTurfYScale;
  Int_t fFixSurfYScale;
  Int_t fSurfPhiView;
  Int_t fFixAvgSurfYScale;
  Int_t fAvgSurfPhiView;
  Int_t fFixSumTurfYScale;
  
  

 protected:
   static AnitaRFCanvasMaker *fgInstance;  
   void getSurfHkSurfCanvas(SurfHk *surfPtr,TPad *plotPad);
   void getSurfHkPhiCanvas(SurfHk *surfPtr,TPad *plotPad);
   void getAvgSurfHkSurfCanvas(AveragedSurfHk *avgSurfPtr,TPad *plotPad);
   void getAvgSurfHkPhiCanvas(AveragedSurfHk *avgSurfPtr,TPad *plotPad);
   // protect against multiple instances


};


#endif //ANITARFCANVASMAKER_H
