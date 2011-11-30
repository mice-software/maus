#include "ProcessorBase.h"
#include <cstdlib>

ProcessorBase::ProcessorBase(const char* codeFileName,
			     MsgStream::LEVEL loglevel,
			     TTree* tree
			     ):
  m_log("ProcessorBase",loglevel),
  m_tree(tree){   
    if(!m_tree){
      m_log << MsgStream::ERROR <<"TTree pointer didnt get copied properly!"<<std::endl;
      m_log << MsgStream::FATAL<<"Processor not initialised correctly"<<std::endl;
      exit(1);
    }
    m_branchName = new char[40]();
    strcpy(m_branchName,"");
    m_log.setName(codeFileName);
  }

ProcessorBase::~ProcessorBase(){
  delete m_branchName;
  m_branchName=0;
}

bool ProcessorBase::process(Json::Value& rootNode){
  std::string codeName = m_log.getName();
  m_log.setName("ProcessorBase");
  if(!rootNode.isObject()){
    m_log << MsgStream::ERROR << "The root object for an event should be an 'Object type'"<<std::endl;    
    return false;
  }

  if(!rootNode.isMember(m_branchName)){
    m_log << MsgStream::INFO << "Could not find the '"<< m_branchName <<"' object for this event, skipping..."<<std::endl;    
    return true;
  }

  Json::Value treeRootNode = rootNode.get(m_branchName,Json::Value::null);
  if(treeRootNode.isNull()){
    m_log << MsgStream::ERROR << "problem extracting object '"<< m_branchName <<"'"<<std::endl;
    return false;
  }
  m_log.setName(codeName);
  bool r = processEvent(treeRootNode);
  return r;
}
