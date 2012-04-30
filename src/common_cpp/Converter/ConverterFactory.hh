#ifndef CONVERTER_FACTORY_H
#define CONVERTER_FACTORY_H
#include "Converter/IConverter.hh"
#include <exception>
#include "Converter/ConverterBase.hh"
#include <iostream>

class myDoubleIntConverter : public ConverterBase<double,int>{
private:
  int* _convert(const double* d) const{
    return new int(*d);
  }
};


class ConverterNotFoundException: public std::exception {
public:
  virtual const char* what() const throw() {
    return "The required converter was not found";
  }
};



class ConverterFactory{
public:
  template <typename INPUT, typename OUTPUT> 
  IConverter<INPUT, OUTPUT>* getConverter() const throw (ConverterNotFoundException);

  template <typename INPUT, typename OUTPUT>
  OUTPUT* convert(INPUT*) const;
};

template <typename INPUT, typename OUTPUT>
OUTPUT* ConverterFactory::convert(INPUT* i) const {
  if(!i){ return 0; }
  IConverter<INPUT, OUTPUT>* c = 0;
  try{
    c = getConverter<INPUT,OUTPUT>();
  }
  catch (Squeal& s){
    if(c){ delete c; }
    return 0;
  }
  catch (std::exception & e) {
    if(c){ delete c; }
    std::cout<<"No Converter found!"<<std::endl;
    return 0;
  }
  catch (...){
    if(c){ delete c; }
    return 0;
  }

  OUTPUT* o = c->convert(i);
  delete c;

  return o;
}




template <typename INPUT, typename OUTPUT>
IConverter<INPUT, OUTPUT>* ConverterFactory::getConverter() const throw (ConverterNotFoundException){
  throw ConverterNotFoundException();
}
// template <>
// IConverter<double,int>* ConverterFactory::getConverter<double, int>() const throw (ConverterNotFoundException){
//   return new myDoubleIntConverter();
// }
#endif
