
void runMagicDisplayUCL(int run) {
  gROOT->ProcessLine(".x loadAnitaLibs.C");
  char rootLine[180];
  sprintf(rootLine,".x actuallyRunMagicDisplayUCL.C(%d)",run);
  gROOT->ProcessLine(rootLine);

  
}
