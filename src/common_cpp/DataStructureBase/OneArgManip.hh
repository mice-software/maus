/*!
 * \file oneArgManip.h
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 *
 * This file contains a class template representing a one argument manipulator function
 * which will act on the rstream. It also contains a function "branchName" which returns
 * a oneArgManip rstream manipulator which will allow for setting up of branches within
 * a root tree.
 */
#ifndef ONEARG_MANIP_H
#define ONEARG_MANIP_H

#include "RStream.hh"

// Single argument manipulator object
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
/*!
 * \class oneArgManip
 *
 * \brief Wrapper class for the single argument manipulators
 *
 * This class wraps a single argument manipulator by storing the intended argument and passing it
 * along with the stream when called. The template type T represents the argument type of the function
 * pointed to by the stored function pointer.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
template<typename T>
class oneArgManip{
public:
  /*!
   * \brief call the wrapped function
   * \param rstream& The rstream object that the function will operate on.
   */
  rstream& operator()(rstream& rs){
    return m_manip(rs, m_value);
  }

  /*!
   * \brief single arg manipulator to set the branch name within the stream.
   * \param char* the name to use as the next branch name.
   */
  static oneArgManip<T>* branchName(T);

private:
#ifdef TESTING
  friend class test_oneArgManip;
#endif
  //! Type def manipulator function pointer
  typedef rstream& (*manip_pointer)(rstream&,T);
  /*!
   * \brief Constructor
   * \param manip_pointer a function pointer to the single arg manipulator
   * \param T the single argument to be wrapped.
   */
  oneArgManip(manip_pointer manip, T value): m_value(value),  m_manip(manip) {}

  /*!
   * \var T m_value
   * \brief The argument that the single arg manipulator should take.
   */
  T m_value;
  /*!
   * \var manip_pointer m_manip
   * \brief pointer to the single arg manipulator function.
   */  
  manip_pointer m_manip;
};

template<>
oneArgManip<const char*>* oneArgManip<const char*>::branchName(const char* name);


// Related single arg function.
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

// Forward declaration of setBranch
//rstream& setBranch(rstream& rs,const char* name);

#endif
