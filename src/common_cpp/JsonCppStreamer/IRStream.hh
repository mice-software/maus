#ifndef IRSTREAM_H
#define IRSTREAM_H

#include "gtest/gtest_prod.h"

#include "RStream.hh"
#include "OneArgManip.hh"

class irstream : public rstream{
 public:
  irstream(const char*,
	   const char* = "Data",
	   const char* = "READ",
	   MsgStream::LEVEL = MsgStream::INFO);
  
  void open(const char*,
	    const char* = "Data",
	    const char* = "READ");
  
  void close();
// Note changed this to a pointer so can return 0 to get
//  while(irstream>>) functionality. This was done afterwards so may
//  break things
//  friend irstream& readEvent(irstream& ors);
//  irstream& operator>>(irstream& (*manip_pointer)(irstream&));
  friend irstream* readEvent(irstream& ors);
  irstream* operator>>(irstream* (*manip_pointer)(irstream&));

  template<typename T> irstream& operator>>(oneArgManip<T>* manip);
  template<typename T> irstream& operator>>(T& d);
  template<typename T> irstream& operator>>(T* & d);

  long getEventCount() {return m_evtCount;}

 private:
  template<typename T> irstream& basetype_extraction(T&);
  template<typename T> irstream& basetype_extraction(T* &);
  long m_evtCount;  //added later.

  FRIEND_TEST(IRStreamTest, TestConstructor);
  FRIEND_TEST(IRStreamTest, TestFileOpen);
  FRIEND_TEST(IRStreamTest, TestFileClose);
  FRIEND_TEST(IRStreamTest, TestReadEvent);
};


// Member functions template definitions
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
template<typename T> irstream& irstream::operator>>(oneArgManip<T>* manip){
  (*manip)(*this);
  delete manip;
  return *this;
}

template<typename T>
irstream& irstream::operator>>(T& d){
  if(!strcmp(m_branchName,"")){
    m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
    m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using >> branchName(\"MyBranch\")" <<std::endl;
    return *this;
  }
  if(!m_tree->FindBranch(m_branchName)){
    m_log << MsgStream::ERROR << "Couldn't find requested branch '" << m_branchName << "' in TTree"<< std::endl;
    return *this;
  }
  m_pointers.push_back(&d);
  T** data = reinterpret_cast<T**>(&m_pointers.back()); //need to hold on to the pointers to the type so they dont go out of scope
  m_tree->SetBranchAddress(m_branchName,data);
  strcpy(m_branchName,"");
  return *this;
}
template<>irstream& irstream::operator>> <int>(int& d){return basetype_extraction(d);}
template<>irstream& irstream::operator>> <short>(short& d){return basetype_extraction(d);}
template<>irstream& irstream::operator>> <long>(long& d){return basetype_extraction(d);}
template<>irstream& irstream::operator>> <double>(double& d){return basetype_extraction(d);}
template<>irstream& irstream::operator>> <float>(float& d){return basetype_extraction(d);}

template<typename T>
irstream& irstream::operator>>(T* & d){
  if(!strcmp(m_branchName,"")){
    m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
    m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using >> branchName(\"MyBranch\")" <<std::endl;
    return *this;
  }
  if(!m_tree->FindBranch(m_branchName)){
    m_log << MsgStream::ERROR << "Couldn't find requested branch '" << m_branchName << "' in TTree" << std::endl;
    return *this;
  }
  m_tree->SetBranchAddress(m_branchName,&d);
  strcpy(m_branchName,"");
  return *this;
}
template<>irstream& irstream::operator>> <int>(int* & d){return basetype_extraction(d);}
template<>irstream& irstream::operator>> <short>(short* & d){return basetype_extraction(d);}
template<>irstream& irstream::operator>> <long>(long* & d){return basetype_extraction(d);}
template<>irstream& irstream::operator>> <double>(double* & d){return basetype_extraction(d);}
template<>irstream& irstream::operator>> <float>(float* & d){return basetype_extraction(d);}

template<typename T> irstream& irstream::basetype_extraction(T& d){
  m_pointers.push_back(&d);
  T* data = reinterpret_cast<T*>(m_pointers.back());//need to hold onto pointers to data so dont go out of scopewhile running.
  if(!strcmp(m_branchName,"")){
    m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
    m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using << branchName(\"MyBranch\")" <<std::endl;
    return *this;
  }
  if(!m_tree->FindBranch(m_branchName)){
    m_log << MsgStream::ERROR << "Couldn't find requested '" << m_branchName << "' in TTree" << std::endl;
    return *this;
  }
  m_tree->SetBranchAddress(m_branchName,data);
  strcpy(m_branchName,""); //this has effect of forcing the user to use branchName("blah") before setting every variable as resets the branchname and will get above warning
  return *this;
}

template<typename T> irstream& irstream::basetype_extraction(T* & d){
  if(!strcmp(m_branchName,"")){ 
    m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
    m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using << branchName(\"MyBranch\")" <<std::endl;
    return *this; 
  }
  if(!m_tree->FindBranch(m_branchName)){
    m_log << MsgStream::ERROR << "Couldn't find requested branch '" << m_branchName << "' in TTree"<< std::endl;
    return *this;
  }
  m_tree->SetBranchAddress(m_branchName,d);
  strcpy(m_branchName,""); //this has effect of forcing the user to use branchName("blah") before setting every variable as resets the branchname and will get above warning
  return *this;
}
// Friend function definitions
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Note changed this to a pointer so can return 0 to get
//  while(irstream>>) functionality. This was done afterwards so may
//  break things
//  irstream& readEvent(irstream& irs){
#ifndef READEVENT
#define READEVENT

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
#endif

#endif
