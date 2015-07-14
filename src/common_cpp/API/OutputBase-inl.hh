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
#ifndef _SRC_COMMON_CPP_API_OUTPUTBASE_INL_
#define _SRC_COMMON_CPP_API_OUTPUTBASE_INL_

#include <string>
#include "src/common_cpp/API/APIExceptions.hh"
#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {
  OutputBase::OutputBase(const std::string& s) : ModuleBase(s) {}

  OutputBase::OutputBase(const OutputBase& ob) : ModuleBase(ob._classname) {}

  OutputBase::~OutputBase() {}

  PyObject* OutputBase::save_pyobj(PyObject* data_in) {
    bool ret = false;
    try {
      ret = _save(data_in);
    }
    catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    catch (...) {
      throw UnhandledException(_classname);
    }
    if (ret) {
        PyObject* py_true_ = reinterpret_cast<PyObject*>(Py_True);
        Py_INCREF(py_true_);
        return py_true_;
    } else {
        PyObject* py_false_ = reinterpret_cast<PyObject*>(Py_False);
        Py_INCREF(py_false_);
        return py_false_;
    }
  }

}// end of namespace
#endif
