void rootlogon(){
  
  gROOT->ProcessLine(".L DigitsStructs.cc+");
  gROOT->ProcessLine(".L MCStructs.cc+");
  gROOT->ProcessLine(".L MausEventStruct.cc+");
  TMethodBrowsable::Unregister();
}
