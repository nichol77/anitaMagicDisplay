//////////////////////////////////////////////////////////////////////////////
/////  MagicControlPanel.h        Magic Display Control Panel            /////
/////                                                                    /////
/////  Description:                                                      /////
/////                                                                    ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef MAGICCONTROLPANEL_H
#define MAGICCONTROLPANEL_H


#include <TGResourcePool.h>
#include <TGListBox.h>
#include <TGListTree.h>
#include <TGFSContainer.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
#include <TGTextEdit.h>
#include <TGShutter.h>
#include <TGProgressBar.h>
#include <TGColorSelect.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TEnv.h>
#include <TFile.h>
#include <TKey.h>
#include <TGDockableFrame.h>
#include <TGFontDialog.h>


class MagicControlPanel : public TGMainFrame
{
 private:
 
 public:
  TGTextButton    *fButton0,*fButton1,*fButton2,*fButton3;
  TGTextButton    *fGotoBut;
  TGCheckButton   *fEventBut,*fSurfBut,*fTurfBut,*fAvgSurfBut,*fSumTurfBut;
  TGLayoutHints   *fLayout, *fLeftLayout, *fRightLayout, *fButtonLayout, *fCenterLayout;
  TGNumberEntry   *fRunEntry;
  TGNumberEntry   *fEventEntry;
  TGLabel         *fRunLabel,*fEventLabel,*fQuickJumpLabel,*fUpdateLabel;

  TGVerticalFrame *fEntryPanel;
  TGHorizontalFrame *fRunPanel;
  TGHorizontalFrame *fEventPanel;
  TGVerticalFrame *fButtonPanel;



    MagicControlPanel();
  //  virtual ~MagicControlPanel();

  MagicControlPanel(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~MagicControlPanel();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);

  

  //Instance generator
  static MagicControlPanel*  Instance();
  
  

 protected:
   static MagicControlPanel *fgInstance;  
   // protect against multiple instances


   ClassDef(MagicControlPanel,1);
};


#endif //MAGICCONTROLPANEL_H
