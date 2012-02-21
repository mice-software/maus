void rootlogon(){
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/Spill.cc+");
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/DAQData.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/EMRSpillData.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/Scalars.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/MCEvent.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/Primary.cc+");

  gROOT->ProcessLine(".L src/common_cpp/DataStructure/ReconEvent.cc+");  
  TMethodBrowsable::Unregister();
}
