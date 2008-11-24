//////////////////////////////////////////////////////////////////////////////
/////  MagicDisplay.h        MagicDisplay                                /////
/////                                                                    /////
/////  Description:                                                      /////
/////      The Marvellous ANITA Graphical Interface and Classy Display   /////
/////     (Magic Display) is a simple event display for ANITA. It's not  /////
/////     nearly as all singing and dancing as Ped's display, but it     /////
/////     does provide a convenient interface to the data and it has     /////
/////     some FFT etc. capabilities.                                    ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef MAGICDISPLAY_H
#define MAGICDISPLAY_H

//Includes
#include "TChain.h"
#include "MagicDisplayConventions.h"
#include "AnitaConventions.h"

class TCanvas;
class TPad;
class RawAnitaHeader;
class PrettyAnitaHk;
class RawAnitaEvent;
class UsefulAnitaEvent;
class TurfRate;
class SummedTurfRate;
class AveragedSurfHk;
class SurfHk;
class TButton;
class TTreeIndex;
class TFile;

//!  The Marvellous ANITA Graphical Interface and Class Display (Magic Display)
/*!
  MagicDisplay is the main class that controls the display. This class is responsible for opening files and reading trees and owns the various canvases and that sort of thing.

Typically one starts MagicDisplay by giving the constructor the base directory that contains the runXXXX directories and the desired start run and the desired calibration type (see <A HREF="/uhen/anita/eventReader/">the event reader documentation for the different available calibration options</A>).

Most of the functions are called by pressig one of the buttons on the display. They can, of course, be called directly through MagicDisplay::Instance()->functionName().
*/
class MagicDisplay 
{
 public:
  
 //! The assignment constructor. Most MagicDisplay sessions start with a call to this.
  /*!
    \param baseDir The directory that contains the runXXXX directories (that contain the ROOT files
    \param run The run number to start with
    \param calType The calibration option desired (see <a HREF="/uhen/anita/eventReader/">the event reader documentation for the different available calibration options</A>)
  */
  MagicDisplay(char *baseDir, int run, WaveCalType::WaveCalType_t calType=WaveCalType::kVoltageTime);
  MagicDisplay(); ///< Default constructor
  ~MagicDisplay(); ///< Destructor

  //Control Panel Functions
  void startControlPanel(); ///<Starts the (currently highly buggy) control panel

  Int_t getCurrentRun() 
  {return fCurrentRun;} ///< Returns the current run number
  UInt_t getCurrentEvent(); ///< Returns the current event number

  void startEventPlaying(); ///< Starts event playback mode, normally called by pressing the "Play" button the display.
  void startEventPlayingReverse(); ///< Starts the event playback in reverse mode, normally called by pressing the "Rev" button on the main display.
  void stopEventPlaying(); ///< Stops the event playback, normally called by pressing the "Stop" button on the main display.

  void closeCurrentRun(); ///< Closes all currently opened run TFiles, in preparation for changing to a new run.

  int loadEventTree(); ///< Loads the event tree corresponding to the current run number <i>fCurrentRun</i>
  void startEventDisplay(); ///< Starts the main event display. This is typically the first thing that gets done after creating a MagicDisplay object in the runMagicDisplay macro.
  //! Displays a specified event number (from the specified run number). This is an increddibly useful and well hidden function. From the ROOT prompt you can call this by MagicDisplay::Instance()->displayThisEvent(10001,3000);
  /*!
    \param eventNumber The event number to display
    \param runNumber The run which contains the desired event
    \return Zero on success
  */
  int displayThisEvent(UInt_t eventNumber, UInt_t runNumber); 
  int displayNextEvent(); ///< Displays the next event in the file, normally called by pressing the "Next" button.
  int displayFirstEvent(); ///< Displays the first event in the file, normally called by pressing the "First" button.
  int displayLastEvent(); ///< Displays the last event in the file, normally called by pressing the "Last" button.
  int displayPreviousEvent(); ///< Displays the previous event in the file, normally called by pressing the "Previous" button.
  void refreshEventDisplay(); ///< Refresh the event display and redraw the graphs, this is called everytime a new event is displayed.
  int getEventEntry(); ///< Tries to retrieve the event corresponding to entry <i>fEventEntry</i> from the evnt file. Returns zero on success.
  void drawEventButtons(); ///< Worker function to draw the buttons on the main event display canvas.
  //! Toggles between waveform and FFT view modes
  /*!
    \param option See MagicDisplayCanvasLayoutOption for options.
  */
  void setCanvasLayout(MagicDisplayCanvasLayoutOption::MagicDisplayCanvasLayoutOption_t option); ///< Sets the drawing option for the event display see MagicDisplayCanvasLayoutOption for available options.
  //! Toggles between waveform and FFT view modes
  /*!
    \param waveformView See MagicDisplayFormatOption for options.
  */
  void setWaveformFormat(MagicDisplayFormatOption::MagicDisplayFormatOption_t waveformView); 

  int loadTurfTree(); ///< Opens the TURF rate file for the current run.
  void startTurfDisplay(); ///< Starts the TURF Rate display window.
  int displayNextTurf(); ///< Displays the next event in the TURF tree.
  int displayPreviousTurf(); ///< Displays the previous event in the TURF tree.
  void refreshTurfDisplay(); ///< Refreshs the TURF rate display. 
  int getTurfEntry(); ///< Attempts to load the entry corresponding to <i>fTurfRateEntry</i>.
  void drawTurfButtons(); ///< Draws the buttons on the TURF rate display
  void toggleTurfYScale(); ///< Toggles betweeen fixed and auto-scaling y-axis pn the TURF rate display.
 
  int loadSurfTree(); ///< Opens the SURF hk file for the current run.
  void startSurfDisplay(); ///< Starts the SURF Hk display window.
  int displayNextSurf(); ///< Displays the next SURF hk entry.
  int displayPreviousSurf(); ///< Displays the previous SURF hk entry.
  void refreshSurfDisplay(); ///< Refreshes the SURF Hk display.
  int getSurfEntry(); ///< Attempts to load the entry corresponding to <i>fSurfHkEntry</i>.
  void drawSurfButtons(); ///< Draws the buttons on the SURF hk display.
  void toggleSurfSurfView(Int_t surfView); ///< Toggles between surf and phi views.

  int loadAvgSurfTree(); ///< Opens the averaged SURF hk file for the current rune.
  void startAvgSurfDisplay(); ///< Starts the averaged SURF hk display window.
  int displayNextAvgSurf(); ///< Displays the next entry in the averaged SURF hk tree.
  int displayPreviousAvgSurf(); ///< Displays the previous entry in the averaged SURF hk tree.
  void refreshAvgSurfDisplay(); ///< Refreshes the averaged SURF hk display.
  int getAvgSurfEntry(); ///< Attempts to load get the entry corresponding to <i>fAvgSurfHkEntry</i>.
  void drawAvgSurfButtons(); ///< Draws the buttons on the averaged SURF hk display.
  void toggleAvgSurfSurfView(Int_t surfView); ///< Toggles between surf and phi views.

  int loadSumTurfTree(); ///< Opens the summed TURF rate file for the current run.
  void startSumTurfDisplay(); ///< Starts the summed TURF rate display program.
  int displayNextSumTurf(); ///< Displays the next summed TURF rate entry in the tree.
  int displayPreviousSumTurf();///< Displays the previous summed TURF rate entry in the tree.
  void refreshSumTurfDisplay(); ///< Refreshes the summed TURF rate display.
  int getSumTurfEntry(); ///< Attempts to get the entry corresponding to <i>fSumTurfRateFile</i>.
  void drawSumTurfButtons(); ///< Draws the buttons on the summed TURF rate display.
  void toggleSumTurfYScale(); ///< Toggles between fixed and auto-scaling y-axis on the summed TURF display.


  //! Returns a pointer to the active MagicDisplay. This is very useful if you want to access the TTree's directly or if you want to explicitly call one of the methods.
  /*!
    \return A pointer to the active MagicDisplay.

  */
  static MagicDisplay*  Instance();
  

  TFile *fHeadFile; ///< A pointer to the current header file.
  TFile *fEventFile; ///< A pointer to the current event file.
  TFile *fTurfRateFile; ///< A pointer to the current TURF rate file.
  TFile *fSumTurfRateFile; ///< A pointer to the current summed TURF rate file.
  TFile *fSurfHkFile; ///< A pointer to the current SURF hk file.
  TFile *fAvgSurfHkFile; ///< A pointer to the current averaged SURF hk file.

  //Here are the data managers
  TTree *fHeadTree; ///< A pointer to the current header tree.
  TChain *fEventTree; ///< A pointer to the current event tree.
  TTree *fPrettyHkTree; ///< A pointer to the current pretty hk tree.
  TTree *fTurfRateTree; ///< A pointer to the current TURF rate tree.
  TTree *fSurfHkTree; ///< A pointer to the current SURF hk tree.
  TTree *fSumTurfRateTree; ///< A pointer to the current summed TURF rate tree.
  TTree *fAvgSurfHkTree;///< A pointer to the current averaged SURF hk tree.

  //And some useful info to keep track of what is where
  Long64_t fEventEntry; ///< The current event+header entry.
  Long64_t fPrettyHkEntry; ///< The current pretty hk entry.
  Long64_t fTurfRateEntry; ///< The current TURF rate entry.
  Long64_t fSurfHkEntry; ///< The current SURF hk entry.
  Long64_t fSumTurfRateEntry; ///< The current summed TURF rate entry.
  Long64_t fAvgSurfHkEntry; ///< The current averaged SURF hk entry.

  //And something to help with the indexing
  TTreeIndex *fHeadIndex; ///< Disused.

  UInt_t fCurrentRun; ///< The current run number.
  Char_t fCurrentBaseDir[180]; ///< The base directory for the ROOT files.
  

 protected:
  static MagicDisplay *fgInstance;  ///< The pointer to the current MagicDisplay
   // protect against multiple instances

 private:
  MagicDisplayFormatOption::MagicDisplayFormatOption_t fWaveformFormat; ///< The format for displaying waveforms.
  MagicDisplayCanvasLayoutOption::MagicDisplayCanvasLayoutOption_t fCanvasLayout; ///< The format for the canvas layout

   TCanvas *fMagicCanvas; ///< The main event display canvas.
   TPad *fMagicMainPad; ///< The main event display pad.
   TPad *fMagicEventInfoPad; ///< The event display info pad.

   TCanvas *fTurfCanvas; ///< The TURF rate canvas.
   TPad *fTurfMainPad; ///< The TURF rate pad.
   TPad *fTurfInfoPad; ///< The TURF rate info pad.

   TCanvas *fSumTurfCanvas; ///< The summed TURF rate canvas.
   TPad *fSumTurfMainPad; ///< The summed TURF rate pad.
   TPad *fSumTurfInfoPad; ///< The summed TURF rate info pad.

   TCanvas *fSurfCanvas; ///< The SURF hk canvas.
   TPad *fSurfMainPad; ///< The SURF hk pad.
   TPad *fSurfInfoPad; ///< The SURF hk info pad.

   TCanvas *fAvgSurfCanvas; ///< The averaged SURF hk canvas.
   TPad *fAvgSurfMainPad; ///< The averaged SURF hk pad.
   TPad *fAvgSurfInfoPad; ///< The averaged SURF hk info pad.

   RawAnitaHeader *fHeadPtr; ///< Pointer to the header.
   PrettyAnitaHk *fHkPtr; ///< Pointer to the pretty hk.
   RawAnitaEvent *fRawEventPtr; ///< Pointer to the raw event.
   UsefulAnitaEvent *fUsefulEventPtr; ///< Pointer to the calibrated event.
   TurfRate *fTurfPtr; ///< Pointer to the TURF rate.
   SurfHk *fSurfPtr; ///< Pointer to the SURF hk.
   SummedTurfRate *fSumTurfPtr; ///< Pointer to the summed TURF rate.
   AveragedSurfHk *fAvgSurfPtr; ///< Pointer to the averaged SURF hk.

   TButton *fVertButton; ///< The vertical polarisation button.
   TButton *fHorizButton; ///< The horizontal polarisation button.
   TButton *fBothButton; ///< The both polarisations button.
   TButton *fSurfButton; ///< The SURF view button.

   TButton *fWaveformButton; ///< The waveform view button.
   TButton *fPowerButton; ///< The FFT view button.
   TButton *fHilbertButton; ///< The Hilbert envelope view button.
   TButton *fAverageFFTButton; ///< The average FFT button.

   TButton *fTurfYScaleButton; ///< The TURF rate Y scale button.
   TButton *fSurfSurfViewButton; ///< The SURF hk SURF view button.
   TButton *fSurfPhiViewButton; ///< The SURF hk phi view button.

   TButton *fSumTurfYScaleButton; ///< The summed TURF rate Y scale button.
   TButton *fAvgSurfSurfViewButton; ///< The averaged SURF hk SURF view button.
   TButton *fAvgSurfPhiViewButton; ///< The averaged SURF hk phi view button.

   Int_t fInEventPlayMode; ///< Flag that indicates playback mode
   Int_t fEventPlaySleepMs; ///< Length of sleep between events in playback mode.

   WaveCalType::WaveCalType_t fCalType; ///< The waveform calibration type.

};


#endif //MAGICDISPLAY_H
