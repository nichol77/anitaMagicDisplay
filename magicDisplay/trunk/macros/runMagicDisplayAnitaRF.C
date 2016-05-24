gSystem->Reset();

void runMagicDisplayAnitaRF(int run) {
  runMagicDisplayRemoteRun(run);
}

void runMagicDisplayRemoteRun(int run) {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  //  cout << gSystem->GetIncludePath() <<endl;
		
  gSystem->Load("libfftw3.so");
  gSystem->Load("libgsl.so");
  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libGeom.so");  
  gSystem->Load("libGraf3d.so");
  gSystem->Load("libGraf.so");
  gSystem->Load("libRootFftwWrapper.so");     	  
  gSystem->Load("libAnitaEvent.so");   	  
  gSystem->Load("libMagicDisplay.so");

  TChain *fred=0; //Will this work?

MagicDisplay *magicPtr = new MagicDisplay("http://157.132.95.98/antarctica14/root",run,WaveCalType::kVTLabAG);
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
  magicPtr->applyCut("header.getCoherentSumPeak()>1000");
  //  magicPtr->startControlPanel();
  //  magicPtr->startGpsDisplay();

}
