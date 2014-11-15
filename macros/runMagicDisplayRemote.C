gSystem->Reset();

void runMagicDisplayRemote(int run=165) ;

void runMagicDisplayRemote(int run)  {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  //  cout << gSystem->GetIncludePath() <<endl;
		
  gSystem->Load("libfftw3.so");
  gSystem->Load("libgsl.so");
  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libGeom.so");  
  gSystem->Load("libRootFftwWrapper.so");     	  
  gSystem->Load("libAnitaEvent.so");   	  
  gSystem->Load("libMagicDisplay.so");

  TChain *fred=0; //Will this work?

  //  MagicDisplay *magicPtr = new MagicDisplay("root://anita@192.168.2.2//data/anita/palestine08/webPlotter/events/root",run);
  //  MagicDisplay *magicPtr = new MagicDisplay("root://rjn@localhost:1094//unix/anita1/flight0809/root",run);
  MagicDisplay *magicPtr = new MagicDisplay("http://www.hep.ucl.ac.uk/~rjn/forMe/root",run);
  //  magicPtr->startSurfDisplay();
  //   magicPtr->startAvgSurfDisplay();
  //  magicPtr->startTurfDisplay();
  //  magicPtr->startSumTurfDisplay();
  magicPtr->startEventDisplay();
  //  magicPtr->startControlPanel();
  //  magicPtr->displayThisEvent(1057796,16);
  magicPtr->applyCut("triggerTimeNs>1e4 && triggerTimeNs<2e6");

}