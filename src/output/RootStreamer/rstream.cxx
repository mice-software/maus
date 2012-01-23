#include "rstream.h"


rstream::rstream(const char* filename,
		 const char* mode,
		 MsgStream::LEVEL loglevel):
  m_file(new TFile(filename,mode)),
  m_log("rstream", loglevel),
  m_tree(0){
 
  m_branchName = new char[40]();
  strcpy(m_branchName,"");
}

rstream::~rstream(){
  m_pointers.clear();
  if(m_branchName){
    delete m_branchName;
    m_branchName=0;
  }
  if (m_file){
    m_file->Close();
    delete m_file;
    m_file =0;
  }
  if(m_tree){
    delete m_tree;
    m_tree=0;
  }
}
