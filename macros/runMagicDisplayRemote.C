gSystem->Reset();

void runMagicDisplayRemote() {
  runMagicDisplayRemoteRun(3492);
}

void runMagicDisplayRemoteRun(int run) {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  //  cout << gSystem->GetIncludePath() <<endl;
		
  gSystem->Load("libfftw3.so");
  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libRootFftwWrapper.so");     	  
  gSystem->Load("libAnitaEvent.so");   	  
  gSystem->Load("libMagicDisplay.so");

  TChain *fred=0; //Will this work?

  //  MagicDisplay *magicPtr = new MagicDisplay("root://anita@192.168.2.2//data/anita/palestine08/webPlotter/events/root",run);
  MagicDisplay *magicPtr = new MagicDisplay("root://rjn@pc126//unix/anita2/palestine08//root",run);
  //  magicPtr->startSurfDisplay();
  //   magicPtr->startAvgSurfDisplay();
  magicPtr->startTurfDisplay();
  //  magicPtr->startSumTurfDisplay();
  magicPtr->startEventDisplay();
  //  magicPtr->startControlPanel();

}
