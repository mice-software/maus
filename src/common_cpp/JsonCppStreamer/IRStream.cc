#include "IRStream.hh"
#include "Interface/Squeal.hh"

irstream::irstream(const char* fileName,
		   const char* treeName,
		   const char* mode):
  rstream(fileName,mode){
 
  m_tree = (TTree*)m_file->GetObjectChecked(treeName,"TTree");
  if(!m_tree){ 
    Squeak::mout(Squeak::fatal) << "The requested tree '"<<treeName << "' was not found in the tree." <<std::endl;
    throw Squeal(Squeal::nonRecoverable,
		 "irstream object not correctly initialised as couldn;t find requested TTree.",
		 "irstream::irstream(const char*, const char*, const char*)");
  }
}


void irstream::open(const char* fileName,
		    const char* treeName,
		    const char* mode){
  if( !strcmp(fileName,"") ) {
    Squeak::mout(Squeak::error) << "Couldn't open ROOT TFile as no filename given" << std::endl;
    throw Squeal(Squeal::recoverable,
		 "Cannot open file as null \"\" string passed as filename",
		 "void irstream::open(const char*, const char*, const char*)");
  } 

  m_file = new TFile(fileName,mode);
  if(!m_file){
    Squeak::mout(Squeak::error) << "ROOT TFile opened incorrectly" << std::endl;
    throw Squeal(Squeal::recoverable,
		 "TFile object not opened properly",
		 "void irstream::open(const char*, const char*, const char*)");
  }
  
  m_tree = (TTree*)m_file->GetObjectChecked(treeName,"TTree");
  if(!m_tree){ 
    Squeak::mout(Squeak::error) << "The requested tree '"<<treeName << "' was not found in the tree." <<std::endl;
    throw Squeal(Squeal::recoverable,
		 "Could not find requested TTree.",
		 "void irstream::open(const char*, const char*, const char*)");
  }

  strcpy(m_branchName,"");
  m_evtCount=0;
}

void irstream::close(){
  m_file->Close();
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

irstream* irstream::operator>>(irstream* (*manip_pointer)(irstream&)){
  return manip_pointer(*this);
}


irstream* readEvent(irstream& irs){
  long nextEvent = irs.m_tree->GetReadEntry() + 1;

  if(nextEvent >= irs.m_tree->GetEntries()){
    Squeak::mout(Squeak::info) << "Read "<<irs.m_evtCount << " event(s) from file."  <<std::endl;
    return 0;
  }
  irs.m_tree->GetEntry(nextEvent);
  ++irs.m_evtCount;

  return &irs;
}
