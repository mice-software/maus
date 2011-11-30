#include "ProcessorFactory.h"
#include "DigitsProcessor.h"

ProcessorFactory::ProcessorFactory(MsgStream::LEVEL loglevel, TTree* tree):
  m_log("ProcessorFactory",loglevel),
  m_tree(tree){
    m_processor_map["digits"] = &ProcessorFactory::create<DigitsProcessor>;
}

IProcessor* ProcessorFactory::operator()(std::string& name){

    std::map<std::string, processor_pointer>::const_iterator mItr;
    mItr = m_processor_map.find(name);
    if(mItr!=m_processor_map.end()){
      return (this->*(mItr->second))();
    }
    m_log << MsgStream::WARNING <<"Didn't find the requested processor for object '"<<name <<"'"<<std::endl;
    m_log << MsgStream::WARNING <<"Returning null pointer!!!"<<std::endl;
    return 0;
}

