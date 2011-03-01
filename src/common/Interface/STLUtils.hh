// Copyright 2010 Chris Rogers
//
// This file is a part of G4MICE
//
// G4MICE is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// G4MICE is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with G4MICE in the doc folder.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef STLUtils_HH
#define STLUtils_HH

/// STLUtils namespace contains useful utility functions for the STL.

/// There are a few things one would like to do for Standard Template Library
/// (STL) objects - for example, equality tests on STL containers, nicely
/// formatted print function for STL containers, etc. They're defined here.

namespace STLUtils {
/// Return a == b if a and b are same size and a[i] == b[i] for all i.

/// The following operations must be defined for TEMP_ITER it:
///   - ++it prefix increment operator
///   - (*it) (that is unary *, i.e. dereference operator)
///   - it1 != it2 not equals operator
///   - (*it1) != (*it2) not equals operator of dereferenced object
///
/// Call like e.g. \n
///     std::vector<int> a,b;\n
///     bool test_equal = IterableEquality(a.begin(), a.end(), b.begin(),
///                       b.end());\n
/// 
/// Can give a segmentation fault if a.begin() is not between a.begin() and 
/// a.end() (inclusive)
template <class TEMP_ITER>
bool IterableEquality(TEMP_ITER a_begin, TEMP_ITER a_end,
                      TEMP_ITER b_begin, TEMP_ITER b_end);


/// Return a == b if a and b are same size and a[i] == b[i] for all i.

/// This should work for STL containers of c-type objects. If you want to use
/// this function for STL containers of custom objects, you will need to define
/// != operator for your custom object. If you want to use this function for
/// custom containers of custom objects, you will need to define all of the
/// functions described here.
///
/// Call like e.g. \n
///     std::vector<int> a,b;\n
///     bool test_equal = IterableEquality(a, b);\n
///
/// Specifically, the following operations must be defined for TEMP_CLASS c:
///   - c.begin() which returns an iterator object, here denoted as type
///     TEMP_ITER
///   - c.end() which returns an iterator object, here denoted as type
///     TEMP_ITER
///
/// The following operations must be defined for container iterator TEMP_ITER
/// it:
///   - ++it prefix increment operator
///   - (*it) (that is unary *, i.e. dereference operator) that returns the
///     contained object, here denoted as TEMP_OBJ
///   - it1 != it2 not equals operator
///
/// The following operations must be defined for contained object TEMP_OBJ obj:
///   - obj != obj not equals operator of dereferenced object
///
template <class TEMP_CLASS>
inline bool IterableEquality(const TEMP_CLASS& a, const TEMP_CLASS& b);

}  // STLUtils namespace end

// Function definitions for inline and templates
namespace STLUtils {
template <class TEMP_CLASS>
bool IterableEquality(const TEMP_CLASS& a, const TEMP_CLASS& b) {
  return IterableEquality(a.begin(), a.end(), b.begin(), b.end());
}

template <class TEMP_ITER>
bool IterableEquality(TEMP_ITER a_begin, TEMP_ITER a_end, TEMP_ITER b_begin,
                      TEMP_ITER b_end) {
  TEMP_ITER a_it = a_begin;
  TEMP_ITER b_it = b_begin;
  while (a_it != a_end && b_it != b_end) {
    if (*a_it != *b_it) return false;
    ++a_it;
    ++b_it;
  }
  if ( a_it != a_end || b_it != b_end ) return false;
  return true;
}
}

/// A macro to disallow the copy constructor and operator= functions

/// The idea is that most classes don't want copy ctor and assignment
/// operator - by making them private we force the compiler to try to link
/// against the private function - which fails. This should be used in the
/// private: declarations for all classes wher copy constructor and assignment
/// operator are not defined.
///
/// Summary - put this in private: declaration for all classes like
/// #include "Interface/include/STLUtils.hh"
/// class Foo {
///  public:
///   Foo();
///   ~Foo();
/// private:
///   DISALLOW_COPY_AND_ASSIGN(Foo);
/// };
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

#endif


