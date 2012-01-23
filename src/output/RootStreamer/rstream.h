#ifndef RSTREAM_H
#define RSTREAM_H
#include <cstring>
#include <TFile.h>
#include <TTree.h>
#include "MsgStream/MsgStream.h"
#include <vector>

class rstream{
 public:
  rstream(const char*,
	  const char*,
	  MsgStream::LEVEL);
  virtual bool is_open(){ return !m_file? false :  m_file->IsOpen(); }
  virtual ~rstream();
  virtual void close()=0;
  friend  rstream& setBranch(rstream& rs,const char* name);
#ifdef TESTING
  friend class test_rstream;
#endif
 protected:
  TFile* m_file;
  MsgStream m_log;
  char* m_branchName;
  TTree* m_tree;
  std::vector<void*> m_pointers;
};

// Friend function definitions
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef EXSETBRANCH
#define EXSETBRANCH
rstream& setBranch(rstream& rs,const char* name){
  strcpy(rs.m_branchName,name);
  return rs;
}

#endif


// Includes such that user may use only #include "rstream.h"
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef EXINCL
#define EXINCL

#include "orstream.h"
#include "irstream.h"

#endif


#endif
