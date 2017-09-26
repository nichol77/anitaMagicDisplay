#include "AnitaConventions.h"
#include "AnitaVersion.h" 
#include "AnitaDataset.h" 





void runMagicDisplayPlaylist(const char* playlist = "hc2adirect.txt", int anita=AnitaVersion::get()) {

  AnitaVersion::set(anita);

  MagicDisplay *magicPtr = new MagicDisplay(playlist,(AnitaDataset::DataDirectory) anita,WaveCalType::kDefault);

  //magicPtr->startSurfDisplay();
  //  magicPtr->startAvgSurfDisplay();
  //   magicPtr->startTurfDisplay();
  //  magicPtr->startSumTurfDisplay();
  magicPtr->startEventDisplay();
  // magicPtr->applyCut("triggerTimeNs>999.9e6 && triggerTimeNs<1000e6 && eventNumber%2==1");
  //  magicPtr->applyCut("eventNumber%2==1");

  //  magicPtr->startGpsDisplay();
  //  magicPtr->startControlPanel();

}
