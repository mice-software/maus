/*!
 * \file rstream.h
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 *
 * This is the abstract base class for root file streaming. It defines some common
 * functionality at this abstract level. Such functionality includes checking 
 * is a file is open. Initialising a root file and attaching externally defined
 * variables/objects to branches. It also stores a collection of pointers to 
 * branch variables. This is necessary as when using root trees, pointers to 
 * variables may not go out of scope and be destroyed.
 *
 */
#ifndef RSTREAM_H
#define RSTREAM_H 1

#include <cstring>
#include <TFile.h>
#include <TTree.h>
#include "MsgStream.hh"
#include <vector>

/*!
 * \class rstream
 *
 * \brief Abstract base class for Root streaming
 *
 * This class is set up to be intuitive to users of the standard c++
 * iostream library. A lot of common functionality is abstracted at this level
 * to make the code more scalable and managable.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class rstream{
 public:
  /*!\brief Constructor
   * \param char* the name of the root file to open
   * \param char* the mode specifier to use when opening
   * \param MsgStream::LEVEL the level of the logging output.
   */
  rstream(const char*,
	  const char*,
	  MsgStream::LEVEL = MsgStream::INFO);
  /*!\brief check if the file is open
   * \return boolean value indicating the presence of an open file.
   */
  virtual bool is_open(){ return !m_file? false :  m_file->IsOpen(); }
  //! Destructor
  virtual ~rstream();
  //! Close the file and cleanup, pure virtual function.
  virtual void close()=0;
  /*!\brief set the currently active branch
   * \param rstream& the stream to set the active branch of
   * \param char* the name to use for the branch
   * \return the rstream object from param 1
   */
  static rstream& setBranch(rstream& rs,const char* name);

 protected:
  /*!
   * \brief attach a branch to the non-pointer data type d
   * Templated function to attach the currently defined branch to 
   * a data type T.
   * \param T& The data type to attach to the branch.
   * \return *this
   */
  template<typename T> rstream& attachBranch(T&   d, bool, bool);
  /*!
   * \brief attach a branch to the pointer data type d
   * Templated function to attach the currently defined branch to 
   * a data type T.
   * \param T*& The data type to attach to the branch.
   * \return *this
   */
  template<typename T> rstream& attachBranch(T* & d, bool, bool);

  /*!\var TFile* m_file
   * \brief Pointer to the open file.
   */
  TFile* m_file;
  /*!\var MsgStream m_log
   * \brief The logging message stream.
   */
  MsgStream m_log;
  /*!\var char* m_branchName
   * \brief The currently defined branch name.
   */
  char* m_branchName;
  /*!\var TTree* m_tree
   * \brief pointer to the working tree.
   */
  TTree* m_tree;
  /*!\var std::vector<void*> m_pointers
   * \brief collection of pointers to data types.
   */
  std::vector<void*> m_pointers;
  /*!\var long m_evtCount
   * \brief event counter.
   */
  long m_evtCount;  //added later.

};


// Member functions template definitions
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

// template implementation - Non pointer
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
template<typename T> rstream& rstream::attachBranch(T& d,bool doublePointer, bool createNonExistentBranch){
  if(!strcmp(m_branchName,"")){
    m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
    m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using << branchName(\"MyBranch\")" <<std::endl;
    strcpy(m_branchName,"");
    return *this;
  }
  if(!m_tree->FindBranch(m_branchName) && !createNonExistentBranch){
    m_log << MsgStream::ERROR << "Branch not found"<< std::endl;
    m_log << MsgStream::INFO  << "Could not find the requested branch in the tree" <<std::endl;
    strcpy(m_branchName,"");
    return *this; 
  }
  m_pointers.push_back(&d);
  T** data1=0;
  T*  data2=0;
  if(doublePointer){ data1= reinterpret_cast<T**>(&m_pointers.back());}
  else{              data2= reinterpret_cast<T* >( m_pointers.back());}

  if(!m_tree->FindBranch(m_branchName)){
    if(doublePointer){ m_tree->Branch(m_branchName,data1);}
    else{              m_tree->Branch(m_branchName,data2);}
  }
  else{
    if(doublePointer){ m_tree->SetBranchAddress(m_branchName,data1);}
    else{              m_tree->SetBranchAddress(m_branchName,data2);}
  }
  strcpy(m_branchName,""); //this has effect of forcing the user to use branchName("blah") before setting 
  return *this;            //every variable as resets the branchname and will get above warning
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// template implementation - Pointer
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
template<typename T> rstream& rstream::attachBranch(T* & d,bool doublePointer, bool createNonExistentBranch){
  if(!strcmp(m_branchName,"")){
    m_log << MsgStream::ERROR << "No branch name set"<< std::endl;
    m_log << MsgStream::INFO  << "Setup a branch name before attaching a data object using << branchName(\"MyBranch\")" <<std::endl;
    strcpy(m_branchName,"");
    return *this;
  }
  if(!m_tree->FindBranch(m_branchName) && !createNonExistentBranch){
    m_log << MsgStream::ERROR << "Branch not found"<< std::endl;
    m_log << MsgStream::INFO  << "Could not find the requested branch in the tree" <<std::endl;
    strcpy(m_branchName,"");
    return *this; 
  }
  if(!m_tree->FindBranch(m_branchName)){
    if(doublePointer){ m_tree->Branch(m_branchName,&d);}
    else{              m_tree->Branch(m_branchName, d);}
  }
  else{
    if(doublePointer){ m_tree->SetBranchAddress(m_branchName,&d);}
    else{              m_tree->SetBranchAddress(m_branchName, d);}
  }
  strcpy(m_branchName,""); //this has effect of forcing the user to use branchName("blah") before setting
  return *this;            // every variable as resets the branchname and will get above warning
}

#endif

