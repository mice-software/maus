#ifndef ICONVERTER_H
#define ICONVERTER_H

namespace MAUS{

  template <typename INPUT, typename OUTPUT>
  class IConverter {
  public:
    virtual ~IConverter() {}
    
  public:
    virtual OUTPUT* operator()(const INPUT* ) const = 0;
    virtual INPUT*  operator()(const OUTPUT*) const = 0;
    virtual OUTPUT* convert   (const INPUT* ) const = 0;
    virtual INPUT*  convert   (const OUTPUT*) const = 0;
  };
  
} //end of namespace
#endif
