/*!
 * \file irstream.h
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 *
 * This file defines the irstream class as well as the stream manipulator
 * readEvent. This manipulator is responsible for getting entries from the 
 * tree as well as incrementing the event counter.
 */
#ifndef IRSTREAM_H
#define IRSTREAM_H

#include "gtest/gtest_prod.h"

#include "JsonCppStreamer/RStream.hh"
#include "JsonCppStreamer/OneArgManip.hh"

/*!
 * \class irstream
 *
 * \brief class responsible for streaming from root files.
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class irstream : public rstream {
 public:
  /*!
   * \brief Constructor
   * \param char* the name of the root file to open
   * \param char* the name of the root tree to read
   * \param char* the mode specifier to use when opening
   * \param MsgStream::LEVEL the level of the logging output.
   */
  irstream(const char*,
	   const char* = "Data",
	   const char* = "READ");

  /*!
   * \brief open a root input stream
   * \param char* the name of the root file to open
   * \param char* the name of the root tree to read
   * \param char* the mode specifier to use when opening
   * \param MsgStream::LEVEL the level of the logging output.
   */
  void open(const char*,
	    const char* = "Data",
	    const char* = "READ");

  // ! \brief close the file and cleanup
  void close();
  // ! Declare friend function.
  friend irstream* readEvent(irstream& ors);

  /*!
   * \brief extraction operator dealing with zero-arg manipulators
   * \param irstream*(*)(irstream&) function pointer pointing to the manipulator function.
   * \return *this
   */
  irstream* operator>>(irstream* (*manip_pointer)(irstream&));

  /*!
   * \brief extraction operator dealing with single-arg manipulators
   * Templated function to call the oneArgManip wrapper functor.
   * \param oneArgManip<T>* Pointer to a oneArgManip single-arg wrapper functor
   * \return *this
   */
  template<typename T> irstream& operator>>(oneArgManip<T>* manip);
  /*!
   * \brief link a data type to a branch
   * Templated extraction operator takes a data type and tries to attach it to the 
   * currently defined branch.
   * \param T& templated data type T
   * \return *this
   */
  template<typename T> irstream& operator>>(T& d);
  /*!
   * \brief link a data type to a branch
   * Templated extraction operator takes a pointer to a data type and tries to 
   * attach it to the currently defined branch.
   * \param T*& pointer to a templated data type T
   * \return *this
   */
  template<typename T> irstream& operator>>(T* & d);

 private:
  FRIEND_TEST(IRStreamTest, TestConstructor);
  FRIEND_TEST(IRStreamTest, TestFileOpen);
  FRIEND_TEST(IRStreamTest, TestFileClose);
  FRIEND_TEST(IRStreamTest, TestReadEvent);
};


// Member functions template definitions
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
template<typename T> irstream& irstream::operator>>(oneArgManip<T>* manip) {
  (*manip)(*this);
  delete manip;
  return *this;
}

template<typename T> irstream& irstream::operator>>         (T&      d){attachBranch(d,true ,false); return *this;}
template<>           irstream& irstream::operator>> <int>   (int&    d){attachBranch(d,false,false); return *this;}
template<>           irstream& irstream::operator>> <short> (short&  d){attachBranch(d,false,false); return *this;}
template<>           irstream& irstream::operator>> <long>  (long&   d){attachBranch(d,false,false); return *this;}
template<>           irstream& irstream::operator>> <double>(double& d){attachBranch(d,false,false); return *this;}
template<>           irstream& irstream::operator>> <float> (float&  d){attachBranch(d,false,false); return *this;}

template<typename T> irstream& irstream::operator>>         (T* &      d){attachBranch(d,true ,false); return *this;}
template<>           irstream& irstream::operator>> <int>   (int* &    d){attachBranch(d,false,false); return *this;}
template<>           irstream& irstream::operator>> <short> (short* &  d){attachBranch(d,false,false); return *this;}
template<>           irstream& irstream::operator>> <long>  (long* &   d){attachBranch(d,false,false); return *this;}
template<>           irstream& irstream::operator>> <double>(double* & d){attachBranch(d,false,false); return *this;}
template<>           irstream& irstream::operator>> <float> (float* &  d){attachBranch(d,false,false); return *this;}


// Friend function definitions
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
irstream* readEvent(irstream& irs);

#endif
