#include "FilteringPanel.h"
#include "MagicDisplay.h"
#include "FilterOperation.h"

ClassImp(FilteringPanel)

FilteringPanel*  FilteringPanel::fgInstance = 0;


enum ETestCommandIdentifiers {
  M_APPLY_BUT = 1
  // M_CANCEL_BUT 
};

FilteringPanel::FilteringPanel()
{
  //Default constructor 
  fgInstance=this;  
  //MagicDisplay Stuff

  // fMainFrame = new TGMainFrame(gClient->GetRoot(),200,300,kVerticalFrame);
  fMainFrame = new TGMainFrame(gClient->GetRoot(),200,300,kHorizontalFrame);  

  TGVerticalFrame* fCurrentStratFrame = new TGVerticalFrame(fMainFrame,200,300);  
  TGVerticalFrame* fSelectedStratFrame = new TGVerticalFrame(fMainFrame,200,300);  

  fLeftLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft,2,2,2,2);
  fRightLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight,2,2,2,2);
  fCenterLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX,2,2,2,2);
  // fMainFrame->AddFrame(fEntryPanel,fLeftLayout);

  fCombo = new TGComboBox(fMainFrame,100);  

  MagicDisplay *md = MagicDisplay::Instance();
  FilterStrategy* currentStrat = md->getStrategy();

  for(unsigned i=0; i < currentStrat->nOperations(); i++){
    const FilterOperation * op = currentStrat->getOperation(i); 
    std::cout << i << "\t" << op->description() << std::endl;
  }
  std::map<TString, FilterStrategy*> filterStrats = md->getFilterStrats();
  std::map<TString, FilterStrategy*>::iterator it = filterStrats.begin();



  int entry=1;
  int thisEntry = entry;
  TString stratName;  
  for(; it!=filterStrats.end(); ++it){

    fCombo->AddEntry(it->first.Data(), entry);

    if(it->second == currentStrat){
      thisEntry = entry;
      stratName = it->first;
    }
    entry++;
  }
  fCombo->Resize(150, 20);
  fCombo->Select(thisEntry);


  // create buttons
  fApplyBut = new TGTextButton(fMainFrame, "&Apply", M_APPLY_BUT);
  fApplyBut->Connect("Pressed()","FilteringPanel",this,"apply()");
  
  // fCancelBut = new TGTextButton(fMainFrame, "&Cancel", M_CANCEL_BUT);
  // fCancelBut->Connect("Pressed()","FilteringPanel",this,"closeWindow()");
  



  TGTextView* fAppliedText = new TGTextView(fCurrentStratFrame, 100, 200, stratName.Data());
  TGTextView* fSelectedText = new TGTextView(fSelectedStratFrame, 100, 200, "");



  fMainFrame->AddFrame(fAppliedText, fCenterLayout);
  fMainFrame->AddFrame(fCombo, fCenterLayout);
  fMainFrame->AddFrame(fSelectedText, fCenterLayout);  
  fMainFrame->AddFrame(fApplyBut, fCenterLayout);

  // fMainFrame->AddFrame(fCancelBut, fCenterLayout);  
  // fMainFrame->AddFrame(fAppliedText, fCenterLayout);
  // fMainFrame->AddFrame(fCombo, fCenterLayout);
  // fMainFrame->AddFrame(fSelectedText, fCenterLayout);  
  // fMainFrame->AddFrame(fApplyBut, fCenterLayout);
  // fMainFrame->AddFrame(fCancelBut, fCenterLayout);  
  
  

  fMainFrame->AddFrame(fCurrentStratFrame, fCenterLayout);
  fMainFrame->AddFrame(fSelectedStratFrame, fCenterLayout);
  
  
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
  FilteringPanel* fp = (fgInstance) ? (FilteringPanel*) fgInstance : new FilteringPanel();
  // fp->fMainFrame->cd();
  fp->fMainFrame->RequestFocus();
  return fp;  
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

      for(unsigned i=0; i < it->second->nOperations(); i++){
	const FilterOperation * op = it->second->getOperation(i); 
	std::cout << i << "\t" << op->description() << std::endl;
      }
      
      break;
    }
    entry++;
  }

  
  
}
