#ifndef WAVEFORMGRAPH_H
#define WAVEFORMGRAPH_H
#include "TGraph.h"

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
  
  virtual void DrawFFT(); // *MENU*

 private:
  
  void ExecuteEvent(Int_t event, Int_t px, Int_t py);
  void ClearSelection(void);
  void ApplySelectionToButtons();
  void drawInNewCanvas();

  Int_t fNewCanvas;
  
  
  ClassDef(WaveformGraph,1)
};                              // end of class WaveformGraph


#endif  // WAVEFORMGRAPH_H
