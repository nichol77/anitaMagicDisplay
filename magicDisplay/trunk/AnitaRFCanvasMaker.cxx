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
#include "SurfHk.h"

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


AnitaRFCanvasMaker*  AnitaRFCanvasMaker::fgInstance = 0;
AnitaGeomTool *fARFCGeomTool=0;

TH1F *histTurfRate[5]={0};
TH1F *histAntMask[2]={0};

TH1F *histSurfHk[3][ACTIVE_SURFS]={0};
TH1F *histSurfHkPhi[3][PHI_SECTORS]={0};
TH1F *histBadThreshold;
TH1F *histBandMask;
THStack *histSurfHkStack[3];
THStack *histSurfHkPhiStack[3];
TH1F *surfHkFramey[3]={0};
AnitaRFCanvasMaker::AnitaRFCanvasMaker()
{
  //Default constructor
  fL1Max=0;
  fL1Min=0;
  fL2Max=0;
  fL2Min=0;
  fL3Max=0;
  fL3Min=0;
  fUseLog=0;
  fFixTurfYScale=0;
  fFixSurfYScale=0;
  fSurfPhiView=0;
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

  char *histNames[5]={"histUpperL1","histLowerL1","histUpperL2","histLowerL2","histL3"};
  char *histTitles[5]={"Upper L1","Lower L1","Upper L2","Lower L2","L3"};

  for(int i=0;i<5;i++) {
    if(histTurfRate[i]) {
      histTurfRate[i]->Reset();
    }
    else {
      histTurfRate[i]= new TH1F(histNames[i],histTitles[i],PHI_SECTORS,0.5,PHI_SECTORS+0.5);
    }
  }
  char *maskNames[2]={"histAntMask0","histAntMask1"};
  for(int i=0;i<2;i++) {
    if(histAntMask[i])
      histAntMask[i]->Reset();
    else {
      histAntMask[i] = new TH1F(maskNames[i],maskNames[i],PHI_SECTORS,0.5,PHI_SECTORS+0.5);
    }
  }


  Double_t l1Max=0;
  Double_t l2Max=0;
  int maskedAnts[2][PHI_SECTORS]={0};
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
    
      
    histTurfRate[0]->Fill(phi+1,1e-3*turfPtr->getL1Rate(phi,0));
    histTurfRate[1]->Fill(phi+1,1e-3*turfPtr->getL1Rate(phi,1));
    histTurfRate[2]->Fill(phi+1,turfPtr->getL2Rate(phi,0));
    histTurfRate[3]->Fill(phi+1,turfPtr->getL2Rate(phi,1));
    histTurfRate[4]->Fill(phi+1,turfPtr->getL3Rate(phi));
  }
  l1Max*=1.1e-3;
  l2Max*=1.1;
  plotPad->SetRightMargin(0.0);
  plotPad->SetLeftMargin(0.1);
  plotPad->SetTopMargin(0.05);
  plotPad->SetBottomMargin(0.1);
  plotPad->Divide(1,5,0,0);
   
  TLatex texy;
  texy.SetTextAlign(21);
  texy.SetTextAngle(270);
  texy.SetTextSize(0.17);

  if(!fFixTurfYScale) {
    if(fL1Max)
      l1Max=fL1Max;
    histTurfRate[0]->SetMaximum(l1Max);
    histTurfRate[1]->SetMaximum(l1Max);
    if(fL2Max)
      l2Max=fL2Max;
    histTurfRate[2]->SetMaximum(l2Max);
    histTurfRate[3]->SetMaximum(l2Max);
  }
  

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


  for(int i=0;i<5;i++) {
    TPad* paddy = (TPad*)plotPad->cd(i+1);
    //      if(i==4)
    paddy->SetBottomMargin(0.12);
    if(i==4)
      paddy->SetBottomMargin(0.18);
    paddy->SetRightMargin(0.05);
    histTurfRate[i]->SetFillStyle(3001);
    histTurfRate[i]->SetFillColor(getNiceColour(i));
    histTurfRate[i]->SetLineColor(getNiceColour(i));
    if(i==4)      
      histTurfRate[i]->SetXTitle("Phi Sector");
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
      histAntMask[i]->Draw("same");
    }
    texy.DrawTextNDC(0.96,0.5,histTitles[i]);
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
  static int lastView=0;
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
  if(fSurfPhiView!=lastView) {
    plotPad->Clear();
    surfHkFramey[0]=0;
    surfHkFramey[1]=0;
    surfHkFramey[2]=0;    
    lastView=fSurfPhiView;
  }
    
  if(fSurfPhiView)
    getSurfHkPhiCanvas(surfPtr,plotPad);
  else 
    getSurfHkSurfCanvas(surfPtr,plotPad);
  if(useCan)
    return plotPad;
  else
    return canSurfHk;
}

void AnitaRFCanvasMaker::getSurfHkSurfCanvas(SurfHk *surfPtr,TPad *plotPad) {
  //   plotPad->Clear();

  char actualName[180];
  char *histNames[3]={"histScaler","histThreshold","histRFPower"};
  char *histTitles[3]={"Scaler","Threshold","RF Power"};

  if(histBadThreshold)
    histBadThreshold->Reset();
  else
    histBadThreshold = new TH1F("histBadThreshold","histBadThreshold",
				ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
				(ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
  if(histBandMask)
    histBandMask->Reset();
  else
    histBandMask = new TH1F("histBandMask","histBandMask",
			    ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
			    (ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
  Int_t maskedBands[ACTIVE_SURFS*SCALERS_PER_SURF]={0};

  for(int i=0;i<3;i++) {
    if(histSurfHkStack[i])
      delete histSurfHkStack[i];      
    histSurfHkStack[i] = new THStack(histNames[i],histTitles[i]);
    sprintf(actualName,"%s_base",histNames[i]);
      

    for(int surf=0;surf<ACTIVE_SURFS;surf++){
      if(histSurfHk[i][surf]) {
	histSurfHk[i][surf]->Reset();
      }
      else {
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
      histSurfHk[2][surf]->Fill(chan+surf*RFCHAN_PER_SURF,
				surfPtr->rfPower[surf][chan]);

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
    if(i==2)
      surfHkFramey[i]->GetYaxis()->SetTitle("RF Power (ADC)");
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
  char *histNames[3]={"histScaler","histThreshold","histRFPower"};
  char *histTitles[3]={"Scaler","Threshold","RF Power"};

  if(histBadThreshold)
    histBadThreshold->Reset();
  else
    histBadThreshold = new TH1F("histBadThreshold","histBadThreshold",
				ACTIVE_SURFS*SCALERS_PER_SURF,-0.5,
				(ACTIVE_SURFS*SCALERS_PER_SURF)-0.5);
  if(histBandMask)
    histBandMask->Reset();
  else
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
	histSurfHkPhi[i][phi]->Reset();
      }
      else {
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
	histSurfHkPhi[2][phi]->Fill(index,
				 surfPtr->rfPower[surf][rfChan]);
	 
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
      if(i==2)
	surfHkFramey[i]->GetYaxis()->SetTitle("RF Power (ADC)");
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
  topPad->cd();
  TPaveText *leftPave = new TPaveText(0.05,0,0.5,0.9);
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


  TPaveText *rightPave = new TPaveText(0.55,0,0.9,0.9);
  rightPave->SetBorderSize(0);
  rightPave->SetFillColor(0);
  rightPave->SetTextAlign(13);
  sprintf(textLabel,"Global: %d",surfPtr->globalThreshold);
  TText *globalText = rightPave->AddText(textLabel);
  globalText->SetTextColor(1);
  sprintf(textLabel,"Goal Rates: (%d %d %d %d) kHz",
	  surfPtr->scalerGoals[0],surfPtr->scalerGoals[1],
	  surfPtr->scalerGoals[2],surfPtr->scalerGoals[2]);
  TText *goalText = rightPave->AddText(textLabel);
  goalText->SetTextColor(1);
  sprintf(textLabel,"Error Flag: %#x",surfPtr->errorFlag);
  TText *errorText = rightPave->AddText(textLabel);
  errorText->SetTextColor(1);
  rightPave->Draw();   
  return topPad;

}
