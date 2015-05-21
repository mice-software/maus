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

/** @class PyObjectWrapper
 *  \brief Provide utilities to wrap and unwrap various MAUS objects
 *  in PyObject containers, vital for the Python / C API.
 */

#include <string>
#include <iostream>

#ifndef _SRC_COMMON_CPP_UTILS_PYOBJECTWRAPPER_HH
#define _SRC_COMMON_CPP_UTILS_PYOBJECTWRAPPER_HH

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"
#include "json/value.h"

#include "src/common_cpp/Utils/Exception.hh"

namespace MAUS {

class Data;
class JobHeaderData;
class JobFooterData;
class RunHeaderData;
class RunFooterData;

/* @brief PyObjectWrapper handles wrapping and unwrapping of python objects
 *
 * Define templated functions for unwrapping PyObjects. The wrapping process
 * is lazy, in that we wrap whatever data format is currently in use (and
 * presumably hand to python whatever data format is in use). The
 * unwrapping process is not lazy and will perform any required conversions to
 * get the correct C++ data structure.
 *
 * The following C++ objects can be used. A brief description is given of the
 * resultant python object. The C++ object that is used is controlled by the
 * templates, while the Python object is always a PyObject*
 *   - MAUS::Data -> PyROOT ROOT.MAUS.Data object
 *   - Json::Value -> PyCapsule* with type string "JsonCpp"
 *   - std::string -> PyObject* string
 *   - PyObject* dict -> PyObject* dict; presumed to be a PyJson structure
 * Attempts to wrap or unwrap to other C++ objects will be thrown out at compile
 * time. Attempts to wrap or unwrap from malformed Python objects will result in
 * a MAUS::Exception.
 */
class PyObjectWrapper {
  public:
    /* @brief Template for Unwrapping a PyObject*
     *
     * - args - the argument to be unwrapped - should be a tuple containing a
     * single object.
     *
     * Makes any necessary conversions on the PyObject* to get into TEMP format
     *
     * Caller keeps ownership of memory allocated by args. INPUT* is a new
     * block of memory that is now owned by caller.
     *
     * @throws MAUS::Exception if input is NULL or any part of the conversion
     *         fails
     *
     * e.g. Data* data = unwrap_pyobject<Data>(my_py_object);
     * e.g. Json::Value* value = unwrap_pyobject<Json::Value>(my_py_object);
     * 
     */
    template <typename TEMP>
    static inline TEMP* unwrap(PyObject *args);

    /* @brief Wrap a MAUS::Data into a PyObject*
     *
     * @param data - the input value to be wrapped. Python takes ownership of
     *     memory pointed to by data.
     *
     * @returns a block of memory that is now owned by Python (Py_INCREF is
     *  called).
     *
     * @throws MAUS::Exception if output is NULL
     */
    static inline PyObject* wrap(MAUS::Data* data);

    /* @brief Wrap a MAUS::JobHeaderData into a PyObject*
     *
     * @param data - the input value to be wrapped. Python takes ownership of
     *     memory pointed to by data.
     *
     * @returns a block of memory that is now owned by Python (Py_INCREF is
     *  called).
     *
     * @throws MAUS::Exception if output is NULL
     */
    static inline PyObject* wrap(MAUS::JobHeaderData* data);

    /* @brief Wrap a MAUS::JobFooterData into a PyObject*
     *
     * @param data - the input value to be wrapped. Python takes ownership of
     *     memory pointed to by data.
     *
     * @returns a block of memory that is now owned by Python (Py_INCREF is
     *  called).
     *
     * @throws MAUS::Exception if output is NULL
     */
    static inline PyObject* wrap(MAUS::JobFooterData* data);

    /* @brief Wrap a MAUS::RunHeaderData into a PyObject*
     *
     * @param data - the input value to be wrapped. Python takes ownership of
     *     memory pointed to by data.
     *
     * @returns a block of memory that is now owned by Python (Py_INCREF is
     *  called).
     *
     * @throws MAUS::Exception if output is NULL
     */
    static inline PyObject* wrap(MAUS::RunHeaderData* data);

    /* @brief Wrap a MAUS::RunFooterData into a PyObject*
     *
     * @param data - the input value to be wrapped. Python takes ownership of
     *     memory pointed to by data.
     *
     * @returns a block of memory that is now owned by Python (Py_INCREF is
     *  called).
     *
     * @throws MAUS::Exception if output is NULL
     */
    static inline PyObject* wrap(MAUS::RunFooterData* data);

    /* @brief Wrap a Json::Value into a PyObject*
     *
     * @param data - the input value to be wrapped. Python takes ownership of
     *     memory pointed to by json. 
     *
     * @returns a new block of memory that is now owned by Python (Py_INCREF is
     *  called).
     *
     * @throws MAUS::Exception if output is NULL
     */
    static inline PyObject* wrap(Json::Value* json);

    /* @brief Wrap a std::string into a PyObject*
     *
     * @param data - the input value to be wrapped. Python takes ownership of
     *     memory pointed to by str.
     *
     * @returns a new block of memory that is now owned by Python (Py_INCREF is
     *  called).
     *
     * @throws MAUS::Exception if output is NULL
     */
    static inline PyObject* wrap(std::string* str);

    /* @brief Wrap a PyObject* into a PyObject* (NULL op)
     *
     * @throws MAUS::Exception if py_object is NULL
     */
    static inline PyObject* wrap(PyObject* py_object);

    /** @brief delete the pycapsule object and any Json::Value stored within
     */
    static inline void delete_jsoncpp_pycapsule(PyObject* object);

  private:
    // lazy unwrap
    // fills the unwrapped data or leaves the pointer to the unwrapped data as
    // NULL
    //
    // py_input should be a function argument, i.e. a tuple of a single python
    // object
    // string_ret, json_ret, data_ret, py_ret should be pointers to NULL pointer
    //
    // lazy_unwrap will only fill one of <blah>_ret; lazy_unwrap will allocate
    // new memory (possibly unnecessary)
    static inline void lazy_unwrap(PyObject* args,
                            PyObject** py_ret,
                            std::string** string_ret,
                            Json::Value** json_ret,
                            MAUS::Data** data_ret,
                            JobHeaderData** jh_ret,
                            JobFooterData** jf_ret,
                            RunHeaderData** rh_ret,
                            RunFooterData** rf_ret);

    // parse a PyROOT ObjectProxy into a C++ Event object
    //
    // py_object_proxy should be an object that e.g. passes
    // TPython::ObjectProxy_Check
    // data_ret should be a pointer to a NULL pointer (which we fill with
    // MAUS::Data data)
    template <class TEMP>
    static inline void unwrap_root_object_proxy(
                                        PyObject* py_object_proxy,
                                        TEMP** data_ret,
                                        std::string name);

    template <class TEMP>
    static inline PyObject* wrap_root_object_proxy(TEMP* data, std::string name);
};
}  // namespace MAUS

#include "src/common_cpp/Utils/PyObjectWrapper-inl.hh"

#endif


