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
class TurfRate;
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
  TPad *getSurfHkCanvas(SurfHk *surfPtr, TPad *useCan=0);
  TPad *getSurfInfoCanvas(SurfHk *surfPtr,TPad *useCan);
  void setLogFlag(Int_t logFlag)
     {fUseLog=logFlag;}

  Int_t fL1Max;
  Int_t fL1Min;
  Int_t fL2Max;
  Int_t fL2Min;
  Int_t fL3Max;
  Int_t fL3Min;
  Int_t fUseLog;
  
  

 protected:
   static AnitaRFCanvasMaker *fgInstance;  
   // protect against multiple instances


};


#endif //ANITARFCANVASMAKER_H
