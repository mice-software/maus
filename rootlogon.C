{
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  
  
  string G4SYSTEM = getenv("G4SYSTEM");
  string libname = string("libRATEvent_") + G4SYSTEM;
  //  gSystem->Load(libname.c_str());
    //  gSystem->AddIncludePath(" -I$RATROOT/include");
}

