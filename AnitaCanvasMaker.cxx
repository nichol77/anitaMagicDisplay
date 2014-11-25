//////////////////////////////////////////////////////////////////////////////
/////  AnitaCanvasMaker.cxx        ANITA Event Canvas make               /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for making pretty event canvases for ANITA-II            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include "AnitaCanvasMaker.h"
#include "AnitaConventions.h"
#include "AnitaGeomTool.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "WaveformGraph.h"
#include "FFTGraph.h"


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
#include "TStyle.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"
#include "TList.h"
#include "TFile.h"
#include "TObject.h"
#include "TTimeStamp.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TView3D.h"

#include "FFTtools.h"


AnitaCanvasMaker*  AnitaCanvasMaker::fgInstance = 0;
AnitaGeomTool *fACMGeomTool=0;


int phiMap[6][8]={{0,2,4,6,8,10,12,14},
		  {1,3,5,7,9,11,13,15},
		  {0,2,4,6,8,10,12,14},
		  {1,3,5,7,9,11,13,15},
		  {0,2,4,6,8,10,12,14},
		  {1,3,5,7,9,11,13,15}};


int rowMap[6]={0,0,1,1,2,2};

AnitaRing::AnitaRing_t ringMap[6]={AnitaRing::kTopRing,
				   AnitaRing::kTopRing,
				   AnitaRing::kMiddleRing,
				   AnitaRing::kMiddleRing,
				   AnitaRing::kBottomRing,
				   AnitaRing::kBottomRing};


WaveformGraph *grSurf[ACTIVE_SURFS][CHANNELS_PER_SURF]={{0}};
WaveformGraph *grSurfFiltered[ACTIVE_SURFS][CHANNELS_PER_SURF]={{0}};
WaveformGraph *grSurfHilbert[ACTIVE_SURFS][CHANNELS_PER_SURF]={{0}};
FFTGraph *grSurfFFT[ACTIVE_SURFS][CHANNELS_PER_SURF]={{0}};
FFTGraph *grSurfAveragedFFT[ACTIVE_SURFS][CHANNELS_PER_SURF]={{0}};


AnitaCanvasMaker::AnitaCanvasMaker(WaveCalType::WaveCalType_t calType)
{
  //Default constructor
  fACMGeomTool=AnitaGeomTool::Instance();
  fPassBandFilter=0;
  fNotchFilter=0;
  fLowPassEdge=200;
  fHighPassEdge=1200;
  fLowNotchEdge=235;
  fHighNotchEdge=500;
  fMinVoltLimit=-60;
  fMaxVoltLimit=60;
  fPhiMax=0;
  fMinVertVoltLimit=-60;
  fMaxVertVoltLimit=60;
  fMinClockVoltLimit=-200;
  fMaxClockVoltLimit=200;
  fAutoScale=1;
  fAutoScaleNeg=1;
  fMinTimeLimit=0;
  fMaxTimeLimit=100;
  fMinPowerLimit=-60;
  fMaxPowerLimit=40;
  fMinFreqLimit=0;
  fMaxFreqLimit=1200;
  fCanvasView=MagicDisplayCanvasLayoutOption::kPhiVerticalOnly;
  fWaveformOption=MagicDisplayFormatOption::kWaveform;
  fRedoEventCanvas=0;
  //fRedoSurfCanvas=0;
  fLastCanvasView=MagicDisplayCanvasLayoutOption::kPhiVerticalOnly;
  fLastWaveformFormat=MagicDisplayFormatOption::kWaveform;
  fNewEvent=1;
  fCalType=calType;
  fgInstance=this;
  memset(grSurf,0,sizeof(WaveformGraph*)*ACTIVE_SURFS*CHANNELS_PER_SURF);
  memset(grSurfFiltered,0,sizeof(WaveformGraph*)*ACTIVE_SURFS*CHANNELS_PER_SURF);
  memset(grSurfHilbert,0,sizeof(WaveformGraph*)*ACTIVE_SURFS*CHANNELS_PER_SURF);
  memset(grSurfFFT,0,sizeof(FFTGraph*)*ACTIVE_SURFS*CHANNELS_PER_SURF);
  memset(grSurfAveragedFFT,0,sizeof(FFTGraph*)*ACTIVE_SURFS*CHANNELS_PER_SURF);
  fAnitaGeomFile=0;
  fAnitaGeomManager=0;
  switch(fCalType) {
  case WaveCalType::kNoCalib:
  case WaveCalType::kJustUnwrap:
    fMinTimeLimit=0;
    fMaxTimeLimit=260;
    break;
  case WaveCalType::kAddPeds:
    fMinVoltLimit=0;
    fMaxVoltLimit=2000;
    fAutoScaleNeg=0;
    break;
  default:
    break;
  }


}

AnitaCanvasMaker::~AnitaCanvasMaker()
{
   //Default destructor
}



//______________________________________________________________________________
AnitaCanvasMaker*  AnitaCanvasMaker::Instance()
{
   //static function
   return (fgInstance) ? (AnitaCanvasMaker*) fgInstance : new AnitaCanvasMaker();
}


TPad *AnitaCanvasMaker::getEventInfoCanvas(UsefulAnitaEvent *evPtr, RawAnitaHeader *hdPtr, TPad *useCan)
{
   static UInt_t lastEventNumber=0;
   static TPaveText *leftPave=0;
   static TPaveText *midLeftPave=0;
   static TPaveText *midRightPave=0;
   static TPaveText *rightPave=0;


   if(!fACMGeomTool)
      fACMGeomTool=AnitaGeomTool::Instance();
   char textLabel[180];
   TPad *topPad;
   if(!useCan) {
      topPad = new TPad("padEventInfo","padEventInfo",0.2,0.9,0.8,1);
      topPad->Draw();
   }
   else {
      topPad=useCan;
   } 
   if(hdPtr->eventNumber != lastEventNumber) {
     fNewEvent=1;
     topPad->Clear();
     topPad->SetTopMargin(0.05);
     topPad->Divide(4,1);
     topPad->cd(1);
     if(leftPave) delete leftPave;
     leftPave = new TPaveText(0,0,1,0.9);
     leftPave->SetName("leftPave");
     leftPave->SetBorderSize(0);
     leftPave->SetFillColor(0);
     leftPave->SetTextAlign(13);
     sprintf(textLabel,"Run:    %d",hdPtr->run);
     TText *runText = leftPave->AddText(textLabel);
     runText->SetTextColor(50);
     sprintf(textLabel,"Event: %d",hdPtr->eventNumber);
     TText *eventText = leftPave->AddText(textLabel);
     eventText->SetTextColor(50);
     leftPave->Draw();


     topPad->cd(2);
     gPad->SetRightMargin(0);
     gPad->SetLeftMargin(0);
     if(midLeftPave) delete midLeftPave;
     midLeftPave = new TPaveText(0,0,0.99,0.9);
     midLeftPave->SetName("midLeftPave");
     midLeftPave->SetBorderSize(0);
     midLeftPave->SetTextAlign(13);
     TTimeStamp trigTime((time_t)hdPtr->triggerTime,(Int_t)hdPtr->triggerTimeNs);
     sprintf(textLabel,"Time: %s",trigTime.AsString("s"));
     TText *timeText = midLeftPave->AddText(textLabel);
     timeText->SetTextColor(1);
     sprintf(textLabel,"Trigger: %8.6f ms",1e-6*hdPtr->triggerTimeNs);
     TText *timeText2 = midLeftPave->AddText(textLabel);
     timeText2->SetTextColor(1);
     sprintf(textLabel,"Priority: %d -- Queue: %d",(hdPtr->priority&0xf0)/16,hdPtr->priority&0xf);
     midLeftPave->AddText(textLabel);
     //      sprintf(textLabel,"Lab Chip %d",labChip);
     //      midLeftPave->AddText(textLabel);
     midLeftPave->Draw();
     //     midLeftPave->Modified();
     gPad->Modified();
     gPad->Update();
     
     topPad->cd(3);
     if(midRightPave) delete midRightPave;
     midRightPave = new TPaveText(0,0,1,0.95);
     midRightPave->SetBorderSize(0);
     midRightPave->SetTextAlign(13);
     sprintf(textLabel,"Trig Num: %d -- Trig Type: %s",hdPtr->trigNum,hdPtr->trigTypeAsString());
     midRightPave->AddText(textLabel);
     if(hdPtr->errorFlag&0x1) {
       TText *slipText = midRightPave->AddText("Possible Sync Slip");
	 slipText->SetTextColor(6);   
     }
     sprintf(textLabel,"TURF: %d",hdPtr->turfEventId&0xfffff);
     midRightPave->AddText(textLabel);
     for(int surf=0;surf<ACTIVE_SURFS;surf++) {
       sprintf(textLabel,"SURF %d: %d",surf+1,evPtr->surfEventId[surf]&0xfffff);
       
       if(evPtr->surfEventId[surf]!=hdPtr->turfEventId) {
	 TText *slipText = midRightPave->AddText(textLabel);
	 slipText->SetTextColor(6);
       }
     }
     midRightPave->Draw();

     
     topPad->cd(4);
     if(rightPave) delete rightPave;
     rightPave = new TPaveText(0,0,1,0.95);
     rightPave->SetBorderSize(0);
     rightPave->SetTextAlign(13);
     sprintf(textLabel,"TURF This Hold: %#x",hdPtr->reserved[0]&0xf);
     rightPave->AddText(textLabel);
     sprintf(textLabel,"TURF Active Holds: %#x",(hdPtr->reserved[0]&0xf0)>>4);
     rightPave->AddText(textLabel);

     char labLetter[4]={'A','B','C','D'};
     sprintf(textLabel,"Labrador ");
     
     int good=1;
     for(int surf=0;surf<ACTIVE_SURFS;surf++) {
       sprintf(textLabel,"%s%c",textLabel,labLetter[evPtr->getLabChip(1+ 9*surf)]);
       if(evPtr->getLabChip(10) != evPtr->getLabChip(9*surf + 1))
	 good=0;
     }
     TText *labText=rightPave->AddText(textLabel);
     if(!good)
       labText->SetTextColor(6);

     sprintf(textLabel,"Phi Mask: %#x",(hdPtr->phiTrigMask));
     rightPave->AddText(textLabel);     
     rightPave->Draw();
     topPad->Update();
     topPad->Modified();
     
     
     
     lastEventNumber=hdPtr->eventNumber;
   }
      
   return topPad;
}


TPad *AnitaCanvasMaker::quickGetEventViewerCanvasForWebPlottter(UsefulAnitaEvent *evPtr,RawAnitaHeader *hdPtr, TPad *useCan)
{
  TPad *retCan=0;

  //  static Int_t lastEventView=0;

  if(fAutoScale) {
    fMinVoltLimit=0;
    fMaxVoltLimit=0;
    fMinVertVoltLimit=0;
    fMaxVertVoltLimit=0;
    fMinClockVoltLimit=0;
    fMaxClockVoltLimit=0;
  }


  for(int surf=0;surf<ACTIVE_SURFS;surf++){
    for(int chan=0;chan<CHANNELS_PER_SURF;chan++){
      
      if(grSurf[surf][chan]){
	delete grSurf[surf][chan];
	//grSurf[surf][chan]=0;
      }
      
      
      TGraph *grTemp = evPtr->getGraphFromSurfAndChan(surf,chan);
      grSurf[surf][chan] = new WaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
      //      std::cout << hdPtr->eventNumber << "\n";
      //      std::cout << surf << "\t" << chan << "\t" 
      //		<< grSurf[surf][chan]->GetRMS(2) << std::endl;

      if(fAutoScale) {
	  Int_t numPoints=grTemp->GetN();
	  Double_t *yVals=grTemp->GetY();
	  
	  if(chan<8) {
	    int ant=0;
	    AnitaPol::AnitaPol_t pol;
	    AnitaGeomTool::getAntPolFromSurfChan(surf,chan,ant,pol);

	    for(int i=0;i<numPoints;i++) {
	      
	      if(yVals[i]<fMinVoltLimit)
		fMinVoltLimit=yVals[i];
	      if(yVals[i]>fMaxVoltLimit)
		fMaxVoltLimit=yVals[i];
	      
	      if(pol==AnitaPol::kVertical) {
		if(yVals[i]<fMinVertVoltLimit)
		  fMinVertVoltLimit=yVals[i];
		if(yVals[i]>fMaxVertVoltLimit) {
		  fMaxVertVoltLimit=yVals[i];
		}

	      }	      
	    }
	  }
	  else {	     
	    for(int i=0;i<numPoints;i++) {	      
	      if(yVals[i]<fMinClockVoltLimit)
		fMinClockVoltLimit=yVals[i];
	      if(yVals[i]>fMaxClockVoltLimit)
		fMaxClockVoltLimit=yVals[i];	    
	    }
	  }
	  
	}

	delete grTemp;
	
    }
  }

  if(fAutoScale) {
      if(fMaxVoltLimit>-1*fMinVoltLimit) {
	fMinVoltLimit=-1*fMaxVoltLimit;
      }
      else {
	fMaxVoltLimit=-1*fMinVoltLimit;
      }

    if(fMaxVertVoltLimit>-1*fMinVertVoltLimit) {
      fMinVertVoltLimit=-1*fMaxVertVoltLimit;
    }
    else {
      fMaxVertVoltLimit=-1*fMinVertVoltLimit;
    }

   if(fMaxClockVoltLimit>-1*fMinClockVoltLimit) {
      fMinClockVoltLimit=-1*fMaxClockVoltLimit;
    }
    else {
      fMaxClockVoltLimit=-1*fMinClockVoltLimit;
    }
  }
    

  fRedoEventCanvas=0;

  retCan=AnitaCanvasMaker::getVerticalCanvasForWebPlotter(hdPtr,useCan);

 

  return retCan;

}

TPad *AnitaCanvasMaker::getEventViewerCanvas(UsefulAnitaEvent *evPtr,
					     RawAnitaHeader *hdPtr,
					     TPad *useCan)
{
  TPad *retCan=0;

  static UInt_t lastEventNumber=0;

  
  if(evPtr->eventNumber!=lastEventNumber || fWaveformOption!=fLastWaveformFormat) {
    lastEventNumber=evPtr->eventNumber;
    
    if(fAutoScale) {
      fMinVoltLimit=0;
      fMaxVoltLimit=0;
      fMinVertVoltLimit=0;
      fMaxVertVoltLimit=0;
      fMinClockVoltLimit=0;
      fMaxClockVoltLimit=0;
    }
    
    Double_t maxVal=0;

    for(int surf=0;surf<ACTIVE_SURFS;surf++){
      for(int chan=0;chan<CHANNELS_PER_SURF;chan++){

	if(grSurf[surf][chan]){
	  delete grSurf[surf][chan];
	  grSurf[surf][chan]=0;
	}
	if(grSurfHilbert[surf][chan]) {
	  delete grSurfHilbert[surf][chan];
	  grSurfHilbert[surf][chan]=0;
	}
	  
	if(grSurfFFT[surf][chan]){
	  delete grSurfFFT[surf][chan];
	  grSurfFFT[surf][chan]=0;
	}
	
	if(grSurfFiltered[surf][chan]) {
	  delete grSurfFiltered[surf][chan];
	  grSurfFiltered[surf][chan]=0;
	}


	TGraph *grTemp = evPtr->getGraphFromSurfAndChan(surf,chan);
	if(fAutoScale || fCanvasView==MagicDisplayCanvasLayoutOption::kPayloadView) {
	  Int_t numPoints=grTemp->GetN();
	  Double_t *yVals=grTemp->GetY();
	  
	  if(chan<8) {
	    int ant=0;
	    AnitaPol::AnitaPol_t pol;
	    AnitaGeomTool::getAntPolFromSurfChan(surf,chan,ant,pol);	    
	    Int_t phi=AnitaGeomTool::getPhiFromAnt(ant);

	    for(int i=0;i<numPoints;i++) {
	      
	      if(yVals[i]<fMinVoltLimit)
		fMinVoltLimit=yVals[i];
	      if(yVals[i]>fMaxVoltLimit)
		fMaxVoltLimit=yVals[i];
	      
	      if(pol==AnitaPol::kVertical) {
		if(yVals[i]<fMinVertVoltLimit)
		  fMinVertVoltLimit=yVals[i];
		if(yVals[i]>fMaxVertVoltLimit)
		  fMaxVertVoltLimit=yVals[i];
		if(yVals[i]>maxVal) {
		  maxVal=yVals[i];
		  fPhiMax=phi;
		}
	      }	      
	    }
	  }
	  else {	     
	    for(int i=0;i<numPoints;i++) {	      
	      if(yVals[i]<fMinClockVoltLimit)
		fMinClockVoltLimit=yVals[i];
	      if(yVals[i]>fMaxClockVoltLimit)
		fMaxClockVoltLimit=yVals[i];	    
	    }
	  }
	  
	}

	if(fPassBandFilter) {
	  TGraph *grDoubleTemp= FFTtools::simplePassBandFilter(grTemp,fLowPassEdge,fHighPassEdge);
	  delete grTemp;
	  grTemp=grDoubleTemp;
	}
	if(fNotchFilter) {
	  TGraph *grDoubleTemp= FFTtools::simpleNotchFilter(grTemp,fLowNotchEdge,fHighNotchEdge);
	  delete grTemp;
	  grTemp=grDoubleTemp;
	}

	grSurf[surf][chan] = new WaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());	
	if(fWaveformOption==MagicDisplayFormatOption::kFFT) {
	  TGraph *grTempFFT = grSurf[surf][chan]->getFFT();
	  grSurfFFT[surf][chan] = new FFTGraph(grTempFFT->GetN(),grTempFFT->GetX(),grTempFFT->GetY());
	  delete grTempFFT;
	}
	if(fWaveformOption==MagicDisplayFormatOption::kHilbertEnvelope) {
	  TGraph *grTempHilbert = grSurf[surf][chan]->getHilbert();
	  grSurfHilbert[surf][chan] = new WaveformGraph(grTempHilbert->GetN(),grTempHilbert->GetX(),grTempHilbert->GetY());
	  delete grTempHilbert;
	}	
	if(fWaveformOption==MagicDisplayFormatOption::kAveragedFFT) {
	  TGraph *grTempFFT = grSurf[surf][chan]->getFFT();
	  grSurfFFT[surf][chan] = new FFTGraph(grTempFFT->GetN(),grTempFFT->GetX(),grTempFFT->GetY());
	  if(!grSurfAveragedFFT[surf][chan]) {
	    grSurfAveragedFFT[surf][chan] = new FFTGraph(grTempFFT->GetN(),grTempFFT->GetX(),grTempFFT->GetY());
	  }
	  else {
	    grSurfAveragedFFT[surf][chan]->AddFFT(grSurfFFT[surf][chan]);
	  }
	  delete grTempFFT;
	}
	delete grTemp;
      }
    }
  }

  if(fAutoScale) {
    if(fAutoScaleNeg) {
      if(fMaxVoltLimit>-1*fMinVoltLimit) {
	fMinVoltLimit=-1*fMaxVoltLimit;
      }
      else {
	fMaxVoltLimit=-1*fMinVoltLimit;
      }
    }

    if(fMaxVertVoltLimit>-1*fMinVertVoltLimit) {
      fMinVertVoltLimit=-1*fMaxVertVoltLimit;
    }
    else {
      fMaxVertVoltLimit=-1*fMinVertVoltLimit;
    }

   if(fMaxClockVoltLimit>-1*fMinClockVoltLimit) {
      fMinClockVoltLimit=-1*fMaxClockVoltLimit;
    }
    else {
      fMaxClockVoltLimit=-1*fMinClockVoltLimit;
    }
    
  }

  fNewEvent=0;

  fRedoEventCanvas=0;
  if(fLastWaveformFormat!=fWaveformOption) fRedoEventCanvas=1;
  if(fLastCanvasView!=fCanvasView) fRedoEventCanvas=1;

  if(fCanvasView==MagicDisplayCanvasLayoutOption::kPhiVerticalOnly) retCan=AnitaCanvasMaker::getVerticalCanvas(hdPtr,useCan);
  else if(fCanvasView==MagicDisplayCanvasLayoutOption::kPhiHorizontalOnly) retCan=AnitaCanvasMaker::getHorizontalCanvas(hdPtr,useCan);
  else if(fCanvasView==MagicDisplayCanvasLayoutOption::kPhiCombined) retCan=AnitaCanvasMaker::getCombinedCanvas(hdPtr,useCan);
  else if(fCanvasView==MagicDisplayCanvasLayoutOption::kSurfOnly) retCan=AnitaCanvasMaker::getSurfChanCanvas(hdPtr,useCan);
  else if(fCanvasView==MagicDisplayCanvasLayoutOption::kPayloadView) retCan=AnitaCanvasMaker::getPayloadCanvas(hdPtr,useCan);

  fLastWaveformFormat=fWaveformOption;
  fLastCanvasView=fCanvasView;

  return retCan;

}



TPad *AnitaCanvasMaker::getHorizontalCanvas(RawAnitaHeader *hdPtr,
					    TPad *useCan)

{
   //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fACMGeomTool)
    fACMGeomTool=AnitaGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canHoriz=0;
  TPad *plotPad=0;
  if(!useCan) {
    canHoriz = (TPad*) gROOT->FindObject("canHoriz");
    if(!canHoriz) {
      canHoriz = new TCanvas("canHoriz","canHoriz",1000,600);
    }
    canHoriz->Clear();
    canHoriz->SetTopMargin(0);
    TPaveText *leftPave = new TPaveText(0.05,0.92,0.95,0.98);
    leftPave->SetBorderSize(0);
    sprintf(textLabel,"Event %d",hdPtr->eventNumber);
    TText *eventText = leftPave->AddText(textLabel);
    eventText->SetTextColor(50);
    leftPave->Draw();
    plotPad = new TPad("canHorizMain","canHorizMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  setupPhiPadWithFrames(plotPad);


  // Top
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16
  // Middle
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16

  int count=0;


  for(int column=0;column<8;column++) {
    for(int row=0;row<6;row++) {
      plotPad->cd();
      int phi=phiMap[row][column];
      //      int ring=rowMap[row];
      int ant=0;//=antMap[ring][phi];      
      int surf=0;//=surfMap[ant];
      int chan=0;//=chanMap[ant];

      fACMGeomTool->getSurfChanAntFromRingPhiPol(ringMap[row],
						 phi,AnitaPol::kHorizontal,
						 surf,chan,ant);

      sprintf(padName,"phiChanPad%d",count);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      if(chan>4)
	deleteTGraphsFromPad(paddy1,surf,chan,chan-4);
      else 
	deleteTGraphsFromPad(paddy1,surf,chan,chan+4);
      paddy1->cd();


      if(hdPtr->isInL3Pattern(phi,AnitaPol::kHorizontal)){
	  grSurf[surf][chan]->SetLineColor(kRed-2);
      }
      else{
	grSurf[surf][chan]->SetLineColor(kBlack);
      }

     
      if(hdPtr->isInPhiMask(phi,AnitaPol::kVertical)) {
	grSurf[surf][chan]->SetLineStyle(2);
      }
      else {
	grSurf[surf][chan]->SetLineStyle(1);
      }
      

      grSurf[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
						   AnitaPol::kHorizontal,
						   ringMap[row]);



      if(fWaveformOption==MagicDisplayFormatOption::kPowerSpectralDensity){
	grSurfFFT[surf][chan]->Draw("l");

	grSurfFFT[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
							AnitaPol::kHorizontal,
							ringMap[row]);
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kAveragedFFT){
	grSurfAveragedFFT[surf][chan]->Draw("l");

	grSurfAveragedFFT[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
							AnitaPol::kHorizontal,
							ringMap[row]);
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kHilbertEnvelope) {
	
	grSurfHilbert[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
						   AnitaPol::kHorizontal,
						   ringMap[row]);
	grSurfHilbert[surf][chan]->Draw("l");
      }
      else if (fWaveformOption==MagicDisplayFormatOption::kWaveform) {
	grSurf[surf][chan]->Draw("l");

	if(fAutoScale) {
	  TList *listy = gPad->GetListOfPrimitives();
	  for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      tempHist->GetYaxis()->SetRangeUser(fMinVoltLimit,fMaxVoltLimit);
	    }
	  }
	}

      }


      count++;      
      paddy1->SetEditable(kFALSE);
    }
  }

  if(!useCan)
    return canHoriz;
  else 
    return plotPad;
  
}

TPad *AnitaCanvasMaker::getVerticalCanvas(RawAnitaHeader *hdPtr,
					  TPad *useCan)
{
  //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fACMGeomTool)
    fACMGeomTool=AnitaGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canVert=0;
  TPad *plotPad=0;
  if(!useCan) {
    canVert = (TPad*) gROOT->FindObject("canVert");
    if(!canVert) {
      canVert = new TCanvas("canVert","canVert",1000,600);
    }
    canVert->Clear();
    canVert->SetTopMargin(0);
    TPaveText *leftPave = new TPaveText(0.05,0.92,0.95,0.98);
    leftPave->SetBorderSize(0);
    sprintf(textLabel,"Run %d, Event %d",hdPtr->run,hdPtr->eventNumber);
    TText *eventText = leftPave->AddText(textLabel);
    eventText->SetTextColor(50);
    leftPave->Draw();
    plotPad = new TPad("canVertMain","canVertMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  setupPhiPadWithFrames(plotPad);


  // Top
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16
  // Middle
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16

  int count=0;


  for(int column=0;column<8;column++) {
    for(int row=0;row<6;row++) {
      plotPad->cd();
      int phi=phiMap[row][column];
      //      int ring=rowMap[row];
      int ant=0;//=antMap[ring][phi];      
      int surf=0;//=surfMap[ant];
      int chan=0;//=chanMap[ant];

      fACMGeomTool->getSurfChanAntFromRingPhiPol(ringMap[row],
						 phi,AnitaPol::kVertical,
						 surf,chan,ant);

      sprintf(padName,"phiChanPad%d",count);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      if(chan>4)
	deleteTGraphsFromPad(paddy1,surf,chan,chan-4);
      else 
	deleteTGraphsFromPad(paddy1,surf,chan,chan+4);
      paddy1->cd();

      
      if(hdPtr->isInL3Pattern(phi,AnitaPol::kVertical))
	grSurf[surf][chan]->SetLineColor(kRed-3);


      if(hdPtr->isInPhiMask(phi,AnitaPol::kVertical)) {
	grSurf[surf][chan]->SetLineStyle(2);
      }
      else {
	grSurf[surf][chan]->SetLineStyle(1);
      }
	

      grSurf[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
						   AnitaPol::kVertical,
						   ringMap[row]);


      if(fWaveformOption==MagicDisplayFormatOption::kPowerSpectralDensity){
	grSurfFFT[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
							AnitaPol::kVertical,
							ringMap[row]);
	grSurfFFT[surf][chan]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kAveragedFFT){
	grSurfAveragedFFT[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
							AnitaPol::kVertical,
							ringMap[row]);
	grSurfAveragedFFT[surf][chan]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kHilbertEnvelope) {
	
	grSurfHilbert[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
							    AnitaPol::kVertical,
							    ringMap[row]);
	grSurfHilbert[surf][chan]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kWaveform){
	grSurf[surf][chan]->Draw("l");
	
	if(fAutoScale) {
	  TList *listy = gPad->GetListOfPrimitives();
	  for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      if(chan<8) {
		tempHist->GetYaxis()->SetRangeUser(fMinVertVoltLimit,fMaxVertVoltLimit);
	      }
	      else {
		tempHist->GetYaxis()->SetRangeUser(fMinClockVoltLimit,fMaxClockVoltLimit);
	      }
	    }
	  }
	}
      }


      count++;
      paddy1->SetEditable(kFALSE);
    }
  }

  if(!useCan)
    return canVert;
  else
    return plotPad;
  

}


TPad *AnitaCanvasMaker::getVerticalCanvasForWebPlotter(RawAnitaHeader *hdPtr,
						       TPad *useCan)
{
  //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fACMGeomTool)
    fACMGeomTool=AnitaGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canVert=0;
  TPad *plotPad=0;
  if(!useCan) {
    canVert = (TPad*) gROOT->FindObject("canVert");
    if(!canVert) {
      canVert = new TCanvas("canVert","canVert",1000,600);
    }
    canVert->Clear();
    canVert->SetTopMargin(0);
    TPaveText *leftPave = new TPaveText(0.05,0.92,0.95,0.98);
    leftPave->SetBorderSize(0);
    sprintf(textLabel,"Run %d, Event %d",hdPtr->run,hdPtr->eventNumber);
    TText *eventText = leftPave->AddText(textLabel);
    eventText->SetTextColor(50);
    leftPave->Draw();
    plotPad = new TPad("canVertMain","canVertMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  setupPhiPadWithFrames(plotPad);


  // Top
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16
  // Middle
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16

  int count=0;


  //  std::cout << hdPtr->eventNumber << "\t" << hdPtr->l3TrigPattern << std::endl;
  for(int column=0;column<8;column++) {
    for(int row=0;row<6;row++) {
      plotPad->cd();
      int phi=phiMap[row][column];
      //      int ring=rowMap[row];
      int ant=0;//=antMap[ring][phi];      
      int surf=0;//=surfMap[ant];
      int chan=0;//=chanMap[ant];

      fACMGeomTool->getSurfChanAntFromRingPhiPol(ringMap[row],
						 phi,AnitaPol::kVertical,
						 surf,chan,ant);

      sprintf(padName,"phiChanPad%d",count);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      if(chan<4) 
	deleteTGraphsFromPad(paddy1,surf,chan,chan+4);
      else
	deleteTGraphsFromPad(paddy1,surf,chan,chan-4);
      paddy1->cd();

      //      std::cout << phi << "\t" << ring << "\t" << surf << "\t" 
      //		<< chan << "\t" << grSurf[surf][chan]->GetRMS(2) << "\n";

      if(ringMap[row]==AnitaRing::kTopRing) {
	if(hdPtr->upperL1TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kBlue-2);
	if(hdPtr->upperL2TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kGreen-2);
      }
      else if(ringMap[row]==AnitaRing::kMiddleRing) {
      	if(hdPtr->lowerL1TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kBlue-2);
	if(hdPtr->lowerL2TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kGreen-2);
      }


      grSurf[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
						   AnitaPol::kVertical,
						   ringMap[row]);
      
      if(hdPtr->isInL3Pattern(phi,AnitaPol::kVertical))
	grSurf[surf][chan]->SetLineColor(kRed-3);


      if(hdPtr->isInPhiMask(phi,AnitaPol::kVertical)) {
	grSurf[surf][chan]->SetLineStyle(2);
      }
      else {
	grSurf[surf][chan]->SetLineStyle(1);
      }
     
      grSurf[surf][chan]->Draw("l");

      if(fAutoScale) {
	TList *listy = gPad->GetListOfPrimitives();
	for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      if(chan<8) {
		tempHist->GetYaxis()->SetRangeUser(fMinVertVoltLimit,fMaxVertVoltLimit);
	      }
	      else {
		tempHist->GetYaxis()->SetRangeUser(fMinClockVoltLimit,fMaxClockVoltLimit);
	      }
	    }
	}
      }


      count++;
      paddy1->SetEditable(kFALSE);
    }
  }

  if(!useCan)
    return canVert;
  else
    return plotPad;
  

}

TPad *AnitaCanvasMaker::getSurfChanCanvas(RawAnitaHeader *hdPtr,
					  TPad *useCan)
{
   //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fACMGeomTool)
    fACMGeomTool=AnitaGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canSurf=0;
  TPad *plotPad=0;
  if(!useCan) {
    canSurf = (TPad*) gROOT->FindObject("canSurf");
    if(!canSurf) {
      canSurf = new TCanvas("canSurf","canSurf",1000,600);
    }
    canSurf->Clear();
    canSurf->SetTopMargin(0);
    TPaveText *leftPave = new TPaveText(0.05,0.92,0.95,0.98);
    leftPave->SetBorderSize(0);
    sprintf(textLabel,"Event %d",hdPtr->eventNumber);
    TText *eventText = leftPave->AddText(textLabel);
    eventText->SetTextColor(50);
    leftPave->Draw();
    plotPad = new TPad("canSurfMain","canSurfMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  setupSurfPadWithFrames(plotPad);


  for(int surf=0;surf<ACTIVE_SURFS;surf++) {
    for(int chan=0;chan<CHANNELS_PER_SURF;chan++) {
      plotPad->cd();
      sprintf(padName,"surfChanPad%d_%d",surf,chan);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromPad(paddy1,surf,chan);
      paddy1->cd();


      //grDraw[surf][chan]=grSurf[surf][chan];
      //grDrawFFT[surf][chan]=grSurfFFT[surf][chan];



      Int_t ant=0,phi=0;
      AnitaRing::AnitaRing_t ring;
      AnitaPol::AnitaPol_t pol;
      
      if(chan<8) {
	AnitaGeomTool::getRingAntPolPhiFromSurfChan(surf,chan,ring,ant,pol,phi);
	grSurf[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
						     pol,ring);
	
      
	if(hdPtr->isInL3Pattern(phi,pol))
	  grSurf[surf][chan]->SetLineColor(kRed-3);

	if(hdPtr->isInPhiMask(phi,pol)) {
	  grSurf[surf][chan]->SetLineStyle(2);
	}
	else {
	  grSurf[surf][chan]->SetLineStyle(1);
	}

      }
      if(fWaveformOption==MagicDisplayFormatOption::kPowerSpectralDensity){
	grSurfFFT[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
							pol,ring);
	grSurfFFT[surf][chan]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kAveragedFFT){
	grSurfAveragedFFT[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
							pol,ring);
	grSurfAveragedFFT[surf][chan]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kHilbertEnvelope) {
	grSurfHilbert[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
							    pol,ring);
	grSurfHilbert[surf][chan]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kWaveform) {
	grSurf[surf][chan]->Draw("l");

	if(fAutoScale) {
	  TList *listy = gPad->GetListOfPrimitives();
	  for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      if(chan<8) {
		tempHist->GetYaxis()->SetRangeUser(fMinVertVoltLimit,fMaxVertVoltLimit);
	      }
	      else {

		grSurf[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,-1,-1,AnitaPol::kNotAPol,AnitaRing::kNotARing);
		tempHist->GetYaxis()->SetRangeUser(fMinClockVoltLimit,fMaxClockVoltLimit);
	      }
	    }
	  }
	}
      }

      paddy1->SetEditable(kFALSE);
    }
  }

  

  if(!useCan)
    return canSurf;
  else 
    return plotPad;
  
}


TPad *AnitaCanvasMaker::getPayloadCanvas(RawAnitaHeader *hdPtr,
					 TPad *useCan)
{
   //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fACMGeomTool)
    fACMGeomTool=AnitaGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canPayload=0;
  TPad *plotPad=0;
  if(!useCan) {
    canPayload = (TPad*) gROOT->FindObject("canPayload");
    if(!canPayload) {
      canPayload = new TCanvas("canPayload","canPayload",1000,600);
    }
    canPayload->Clear();
    canPayload->SetTopMargin(0);
    TPaveText *leftPave = new TPaveText(0.05,0.92,0.95,0.98);
    leftPave->SetBorderSize(0);
    sprintf(textLabel,"Event %d",hdPtr->eventNumber);
    TText *eventText = leftPave->AddText(textLabel);
    eventText->SetTextColor(50);
    leftPave->Draw();
    plotPad = new TPad("canPayloadMain","canPayloadMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  setupPayloadViewWithFrames(plotPad);

  loadPayloadViewSutff();

  Int_t leftPhi=(fPhiMax+1)-1;
  if(leftPhi<1) leftPhi+=16;
  Int_t maxPhi=(fPhiMax+1);
  Int_t rightPhi=(fPhiMax+1)+1;
  if(rightPhi>16) rightPhi-=16;

  Int_t phiArray[3]={fPhiMax-1,fPhiMax,fPhiMax+1};
  if(phiArray[0]<0) phiArray[0]+=16;
  if(phiArray[2]>15) phiArray[2]-=16;
  
  //  std::cout << fPhiMax << "\n";

  if(fAnitaGeomManager) {
    TPad *payloadPadLeft = (TPad*) plotPad->FindObject("payloadPadLeft");
    payloadPadLeft->cd();
    payloadPadLeft->Clear();
    
    //Draw anita
    TGeoVolume *anita = fAnitaGeomManager->GetMasterVolume();
    
    int n_ant[4]={8,8,16,8};
    int i_node=0;
    
    Int_t phiNums[4][16]={{1,3,5,7,9,11,13,15,0,0,0,0,0,0,0,0},
			 {0,2,4,6,8,10,12,14,16,0,0,0,0,0,0},
			 {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
			 {0,2,4,6,8,10,12,14,16,0,0,0,0,0,0}};
    AnitaRing::AnitaRing_t littleRingMap[4]={AnitaRing::kTopRing,
					     AnitaRing::kTopRing,
					     AnitaRing::kMiddleRing,
					     AnitaRing::kBottomRing};

    
    for (int i_layer=0;i_layer<4;i_layer++){
      for (int i_ant=0;i_ant<n_ant[i_layer];i_ant++){    
	//	std::cout << anita->GetNode(i_node)->GetName() << "\t" << phiNums[i_layer][i_ant] << std::endl;
	anita->GetNode(i_node)->GetVolume()->GetNode(4)->GetVolume()->SetLineColor(kWhite); //change color of Horn
	
	anita->GetNode(i_node)->GetVolume()->GetNode(0)->GetVolume()->SetLineColor(kWhite); //change color of h-pol

	anita->GetNode(i_node)->GetVolume()->GetNode(1)->GetVolume()->SetLineColor(kWhite); // change color of v-pol
	
	// if(hdPtr->isInL1Pattern(phiNums[i_layer][i_ant],littleRingMap[i_layer])) {
	//     anita->GetNode(i_node)->GetVolume()->GetNode(1)->GetVolume()->SetLineColor(kBlue-2); // change color of v-pol
	//     anita->GetNode(i_node)->GetVolume()->GetNode(4)->GetVolume()->SetLineColor(kBlue-2); 
	//   }
	// if(hdPtr->isInL2Pattern(phiNums[i_layer][i_ant],littleRingMap[i_layer])) {
	//     anita->GetNode(i_node)->GetVolume()->GetNode(1)->GetVolume()->SetLineColor(kGreen-2); // change color of v-pol
	//     anita->GetNode(i_node)->GetVolume()->GetNode(4)->GetVolume()->SetLineColor(kGreen-2); 
	//   }
	if(hdPtr->isInL3Pattern(phiNums[i_layer][i_ant])) {
	    anita->GetNode(i_node)->GetVolume()->GetNode(1)->GetVolume()->SetLineColor(kRed-3); // change color of v-pol
	    anita->GetNode(i_node)->GetVolume()->GetNode(4)->GetVolume()->SetLineColor(kRed-3); 
	  }

	i_node++;
      }
    }


    Double_t rmin[3]={-400,-400,-400};
    Double_t rmax[3]={400,400,400};
    
    TView3D *my3dView = new TView3D(1,rmin,rmax);
    my3dView->Draw();
    
    anita->GetNode(40)->GetVolume()->SetLineColor(kGray); //change color EMI Box
    anita->GetNode(41)->GetVolume()->SetLineColor(kGray); //change color SIP Box
    anita->GetNode(42)->GetVolume()->SetLineColor(kGray); //change color Batt Box
    gGeoManager->GetTopVolume()->Draw(); //draw line only
    //  gGeoManager->GetGeomPainter()->SetRaytracing(1); //ray tracing (but it's slow_
    my3dView->ZoomIn();
    my3dView->ZoomIn();
    Int_t iRep;
    Double_t phiDeg=(maxPhi-1)*22.5;
    my3dView->SetView(phiDeg,90,0,iRep);
    
    gPad->Modified();
    gPad->Update();

  }


  AnitaRing::AnitaRing_t payloadRingMap[3]={AnitaRing::kTopRing,
					    AnitaRing::kMiddleRing,
					    AnitaRing::kBottomRing};

  static Int_t lastSurf[3][3]={{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}};
  static Int_t lastChan[3][3]={{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}};

  for(int row=0;row<3;row++) {
    for(int phiInd=0;phiInd<3;phiInd++) {

      plotPad->cd();
      sprintf(padName,"payloadPad%d_%d",row,phiInd);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      if(lastSurf[row][phiInd]>=0)
	deleteTGraphsFromPad(paddy1,lastSurf[row][phiInd],lastChan[row][phiInd]);
      paddy1->cd();

      Int_t surf,chan,ant;
      Int_t phi=phiArray[phiInd];
      AnitaPol::AnitaPol_t pol=AnitaPol::kVertical;
      AnitaRing::AnitaRing_t ring=payloadRingMap[row];
      if(ring==AnitaRing::kBottomRing &&
	 phiArray[phiInd]%2==1) {
	lastSurf[row][phiInd]=-1;
	continue;
      }
      fACMGeomTool->getSurfChanAntFromRingPhiPol(payloadRingMap[row],
						 phiArray[phiInd],
						 AnitaPol::kVertical,
						 surf,chan,ant);
      lastSurf[row][phiInd]=surf;
      lastChan[row][phiInd]=chan;

      // if(hdPtr->isInL1Pattern(phi,ring))
      // 	grSurf[surf][chan]->SetLineColor(kBlue-2);
      // if(hdPtr->isInL2Pattern(phi,ring))
      // 	grSurf[surf][chan]->SetLineColor(kGreen-2);
      if(hdPtr->isInL3Pattern(phi,pol))
	grSurf[surf][chan]->SetLineColor(kRed-3);
      
      if(fWaveformOption==MagicDisplayFormatOption::kPowerSpectralDensity){
	grSurfFFT[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
							pol,ring);
	grSurfFFT[surf][chan]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kAveragedFFT){
	grSurfAveragedFFT[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
							pol,ring);
	grSurfAveragedFFT[surf][chan]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kHilbertEnvelope) {
	grSurfHilbert[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
							    pol,ring);
	grSurfHilbert[surf][chan]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kWaveform) {
	grSurf[surf][chan]->Draw("l");

	if(fAutoScale) {
	  TList *listy = gPad->GetListOfPrimitives();
	  for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      if(chan<8) {
		tempHist->GetYaxis()->SetRangeUser(fMinVertVoltLimit,fMaxVertVoltLimit);
	      }
	      else {

		grSurf[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,-1,-1,AnitaPol::kNotAPol,AnitaRing::kNotARing);
		tempHist->GetYaxis()->SetRangeUser(fMinClockVoltLimit,fMaxClockVoltLimit);
	      }
	    }
	  }
	}
      }

      paddy1->SetEditable(kFALSE);
    }
  }

  

  if(!useCan)
    return canPayload;
  else 
    return plotPad;
  
}



TPad *AnitaCanvasMaker::getCombinedCanvas(RawAnitaHeader *hdPtr,
					  TPad *useCan)
{
  gStyle->SetOptTitle(0); 

  if(!fACMGeomTool)
    fACMGeomTool=AnitaGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canBoth=0;
  TPad *plotPad=0;
  if(!useCan) {
    canBoth = (TPad*) gROOT->FindObject("canBoth");
    if(!canBoth) {
      canBoth = new TCanvas("canBoth","canBoth",1000,600);
    }
    canBoth->Clear();
    canBoth->SetTopMargin(0);
    TPaveText *leftPave = new TPaveText(0.05,0.92,0.95,0.98);
    leftPave->SetBorderSize(0);  //Try 5 rows with, 8 ants
    sprintf(textLabel,"Run %d, Event %d",hdPtr->run,hdPtr->eventNumber);
    TText *eventText = leftPave->AddText(textLabel);
    eventText->SetTextColor(50);
    leftPave->Draw();
    plotPad = new TPad("canBothMain","canBothMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  setupPhiPadWithFrames(plotPad);
    
  
 

  // Top
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16
  // Middle
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16

  int count=0;

  for(int column=0;column<8;column++) {
    for(int row=0;row<6;row++) {
      plotPad->cd();

      int phi=phiMap[row][column];
      //      int ring=rowMap[row];
      int ant=0;//=antMap[ring][phi];      
      int surf=0;//=surfMap[ant];
      int chanV=0;//=chanMap[ant];
      int chanH=0;

      fACMGeomTool->getSurfChanAntFromRingPhiPol(ringMap[row],
						 phi,AnitaPol::kVertical,
						 surf,chanV,ant);
      fACMGeomTool->getSurfChanAntFromRingPhiPol(ringMap[row],
						 phi,AnitaPol::kHorizontal,
						 surf,chanH,ant);

      sprintf(padName,"phiChanPad%d",count);      
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromPad(paddy1,surf,chanH,chanV);
      paddy1->cd();

      grSurf[surf][chanV]->setSurfChanPhiAntPolRing(surf,chanV,phi,ant,
						       AnitaPol::kVertical,
						       ringMap[row]);
      grSurf[surf][chanH]->setSurfChanPhiAntPolRing(surf,chanH,phi,ant,
						       AnitaPol::kHorizontal,
						       ringMap[row]);


      if(hdPtr->phiTrigMask & (1<<phi)) {
	grSurf[surf][chanV]->SetLineColor(2);
      }
      else {
	grSurf[surf][chanV]->SetLineColor(1);
      }
	

      if(fWaveformOption==MagicDisplayFormatOption::kPowerSpectralDensity){
	
	grSurfFFT[surf][chanV]->setSurfChanPhiAntPolRing(surf,chanV,phi,ant,
							 AnitaPol::kVertical,
							 ringMap[row]);
	grSurfFFT[surf][chanH]->setSurfChanPhiAntPolRing(surf,chanH,phi,ant,
							 AnitaPol::kHorizontal,
							 ringMap[row]);
	grSurfFFT[surf][chanH]->SetLineColor(kBlue);
	grSurfFFT[surf][chanH]->Draw("l");
	grSurfFFT[surf][chanV]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kAveragedFFT){
	
	grSurfAveragedFFT[surf][chanV]->setSurfChanPhiAntPolRing(surf,chanV,phi,ant,
							 AnitaPol::kVertical,
							 ringMap[row]);
	grSurfAveragedFFT[surf][chanH]->setSurfChanPhiAntPolRing(surf,chanH,phi,ant,
							 AnitaPol::kHorizontal,
							 ringMap[row]);
	grSurfAveragedFFT[surf][chanH]->SetLineColor(kBlue);
	grSurfAveragedFFT[surf][chanH]->Draw("l");
	grSurfAveragedFFT[surf][chanV]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kHilbertEnvelope) {
	
	grSurfHilbert[surf][chanV]->setSurfChanPhiAntPolRing(surf,chanV,phi,ant,
							     AnitaPol::kVertical,
							     ringMap[row]);
	grSurfHilbert[surf][chanH]->setSurfChanPhiAntPolRing(surf,chanH,phi,ant,
							     AnitaPol::kHorizontal,
							     ringMap[row]);
	grSurfHilbert[surf][chanH]->SetLineColor(kBlue);
	grSurfHilbert[surf][chanH]->Draw("l");
	grSurfHilbert[surf][chanV]->Draw("l");
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kWaveform){
	
	grSurf[surf][chanH]->SetLineColor(kBlue);
	grSurf[surf][chanH]->Draw("l");
	grSurf[surf][chanV]->Draw("l");

	if(fAutoScale) {
	  TList *listy = gPad->GetListOfPrimitives();
	  for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      //	      std::cout << tempHist->ClassName() << "\t" << tempHist << std::endl;
	      //	  std::cout << tempHist->GetYaxis()->GetXmin() << "\n";
	      tempHist->GetYaxis()->SetRangeUser(fMinVoltLimit,fMaxVoltLimit);
	      //	      std::cout << fMinVoltLimit << "\t" << fMaxVoltLimit << "\n";
	    }
	  }
	}

      }


      count++;
      paddy1->SetEditable(kFALSE);
    }
  }
  if(!useCan)
    return canBoth;
  else
    return plotPad;
  
}

void AnitaCanvasMaker::setupPhiPadWithFrames(TPad *plotPad)
{
  char textLabel[180];
  char padName[180];
  plotPad->cd();
  if(fLastCanvasView==MagicDisplayCanvasLayoutOption::kSurfOnly) {
    plotPad->Clear();
  } 
  if(fRedoEventCanvas){
    plotPad->Clear();
  }

  fLastCanvasView=MagicDisplayCanvasLayoutOption::kPhiVerticalOnly; //Well just phi

  static int phiPadsDone=0;
  if(phiPadsDone && !fRedoEventCanvas) {
    int errors=0;
    for(int i=0;i<48;i++) {
      sprintf(padName,"phiChanPad%d",i);
      TPad *paddy = (TPad*) plotPad->FindObject(padName);
      if(!paddy)
	errors++;
    }
    if(!errors)
      return;
  }

  phiPadsDone=1;
    

  Double_t left[8]={0.04,0.165,0.28,0.395,0.51,0.625,0.74,0.855};
  Double_t right[8]={0.165,0.28,0.395,0.51,0.625,0.74,0.855,0.97};
  Double_t top[6]={0.95,0.8,0.65,0.50,0.35,0.20};
  Double_t bottom[6]={0.8,0.65,0.50,0.35,0.20,0.03};
  
  //Now add some labels around the plot
  TLatex texy;
  texy.SetTextSize(0.03); 
  texy.SetTextAlign(12);  
  for(int column=0;column<8;column++) {
    sprintf(textLabel,"Phi %d/%d",1+(2*column),2+(2*column));
    if(column==7)
      texy.DrawTextNDC(right[column]-0.1,0.97,textLabel);
    else
      texy.DrawTextNDC(right[column]-0.09,0.97,textLabel);
  }
  texy.SetTextAlign(21);  
  texy.SetTextAngle(90);
  texy.DrawTextNDC(left[0]-0.01,bottom[0],"Top Ring");
  texy.DrawTextNDC(left[0]-0.01,bottom[2],"Middle Ring");
  texy.DrawTextNDC(left[0]-0.01,bottom[4],"Bottom Ring");

  // Top
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16
  // Middle
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16

  int count=0;




  for(int column=0;column<8;column++) {
    for(int row=0;row<6;row++) {
      plotPad->cd();
      //      int phi=phiMap[row][column];
      sprintf(padName,"phiChanPad%d",count);
      TPad *paddy1 = new TPad(padName,padName,left[column],bottom[row],right[column],top[row]);   
      paddy1->SetTopMargin(0);
      paddy1->SetBottomMargin(0);
      paddy1->SetLeftMargin(0);
      paddy1->SetRightMargin(0);
      if(column==7)
	paddy1->SetRightMargin(0.01);
      if(column==0)
	paddy1->SetLeftMargin(0.1);
      if(row==5)
	paddy1->SetBottomMargin(0.1);
      paddy1->Draw();
      paddy1->cd();

      TH1F *framey=0;
      
      if(fWaveformOption==MagicDisplayFormatOption::kWaveform || fWaveformOption==MagicDisplayFormatOption::kHilbertEnvelope) 
	framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinVoltLimit,fMaxTimeLimit,fMaxVoltLimit);
      else if(fWaveformOption==MagicDisplayFormatOption::kFFT || fWaveformOption==MagicDisplayFormatOption::kAveragedFFT)
	framey = (TH1F*) paddy1->DrawFrame(fMinFreqLimit,fMinPowerLimit,fMaxFreqLimit,fMaxPowerLimit); 

      framey->GetYaxis()->SetLabelSize(0.08);
      framey->GetYaxis()->SetTitleSize(0.1);
      framey->GetYaxis()->SetTitleOffset(0.5);
      if(row==5) {
	framey->GetXaxis()->SetLabelSize(0.09);
	framey->GetXaxis()->SetTitleSize(0.09);
	framey->GetYaxis()->SetLabelSize(0.09);
	framey->GetYaxis()->SetTitleSize(0.09);
      }
      count++;
    }
  }

}



void AnitaCanvasMaker::setupSurfPadWithFrames(TPad *plotPad)
{
  static int surfPadsDone=0;
  char textLabel[180];
  char padName[180];
  plotPad->cd();
  if(fLastCanvasView!=MagicDisplayCanvasLayoutOption::kSurfOnly) {
    plotPad->Clear();
  }

  if(fRedoEventCanvas && surfPadsDone){
    plotPad->Clear();
  }

  fLastCanvasView=MagicDisplayCanvasLayoutOption::kSurfOnly;

  if(surfPadsDone && !fRedoEventCanvas) {
    int errors=0;
    for(int surf=0;surf<ACTIVE_SURFS;surf++) {
      for(int chan=0;chan<CHANNELS_PER_SURF;chan++) {
	sprintf(padName,"surfChanPad%d_%d",surf,chan);
	TPad *paddy = (TPad*) plotPad->FindObject(padName);
	if(!paddy)
	  errors++;
      }
    }
    if(!errors)
      return;
  }


  surfPadsDone=1;
      
  Double_t leftEdge=0.04;
  Double_t rightEdge=0.97;
  Double_t standardStep=(rightEdge-leftEdge)/(1.01*ACTIVE_SURFS);
  Double_t firstStep=(rightEdge-leftEdge)-(ACTIVE_SURFS-1)*standardStep;

  Double_t left[ACTIVE_SURFS]={0};
  Double_t right[ACTIVE_SURFS]={0};  
  left[0]=leftEdge;
  right[0]=leftEdge+firstStep;
  for(int surf=1;surf<ACTIVE_SURFS;surf++) {
    left[surf]=right[surf-1];
    right[surf]=left[surf]+standardStep;
  }
    
  Double_t top[9]={0.95,0.85,0.75,0.65,0.55,0.45,0.35,0.25,0.15};
  Double_t bottom[9]={0.85,0.75,0.65,0.55,0.45,0.35,0.25,0.15,0.03};
  
  //Now add some labels around the plot
  TLatex texy;
  texy.SetTextSize(0.03); 
  texy.SetTextAlign(12);  
  for(int column=0;column<ACTIVE_SURFS;column++) {
    sprintf(textLabel,"Surf %d",1+column);
    if(column==9)
      texy.DrawTextNDC(right[column]-0.1,0.97,textLabel);
    else
      texy.DrawTextNDC(right[column]-0.09,0.97,textLabel);
  }
  texy.SetTextAlign(21);  
  texy.SetTextAngle(90);
  //  texy.DrawTextaNDC(left[0]-0.01,bottom[0],"Top Ring");
  //  texy.DrawTextNDC(left[0]-0.01,bottom[2],"Middle Ring");
  //  texy.DrawTextNDC(left[0]-0.01,bottom[4]+0.09,"Bottom Ring");

 
  int count=0;




  for(int column=0;column<ACTIVE_SURFS;column++) {
    for(int row=0;row<9;row++) {
      plotPad->cd();
      //      int surf=surfMap[row][column];
      sprintf(padName,"surfChanPad%d_%d",column,row);
      TPad *paddy1 = new TPad(padName,padName,left[column],bottom[row],right[column],top[row]);   
      paddy1->SetTopMargin(0);
      paddy1->SetBottomMargin(0);
      paddy1->SetLeftMargin(0);
      paddy1->SetRightMargin(0);
      if(column==9)
	paddy1->SetRightMargin(0.01);
      if(column==0)
	paddy1->SetLeftMargin(0.1);
      if(row==8)
	paddy1->SetBottomMargin(0.1);
      paddy1->Draw();
      paddy1->cd();
      TH1F *framey=0;
      if(fWaveformOption==MagicDisplayFormatOption::kFFT || fWaveformOption==MagicDisplayFormatOption::kAveragedFFT){
	  framey = (TH1F*) paddy1->DrawFrame(fMinFreqLimit,fMinPowerLimit,fMaxFreqLimit,fMaxPowerLimit);
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kWaveform || 
	      fWaveformOption==MagicDisplayFormatOption::kHilbertEnvelope) {
	if(row<8) {
	  framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinVoltLimit,fMaxTimeLimit,fMaxVoltLimit);
	}
	else{
	  framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinClockVoltLimit,fMaxTimeLimit,fMaxClockVoltLimit);
	}
      }

      framey->GetYaxis()->SetLabelSize(0.1);
      framey->GetYaxis()->SetTitleSize(0.1);
      framey->GetYaxis()->SetTitleOffset(0.5);
      if(row==8) {
	framey->GetXaxis()->SetLabelSize(0.09);
	framey->GetXaxis()->SetTitleSize(0.09);
	framey->GetYaxis()->SetLabelSize(0.09);
	framey->GetYaxis()->SetTitleSize(0.09);
      }
      count++;
    }
  }
}




void AnitaCanvasMaker::setupPayloadViewWithFrames(TPad *plotPad)
{
  static int payloadPadsDone=0;
  //  char textLabel[180];
  char padName[180];
  plotPad->cd();
  if(fLastCanvasView!=MagicDisplayCanvasLayoutOption::kPayloadView) {
    plotPad->Clear();
  }

  if(fRedoEventCanvas && payloadPadsDone){
    plotPad->Clear();
  }

  fLastCanvasView=MagicDisplayCanvasLayoutOption::kPayloadView;

  if(payloadPadsDone && !fRedoEventCanvas) {
    int errors=0;
    sprintf(padName,"payloadPadLeft");
    TPad *paddy = (TPad*) plotPad->FindObject(padName);
    if(!paddy)
      errors++;

    for(int ring=0;ring<3;ring++) {
      for(int phi=0;phi<3;phi++) {
	sprintf(padName,"payloadPad%d_%d",ring,phi);
	TPad *paddy = (TPad*) plotPad->FindObject(padName);
	if(!paddy)
	  errors++;
      }
    }
    if(!errors)
      return;
  }


  payloadPadsDone=1;
      

  Double_t left[3]={0.53,0.7,0.85};
  Double_t right[3]={0.7,0.85,0.99};
  Double_t top[3]={0.95,0.65,0.35};
  Double_t bottom[3]={0.65,0.35,0.03};
  
  //Now add some labels around the plot
  TLatex texy;
  texy.SetTextSize(0.03); 
  texy.SetTextAlign(12);  
 
  int count=0;


  sprintf(padName,"payloadPadLeft");
  TPad *padLeft =new TPad(padName,padName,0.05,0.05,0.5,0.95);
  padLeft->SetFillColor(kBlack);
  padLeft->Draw();


  for(int column=0;column<3;column++) {
    for(int row=0;row<3;row++) {
      plotPad->cd();
      //      int surf=surfMap[row][column];
      sprintf(padName,"payloadPad%d_%d",row,column);
      TPad *paddy1 = new TPad(padName,padName,left[column],bottom[row],right[column],top[row]);   
      paddy1->SetTopMargin(0);
      paddy1->SetBottomMargin(0);
      paddy1->SetLeftMargin(0);
      paddy1->SetRightMargin(0);
      if(column==2)
	paddy1->SetRightMargin(0.01);
      if(column==0)
	paddy1->SetLeftMargin(0.1);
      if(row==2)
	paddy1->SetBottomMargin(0.1);
      paddy1->Draw();
      paddy1->cd();
      TH1F *framey=0;
      if(fWaveformOption==MagicDisplayFormatOption::kFFT || fWaveformOption==MagicDisplayFormatOption::kAveragedFFT){
	  framey = (TH1F*) paddy1->DrawFrame(fMinFreqLimit,fMinPowerLimit,fMaxFreqLimit,fMaxPowerLimit);
      }
      else if(fWaveformOption==MagicDisplayFormatOption::kWaveform || 
	      fWaveformOption==MagicDisplayFormatOption::kHilbertEnvelope) {
	framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinVoltLimit,fMaxTimeLimit,fMaxVoltLimit);
      }

      framey->GetYaxis()->SetLabelSize(0.1);
      framey->GetYaxis()->SetTitleSize(0.1);
      framey->GetYaxis()->SetTitleOffset(0.5);
      if(row==8) {
	framey->GetXaxis()->SetLabelSize(0.09);
	framey->GetXaxis()->SetTitleSize(0.09);
	framey->GetYaxis()->SetLabelSize(0.09);
	framey->GetYaxis()->SetTitleSize(0.09);
      }
      count++;
    }
  }
}




void AnitaCanvasMaker::deleteTGraphsFromPad(TPad *paddy,int surf,int chan)
{
  paddy->cd();
  if(fLastWaveformFormat==MagicDisplayFormatOption::kWaveform) paddy->GetListOfPrimitives()->Remove(grSurf[surf][chan]);
  else if(fLastWaveformFormat==MagicDisplayFormatOption::kFFT) paddy->GetListOfPrimitives()->Remove(grSurfFFT[surf][chan]); 
  else if(fLastWaveformFormat==MagicDisplayFormatOption::kAveragedFFT) paddy->GetListOfPrimitives()->Remove(grSurfAveragedFFT[surf][chan]); 
  else if(fLastWaveformFormat==MagicDisplayFormatOption::kHilbertEnvelope) paddy->GetListOfPrimitives()->Remove(grSurfHilbert[surf][chan]);  
  //  paddy->Update();
}

void AnitaCanvasMaker::deleteTGraphsFromPad(TPad *paddy,int surf,int chan,int chan2)
{
  paddy->cd();
  if(fLastWaveformFormat==MagicDisplayFormatOption::kWaveform){
    paddy->GetListOfPrimitives()->Remove(grSurf[surf][chan]);
    paddy->GetListOfPrimitives()->Remove(grSurf[surf][chan2]);
  }
  else if(fLastWaveformFormat==MagicDisplayFormatOption::kFFT){
    paddy->GetListOfPrimitives()->Remove(grSurfFFT[surf][chan]);
    paddy->GetListOfPrimitives()->Remove(grSurfFFT[surf][chan2]);
  }
  else if(fLastWaveformFormat==MagicDisplayFormatOption::kAveragedFFT){
    paddy->GetListOfPrimitives()->Remove(grSurfAveragedFFT[surf][chan]);
    paddy->GetListOfPrimitives()->Remove(grSurfAveragedFFT[surf][chan2]);
  }
  else if(fLastWaveformFormat==MagicDisplayFormatOption::kHilbertEnvelope){
    paddy->GetListOfPrimitives()->Remove(grSurfHilbert[surf][chan]);
    paddy->GetListOfPrimitives()->Remove(grSurfHilbert[surf][chan2]);
  }
  
  //  paddy->Update();
}

void AnitaCanvasMaker::resetAverage() 
{
  for(int surf=0;surf<ACTIVE_SURFS;surf++) {
    for(int chan=0;chan<CHANNELS_PER_SURF;chan++) {
      if(grSurfAveragedFFT[surf][chan]) {
	delete grSurfAveragedFFT[surf][chan];
	grSurfAveragedFFT[surf][chan]=0;
      }
    }
  }
}




void AnitaCanvasMaker::loadPayloadViewSutff()
{
  if(fAnitaGeomManager) return;
  char fileName[FILENAME_MAX];
  char *dirName=getenv("ANITA_UTIL_INSTALL_DIR");
  if(dirName) {
    sprintf(fileName,"%s/share/anitaMap/anitageom.root",dirName);
  }
  else
    sprintf(fileName,"anitageom.root");
  
  fAnitaGeomFile = new TFile(fileName);
  if(!fAnitaGeomFile) {
    std::cerr << "Couldn't open: " << fileName << "\n";
    return;
  }
  
  fAnitaGeomManager= (TGeoManager*) fAnitaGeomFile->Get("anita");
  if(!fAnitaGeomManager) {
    std::cerr << "Couldn't find anita geometry\n";
  }

}
