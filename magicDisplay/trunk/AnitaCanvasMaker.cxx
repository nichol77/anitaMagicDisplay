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


AnitaCanvasMaker*  AnitaCanvasMaker::fgInstance = 0;
AnitaGeomTool *fACMGeomTool=0;


AnitaCanvasMaker::AnitaCanvasMaker()
{
  //Default constructor
  fACMGeomTool=AnitaGeomTool::Instance();
  fSetVoltLimits=1;
  fMinVoltLimit=-60;
  fMaxVoltLimit=60;
  
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
    TPaveText *topPave = new TPaveText(0.05,0.92,0.95,0.98);
    topPave->SetBorderSize(0);
    sprintf(textLabel,"Event %d",hdPtr->eventNumber);
    TText *eventText = topPave->AddText(textLabel);
    eventText->SetTextColor(50);
    topPave->Draw();
    plotPad = new TPad("canHorizMain","canHorizMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
 
  if(!useCan)
    return canHoriz;
  else 
    return plotPad;
  
}

TPad *AnitaCanvasMaker::getVerticalCanvas(UsefulAnitaEvent *evPtr,
					  RawAnitaHeader *hdPtr,
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
    TPaveText *topPave = new TPaveText(0.05,0.92,0.95,0.98);
    topPave->SetBorderSize(0);
    sprintf(textLabel,"Run %d, Event %d",hdPtr->run,hdPtr->eventNumber);
    TText *eventText = topPave->AddText(textLabel);
    eventText->SetTextColor(50);
    topPave->Draw();
    plotPad = new TPad("canVertMain","canVertMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  static WaveformGraph *gr[NUM_SEAVEYS]={0};

  

  //Try 5 rows with, 8 ants
  for(int ind=0;ind<NUM_SEAVEYS;ind++) {
    if(gr[ind]) delete gr[ind];
  }
 
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
      texy.DrawTextNDC(right[column]-0.1,0.88,textLabel);
    else
      texy.DrawTextNDC(right[column]-0.09,0.88,textLabel);
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

  for(int column=0;column<8;column++) {
    for(int row=0;row<5;row++) {
      canVert->cd();
      int phi=phiMap[row][column];
      sprintf(padName,"chanPad%d",count);
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
      TGraph *grTemp=evPtr->getGraph(ringMap[row],phi,AnitaPol::kVertical);
      gr[count] = new WaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
      if(ringMap[row]==AnitaRing::kUpperRing) {
	if(hdPtr->upperL1TrigPattern & (1<<phi))
	  gr[count]->SetLineColor(kBlue-2);
	if(hdPtr->upperL2TrigPattern & (1<<phi))
	  gr[count]->SetLineColor(kGreen-2);
      }
      else if(ringMap[row]==AnitaRing::kLowerRing) {
      	if(hdPtr->lowerL1TrigPattern & (1<<phi))
	  gr[count]->SetLineColor(kBlue-2);
	if(hdPtr->lowerL2TrigPattern & (1<<phi))
	  gr[count]->SetLineColor(kGreen-2);
      }
      
      if(hdPtr->l3TrigPattern & (1<<phi))
	gr[count]->SetLineColor(kRed-3);

      if(fSetVoltLimits) 
	gr[count]->GetYaxis()->SetRangeUser(fMinVoltLimit,fMaxVoltLimit);

    //   gr[count]->GetXaxis()->SetRangeUser(-5,gr[count]->GetXaxis()->GetXmax());
//       std::cout << gr[count]->GetXaxis()->GetXmin() << "\t"
// 		<< gr[count]->GetXaxis()->GetXmax() << std::endl;
      gr[count]->Draw("al");
      gr[count]->GetYaxis()->SetLabelSize(0.1);
      gr[count]->GetYaxis()->SetTitleSize(0.1);
      gr[count]->GetYaxis()->SetTitleOffset(0.5);
      if(row==4) {
	gr[count]->GetXaxis()->SetLabelSize(0.09);
	gr[count]->GetXaxis()->SetTitleSize(0.09);
	gr[count]->GetYaxis()->SetLabelSize(0.09);
	gr[count]->GetYaxis()->SetTitleSize(0.09);
      }
      delete grTemp;
      count++;
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
  return NULL;
}


