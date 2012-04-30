#ifndef IMAP_H
#define IMAP_H
#include "IModule.hh"

template <typename INPUT, typename OUTPUT>
class IMap : public virtual IModule {
public:
  virtual ~IMap() {}

public:
  virtual OUTPUT* process(INPUT*) const = 0;
};
#endif
