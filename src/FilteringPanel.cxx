#include "FilteringPanel.h"
#include "MagicDisplay.h"
#include "FilterOperation.h"
#include "KeySymbols.h"
#include "TVirtualX.h"

ClassImp(FilteringPanel)

FilteringPanel*  FilteringPanel::fgInstance = 0;


FilteringPanel::FilteringPanel() : TGMainFrame(gClient->GetRoot(),1200,300,kVerticalFrame)
{
  //Default constructor 
  fgInstance=this;  
  //MagicDisplay Stuff

  fCombo = new TGComboBox(this,100);
  fAddedEntries = false; // must come before selectMagicDisplayFilterInComboBox in constructor;
  selectMagicDisplayFilterInComboBox();
  fCombo->Connect("Selected(Int_t)", "FilteringPanel", this, "updateTextAndSetFilter()"); // then connect selection to updating MagicDisplay
  fCombo->Resize(150, 20); // needed to set reasonable height.
  
  int id=0;
  fSelectedTextView = new TGTextView(this, 1200, 100, id, kLHintsExpandX | kLHintsExpandY);  

  TColor* yellow = gROOT->GetColor(kYellow - 9); // -9 tones down the yellow, which is a bit in-your-face otherwise
  fSelectedTextView->SetBackgroundColor(yellow->GetPixel());  
  
  // this->AddFrame(fCombo, new TGLayoutHints(kLHintsTop | kLHintsLeft,2,2,2,2));
  this->AddFrame(fCombo, new TGLayoutHints(kLHintsTop | kLHintsExpandX,2,2,2,2));  
  this->AddFrame(fSelectedTextView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));  

  this->SetWindowName("FilteringPanel");
  this->MapSubwindows();  
  this->Connect("CloseWindow()", "FilteringPanel", this, "closeWindow()");  

  this->Resize();
  this->MapRaised();

  updateTextAndSetFilter();
  gVirtualX->GrabKey(fId, kAnyKey, kAnyModifier, kTRUE);
  

}


Bool_t FilteringPanel::HandleKey(Event_t* ev){

  // want a special case for the F/f keys to close the window...
  if(ev->fType == kGKeyPress){

    UInt_t keysym; 
    char tmp[2]; 
    gVirtualX->LookupString(ev,tmp,sizeof(tmp),keysym);
    switch(keysym){
    case kKey_Q:
    case kKey_q:
    case kKey_F:
    case kKey_f:
      CloseWindow();
      return false;
    default:
      ;
      // do nothing
    }
  }
  
  return MagicDisplay::Instance()->HandleKey(ev);
}


//______________________________________________________________________________
FilteringPanel*  FilteringPanel::Instance()
{

  // slightly different instance function, force redisplay by deleting if there is an instance...

  if(fgInstance){
    fgInstance->CloseWindow();
  }
  return new FilteringPanel();
  // FilteringPanel* fp = (fgInstance) ? (FilteringPanel*) fgInstance : new FilteringPanel();
  // // 
  // // fp->this->cd();
  // // fp->this->RequestFocus();
  // return fp;  
}


FilteringPanel::~FilteringPanel()
{
  fgInstance = 0;
  MagicDisplay* md = MagicDisplay::Instance();
  md->fFilteringPanel = 0;
}

void FilteringPanel::closeWindow()
{
  delete this;
}



void FilteringPanel::selectMagicDisplayFilterInComboBox(){

  MagicDisplay *md = MagicDisplay::Instance();
  FilterStrategy* currentStrat = md->getStrategy();
  std::map<TString, FilterStrategy*> filterStrats = md->getFilterStrats();
  std::map<TString, FilterStrategy*>::iterator it = filterStrats.begin();

  int entry=1;
  int thisEntry = entry;
  TString stratName;  
  for(; it!=filterStrats.end(); ++it){

    if(!fAddedEntries){
      fCombo->AddEntry(it->first.Data(), entry);
    }
    
    if(it->second == currentStrat){
      thisEntry = entry;
      stratName = it->first;
    }
    entry++;
  }  
  fCombo->Select(thisEntry); // select the current filter

  fAddedEntries = true;
}


void FilteringPanel::updateText(){//TGTextView* tv, std::map<TString, FilterStrategy*>::iterator it){

  fSelectedTextView->Clear();

  int selectedEntry = fCombo->GetSelected();

  MagicDisplay* md = MagicDisplay::Instance();
  std::map<TString, FilterStrategy*> filterStrats = md->getFilterStrats();
  std::map<TString, FilterStrategy*>::iterator it = filterStrats.begin();
  
  int entry=1;
  for(; it!=filterStrats.end(); ++it){
    if(entry==selectedEntry){      
      break;      
    }
    entry++;
  }  
  
   // Set an appropriate title, showing the filter strategy name in MagicDisplay

  TString firstLine = it->first + "\n\n";

  fSelectedTextView->AddLineFast(firstLine.Data());
  
  for(unsigned i=0; i < it->second->nOperations(); i++){
    const FilterOperation * op = it->second->getOperation(i);
    TString thisLine = TString::Format("Step %u: %s \n", i+1, op->description());
    fSelectedTextView->AddLineFast(thisLine.Data());
  }

  fSelectedTextView->Update();

}





void FilteringPanel::setFilter(){//TGTextView* tv, std::map<TString, FilterStrategy*>::iterator it){

  int selectedEntry = fCombo->GetSelected();

  MagicDisplay* md = MagicDisplay::Instance();
  std::map<TString, FilterStrategy*> filterStrats = md->getFilterStrats();
  std::map<TString, FilterStrategy*>::iterator it = filterStrats.begin();
  
  int entry=1;
  for(; it!=filterStrats.end(); ++it){
    if(entry==selectedEntry){      
      md->setFilterStrategy(it->second);
      // setText(fAppliedTexfSelectedTextViewiew, it);
      break;
      
    }
    entry++;
  }  
}
