

void runMagicDisplay() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/includes");
  //  cout << gSystem->GetIncludePath() <<endl;
		
  gSystem->Load("/sw/lib/libfftw3.so");

  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libRootFftwWrapper.so");     	  
  gSystem->Load("libAnitaEvent.so");   	  
  gSystem->Load("libMagicDisplay.so");

  TChain *fred=0; //Will this work?

  MagicDisplay *magicPtr = new MagicDisplay("/home/rjn/anita/data/testing",2254);

}
