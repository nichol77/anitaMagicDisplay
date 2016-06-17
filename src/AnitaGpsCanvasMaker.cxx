//////////////////////////////////////////////////////////////////////////////
/////  AnitaGpsCanvasMaker.cxx        ANITA GPS Canvas maker             /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for making pretty GPS canvases for ANITA-II              /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include "MagicDisplayConventions.h"
#include "AnitaGpsCanvasMaker.h"
#include "AnitaConventions.h"
#include "AnitaGeomTool.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "Adu5Pat.h"
#include "Adu5Sat.h"
#include "Adu5Vtg.h"
#include "G12Pos.h"
#include "G12Sat.h"


#include "TString.h"
#include "TImage.h"
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
#include "TMarker.h"
#include "TMultiGraph.h"

AnitaGpsCanvasMaker*  AnitaGpsCanvasMaker::fgInstance = 0;
TMultiGraph *mgSat=0;


AnitaGpsCanvasMaker::AnitaGpsCanvasMaker()
{
  //Default constructor
  fgInstance=this;
  fDisplayOpt=MagicDisplayGpsDisplay::kSatView;
  fNumG12Sat=0;
  fNumAdu5ASat=0;
  fNumAdu5BSat=0;
  fReloadMap=1;
  
}

AnitaGpsCanvasMaker::~AnitaGpsCanvasMaker()
{
  //Default destructor
}



//______________________________________________________________________________
AnitaGpsCanvasMaker*  AnitaGpsCanvasMaker::Instance()
{
  //static function
  return (fgInstance) ? (AnitaGpsCanvasMaker*) fgInstance : new AnitaGpsCanvasMaker();
}


TPad *AnitaGpsCanvasMaker::getGpsMainCanvas(Adu5Sat *adu5ASatPtr, Adu5Sat *adu5BSatPtr, G12Sat *g12SatPtr, Adu5Pat *adu5APatPtr, Adu5Pat *adu5BPatPtr, G12Pos *g12PosPtr, TPad *useCan) ///<Returns the main GPS canvas
{
  static MagicDisplayGpsDisplay::MagicDisplayGpsDisplay_t lastDisplayOpt=MagicDisplayGpsDisplay::kNotAView;
  TPad *canGps;
  TPad *plotPad;
  if(!useCan) {
    canGps = (TPad*) gROOT->FindObject("canGps");
    if(!canGps) {
      canGps = new TCanvas("canGps","canGps",1000,600);
    }
    canGps->Clear();
    canGps->SetTopMargin(0);
    plotPad = new TPad("canGpsMain","canGpsMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();

  if(fDisplayOpt!=lastDisplayOpt) {
    plotPad->Clear();
    fReloadMap=1;
  }
  lastDisplayOpt=fDisplayOpt;

  if(fDisplayOpt==MagicDisplayGpsDisplay::kSatView) {
    addSatellitesToTimeArray(adu5ASatPtr,adu5BSatPtr,g12SatPtr);
    getSatelliteView(adu5ASatPtr,adu5BSatPtr,g12SatPtr,plotPad);
  }
  else if(fDisplayOpt==MagicDisplayGpsDisplay::kMapView) {
    if(adu5APatPtr)
      getMapView(adu5APatPtr,plotPad);
    else if(adu5BPatPtr)
      getMapView(adu5BPatPtr,plotPad);
    else if(g12PosPtr) {
      Adu5Pat tempPat;
      tempPat.latitude=g12PosPtr->latitude;
      tempPat.longitude=g12PosPtr->longitude;
      tempPat.altitude=g12PosPtr->altitude;
      getMapView(&tempPat,plotPad);
    }
    else {
      std::cout << "No ADU5 attitude data available will give up";
      return plotPad;
    }      
  }
  if(useCan)
    return plotPad;
  else
    return canGps;
}

TPad *AnitaGpsCanvasMaker::getGpsInfoCanvas(Adu5Sat *adu5ASatPtr, Adu5Sat *adu5BSatPtr, G12Sat *g12SatPtr, Adu5Pat *adu5APatPtr, Adu5Pat *adu5BPatPtr, G12Pos *g12PosPtr,TPad *useCan)
{
 
  char textLabel[180];
  TPad *topPad;
  if(!useCan) {
    topPad = new TPad("padGpsInfo","padGpsInfo",0.2,0.9,0.8,1);
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
  if(adu5APatPtr) {
    sprintf(textLabel,"Run:    %d",adu5APatPtr->run);
    TText *runText = leftPave->AddText(textLabel);
    runText->SetTextColor(50);
    TTimeStamp g12PosTime((time_t)adu5APatPtr->realTime,0);
    sprintf(textLabel,"Time: %s",g12PosTime.AsString("s"));  
    TText *timeText = leftPave->AddText(textLabel);
    timeText->SetTextColor(1);
  }
  else if(adu5BPatPtr) {
    sprintf(textLabel,"Run:    %d",adu5BPatPtr->run);
    TText *runText = leftPave->AddText(textLabel);
    runText->SetTextColor(50);
    TTimeStamp g12PosTime((time_t)adu5BPatPtr->realTime,0);
    sprintf(textLabel,"Time: %s",g12PosTime.AsString("s"));  
    TText *timeText = leftPave->AddText(textLabel);
    timeText->SetTextColor(1);

  }
  else if(g12PosPtr) {
    sprintf(textLabel,"Run:    %d",g12PosPtr->run);
    TText *runText = leftPave->AddText(textLabel);
    runText->SetTextColor(50);
    TTimeStamp g12PosTime((time_t)g12PosPtr->realTime,0);
    sprintf(textLabel,"Time: %s",g12PosTime.AsString("s"));  
    TText *timeText = leftPave->AddText(textLabel);
    timeText->SetTextColor(1);

  }
    leftPave->Draw();

  topPad->cd(2);
  TPaveText *rightPave = new TPaveText(0.05,0,0.95,0.9);
  rightPave->SetBorderSize(0);
  rightPave->SetFillColor(0);
  rightPave->SetTextAlign(13);
  if(g12SatPtr)
      sprintf(textLabel,"G12 Sats: %d",g12SatPtr->numSats);
  else if(g12PosPtr)
    sprintf(textLabel,"G12 Sats: %d",g12PosPtr->numSats);
  TText *g12Text = rightPave->AddText(textLabel);
  g12Text->SetTextColor(1);
  sprintf(textLabel,"ADU5 A Sats: %d %d %d %d",adu5ASatPtr->numSats[0],
	  adu5ASatPtr->numSats[1],adu5ASatPtr->numSats[2],adu5ASatPtr->numSats[3]);
  TText *adu5aText = rightPave->AddText(textLabel);
  adu5aText->SetTextColor(1);
  sprintf(textLabel,"ADU5 B Sats: %d %d %d %d",adu5BSatPtr->numSats[0],
	  adu5BSatPtr->numSats[1],adu5BSatPtr->numSats[2],adu5BSatPtr->numSats[3]);
  TText *adu5bText = rightPave->AddText(textLabel);
  adu5bText->SetTextColor(1);
  rightPave->Draw();   
  return topPad;

}

void AnitaGpsCanvasMaker::getSatelliteView(Adu5Sat *adu5ASatPtr, Adu5Sat *adu5BSatPtr, G12Sat *g12SatPtr, TPad *plotPad)
{
  if(mgSat) delete mgSat;
  plotPad->Clear();
  plotPad->Divide(1,2);
  TPad *topHalf=(TPad*)plotPad->cd(1);
  topHalf->Divide(3,1);
  TPad *g12Pad=(TPad*)topHalf->cd(1);
  g12SatPtr->getCirclePlot(g12Pad);
  g12Pad->SetEditable(0);
  TPad *adu5aPad=(TPad*)topHalf->cd(2);
  adu5ASatPtr->getCirclePlot(adu5aPad,"ADU5A");
  adu5aPad->SetEditable(0);
  TPad *adu5bPad=(TPad*)topHalf->cd(3);
  adu5BSatPtr->getCirclePlot(adu5bPad,"ADU5B");
  adu5bPad->SetEditable(0);
  plotPad->cd(2);
  mgSat = new TMultiGraph();
  Int_t shouldDraw=0;
  if(fNumG12Sat>0) {
    TGraph *grG12=new TGraph(fNumG12Sat,fG12SatTimeArray,fG12SatNumArray);
    grG12->SetLineColor(getNiceColour(0));
    grG12->SetMarkerColor(getNiceColour(0));
    grG12->SetMarkerStyle(getMarker(0));
    grG12->SetName("G12");
    mgSat->Add(grG12,"p");
    shouldDraw++;
  }
  char graphName[180];
  
  for(int ant=0;ant<4;ant++) {
    if(fNumAdu5ASat) {
      TGraph *grAdu5=new TGraph(fNumAdu5ASat,fAdu5ASatTimeArray,fAdu5ASatNumArray[ant]);
      grAdu5->SetLineColor(getNiceColour(1));
      grAdu5->SetMarkerColor(getNiceColour(1));
      grAdu5->SetMarkerStyle(getMarker(1+ant));
      sprintf(graphName,"ADU5A_%d",ant+1);
      grAdu5->SetName(graphName);
      mgSat->Add(grAdu5,"p");      
      shouldDraw++;
    }
    if(fNumAdu5BSat) {
      TGraph *grAdu5=new TGraph(fNumAdu5BSat,fAdu5BSatTimeArray,fAdu5BSatNumArray[ant]);
      grAdu5->SetLineColor(getNiceColour(2));
      grAdu5->SetMarkerColor(getNiceColour(2));
      grAdu5->SetMarkerStyle(getMarker(2+ant));
      sprintf(graphName,"ADU5B_%d",ant+1);
      grAdu5->SetName(graphName);
      mgSat->Add(grAdu5,"p");   
      shouldDraw++;
    }
  }
  if(shouldDraw) {
    mgSat->SetTitle("Number of Satellites Used");
    mgSat->Draw("ap");
    mgSat->GetXaxis()->SetTitle("Time");
    mgSat->GetYaxis()->SetTitle("Number");
    mgSat->GetXaxis()->SetTimeDisplay(1);
    gPad->Update();
    gPad->Modified();
  }

}



void AnitaGpsCanvasMaker::addSatellitesToTimeArray(Adu5Sat *adu5ASatPtr, Adu5Sat *adu5BSatPtr, G12Sat *g12SatPtr) {
  if(adu5ASatPtr) {
    if(fNumAdu5ASat>=MAX_GPS_TIME_POINTS) {
      //We'll average everythin
      fNumAdu5ASat=0;
      for(int i=0;i<MAX_GPS_TIME_POINTS/2;i++) {
	fAdu5ASatTimeArray[i]=0.5*(fAdu5ASatTimeArray[2*i]+fAdu5ASatTimeArray[(2*i)+1]);
	for(int ant=0;ant<4;ant++) {
	  fAdu5ASatNumArray[ant][i]=0.5*(fAdu5ASatNumArray[ant][2*i]+fAdu5ASatNumArray[ant][(2*i)+1]);
	}
	fNumAdu5ASat++;	
      }
    }
    fAdu5ASatTimeArray[fNumAdu5ASat]=adu5ASatPtr->realTime;
    for(int ant=0;ant<4;ant++) {
      fAdu5ASatNumArray[ant][fNumAdu5ASat]=adu5ASatPtr->numSats[ant];
    }      
    fNumAdu5ASat++;
  }
  if(adu5BSatPtr) {
    if(fNumAdu5BSat>=MAX_GPS_TIME_POINTS) {
      //We'll average everythin
      fNumAdu5BSat=0;
      for(int i=0;i<MAX_GPS_TIME_POINTS/2;i++) {
	fAdu5BSatTimeArray[i]=0.5*(fAdu5BSatTimeArray[2*i]+fAdu5BSatTimeArray[(2*i)+1]);
	for(int ant=0;ant<4;ant++) {
	  fAdu5BSatNumArray[ant][i]=0.5*(fAdu5BSatNumArray[ant][2*i]+fAdu5BSatNumArray[ant][(2*i)+1]);
	}
	fNumAdu5BSat++;	
      }
    }
    fAdu5BSatTimeArray[fNumAdu5BSat]=adu5BSatPtr->realTime;
    for(int ant=0;ant<4;ant++) {
      fAdu5BSatNumArray[ant][fNumAdu5BSat]=adu5BSatPtr->numSats[ant];
    }  
    fNumAdu5BSat++;
  }

  if(g12SatPtr) {
    if(fNumG12Sat>=MAX_GPS_TIME_POINTS) {
      //We'll average everythin
      fNumG12Sat=0;
      for(int i=0;i<MAX_GPS_TIME_POINTS/2;i++) {
	fG12SatTimeArray[i]=0.5*(fG12SatTimeArray[2*i]+fG12SatTimeArray[(2*i)+1]);
	fG12SatNumArray[i]=0.5*(fG12SatNumArray[2*i]+fG12SatNumArray[(2*i)+1]);
      }
      fNumG12Sat++;	
    }
    //    std::cout << g12SatPtr->realTime << "\n";
    fG12SatTimeArray[fNumG12Sat]=g12SatPtr->realTime;
    fG12SatNumArray[fNumG12Sat]=g12SatPtr->numSats;
    fNumG12Sat++;
  }


}

#define xSize 750
#define ySize 625

void AnitaGpsCanvasMaker::getMapView(Adu5Pat *adu5PatPtr, TPad *plotPad)
{
  //  std::cout << "fReloadMap " << fReloadMap << "\n";
  if(fReloadMap) {
    char mapFileName[FILENAME_MAX];
    plotPad->Clear();
    //    plotPad->SetCanvasSize(750,625);
    plotPad->SetTopMargin(0);
    plotPad->SetBottomMargin(0);
    plotPad->SetLeftMargin(0);
    plotPad->SetRightMargin(0);
    char *installDir=getenv("ANITA_UTIL_INSTALL_DIR");
    if(installDir) {
      sprintf(mapFileName,"%s/share/anitaMap/antarcticaIceMap.png",installDir);
    }
    else {
      sprintf(mapFileName,"antarcticaIceMap.png");
    }
    TImage *img = TImage::Open(mapFileName); //Will have to put this somewhere world visible at some point
    if(!img) {
      std::cout << "Couldn't create an image\n";
      return;
    }
    img->SetConstRatio(kFALSE);
    img->Draw("");
  }
  fReloadMap=0;
  //For now just draw one point  
  float xBalloon,yBalloon;
  float tempLong=adu5PatPtr->longitude;
  static int wentNegative=0;
  static int loop=0;
  static int lastChange=0;
  static float lastLong=0;
  static int point=0;

  //  tempLong-=7;
  //  if(tempLong<-180) tempLong+=360;
 
  getRelXYFromLatLong(adu5PatPtr->latitude,tempLong,xBalloon,yBalloon);

  if(tempLong<0) wentNegative=1;

  if(lastLong>167 || lastLong<0) {
   
    if(tempLong<167 && tempLong>0) {
      if(point>lastChange+10) {
	if(wentNegative) {
	  loop++;
	  lastChange=point;
	  wentNegative=0;
	}
      }
    }
  }
  lastLong=tempLong;
  int gpsPointerColours[5]={6,2,1,49,46};

  TMarker *mark = new TMarker();//375./xSize,312.5./ySize,21);
  if(loop>=0 && loop<=4) 
    mark->SetMarkerColor(gpsPointerColours[loop]);
  mark->SetMarkerStyle(26);
  mark->SetMarkerSize(1);
  mark->DrawMarker(xBalloon,yBalloon);

  lastLong=tempLong;
  point++;
}



const float TrueScaleLat=71;
const float CentralMeridian=0;
const float RadiusOfEarth=6378.1e3; //Metres
const float xOffest=375;
const float yOffset=312.5;
const float scale=271.5/2.19496e+06;

void AnitaGpsCanvasMaker::getXYFromLatLong(double latitude, double longitude,
					   double &x, double &y)
{
    //Negative longitude is west
 //    //All latitudes assumed south
    double absLat=TMath::Abs(latitude);
    double r=RadiusOfEarth*TMath::Cos((90.-TrueScaleLat)*TMath::DegToRad())*TMath::Tan((90-absLat)*TMath::DegToRad());
    y=r*TMath::Cos(longitude*TMath::DegToRad());
    x=r*TMath::Sin(longitude*TMath::DegToRad());    
}


//const float xSize=750;
//const float ySize=625;

void AnitaGpsCanvasMaker::getRelXYFromLatLong(float latitude, float longitude,
					      float &x, float &y)
{
    //Negative longitude is west
 //    //All latitudes assumed south
    float absLat=TMath::Abs(latitude);
    float r=RadiusOfEarth*TMath::Cos((90.-TrueScaleLat)*TMath::DegToRad())*TMath::Tan((90-absLat)*TMath::DegToRad());
    y=r*TMath::Cos(longitude*TMath::DegToRad());
    x=r*TMath::Sin(longitude*TMath::DegToRad());   

    y*=scale;
    y+=yOffset;
    y/=ySize;
    x*=scale;
    x+=xOffest;
    x/=xSize;
 
}
