#include "IRStream.hh"
//#include <cstdlib>

irstream::irstream(const char* fileName,
		   const char* treeName,
		   const char* mode,
		   MsgStream::LEVEL loglevel):
  rstream(fileName,mode, loglevel){
 
  m_log.setName("irstream");
  m_tree = (TTree*)m_file->GetObjectChecked(treeName,"TTree");
  if(!m_tree){ 
    m_log << MsgStream::FATAL << "The requested tree '"<<treeName << "' was not found in the tree." <<std::endl;
    throw 3;
    //exit(1); // added later.
  }
}


void irstream::open(const char* fileName,
		    const char* treeName,
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
  
  m_tree = (TTree*)m_file->GetObjectChecked(treeName,"TTree");
  if(!m_tree){ 
    m_log << MsgStream::FATAL << "The requested tree '"<<treeName << "' was not found in the tree." <<std::endl;
    throw 3;
    //exit(1); // added later.
  }

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


irstream* readEvent(irstream& irs){
  long nextEvent = irs.m_tree->GetReadEntry() + 1;
  // Original refactored now can return 0 and also want event count
/*   if (nextEvent < irs.m_tree->GetEntries()){ */
/*     irs.m_tree->GetEntry(nextEvent); */
/*   } */
/*   else{ */
/*     irs.m_log << MsgStream::WARNING  << "End of file reached, cannot extract any more." <<std::endl; */
/*     return 0; //added as part of above change. */
/*   } */
/*   return &irs; */



  if(nextEvent >= irs.m_tree->GetEntries()){
    //irs.m_log << MsgStream::WARNING  << "End of file reached, cannot extract any more." <<std::endl;
    irs.m_log << MsgStream::INFO  << "Read "<<irs.m_evtCount << " event(s) from file."  <<std::endl;
    return 0; //added as part of above change.
  }
  irs.m_tree->GetEntry(nextEvent);
  ++irs.m_evtCount;

  return &irs;
}
