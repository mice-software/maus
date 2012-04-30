#include "ModuleBase.hh"
#include <exception>
#include "/home/hep/arichard/Maus/merged/root_io/src/legacy/Interface/Squeal.hh"
//class Squeal;

ModuleBase::ModuleBase(const std::string& s) : IModule(), _classname(s) {}
ModuleBase::ModuleBase(const ModuleBase& mb) : IModule(), _classname(mb._classname) {}
ModuleBase::~ModuleBase() {}

void ModuleBase::birth(const std::string& s) {
  try {
    return _birth(s);
  }
  catch (Squeal& s) {
    
  }
  catch (std::exception & e) {
    
  }
  catch (...){
    
  }
}

void ModuleBase::death() {
  try {
    return _death();
  }
  catch (Squeal& s) {
    
  }
  catch (std::exception & e) {
    
  }
  catch (...){
    
  }
}
