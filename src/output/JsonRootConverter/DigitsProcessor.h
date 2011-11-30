#ifndef DIGITS_PROCESSOR_H
#define DIGITS_PROCESSOR_H
#include "ProcessorBase.h"
#include "DigitsStructs.h"
#include "DigitsStructsDict.h"

class DigitsProcessor : public ProcessorBase{
 public:
  DigitsProcessor(MsgStream::LEVEL, TTree*);
  virtual bool processEvent(Json::Value&);
  virtual bool clear();
 private:
  Digits m_digits;
};


#endif
