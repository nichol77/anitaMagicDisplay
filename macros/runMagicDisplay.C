gSystem->Reset();

void runMagicDisplay() {
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

  MagicDisplay *magicPtr = new MagicDisplay("/unix/anita2/palestine08/root/",3871,WaveCalType::kJustTimeNoUnwrap);
  //  magicPtr->startSurfDisplay();
  //   magicPtr->startAvgSurfDisplay();
  //   magicPtr->startTurfDisplay();
  //  magicPtr->startSumTurfDisplay();
  magicPtr->startEventDisplay();
  //  magicPtr->startControlPanel();

}
