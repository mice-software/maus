/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


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

#include <TFile.h>
#include <TTree.h>
#include <cstring>
#include <vector>
#include "Utils/Squeak.hh"

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
class rstream {
 public:
  /*!\brief Constructor
   * \param char* the name of the root file to open
   * \param char* the mode specifier to use when opening
   * \param MsgStream::LEVEL the level of the logging output.
   */
  rstream(const char*, const char*);
  /*!\brief check if the file is open
   * \return boolean value indicating the presence of an open file.
   */
  virtual bool is_open();
  // ! Destructor
  virtual ~rstream();
  // ! Close the file and cleanup, pure virtual function.
  virtual void close() = 0;
  /*!\brief set the currently active branch
   * \param rstream& the stream to set the active branch of
   * \param char* the name to use for the branch
   * \return the rstream object from param 1
   */
  static rstream& setBranch(rstream& rs, const char* name);

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

  /*!\var TTree* m_tree
   * \brief pointer to the working tree.
   */
  TTree* m_tree;
  /*!\var long m_evtCount
   * \brief event counter.
   */
  long m_evtCount;
  /*!\var TFile* m_file
   * \brief Pointer to the open file.
   */
  TFile* m_file;
  /*!\var char* m_branchName
   * \brief The currently defined branch name.
   */
  char* m_branchName;
  /*!\var std::vector<void*> m_pointers
   * \brief collection of pointers to data types.
   */
  std::vector<void*> m_pointers;
};


// Member functions template definitions
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

// template implementation - Non pointer
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
template<typename T> rstream& rstream::attachBranch(T& d,
						    bool doublePointer,
						    bool createNonExistentBranch) {
  if (!strcmp(m_branchName, "")) {
    MAUS::Squeak::mout(MAUS::Squeak::error)
      << "No branch name set"
      << std::endl;
    MAUS::Squeak::mout(MAUS::Squeak::info)
      << "Setup a branch name before attaching a data object using << branchName(\"MyBranch\")"
      << std::endl;
    strcpy(m_branchName, "");
    return *this;
  }
  if (!m_tree->FindBranch(m_branchName) && !createNonExistentBranch) {
    MAUS::Squeak::mout(MAUS::Squeak::error)
      << "Branch not found"
      << std::endl;
    MAUS::Squeak::mout(MAUS::Squeak::info)
      << "Could not find the requested branch in the tree"
      << std::endl;
    strcpy(m_branchName, "");
    return *this;
  }
  m_pointers.push_back(&d);
  T** data1 = 0;
  T*  data2 = 0;
  if (doublePointer) {
    data1 = reinterpret_cast<T**>(&m_pointers.back());
  } else {
    data2 = reinterpret_cast<T* >( m_pointers.back());
  }

  if (!m_tree->FindBranch(m_branchName)) {
    if (doublePointer) {
      m_tree->Branch(m_branchName, data1);
    } else {
      m_tree->Branch(m_branchName, data2);
    }
  } else {
    if (doublePointer) {
      m_tree->SetBranchAddress(m_branchName, data1);
    } else {
      m_tree->SetBranchAddress(m_branchName, data2);
    }
  }
  // this has effect of forcing the user to use branchName("blah") before setting
  // every variable as resets the branchname and will get above warning
  strcpy(m_branchName, "");
  return *this;
}

// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// template implementation - Pointer
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
template<typename T> rstream& rstream::attachBranch(T* & d,
						    bool doublePointer,
						    bool createNonExistentBranch) {
  if (!strcmp(m_branchName, "")) {
    MAUS::Squeak::mout(MAUS::Squeak::error)
      << "No branch name set"
      << std::endl;
    MAUS::Squeak::mout(MAUS::Squeak::info)
      << "Setup a branch name before attaching a data object using << branchName(\"MyBranch\")"
      << std::endl;
    strcpy(m_branchName, "");
    return *this;
  }
  if (!m_tree->FindBranch(m_branchName) && !createNonExistentBranch) {
    MAUS::Squeak::mout(MAUS::Squeak::error)
      << "Branch not found"
      << std::endl;
    MAUS::Squeak::mout(MAUS::Squeak::info)
      << "Could not find the requested branch in the tree"
      << std::endl;
    strcpy(m_branchName, "");
    return *this;
  }
  if (!m_tree->FindBranch(m_branchName)) {
    if (doublePointer) {
      m_tree->Branch(m_branchName, &d);
    } else {
      m_tree->Branch(m_branchName,  d);
    }
  } else {
    if (doublePointer) {
      m_tree->SetBranchAddress(m_branchName, &d);
    } else {
      m_tree->SetBranchAddress(m_branchName,  d);
    }
  }
  // this has effect of forcing the user to use branchName("blah") before setting
  // every variable as resets the branchname and will get above warning
  strcpy(m_branchName, "");
  return *this;
}

#endif

