

void loadAnitaLibs() {
  const char* anitaUtilInstallDir = getenv("ANITA_UTIL_INSTALL_DIR");
  TString incDir = TString::Format("%s/include", anitaUtilInstallDir);
  gInterpreter->AddIncludePath(incDir.Data());

  //gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  gSystem->Load("libRootFftwWrapper.3.dylib");  
  gSystem->Load("libAnitaEvent");  
  gSystem->Load("libAnitaAnalysisTools");
  gSystem->Load("libAnitaMagicDisplay");
}
