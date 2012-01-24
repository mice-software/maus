#include "IRStream.hh"
#include <cstdlib>

irstream::irstream(const char* fileName,
		   const char* treeName,
		   const char* mode,
		   MsgStream::LEVEL loglevel):
  rstream(fileName,mode, loglevel),
 m_evtCount(0){// added later.
 
  m_log.setName("irstream");
  m_tree = (TTree*)m_file->GetObjectChecked(treeName,"TTree");
  if(!m_tree){ 
    m_log << MsgStream::ERROR << "The requested tree '"<<treeName << "' was not found in the tree." <<std::endl;
    exit(1); // added later.
  }
}


void irstream::open(const char* fileName,
		    const char* treeName,
		    const char* mode){
  m_file = new TFile(fileName,mode);
  m_tree = (TTree*)m_file->GetObjectChecked(treeName,"TTree");
  strcpy(m_branchName,"");
  m_evtCount=0;// added later.
}

void irstream::close(){
  m_file->Close();
  strcpy(m_branchName,"");
  if(m_file){
    delete m_file;
    m_file=0;
  }
  if(m_tree){
    //delete m_tree; //crashes root?! ROOT must do this step automatically
    m_tree=0;
  }
}

// Note changed this to a pointer so can return 0 to get
//  while(irstream>>) functionality. This was done afterwards so may
//  break things
//irstream& irstream::operator>>(irstream& (*manip_pointer)(irstream&)){
irstream* irstream::operator>>(irstream* (*manip_pointer)(irstream&)){
  return manip_pointer(*this);
}
