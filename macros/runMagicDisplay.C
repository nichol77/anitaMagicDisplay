#include "AnitaConventions.h"


void runMagicDisplayRun(int run);

void runMagicDisplay(int run=1002) {
  cout << "runMagicDisplay(" << run << ")\n";

  gSystem->Load("libNet.so");

  TChain *fred=0; //Will this work?
  runMagicDisplayRun(run);
}


void runMagicDisplayRun(int run) {

  MagicDisplay *magicPtr = new MagicDisplay(getenv("ANITA_ROOT_DATA"),run,WaveCalType::kDefault);

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
