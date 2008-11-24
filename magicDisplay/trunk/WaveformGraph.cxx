#include "WaveformGraph.h"
#include "TButton.h"
#include "TList.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TAxis.h"
#include <iostream>
using namespace std;

#include "FFTtools.h"


ClassImp(WaveformGraph);

WaveformGraph::WaveformGraph()

  : TGraph(),fNewCanvas(0)
    
{
  this->SetEditable(kFALSE);
}


WaveformGraph::WaveformGraph(int N, const Int_t *x, const Int_t *y)

  : TGraph(N,x,y),fNewCanvas(0)
{
  this->SetEditable(kFALSE);
}

WaveformGraph::WaveformGraph(int N, const Float_t *x, const Float_t *y)

  : TGraph(N,x,y),fNewCanvas(0)
{
  this->SetEditable(kFALSE);
}

WaveformGraph::WaveformGraph(int N, const Double_t *x, const Double_t *y)

  : TGraph(N,x,y),fNewCanvas(0)
{  
  this->SetEditable(kFALSE);
}

WaveformGraph::~WaveformGraph()
{
   //   std::cout << "~WaveformGraph" << std::endl;
}

void WaveformGraph::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
   switch (event) {
   case kButtonPress:
     //     cout << "kButtonPress" << endl;
     break;
     
   case kButtonDoubleClick:
     //     std::cout << "kButtonDoubleClick" << std::endl;
     new TCanvas();
     break;

   case kButton1Down:
     //     std::cout << "kButton1Down" << std::endl;
     if(!fNewCanvas) drawInNewCanvas();
     else this->TGraph::ExecuteEvent(event,px,py);
     break;
          
   default:
       this->TGraph::ExecuteEvent(event,px,py);
       break;
   }
}

void WaveformGraph::drawInNewCanvas()
{
  char graphTitle[180];
  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(1);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.1);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetCanvasDefW(600);
  gStyle->SetCanvasDefH(400);
  //   gROOT->ForceStyle();
  WaveformGraph *thisCopy = (WaveformGraph*)this->Clone();
  thisCopy->GetXaxis()->SetLabelSize(0.06);
  thisCopy->GetXaxis()->SetTitleSize(0.06);
  thisCopy->GetYaxis()->SetLabelSize(0.06);
  thisCopy->GetYaxis()->SetTitleSize(0.06);
  thisCopy->GetXaxis()->SetTitle("Time (ns)");
  thisCopy->GetYaxis()->SetTitle("Voltage (mV-ish)");
  sprintf(graphTitle,"Ant %d%c (%s Ring --  Phi %d -- SURF %d -- Chan %d)",
	  fAnt+1,AnitaPol::polAsChar(fPol),AnitaRing::ringAsString(fRing),
	  fPhi+1,fSurf+1,fChan+1);
  thisCopy->SetTitle(graphTitle);
  TCanvas *can = new TCanvas();
  can->SetLeftMargin(0.15);
  can->SetBottomMargin(0.15);
  can->SetTopMargin(0.1);
  can->SetRightMargin(0.1);
  thisCopy->Draw("al");
  //  fNewCanvas=1;
  
}

//______________________________________________________________________________
void WaveformGraph::DrawFFT()
{
  char graphTitle[180];
  gStyle->SetTitleH(0.1);
  gStyle->SetLabelSize(0.1,"xy");
  gStyle->SetTitleSize(0.1,"xy");
  gStyle->SetTitleOffset(0.5,"y");
  gStyle->SetOptTitle(1);
  gStyle->SetCanvasDefW(600);
  gStyle->SetCanvasDefH(400);
  //   gROOT->ForceStyle();
   
  TCanvas *can = new TCanvas();
  can->SetLeftMargin(0.15);
  can->SetBottomMargin(0.15);
  can->SetTopMargin(0.1);
  can->SetRightMargin(0.1);

  TGraph *grFFT  = this->getFFT();
  grFFT->GetXaxis()->SetLabelSize(0.06);
  grFFT->GetXaxis()->SetTitleSize(0.06);
  grFFT->GetYaxis()->SetLabelSize(0.06);
  grFFT->GetYaxis()->SetTitleSize(0.06);
  grFFT->GetXaxis()->SetTitle("Frequency (MHz)");
  grFFT->GetYaxis()->SetTitle("dB (m maybe)");
  sprintf(graphTitle,"Ant %d%c (%s Ring --  Phi %d -- SURF %d -- Chan %d)",
	  fAnt+1,AnitaPol::polAsChar(fPol),AnitaRing::ringAsString(fRing),
	  fPhi+1,fSurf+1,fChan+1);
  grFFT->SetTitle(graphTitle);
  grFFT->Draw("al");
  
  //   printf("WaveformGraph::DrawFFT: not yet implemented\n");
}

//______________________________________________________________________________
void WaveformGraph::DrawHilbert()
{
  char graphTitle[180];
  gStyle->SetTitleH(0.1);
  gStyle->SetLabelSize(0.1,"xy");
  gStyle->SetTitleSize(0.1,"xy");
  gStyle->SetTitleOffset(0.5,"y");
  gStyle->SetOptTitle(1);
  gStyle->SetCanvasDefW(600);
  gStyle->SetCanvasDefH(400);
  //   gROOT->ForceStyle();
   
  TCanvas *can = new TCanvas();
  can->SetLeftMargin(0.15);
  can->SetBottomMargin(0.15);
  can->SetTopMargin(0.1);
  can->SetRightMargin(0.1);

  TGraph *grHilbert  = this->getHilbert();
  grHilbert->GetXaxis()->SetLabelSize(0.06);
  grHilbert->GetXaxis()->SetTitleSize(0.06);
  grHilbert->GetYaxis()->SetLabelSize(0.06);
  grHilbert->GetYaxis()->SetTitleSize(0.06);
  grHilbert->GetXaxis()->SetTitle("Time (ns)");
  grHilbert->GetYaxis()->SetTitle("Voltage^2 (mv^2) ");
  sprintf(graphTitle,"Ant %d%c (%s Ring --  Phi %d -- SURF %d -- Chan %d)",
	  fAnt+1,AnitaPol::polAsChar(fPol),AnitaRing::ringAsString(fRing),
	  fPhi+1,fSurf+1,fChan+1);
  grHilbert->SetTitle(graphTitle);
  grHilbert->Draw("al");
  
  //   printf("WaveformGraph::DrawFFT: not yet implemented\n");
}

TGraph *WaveformGraph::getFFT()
{
  Double_t newX[248],newY[248];
  TGraph *grInt = FFTtools::getInterpolatedGraph(this,1./2.6);
  Int_t numSamps=grInt->GetN();
  Double_t *xVals=grInt->GetX();
  Double_t *yVals=grInt->GetY();
  for(int i=0;i<248;i++) {
    if(i<numSamps) {
      newX[i]=xVals[i];
      newY[i]=yVals[i];
    }
    else {
      newX[i]=newX[i-1]+(1./2.6);
      newY[i]=0;
    }      
  }
  TGraph *grNew = new TGraph(248,newX,newY);
  TGraph *grFFT = FFTtools::makePowerSpectrumMilliVoltsNanoSecondsdB(grNew);
  delete grNew;
  delete grInt;
  return grFFT;
}


TGraph *WaveformGraph::getHilbert()
{
  TGraph *grInt = FFTtools::getInterpolatedGraph(this,1./2.6);
  TGraph *grHilbert = FFTtools::getHilbertEnvelope(grInt);
  delete grInt;
  return grHilbert;
}
