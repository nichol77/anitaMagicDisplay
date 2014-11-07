#include "WaveformGraph.h"
#include "CorrelationFactory.h"
#include "TButton.h"
#include "TList.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TAxis.h"
#include <iostream>
using namespace std;

#include "FFTtools.h"

static Int_t zoomCounter=0;

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
  static int keyWasPressed=0;
   switch (event) {
   case kKeyPress:
     //     std::cout << "kKeyPress" << std::endl;
     keyWasPressed=1;
     break;
   case kButtonPress:
     //     cout << "kButtonPress" << endl;
     break;
     
   case kButton1Double:
     //     std::cout << "kButtonDoubleClick" << std::endl;
     //     new TCanvas();
     break;

   case kButton1Down:
     //     std::cout << "kButton1Down" << std::endl;
     if(!keyWasPressed) {
       if(!fNewCanvas) drawInNewCanvas();
       else this->TGraph::ExecuteEvent(event,px,py);
     }
     else {
       //       std::cout << "ctrl + click\n";
       CorrelationFactory::Instance()->addWaveformToCorrelation(this);
       keyWasPressed=0;
     }

     break;
          
   default:
       this->TGraph::ExecuteEvent(event,px,py);
       break;
   }
}

void WaveformGraph::drawInNewCanvas()
{
  char graphTitle[180];
  char graphName[180];
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


  if(fChan!=8) {
    sprintf(graphTitle,"Ant %d%c%c (%s Ring --  Phi %d -- SURF %d -- Chan %d)",
	    fPhi+1,AnitaRing::ringAsChar(fRing),AnitaPol::polAsChar(fPol),AnitaRing::ringAsString(fRing),
	    fPhi+1,fSurf+1,fChan+1);
  }
  else {
    sprintf(graphTitle,"Clock SURF %d -- Chan %d",fSurf+1,fChan+1);
  }
  sprintf(graphName,"grZoom%d",zoomCounter++);
  thisCopy->SetName(graphName);
  thisCopy->SetTitle(graphTitle);
  //  std::cout << graphName << "\t" << graphTitle << "\n";
  TCanvas *can = new TCanvas();
  can->SetLeftMargin(0.15);
  can->SetBottomMargin(0.15);
  can->SetTopMargin(0.1);
  can->SetRightMargin(0.1);
  thisCopy->Draw("al");
  int numPoints=thisCopy->GetN();
  can->Update();
  can->Modified();


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

  if(fChan!=8) {
    sprintf(graphTitle,"Ant %d%c (%s Ring --  Phi %d -- SURF %d -- Chan %d)",
	    fAnt+1,AnitaPol::polAsChar(fPol),AnitaRing::ringAsString(fRing),
	    fPhi+1,fSurf+1,fChan+1);
  }
  else {
    sprintf(graphTitle,"Clock SURF %d -- Chan %d",fSurf+1,fChan+1);
  }

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
  if(fChan!=8) {
    sprintf(graphTitle,"Ant %d%c (%s Ring --  Phi %d -- SURF %d -- Chan %d)",
	    fAnt+1,AnitaPol::polAsChar(fPol),AnitaRing::ringAsString(fRing),
	    fPhi+1,fSurf+1,fChan+1);
  }
  else {
    sprintf(graphTitle,"Clock SURF %d -- Chan %d",fSurf+1,fChan+1);
  }
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

void WaveformGraph::AddToCorrelation()
{
  CorrelationFactory::Instance()->addWaveformToCorrelation(this);
}
