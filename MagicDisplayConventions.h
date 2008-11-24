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
      kPowerSurfOnly, ///< Power spectral density view of all channels arranged by SURF.
      kHilbertEnvelopePhiVerticalOnly, ///< Hilbert envelope view of vertical polarisation arranged by phi.
      kHilbertEnvelopePhiHorizontalOnly, ///< Hilbert envelope view of horizontal polarisation arranged by phi.
      kHilbertEnvelopePhiCombined, ///< Hilbert envelope view of both polarisations arranged by phi.
      kHilbertEnvelopeSurfOnly, ///<  Hilbert envelope view of all channels arranged by SURF.
      kAveragedPowerPhiVerticalOnly, ///< Power spectral density view of vertical polarisation arranged by phi.
      kAveragedPowerPhiHorizontalOnly, ///< Power spectral density view of horizontal polarisation arranged by phi.
      kAveragedPowerPhiCombined, ///< Power spectral density view of both polarisations arranged by phi.
      kAveragedPowerSurfOnly ///< Power spectral density view of all channels arranged by SURF.
   } MagicDisplayOption_t; ///< The event display option enumeration.
};

//!  Canvas layout option
/*!
  A simple enumeration to enumerate the canvas layout options for MagicDisplay
*/
namespace MagicDisplayCanvasLayoutOption {
  typedef enum EMagicDisplayCanvasLayoutOption {
    kPhiVerticalOnly = 0, ///< View of vertical polarisation arranged by phi.
    kPhiHorizontalOnly, ///< View of horizontal polarisation arranged by phi.
    kPhiCombined, ///< View of both polarisations arranged by phi.
    kSurfOnly ///< View of all channels arranged by SURF. 
  } MagicDisplayCanvasLayoutOption_t; ///< Enumeration for canvas layout.
};


//!  Waveform display format option
/*!
  A simple enumeration to enumerate the different waveform display options for MagicDisplay (eg. waveform, PSD (FFT), Hilbert envelope, etc.)
*/
namespace MagicDisplayFormatOption {
  typedef enum EMagicDisplayFormatOption {
    kWaveform = 0, ///< View of waveforms
    kPowerSpectralDensity = 1, ///< View of FFTs
    kFFT = 1, ///< View of FFTs
    kHilbertEnvelope, ///< View of hilbert envelopes
    kAveragedFFT ///< View of avergaed FFTs
  } MagicDisplayFormatOption_t; ///< Enumeration for plot style.
};
    

int getNiceColour(int ind); ///< Simple fucntion to return some of my favourite colours.

#endif //MAGICDISPLAYCONVENTIONS_H
