#ifndef PROCESSOR_BASE_H
#define PROCESSOR_BASE_H
#include <TTree.h>
#include "json/json.h"
#include "MsgStream.h"
#include "IProcessor.h"

class ProcessorBase : public IProcessor {
 public:
  ProcessorBase(const char*,
		MsgStream::LEVEL, 
		TTree*);
  virtual ~ProcessorBase();
  virtual bool clear() =0;
  template<typename T>
    void initialise(const char* branchName, T& data){
    m_tree->Branch(branchName,&data);
    strcpy(m_branchName,branchName);
  }
  virtual bool process(Json::Value&);
  virtual bool processEvent(Json::Value&) =0;

 protected:
  MsgStream m_log;
  char* m_branchName; 
  TTree * m_tree;
};


#endif
