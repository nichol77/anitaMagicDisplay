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

//!  Canvas layout option
/*!
  A simple enumeration to enumerate the canvas layout options for MagicDisplay
*/
namespace MagicDisplayCanvasLayoutOption {
  typedef enum EMagicDisplayCanvasLayoutOption {
    kPhiVerticalOnly = 0, ///< View of vertical polarisation arranged by phi.
    kPhiHorizontalOnly, ///< View of horizontal polarisation arranged by phi.
    kPhiCombined, ///< View of both polarisations arranged by phi.
    kSurfOnly, ///< View of all channels arranged by SURF. 
    kPayloadView, ///< View of Jiwoo's pretty 3-d payload
    kInterferometry, 
    kUCorrelator
  } MagicDisplayCanvasLayoutOption_t; ///< Enumeration for canvas layout.

  static const char * toString(MagicDisplayCanvasLayoutOption_t option)
  {
    return option == kPhiVerticalOnly ? "vpol" : 
           option == kPhiHorizontalOnly ? "hpol" : 
           option == kPhiCombined ? "allpol" : 
           option == kSurfOnly ? "surf" : 
           option == kPayloadView ? "payload" : 
           option == kInterferometry ? "int" : 
           option == kInterferometry ? "UCorr"  : 
           0; 
  }
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

  static const char * toString(MagicDisplayFormatOption_t opt)
  {
    return opt == kWaveform ? "wf" : 
           opt == kPowerSpectralDensity ? "psd" : 
           opt == kFFT ? "psd" : 
           opt == kHilbertEnvelope ? "hilbert" : 
           opt == kAveragedFFT ? "avgpsd" : 
           0;
  }
};

//!  SURF hk display format option
/*!
  A simple enumeration to enumerate the different ways to display the SURF hk data.
*/
namespace MagicDisplaySurfHkDisplay {
  typedef enum EMagicDisplaySurfHkDisplay {
    kSurfView = 0,
    kPhiView =  1,
    kTimeView = 2,
    kNotAView = 3
  } MagicDisplaySurfHkDisplay_t; ///< Enumeration for plot style
};
    
//!  GPS display format option
/*!
  A simple enumeration to enumerate the different ways to display the GPS data.
*/
namespace MagicDisplayGpsDisplay {
  typedef enum EMagicDisplayGpsDisplay {
    kMapView = 0,
    kSatView =  1,
    kTimeView = 2,
    kNotAView = 3
  } MagicDisplayGpsDisplay_t; ///< Enumeration for plot style
};

namespace MagicDisplayFileType {
   typedef enum EMagicDisplayFileType {
      kRawEvent = 0,
      kCalEvent = 1,
      kMcEvent = 2
   } MagicDisplayFileType_t; ///<Enumeration for file kind
};

int getNiceColour(int ind); ///< Simple fucntion to return some of my favourite colours.
int getMarker(int ind);///< Returns a maker number

#endif //MAGICDISPLAYCONVENTIONS_H
