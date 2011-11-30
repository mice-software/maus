#ifndef PROCESSOR_FACTORY_H
#define PROCESSOR_FACTORY_H
#include <map>
#include <string>
#include "IProcessor.h"
#include "MsgStream.h"
#include <TTree.h>

class ProcessorFactory{
 public:
  ProcessorFactory(MsgStream::LEVEL, TTree*);

  IProcessor* operator()(std::string&);

 private:
  typedef IProcessor* (ProcessorFactory::*processor_pointer)();

  template<typename T> IProcessor* create(){
    
    return new T(m_log.getLogLevel(),m_tree);
  }

  MsgStream m_log;
  TTree * m_tree;
  std::map<std::string, processor_pointer> m_processor_map;  
};

#endif
