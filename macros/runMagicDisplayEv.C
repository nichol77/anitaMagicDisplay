#include "AnitaConventions.h"
#include "AnitaVersion.h"

int evToRun(int ev) {

  char* installDir = getenv("ANITA_UTIL_INSTALL_DIR");

  stringstream name;
  ifstream runToEv;
  if (AnitaVersion::get() == 3) {
    name.str("");
    name << installDir << "/bin/runToEvA3.txt";
    cout << "Using: " << name.str() << " as translation file" << endl;
    runToEv.open(name.str().c_str());
    if (!runToEv.is_open()) {
      cout << "I couldn't open that file though sorry!" << endl;
      return -1;
    }

  }
  else {
    cout << "I only have a runToEv list for Anita3 right now sorry :(" << endl;
    return -1;
  }

  int run,evLow,evHigh;
  int runOut = -1;
  while (runToEv >> run >> evLow >> evHigh)
    if (ev >= evLow && ev <= evHigh) {
      runOut = run;
    }

  runToEv.close();
  
  return runOut;

}



void runMagicDisplayEv(int ev) {

  int anita = AnitaVersion::get();

  cout << "Using AnitaVersion " << anita << endl;

  int run = evToRun(ev);
  if (run == -1) {
    cout << "Couldn't relate event to run :( Quitting" << endl;
    return;
  }

  cout << "Event number " << ev << " is in run " << run << endl;

  MagicDisplay *magicPtr = new MagicDisplay(run,(AnitaDataset::DataDirectory) anita, WaveCalType::kDefault);

  magicPtr->startEventDisplay();

  magicPtr->displayThisEvent(ev,run); 


}
