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

  fMainFrame = new TGMainFrame(gClient->GetRoot(),10,10,kVerticalFrame);
  fEntryPanel = new TGVerticalFrame(fMainFrame,200,300);  
  fLeftLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft,2,2,2,2);
  fRightLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight,2,2,2,2);
  fCenterLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX,2,2,2,2);
  fMainFrame->AddFrame(fEntryPanel,fLeftLayout);

  fCombo = new TGComboBox(fMainFrame,100);

  MagicDisplay *md = MagicDisplay::Instance();
  FilterStrategy* currentStrat = md->getStrategy();
  std::map<TString, FilterStrategy*> filterStrats = md->getFilterStrats();
  std::map<TString, FilterStrategy*>::iterator it = filterStrats.begin();

  int entry=1;
  int thisEntry = entry;
  for(; it!=filterStrats.end(); ++it){

    fCombo->AddEntry(it->first.Data(), entry);

    if(it->second == currentStrat){
      thisEntry = entry;
    }
    entry++;
  }
  fCombo->Resize(150, 20);
  fCombo->Select(thisEntry);

  fMainFrame->AddFrame(fCombo, fCenterLayout);  

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
  // std::cout << "Congratulations, you pressed the button!" << std::endl;

  MagicDisplay* md = MagicDisplay::Instance();
  std::map<TString, FilterStrategy*> filterStrats = md->getFilterStrats();
  std::map<TString, FilterStrategy*>::iterator it = filterStrats.begin();
  
  int selectedEntry = fCombo->GetSelected();
  // std::cout << fCombo << "\t" << selectedEntry << std::endl;
  int entry=1;
  for(; it!=filterStrats.end(); ++it){
    // std::cout << it->first.Data() << "\t" << entry << std::endl;
    if(entry==selectedEntry){
      md->setFilterStrategy(it->second);
      std::cout << "Found it! " << it->first.Data() << std::endl;
      break;
    }
    entry++;
  }
  
}
