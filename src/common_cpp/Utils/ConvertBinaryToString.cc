/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#include <string>
#include <cstring>
#include <iostream>

#include "Utils/Exception.hh"

#include "src/common_cpp/Utils/ConvertBinaryToString.hh"

namespace MAUS {
namespace Utils {

ConvertBinaryToString::ConvertBinaryToString() :  _convert_func(NULL),
      _utils_mod(NULL), _utils_mod_dict(NULL) {
    Py_Initialize();
    reset();
}

ConvertBinaryToString::~ConvertBinaryToString() {
    clear();
}

void ConvertBinaryToString::clear() {
    // clear any memory allocations
    if (_utils_mod != NULL) {
        Py_DECREF(_utils_mod);
        _utils_mod = NULL;
    }
    if (_utils_mod_dict != NULL) {
        Py_DECREF(_utils_mod_dict);
        _utils_mod_dict = NULL;
    }
    if (_convert_func != NULL) {
        Py_DECREF(_convert_func);
        _convert_func = NULL;
    }
}

void ConvertBinaryToString::reset() {
    // check that we don't have anything allocated already
    clear();
    // import framework.utilities and handle any errors
    _utils_mod = PyImport_ImportModule("framework.utilities");
    if (_utils_mod == NULL) {
        PyErr_PrintEx(1);
        throw(Exceptions::Exception(Exceptions::recoverable,
                      "Failed to import framework.utilities module",
                      "Utils::ConvertBinaryToString::reset()"));
    }
    // get framework.utilities.__dict__ which is a dict of all elements in
    // framework.utilities and handle any errors
    _utils_mod_dict = PyModule_GetDict(_utils_mod);
    if (_utils_mod_dict == NULL) {
        PyErr_PrintEx(1);
        throw(Exceptions::Exception(Exceptions::recoverable,
                      "Failed to find framework.utilities.__dict__",
                      "Utils::ConvertBinaryToString::reset()"));
    }

    // get convert_binary_to_string from the dict and handle any errors
    _convert_func = PyDict_GetItemString
                                   (_utils_mod_dict, "convert_binary_to_string");
    Py_INCREF(_convert_func); // Apparently PyDict_GetItemString does not INCREF
    if (_convert_func == NULL || !PyCallable_Check(_convert_func)) {
        PyErr_PrintEx(1);
        throw(Exceptions::Exception(Exceptions::recoverable,
                      "Failed to find convert_binary_to_string",
                      "Utils::ConvertBinaryToString::reset()"));
    }
}

std::string ConvertBinaryToString::convert
                                     (std::string file_name, bool delete_file) {
    // build input values and check for errors
    // nb int(bool) does always return 0 or 1, I checked C++ standard
    PyObject* py_arg = Py_BuildValue("(si)", file_name.c_str(),
                                     static_cast<int>(delete_file));
    if (py_arg == NULL) {
        PyErr_Clear();
        PyErr_PrintEx(1);
        throw(Exceptions::Exception(Exceptions::recoverable,
                   "Failed to build value",
                   "ConvertBinaryToString::convert"));
    }

    // check that _convert_func is valid; run the evaluator to calculate
    // function value; check for errors
    if (_convert_func == NULL || PyCallable_Check(_convert_func) == 0)  {
        Py_DECREF(py_arg);
        throw(Exceptions::Exception(Exceptions::recoverable,
                   "Failed to get converter function",
                   "ConvertBinaryToString::convert"));
    }

    // call convert_func; check for errors
    PyObject* py_value = PyObject_CallObject(_convert_func, py_arg);
    if (py_value == NULL) {
        PyErr_Clear();
        Py_DECREF(py_arg);
        PyErr_PrintEx(1);
        throw(Exceptions::Exception(Exceptions::recoverable,
                   "Failed to convert binary \""+file_name+"\"",
                   "ConvertBinaryToString::convert"));
    }
    char * my_cstr = NULL;
    PyArg_Parse(py_value, "s", &my_cstr);
    std::string string_out(my_cstr);

    // clean up
    Py_DECREF(py_value);
    Py_DECREF(py_arg);

    // return
    return string_out;
}
}
}

