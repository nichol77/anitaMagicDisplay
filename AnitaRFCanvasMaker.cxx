//////////////////////////////////////////////////////////////////////////////
/////  AnitaRFCanvasMaker.cxx        ANITA Event Canvas make               /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for making pretty event canvases for ANITA-II            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include "MagicDisplayConventions.h"
#include "AnitaRFCanvasMaker.h"
#include "AnitaConventions.h"
#include "AnitaGeomTool.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "TurfRate.h"
#include "SummedTurfRate.h"
#include "SurfHk.h"
#include "AveragedSurfHk.h"

#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TVector3.h"
#include "TROOT.h"
#include "TPaveText.h"
#include "TPad.h"
#include "TText.h"
#include "TLatex.h"
#include "TGraph.h"
#include "WaveformGraph.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"
#include "THStack.h"
#include "TList.h"
#include "TObject.h"
#include "TTimeStamp.h"
#include "TMath.h"
#include "TMultiGraph.h"

AnitaRFCanvasMaker*  AnitaRFCanvasMaker::fgInstance = 0;
AnitaGeomTool *fARFCGeomTool=0;

TH1F *histTurfRate[6]={0};
TH1F *histSumTurfRate[6]={0};
TH1F *histAntMask[2]={0};
TH1F *histSumAntMask[2]={0};
TH1F *histPhiMask=0;
TH1F *histSumPhiMask=0;


TH1F *histSurfHk[3][ACTIVE_SURFS]={{0}};
TH1F *histSurfHkPhi[3][PHI_SECTORS]={{0}};
TH1F *histBadThreshold=0;
TH1F *histBandMask=0;
THStack *histSurfHkStack[3];
THStack *histSurfHkPhiStack[3];
TH1F *surfHkFramey[3]={0};
TH1F *surfHkTimeFramey[3]={0};
TMultiGraph *mgSurfTime[3]={0};
TGraph *grSurfScaler[NUM_SURF][SCALERS_PER_SURF]={{0}};
TGraph *grSurfThresh[NUM_SURF][SCALERS_PER_SURF]={{0}};
TGraph *grSurfRfPwr[NUM_SURF][RFCHAN_PER_SURF]={{0}};


TH1F *histAvgSurfHk[3][ACTIVE_SURFS]={{0}};
TH1F *histAvgSurfHkPhi[3][PHI_SECTORS]={{0}};
TH1F *histAvgSurfBadThreshold=0;
TH1F *histAvgSurfBandMask=0;
THStack *histAvgSurfHkStack[3];
THStack *histAvgSurfHkPhiStack[3];
TH1F *avgSurfHkFramey[3]={0};

AnitaRFCanvasMaker::AnitaRFCanvasMaker()
{
  //Default constructor
  fTurfL1Max=0;
  fTurfL1Min=0;
  fTurfL2Max=0;
  fTurfL2Min=0;
  fTurfL3Max=0;
  fTurfL3Min=0;
  fSumTurfL1Max=0;
  fSumTurfL1Min=0;
  fSumTurfL2Max=0;
  fSumTurfL2Min=0;
  fSumTurfL3Max=0;
  fSumTurfL3Min=0;
  fSurfUseLog=0;
  fAvgSurfUseLog=0;
  fFixTurfYScale=0;
  fFixSurfYScale=0;
  fSurfKelvinView=1;
  fAvgSurfKelvinView=1;
  fNumSurfHks=0;
  fgInstance=this;
  
}

AnitaRFCanvasMaker::~AnitaRFCanvasMaker()
{
  //Default destructor
}



//______________________________________________________________________________
AnitaRFCanvasMaker*  AnitaRFCanvasMaker::Instance()
{
  //static function
  return (fgInstance) ? (AnitaRFCanvasMaker*) fgInstance : new AnitaRFCanvasMaker();
}


TPad *AnitaRFCanvasMaker::getTurfRateCanvas(TurfRate *turfPtr, TPad *useCan)
{
  TPad *canTurfRate;
  TPad *plotPad;
  if(!useCan) {
    canTurfRate = (TPad*) gROOT->FindObject("canTurfRate");
    if(!canTurfRate) {
      canTurfRate = new TCanvas("canTurfRate","canTurfRate",1000,600);
    }
    canTurfRate->Clear();
    canTurfRate->SetTopMargin(0);
    plotPad = new TPad("canTurfRateMain","canTurfRateMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  plotPad->Clear();

  const char *histNames[6]={"histUpperL1","histLowerL1","histUpperL2","histLowerL2","histNadirL","histL3"};
  const char *histTitles[6]={"Upper L1","Lower L1","Upper L2","Lower L2","Nadir L1/2","L3"};

  for(int i=0;i<6;i++) {
    if(histTurfRate[i]) {
      histTurfRate[i]->Reset();
    }
    else {
      histTurfRate[i]= new TH1F(histNames[i],histTitles[i],PHI_SECTORS,0.5,PHI_SECTORS+0.5);
    }
  }
  const char *maskNames[2]={"histAntMask0","histAntMask1"};
  for(int i=0;i<2;i++) {
    if(histAntMask[i])
      histAntMask[i]->Reset();
    else {
      histAntMask[i] = new TH1F(maskNames[i],maskNames[i],PHI_SECTORS,0.5,PHI_SECTORS+0.5);
    }
  }
  if(histPhiMask) 
    histPhiMask->Reset();
  else
    histPhiMask = new TH1F("histPhiMask","histPhiMask",PHI_SECTORS,0.5,PHI_SECTORS+0.5);
      


  Double_t l1Max=0;
  Double_t l2Max=0;
  Double_t l3Max=0;
  int maskedAnts[2][PHI_SECTORS];
  memset(maskedAnts,0,2*PHI_SECTORS*sizeof(int));
  for(int phi=0;phi<PHI_SECTORS;phi++) {
    if(turfPtr->getL1Rate(phi,0)>l1Max)
      l1Max=turfPtr->getL1Rate(phi,0);
    if(turfPtr->getL1Rate(phi,1)>l1Max)
      l1Max=turfPtr->getL1Rate(phi,1);
    if(turfPtr->getL2Rate(phi,0)>l2Max)
      l2Max=turfPtr->getL2Rate(phi,0);
    if(turfPtr->getL2Rate(phi,1)>l2Max)
      l2Max=turfPtr->getL2Rate(phi,1);
    if(turfPtr->isAntMasked(phi,0))
      maskedAnts[0][phi]=1;
    if(turfPtr->isAntMasked(phi,1))
      maskedAnts[1][phi]=1;
    if(turfPtr->getL3Rate(phi)>l3Max)
      l3Max=turfPtr->getL3Rate(phi);
    
      
    histTurfRate[0]->Fill(phi+1,1e-3*turfPtr->getL1Rate(phi,0));
    histTurfRate[1]->Fill(phi+1,1e-3*turfPtr->getL1Rate(phi,1));
    histTurfRate[2]->Fill(phi+1,turfPtr->getL2Rate(phi,0));
    histTurfRate[3]->Fill(phi+1,turfPtr->getL2Rate(phi,1));    
    histTurfRate[4]->Fill(phi+1,turfPtr->getNadirL12Rate(phi));
    histTurfRate[5]->Fill(phi+1,turfPtr->getL3Rate(phi));

    if(turfPtr->phiTrigMask & (1<<phi))
      histPhiMask->Fill(phi+1);


  }
  l1Max*=1.1e-3;
  l2Max*=1.1;
  l3Max*=1.1;
  plotPad->SetRightMargin(0.0);
  plotPad->SetLeftMargin(0.1);
  plotPad->SetTopMargin(0.05);
  plotPad->SetBottomMargin(0.1);
  plotPad->Divide(1,6,0,0);
   
  TLatex texy;
  texy.SetTextAlign(21);
  texy.SetTextAngle(270);
  texy.SetTextSize(0.17);

  if(!fFixTurfYScale) {
    if(fTurfL1Max)
      l1Max=fTurfL1Max;
    histTurfRate[0]->SetMaximum(l1Max);
    histTurfRate[1]->SetMaximum(l1Max);
    if(fTurfL2Max)
      l2Max=fTurfL2Max;
    histTurfRate[2]->SetMaximum(l2Max);
    histTurfRate[3]->SetMaximum(l2Max);
  }
  histPhiMask->Scale(l3Max);
  //  histTurfRate[5]->SetMaximum(l3Max);

  

  for(int ring=0;ring<2;ring++) {
    for(int phi=0;phi<PHI_SECTORS;phi++) {
      if(maskedAnts[ring][phi]) {
	histAntMask[ring]->Fill(phi+1,l1Max);
      }
    }    
    histAntMask[ring]->SetLineWidth(0);
    histAntMask[ring]->SetFillColor(kGray);
    histAntMask[ring]->SetFillStyle(3001);
  }

  histPhiMask->SetLineWidth(0);
  histPhiMask->SetFillColor(kGray);
  histPhiMask->SetFillStyle(3001);

  for(int i=0;i<6;i++) {
    TPad* paddy = (TPad*)plotPad->cd(i+1);
    //      if(i==4)
    paddy->SetBottomMargin(0.12);
    if(i==5)
      paddy->SetBottomMargin(0.18);
    paddy->SetRightMargin(0.05);
    histTurfRate[i]->SetFillStyle(3001);
    histTurfRate[i]->SetFillColor(getNiceColour(i));
    histTurfRate[i]->SetLineColor(getNiceColour(i));
    if(i==5) {
      histTurfRate[i]->SetXTitle("Phi Sector");
    }
    if(i<2)
      histTurfRate[i]->SetYTitle("Rate (kHz)");
    else
      histTurfRate[i]->SetYTitle("Rate (Hz)");
    histTurfRate[i]->SetStats(0);
    histTurfRate[i]->GetYaxis()->SetNoExponent(1);
    histTurfRate[i]->GetXaxis()->SetLabelSize(0.08);
    histTurfRate[i]->GetYaxis()->SetLabelSize(0.08);
    histTurfRate[i]->GetXaxis()->SetTitleSize(0.12);
    histTurfRate[i]->GetYaxis()->SetTitleSize(0.12);
    histTurfRate[i]->GetXaxis()->SetTitleOffset(0.65);
    histTurfRate[i]->GetYaxis()->SetTitleOffset(0.4);
    histTurfRate[i]->SetTitle("");
    histTurfRate[i]->Draw();
    if(i<2) {
      histAntMask[i%2]->Draw("same");
    }
    texy.DrawTextNDC(0.96,0.5,histTitles[i]);

    if(i==5)
      histPhiMask->Draw("same");

  }
  plotPad->Update();
  if(useCan)
    return plotPad;
  else
    return canTurfRate;
}

TPad *AnitaRFCanvasMaker::getTurfInfoCanvas(TurfRate *turfPtr,TPad *useCan)
{
  char textLabel[180];
  TPad *topPad;
  if(!useCan) {
    topPad = new TPad("padTurfInfo","padTurfInfo",0.2,0.9,0.8,1);
    topPad->Draw();
  }
  else {
    topPad=useCan;
  } 
  topPad->Clear();
  topPad->SetTopMargin(0.05);
  topPad->cd();
  TPaveText *leftPave = new TPaveText(0.2,0,0.8,0.9);
  leftPave->SetBorderSize(0);
  leftPave->SetFillColor(0);
  leftPave->SetTextAlign(13);
  sprintf(textLabel,"Run:    %d",turfPtr->run);
  TText *runText = leftPave->AddText(textLabel);
  runText->SetTextColor(50);
  TTimeStamp turfTime((time_t)turfPtr->realTime,0);
  sprintf(textLabel,"Time: %s",turfTime.AsString("s"));
  TText *timeText = leftPave->AddText(textLabel);
  timeText->SetTextColor(1);
  sprintf(textLabel,"ppsNum: %d",turfPtr->ppsNum);
  TText *ppsText = leftPave->AddText(textLabel);
  ppsText->SetTextColor(1);
  leftPave->Draw();
  return topPad;
}

TPad *AnitaRFCanvasMaker::getSurfHkCanvas(SurfHk *surfPtr, TPad *useCan)
{
  static MagicDisplaySurfHkDisplay::MagicDisplaySurfHkDisplay_t lastView=MagicDisplaySurfHkDisplay::kNotAView;
  static int lastKelvinView=1;
  gStyle->SetOptTitle(0);
  TPad *canSurfHk;
  TPad *plotPad;
  if(!useCan) {
    canSurfHk = (TPad*) gROOT->FindObject("canSurfHk");
    if(!canSurfHk) {
      canSurfHk = new TCanvas("canSurfHk","canSurfHk",1000,600);
    }
    canSurfHk->Clear();
    canSurfHk->SetTopMargin(0);
    plotPad = new TPad("canSurfHkMain","canSurfHkMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  //  std::cout << fSurfPhiView << "\t" << lastView << std::endl;
  static int callCounter=0;
  if(fSurfDisplay!=lastView  || lastKelvinView!=fSurfKelvinView) {
    plotPad->Clear();
    surfHkFramey[0]=0;
    surfHkFramey[1]=0;
    surfHkFramey[2]=0;    
  }
  lastView=fSurfDisplay;
  lastKelvinView=fSurfKelvinView;
  callCounter++;

  addToTimePlots(surfPtr);
  if(fSurfDisplay==MagicDisplaySurfHkDisplay::kPhiView)
    getSurfHkPhiCanvas(surfPtr,plotPad);
  else if(fSurfDisplay==MagicDisplaySurfHkDisplay::kSurfView)
    getSurfHkSurfCanvas(surfPtr,plotPad);
  else if(fSurfDisplay==MagicDisplaySurfHkDisplay::kTimeView)
    getSurfHkTimeCanvas(plotPad);
  if(useCan)
    return plotPad;
  else
    return canSurfHk;
}

void AnitaRFCanvasMaker::getSurfHkSurfCanvas(SurfHk *surfPtr,TPad *plotPad) {
  //   plotPad->Clear();

  char actualName[180];
  const char *histNames[3]={"histScaler","histThreshold","histRFPower"};
  const char *histTitles[3]={"Scaler","Threshold","RF Power"};

  if(histBadThreshold)
    delete histBadThreshold;
  //  histBadThreshold->Reset();
  //  else
  histBadThreshold = new TH1F("histBadThreshold","histBadThreshold",
			      ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
			      (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
  if(histBandMask)
    delete histBandMask;
  //    histBandMask->Reset();
  //  else
  histBandMask = new TH1F("histBandMask","histBandMask",
			  ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
			  (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
  Int_t maskedBands[ACTIVE_SURFS*SCALERS_PER_SURF]={0};

  for(int i=0;i<3;i++) {
    if(histSurfHkStack[i])
      delete histSurfHkStack[i];    
    sprintf(actualName,"%s_base",histNames[i]);  
    histSurfHkStack[i] = new THStack(actualName,histTitles[i]);
      

    for(int surf=0;surf<ACTIVE_SURFS;surf++){
      if(histSurfHk[i][surf]) {
	delete histSurfHk[i][surf];
	//	histSurfHk[i][surf]->Reset();
      }
      
      sprintf(actualName,"%s_%d",histNames[i],surf);
      if(i<2) {
	histSurfHk[i][surf]= new TH1F(actualName,histTitles[i],
				      ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
				      (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
      }
      else {
	histSurfHk[i][surf]= new TH1F(actualName,histTitles[i],
				      ACTIVE_SURFS*RFCHAN_PER_SURF,-0.5,
				      (ACTIVE_SURFS*RFCHAN_PER_SURF)-0.5);
      }

      
    }
  }

   

  for(int surf=0;surf<ACTIVE_SURFS;surf++) {
    for(int scl=0;scl<SCALERS_PER_SURF;scl++) {
      maskedBands[scl+surf*SCALERS_PER_SURF]=
	surfPtr->isBandMasked(surf,scl);

      histSurfHk[0][surf]->Fill(scl+surf*SCALERS_PER_SURF,
				surfPtr->scaler[surf][scl]);

      histSurfHk[1][surf]->Fill(scl+surf*SCALERS_PER_SURF,
				surfPtr->threshold[surf][scl]);
	 
      if(!surfPtr->globalThreshold && 
	 (surfPtr->threshold[surf][scl]!=surfPtr->setThreshold[surf][scl])){
	histBadThreshold->Fill(scl+surf*SCALERS_PER_SURF,
			       surfPtr->threshold[surf][scl]);
      }
	 
    }
    for(int chan=0;chan<RFCHAN_PER_SURF;chan++) {
      if(fSurfKelvinView==1) {
	histSurfHk[2][surf]->Fill(chan+surf*RFCHAN_PER_SURF,
				  surfPtr->getRFPowerInK(surf,chan));
      }
      else if(fSurfKelvinView==2) {
	histSurfHk[2][surf]->Fill(chan+surf*RFCHAN_PER_SURF,
				  surfPtr->getMeasuredRFPowerInK(surf,chan));

      }
      else {
	histSurfHk[2][surf]->Fill(chan+surf*RFCHAN_PER_SURF,
				  surfPtr->rfPower[surf][chan]);
      }

    }
    histSurfHk[0][surf]->SetFillColor(getNiceColour(surf%2));


    histSurfHk[1][surf]->SetFillColor(getNiceColour(surf%2));
    histSurfHk[2][surf]->SetFillColor(getNiceColour(surf%2));
    histSurfHkStack[0]->Add(histSurfHk[0][surf]);
    histSurfHkStack[1]->Add(histSurfHk[1][surf]);
    histSurfHkStack[2]->Add(histSurfHk[2][surf]);
  }   
  histBadThreshold->SetFillColor(kRed+1);
  histSurfHkStack[1]->Add(histBadThreshold);

   
  if(!plotPad->cd(3)) {
    plotPad->Clear();
    surfHkFramey[0]=0;
    surfHkFramey[1]=0;
    surfHkFramey[2]=0;    
    plotPad->SetRightMargin(0.0);
    plotPad->SetLeftMargin(0.1);
    plotPad->SetTopMargin(0.05);
    plotPad->SetBottomMargin(0.0);
    plotPad->Divide(1,3,0,0);
  }
   
  TLatex texy;
  texy.SetTextAlign(21);
  texy.SetTextAngle(270);
  texy.SetTextSize(0.17);

   

  for(int i=0;i<3;i++) {
    Double_t maxVal=histSurfHkStack[i]->GetMaximum("nostack");
    TPad* paddy = (TPad*)plotPad->cd(i+1);      
    if(!surfHkFramey[i]) {      
      paddy->SetBottomMargin(0.15);
      paddy->Clear();
      paddy->SetBottomMargin(0.18);
      paddy->SetRightMargin(0.05);
      
    if(i<2) {
      surfHkFramey[i]=paddy->DrawFrame(0,0,ACTIVE_SURFS*SCALERS_PER_SURF,maxVal*1.05);
    }
    else {
      surfHkFramey[i]=paddy->DrawFrame(0,0,ACTIVE_SURFS*RFCHAN_PER_SURF,maxVal*1.05);
    }


    if(i==0)
      surfHkFramey[i]->GetYaxis()->SetTitle("Rate (kHz)");
    if(i==1)
      surfHkFramey[i]->GetYaxis()->SetTitle("Threshold (DAC)");
    if(i==2) {
      if(fSurfKelvinView) {
	surfHkFramey[i]->GetYaxis()->SetTitle("RF Power (K)");
      }
      else {
	surfHkFramey[i]->GetYaxis()->SetTitle("RF Power (ADC)");
      }
    }
    surfHkFramey[i]->GetXaxis()->SetTitle("Logical Channel");
    surfHkFramey[i]->GetYaxis()->SetNoExponent(1);
    surfHkFramey[i]->GetXaxis()->SetLabelSize(0.08);
    surfHkFramey[i]->GetYaxis()->SetLabelSize(0.08);
    surfHkFramey[i]->GetXaxis()->SetTitleSize(0.09);
    surfHkFramey[i]->GetYaxis()->SetTitleSize(0.09);
    surfHkFramey[i]->GetXaxis()->SetTitleOffset(0.8);
    surfHkFramey[i]->GetYaxis()->SetTitleOffset(0.6);
    surfHkFramey[i]->GetYaxis()->SetTickLength(-0.01);
    surfHkFramey[i]->GetYaxis()->SetLabelOffset(0.015);
    surfHkFramey[i]->GetXaxis()->SetTickLength(-0.01);
    surfHkFramey[i]->GetXaxis()->SetLabelOffset(0.015);
    surfHkFramey[i]->SetTitle("");
    }
    else {
      if(!fFixSurfYScale) {
	surfHkFramey[i]->GetYaxis()->SetRangeUser(0,maxVal*1.05);
      }
      gPad->SetLogy(0);
      if(fSurfUseLog) {
	surfHkFramey[i]->GetYaxis()->SetRangeUser(0.1,maxVal*2);
	gPad->SetLogy(1);
      }
      
    }
    for(int surf=0;surf<ACTIVE_SURFS;surf++) {
      histSurfHk[i][surf]->Draw("same");
    }
    if(i==0) {
      maxVal*=1.1;
      for(int i=0;i<ACTIVE_SURFS*SCALERS_PER_SURF;i++) {
	if(maskedBands[i])
	  histBandMask->Fill(i,maxVal);
      }
      histBandMask->SetLineWidth(0);
      histBandMask->SetFillColor(kGray);
      histBandMask->SetFillStyle(3001);
      histBandMask->Draw("same");
    }
  }
  plotPad->Update();
}



void AnitaRFCanvasMaker::getSurfHkPhiCanvas(SurfHk *surfPtr,TPad *plotPad) {
  //   plotPad->Clear();
  if(!fARFCGeomTool)
    fARFCGeomTool=AnitaGeomTool::Instance();
  char actualName[180];
  const char *histNames[3]={"histScalerPhi","histThresholdPhi","histRFPowerPhi"};
  const char *histTitles[3]={"Scaler","Threshold","RF Power"};

  if(histBadThreshold)
    delete histBadThreshold;
  //  histBadThreshold->Reset();
  //  else
  histBadThreshold = new TH1F("histBadThreshold","histBadThreshold",
			      ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
			      (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
  if(histBandMask)
    delete histBandMask;
  //  histBandMask->Reset();
  //  else
  histBandMask = new TH1F("histBandMask","histBandMask",
			  ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
			  (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
  Int_t maskedBands[ACTIVE_SURFS*SCALERS_PER_SURF]={0};

  for(int i=0;i<3;i++) {
    if(histSurfHkPhiStack[i])
      delete histSurfHkPhiStack[i];      
    histSurfHkPhiStack[i] = new THStack(histNames[i],histTitles[i]);
    sprintf(actualName,"%s_base",histNames[i]);
      

    for(int phi=0;phi<PHI_SECTORS;phi++){
      if(histSurfHkPhi[i][phi]) {
	delete histSurfHkPhi[i][phi];
	//	histSurfHkPhi[i][phi]->Reset();
      }
      {
	sprintf(actualName,"%s_%d",histNames[i],phi);
	if(i<2) {
	  histSurfHkPhi[i][phi]= new TH1F(actualName,histTitles[i],
				       ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
				       (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
	}
	else {
	  histSurfHkPhi[i][phi]= new TH1F(actualName,histTitles[i],
					  3*PHI_SECTORS*2,-0.5,
					  (3*PHI_SECTORS*2)-0.5);
	}

      }
    }
  }

   
  for(int ring=0;ring<3;ring++) {
    for(int phi=0;phi<PHI_SECTORS;phi++) {     
      if(ring==2) {
	if(phi%2)
	  continue;
      }
      for(int band=0;band<4;band++) {
	Int_t index=band+4*phi+64*ring;
	if(ring==2)
	  index=128 + band+4*(phi/2);
	maskedBands[index]=
	  surfPtr->isBandMasked(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band);
	 
	histSurfHkPhi[0][phi]->Fill(index,surfPtr->getScaler(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band));
	 
	histSurfHkPhi[1][phi]->Fill(index,surfPtr->getThreshold(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band));
	 
	if(!surfPtr->globalThreshold && 
	   (surfPtr->getThreshold(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band)!=surfPtr->getSetThreshold(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band))){
	  histBadThreshold->Fill(index,surfPtr->getThreshold(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band));
	}
	 
      }
       
      //Now for the RF power
      //Argh
      for(int pol=0;pol<2;pol++) {
	Int_t surf,rfChan;
	Int_t chanIndex=
	  fARFCGeomTool->getChanIndexFromRingPhiPol((AnitaRing::AnitaRing_t)ring,phi,(AnitaPol::AnitaPol_t)pol);
	fARFCGeomTool->getSurfChanFromChanIndex(chanIndex,surf,rfChan);
	Int_t index=pol + 2 *phi + 32*ring;

	if(fSurfKelvinView==1) {
	  histSurfHkPhi[2][surf]->Fill(index,
				    surfPtr->getRFPowerInK(surf,rfChan));
	}
	else if(fSurfKelvinView==2) {
	  histSurfHkPhi[2][surf]->Fill(index,
				       surfPtr->getMeasuredRFPowerInK(surf,rfChan));

	}
	else {
	  histSurfHkPhi[2][surf]->Fill(index,
				    surfPtr->rfPower[surf][rfChan]);
	}
	 
      }
    }
  }
  for(int phi=0;phi<PHI_SECTORS;phi++) {
    histSurfHkPhi[0][phi]->SetFillColor(getNiceColour(phi%4));            
    histSurfHkPhi[1][phi]->SetFillColor(getNiceColour(phi%4));
    histSurfHkPhi[2][phi]->SetFillColor(getNiceColour(phi%4));
    histSurfHkPhiStack[0]->Add(histSurfHkPhi[0][phi]);
    histSurfHkPhiStack[1]->Add(histSurfHkPhi[1][phi]);
    histSurfHkPhiStack[2]->Add(histSurfHkPhi[2][phi]);
  }   
  histBadThreshold->SetFillColor(kRed+1);
  histSurfHkPhiStack[1]->Add(histBadThreshold);
     
   
  if(!plotPad->cd(3)) {
    plotPad->Clear();    
    surfHkFramey[0]=0;
    surfHkFramey[1]=0;
    surfHkFramey[2]=0;    
    plotPad->SetRightMargin(0.0);
    plotPad->SetLeftMargin(0.1);
    plotPad->SetTopMargin(0.05);
    plotPad->SetBottomMargin(0.0);
    plotPad->Divide(1,3,0,0);
  }
     
  TLatex texy;
  texy.SetTextAlign(21);
  texy.SetTextAngle(270);
  texy.SetTextSize(0.17);
     

  char label[10];
  for(int i=0;i<3;i++) {
    Double_t maxVal=histSurfHkPhiStack[i]->GetMaximum("nostack");
    TPad* paddy = (TPad*)plotPad->cd(i+1);      
    if(!surfHkFramey[i]) {      
      paddy->SetBottomMargin(0.15);
      paddy->Clear();
      paddy->SetBottomMargin(0.18);
      paddy->SetRightMargin(0.05);
      
      if(i<2) {
	surfHkFramey[i]=paddy->DrawFrame(0,0,ACTIVE_SURFS*SCALERS_PER_SURF,maxVal*1.05);
	//      surfHkFramey[i]->GetXaxis()->SetNdivisions(PHI_SECTORS);
      }
      else {
	surfHkFramey[i]=paddy->DrawFrame(0,0,3*PHI_SECTORS*2,maxVal*1.05);
      }

      
      if(i==0)
	surfHkFramey[i]->GetYaxis()->SetTitle("Rate (kHz)");
      if(i==1)
	surfHkFramey[i]->GetYaxis()->SetTitle("Threshold (DAC)");
      if(i==2) {
	if(fSurfKelvinView) {
	  surfHkFramey[i]->GetYaxis()->SetTitle("RF Power (K)");
	}
	else {
	  surfHkFramey[i]->GetYaxis()->SetTitle("RF Power (ADC)");
	}
      }
      surfHkFramey[i]->GetXaxis()->SetTitle("Phi Sector");
      if(i<2) {
	//      surfHkFramey[i]->GetXaxis()->SetNdivisions(-16);
	surfHkFramey[i]->SetBins(ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
		   		 (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
	for(int phi=0;phi<16;phi+=2) {
	  sprintf(label,"%d",phi+1);
	  surfHkFramey[i]->GetXaxis()->SetBinLabel(4*phi+2,label);
	  surfHkFramey[i]->GetXaxis()->SetBinLabel(4*phi+66,label);
	  if(phi%4==0) {
	    surfHkFramey[i]->GetXaxis()->SetBinLabel(2*phi+130,label);
	  }
	}

      }
      else {
	surfHkFramey[i]->SetBins(3*PHI_SECTORS*2,-0.5,
				 (3*PHI_SECTORS*2)-0.5);
	for(int phi=0;phi<16;phi+=2) {
	  sprintf(label,"%d",phi+1);	  
	  surfHkFramey[i]->GetXaxis()->SetBinLabel(2*phi +1,label);
	  surfHkFramey[i]->GetXaxis()->SetBinLabel(2*phi +33,label);
	  surfHkFramey[i]->GetXaxis()->SetBinLabel(2*phi +65,label);
	}
      }
	

    surfHkFramey[i]->GetYaxis()->SetNoExponent(1);
    surfHkFramey[i]->GetXaxis()->SetLabelSize(0.08);
    surfHkFramey[i]->GetYaxis()->SetLabelSize(0.08);
    surfHkFramey[i]->GetXaxis()->SetTitleSize(0.09);
    surfHkFramey[i]->GetYaxis()->SetTitleSize(0.09);
    surfHkFramey[i]->GetXaxis()->SetTitleOffset(0.8);
    surfHkFramey[i]->GetYaxis()->SetTitleOffset(0.6);
    surfHkFramey[i]->GetYaxis()->SetTickLength(-0.01);
    surfHkFramey[i]->GetYaxis()->SetLabelOffset(0.015);
    surfHkFramey[i]->GetXaxis()->SetTickLength(-0.01);
    surfHkFramey[i]->GetXaxis()->SetLabelOffset(0.015);
    surfHkFramey[i]->SetTitle("");
    }
    else {
      if(!fFixSurfYScale) {
	surfHkFramey[i]->GetYaxis()->SetRangeUser(0,maxVal*1.05);
      }      
      gPad->SetLogy(0);
      if(fSurfUseLog) {
	surfHkFramey[i]->GetYaxis()->SetRangeUser(0.1,maxVal*2);
	gPad->SetLogy(1);
      }
    }
    for(int phi=0;phi<PHI_SECTORS;phi++) {
      histSurfHkPhi[i][phi]->Draw("same");
    }
    if(i==0) {
      maxVal*=1.1;
      for(int i=0;i<ACTIVE_SURFS*SCALERS_PER_SURF;i++) {
	if(maskedBands[i])
	  histBandMask->Fill(i,maxVal);
      }
      histBandMask->SetLineWidth(0);
      histBandMask->SetFillColor(kGray);
      histBandMask->SetFillStyle(3001);
      histBandMask->Draw("same");
    }
  }
  plotPad->Update();
}

void AnitaRFCanvasMaker::getSurfHkTimeCanvas(TPad *plotPad) {
  char graphName[180];
  plotPad->Clear();
  for(int i=0;i<3;i++) {
    if(mgSurfTime[i]) delete mgSurfTime[i];
    mgSurfTime[i]=0;
  }
  if(!plotPad->cd(3)) {
    plotPad->Clear();    
    plotPad->SetRightMargin(0);
    plotPad->SetLeftMargin(0.1);
    plotPad->SetTopMargin(0.05);
    plotPad->SetBottomMargin(0.1);
    plotPad->Divide(1,3,0,0);
  //   for(int pad=1;pad<=3;pad++) {
//       plotPad->cd(pad);
//       gPad->SetRightMargin(0.05);
//     }
  }

  Int_t numPoints=fNumSurfHks;
  if(numPoints>MAX_SURF_HK_TIME_POINTS) numPoints=MAX_SURF_HK_TIME_POINTS;
 //  Double_t minTime=TMath::MinElement(numPoints,fTimeVals);
//   Double_t maxTime=TMath::MaxElement(numPoints,fTimeVals);

//   Double_t minScaler=TMath::MinElement(numPoints*NUM_SURF*SCALERS_PER_SURF,fScalerVals[0][0]);
//   Double_t maxScaler=TMath::MaxElement(numPoints*NUM_SURF*SCALERS_PER_SURF,fScalerVals[0][0]);
//   Double_t minThresh=TMath::MinElement(numPoints*NUM_SURF*SCALERS_PER_SURF,fThreshVals[0][0]);
//   Double_t maxThresh=TMath::MaxElement(numPoints*NUM_SURF*SCALERS_PER_SURF,fThreshVals[0][0]);
//   Double_t minRfPwr=TMath::MinElement(numPoints*NUM_SURF*RFCHAN_PER_SURF,fRfPowVals[0][0]);
//   Double_t maxRfPwr=TMath::MaxElement(numPoints*NUM_SURF*RFCHAN_PER_SURF,fRfPowVals[0][0]);
  
  plotPad->cd(1);
  gPad->Clear();
  gPad->SetRightMargin(0.05);
  //  if(mgSurfTime[0]) delete mgSurfTime[0];
  mgSurfTime[0] = new TMultiGraph();
  for(int surf=0;surf<NUM_SURF;surf++) {
    for(int scl=0;scl<SCALERS_PER_SURF;scl++) {
      //Will change this in a bit
      grSurfScaler[surf][scl] = new TGraph(numPoints,fTimeVals,fScalerVals[surf][scl]);
      grSurfScaler[surf][scl]->SetLineColor(getNiceColour(surf));
      grSurfScaler[surf][scl]->SetMarkerColor(getNiceColour(surf));
      grSurfScaler[surf][scl]->SetMarkerStyle(getMarker(scl));
      
      sprintf(graphName,"grSurfScaler%d_%d",surf,scl);
      grSurfScaler[surf][scl]->SetName(graphName);

      mgSurfTime[0]->Add(grSurfScaler[surf][scl],"lp");
    }
  }
  mgSurfTime[0]->Draw("alp");
  mgSurfTime[0]->GetYaxis()->SetTitle("Scaler (kHz)");
  mgSurfTime[0]->GetXaxis()->SetTimeDisplay(1);
  plotPad->Update();
  plotPad->cd(2);
  gPad->Clear();
  gPad->SetRightMargin(0.05);
  //  if(mgSurfTime[1]) delete mgSurfTime[1];
  mgSurfTime[1] = new TMultiGraph();
  for(int surf=0;surf<NUM_SURF;surf++) {
    for(int scl=0;scl<SCALERS_PER_SURF;scl++) {
      //Will change this in a bit
      //      if(grSurfThresh[surf][scl]) delete grSurfThresh[surf][scl];
      grSurfThresh[surf][scl] = new TGraph(numPoints,fTimeVals,fThreshVals[surf][scl]);
      grSurfThresh[surf][scl]->SetLineColor(getNiceColour(surf));
      grSurfThresh[surf][scl]->SetMarkerColor(getNiceColour(surf));
      grSurfThresh[surf][scl]->SetMarkerStyle(getMarker(scl));


      sprintf(graphName,"grSurfThresh%d_%d",surf,scl);
      grSurfThresh[surf][scl]->SetName(graphName);


      mgSurfTime[1]->Add(grSurfThresh[surf][scl],"lp");
    }
  }
  mgSurfTime[1]->Draw("alp");
  mgSurfTime[1]->GetYaxis()->SetTitle("Threshold (DAC)");
  plotPad->Update();
  

  plotPad->cd(3);
  gPad->Clear();
  gPad->SetRightMargin(0.05);
  //   if(mgSurfTime[2]) delete mgSurfTime[2];
  mgSurfTime[2] = new TMultiGraph();
  for(int surf=0;surf<NUM_SURF;surf++) {
    for(int scl=0;scl<RFCHAN_PER_SURF;scl++) {
      //Will change this in a bit
      grSurfRfPwr[surf][scl] = new TGraph(numPoints,fTimeVals,fRfPowVals[surf][scl]);
      grSurfRfPwr[surf][scl]->SetLineColor(getNiceColour(surf));
      grSurfRfPwr[surf][scl]->SetMarkerColor(getNiceColour(surf));
      grSurfRfPwr[surf][scl]->SetMarkerStyle(getMarker(scl));


      sprintf(graphName,"grSurfRfPwr%d_%d",surf,scl);
      grSurfRfPwr[surf][scl]->SetName(graphName);

      mgSurfTime[2]->Add(grSurfRfPwr[surf][scl],"lp");
    }
  }
  mgSurfTime[2]->Draw("alp");
  mgSurfTime[2]->GetXaxis()->SetTitle("Time");
  mgSurfTime[2]->GetYaxis()->SetTitle("RF Power (K)");
  mgSurfTime[2]->GetXaxis()->SetTimeDisplay(1);
  


   
  plotPad->Update();
}



TPad *AnitaRFCanvasMaker::getSurfInfoCanvas(SurfHk *surfPtr,TPad *useCan)
{
  char textLabel[180];
  TPad *topPad;
  if(!useCan) {
    topPad = new TPad("padSurfInfo","padSurfInfo",0.2,0.9,0.8,1);
    topPad->Draw();
  }
  else {
    topPad=useCan;
  } 
  topPad->Clear();
  topPad->SetTopMargin(0.05);
  topPad->Divide(2,1);
  topPad->cd(1);
  TPaveText *leftPave = new TPaveText(0.05,0,0.95,0.9);
  leftPave->SetBorderSize(0);
  leftPave->SetFillColor(0);
  leftPave->SetTextAlign(13);
  sprintf(textLabel,"Run:    %d",surfPtr->run);
  TText *runText = leftPave->AddText(textLabel);
  runText->SetTextColor(50);
  TTimeStamp surfTime((time_t)surfPtr->realTime,0);
  sprintf(textLabel,"Time: %s",surfTime.AsString("s"));
  TText *timeText = leftPave->AddText(textLabel);
  timeText->SetTextColor(1);
  sprintf(textLabel,"ms: %8.6f ms",1e-3*surfPtr->payloadTimeUs);
  TText *msText = leftPave->AddText(textLabel);
  msText->SetTextColor(1);
  leftPave->Draw();

  topPad->cd(2);
  TPaveText *rightPave = new TPaveText(0.05,0,0.95,0.9);
  rightPave->SetBorderSize(0);
  rightPave->SetFillColor(0);
  rightPave->SetTextAlign(13);
  sprintf(textLabel,"Global: %d",surfPtr->globalThreshold);
  TText *globalText = rightPave->AddText(textLabel);
  globalText->SetTextColor(1);
  sprintf(textLabel,"Goal Rates: (%d %d %d %d) kHz",
	  surfPtr->scalerGoals[0],surfPtr->scalerGoals[1],
	  surfPtr->scalerGoals[2],surfPtr->scalerGoals[3]);
  TText *goalText = rightPave->AddText(textLabel);
  goalText->SetTextColor(1);
  sprintf(textLabel,"Error Flag: %#x",surfPtr->errorFlag);
  TText *errorText = rightPave->AddText(textLabel);
  errorText->SetTextColor(1);
  rightPave->Draw();   
  return topPad;

}



TPad *AnitaRFCanvasMaker::getSumTurfRateCanvas(SummedTurfRate *sumTurfPtr, TPad *useCan)
{
  TPad *canTurfRate;
  TPad *plotPad;
  if(!useCan) {
    canTurfRate = (TPad*) gROOT->FindObject("canTurfRate");
    if(!canTurfRate) {
      canTurfRate = new TCanvas("canTurfRate","canTurfRate",1000,600);
    }
    canTurfRate->Clear();
    canTurfRate->SetTopMargin(0);
    plotPad = new TPad("canTurfRateMain","canTurfRateMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  plotPad->Clear();

  const char *histNames[6]={"histSumUpperL1","histSumLowerL1","histSumUpperL2","histSumLowerL2","histSumNadirL","histSumL3"};
  const char *histTitles[6]={"Upper L1","Lower L1","Upper L2","Lower L2","Nadir L1/2","L3"};

  for(int i=0;i<6;i++) {
    if(histSumTurfRate[i]) {
      delete histSumTurfRate[i];
      //      histSumTurfRate[i]->Reset();
    }
    //    else {
    histSumTurfRate[i]= new TH1F(histNames[i],histTitles[i],PHI_SECTORS,0.5,PHI_SECTORS+0.5);
      //    }
  }
  const char *maskNames[2]={"histSumAntMask0","histSumAntMask1"};
  for(int i=0;i<2;i++) {
    if(histSumAntMask[i])
      delete histSumAntMask[i];
    //    histSumAntMask[i]->Reset();
    //    else {
    histSumAntMask[i] = new TH1F(maskNames[i],maskNames[i],PHI_SECTORS,0.5,PHI_SECTORS+0.5);
    //  }
  }

  if(histSumPhiMask) 
    delete histSumPhiMask;
  //  histSumPhiMask->Reset();
  //  else
  histSumPhiMask = new TH1F("histSumPhiMask","histSumPhiMask",PHI_SECTORS,0.5,PHI_SECTORS+0.5);

  Double_t l1Max=0;
  Double_t l2Max=0;
  Double_t l3Max=0;
  int maskedAnts[2][PHI_SECTORS];
  memset(maskedAnts,0,2*PHI_SECTORS*sizeof(int));
  
  for(int phi=0;phi<PHI_SECTORS;phi++) {
    if(sumTurfPtr->getL1Rate(phi,0)>l1Max)
      l1Max=sumTurfPtr->getL1Rate(phi,0);
    if(sumTurfPtr->getL1Rate(phi,1)>l1Max)
      l1Max=sumTurfPtr->getL1Rate(phi,1);
    if(sumTurfPtr->getL2Rate(phi,0)>l2Max)
      l2Max=sumTurfPtr->getL2Rate(phi,0);
    if(sumTurfPtr->getL2Rate(phi,1)>l2Max)
      l2Max=sumTurfPtr->getL2Rate(phi,1);
    if(sumTurfPtr->isAntMasked(phi,0))
      maskedAnts[0][phi]=1;
    if(sumTurfPtr->isAntMasked(phi,1))
      maskedAnts[1][phi]=1;
    if(sumTurfPtr->getL3Rate(phi)>l3Max)
      l3Max=sumTurfPtr->getL3Rate(phi);
    
      
    histSumTurfRate[0]->Fill(phi+1,(1e-3*sumTurfPtr->getL1Rate(phi,0)));
    histSumTurfRate[1]->Fill(phi+1,(1e-3*sumTurfPtr->getL1Rate(phi,1)));
    histSumTurfRate[2]->Fill(phi+1,(sumTurfPtr->getL2Rate(phi,0)));
    histSumTurfRate[3]->Fill(phi+1,(sumTurfPtr->getL2Rate(phi,1)));
    histSumTurfRate[4]->Fill(phi+1,sumTurfPtr->getNadirL12Rate(phi));
    histSumTurfRate[5]->Fill(phi+1,sumTurfPtr->getL3Rate(phi));

    if(sumTurfPtr->phiTrigMask & (1<<phi))
      histSumPhiMask->Fill(phi+1);
    
  }

  float scale=1./sumTurfPtr->numRates;
  for(int i=0;i<6;i++)
    histSumTurfRate[i]->Scale(scale);

  l1Max*=1.1e-3;
  l2Max*=1.1;
  l3Max*=1.1;
  l1Max*=scale;
  l2Max*=scale;
  plotPad->SetRightMargin(0.0);
  plotPad->SetLeftMargin(0.1);
  plotPad->SetTopMargin(0.05);
  plotPad->SetBottomMargin(0.1);
  plotPad->Divide(1,6,0,0);
   
  //  histSumTurfRate[5]->SetMaximum(l3Max);
  histSumPhiMask->Scale(l3Max);


  TLatex texy;
  texy.SetTextAlign(21);
  texy.SetTextAngle(270);
  texy.SetTextSize(0.17);

  if(!fFixSumTurfYScale) {
    if(fSumTurfL1Max)
      l1Max=fSumTurfL1Max;
    histSumTurfRate[0]->SetMaximum(l1Max);
    histSumTurfRate[1]->SetMaximum(l1Max);
    if(fSumTurfL2Max)
      l2Max=fSumTurfL2Max;
    histSumTurfRate[2]->SetMaximum(l2Max);
    histSumTurfRate[3]->SetMaximum(l2Max);
  }
  

  for(int ring=0;ring<2;ring++) {
    for(int phi=0;phi<PHI_SECTORS;phi++) {
      if(maskedAnts[ring][phi]) {
	//	std::cout << ring << "\t" << phi << "\t" << maskedAnts[ring][phi] << std::endl;
	histSumAntMask[ring]->Fill(phi+1,l1Max);
      }
    }    
    histSumAntMask[ring]->SetLineWidth(0);
    histSumAntMask[ring]->SetFillColor(kGray);
    histSumAntMask[ring]->SetFillStyle(3001);
  }

  histSumPhiMask->SetLineWidth(0);
  histSumPhiMask->SetFillColor(kGray);
  histSumPhiMask->SetFillStyle(3001);

  for(int i=0;i<6;i++) {
    TPad* paddy = (TPad*)plotPad->cd(i+1);
    //      if(i==4)
    paddy->SetBottomMargin(0.12);
    if(i==5) {
      paddy->SetBottomMargin(0.18);      
    }
    paddy->SetRightMargin(0.05);
    histSumTurfRate[i]->SetFillStyle(3001);
    histSumTurfRate[i]->SetFillColor(getNiceColour(i));
    histSumTurfRate[i]->SetLineColor(getNiceColour(i));
    if(i==5)      
      histSumTurfRate[i]->SetXTitle("Phi Sector");
    if(i<2)
      histSumTurfRate[i]->SetYTitle("Rate (kHz)");
    else
      histSumTurfRate[i]->SetYTitle("Rate (Hz)");
    histSumTurfRate[i]->SetStats(0);
    histSumTurfRate[i]->GetYaxis()->SetNoExponent(1);
    histSumTurfRate[i]->GetXaxis()->SetLabelSize(0.08);
    histSumTurfRate[i]->GetYaxis()->SetLabelSize(0.08);
    histSumTurfRate[i]->GetXaxis()->SetTitleSize(0.12);
    histSumTurfRate[i]->GetYaxis()->SetTitleSize(0.12);
    histSumTurfRate[i]->GetXaxis()->SetTitleOffset(0.65);
    histSumTurfRate[i]->GetYaxis()->SetTitleOffset(0.4);
    histSumTurfRate[i]->SetTitle("");
    histSumTurfRate[i]->Draw();
    if(i<2) {
      histSumAntMask[i%2]->Draw("same");
    }
    texy.DrawTextNDC(0.96,0.5,histTitles[i]);


    if(i==5)
      histSumPhiMask->Draw("same");

  }
  plotPad->Update();
  if(useCan)
    return plotPad;
  else
    return canTurfRate;
}

TPad *AnitaRFCanvasMaker::getSumTurfInfoCanvas(SummedTurfRate *sumTurfPtr,TPad *useCan)
{
  char textLabel[180];
  TPad *topPad;
  if(!useCan) {
    topPad = new TPad("padTurfInfo","padTurfInfo",0.2,0.9,0.8,1);
    topPad->Draw();
  }
  else {
    topPad=useCan;
  } 
  topPad->Clear();
  topPad->SetTopMargin(0.05);
  topPad->cd();
  TPaveText *leftPave = new TPaveText(0.2,0,0.8,0.9);
  leftPave->SetBorderSize(0);
  leftPave->SetFillColor(0);
  leftPave->SetTextAlign(13);
  sprintf(textLabel,"Run:    %d",sumTurfPtr->run);
  TText *runText = leftPave->AddText(textLabel);
  runText->SetTextColor(50);
  TTimeStamp turfTime((time_t)sumTurfPtr->realTime,0);
  sprintf(textLabel,"Time: %s",turfTime.AsString("s"));
  TText *timeText = leftPave->AddText(textLabel);
  timeText->SetTextColor(1);
  sprintf(textLabel,"Num Rates: %d",sumTurfPtr->numRates);
  TText *numRatesText = leftPave->AddText(textLabel);
  numRatesText->SetTextColor(1);
  sprintf(textLabel,"Delta T: %d",sumTurfPtr->deltaT);
  TText *deltatText = leftPave->AddText(textLabel);
  deltatText->SetTextColor(1);
  leftPave->Draw();
  return topPad;
}


TPad *AnitaRFCanvasMaker::getAvgSurfHkCanvas(AveragedSurfHk *avgSurfPtr, TPad *useCan)
{
  static MagicDisplaySurfHkDisplay::MagicDisplaySurfHkDisplay_t lastView=MagicDisplaySurfHkDisplay::kNotAView;
  static int lastKelvinView=1;
  gStyle->SetOptTitle(0);
  TPad *canAvgSurfHk;
  TPad *plotPad;
  if(!useCan) {
    canAvgSurfHk = (TPad*) gROOT->FindObject("canAvgSurfHk");
    if(!canAvgSurfHk) {
      canAvgSurfHk = new TCanvas("canAvgSurfHk","canAvgSurfHk",1000,600);
    }
    canAvgSurfHk->Clear();
    canAvgSurfHk->SetTopMargin(0);
    plotPad = new TPad("canAvgSurfHkMain","canAvgSurfHkMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  //  std::cout << fAvgSurfPhiView << "\t" << lastView << std::endl;
  if(fAvgSurfDisplay!=lastView || lastKelvinView!=fAvgSurfKelvinView) {
    plotPad->Clear();
    avgSurfHkFramey[0]=0;
    avgSurfHkFramey[1]=0;
    avgSurfHkFramey[2]=0;    
  }
  lastView=fAvgSurfDisplay;
  lastKelvinView=fAvgSurfKelvinView;

    
  if(fAvgSurfDisplay==MagicDisplaySurfHkDisplay::kPhiView)
    getAvgSurfHkPhiCanvas(avgSurfPtr,plotPad);
  else if(fAvgSurfDisplay==MagicDisplaySurfHkDisplay::kSurfView)
    getAvgSurfHkSurfCanvas(avgSurfPtr,plotPad);
  if(useCan)
    return plotPad;
  else
    return canAvgSurfHk;
}

void AnitaRFCanvasMaker::getAvgSurfHkSurfCanvas(AveragedSurfHk *avgSurfPtr,TPad *plotPad) {
  //   plotPad->Clear();

  char actualName[180];
  const char *histNames[3]={"histAvgScaler","histAvgThreshold","histAvgRFPower"};
  const char *histTitles[3]={"Scaler","Threshold","RF Power"};

  if(histAvgSurfBadThreshold)
    delete histAvgSurfBadThreshold;
  //  histAvgSurfBadThreshold->Reset();
  //  else
  histAvgSurfBadThreshold = new TH1F("histAvgSurfBadThreshold","histAvgSurfBadThreshold",
				     ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
				     (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
  if(histAvgSurfBandMask)
    delete histAvgSurfBandMask;
  //  histAvgSurfBandMask->Reset();
  //  else
  histAvgSurfBandMask = new TH1F("histAvgSurfBandMask","histAvgSurfBandMask",
				 ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
				 (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
  Int_t maskedBands[ACTIVE_SURFS*SCALERS_PER_SURF]={0};

  for(int i=0;i<3;i++) {
    if(histAvgSurfHkStack[i])
      delete histAvgSurfHkStack[i];      
    histAvgSurfHkStack[i] = new THStack(histNames[i],histTitles[i]);
    sprintf(actualName,"%s_base",histNames[i]);
      

    for(int surf=0;surf<ACTIVE_SURFS;surf++){
      if(histAvgSurfHk[i][surf]) {
	delete histAvgSurfHk[i][surf];
	//	histAvgSurfHk[i][surf]->Reset();
      }
      //      else {
      sprintf(actualName,"%s_%d",histNames[i],surf);
      if(i<2) {
	histAvgSurfHk[i][surf]= new TH1F(actualName,histTitles[i],
					 ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
					 (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
      }
      else {
	histAvgSurfHk[i][surf]= new TH1F(actualName,histTitles[i],
					 ACTIVE_SURFS*RFCHAN_PER_SURF,-0.5,
					(ACTIVE_SURFS*RFCHAN_PER_SURF)-0.5);
      }
        
    }
  }

  Float_t maxVals[3]={0};

  for(int surf=0;surf<ACTIVE_SURFS;surf++) {
    for(int scl=0;scl<SCALERS_PER_SURF;scl++) {
      maskedBands[scl+surf*SCALERS_PER_SURF]=
	avgSurfPtr->isBandMasked(surf,scl);

      histAvgSurfHk[0][surf]->Fill(scl+surf*SCALERS_PER_SURF,
				avgSurfPtr->avgScaler[surf][scl]);
      
      Float_t error1=0;
      Float_t error2=0;
      if(avgSurfPtr->numHks>0) {
	error1=avgSurfPtr->rmsScaler[surf][scl];///sqrt(avgSurfPtr->numHks);
	error2=avgSurfPtr->rmsThresh[surf][scl];///sqrt(avgSurfPtr->numHks);
      }
      //      std::cout << error1 << "\t" << error2 << "\n";
      histAvgSurfHk[0][surf]->SetBinError(1+scl+surf*SCALERS_PER_SURF,
					     error1);
      histAvgSurfHk[1][surf]->Fill(scl+surf*SCALERS_PER_SURF,
				      avgSurfPtr->avgThresh[surf][scl]);
      histAvgSurfHk[1][surf]->SetBinError(1+scl+surf*SCALERS_PER_SURF,
					     error2);
      
      if(maxVals[0]<avgSurfPtr->avgScaler[surf][scl]+error1)
	maxVals[0]=avgSurfPtr->avgScaler[surf][scl]+error1;
      if(maxVals[1]<avgSurfPtr->avgThresh[surf][scl]+error2)
	maxVals[1]=avgSurfPtr->avgThresh[surf][scl]+error2;
      

    }
    for(int chan=0;chan<RFCHAN_PER_SURF;chan++) {
      Float_t error=0;
      if(avgSurfPtr->numHks>0) {
	if(fAvgSurfKelvinView==1) {
	  error=avgSurfPtr->getRMSRFPowerInK(surf,chan);///sqrt(avgSurfPtr->numHks);
	}
	else if(fAvgSurfKelvinView==2) {
	  error=avgSurfPtr->getMeasuredRMSRFPowerInK(surf,chan);///sqrt(avgSurfPtr->numHks);
	}	  
	else {
	  error=avgSurfPtr->rmsRFPower[surf][chan];///sqrt(avgSurfPtr->numHks);
	}
      }
      if(fAvgSurfKelvinView==1) {
	Double_t val=avgSurfPtr->getRFPowerInK(surf,chan);
	histAvgSurfHk[2][surf]->Fill(chan+surf*RFCHAN_PER_SURF,val);
	if(maxVals[2]<val+error)
	  maxVals[2]=val+error;
      }
      else if(fAvgSurfKelvinView==2) {
	Double_t val=avgSurfPtr->getMeasuredRMSRFPowerInK(surf,chan);
	histAvgSurfHk[2][surf]->Fill(chan+surf*RFCHAN_PER_SURF,val);
	if(maxVals[2]<val+error)
	  maxVals[2]=val+error;
      }
      else {
	histAvgSurfHk[2][surf]->Fill(chan+surf*RFCHAN_PER_SURF,
				     avgSurfPtr->avgRFPower[surf][chan]);
	if(maxVals[2]<avgSurfPtr->avgRFPower[surf][chan]+error)
	  maxVals[2]=avgSurfPtr->avgRFPower[surf][chan]+error;
      }
      histAvgSurfHk[2][surf]->SetBinError(1+chan+surf*RFCHAN_PER_SURF,
					     error);      
    }
    histAvgSurfHk[0][surf]->SetFillColor(getNiceColour(surf%2));
    histAvgSurfHk[1][surf]->SetFillColor(getNiceColour(surf%2));
    histAvgSurfHk[2][surf]->SetFillColor(getNiceColour(surf%2));
    //    histAvgSurfHk[0][surf]->SetLineColor(getNiceColour(surf%2));
    //    histAvgSurfHk[1][surf]->SetLineColor(getNiceColour(surf%2));
    //    histAvgSurfHk[2][surf]->SetLineColor(getNiceColour(surf%2));
    histAvgSurfHkStack[0]->Add(histAvgSurfHk[0][surf]);
    histAvgSurfHkStack[1]->Add(histAvgSurfHk[1][surf]);
    histAvgSurfHkStack[2]->Add(histAvgSurfHk[2][surf]);
  }   
  histAvgSurfBadThreshold->SetFillColor(kRed+1);
  histAvgSurfHkStack[1]->Add(histAvgSurfBadThreshold);

   
  if(!plotPad->cd(3)) {
    plotPad->Clear();
    avgSurfHkFramey[0]=0;
    avgSurfHkFramey[1]=0;
    avgSurfHkFramey[2]=0;    
    plotPad->SetRightMargin(0.0);
    plotPad->SetLeftMargin(0.1);
    plotPad->SetTopMargin(0.05);
    plotPad->SetBottomMargin(0.0);
    plotPad->Divide(1,3,0,0);
  }
   
  TLatex texy;
  texy.SetTextAlign(21);
  texy.SetTextAngle(270);
  texy.SetTextSize(0.17);

   

  for(int i=0;i<3;i++) {
    //    Double_t maxVal=histAvgSurfHkStack[i]->GetMaximum("nostack");
    Double_t maxVal=maxVals[i];
    TPad* paddy = (TPad*)plotPad->cd(i+1);      
    if(!avgSurfHkFramey[i]) {      
      paddy->SetBottomMargin(0.15);
      paddy->Clear();
      paddy->SetBottomMargin(0.18);
      paddy->SetRightMargin(0.05);
      
    if(i<2) {
      avgSurfHkFramey[i]=paddy->DrawFrame(0,0,ACTIVE_SURFS*SCALERS_PER_SURF,maxVal*1.05);
    }
    else {
      avgSurfHkFramey[i]=paddy->DrawFrame(0,0,ACTIVE_SURFS*RFCHAN_PER_SURF,maxVal*1.05);
    }


  

    if(i==0)
      avgSurfHkFramey[i]->GetYaxis()->SetTitle("Rate (kHz)");
    if(i==1)
      avgSurfHkFramey[i]->GetYaxis()->SetTitle("Threshold (DAC)");
    if(i==2) {
      if(fAvgSurfKelvinView) {
	avgSurfHkFramey[i]->GetYaxis()->SetTitle("RF Power (K)");
      }
      else {
	avgSurfHkFramey[i]->GetYaxis()->SetTitle("RF Power (ADC)");
      }
    }
    avgSurfHkFramey[i]->GetXaxis()->SetTitle("Logical Channel");
    avgSurfHkFramey[i]->GetYaxis()->SetNoExponent(1);
    avgSurfHkFramey[i]->GetXaxis()->SetLabelSize(0.08);
    avgSurfHkFramey[i]->GetYaxis()->SetLabelSize(0.08);
    avgSurfHkFramey[i]->GetXaxis()->SetTitleSize(0.09);
    avgSurfHkFramey[i]->GetYaxis()->SetTitleSize(0.09);
    avgSurfHkFramey[i]->GetXaxis()->SetTitleOffset(0.8);
    avgSurfHkFramey[i]->GetYaxis()->SetTitleOffset(0.6);
    avgSurfHkFramey[i]->GetYaxis()->SetTickLength(-0.01);
    avgSurfHkFramey[i]->GetYaxis()->SetLabelOffset(0.015);
    avgSurfHkFramey[i]->GetXaxis()->SetTickLength(-0.01);
    avgSurfHkFramey[i]->GetXaxis()->SetLabelOffset(0.015);
    avgSurfHkFramey[i]->SetTitle("");
    }
    else {
      if(!fFixAvgSurfYScale) {
	avgSurfHkFramey[i]->GetYaxis()->SetRangeUser(0,maxVal*1.05);
      }
      gPad->SetLogy(0);
      if(fAvgSurfUseLog) {
	avgSurfHkFramey[i]->GetYaxis()->SetRangeUser(0.1,maxVal*2);
	gPad->SetLogy(1);
      }
    }
    for(int surf=0;surf<ACTIVE_SURFS;surf++) {
      histAvgSurfHk[i][surf]->SetMarkerStyle(0);
      histAvgSurfHk[i][surf]->Draw("same bar e1");
    }
    if(i==0) {
      maxVal*=1.1;
      for(int i=0;i<ACTIVE_SURFS*SCALERS_PER_SURF;i++) {
	if(maskedBands[i])
	  histAvgSurfBandMask->Fill(i,maxVal);
      }
      histAvgSurfBandMask->SetLineWidth(0);
      histAvgSurfBandMask->SetFillColor(kGray);
      histAvgSurfBandMask->SetFillStyle(3001);
      histAvgSurfBandMask->Draw("same");
    }
  }
  plotPad->Update();
}



void AnitaRFCanvasMaker::getAvgSurfHkPhiCanvas(AveragedSurfHk *avgSurfPtr,TPad *plotPad) {
  //   plotPad->Clear();
  if(!fARFCGeomTool)
    fARFCGeomTool=AnitaGeomTool::Instance();
  char actualName[180];
  const char *histNames[3]={"histAvgScalerPhi","histAvgThresholdPhi","histAvgRFPowerPhi"};
  const char *histTitles[3]={"Scaler","Threshold","RF Power"};

  if(histAvgSurfBadThreshold)
    delete histAvgSurfBadThreshold;
  //    histAvgSurfBadThreshold->Reset();
  //  else
  histAvgSurfBadThreshold = new TH1F("histAvgSurfBadThreshold","histAvgSurfBadThreshold",
				     ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
				     (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
  if(histAvgSurfBandMask)
    delete histAvgSurfBandMask;
  //  histAvgSurfBandMask->Reset();
  //  else
  histAvgSurfBandMask = new TH1F("histAvgSurfBandMask","histAvgSurfBandMask",
				 ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
				 (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
  Int_t maskedBands[ACTIVE_SURFS*SCALERS_PER_SURF]={0};

  for(int i=0;i<3;i++) {
    if(histAvgSurfHkPhiStack[i])
      delete histAvgSurfHkPhiStack[i];      
    histAvgSurfHkPhiStack[i] = new THStack(histNames[i],histTitles[i]);
    sprintf(actualName,"%s_base",histNames[i]);
      

    for(int phi=0;phi<PHI_SECTORS;phi++){
      if(histAvgSurfHkPhi[i][phi]) {
	delete histAvgSurfHkPhi[i][phi];
	//	histAvgSurfHkPhi[i][phi]->Reset();
      }
      //      else {
	sprintf(actualName,"%s_%d",histNames[i],phi);
	if(i<2) {
	  histAvgSurfHkPhi[i][phi]= new TH1F(actualName,histTitles[i],
				       ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
				       (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
	}
	else {
	  histAvgSurfHkPhi[i][phi]= new TH1F(actualName,histTitles[i],
					  3*PHI_SECTORS*2,-0.5,
					  (3*PHI_SECTORS*2)-0.5);
	}

	//      }
    }
  }

  Float_t maxVals[3]={0};
   
  for(int ring=0;ring<3;ring++) {
    for(int phi=0;phi<PHI_SECTORS;phi++) {     
      if(ring==2) {
	if(phi%2)
	  continue;
      }
      for(int band=0;band<4;band++) {
	Int_t index=band+4*phi+64*ring;
	if(ring==2)
	  index=128 + band+4*(phi/2);
	maskedBands[index]=
	  avgSurfPtr->isBandMasked(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band);
	 
	histAvgSurfHkPhi[0][phi]->Fill(index,avgSurfPtr->getScaler(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band));
	histAvgSurfHkPhi[0][phi]->SetBinError(index+1,avgSurfPtr->getScalerRMS(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band));
	 
	if(avgSurfPtr->getScaler(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band)>maxVals[0])
	  maxVals[0]=avgSurfPtr->getScaler(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band);

	histAvgSurfHkPhi[1][phi]->Fill(index,avgSurfPtr->getThreshold(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band));
	histAvgSurfHkPhi[1][phi]->SetBinError(index+1,avgSurfPtr->getThresholdRMS(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band));
	

	if(avgSurfPtr->getThreshold(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band)>maxVals[1])
	  maxVals[1]=avgSurfPtr->getThreshold(phi,(AnitaRing::AnitaRing_t)ring,(AnitaBand::AnitaBand_t)band);
	
	 
      }
       
      //Now for the RF power
      //Argh
      for(int pol=0;pol<2;pol++) {
	Int_t surf,rfChan;
	Int_t chanIndex=
	  fARFCGeomTool->getChanIndexFromRingPhiPol((AnitaRing::AnitaRing_t)ring,phi,(AnitaPol::AnitaPol_t)pol);
	fARFCGeomTool->getSurfChanFromChanIndex(chanIndex,surf,rfChan);
	Int_t index=pol + 2 *phi + 32*ring;

	Float_t error=0;
	if(avgSurfPtr->numHks>0) {
	  if(fAvgSurfKelvinView==1) {
	    error=avgSurfPtr->getRMSRFPowerInK(surf,rfChan);///sqrt(avgSurfPtr->numHks);
	  }
	  else if(fAvgSurfKelvinView==2) {
	    error=avgSurfPtr->getMeasuredRMSRFPowerInK(surf,rfChan);///sqrt(avgSurfPtr->numHks);
	  }
	  else {
	    error=avgSurfPtr->rmsRFPower[surf][rfChan];///sqrt(avgSurfPtr->numHks);
	  }
	}
	if(fAvgSurfKelvinView==1) {
	  Double_t val=avgSurfPtr->getRFPowerInK(surf,rfChan);
	  histAvgSurfHkPhi[2][phi]->Fill(index,val);
	  if(maxVals[2]<val+error)
	    maxVals[2]=val+error;
	}
	else if(fAvgSurfKelvinView==2) {
	  Double_t val=avgSurfPtr->getMeasuredRFPowerInK(surf,rfChan);
	  histAvgSurfHkPhi[2][phi]->Fill(index,val);
	  if(maxVals[2]<val+error)
	    maxVals[2]=val+error;
	}
	else {
	  histAvgSurfHkPhi[2][phi]->Fill(index,
					 avgSurfPtr->avgRFPower[surf][rfChan]);
	  if(maxVals[2]<avgSurfPtr->avgRFPower[surf][rfChan]+error)
	    maxVals[2]=avgSurfPtr->avgRFPower[surf][rfChan]+error;
	}
	histAvgSurfHkPhi[2][phi]->SetBinError(index+1,
					       error);  	
	//	histAvgSurfHkPhi[2][phi]->Fill(index,
	//				 avgSurfPtr->avgRFPower[surf][rfChan]);
	 
	
      }
    }
  }
  for(int phi=0;phi<PHI_SECTORS;phi++) {
    histAvgSurfHkPhi[0][phi]->SetFillColor(getNiceColour(phi%4));            
    histAvgSurfHkPhi[1][phi]->SetFillColor(getNiceColour(phi%4));
    histAvgSurfHkPhi[2][phi]->SetFillColor(getNiceColour(phi%4));
    histAvgSurfHkPhiStack[0]->Add(histAvgSurfHkPhi[0][phi]);
    histAvgSurfHkPhiStack[1]->Add(histAvgSurfHkPhi[1][phi]);
    histAvgSurfHkPhiStack[2]->Add(histAvgSurfHkPhi[2][phi]);
  }   
  histAvgSurfBadThreshold->SetFillColor(kRed+1);
  histAvgSurfHkPhiStack[1]->Add(histAvgSurfBadThreshold);
     
   
  if(!plotPad->cd(3)) {
    plotPad->Clear();    
    avgSurfHkFramey[0]=0;
    avgSurfHkFramey[1]=0;
    avgSurfHkFramey[2]=0;    
    plotPad->SetRightMargin(0.0);
    plotPad->SetLeftMargin(0.1);
    plotPad->SetTopMargin(0.05);
    plotPad->SetBottomMargin(0.0);
    plotPad->Divide(1,3,0,0);
  }
     
  TLatex texy;
  texy.SetTextAlign(21);
  texy.SetTextAngle(270);
  texy.SetTextSize(0.17);
     

  char label[10];
  for(int i=0;i<3;i++) {
    //    Double_t maxVal=histAvgSurfHkPhiStack[i]->GetMaximum("nostack");
    Double_t maxVal=maxVals[i];
    TPad* paddy = (TPad*)plotPad->cd(i+1);      
    if(!avgSurfHkFramey[i]) {      
      paddy->SetBottomMargin(0.15);
      paddy->Clear();
      paddy->SetBottomMargin(0.18);
      paddy->SetRightMargin(0.05);
      
      if(i<2) {
	avgSurfHkFramey[i]=paddy->DrawFrame(0,0,ACTIVE_SURFS*SCALERS_PER_SURF,maxVal*1.05);
	//      avgSurfHkFramey[i]->GetXaxis()->SetNdivisions(PHI_SECTORS);
      }
      else {
	avgSurfHkFramey[i]=paddy->DrawFrame(0,0,3*PHI_SECTORS*2,maxVal*1.05);
      }
      

      

      if(i==0)
	avgSurfHkFramey[i]->GetYaxis()->SetTitle("Rate (kHz)");
      if(i==1)
	avgSurfHkFramey[i]->GetYaxis()->SetTitle("Threshold (DAC)");
      if(i==2) {
	if(fAvgSurfKelvinView) {
	  avgSurfHkFramey[i]->GetYaxis()->SetTitle("RF Power (K)");
	}
	else {
	  avgSurfHkFramey[i]->GetYaxis()->SetTitle("RF Power (ADC)");
	}
      }
    
      avgSurfHkFramey[i]->GetXaxis()->SetTitle("Phi Sector");
      if(i<2) {
	//      avgSurfHkFramey[i]->GetXaxis()->SetNdivisions(-16);
	avgSurfHkFramey[i]->SetBins(ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
		   		 (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
	for(int phi=0;phi<16;phi+=2) {
	  sprintf(label,"%d",phi+1);
	  avgSurfHkFramey[i]->GetXaxis()->SetBinLabel(4*phi+2,label);
	  avgSurfHkFramey[i]->GetXaxis()->SetBinLabel(4*phi+66,label);
	  if(phi%4==0) {
	    avgSurfHkFramey[i]->GetXaxis()->SetBinLabel(2*phi+130,label);
	  }
	}

      }
      else {
	avgSurfHkFramey[i]->SetBins(3*PHI_SECTORS*2,-0.5,
				 (3*PHI_SECTORS*2)-0.5);
	for(int phi=0;phi<16;phi+=2) {
	  sprintf(label,"%d",phi+1);	  
	  avgSurfHkFramey[i]->GetXaxis()->SetBinLabel(2*phi +1,label);
	  avgSurfHkFramey[i]->GetXaxis()->SetBinLabel(2*phi +33,label);
	  avgSurfHkFramey[i]->GetXaxis()->SetBinLabel(2*phi +65,label);
	}
      }
	

    avgSurfHkFramey[i]->GetYaxis()->SetNoExponent(1);
    avgSurfHkFramey[i]->GetXaxis()->SetLabelSize(0.08);
    avgSurfHkFramey[i]->GetYaxis()->SetLabelSize(0.08);
    avgSurfHkFramey[i]->GetXaxis()->SetTitleSize(0.09);
    avgSurfHkFramey[i]->GetYaxis()->SetTitleSize(0.09);
    avgSurfHkFramey[i]->GetXaxis()->SetTitleOffset(0.8);
    avgSurfHkFramey[i]->GetYaxis()->SetTitleOffset(0.6);
    avgSurfHkFramey[i]->GetYaxis()->SetTickLength(-0.01);
    avgSurfHkFramey[i]->GetYaxis()->SetLabelOffset(0.015);
    avgSurfHkFramey[i]->GetXaxis()->SetTickLength(-0.01);
    avgSurfHkFramey[i]->GetXaxis()->SetLabelOffset(0.015);
    avgSurfHkFramey[i]->SetTitle("");
    }
    else {
      if(!fFixAvgSurfYScale) {
	avgSurfHkFramey[i]->GetYaxis()->SetRangeUser(0,maxVal*1.05);
      }
      gPad->SetLogy(0);
      if(fAvgSurfUseLog) {
	avgSurfHkFramey[i]->GetYaxis()->SetRangeUser(0.1,maxVal*2);
	gPad->SetLogy(1);
      }
    }
    for(int phi=0;phi<PHI_SECTORS;phi++) {
      histAvgSurfHkPhi[i][phi]->SetMarkerStyle(0);
      histAvgSurfHkPhi[i][phi]->Draw("same bar e1");
    }
    if(i==0) {
      maxVal*=1.1;
      for(int i=0;i<ACTIVE_SURFS*SCALERS_PER_SURF;i++) {
	if(maskedBands[i])
	  histAvgSurfBandMask->Fill(i,maxVal);
      }
      histAvgSurfBandMask->SetLineWidth(0);
      histAvgSurfBandMask->SetFillColor(kGray);
      histAvgSurfBandMask->SetFillStyle(3001);
      histAvgSurfBandMask->Draw("same");
    }
  }
   

   

  plotPad->Update();
}

TPad *AnitaRFCanvasMaker::getAvgSurfInfoCanvas(AveragedSurfHk *avgSurfPtr,TPad *useCan)
{
  char textLabel[180];
  TPad *topPad;
  if(!useCan) {
    topPad = new TPad("padAvgSurfInfo","padAvgSurfInfo",0.2,0.9,0.8,1);
    topPad->Draw();
  }
  else {
    topPad=useCan;
  } 
  topPad->Clear();
  topPad->SetTopMargin(0.05);
  topPad->Divide(2,1);
  topPad->cd(1);
  TPaveText *leftPave = new TPaveText(0.05,0,0.95,0.9);
  leftPave->SetBorderSize(0);
  leftPave->SetFillColor(0);
  leftPave->SetTextAlign(13);
  sprintf(textLabel,"Run:    %d",avgSurfPtr->run);
  TText *runText = leftPave->AddText(textLabel);
  runText->SetTextColor(50);
  TTimeStamp avgSurfTime((time_t)avgSurfPtr->realTime,0);
  sprintf(textLabel,"Time: %s",avgSurfTime.AsString("s"));
  TText *timeText = leftPave->AddText(textLabel);
  timeText->SetTextColor(1);
  sprintf(textLabel,"Num Hks: %d",avgSurfPtr->numHks);
  leftPave->AddText(textLabel);
  sprintf(textLabel,"Delta T: %d",avgSurfPtr->deltaT);
  leftPave->AddText(textLabel);
  leftPave->Draw();

  topPad->cd(2);
  TPaveText *rightPave = new TPaveText(0.05,0,0.95,0.9);
  rightPave->SetBorderSize(0);
  rightPave->SetFillColor(0);
  rightPave->SetTextAlign(13);
  sprintf(textLabel,"Global: %d",avgSurfPtr->globalThreshold);
  TText *globalText = rightPave->AddText(textLabel);
  globalText->SetTextColor(1);
  sprintf(textLabel,"Goal Rates: (%d %d %d %d) kHz",
	  avgSurfPtr->scalerGoals[0],avgSurfPtr->scalerGoals[1],
	  avgSurfPtr->scalerGoals[2],avgSurfPtr->scalerGoals[3]);
  TText *goalText = rightPave->AddText(textLabel);
  goalText->SetTextColor(1);
  sprintf(textLabel,"Had Error: %#x (%d)",avgSurfPtr->hadError,avgSurfPtr->hadError);
  TText *errorText = rightPave->AddText(textLabel);
  errorText->SetTextColor(1);
  rightPave->Draw();   
  return topPad;

}

void AnitaRFCanvasMaker::addToTimePlots(SurfHk *surfPtr) {
  //  std::cout <<fNumSurfHks << "\n";
  if(fNumSurfHks<MAX_SURF_HK_TIME_POINTS) {
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int scl=0;scl<SCALERS_PER_SURF;scl++) {
	fScalerVals[surf][scl][fNumSurfHks]=surfPtr->scaler[surf][scl];
	fThreshVals[surf][scl][fNumSurfHks]=surfPtr->threshold[surf][scl];
      }
      for(int chan=0;chan<RFCHAN_PER_SURF;chan++) {
	fRfPowVals[surf][chan][fNumSurfHks]=surfPtr->getRFPowerInK(surf,chan);
      }
    }
    fTimeVals[fNumSurfHks]=surfPtr->realTime+Double_t(1e-6*surfPtr->payloadTimeUs);
  }
  else {
    for(int surf=0;surf<NUM_SURF;surf++) {
      for(int scl=0;scl<SCALERS_PER_SURF;scl++) {
	memmove(&fScalerVals[surf][scl][0],&fScalerVals[surf][scl][1],sizeof(Double_t)*(MAX_SURF_HK_TIME_POINTS-1));
	memmove(&fThreshVals[surf][scl][0],&fThreshVals[surf][scl][1],sizeof(Double_t)*(MAX_SURF_HK_TIME_POINTS-1));
	fScalerVals[surf][scl][MAX_SURF_HK_TIME_POINTS-1]=surfPtr->scaler[surf][scl];
	fThreshVals[surf][scl][MAX_SURF_HK_TIME_POINTS-1]=surfPtr->threshold[surf][scl];
      }
      for(int chan=0;chan<RFCHAN_PER_SURF;chan++) {
	memmove(&fRfPowVals[surf][chan][0],&fRfPowVals[surf][chan][1],sizeof(Double_t)*(MAX_SURF_HK_TIME_POINTS-1));
	fRfPowVals[surf][chan][MAX_SURF_HK_TIME_POINTS-1]=surfPtr->getRFPowerInK(surf,chan);
      }
    }
    memmove(&fTimeVals[0],&fTimeVals[1],sizeof(Double_t)*(MAX_SURF_HK_TIME_POINTS-1));
    fTimeVals[MAX_SURF_HK_TIME_POINTS-1]=surfPtr->realTime;    
  }
  fNumSurfHks++;
}
