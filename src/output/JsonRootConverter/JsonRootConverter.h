#ifndef JSON_ROOT_CONVERTER_H
#define JSON_ROOT_CONVERTER_H
#include <vector>
#include "TTree.h"
#include "json/json.h"
#include "IProcessor.h"
#include "MsgStream.h"

class JsonRootConverter{
 public:
  JsonRootConverter(MsgStream::LEVEL =MsgStream::INFO);
  ~JsonRootConverter();
  bool initialise(std::vector<std::string>&);
  bool execute(std::string&);
  bool write(const char* const, const char* const = "RECREATE");
 private:
  bool m_initialised;
  MsgStream m_log;
  TTree * m_tree;
  std::vector<IProcessor*> m_processors;
};

class DeleteObject{
 public:
  template<typename T> void operator()(T* data){
    delete data;
    data=0;
    return;
  }

};

class processorAccumulator {
 public:
  processorAccumulator(Json::Value& rootNode):m_rootNode(rootNode){}
  bool operator()(bool init, IProcessor* p){
    return init && p->process(m_rootNode);
  }
 private:
  Json::Value& m_rootNode;
};

#endif
