#ifndef FFTGRAPH_H
#define FFTGRAPH_H
#include "TGraph.h"

#include "AnitaConventions.h"

class TObject;
class TGaxis;

//!  The PSD display graph class that inherits from ROOT's TGraph.
/*!
  The FFTGraph is one of the things that makes MagicDisplay purr. It's really quite simple but allows for the fucntionality to click on a graph and have it draw in a new window.
*/
class FFTGraph : public TGraph
{

 public:
   FFTGraph();  ///< Constructor
   FFTGraph(int n, const Int_t *x, const Int_t *y); ///< Int_t constructor
   FFTGraph(int n, const Float_t *x, const Float_t *y); ///< Float_t constructor
   FFTGraph(int n, const Double_t *x, const Double_t *y); ///< Double_t constructor
   virtual ~FFTGraph(); ///< Destructor
  

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
  
  
  ClassDef(FFTGraph,2)
};                              // end of class FFTGraph


#endif  // FFTGRAPH_H
