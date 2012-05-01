#ifndef IMAP_H
#define IMAP_H
#include "IModule.hh"

namespace MAUS {

  template <typename INPUT, typename OUTPUT>
  class IMap : public virtual IModule {
  public:
    virtual ~IMap() {}
    
  public:
    virtual OUTPUT* process(INPUT*) const = 0;
  };
  
}//end of namespace
#endif
