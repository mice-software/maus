#ifndef MAP_BASE_H
#define MAP_BASE_H
#include "IMap.hh"
#include "ModuleBase.hh"
#include "API/APIExceptions.hh"
#include "Interface/Squeal.hh"
#include "Utils/CppErrorHandler.hh"
#include "Converter/ConverterFactory.hh"

namespace MAUS {

  template <typename INPUT, typename OUTPUT>
  class MapBase : public virtual IMap<INPUT, OUTPUT>, public ModuleBase {
    
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
  MapBase<INPUT, OUTPUT>::MapBase(const std::string& s) : IMap<INPUT, OUTPUT>(), ModuleBase(s) {std::cout<<"HERE2"<<std::endl;}
  
  template <typename INPUT, typename OUTPUT>
  MapBase<INPUT, OUTPUT>::MapBase(const MapBase& mb) : IMap<INPUT, OUTPUT>(), ModuleBase(mb._classname)  {}
  
  template <typename INPUT, typename OUTPUT>
  MapBase<INPUT, OUTPUT>::~MapBase() {}
  
  template<typename INPUT, typename OUTPUT>
  template<typename OTHER>
  OUTPUT* MapBase<INPUT, OUTPUT>::process(OTHER* o) const {
    
    ConverterFactory c;
    INPUT* tmp;
    OUTPUT* ret;
    try{
      tmp = c.convert<OTHER,INPUT>(o);
      ret =  process( tmp );
    }
    catch (std::exception& e) {
      if(tmp) { delete tmp; }
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    
    if((void*)tmp != (void*)o  && 
       (void*)tmp != (void*)ret){ // catch the pass through case
      delete tmp;
    }
    return ret;
  }
  
  template<typename INPUT, typename OUTPUT>
  OUTPUT* MapBase<INPUT, OUTPUT>::process(INPUT* i) const {
    if(!i){ throw NullInputException(_classname); }
    try {
      return _process(i);
    }
    catch (Squeal& s) {
      CppErrorHandler::getInstance()->HandleSqueal(i, s, _classname);
    }
    catch (std::exception& e) {
      CppErrorHandler::getInstance()->HandleStdExc(i, e, _classname);
    }
    catch (...){
      throw UnhandledException(_classname);
    }
  }

}//end of namespace
#endif

