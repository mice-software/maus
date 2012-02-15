#include "ORStream.hh"
#include "Interface/Squeal.hh"

orstream::orstream(const char* fileName,
		   const char* treeName,
		   const char* treeTitle,
		   const char* mode): 
  rstream(fileName, mode){
  m_tree = new TTree(treeName,treeTitle);
}


void orstream::open(const char* fileName,
		    const char* treeName,
		    const char* treeTitle,
		    const char* mode){

  if( !strcmp(fileName,"") ) {
    Squeak::mout(Squeak::error) << "Couldn't open ROOT TFile as no filename given" << std::endl;
    throw Squeal(Squeal::recoverable,
		 "Cannot open file as null \"\" string passed as filename",
		 "void orstream::open(const char*, const char*, const char*, const char*)");
  }

  m_file = new TFile(fileName,mode);
  if(!m_file){
    Squeak::mout(Squeak::error) << "ROOT TFile opened incorrectly" << std::endl;
    throw Squeal(Squeal::recoverable,
		 "TFile object not opened properly",
		 "void orstream::open(const char*, const char*, const char*, const char*)");
  }

  m_tree = new TTree(treeName,treeTitle);
  strcpy(m_branchName,"");
  m_evtCount=0;
}


void orstream::close(){
  
  m_file->cd();
  m_tree->Write();
  m_file->Close();
  Squeak::mout(Squeak::info) << "Written "<<m_evtCount << " event(s) to file." << std::endl;
  strcpy(m_branchName,"");
  
  if(m_file){
      delete m_file;
      m_file=0;
  }
  if(m_tree){
    //delete m_tree; //ROOT does this automatically on closing file.
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


