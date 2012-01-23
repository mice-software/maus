void rootlogon(){
  
  gROOT->ProcessLine(".L DigitsStructs.cxx+");
  gROOT->ProcessLine(".L MCStructs.cxx+");
  gROOT->ProcessLine(".L MausEventStruct.cxx+");
  TMethodBrowsable::Unregister();
}
