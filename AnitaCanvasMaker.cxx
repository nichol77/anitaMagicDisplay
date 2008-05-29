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
#include "TList.h"
#include "TObject.h"
#include "TTimeStamp.h"


AnitaCanvasMaker*  AnitaCanvasMaker::fgInstance = 0;
AnitaGeomTool *fACMGeomTool=0;


int phiMap[5][8]={{0,2,4,6,8,10,12,14},
		  {1,3,5,7,9,11,13,15},
		  {0,2,4,6,8,10,12,14},
		  {1,3,5,7,9,11,13,15},
		  {0,2,4,6,8,10,12,14}};
		    
AnitaRing::AnitaRing_t ringMap[5]={AnitaRing::kUpperRing,
				   AnitaRing::kUpperRing,
				   AnitaRing::kLowerRing,
				   AnitaRing::kLowerRing,
				   AnitaRing::kNadirRing};


WaveformGraph *grSeavey[NUM_SEAVEYS][2]={0};

AnitaCanvasMaker::AnitaCanvasMaker()
{
  //Default constructor
  fACMGeomTool=AnitaGeomTool::Instance();
  fSetVoltLimits=1;
  fMinVoltLimit=-60;
  fMaxVoltLimit=60;
  fMinTimeLimit=0;
  fMaxTimeLimit=250;
  fgInstance=this;
  
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


TPad *AnitaCanvasMaker::getHorizontalCanvas(UsefulAnitaEvent *evPtr,
					    RawAnitaHeader *hdPtr,
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

  for(int ind=0;ind<NUM_SEAVEYS;ind++) {
    for(int pol=0;pol<2;pol++) {
      if(grSeavey[ind][pol]) delete grSeavey[ind][pol];
      grSeavey[ind][pol]=0;
    }
  }

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
      sprintf(padName,"phiChanPad%d",count);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromPad(paddy1);
      paddy1->cd();
      TGraph *grTemp=evPtr->getGraph(ringMap[row],phi,AnitaPol::kHorizontal);
      grSeavey[count][1] = new WaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
      if(ringMap[row]==AnitaRing::kUpperRing) {
	if(hdPtr->upperL1TrigPattern & (1<<phi))
	  grSeavey[count][1]->SetLineColor(kBlue-2);
	if(hdPtr->upperL2TrigPattern & (1<<phi))
	  grSeavey[count][1]->SetLineColor(kGreen-2);
      }
      else if(ringMap[row]==AnitaRing::kLowerRing) {
      	if(hdPtr->lowerL1TrigPattern & (1<<phi))
	  grSeavey[count][1]->SetLineColor(kBlue-2);
	if(hdPtr->lowerL2TrigPattern & (1<<phi))
	  grSeavey[count][1]->SetLineColor(kGreen-2);
      }
      
      if(hdPtr->l3TrigPattern & (1<<phi))
	grSeavey[count][1]->SetLineColor(kRed-3);

      grSeavey[count][1]->Draw("l");
      delete grTemp;
      count++;      
      paddy1->SetEditable(kFALSE);
    }
  }

  if(!useCan)
    return canHoriz;
  else 
    return plotPad;
  
}

TPad *AnitaCanvasMaker::getVerticalCanvas(UsefulAnitaEvent *evPtr,
					  RawAnitaHeader *hdPtr,
					  TPad *useCan)
{
  //  std::cout << "AnitaCanvasMaker::getVerticalCanvas: " << evPtr << "\t" << hdPtr << "\t" << useCan << std::endl;
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


  for(int ind=0;ind<NUM_SEAVEYS;ind++) {
    for(int pol=0;pol<2;pol++) {
      if(grSeavey[ind][pol]) delete grSeavey[ind][pol];
      grSeavey[ind][pol]=0;
    }
  }

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
      sprintf(padName,"phiChanPad%d",count);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromPad(paddy1);
      paddy1->cd();
      TGraph *grTemp=evPtr->getGraph(ringMap[row],phi,AnitaPol::kVertical);
      grSeavey[count][0] = new WaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
      if(ringMap[row]==AnitaRing::kUpperRing) {
	if(hdPtr->upperL1TrigPattern & (1<<phi))
	  grSeavey[count][0]->SetLineColor(kBlue-2);
	if(hdPtr->upperL2TrigPattern & (1<<phi))
	  grSeavey[count][0]->SetLineColor(kGreen-2);
      }
      else if(ringMap[row]==AnitaRing::kLowerRing) {
      	if(hdPtr->lowerL1TrigPattern & (1<<phi))
	  grSeavey[count][0]->SetLineColor(kBlue-2);
	if(hdPtr->lowerL2TrigPattern & (1<<phi))
	  grSeavey[count][0]->SetLineColor(kGreen-2);
      }
      
      if(hdPtr->l3TrigPattern & (1<<phi))
	grSeavey[count][0]->SetLineColor(kRed-3);


      grSeavey[count][0]->Draw("l");
      delete grTemp;
      count++;
      paddy1->SetEditable(kFALSE);
    }
  }
  if(!useCan)
    return canVert;
  else
    return plotPad;
  

}

TPad *AnitaCanvasMaker::getSurfChanCanvas(UsefulAnitaEvent *evPtr,
					  RawAnitaHeader *hdPtr,
					  TPad *useCan)
{
  return NULL;
}

TPad *AnitaCanvasMaker::getCombinedCanvas(UsefulAnitaEvent *evPtr,
					  RawAnitaHeader *hdPtr,
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
    
  for(int ind=0;ind<NUM_SEAVEYS;ind++) {
    for(int pol=0;pol<2;pol++) {
      if(grSeavey[ind][pol]) delete grSeavey[ind][pol];
      grSeavey[ind][pol]=0;
    }
  }
  
 

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
      sprintf(padName,"phiChanPad%d",count);
      
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromPad(paddy1);
      paddy1->cd();

      TGraph *grTemp=evPtr->getGraph(ringMap[row],phi,AnitaPol::kVertical);
      grSeavey[count][0] = new WaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
      TGraph *grTemp2=evPtr->getGraph(ringMap[row],phi,AnitaPol::kHorizontal);
      grSeavey[count][1] = new WaveformGraph(grTemp2->GetN(),grTemp2->GetX(),grTemp2->GetY());
      grSeavey[count][1]->SetLineColor(kBlue);

      
      grSeavey[count][0]->Draw("l");
      grSeavey[count][1]->Draw("l");
      delete grTemp;
      delete grTemp2;
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
  static int phiPadsDone=0;
  char textLabel[180];
  char padName[180];
  plotPad->cd();
  if(phiPadsDone) {
    int errors=0;
    for(int i=0;i<32;i++) {
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


  //  std::cout << hdPtr->eventNumber << "\t" << hdPtr->l3TrigPattern << std::endl;


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


      
      
      TH1F *framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinVoltLimit,fMaxTimeLimit,fMaxVoltLimit);

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
      for(int ant=0;ant<NUM_SEAVEYS;ant++) {
	for(int pol=0;pol<2;pol++) {
	  if(fred == grSeavey[ant][pol]) {
	    delete grSeavey[ant][pol];
	    //	    grSeavey[ant][pol]=0;
	    std::cout << "Yes" << std::endl;
	  }
	  else {
	    fred->Delete("");
	  }
	}
      }

    }
  }
  
  //  paddy->Update();
}
