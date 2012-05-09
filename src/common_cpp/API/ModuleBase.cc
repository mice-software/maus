#include "ModuleBase.hh"
#include "API/APIExceptions.hh"
#include "Interface/Squeal.hh"
#include "Utils/CppErrorHandler.hh"

namespace MAUS {

  ModuleBase::ModuleBase(const std::string& s) : IModule(), _classname(s) {std::cout<<"HERE1"<<std::endl;}
  ModuleBase::ModuleBase(const ModuleBase& mb) : IModule(), _classname(mb._classname) {}
  ModuleBase::~ModuleBase() {}
  
  void ModuleBase::birth(const std::string& s) {
    try {
      _birth(s);
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
      _death();
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
