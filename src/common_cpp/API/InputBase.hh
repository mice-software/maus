#ifndef INPUT_BASE_H
#define INPUT_BASE_H
#include <exception>
#include "IInput.hh"
#include "ModuleBase.hh"
#include "API/APIExceptions.hh"
#include "Interface/Squeal.hh"
#include "Utils/CppErrorHandler.hh"

//class Squeal;

namespace MAUS {

  template <typename T>
  class InputBase : public virtual IInput<T>, public ModuleBase {
    
  public:
    explicit InputBase(const std::string&);
    InputBase(const InputBase&);
    virtual ~InputBase();
    
  public:
    T* emitter();
    
  private:
    virtual T* _emitter() = 0;
  };
  
  template <typename T>
  InputBase<T>::InputBase(const std::string& s) : IInput<T>(), ModuleBase(s) {}
  
  template <typename T>
  InputBase<T>::InputBase(const InputBase& ib) : IInput<T>(), ModuleBase(ib._classname) {}
  
  template <typename T>
  InputBase<T>::~InputBase() {}
  
  template <typename T>
  T* InputBase<T>::emitter() {
    T* o = 0;
    try {
      o = _emitter();
    }
    catch (Squeal& s) {
      CppErrorHandler::getInstance()->HandleSquealNoJson(s, _classname);
    }
    catch (std::exception& e) {
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    catch (...){
      throw UnhandledException(_classname);
    }
    return o;
  }
  
}//end of namespace
#endif

