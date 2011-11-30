#include "JsonRootConverter.h"
#include <numeric>
#include <algorithm>
#include <functional>
#include "ProcessorFactory.h"
#include "TFile.h"

JsonRootConverter::JsonRootConverter(MsgStream::LEVEL loglevel):
  m_initialised(false),
  m_log("JsonRootConverter",loglevel),
  m_tree(new TTree("Data","Created by JsonRootConverter")){
    if(!m_tree){
      m_log << MsgStream::FATAL <<"TTree object didn't get created properly!"<<std::endl;
      exit(1);
    }
}

JsonRootConverter::~JsonRootConverter(){
  std::for_each(m_processors.begin(),m_processors.end(),DeleteObject());
  if(m_tree){
    delete m_tree;
    m_tree=0;
  }
}

bool JsonRootConverter::initialise(std::vector<std::string>& members){
  if(m_initialised) return m_initialised;
  m_processors.resize(members.size());
  std::transform(members.begin(),members.end(),m_processors.begin(),ProcessorFactory(m_log.getLogLevel(), m_tree));

  std::vector<IProcessor*>::iterator marker;
  marker = std::partition(m_processors.begin(),m_processors.end(),std::bind2nd(std::not_equal_to<IProcessor*>(),0));
  if(marker!=m_processors.end()){
    m_log << MsgStream::WARNING <<"Some of the requested trees had no associated processor defined in the ProcessorFactory."<<std::endl;
    m_log << MsgStream::WARNING <<"Will therefore not process or save these ROOT trees."<<std::endl;
  }

  m_initialised = true;
  return m_initialised;
}

bool JsonRootConverter::execute(std::string& eventDocument){
  if(!m_initialised){
    m_log<< MsgStream::ERROR <<"Cannot call execute before JsonRootConverter is intialised."<<std::endl;
    return false;
  }

  Json::Reader r;
  Json::Value rootNode = Json::Value::null;
  if(!r.parse(eventDocument,rootNode)){
       m_log<< MsgStream::ERROR <<"Problem parsing the Event Json document string."<<std::endl;
       return false;
  }

  std::vector<IProcessor*>::iterator marker;
  marker = std::partition(m_processors.begin(),m_processors.end(),std::bind2nd(std::not_equal_to<IProcessor*>(),0));

  bool result = std::accumulate(m_processors.begin(),marker,true,processorAccumulator(rootNode));

  if(!result){
    m_log<< MsgStream::FATAL <<"JsonRootConverter stopping in execute as one or more processors failed to process"<<std::endl;    
    exit(1);
  }
  m_tree->Fill();
  std::for_each(m_processors.begin(),marker,std::mem_fun(&IProcessor::clear));
  return result;
}

bool JsonRootConverter::write(const char* const name, const char* const mode){
  TFile f(name,mode);
  m_tree->Write();
  f.Close();
  return true;
}

