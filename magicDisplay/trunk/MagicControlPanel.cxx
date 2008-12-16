//////////////////////////////////////////////////////////////////////////////
/////  MagicControlPanel.cxx        Magic Display Control Panel          /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for the Magic Display Control Panel                      /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include "MagicDisplayConventions.h"
#include "MagicControlPanel.h"
#include "MagicDisplay.h"

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
#include "TApplication.h"


ClassImp(MagicControlPanel) ;

MagicControlPanel*  MagicControlPanel::fgInstance = 0;


enum ETestCommandIdentifiers {
  M_EVENT_BUT =1,
  M_SURF_BUT,
  M_TURF_BUT,
  M_AVG_SURF_BUT,
  M_SUM_TURF_BUT,
  M_GOTO_EVENT
};

MagicControlPanel::MagicControlPanel()
{
  //Default constructor 
  fgInstance=this;  
  //MagicDisplay Stuff
  MagicDisplay *magicDisPtr = MagicDisplay::Instance();

  fMainFrame = new TGMainFrame(gClient->GetRoot(),10,10,kVerticalFrame);


  fEntryPanel = new TGVerticalFrame(fMainFrame,200,300);
  fLeftLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft,2,2,2,2);
  fRightLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight,2,2,2,2);
  fCenterLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX,2,2,2,2);
  fMainFrame->AddFrame(fEntryPanel,fLeftLayout);

  fRunPanel = new TGHorizontalFrame(fEntryPanel,200,30);
  fEntryPanel->AddFrame(fRunPanel,fRightLayout);
  fRunEntry =new TGNumberEntry(fRunPanel,magicDisPtr->getCurrentRun(),12,20,(TGNumberEntry::EStyle)0);
  fRunPanel->AddFrame(fRunEntry,fRightLayout);
  fRunLabel = new TGLabel(fRunPanel,"Run: ");
  fRunPanel->AddFrame(fRunLabel,fRightLayout);

  fEventPanel = new TGHorizontalFrame(fEntryPanel,200,30);
  fEntryPanel->AddFrame(fEventPanel,fRightLayout);
  fEventEntry =new TGNumberEntry(fEventPanel,magicDisPtr->getCurrentEvent(),12,20,(TGNumberEntry::EStyle)0);  
  fEventPanel->AddFrame(fEventEntry,fRightLayout);
  fEventLabel = new TGLabel(fEventPanel,"Event: ");
  fEventPanel->AddFrame(fEventLabel,fRightLayout);

  fGotoBut = new TGTextButton(fEntryPanel, "&Go to Event", M_GOTO_EVENT);
  //  fGotoBut->SetBackgroundColor(TColor::Number2Pixel(5));
  //  fGotoBut->Associate(fMainFrame);
  fGotoBut->Connect("Pressed()","MagicControlPanel",this,"goToEvent()");
  fEntryPanel->AddFrame(fGotoBut, fCenterLayout);

//   //  fMainFrame->SetBackgroundColor(TColor::Number2Pixel(kRed));
//   fButtonPanel = new TGVerticalFrame(fMainFrame,200,500);
//   fUpdateLabel = new TGLabel(fButtonPanel,"Update Canvas?");
//   fButtonPanel->AddFrame(fUpdateLabel,fLeftLayout);
//   fButtonLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2);
//   fMainFrame->AddFrame(fButtonPanel,fButtonLayout);
//   fEventBut = new TGCheckButton(fButtonPanel, "Event Canvas", M_EVENT_BUT);
//   fEventBut->Associate(fMainFrame);
//   fEventBut->SetBackgroundColor(TColor::Number2Pixel(5));
//   fButtonPanel->AddFrame(fEventBut, fLayout);
//   fSurfBut = new TGCheckButton(fButtonPanel, "SURF Canvas", M_SURF_BUT);
//   fSurfBut->Associate(fMainFrame);
//   fSurfBut->SetBackgroundColor(TColor::Number2Pixel(5));
//   fButtonPanel->AddFrame(fSurfBut, fLayout);
//   fTurfBut = new TGCheckButton(fButtonPanel, "TURF Canvas", M_TURF_BUT);
//   fTurfBut->Associate(fMainFrame);
//   fTurfBut->SetBackgroundColor(TColor::Number2Pixel(5));
//   fButtonPanel->AddFrame(fTurfBut, fLayout);
//   fAvgSurfBut = new TGCheckButton(fButtonPanel, "Avg. SURF Canvas", M_AVG_SURF_BUT);
//   fAvgSurfBut->Associate(fMainFrame);
//   fAvgSurfBut->SetBackgroundColor(TColor::Number2Pixel(5));
//   fButtonPanel->AddFrame(fAvgSurfBut, fLayout);
//   fSumTurfBut = new TGCheckButton(fButtonPanel, "Sum. TURF Canvas", M_SUM_TURF_BUT);
//   fSumTurfBut->Associate(fMainFrame);
//   fSumTurfBut->SetBackgroundColor(TColor::Number2Pixel(5));
//   fButtonPanel->AddFrame(fSumTurfBut, fLayout);




  fMainFrame->SetWindowName("MagicControlPanel");
  fMainFrame->MapSubwindows();

  
  fMainFrame->Connect("CloseWindow()", "MagicControlPanel", this, "closeControlPanel()");

   // we need to use GetDefault...() to initialize the layout algorithm...
  fMainFrame->Resize();   // resize to default size
  fMainFrame->MapRaised();
  //  fMainFrame->Print();   
 
}


//______________________________________________________________________________
MagicControlPanel*  MagicControlPanel::Instance()
{
  //static function
  return (fgInstance) ? (MagicControlPanel*) fgInstance : new MagicControlPanel();
}


  


MagicControlPanel::~MagicControlPanel()
{
}

void MagicControlPanel::goToEvent() 
{
  //  std::cout << "Go To Event: " << this->fRunEntry->GetNumber()
  //	    << "\t" << (Int_t) this->fEventEntry->GetNumber()
  //	    << std::endl;
  MagicDisplay::Instance()->displayThisEvent((UInt_t)this->fEventEntry->GetNumber(),(UInt_t)this->fRunEntry->GetNumber());

}

void MagicControlPanel::closeControlPanel()
{
  fgInstance=0;
  delete this;
}


//void MagicControlPanel::CloseWindow()
//{
   // Got close message for this MainFrame. Terminate the application
   // or returns from the TApplication event loop (depending on the
   // argument specified in TApplication::Run()).
//}

// Bool_t MagicControlPanel::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
// {
//    // Handle messages send to the MagicControlPanel object. E.g. all menu button
//     switch (GET_MSG(msg)) {
//    case kC_COMMAND:
//       {
//          switch (GET_SUBMSG(msg)) {
//          case kCM_BUTTON:
//             {
//                switch (parm1) {
//                   // exit button
//                case M_EVENT_BUT:
//                   {
// 		    //                     CloseWindow();
// 		    std::cout << "Event But" << std::endl;
//                      break;
//                   }
//                case M_SURF_BUT:
//                   {
// 		    //                     CloseWindow();
// 		    std::cout << "SURF But" << std::endl;
//                      break;
//                   }
//                   // set button
//                case M_GOTO_EVENT:
//                   {
// 		    std::cout << "Goto Event: " << this->fRunEntry->GetNumber()
// 			      << "\t" << this->fEventEntry->GetNumber()
// 			      << std::endl;
// 		    std::cout << "Event State: " << this->fEventBut->GetState() << std::endl;

// 		    if(this->fEventBut->GetState()) {
// 		      MagicDisplay::Instance()->displayThisEvent((UInt_t)this->fEventEntry->GetNumber(),(UInt_t)this->fRunEntry->GetNumber());
// 		  }
		    
// 		    break;
//                   }
//                }
//                break;
//             }
//          }
//          break;
//       }
//    }
//    return kTRUE;


// }
