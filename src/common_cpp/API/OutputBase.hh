#ifndef OUTPUT_BASE_H
#define OUTPUT_BASE_H
#include <exception>
#include "IOutput.hh"
#include "ModuleBase.hh"

class Squeal;

template <typename T>
class OutputBase : public ModuleBase, public virtual IOutput<T> {

public:
  explicit OuputBase(const std::string&);
  OutputBase(const OutputBase&);
  virtual ~OutputBase();

public:
  bool save(T*);

private:
  virtual bool _save(T*) = 0;
};

template <typename T>
OutputBase<T>::OutputBase(const std::string& s) : ModuleBase(s), IOutput<T>() {}

template <typename T>
OutputBase<T>::OutputBase(const OutputBase& ob) : ModuleBase(ob._classname), IOutput<T>() {}

template <typename T>
OutputBase<T>::~OutputBase() {}

template <typename T>
bool OutputBase<T>::save (T* t) {
  try {
    return _save(t);
  }
  catch (Squeal& s) {
    
  }
  catch (std::exception & e) {

  }
  catch (...){
    
  }
}


#endif
