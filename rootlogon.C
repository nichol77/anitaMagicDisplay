{


  gSystem->Load("${ANITA_UTIL_INSTALL_DIR}/lib/libRootFftwWrapper.so") && gSystem->Load("$ANITA_UTIL_INSTALL_DIR/lib/libRootFftwWrapper.dylib");
  gSystem->Load("${ANITA_UTIL_INSTALL_DIR}/lib/libAnitaEvent.so") && gSystem->Load("$ANITA_UTIL_INSTALL_DIR/lib/libAnitaEvent.dylib"); 
  gSystem->Load("${ANITA_UTIL_INSTALL_DIR}/lib/libAnitaCorrelator.so") && gSystem->Load("$ANITA_UTIL_INSTALL_DIR/lib/libAnitaCorrelator.dylib"); 
  gSystem->Load("${ANITA_UTIL_INSTALL_DIR}/lib/libAnitaAnalysis.so") && gSystem->Load("$ANITA_UTIL_INSTALL_DIR/lib/libAnitaAnalysis.dylib"); 
  gSystem->Load("${ANITA_UTIL_INSTALL_DIR}/lib/libAnitaAnalysisTools.so") && gSystem->Load("$ANITA_UTIL_INSTALL_DIR/lib/libAnitaAnalysisTools.dylib"); 
  gSystem->Load("${ANITA_UTIL_INSTALL_DIR}/lib/libUCorrelator.so") && gSystem->Load("$ANITA_UTIL_INSTALL_DIR/lib/libUCorrelator.dylib"); 
  gSystem->Load("${ANITA_UTIL_INSTALL_DIR}/lib/libAnitaMagicDisplay.so") && gSystem->Load("$ANITA_UTIL_INSTALL_DIR/lib/libAnitaMagicDisplay.dylib"); 

  gStyle->SetNumberContours(255);
  gStyle->SetPalette(kTemperatureMap);
}
