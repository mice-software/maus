#ifndef ORSTREAM_H
#define ORSTREAM_H
#include "RStream.hh"
#include "OneArgManip.hh"

class orstream : public rstream{
 public:
  orstream(const char*,
	   const char* = "Data",
	   const char* = "Created by orstream",
	   const char* = "RECREATE",
	   MsgStream::LEVEL= MsgStream::INFO);

  void open(const char*,
	    const char* = "Data",
	    const char* = "Created by orstream",
	    const char* = "READ");

  void close();
  friend orstream& fillEvent(orstream& ors);
  orstream& operator<<(orstream& (*)(orstream&));

  template<typename T> orstream& operator<<(oneArgManip<T>*);
  template<typename T> orstream& operator<<(T&);
  template<typename T> orstream& operator<<(T* &);
#ifdef TESTING
  friend class test_orstream;
#endif
 private:
  template<typename T> orstream& basetype_insertion(T&);
  template<typename T> orstream& basetype_insertion(T* &);
  long m_evtCount;
};


// Member functions template definitions
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
template<typename T>
orstream& orstream::operator<<(oneArgManip<T>* manip){
  (*manip)(*this);
  delete manip;
  return *this;
}


template<typename T> orstream& orstream::operator<<(T& d){
  m_pointers.push_back(&d);
  T** data = reinterpret_cast<T**>(&m_pointers.back());
  if(!strcmp(m_branchName,"")){
    m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
    m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using << branchName(\"MyBranch\")" <<std::endl;
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
template<>orstream& orstream::operator<< <int>(int& d){return basetype_insertion(d);}
template<>orstream& orstream::operator<< <short>(short& d){return basetype_insertion(d);}
template<>orstream& orstream::operator<< <long>(long& d){return basetype_insertion(d);}
template<>orstream& orstream::operator<< <double>(double& d){return basetype_insertion(d);}
template<>orstream& orstream::operator<< <float>(float& d){return basetype_insertion(d);}

template<typename T> orstream& orstream::operator<<(T* & d){
  if(!strcmp(m_branchName,"")){ 
    m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
    m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using << branchName(\"MyBranch\")" <<std::endl;
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
template<>orstream& orstream::operator<< <int>(int* & d){return basetype_insertion(d);}
template<>orstream& orstream::operator<< <short>(short* & d){return basetype_insertion(d);}
template<>orstream& orstream::operator<< <long>(long* & d){return basetype_insertion(d);}
template<>orstream& orstream::operator<< <double>(double* & d){return basetype_insertion(d);}
template<>orstream& orstream::operator<< <float>(float* & d){return basetype_insertion(d);}

template<typename T> orstream& orstream::basetype_insertion(T& d){
  m_pointers.push_back(&d);
  T* data = reinterpret_cast<T*>(m_pointers.back());//need to hold onto pointers to data so dont go out of scopewhile running.
  if(!strcmp(m_branchName,"")){
    m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
    m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using << branchName(\"MyBranch\")" <<std::endl;
    return *this;
  }
  if(!m_tree->FindBranch(m_branchName)){
    m_tree->Branch(m_branchName,data);
  }
  else{
    m_tree->SetBranchAddress(m_branchName,data);
  }
  strcpy(m_branchName,""); //this has effect of forcing the user to use branchName("blah") before setting every variable as resets the branchname and will get above warning
  return *this;
}

template<typename T> orstream& orstream::basetype_insertion(T* & d){
  if(!strcmp(m_branchName,"")){ 
    m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
    m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using << branchName(\"MyBranch\")" <<std::endl;
    return *this; 
  }
  if(!m_tree->FindBranch(m_branchName)){
    m_tree->Branch(m_branchName,d);
  }
  else{
    m_tree->SetBranchAddress(m_branchName,d);
  }
  strcpy(m_branchName,""); //this has effect of forcing the user to use branchName("blah") before setting every variable as resets the branchname and will get above warning
  return *this;
}
// Friend function definitions
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
orstream& fillEvent(orstream& ors);

#endif
