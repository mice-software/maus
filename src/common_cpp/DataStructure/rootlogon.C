void rootlogon(){
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/DAQData.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/EMRSpillData.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/Scalars.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/MCEvent.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/MCEventArray.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/ReconEvent.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/Spill.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/MCStructs.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/MausEventStruct.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/MausEventStructRogers.cc+");  
  gROOT->ProcessLine(".L src/common_cpp/DataStructure/DigitsStructs.cc+");  
  TMethodBrowsable::Unregister();
}
