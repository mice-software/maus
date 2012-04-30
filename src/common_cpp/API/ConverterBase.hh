#ifndef CONVERTER_BASE_H
#define CONVERTER_BASE_H
#include "IConverter.hh"
#include "/home/hep/arichard/Maus/merged/root_io/src/legacy/Interface/Squeal.hh"
template <typename INPUT, typename OUTPUT>
class ConverterBase : public IConverter<INPUT, OUTPUT> {
public:
  virtual ~ConverterBase();

public:
  OUTPUT* operator()(const INPUT*) const;
  OUTPUT* convert   (const INPUT*) const;

private:
  virtual OUTPUT* _convert(const INPUT*) const = 0;
};

template <typename INPUT, typename OUTPUT>
ConverterBase<INPUT, OUTPUT>::~ConverterBase() {}

template <typename INPUT, typename OUTPUT>
OUTPUT* ConverterBase<INPUT, OUTPUT>::operator()(const INPUT* i) const {
  return convert(i);
}

template <typename INPUT, typename OUTPUT>
OUTPUT* ConverterBase<INPUT, OUTPUT>::convert(const INPUT* i) const {
  try {
    return _convert(i);
  }
  catch (Squeal& s) {
    
  }
  catch (std::exception & e) {

  }
  catch (...){
    
  }
}


#endif
