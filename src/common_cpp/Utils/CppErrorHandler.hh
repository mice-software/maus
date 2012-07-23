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

#ifndef _SRC_COMMON_CPP_CPPERRORHANDLER_HH_
#define _SRC_COMMON_CPP_CPPERRORHANDLER_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

#include <string>

#include "json/json.h"

#include "src/legacy/Interface/Squeal.hh"

namespace MAUS {

// TODO (Rogers): If I am worried about bad memory allocation I may not want to
//                pass by value...
//                Need to think about how we handle "non recoverable" (meaning
//                memory problem or something - should exit)
//                I think the python interface done in MAUSEvaluator.hh is more
//                straightforward - worth doing things that way...

/** @class CppErrorHandler
 *  \brief Handler for c++ errors
 *
 *  In MAUS we typically have two sorts of errors - Squeals (which are specific
 *  to MAUS) and std::exceptions (which are more generic). These static
 *  functions are used to handle the errors, by default handing them up to the
 *  PyErrorHandler (See src/common_py/ErrorHandler).
 *
 *  Interface with python is defined by a callback function
 *  HandleExceptionFunction that is set through PyMausCpp.hh python interface.
 */

class CppErrorHandler {

 public:
  /** @brief Call default Cpp exception handler
   *
   * This makes a python exception of type CppError and hands it to the Python
   * error handler.
   *
   *  @param val Json document that will take any error
   *  @param exc the (MAUS Squeal) exception
   *  @param class_name the name of the class that generated the error
   */
  Json::Value HandleSqueal(Json::Value val, Squeal exc, std::string class_name);

  /** @brief Call default Cpp exception handler
   *
   * This makes a python exception of type CppError and hands it to the Python
   * error handler. Any Json return values are discarded.
   *
   *  @param exc the (std) exception
   *  @param class_name the name of the class that generated the error
   */
  void HandleSquealNoJson(Squeal exc, std::string class_name);

  /** @brief Call default Cpp exception handler
   *
   * This makes a python exception of type CppError and hands it to the Python
   * error handler.
   *
   *  @param val Json document that will take any error
   *  @param exc the std::exception
   *  @param class_name name of the class that generated the error
   */
  Json::Value HandleStdExc
                  (Json::Value val, std::exception& exc, std::string class_name);

  /** @brief Call default Cpp exception handler when Json not initialised
   *
   * This makes a python exception of type CppError and hands it to the Python
   * error handler. Any Json return values are discarded.
   *
   *  @param exc the std::exception
   *  @param class_name name of the class that generated the error
   */
  void HandleStdExcNoJson(std::exception& exc, std::string class_name);

  /** @brief Set function that is called to pass errors to python
   *
   *  Note that this function INCREFs the input PyObject and DECREFs any
   *  existing PyObject (i.e. CppErrorHandler owns memory).
   */
  void SetPyErrorHandler(PyObject* fn) {
    if (getInstance()->HandleExceptionFunction != NULL)
      Py_XDECREF(getInstance()->HandleExceptionFunction);
    Py_XINCREF(fn);
    getInstance()->HandleExceptionFunction = fn;
  }

  /** @brief Get function that is called to pass errors to python
   */
  PyObject* GetPyErrorHandler() {
    return getInstance()->HandleExceptionFunction;
  }

  static CppErrorHandler* getInstance();

  ~CppErrorHandler();

 private:
  static CppErrorHandler* instance;
  PyObject* HandleExceptionFunction;  // set by callback defined in PyMausCpp

  Json::Value ExceptionToPython
             (std::string what, Json::Value json_value, std::string class_name);

  CppErrorHandler();
};

}  // namespace MAUS

#endif

