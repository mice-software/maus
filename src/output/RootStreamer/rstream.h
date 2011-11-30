#ifndef RSTREAM_H
#define RSTREAM_H
#include <cstring>
#include <TFile.h>
#include <TTree.h>
#include "MsgStream.h"
#include <vector>
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

class rstream{
 public:
  rstream(const char* filename,
	  const char* mode,
	  MsgStream::LEVEL loglevel):
    m_file(new TFile(filename,mode)),
    m_log("rstream", loglevel){
      m_branchName = new char[40]();
      strcpy(m_branchName,"");
  }
  virtual bool is_open(){ return !m_file? false :  m_file->IsOpen(); }
  virtual ~rstream(){
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
  virtual void close()=0;
  friend rstream& setBranch(rstream& rs,const char* name);
 protected:
  TFile* m_file;
  MsgStream m_log;
  char* m_branchName;
  TTree* m_tree;
  std::vector<void*> m_pointers;
};

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

template<typename T>
class oneArgManip{
 private:
    typedef rstream& (*manip_pointer)(rstream&,T);
    T m_value;
    manip_pointer m_manip;
 public:
    oneArgManip(manip_pointer manip,
		T value):
      m_manip(manip),
      m_value(value){
    }
      rstream& operator()(rstream& rs){
      return m_manip(rs, m_value);
    }
};



rstream& setBranch(rstream& rs,const char* name){
  strcpy(rs.m_branchName,name);
  return rs;
}

oneArgManip<const char*>* branchName(const char* name){
  return new oneArgManip<const char*>(setBranch,name);
}





//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


class orstream : public rstream{
 public:
  orstream(const char* fileName,
	   const char* treeName ="Data",
	   const char* treeTitle ="Created by orstream",
	   const char* mode = "RECREATE",
	   MsgStream::LEVEL loglevel= MsgStream::INFO): 
    rstream(fileName, mode, loglevel),
    m_evtCount(0){
    m_log.setName("orstream");
    m_tree = new TTree(treeName,treeTitle);
}

  void open(const char* fileName,
	    const char* treeName ="Data",
	    const char* treeTitle ="Created by orstream",
	    const char* mode = "READ"){
    m_file = new TFile(fileName,mode);
    m_tree = new TTree(treeName,treeTitle);
    strcpy(m_branchName,"");
    m_evtCount=0;
  }
    friend orstream& fillEvent(orstream& ors);

    template<typename J> orstream& operator<<(oneArgManip<J>*);
    template<typename T> orstream& operator<<(T&);
    template<typename T> orstream& operator<<(T* &);
    orstream& operator<<(orstream& (*)(orstream&));
  void close(){

    m_file->cd();
    m_tree->Write();
    m_file->Close();
    m_log << MsgStream::INFO << "Written "<<m_evtCount << " events to file." << std::endl;
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

 private:
  long m_evtCount;
};

orstream& orstream::operator<<(orstream& (*manip_pointer)(orstream&)){
    return manip_pointer(*this);
  }

template<typename T>
orstream& orstream::operator<<(T& d){
    m_pointers.push_back(&d);
    T** data = reinterpret_cast<T**>(&m_pointers.back());
    if(!strcmp(m_branchName,"")){
      m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
      m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using << beanchName(\"MyBranch\")" <<std::endl;
      return *this;
    }
    else if(!m_tree->FindBranch(m_branchName)){
      m_tree->Branch(m_branchName,data);
    }
    else{
      m_tree->SetBranchAddress(m_branchName,data);
    }

    return *this;
  }
template<typename T>
orstream& orstream::operator<<(T* & d){
    if(!strcmp(m_branchName,"")){ 
      m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
      m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using << beanchName(\"MyBranch\")" <<std::endl;
      return *this; 
    }
    else if(!m_tree->FindBranch(m_branchName)){
      m_tree->Branch(m_branchName,&d);
    }
    else{
      m_tree->SetBranchAddress(m_branchName,&d);
    }
    return *this;
  }

template<typename J>
orstream& orstream::operator<<(oneArgManip<J>* manip){
  (*manip)(*this);
  delete manip;
  return *this;
  }


orstream& fillEvent(orstream& ors){
  ors.m_tree->Fill();
  ++ors.m_evtCount;
  return ors;
}


//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

class irstream : public rstream{
 public:
  irstream(const char* fileName,
	   const char* treeName ="Data",
	   const char* mode = "READ",
	   MsgStream::LEVEL loglevel= MsgStream::INFO):
    rstream(fileName,mode, loglevel){
    m_log.setName("irstream");
    m_tree = (TTree*)m_file->GetObjectChecked(treeName,"TTree");
    if(!m_tree){ 
      m_log << MsgStream::ERROR << "The requested tree '"<<treeName << "' was not found in the tree." <<std::endl;
    }
  }

  void open(const char* fileName,
	    const char* treeName ="Data",
	    const char* mode = "READ"){
    m_file = new TFile(fileName,mode);
    m_tree = (TTree*)m_file->GetObjectChecked(treeName,"TTree");
    strcpy(m_branchName,"");
  }

  friend irstream& readEvent(irstream& ors);
template<typename T>
  irstream& operator>>(T& d){
    if(!m_tree->FindBranch(m_branchName)){
      m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
      m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using >> beanchName(\"MyBranch\")" <<std::endl;
      return *this;
    }
    m_pointers.push_back(&d);
    T** data = reinterpret_cast<T**>(&m_pointers.back());
    m_tree->SetBranchAddress(m_branchName,data);
    return *this;
  }
template<typename T>
  irstream& operator>>(T* & d){
    if(!m_tree->FindBranch(m_branchName)){
      m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
      m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using >> beanchName(\"MyBranch\")" <<std::endl;
      return *this;
    }
    m_tree->SetBranchAddress(m_branchName,&d);
    return *this;
  }

 irstream& operator>>(irstream& (*manip_pointer)(irstream&)){
   return manip_pointer(*this);
 }
 template<typename J> irstream& operator>>(oneArgManip<J>* manip){
  (*manip)(*this);
  delete manip;
  return *this;
 }

  void close(){
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
};

irstream& readEvent(irstream& irs){
  long nextEvent = irs.m_tree->GetReadEntry() + 1;
  if (nextEvent < irs.m_tree->GetEntries()){
    irs.m_tree->GetEntry(nextEvent);
  }
  else{
    irs.m_log << MsgStream::WARNING  << "End of file reached, cannot extract any more." <<std::endl;
  }
  return irs;
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

#endif
