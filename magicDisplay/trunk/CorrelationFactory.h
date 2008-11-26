//////////////////////////////////////////////////////////////////////////////
/////  CorrelationFactory.h       The correlation factory                /////
/////                                                                    /////
/////  Description:                                                      /////
/////                                                                    ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef CORRELATIONFACTORY_H
#define CORRELATIONFACTORY_H

//Includes
#include <iostream>
#include <TObject.h>
#include <TMath.h>
#include <TVector3.h>

class WaveformGraph;


//!  The correlation factory which takes two waveforms and correlates them
/*!
  It's a factory the produces correlations.
*/
class CorrelationFactory 
{
 public:
   CorrelationFactory(); ///<Constructor
   ~CorrelationFactory(); ///<Destructor

  //Instance generator
   static CorrelationFactory*  Instance(); ///<Instance generator
 
   void addWaveformToCorrelation(WaveformGraph *inputWave); ///< Adds a a waveform to the correlation

   void setCorrelationDeltaT(Double_t deltaT) {fDeltaT=deltaT;} ///< Sets the deltaT for the correlation.
   Double_t getCorrelationDeltaT() { return fDeltaT;} ///<Returns the deltaT used in correlation.


   WaveformGraph *fGrWave[2]; ///<The input WaveformGraph's
   Double_t fDeltaT; ///< The deltaT used in the correlation.

 protected:
   static CorrelationFactory *fgInstance; ///< Protect against multiple instances.
 
};


#endif //CORRELATIONFACTORY_H
