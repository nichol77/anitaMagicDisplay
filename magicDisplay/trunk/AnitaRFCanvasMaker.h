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

#include "AnitaConventions.h"

class SurfHk;
class AveragedSurfHk;
class TurfRate;
class SummedTurfRate;
class TPad;

#define MAX_SURF_HK_TIME_POINTS 1000

//!  The SURF and TURF hk display canvas maker.
/*!
  This is where all the heavy lifting regarding the SURF hk and TURF rates.
*/
class AnitaRFCanvasMaker 
{
 public:
   AnitaRFCanvasMaker(); ///<Constructor
   ~AnitaRFCanvasMaker(); ///<Destructor

  //Instance generator
   static AnitaRFCanvasMaker*  Instance(); ///<Instance generator
   TPad *getTurfRateCanvas(TurfRate *turfPtr, TPad *useCan=0); ///<Returns the TurfRate histogram canvas, the canvas can be provided in <i>useCan</i>
  TPad *getTurfInfoCanvas(TurfRate *turfPtr,TPad *useCan); ///<Returns the TurfRate info canvas, the canvas can be provided in <i>useCan</i>
  TPad *getSumTurfRateCanvas(SummedTurfRate *sumTurfPtr, TPad *useCan=0); ///<Returns the summed TURF rate histogram canvas, the canvas can be provided in <i>useCan</i>
  TPad *getSumTurfInfoCanvas(SummedTurfRate *sumTurfPtr, TPad *useCan); ///<Returns the summed TURF Rate info canvas, the canvas can be provided in <i>useCan</i>
  TPad *getSurfHkCanvas(SurfHk *surfPtr, TPad *useCan=0); ///<Returns the SURF hk histogram canvas, the canvas can be provided in <i>useCan</i>
  TPad *getSurfInfoCanvas(SurfHk *surfPtr,TPad *useCan); ///<Returns the SURF hk info canvas, the canvas can be provided in <i>useCan</i>
  TPad *getAvgSurfHkCanvas(AveragedSurfHk *avgSurfPtr, TPad *useCan=0); ///<Returns the averaged SURF hk histogram canvas, the canvas can be provided in <i>useCan</i>
  TPad *getAvgSurfInfoCanvas(AveragedSurfHk *avgSurfPtr,TPad *useCan); ///<Returns the averaged SURF hk info canvas, the canvas can be provided in <i>useCan</i>

  void setSurfLogFlag(Int_t logFlag)
     {fSurfUseLog=logFlag;} ///< Log or linear scale?
  void setAvgSurfLogFlag(Int_t logFlag)
     {fAvgSurfUseLog=logFlag;} ///< Log or linear scale?
  
  void setSurfHkDisplay(MagicDisplaySurfHkDisplay::MagicDisplaySurfHkDisplay_t surfOpt) {fSurfDisplay=surfOpt;} ///<Phi, surf or time

  Int_t fTurfL1Max; ///< Maximum TURF L1
  Int_t fTurfL1Min; ///< Minimum TURF L1
  Int_t fTurfL2Max; ///< Maximum TURF L2
  Int_t fTurfL2Min; ///< Minimum TURF L2
  Int_t fTurfL3Max; ///< Maximum TURF L3
  Int_t fTurfL3Min; ///< Minimum TURF L3
  Int_t fSumTurfL1Max; ///< Maximum summed TURF L1
  Int_t fSumTurfL1Min; ///< Minimum summed TURF L1
  Int_t fSumTurfL2Max; ///< Maximum summed TURF L2
  Int_t fSumTurfL2Min; ///< Minimum summed TURF L2
  Int_t fSumTurfL3Max; ///< Maximum summed TURF L3
  Int_t fSumTurfL3Min; ///< Minimum summed TURF L3
  Int_t fSurfUseLog; ///< Log or linear?
  Int_t fAvgSurfUseLog; ///< Log or linear?

  Int_t fFixTurfYScale; ///< Fixed or autoscaling TURF
  Int_t fFixSurfYScale; ///< Fixed or autoscaling SURF

/*   Int_t fSurfTimeView; ///< Time view enabled? */
/*   Int_t fSurfPhiView; ///< Phi or SURF view */
  Int_t fSurfKelvinView; ///< ADC or Kelvin view

  Int_t fFixSumTurfYScale; ///< Fixed or autoscaling summed TURF.

  Int_t fFixAvgSurfYScale; ///< Fixed or autoscaling averaged SURF
  Int_t fAvgSurfPhiView; ///< Phi or SURF view (for Averaged SURF hk)
  Int_t fAvgSurfKelvinView; ///< ADC or Kelvin view
  
  MagicDisplaySurfHkDisplay::MagicDisplaySurfHkDisplay_t fSurfDisplay;

 protected:
  static AnitaRFCanvasMaker *fgInstance; ///< Protect against multiple instances.
  void getSurfHkSurfCanvas(SurfHk *surfPtr,TPad *plotPad); ///< Worker function
  void getSurfHkPhiCanvas(SurfHk *surfPtr,TPad *plotPad); ///< Worker function
  void getSurfHkTimeCanvas(TPad *plotPad); ///< Worker function
  void getAvgSurfHkSurfCanvas(AveragedSurfHk *avgSurfPtr,TPad *plotPad); ///< Worker function
  void getAvgSurfHkPhiCanvas(AveragedSurfHk *avgSurfPtr,TPad *plotPad); ///< Worker function
  void addToTimePlots(SurfHk *surfPtr);
  
  Int_t  fNumSurfHks;
  Double_t fScalerVals[NUM_SURF][SCALERS_PER_SURF][MAX_SURF_HK_TIME_POINTS];
  Double_t fThreshVals[NUM_SURF][SCALERS_PER_SURF][MAX_SURF_HK_TIME_POINTS];
  Double_t fRfPowVals[NUM_SURF][RFCHAN_PER_SURF][MAX_SURF_HK_TIME_POINTS];
  Double_t fTimeVals[MAX_SURF_HK_TIME_POINTS];



};


#endif //ANITARFCANVASMAKER_H
