#include "RStream.hh"
#include "Interface/Squeal.hh"

rstream::rstream(const char* filename, const char* mode):
  m_tree(0),
  m_evtCount(0){
  if( !strcmp(filename,"") || !strcmp(mode,"")) {
    Squeak::mout(Squeak::fatal) << "Couldn't open ROOT TFile as no filename or open mode given" << std::endl;
    throw Squeal(Squeal::nonRecoverable,
		 "rstream object not correctly initialised as null \"\" string passed as filename or open mode.",
		 "rstream::rstream(const char*, const char*)");
  }

  m_file = new TFile(filename,mode);
  if(!m_file){
    Squeak::mout(Squeak::fatal) << "ROOT TFile opened incorrectly" << std::endl;
    throw Squeal(Squeal::nonRecoverable,
		 "rstream object not correctly initialised as TFile not opened properly",
		 "rstream::rstream(const char*, const char*)");
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
    //delete m_tree; //ROOT does this automatically on closing file.
    m_tree=0;
  }
}

rstream& rstream::setBranch(rstream& rs,const char* name){
  strcpy(rs.m_branchName,name);
  return rs;
}
