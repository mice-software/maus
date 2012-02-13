#include "RStream.hh"

rstream::rstream(const char* filename,
		 const char* mode,
		 MsgStream::LEVEL loglevel):
  m_log("rstream", loglevel),
  m_tree(0),
  m_evtCount(0){
  if( !strcmp(filename,"") || !strcmp(mode,"")) {
    m_log << MsgStream::FATAL << "Couldn't open ROOT TFile as no filename or open mode given" << std::endl;
    throw 1;
  }

  m_file = new TFile(filename,mode);
  if(!m_file){
    m_log << MsgStream::FATAL << "ROOT TFile opened incorrectly" << std::endl;
    throw 2;
  }

 
  m_branchName = new char[40]();
  strcpy(m_branchName,"");
}

rstream::~rstream(){
  m_pointers.clear();
  if(m_branchName){
    delete [] m_branchName;
    m_branchName=0;
  }
  if (m_file){
    m_file->Close();
    delete m_file;
    m_file =0;
  }
  if(m_tree){
    //delete m_tree;
    m_tree=0;
  }
}

rstream& rstream::setBranch(rstream& rs,const char* name){
  strcpy(rs.m_branchName,name);
  return rs;
}
