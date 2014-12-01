gSystem->Reset();

void runMagicDisplay(int run=10120) {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  //  cout << gSystem->GetIncludePath() <<endl;
		
  gSystem->Load("libfftw3.so");

  gSystem->Load("libMathMore.so");
  gSystem->Load("libGeom.so");;
  gSystem->Load("libGraf3d.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libRootFftwWrapper.so");     	  
  gSystem->Load("libAnitaEvent.so");   	  
  gSystem->Load("libMagicDisplay.so");

  TChain *fred=0; //Will this work?
  runMagicDisplayRun(run);
}


void runMagicDisplayRun(int run) {
  //  MagicDisplay *magicPtr = new MagicDisplay("/anitaStorage/antarctica14/root",run,WaveCalType::kVoltageTime);  //kAddPeds
  MagicDisplay *magicPtr = new MagicDisplay("/anitaStorage/antarctica14/root",run,WaveCalType::kVTLabAG);  //kAddPeds

  //magicPtr->startSurfDisplay();
  //  magicPtr->startAvgSurfDisplay();
  //   magicPtr->startTurfDisplay();
  //  magicPtr->startSumTurfDisplay();
  magicPtr->startEventDisplay();
  //  magicPtr->applyCut("triggerTimeNs>999.9e6 && triggerTimeNs<1000e6 && eventNumber%2==1");
  //  magicPtr->applyCut("eventNumber%2==1");

  //  magicPtr->startGpsDisplay();
  //  magicPtr->startControlPanel();

}
