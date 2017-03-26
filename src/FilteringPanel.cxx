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

  TGVerticalFrame* fAppliedFrame = new TGVerticalFrame(fMainFrame,200,300);  
  TGVerticalFrame* fSelectedFrame = new TGVerticalFrame(fMainFrame,200,300);  

  fLeftLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft,2,2,2,2);
  fRightLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight,2,2,2,2);
  fCenterLayout = new TGLayoutHints(kLHintsCenterY | kLHintsCenterX,2,2,2,2);
  // fMainFrame->AddFrame(fEntryPanel,fLeftLayout);

  fCombo = new TGComboBox(fSelectedFrame,100);
  // fCombo = new TGComboBox(fMainFrame,100);    

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
  fApplyBut = new TGTextButton(fSelectedFrame, "&Apply", M_APPLY_BUT);
  // fApplyBut = new TGTextButton(fMainFrame, "&Apply", M_APPLY_BUT);  
  fApplyBut->Connect("Pressed()","FilteringPanel",this,"apply()");
  
  // fCancelBut = new TGTextButton(fMainFrame, "&Cancel", M_CANCEL_BUT);
  // fCancelBut->Connect("Pressed()","FilteringPanel",this,"closeWindow()");
  

  fMainFrame->AddFrame(fAppliedFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  fMainFrame->AddFrame(fSelectedFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  

  Pixel_t backpxl;
  gClient->GetColorByName("#c0c0c0", backpxl);

  // new TGTextView(this, 500, 94, fIDs.GetUnID(), kFixedWidth | kFixedHeight)

  int id=0;
  fAppliedTextView = new TGTextView(fAppliedFrame, 500, 94, id++, kFixedWidth | kFixedHeight);
  fSelectedTextView = new TGTextView(fSelectedFrame, 500, 94, id++, kFixedWidth | kFixedHeight);
  it = filterStrats.find(stratName);
  setText(fAppliedText, fAppliedTextView, it);
  setText(fSelectedText, fSelectedTextView, it);

  fAppliedTextView->SetBackgroundColor(backpxl);
  fSelectedTextView->SetBackgroundColor(backpxl);
  
  fAppliedFrame->AddFrame(fAppliedTextView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

  fSelectedFrame->AddFrame(fCombo, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  fSelectedFrame->AddFrame(fSelectedTextView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));  
  fSelectedFrame->AddFrame(fApplyBut, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

  fAppliedFrame->MapSubwindows();
  fSelectedFrame->MapSubwindows();    


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
  // fp->fMainFrame->RequestFocus();
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

  // fAppliedTextView->Clear();
  
  int selectedEntry = fCombo->GetSelected();
  // std::cout << fCombo << "\t" << selectedEntry << std::endl;
  int entry=1;
  for(; it!=filterStrats.end(); ++it){
    // std::cout << it->first.Data() << "\t" << entry << std::endl;
    if(entry==selectedEntry){
      
      md->setFilterStrategy(it->second);
      setText(fAppliedText, fAppliedTextView, it);
      break;
      
    }
    entry++;
  }  
}


void FilteringPanel::updateText(){
  fAppliedTextView->Clear();

  // Now have a TObjArray of TObjStrings split by out delimeter :
  TObjArray* tokens = fAppliedText.Tokenize("\n");

  int nTokens = tokens->GetEntries();

  // If there are two tokens, then there was one delimeter
  for(int i=0; i < nTokens; i++){
    TString line = ((TObjString*) tokens->At(i))->GetString();
    fAppliedTextView->AddLineFast(line.Data());
  }  
}


void FilteringPanel::setText(TString& str, TGTextView* tv, std::map<TString, FilterStrategy*>::iterator it){



  str.Clear();

  
   // Set an appropriate title, showing the filter strategy name in MagicDisplay
  str += tv == fAppliedTextView ? "Currently applied FilterStrategy: " : "Selected applied FilterStrategy: ";
  str += it->first + "\n";
  
  for(unsigned i=0; i < it->second->nOperations(); i++){
    const FilterOperation * op = it->second->getOperation(i);
    str += TString::Format("Step %u: %s \n", i+1, op->description());
  }

  std::cout << str.Data() << std::endl;
  updateText();
}
