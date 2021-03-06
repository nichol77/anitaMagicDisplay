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
#include "MagicDisplayConventions.h"
#include "AnalysisReco.h"

class UsefulAnitaEvent;
class RawAnitaHeader;
class FilteredAnitaEvent;
class TPad;
class TFile;
class TGeoManager;

//!  The event display canvas maker.
/*!
  This is where all the heavy lifting regarding the main event display canvas takes place. Lots of things are hard-coded and unchangeable in order to make the display look reasonable (on my computer at least). 
*/
class AnitaCanvasMaker 
{
 public:

  void setCanvasLayout(MagicDisplayCanvasLayoutOption::MagicDisplayCanvasLayoutOption_t canLay) {fCanvasView=canLay;}
  void setWaveformFormat(MagicDisplayFormatOption::MagicDisplayFormatOption_t waveOption) {fWaveformOption=waveOption;}
  // void setInterferometryTypeFlags(Acclaim::AnalysisReco::mapMode_t mapMode, Acclaim::AnalysisReco::zoomMode_t zoomMode){
  //   fInterferometryMapMode=mapMode;
  //   fInterferometryZoomMode=zoomMode;
  // }
  
  void resetAverage();

   Double_t fMinVoltLimit; ///< The minimum voltage. 
   Double_t fMaxVoltLimit; ///< The maximum voltage.
   Double_t fMinInterfLimit; ///< The minimum interferometry. 
   Double_t fMaxInterfLimit; ///< The maximum interferometry.
   Double_t fMinVertVoltLimit; ///< The minimum voltage in vertical channels.
   Double_t fMaxVertVoltLimit; ///< The maximum voltage in vertical channels.
   Int_t fAutoScale; ///< Fixed or auto-scaling y-axis?
   Int_t fAutoScaleNeg; ///< Fixed or auto-scaling y-axis?
   Double_t fMinClockVoltLimit; ///< The minimum voltage in the clock channels.
   Double_t fMaxClockVoltLimit; ///< The maximum voltage in the clock channels.
   Double_t fMinTimeLimit; ///< The minimum time in the waveform (typically 0).
   Double_t fMaxTimeLimit; ///< The maximum time in the waveform (typically 100).
   Double_t fMinPowerLimit; ///< The minimum power in the PSDs.
   Double_t fMaxPowerLimit; ///< The maximum power in the PSDs.
   Double_t fMinFreqLimit; ///< The minimum frequency in the PSDs (typically 0).
   Double_t fMaxFreqLimit; ///< The maximum frequency in the PSDs (typically 1200).
   MagicDisplayCanvasLayoutOption::MagicDisplayCanvasLayoutOption_t fCanvasView; ///< Which canvas layout do we want?
   MagicDisplayFormatOption::MagicDisplayFormatOption_t fWaveformOption; ///< Are we viewing PSDs, waveforms or other stuff?
   Int_t fRedoEventCanvas; ///< Do we neeed to redraw the event canvas (eg. switching from phi to SURF)?
   //Int_t fRedoSurfCanvas;
   MagicDisplayCanvasLayoutOption::MagicDisplayCanvasLayoutOption_t fLastCanvasView; ///< What was the last view (phi or SURF)?
   MagicDisplayFormatOption::MagicDisplayFormatOption_t fLastWaveformFormat; ///< What did we plot last time??
   Int_t fNewEvent; ///< Is this a new event?

  // Acclaim::AnalysisReco::mapMode_t fInterferometryMapMode;
  // Acclaim::AnalysisReco::mapMode_t fLastInterferometryMapMode;
  // Acclaim::AnalysisReco::zoomMode_t fInterferometryZoomMode;	  
  // Acclaim::AnalysisReco::zoomMode_t fLastInterferometryZoomMode;
  
   //!  The main event view canvas getter.
   /*!
     Actually draws all the little squiggly lines.
     /param evPtr Pointer to the UsefulAnitaEvent we want to draw.
     /param hdPtr Pointer to the header of the event we want to draw
     /param pat Pointer to the Adu5Pat matched most closely in time to the event we want to draw
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   // TPad *getEventViewerCanvas(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan=0);
  TPad *getEventViewerCanvas(FilteredAnitaEvent* fEv, TPad *useCan=0, bool forceRedo = false);
   //!  The interferometry canvas getter.
   /*!
     Puts histograms from the Cross Correlator onto the display
     /param evPtr Pointer to the UsefulAnitaEvent we want to draw.
     /param hdPtr Pointer to the header of the event we want to draw
     /param useCan Optional pointer to a canvas to draw in (if zero a canvas will be created).
   */
  TPad *getInterferometryCanvas(const RawAnitaHeader *hdPtr,TPad *useCan=NULL);
  TPad *getUCorrelatorCanvas(const RawAnitaHeader *hdPtr,TPad *useCan=NULL);


  //!  The main event view canvas getter used by QnDWBOM.
   /*!
     Actually draws all the little squiggly lines.
     /param evPtr Pointer to the UsefulAnitaEvent we want to draw.
     /param hdPtr Pointer to the header of the event we want to draw
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *quickGetEventViewerCanvasForWebPlottter(const UsefulAnitaEvent *evPtr, const RawAnitaHeader *hdPtr, TPad *useCan=0);
   
  
  

   //!  The event info canvas getter.
   /*!
     Fills in run number and times and all those good things.
     /param hdPtr Pointer to the header of the event we want to draw
     /param useCan Optional pointer to a canvas to draw all the TPaveText's (if zero a canvas will be created).
   */
   TPad *getEventInfoCanvas(const UsefulAnitaEvent *evPtr, const RawAnitaHeader *hdPtr, const Adu5Pat *pat, TPad *useCan=0);
  
   
   void setupPhiPadWithFrames(TPad *plotPad); ///< Worker function to setup the phi view canvas.
   void setupInterfPadWithFrames(TPad *plotPad); ///< Worker function to setup the phi view canvas.  
   void setupSurfPadWithFrames(TPad *plotPad); ///< Worker function to setup the surf view canvas.
   void setupPayloadViewWithFrames(TPad *plotPad); ///< Worker function to setup the payload view canvas.
   //void deleteTGraphsFromPad(TPad *paddy); ///< Deprecated.
   void deleteTGraphsFromPad(TPad *paddy,int surf,int chan); ///< Worker fucntion to delete a specific graph from a pad.
   void deleteTGraphsFromPad(TPad *paddy,int surf,int chan,int chan2); ///< Worker fucntion to delete specific graphs from a pad.
   
   //!  The assignment constructor.
   /*!
     Creates an AnitaCanvasMaker object/
     /param calType The calibration type used (only needed to set the time axis for ns vs samples).
   */
   AnitaCanvasMaker(WaveCalType::WaveCalType_t calType=WaveCalType::kVoltageTime);
   ~AnitaCanvasMaker(); ///<Destructor.


  Acclaim::AnalysisReco& getAnalysisReco(); ///< get command line access with a reference

  
   //Instance generator
   static AnitaCanvasMaker*  Instance(); ///< The instance generator.
   
   void setPassBandFilterFlag( int flag) { fPassBandFilter=flag;} ///<Flags the pass band filter on or off
   void setNotchFilterFlag( int flag) { fNotchFilter=flag;} ///<Flags the notch filter on or off
   void setPassBandLimits(Double_t low, Double_t high)
   { fLowPassEdge=low; fHighPassEdge=high;} ///<Sets the limits of the pass band (200-1200) in MHz
   void setNotchBandLimits(Double_t low, Double_t high)
   { fLowNotchEdge=low; fHighNotchEdge=high;} ///<Sets the limits of the notched band (235-500) in MHz


  
  
   WaveCalType::WaveCalType_t fCalType; ///< The calibration type.
   
 protected:
   static AnitaCanvasMaker *fgInstance; ///< To protect against multiple instances.

 private:

  static Acclaim::AnalysisReco* reco; ///< Might return a reference to this to allow interaction but not deletion
  
   Int_t fPhiMax;
   TFile *fAnitaGeomFile;
   TGeoManager *fAnitaGeomManager;
   Int_t fPassBandFilter; ///< Whether or not to pass band filter the interpolated waves;
   Int_t fNotchFilter; ///< Whether or not to notch filter;
   Double_t fLowPassEdge; ///< The lower edge of the pass band
   Double_t fHighPassEdge; ///< The higher edge of the pass band
   Double_t fLowNotchEdge; ///< The lower edge of the notch band
   Double_t fHighNotchEdge; ///< The higher edge of the notch band
   //!  A worker function to draw the horizontal canvas -- shouldn't be called directly.
   /*!
     /param hdPtr Pointer to the header of the event we want to draw.
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getHorizontalCanvas(const RawAnitaHeader *hdPtr, TPad *useCan=0);
   //!  A worker function to draw the vertical canvas -- shouldn't be called directly.
   /*!
     /param hdPtr Pointer to the header of the event we want to draw.
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getVerticalCanvas(const RawAnitaHeader *hdPtr, TPad *useCan=0);
   //!  A worker function to draw the combined canvas -- shouldn't be called directly.
   /*!
     /param hdPtr Pointer to the header of the event we want to draw.
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getCombinedCanvas(const RawAnitaHeader *hdPtr, TPad *useCan=0);
   //!  A worker function to draw the SURF canvas -- shouldn't be called directly.
   /*!
     /param hdPtr Pointer to the header of the event we want to draw.
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getSurfChanCanvas(const RawAnitaHeader *hdPtr, TPad *useCan=0);
   //!  A worker function to draw the vertical canvas for QnDWBOM -- shouldn't be called directly.
   /*!
     /param hdPtr Pointer to the header of the event we want to draw.
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getVerticalCanvasForWebPlotter(const RawAnitaHeader *hdPtr, TPad *useCan=0);  //!  A worker function to draw the vertical canvas for QnDWBOM -- shouldn't be called directly.
   /*!
     /param hdPtr Pointer to the header of the event we want to draw.
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getCombinedCanvasForWebPlotter(const RawAnitaHeader *hdPtr, TPad *useCan=0);

   //!  A worker function to draw the Payload view canvas -- shouldn't be called directly.
   /*!
     /param hdPtr Pointer to the header of the event we want to draw.
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getPayloadCanvas(const RawAnitaHeader *hdPtr, TPad *useCan=0);
   void loadPayloadViewSutff(); ///<Loads the anita geometry
};


#endif //ANITACANVASMAKER_H
