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

/** @class CppErrorHandler
 *  \brief Handler for c plus plus errors
 *
 *  In MAUS we typically have two sorts of errors - Squeals (which are specific
 *  to MAUS) and std::exceptions (which are more generic). In both cases we want
 *  to catch the error and handle it in some user-defined way; probably
 *  eventually handing the error up to python error handler.
 */

#include <string>

#include "json/json.h"
#include "Python.h"

#include "src/legacy/Interface/Squeal.hh"

// TODO (Rogers): If I am worried about bad memory allocation I may not want to
//                pass by value...
//                Also would be nice to raise to python error handler (common
//                interface)
//                Need to think about how we handle "non recoverable" (meaning
//                memory leak in the Squeal)

class CppErrorHandler {

 public:
  /** @brief Call default Cpp exception handler
   *
   *  For now this just calls Squeal::Print(), but at some point I intend to
   *  do something more sophisticated (mimicing or even using PyErrorHandler 
   *  functionality).
   *
   *  @param val Json document that will take any error
   *  @param exc the (MAUS Squeal) exception
   */
  static Json::Value HandleSqueal
                          (Json::Value val, Squeal exc, std::string class_name);

  /** @brief Call default Cpp exception handler
   *
   *  For now this just calls Squeal::Print(), but at some point I intend to
   *  do something more sophisticated (mimicing or even using PyErrorHandler 
   *  functionality).
   *
   *  @param exc the (MAUS Squeal) exception
   */
  static void HandleSquealNoJson(Squeal exc, std::string class_name);

  /** @brief Call default Cpp exception handler
   *
   *  For now this just prints std::exception::what(), but at some point I
   *  intend to do something more sophisticated.
   *
   *  @param val Json document that will take any error
   *  @param exc the std::exception
   */
  static Json::Value HandleStdExc
                  (Json::Value val, std::exception exc, std::string class_name);

  /** @brief Call default Cpp exception handler when Json not initialised
   *
   *  For now this just prints std::exception::what(), but at some point I
   *  intend to do something more sophisticated.
   *
   *  @param exc the std::exception
   */
  static void HandleStdExcNoJson(std::exception exc, std::string class_name);

  /** @brief Set function that is called to pass errors to python
   */
  static void SetPyErrorHandler(PyObject* fn) {
    getInstance()->HandleExceptionFunction = fn;
  }

  /** @brief Get function that is called to pass errors to python
   */
  static PyObject* GetPyErrorHandler() {
    return getInstance()->HandleExceptionFunction;
  }


 private:
  static CppErrorHandler* getInstance();
  static CppErrorHandler* instance;
  PyObject* HandleExceptionFunction;

  Json::Value ExceptionToPython
             (std::string what, Json::Value json_value, std::string class_name);

  CppErrorHandler();
  ~CppErrorHandler();
};

#endif

