#ifndef MAP_BASE_H
#define MAP_BASE_H
#include <exception>
#include "IMap.hh"
#include "ModuleBase.hh"
#include "/home/hep/arichard/Maus/merged/root_io/src/legacy/Interface/Squeal.hh"
//#include "IConverter.hh"
#include "ConverterFactory.hh"
//#include <iostream>
//class Squeal;

template <typename INPUT, typename OUTPUT>
class MapBase : public ModuleBase, public virtual IMap<INPUT, OUTPUT> {

public:
  explicit MapBase(const std::string&);
  MapBase(const MapBase&);
  virtual ~MapBase();

public:
  template <typename OTHER> OUTPUT* process(OTHER*) const;
  OUTPUT* process(INPUT*) const;

private:
  virtual OUTPUT* _process(INPUT*) const = 0;
};


template <typename INPUT, typename OUTPUT>
MapBase<INPUT, OUTPUT>::MapBase(const std::string& s) : ModuleBase(s), IMap<INPUT, OUTPUT>() {}

template <typename INPUT, typename OUTPUT>
MapBase<INPUT, OUTPUT>::MapBase(const MapBase& mb) : ModuleBase(mb._classname), IMap<INPUT, OUTPUT>() {}

template <typename INPUT, typename OUTPUT>
MapBase<INPUT, OUTPUT>::~MapBase() {}

template<typename INPUT, typename OUTPUT>
template<typename OTHER>
OUTPUT* MapBase<INPUT, OUTPUT>::process(OTHER* o) const {
  
  ConverterFactory c;
  INPUT* tmp = c.convert<OTHER,INPUT>(o);
  OUTPUT* ret =  process( tmp );
  if((void*)tmp != (void*)o  && 
     (void*)tmp != (void*)ret){ // catch the pass through case
    delete tmp;
  }
  return ret;
}

template<typename INPUT, typename OUTPUT>
OUTPUT* MapBase<INPUT, OUTPUT>::process(INPUT* i) const {
  if(!i){return 0; }
  try {
    return _process(i);

  }
  catch (Squeal& s) {
    
  }
  catch (std::exception & e) {

  }
  catch (...){
    
  }
}


#endif

