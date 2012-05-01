#include "ModuleBase.hh"
#include "API/APIExceptions.hh"
#include "Interface/Squeal.hh"
#include "Utils/CppErrorHandler.hh"
//#include <exception>
//#include "/home/hep/arichard/Maus/merged/root_io/src/legacy/Interface/Squeal.hh"
//class Squeal;

namespace MAUS {

  ModuleBase::ModuleBase(const std::string& s) : IModule(), _classname(s) {}
  ModuleBase::ModuleBase(const ModuleBase& mb) : IModule(), _classname(mb._classname) {}
  ModuleBase::~ModuleBase() {}
  
  void ModuleBase::birth(const std::string& s) {
    try {
      return _birth(s);
    }
    catch (Squeal& s) {
      CppErrorHandler::getInstance()->HandleSquealNoJson(s, _classname);
    }
    catch (std::exception & e) {
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    catch (...){
      throw UnhandledException(_classname);
    }
  }
  
  void ModuleBase::death() {
    try {
      return _death();
    }
    catch (Squeal& s) {
      CppErrorHandler::getInstance()->HandleSquealNoJson(s, _classname);
    }
    catch (std::exception & e) {
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    catch (...){
      throw UnhandledException(_classname);
    }
  }
  

}//end of namespace
