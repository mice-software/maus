#ifndef REDUCE_BASE_H
#define REDUCE_BASE_H
#include "IReduce.hh"
#include "ModuleBase.hh"
#include "API/APIExceptions.hh"
#include "Converter/ConverterFactory.hh"
#include "Interface/Squeal.hh"
#include "Utils/CppErrorHandler.hh"

//class Squeal;

namespace MAUS {

  template <typename T>
  class ReduceBase : public ModuleBase, public virtual IReduce<T> {
    
  public:
    explicit ReduceBase(const std::string&);
    ReduceBase(const ReduceBase&);
    virtual ~ReduceBase();
    
  public:
    T* process(T* t);
    
  private:
    virtual T* _process(T*) = 0;
  };
  
  template <typename T>
  ReduceBase<T>::ReduceBase(const std::string& s) : ModuleBase(s), IReduce<T>() {}
  
  template <typename T>
  ReduceBase<T>::ReduceBase(const ReduceBase& rb) : ModuleBase(rb._classname), IReduce<T>() {}
  
  template <typename T>
  ReduceBase<T>::~ReduceBase() {}
  
  template <typename T>
  T* ReduceBase<T>::process(T* t) {
    if(!t){ throw NullInputException(_classname); }
    try {
      return _process(t);
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
  }
  
}//end of namespace
#endif

