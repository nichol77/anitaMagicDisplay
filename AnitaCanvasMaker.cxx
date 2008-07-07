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
#include "TObject.h"
#include "TTimeStamp.h"

#include "FFTtools.h"


AnitaCanvasMaker*  AnitaCanvasMaker::fgInstance = 0;
AnitaGeomTool *fACMGeomTool=0;


int phiMap[5][8]={{0,2,4,6,8,10,12,14},
		  {1,3,5,7,9,11,13,15},
		  {0,2,4,6,8,10,12,14},
		  {1,3,5,7,9,11,13,15},
		  {0,2,4,6,8,10,12,14}};

int antMap[3][16]={{8,0,9,1,10,2,11,3,12,4,13,5,14,6,15,7},
		   {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
		   {32,100,33,100,34,100,35,100,36,100,37,100,38,100,39,100}};

int surfMap[40]={5,7,6,1,5,7,6,1,0,2,3,4,0,2,3,4,0,5,2,7,3,6,4,1,0,5,2,7,3,6,4,1,8,9,8,9,8,9,8,9};

int chanMap[40]={0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,0,0,1,1,2,2,3,3};

int rowMap[5]={0,0,1,1,2};

AnitaRing::AnitaRing_t ringMap[5]={AnitaRing::kUpperRing,
				   AnitaRing::kUpperRing,
				   AnitaRing::kLowerRing,
				   AnitaRing::kLowerRing,
				   AnitaRing::kNadirRing};


WaveformGraph *grSurf[ACTIVE_SURFS][CHANNELS_PER_SURF]={0};
FFTGraph *grSurfFFT[ACTIVE_SURFS][CHANNELS_PER_SURF]={0};

AnitaCanvasMaker::AnitaCanvasMaker()
{
  //Default constructor
  fACMGeomTool=AnitaGeomTool::Instance();
  fSetVoltLimits=1;
  fMinVoltLimit=-60;
  fMaxVoltLimit=60;
  fMinClockVoltLimit=-200;
  fMaxClockVoltLimit=200;
  fMinTimeLimit=0;
  fMaxTimeLimit=100;
  fMinPowerLimit=-90;
  fMaxPowerLimit=10;
  fMinFreqLimit=0;
  fMaxFreqLimit=1200;
  fPolView=0;
  fPowerSpecView=0;
  fRedoEventCanvas=0;
  //fRedoSurfCanvas=0;
  fLastView=0;
  fNewEvent=1;
  fgInstance=this;
  memset(grSurf,0,sizeof(WaveformGraph*)*ACTIVE_SURFS*CHANNELS_PER_SURF);
  memset(grSurfFFT,0,sizeof(FFTGraph*)*ACTIVE_SURFS*CHANNELS_PER_SURF);
  
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


TPad *AnitaCanvasMaker::getEventInfoCanvas(RawAnitaHeader *hdPtr,TPad *useCan)
{
   static UInt_t lastEventNumber=0;
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
      topPad->Clear();
      topPad->SetTopMargin(0.05);
      topPad->cd();
      TPaveText *leftPave = new TPaveText(0,0,0.28,0.9);
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

      TPaveText *rightPave = new TPaveText(0.32,0,0.58,0.95);
      rightPave->SetBorderSize(0);
      rightPave->SetTextAlign(13);
      TTimeStamp trigTime((time_t)hdPtr->triggerTime,(Int_t)hdPtr->triggerTimeNs);
      sprintf(textLabel,"Time: %s",trigTime.AsString("s"));
      TText *timeText = rightPave->AddText(textLabel);
      timeText->SetTextColor(1);
      sprintf(textLabel,"Trigger: %8.6f ms",1e-6*hdPtr->triggerTimeNs);
      TText *timeText2 = rightPave->AddText(textLabel);
      timeText2->SetTextColor(1);
      sprintf(textLabel,"Priority: %d -- Queue: %d",(hdPtr->priority&0xf0)/16,hdPtr->priority&0xf);
      rightPave->AddText(textLabel);
      //      sprintf(textLabel,"Lab Chip %d",labChip);
      //      rightPave->AddText(textLabel);
      sprintf(textLabel,"Trig Num: %d -- Trig Type: %s",hdPtr->trigNum,hdPtr->trigTypeAsString());
      rightPave->AddText(textLabel);
      if(hdPtr->errorFlag&0x1) {
	 TText *slipText = rightPave->AddText("Possible Sync Slip");
	 slipText->SetTextColor(6);   
      }
      rightPave->Draw();
      

      lastEventNumber=hdPtr->eventNumber;
   }
      
   return topPad;
}


TPad *AnitaCanvasMaker::getEventViewerCanvas(UsefulAnitaEvent *evPtr,
					     RawAnitaHeader *hdPtr,
					     TPad *useCan)
{
  TPad *retCan;

  static Int_t lastEventView=0;


  
  if(fNewEvent==1){

    for(int surf=0;surf<ACTIVE_SURFS;surf++){
      for(int chan=0;chan<CHANNELS_PER_SURF;chan++){

	if(grSurf[surf][chan]){
	  delete grSurf[surf][chan];
	  //grSurf[surf][chan]=0;
	}
	if(grSurfFFT[surf][chan]){
	  delete grSurfFFT[surf][chan];
	  //grSurfFFT[surf][chan]=0;
	}

	TGraph *grTemp = evPtr->getGraphFromSurfAndChan(surf,chan);
	grSurf[surf][chan] = new WaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	TGraph *grTempFFT = grSurf[surf][chan]->getFFT();
	grSurfFFT[surf][chan] = new FFTGraph(grTempFFT->GetN(),grTempFFT->GetX(),grTempFFT->GetY());
	delete grTemp;
	delete grTempFFT;

      }
    }
  }

  fNewEvent=0;

  if(lastEventView==fPowerSpecView) fRedoEventCanvas=0;
  else fRedoEventCanvas=1;

  if(fPolView==0) retCan=AnitaCanvasMaker::getVerticalCanvas(hdPtr,useCan);
  else if(fPolView==1) retCan=AnitaCanvasMaker::getHorizontalCanvas(hdPtr,useCan);
  else if(fPolView==2) retCan=AnitaCanvasMaker::getCombinedCanvas(hdPtr,useCan);
  else if(fPolView==3) retCan=AnitaCanvasMaker::getSurfChanCanvas(hdPtr,useCan);

  lastEventView=fPowerSpecView;

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
  TPad *canHoriz;
  TPad *plotPad;
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


  // Upper
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16
  // Lower
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16

  int count=0;


  //  std::cout << hdPtr->eventNumber << "\t" << hdPtr->l3TrigPattern << std::endl;
  for(int column=0;column<8;column++) {
    for(int row=0;row<5;row++) {
      plotPad->cd();
      int phi=phiMap[row][column];
      int ring=rowMap[row];
      int ant=antMap[ring][phi];
      int surf=surfMap[ant];
      int chan=chanMap[ant]+4;
      sprintf(padName,"phiChanPad%d",count);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromPad(paddy1,surf,chan,chan-4);
      paddy1->cd();


      if(hdPtr->l3TrigPattern & (1<<phi)){
	  grSurf[surf][chan]->SetLineColor(kRed-2);
      }
      else{
	grSurf[surf][chan]->SetLineColor(kBlack);
      }

      if(ringMap[row]==AnitaRing::kUpperRing) {
	if(hdPtr->upperL1TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kBlue-2);
	if(hdPtr->upperL2TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kGreen-2);
      }
      else if(ringMap[row]==AnitaRing::kLowerRing) {
      	if(hdPtr->lowerL1TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kBlue-2);
	if(hdPtr->lowerL2TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kGreen-2);
      }
      else{
	grSurf[surf][chan]->SetLineColor(kBlack);
      }
      

      grSurf[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
						   AnitaPol::kHorizontal,
						   ringMap[row]);
      grSurfFFT[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
						   AnitaPol::kHorizontal,
						   ringMap[row]);


      if(fPowerSpecView){
	grSurfFFT[surf][chan]->Draw("l");
      }
      else{
	grSurf[surf][chan]->Draw("l");
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
  TPad *canVert;
  TPad *plotPad;
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


  // Upper
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16
  // Lower
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16

  int count=0;


  //  std::cout << hdPtr->eventNumber << "\t" << hdPtr->l3TrigPattern << std::endl;
  for(int column=0;column<8;column++) {
    for(int row=0;row<5;row++) {
      plotPad->cd();
      int phi=phiMap[row][column];
      int ring=rowMap[row];
      int ant=antMap[ring][phi];
      int surf=surfMap[ant];
      int chan=chanMap[ant];
      sprintf(padName,"phiChanPad%d",count);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromPad(paddy1,surf,chan,chan+4);
      paddy1->cd();

      if(ringMap[row]==AnitaRing::kUpperRing) {
	if(hdPtr->upperL1TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kBlue-2);
	if(hdPtr->upperL2TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kGreen-2);
      }
      else if(ringMap[row]==AnitaRing::kLowerRing) {
      	if(hdPtr->lowerL1TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kBlue-2);
	if(hdPtr->lowerL2TrigPattern & (1<<phi))
	  grSurf[surf][chan]->SetLineColor(kGreen-2);
      }
      
      if(hdPtr->l3TrigPattern & (1<<phi))
	grSurf[surf][chan]->SetLineColor(kRed-3);


      grSurf[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
						   AnitaPol::kVertical,
						   ringMap[row]);
      grSurfFFT[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
						   AnitaPol::kVertical,
						   ringMap[row]);


      if(fPowerSpecView){
	grSurfFFT[surf][chan]->Draw("l");
      }
      else{
	grSurf[surf][chan]->Draw("l");
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


  static Int_t lastSurfView=0;

  if(lastSurfView==fPowerSpecView) fRedoEventCanvas=0;
  else fRedoEventCanvas=1;

  lastSurfView=fPowerSpecView;

  if(!fACMGeomTool)
    fACMGeomTool=AnitaGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canSurf;
  TPad *plotPad;
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

//       if(ringMap[row]==AnitaRing::kUpperRing) {
// 	if(hdPtr->upperL1TrigPattern & (1<<phi))
// 	  grSurf[count][1]->SetLineColor(kBlue-2);
// 	if(hdPtr->upperL2TrigPattern & (1<<phi))
// 	  grSurf[count][1]->SetLineColor(kGreen-2);
//       }
//       else if(ringMap[row]==AnitaRing::kLowerRing) {
//       	if(hdPtr->lowerL1TrigPattern & (1<<phi))
// 	  grSurf[count][1]->SetLineColor(kBlue-2);
// 	if(hdPtr->lowerL2TrigPattern & (1<<phi))
// 	  grSurf[count][1]->SetLineColor(kGreen-2);
//       }
      
//      if(hdPtr->l3TrigPattern & (1<<phi))
//	grSurf[count][1]->SetLineColor(kRed-3);

      Int_t ant=0,phi=0;
      AnitaRing::AnitaRing_t ring;
      AnitaPol::AnitaPol_t pol;
      
      AnitaGeomTool::getRingAntPolPhiFromSurfChan(surf,chan,ring,ant,pol,phi);
      grSurf[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
						   pol,ring);
      grSurfFFT[surf][chan]->setSurfChanPhiAntPolRing(surf,chan,phi,ant,
						   pol,ring);

      if(fPowerSpecView && chan<(CHANNELS_PER_SURF-1)){
	grSurfFFT[surf][chan]->Draw("l");
      }
      else{
	grSurf[surf][chan]->Draw("l");
      }

      paddy1->SetEditable(kFALSE);
    }
  }

  if(!useCan)
    return canSurf;
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
  TPad *canBoth;
  TPad *plotPad;
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
    
  
 

  // Upper
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16
  // Lower
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16

  int count=0;

  //  std::cout << hdPtr->eventNumber << "\t" << hdPtr->l3TrigPattern << std::endl;
  for(int column=0;column<8;column++) {
    for(int row=0;row<5;row++) {
      plotPad->cd();
      int phi=phiMap[row][column];
      int ring=rowMap[row];
      int ant=antMap[ring][phi];
      int surf=surfMap[ant];
      int chanH=chanMap[ant]+4;
      int chanV=chanMap[ant];
      sprintf(padName,"phiChanPad%d",count);
      
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromPad(paddy1,surf,chanH,chanV);
      paddy1->cd();

      if(fPowerSpecView){
	grSurfFFT[surf][chanH]->SetLineColor(kBlue);
	grSurfFFT[surf][chanV]->Draw("l");
	grSurfFFT[surf][chanH]->Draw("l");
      }
      else{
	grSurf[surf][chanH]->SetLineColor(kBlue);
	grSurf[surf][chanV]->Draw("l");
	grSurf[surf][chanH]->Draw("l");
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
  if(fLastView==2) {
    plotPad->Clear();
  } 
  if(fRedoEventCanvas){
    plotPad->Clear();
  }

  fLastView=1;

  static int phiPadsDone=0;
  if(phiPadsDone && !fRedoEventCanvas) {
    int errors=0;
    for(int i=0;i<40;i++) {
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
  Double_t top[5]={0.95,0.77,0.59,0.41,0.23};
  Double_t bottom[5]={0.77,0.59,0.41,0.23,0.03};
  
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
  texy.DrawTextNDC(left[0]-0.01,bottom[0],"Upper Ring");
  texy.DrawTextNDC(left[0]-0.01,bottom[2],"Lower Ring");
  texy.DrawTextNDC(left[0]-0.01,bottom[4]+0.09,"Nadir Ring");

  // Upper
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16
  // Lower
  // 1 3 5 7 9 11 13 15
  // 2 4 6 8 10 12 14 16

  int count=0;




  for(int column=0;column<8;column++) {
    for(int row=0;row<5;row++) {
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
      if(row==4)
	paddy1->SetBottomMargin(0.1);
      paddy1->Draw();
      paddy1->cd();

      TH1F *framey;
      
      if(!fPowerSpecView) framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinVoltLimit,fMaxTimeLimit,fMaxVoltLimit);
      else framey = (TH1F*) paddy1->DrawFrame(fMinFreqLimit,fMinPowerLimit,fMaxFreqLimit,fMaxPowerLimit); 

      framey->GetYaxis()->SetLabelSize(0.1);
      framey->GetYaxis()->SetTitleSize(0.1);
      framey->GetYaxis()->SetTitleOffset(0.5);
      if(row==4) {
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
  if(fLastView==1) {
    plotPad->Clear();
  }

  if(fRedoEventCanvas && surfPadsDone){
    plotPad->Clear();
  }

  fLastView=2;

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
      

  Double_t left[10]={0.04,0.142,0.234,0.326,0.418,0.51,0.602,0.694,0.786,0.878};
  Double_t right[10]={0.142,0.234,0.326,0.418,0.51,0.602,0.694,0.786,0.878,0.97};
  Double_t top[9]={0.95,0.85,0.75,0.65,0.55,0.45,0.35,0.25,0.15};
  Double_t bottom[9]={0.85,0.75,0.65,0.55,0.45,0.35,0.25,0.15,0.03};
  
  //Now add some labels around the plot
  TLatex texy;
  texy.SetTextSize(0.03); 
  texy.SetTextAlign(12);  
  for(int column=0;column<10;column++) {
    sprintf(textLabel,"Surf %d",1+column);
    if(column==9)
      texy.DrawTextNDC(right[column]-0.1,0.97,textLabel);
    else
      texy.DrawTextNDC(right[column]-0.09,0.97,textLabel);
  }
  texy.SetTextAlign(21);  
  texy.SetTextAngle(90);
  //  texy.DrawTextaNDC(left[0]-0.01,bottom[0],"Upper Ring");
  //  texy.DrawTextNDC(left[0]-0.01,bottom[2],"Lower Ring");
  //  texy.DrawTextNDC(left[0]-0.01,bottom[4]+0.09,"Nadir Ring");

 
  int count=0;


  //  std::cout << hdPtr->eventNumber << "\t" << hdPtr->l3TrigPattern << std::endl;


  for(int column=0;column<10;column++) {
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
      TH1F *framey;
      if(fPowerSpecView){
	if(row<8) {
	  framey = (TH1F*) paddy1->DrawFrame(fMinFreqLimit,fMinPowerLimit,fMaxFreqLimit,fMaxPowerLimit);
	}
	else{
	  framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinClockVoltLimit,fMaxTimeLimit,fMaxClockVoltLimit);
	}
      }
      else{
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

/*
void AnitaCanvasMaker::deleteTGraphsFromPad(TPad *paddy)
{
  paddy->cd();
  //  paddy->Update();
  //  std::cout << "deleteTGraphsFromPad: " << paddy->GetName() << std::endl;
  TList *listy = paddy->GetListOfPrimitives();
  for(int i=0;i<listy->GetSize();i++) {
    TObject *fred = listy->At(i);
    if(fred->InheritsFrom("TGraph")) {
      std::cout << fred->ClassName() << "\t" << fred << std::endl;

      for(int surf=0;surf<ACTIVE_SURFS;surf++){
        for(int chan=0;chan<CHANNELS_PER_SURF;chan++){
          if(fred == grDraw[surf][chan]){
            delete grDraw[surf][chan];
          }
          else if(fred == grDrawFFT[surf][chan]){
            delete grDrawFFT[surf][chan];
          }
          else{
            fred->Delete("");
          }
        }
      }
    
    }
  }
  
  //  paddy->Update();
}
*/


void AnitaCanvasMaker::deleteTGraphsFromPad(TPad *paddy,int surf,int chan)
{
  paddy->cd();
  if(!fPowerSpecView && !fRedoEventCanvas) paddy->GetListOfPrimitives()->Remove(grSurf[surf][chan]);
  else if(fPowerSpecView && fRedoEventCanvas) paddy->GetListOfPrimitives()->Remove(grSurf[surf][chan]);
  else if(!fPowerSpecView && fRedoEventCanvas) paddy->GetListOfPrimitives()->Remove(grSurfFFT[surf][chan]);
  else if(fPowerSpecView && !fRedoEventCanvas) paddy->GetListOfPrimitives()->Remove(grSurfFFT[surf][chan]);
  
  //  paddy->Update();
}

void AnitaCanvasMaker::deleteTGraphsFromPad(TPad *paddy,int surf,int chan,int chan2)
{
  paddy->cd();
  if(!fPowerSpecView && !fRedoEventCanvas){
    paddy->GetListOfPrimitives()->Remove(grSurf[surf][chan]);
    paddy->GetListOfPrimitives()->Remove(grSurf[surf][chan2]);
  }
  else if(fPowerSpecView && fRedoEventCanvas){
    paddy->GetListOfPrimitives()->Remove(grSurf[surf][chan]);
    paddy->GetListOfPrimitives()->Remove(grSurf[surf][chan2]);
  }
  else if(!fPowerSpecView && fRedoEventCanvas){
    paddy->GetListOfPrimitives()->Remove(grSurfFFT[surf][chan]);
    paddy->GetListOfPrimitives()->Remove(grSurfFFT[surf][chan2]);
  }
  else if(fPowerSpecView && !fRedoEventCanvas){
    paddy->GetListOfPrimitives()->Remove(grSurfFFT[surf][chan]);
    paddy->GetListOfPrimitives()->Remove(grSurfFFT[surf][chan2]);
  }
  
  
  //  paddy->Update();
}


