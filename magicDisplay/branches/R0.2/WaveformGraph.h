#ifndef WAVEFORMGRAPH_H
#define WAVEFORMGRAPH_H
#include "TGraph.h"

#include "AnitaConventions.h"

class TObject;
class TGaxis;

class WaveformGraph : public TGraph
{

 public:
  WaveformGraph();
  WaveformGraph(int n, const Int_t *x, const Int_t *y);
  WaveformGraph(int n, const Float_t *x, const Float_t *y);
  WaveformGraph(int n, const Double_t *x, const Double_t *y);
  virtual ~WaveformGraph();
  
  void DrawFFT(); // *MENU*
  TGraph *getFFT();

  void setSurfChanPhiAntPolRing(Int_t surf, Int_t chan, Int_t phi,
				Int_t ant, AnitaPol::AnitaPol_t pol,
				AnitaRing::AnitaRing_t ring)
  {fSurf=surf; fChan=chan; fPhi=phi; fAnt=ant; fPol=pol; fRing=ring;}

 private:
  Int_t fSurf;
  Int_t fChan;
  Int_t fPhi;
  Int_t fAnt;
  AnitaRing::AnitaRing_t fRing;
  AnitaPol::AnitaPol_t fPol;


  void ExecuteEvent(Int_t event, Int_t px, Int_t py);
  void ClearSelection(void);
  void ApplySelectionToButtons();
  void drawInNewCanvas();

  Int_t fNewCanvas;
  
  
  ClassDef(WaveformGraph,2)
};                              // end of class WaveformGraph


#endif  // WAVEFORMGRAPH_H
