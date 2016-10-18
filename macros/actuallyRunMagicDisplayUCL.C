#include "AnitaConventions.h"
//#include "MagicDisplay.h"
R__LOAD_LIBRARY(libAnitaMagicDisplay)

void actuallyRunMagicDisplayUCL(int run) {
  //  CrossCorrelator *crossCorr = new CrossCorrelator();

  
  //  MagicDisplay *magicPtr = new MagicDisplay("http://anita:IceRadio@www.hep.ucl.ac.uk/uhen/anita/private/anita4/hawaii2016/root",run,WaveCalType::kDefault);
    MagicDisplay *magicPtr = new MagicDisplay("http://anita:IceRadio@www.hep.ucl.ac.uk/uhen/anita/private/anita3/flight1415/root/",run,WaveCalType::kDefault);
  //MagicDisplay *magicPtr = new MagicDisplay("http://157.132.95.98/antarctica14/root",run,WaveCalType::kVTLabAG);
  //  magicPtr->startSurfDisplay();
  //  magicPtr->startAvgSurfDisplay();
  //  magicPtr->startTurfDisplay();
  //  magicPtr->startSumTurfDisplay();
  //
  //  magicPtr->applyCut("trigType&0x4");
    magicPtr->startEventDisplay();
  //  magicPtr->applyCut("(priority&0xf)<5");
  //  magicPtr->applyCut("eventNumber%2==1");
  //  magicPtr->applyCut("abs(((triggerTimeNs%100e6)-50e6))<2e6"); 
  //  magicPtr->applyCut("triggerTimeNs<1e5 && triggerTimeNs>8e4");  //Taylor pulses
  //  magicPtr->applyCut("header.getCoherentSumPeak()>1000");
  //  magicPtr->startControlPanel();
  //  magicPtr->startGpsDisplay();

}
