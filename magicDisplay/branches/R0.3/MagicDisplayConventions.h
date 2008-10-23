//////////////////////////////////////////////////////////////////////////////
/////  MagicDisplayConventions.h        Magic Display Conventions        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A handy file full of enumerations and the like identifying     /////
/////     the conventions we use in Magic Display                        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////


#ifndef MAGICDISPLAYCONVENTIONS_H
#define MAGICDISPLAYCONVENTIONS_H

#ifndef ROOT_Rtypes
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Rtypes.h"
#endif
#endif



//!  Waveform display enumeration
/*!
  A simple enumeration to enumerate the different waveform/psd display options for the event data.
*/
namespace MagicDisplayOption {
   typedef enum EMagicDisplayOption {
      kWavePhiVerticalOnly = 0, ///< Waveform view of vertical polarisation arranged by phi.
      kWavePhiHorizontalOnly, ///< Waveform view of horizontal polarisation arranged by phi.
      kWavePhiCombined, ///< Waveform view of both polarisations arranged by phi.
      kWaveSurfOnly, ///< Waveform view of all channels arranged by SURF.
      kPowerPhiVerticalOnly, ///< Power spectral density view of vertical polarisation arranged by phi.
      kPowerPhiHorizontalOnly, ///< Power spectral density view of horizontal polarisation arranged by phi.
      kPowerPhiCombined, ///< Power spectral density view of both polarisations arranged by phi.
      kPowerSurfOnly ///< Power spectral density view of all channels arranged by SURF.
   } MagicDisplayOption_t; ///< The event display option enumeration.
};

int getNiceColour(int ind); ///< Simple fucntion to return some of my favourite colours.

#endif //MAGICDISPLAYCONVENTIONS_H
