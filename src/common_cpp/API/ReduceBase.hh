#ifndef REDUCE_BASE_H
#define REDUCE_BASE_H
#include <exception>
#include "IReduce.hh"
#include "ModuleBase.hh"

class Squeal;

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
  try {
    return _process(t);
  }
  catch (Squeal& s) {
    
  }
  catch (std::exception & e) {

  }
  catch (...){
    
  }
}



#endif

