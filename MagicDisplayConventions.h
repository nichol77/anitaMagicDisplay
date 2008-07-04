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




namespace MagicDisplayOption {
   typedef enum EMagicDisplayOption {
      kWavePhiVerticalOnly = 0,
      kWavePhiHorizontalOnly,
      kWavePhiCombined,
      kWaveSurfOnly,
      kPowerPhiVerticalOnly,
      kPowerPhiHorizontalOnly,
      kPowerPhiCombined,
      kPowerSurfOnly
   } MagicDisplayOption_t;
};

int getNiceColour(int ind);

#endif //MAGICDISPLAYCONVENTIONS_H
