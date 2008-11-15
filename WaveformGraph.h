#ifndef WAVEFORMGRAPH_H
#define WAVEFORMGRAPH_H
#include "TGraph.h"

#include "AnitaConventions.h"

class TObject;
class TGaxis;

//!  The graph class that inherits from ROOT's TGraph.
/*!
  The WaveformGraph is one of the things that makes MagicDisplay purr. It's really quite simple but allows for the fucntionality to click on a graph and have it draw in a new window, and also allows the right click DrawFFT option.
*/
class WaveformGraph : public TGraph
{

 public:
   WaveformGraph(); ///< Constructor.
   WaveformGraph(int n, const Int_t *x, const Int_t *y); ///< Int_t constructor.
   WaveformGraph(int n, const Float_t *x, const Float_t *y); ///< Float_t constructor.
   WaveformGraph(int n, const Double_t *x, const Double_t *y); ///< Double_t constructor.
   virtual ~WaveformGraph(); ///< Destructor.
  
   //!Draws the PSD of the waveform in a new window (NB: It is the *MENU* desigination that ROOT uses to add it to the right click list).   
  void DrawFFT(); // *MENU*   
  void DrawHilbert(); // *MENU* 
  TGraph *getFFT(); ///< Returns a pointer to a TGraph containing the PSD of the waveform.
  TGraph *getHilbert(); ///< Returns a pointer to a TGraph containing the hilbert envelope of the waveform.

  void setSurfChanPhiAntPolRing(Int_t surf, Int_t chan, Int_t phi,
				Int_t ant, AnitaPol::AnitaPol_t pol,
				AnitaRing::AnitaRing_t ring) 
     {fSurf=surf; fChan=chan; fPhi=phi; fAnt=ant; fPol=pol; fRing=ring;} ///< These are just used for labelling.

 private:
  Int_t fSurf; ///< Used in labelling.
  Int_t fChan; ///< Used in labelling.
  Int_t fPhi; ///< Used in labelling.
  Int_t fAnt; ///< Used in labelling.
  AnitaRing::AnitaRing_t fRing; ///< Used in labelling.
  AnitaPol::AnitaPol_t fPol; ///< Used in labelling.


  void ExecuteEvent(Int_t event, Int_t px, Int_t py); ///< ROOT function that is called when a user interacts with the graph using the mouse.
  void ClearSelection(void); ///< ROOT thingy.
  void ApplySelectionToButtons(); ///< ROOT thingy.
  void drawInNewCanvas(); ///< Draws a copy waveform in a new canvas.

  Int_t fNewCanvas; ///< Flag to dictate whether the waveform is a copy drawn in a new canvas (if it is then the title is shown).
  
  
  ClassDef(WaveformGraph,2) ///< ROOT macro for persistence. 
};                              // end of class WaveformGraph


#endif  // WAVEFORMGRAPH_H
