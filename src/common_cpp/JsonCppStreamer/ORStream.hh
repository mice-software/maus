/*!
 * \file orstream.h
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 *
 * This file defines the orstream class as well as the stream manipulator
 * fillEvent. This manipulator is responsible for filling the tree with the 
 * linked variables content as well as incrementing the event counter.
 */
#ifndef ORSTREAM_H
#define ORSTREAM_H

#include "gtest/gtest_prod.h"

#include "RStream.hh"
#include "OneArgManip.hh"

/*!
 * \class orstream
 *
 * \brief class responsible for streaming to root files.
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class orstream : public rstream{
 public:
  /*!
   * \brief Constructor
   * \param char* the name of the root file to open
   * \param char* the name of the root tree to save
   * \param char* the title of the root tree to save
   * \param char* the mode specifier to use when opening
   * \param MsgStream::LEVEL the level of the logging output.
   */
  orstream(const char*,
	   const char* = "Data",
	   const char* = "Created by orstream",
	   const char* = "RECREATE");

  /*!
   * \brief open a root output stream.
   * \param char* the name of the root file to open
   * \param char* the name of the root tree to save
   * \param char* the title of the root tree to save
   * \param char* the mode specifier to use when opening
   */
  void open(const char*,
	    const char* = "Data",
	    const char* = "Created by orstream",
	    const char* = "READ");

  //! \brief close the file and cleanup
  void close();
  //! Declare friend function.
  friend orstream& fillEvent(orstream& ors);
  /*!
   * \brief insertion operator dealing with zero-arg manipulators
   * \param orstream&(*)(orstream&) function pointer pointing to the manipulator function.
   * \return *this
   */
  orstream& operator<<(orstream& (*)(orstream&));

  /*!
   * \brief insertion operator dealing with single-arg manipulators
   * Templated function to call the oneArgManip wrapper functor.
   * \param oneArgManip<T>* Pointer to a oneArgManip single-arg wrapper functor
   * \return *this
   */
  template<typename T> orstream& operator<<(oneArgManip<T>*);
  /*!
   * \brief link a data type to a branch
   * Templated insertion operator takes a data type and tries to attach it to the 
   * currently defined branch.
   * \param T& templated data type T
   * \return *this
   */
  template<typename T> orstream& operator<<(T&);
  /*!
   * \brief link a data type to a branch
   * Templated insertion operator takes a pointer to a data type and tries to 
   * attach it to the currently defined branch.
   * \param T*& pointer to a templated data type T
   * \return *this
   */
  template<typename T> orstream& operator<<(T* &);

 private:
  FRIEND_TEST(ORStreamTest, TestConstructor);
  FRIEND_TEST(ORStreamTest, TestFileOpen);
  FRIEND_TEST(ORStreamTest, TestFileClose);
  FRIEND_TEST(ORStreamTest, TestFillEvent);

};


// Member functions template definitions
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
template<typename T>
orstream& orstream::operator<<(oneArgManip<T>* manip){
  (*manip)(*this);
  delete manip;
  return *this;
}

template<typename T> orstream& orstream::operator<<         (T&      d){attachBranch(d,true ,true); return *this;}
template<>           orstream& orstream::operator<< <int>   (int&    d){attachBranch(d,false,true); return *this;}
template<>           orstream& orstream::operator<< <short> (short&  d){attachBranch(d,false,true); return *this;}
template<>           orstream& orstream::operator<< <long>  (long&   d){attachBranch(d,false,true); return *this;}
template<>           orstream& orstream::operator<< <double>(double& d){attachBranch(d,false,true); return *this;}
template<>           orstream& orstream::operator<< <float> (float&  d){attachBranch(d,false,true); return *this;}

template<typename T> orstream& orstream::operator<<         (T* &      d){attachBranch(d,true ,true); return *this;}
template<>           orstream& orstream::operator<< <int>   (int* &    d){attachBranch(d,false,true); return *this;}
template<>           orstream& orstream::operator<< <short> (short* &  d){attachBranch(d,false,true); return *this;}
template<>           orstream& orstream::operator<< <long>  (long* &   d){attachBranch(d,false,true); return *this;}
template<>           orstream& orstream::operator<< <double>(double* & d){attachBranch(d,false,true); return *this;}
template<>           orstream& orstream::operator<< <float> (float* &  d){attachBranch(d,false,true); return *this;}


// Friend function definitions
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
/*!
 * \brief push the event back into the tree with the current content of the branches.
 * This manipulator function will populate the tree with an additional event
 * coming from the content of the branches.
 * \param orstream& The stream holding the tree to be filled.
 * \return \a orstream& from argument 1.
 */
orstream& fillEvent(orstream& ors);

#endif
