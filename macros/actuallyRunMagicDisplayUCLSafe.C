#include "AnitaConventions.h"
#include <termios.h>
#include <unistd.h>

//#include "MagicDisplay.h"
R__LOAD_LIBRARY(libAnitaMagicDisplay)

void actuallyRunMagicDisplayUCLSafe(int run) {
  //  CrossCorrelator *crossCorr = new CrossCorrelator();
  
  std::cout << "Enter password: " << std::endl;

  termios oldt;
  tcgetattr(STDIN_FILENO, &oldt);
  termios newt = oldt;
  newt.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  
  string blob; getline(cin, blob);
  
  MagicDisplay *magicPtr = new MagicDisplay(TString::Format("http://anita:%s@www.hep.ucl.ac.uk/uhen/anita/private/anita4/telem1617",blob.c_str()),run,WaveCalType::kDefault); // ANITA4

  //MagicDisplay *magicPtr = new MagicDisplay(TString::Format("http://anita:%s@www.hep.ucl.ac.uk/uhen/anita/private/anita4/hawaii2016/root",blob.c_str()),run,WaveCalType::kDefault); // Hawaii

  //MagicDisplay *magicPtr = new MagicDisplay(TString::Format("http://anita:%s@www.hep.ucl.ac.uk/uhen/anita/private/anita3/flight1415/root",blob.c_str()),run,WaveCalType::kDefault); // ANITA3
  
  //MagicDisplay *magicPtr = new MagicDisplay("http://157.132.95.98/antarctica14/root",run,WaveCalType::kVTLabAG); // Ice

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  
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
