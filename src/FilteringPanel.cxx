#include "FilteringPanel.h"
#include "MagicDisplay.h"


ClassImp(FilteringPanel)

FilteringPanel*  FilteringPanel::fgInstance = 0;


enum ETestCommandIdentifiers {
  M_APPLY_BUT = 1,
};

FilteringPanel::FilteringPanel()
{
  //Default constructor 
  fgInstance=this;  
  //MagicDisplay Stuff
  MagicDisplay *magicDisPtr = MagicDisplay::Instance();

  fMainFrame = new TGMainFrame(gClient->GetRoot(),10,10,kVerticalFrame);

  const int widthPx = 200;
  const int heightPx = 1000;  
  // fEntryPanel = new TGVerticalFrame(fMainFrame,200,300);
  fEntryPanel = new TGVerticalFrame(fMainFrame,widthPx,heightPx);  
  fLeftLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft,2,2,2,2);
  fRightLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight,2,2,2,2);
  fCenterLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX,2,2,2,2);
  fMainFrame->AddFrame(fEntryPanel,fLeftLayout);

  // fRunPanel = new TGHorizontalFrame(fEntryPanel,200,30);
  // fEntryPanel->AddFrame(fRunPanel,fRightLayout);
  // fRunEntry =new TGNumberEntry(fRunPanel,magicDisPtr->getCurrentRun(),12,20,(TGNumberEntry::EStyle)0);
  // fRunPanel->AddFrame(fRunEntry,fRightLayout);
  // fRunLabel = new TGLabel(fRunPanel,"Run: ");
  // fRunPanel->AddFrame(fRunLabel,fRightLayout);

  // fEventPanel = new TGHorizontalFrame(fEntryPanel,200,30);
  // fEntryPanel->AddFrame(fEventPanel,fRightLayout);
  // fEventEntry =new TGNumberEntry(fEventPanel,magicDisPtr->getCurrentEvent(),12,20,(TGNumberEntry::EStyle)0);  
  // fEventPanel->AddFrame(fEventEntry,fRightLayout);
  // fEventLabel = new TGLabel(fEventPanel,"Event: ");
  // fEventPanel->AddFrame(fEventLabel,fRightLayout);

  fApplyBut = new TGTextButton(fEntryPanel, "&Apply", M_APPLY_BUT);
  //  fGotoBut->SetBackgroundColor(TColor::Number2Pixel(5));
  //  fGotoBut->Associate(fMainFrame);
  fApplyBut->Connect("Pressed()","FilteringPanel",this,"apply()");
  fEntryPanel->AddFrame(fApplyBut, fCenterLayout);

  fMainFrame->SetWindowName("FilteringPanel");
  fMainFrame->MapSubwindows();

  
  fMainFrame->Connect("CloseWindow()", "FilteringPanel", this, "closeWindow()");

   // we need to use GetDefault...() to initialize the layout algorithm...
  fMainFrame->Resize();   // resize to default size
  fMainFrame->MapRaised();
  //  fMainFrame->Print();   
 
}


//______________________________________________________________________________
FilteringPanel*  FilteringPanel::Instance()
{
  //static function
  return (fgInstance) ? (FilteringPanel*) fgInstance : new FilteringPanel();
}


FilteringPanel::~FilteringPanel()
{
}

void FilteringPanel::closeWindow()
{
  fgInstance = 0;
  delete this;
}


void FilteringPanel::apply()
{
  std::cout << "Congratulations, you pressed the button!" << std::endl;
}
