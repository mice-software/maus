#include "ORStream.hh"

orstream::orstream(const char* fileName,
		   const char* treeName,
		   const char* treeTitle,
		   const char* mode,
		   MsgStream::LEVEL loglevel): 
  rstream(fileName, mode, loglevel){
  m_log.setName("orstream");
  m_tree = new TTree(treeName,treeTitle);
}


void orstream::open(const char* fileName,
		    const char* treeName,
		    const char* treeTitle,
		    const char* mode){

  if( !strcmp(fileName,"") ) {
    m_log << MsgStream::FATAL << "Couldn't open ROOT TFile as no filename given" << std::endl;
    throw 1;
  }

  m_file = new TFile(fileName,mode);
  if(!m_file){
    m_log << MsgStream::FATAL << "ROOT TFile opened incorrectly" << std::endl;
    throw 2;
  }

  m_tree = new TTree(treeName,treeTitle);
  strcpy(m_branchName,"");
  m_evtCount=0;
}


void orstream::close(){
  
  m_file->cd();
  m_tree->Write();
  m_file->Close();
  m_log << MsgStream::INFO << "Written "<<m_evtCount << " event(s) to file." << std::endl;
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

orstream& orstream::operator<<(orstream& (*manip_pointer)(orstream&)){
  return manip_pointer(*this);
}

orstream& fillEvent(orstream& ors) {
  ors.m_tree->Fill();
  ++ors.m_evtCount;
  return ors;
}


