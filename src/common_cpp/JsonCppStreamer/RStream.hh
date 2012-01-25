#ifndef RSTREAM_H
#define RSTREAM_H 1

#include <cstring>
#include <TFile.h>
#include <TTree.h>
#include "MsgStream.hh"
#include <vector>

class rstream{
 public:
  rstream(const char*,
	  const char*,
	  MsgStream::LEVEL);
  virtual bool is_open(){ return !m_file? false :  m_file->IsOpen(); }
  virtual ~rstream();
  virtual void close()=0;
  static rstream& setBranch(rstream& rs,const char* name);

  TFile* getTFile() {return m_file;}
  TTree* getTTree() {return m_tree;}

 protected:
  TFile* m_file;
  MsgStream m_log;
  char* m_branchName;
  TTree* m_tree;
  std::vector<void*> m_pointers;
};


#endif

