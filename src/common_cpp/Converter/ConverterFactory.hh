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
 * \file ConverterFactory.hh
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/06/2012
 *
 * This class is responsible for managing the converters and retrieving
 * the correct converter for the INPUT/OUTPUT types in question. It can
 * be used to generically convert from one type to another without 
 * knowledge of the correct conversion object required for the specific
 * data types.
 *
 */
#ifndef _SRC_COMMON_CPP_CONVERTER_CONVERTERFACTORY_
#define _SRC_COMMON_CPP_CONVERTER_CONVERTERFACTORY_
#include <iostream>
#include <string>

// These ifdefs are required to avoid cpp compiler warning (gcc4.6.1)
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"

#include "json/value.h"

#include "src/common_cpp/Converter/IConverter.hh"
#include "src/common_cpp/Converter/ConverterExceptions.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/API/APIExceptions.hh"



namespace MAUS {

  /*!
   * \class ConverterFactory
   *
   * \brief Factory class for converter objects
   *
   * \author Alexander Richards, Imperial College London
   * \date 06/06/2012
   *
   * This class is responsible for managing the converters and retrieving
   * the correct converter for the INPUT/OUTPUT types in question. It can
   * be used to generically convert from one type to another without 
   * knowledge of the correct conversion object required for the specific
   * data types.
   *
   * conversions are provided for
   *    - string -> string
   *    - string -> json
   *    - string -> MAUS::Data
   *    - string -> PyObject* (py json)
   *    - json -> string
   *    - json -> json
   *    - json -> MAUS::Data
   *    - json -> PyObject* (py json)
   *    - MAUS::Data -> string
   *    - MAUS::Data -> json
   *    - MAUS::Data -> MAUS::Data
   *    - MAUS::Data -> PyObject* (py json)
   *    - PyObject* -> string
   *    - PyObject* -> json
   *    - PyObject* -> MAUS::Data
   *    - PyObject* -> PyObject* (py json)
   * logic is only non-trivial for MAUS -> json and PyObject* -> json (all
   * other conversions are based around this)
   */
  class ConverterFactory {
  public:
    template <typename INPUT, typename OUTPUT>
    IConverter<INPUT, OUTPUT>* getConverter() const;

    template <typename INPUT, typename OUTPUT>
    OUTPUT* convert(INPUT* i) const;

    /** Overloaded function to delete value */
    static void delete_type(Json::Value* value) {delete value;}

    /** Overloaded function to  delete value */
    static void delete_type(MAUS::Data* value) {delete value;}

    /** Overloaded function to  delete value */
    static void delete_type(std::string* value) {delete value;}

    /** Overloaded function to call Py_DECREF on value */
    static void delete_type(PyObject* value) {Py_DECREF(value);}
  };

} // end of namespace

#include "src/common_cpp/Converter/ConverterFactory-inl.hh"
#endif
