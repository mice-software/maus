#ifndef IPROCESSOR_H
#define IPROCESSOR_H
#include "json/json.h"

  class IProcessor{
  public:
    virtual bool process(Json::Value&) =0;
    virtual bool clear() =0;
  };

#endif
