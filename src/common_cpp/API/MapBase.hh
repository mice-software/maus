#ifndef MAP_BASE_H
#define MAP_BASE_H
#include "IMap.hh"
#include "ModuleBase.hh"
#include "API/APIExceptions.hh"
#include "Interface/Squeal.hh"
#include "Utils/CppErrorHandler.hh"
#include "Converter/ConverterFactory.hh"
#include "json/json.h"

namespace MAUS {

  template <typename INPUT, typename OUTPUT>
  class MapBase : public virtual IMap<INPUT, OUTPUT>, public ModuleBase {
    
  public:
    explicit MapBase(const std::string&);
    MapBase(const MapBase&);
    virtual ~MapBase();
    
  public:
    OUTPUT* process(INPUT*) const;
    template <typename OTHER> OUTPUT* process(OTHER*) const;

  private:
    virtual OUTPUT* _process(INPUT*) const = 0;
  };
  template <typename OUTPUT>
  class MapBase<Json::Value, OUTPUT> : public virtual IMap<Json::Value, OUTPUT>, public ModuleBase {
    
  public:
    explicit MapBase(const std::string&);
    MapBase(const MapBase&);
    virtual ~MapBase();
    
  public:
    OUTPUT* process(Json::Value*) const;
    template <typename OTHER> OUTPUT* process(OTHER*) const;
    
  private:
    virtual OUTPUT* _process(Json::Value*) const = 0;
  };
  

  template <typename INPUT, typename OUTPUT>
  MapBase<INPUT, OUTPUT>::MapBase(const std::string& s) : IMap<INPUT, OUTPUT>(), ModuleBase(s) {}
  template <typename OUTPUT>
  MapBase<Json::Value, OUTPUT>::MapBase(const std::string& s) : IMap<Json::Value, OUTPUT>(), ModuleBase(s) {}
  
  template <typename INPUT, typename OUTPUT>
  MapBase<INPUT, OUTPUT>::MapBase(const MapBase& mb) : IMap<INPUT, OUTPUT>(), ModuleBase(mb._classname)  {}
  template <typename OUTPUT>
  MapBase<Json::Value, OUTPUT>::MapBase(const MapBase& mb) : IMap<Json::Value, OUTPUT>(), ModuleBase(mb._classname)  {}
  
  template <typename INPUT, typename OUTPUT>
  MapBase<INPUT, OUTPUT>::~MapBase() {}
  template <typename OUTPUT>
  MapBase<Json::Value, OUTPUT>::~MapBase() {}
  
  template<typename INPUT, typename OUTPUT>
  OUTPUT* MapBase<INPUT, OUTPUT>::process(INPUT* i) const {
    if(!i){ throw NullInputException(_classname); }
    OUTPUT* o = 0;
    try {
      o =  _process(i);
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
  template<typename OUTPUT>
  OUTPUT* MapBase<Json::Value, OUTPUT>::process(Json::Value* i) const {
    if(!i){ throw NullInputException(_classname); }
    OUTPUT* o = 0;
    try {
      o = _process(i);
    }
    catch (Squeal& s) {
      CppErrorHandler::getInstance()->HandleSqueal(*i, s, _classname);
    }
    catch (std::exception& e) {
      CppErrorHandler::getInstance()->HandleStdExc(*i, e, _classname);
    }
    catch (...){
      throw UnhandledException(_classname);
    }
    return o;
  }

  template<typename INPUT, typename OUTPUT>
  template<typename OTHER>
  OUTPUT* MapBase<INPUT, OUTPUT>::process(OTHER* o) const {
    
    ConverterFactory c;
    INPUT* tmp = 0;
    OUTPUT* ret = 0;
    try{
      tmp = c.convert<OTHER,INPUT>(o);
      ret =  process( tmp );
    }
    catch (std::exception& e) {
      if(tmp) { delete tmp; }
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
      return ret;
    }
    
    if((void*)tmp != (void*)o  && 
       (void*)tmp != (void*)ret){ // catch the pass through case
      delete tmp;
    }
    return ret;
  }
  template<typename OUTPUT>
  template<typename OTHER>
  OUTPUT* MapBase<Json::Value, OUTPUT>::process(OTHER* o) const {
    
    ConverterFactory c;
    Json::Value* tmp;
    OUTPUT* ret;
    try{
      tmp = c.convert<OTHER,Json::Value>(o);
      ret =  process( tmp );
    }
    catch (std::exception& e) {
      if(tmp) { delete tmp; }
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
      return ret;
    }
    
    if((void*)tmp != (void*)o  && 
       (void*)tmp != (void*)ret){ // catch the pass through case
      delete tmp;
    }
    return ret;
  }
 
}//end of namespace
#endif

