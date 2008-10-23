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


//!  The Magic Display control panel
/*!
  This is the horribly buggy control panel that was meant to enable easy control of magic Display, without resorting to command line power user mode. Unfortunately, it doesn't yet really work in a non segmentation violation frenzy way. I blame ROOT, others may have different opinions.
*/
class MagicControlPanel : public TGMainFrame
{
 private:
 
 public:
   TGTextButton    *fButton0; ///< Text button.
   TGTextButton    *fButton1; ///< Text button.
   TGTextButton    *fButton2; ///< Text button.
   TGTextButton    *fButton3; ///< Text button.
   TGTextButton    *fGotoBut; ///< Text button.
   TGCheckButton   *fEventBut; ///< Check button
   TGCheckButton   *fSurfBut; ///< Check button
   TGCheckButton   *fTurfBut; ///< Check button
   TGCheckButton   *fAvgSurfBut; ///< Check button
   TGCheckButton   *fSumTurfBut; ///< Check button
   TGLayoutHints   *fLayout; ///< Layout nonsense
   TGLayoutHints   *fLeftLayout; ///< Layout nonsense
   TGLayoutHints   *fRightLayout; ///< Layout nonsense
   TGLayoutHints   *fButtonLayout; ///< Layout nonsense
   TGLayoutHints   *fCenterLayout; ///< Layout nonsense
   TGNumberEntry   *fRunEntry; ///< Number entry form
   TGNumberEntry   *fEventEntry; ///< Number entry form
   TGLabel         *fRunLabel; ///< It's a label
   TGLabel         *fEventLabel; ///< It's a label
   TGLabel         *fQuickJumpLabel; ///< It's a label
   TGLabel         *fUpdateLabel; ///< It's a label

   TGVerticalFrame *fEntryPanel; ///< A vertical frame
   TGHorizontalFrame *fRunPanel; ///< A horizontal frame
   TGHorizontalFrame *fEventPanel; ///< A horizontal frame
   TGVerticalFrame *fButtonPanel; ///< A vertical frame



   MagicControlPanel(); ///< The default constructor
   MagicControlPanel(const TGWindow *p, UInt_t w, UInt_t h); ///< The constructor
   virtual ~MagicControlPanel(); ///< The destructor

   virtual void CloseWindow(); ///< ROOT underlying code fragment
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t); ///< ROOT underlying code fragment

  

  //Instance generator
   static MagicControlPanel*  Instance(); ///< The instance generator (there should only be one instance of the MagicControlPanel at a time
  
  

 protected:
   static MagicControlPanel *fgInstance;   ///< Protect against multiple instances

   ClassDef(MagicControlPanel,1); ///< One of the magic ROOT macros.
};


#endif //MAGICCONTROLPANEL_H
