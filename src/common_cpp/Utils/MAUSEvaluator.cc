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

#include <Python.h>

#include <iostream>

#include "src/legacy/Interface/Squeal.hh"

#include "src/common_cpp/Utils/MAUSEvaluator.hh"

namespace MAUS {

MAUSEvaluator::MAUSEvaluator() : _evaluator(NULL),
                                 _evaluate_func(NULL),
                                 _set_variable_func(NULL)  {
  Py_Initialize();
  reset();
}

MAUSEvaluator::~MAUSEvaluator() {
  free();
}

void MAUSEvaluator::set_variable(std::string name, double value) {
    bool success = false;
    PyObject* py_arg = Py_BuildValue("(sd)", name.c_str(), value);
    if (py_arg != NULL) {
        if (PyCallable_Check(_set_variable_func)) {
            PyObject* py_value = PyObject_CallObject(_set_variable_func,
                                                     py_arg);
            success = py_value != NULL;
            if (py_value != NULL) {
                Py_DECREF(py_value);
            }
        }
    }
    Py_DECREF(py_arg);
    if (!success) {
      PyErr_Print();
      throw(Squeal(Squeal::recoverable, "Failed to parse variable "+name,
                   "MAUSEvaluator::evaluate"));
    }
}

double MAUSEvaluator::evaluate(std::string function) {
  bool success = false;
  double value = 0.;
  PyObject* py_arg = Py_BuildValue("(s)", function.c_str());
  if (py_arg != NULL) {
    if (PyCallable_Check(_evaluate_func))  {
        PyObject* py_value = PyObject_CallObject(_evaluate_func, py_arg);
        if (py_value != NULL) {
          PyArg_Parse(py_value, "d", &value);
          success = true;
          Py_DECREF(py_value);
        }
    }
  }
  Py_DECREF(py_arg);
  if (!success) {
    PyErr_Print();
    throw(Squeal(Squeal::recoverable, "Failed to parse function "+function,
                 "MAUSEvaluator::evaluate"));
  }

  return value;
}

void MAUSEvaluator::free() {
  if (_evaluator != NULL) {
      Py_DECREF(_evaluator);
      _evaluator = NULL;
  }
  if (_evaluate_func != NULL) {
      Py_DECREF(_evaluate_func);
      _evaluate_func = NULL;
  }
  if (_set_variable_func != NULL) {
      Py_DECREF(_set_variable_func);
      _set_variable_func = NULL;
  }
  _parameters = std::map<std::string, double>();
}

void MAUSEvaluator::reset() {
  free();

  PyObject* evaluator_mod = PyImport_ImportModule("evaluator");
  if (evaluator_mod != NULL) {
    // evaluator_mod_dict is a borrowed reference
    PyObject* evaluator_mod_dict = PyModule_GetDict(evaluator_mod);
    if (evaluator_mod_dict != NULL) {
      // evaluator_init is a borrowed reference
      PyObject* evaluator_init = PyDict_GetItemString(evaluator_mod_dict, "Evaluator");
      if (PyCallable_Check(evaluator_init))  {
          _evaluator = PyObject_CallObject(evaluator_init, NULL);
      }
    }
    Py_DECREF(evaluator_mod);
  }
  if (_evaluator == NULL) {
    throw Squeal(Squeal::recoverable,
        "Failed to import expression evaluator",
        "MAUSEvaluator::MAUSEvaluator");
  }

  _evaluate_func = PyObject_GetAttrString(_evaluator, "evaluate");
  if (_evaluate_func == NULL) {
    Py_DECREF(_evaluator);
    throw Squeal(Squeal::recoverable,
        "Failed to import expression evaluator",
        "MAUSEvaluator::MAUSEvaluator");
  }

  _set_variable_func = PyObject_GetAttrString(_evaluator, "set_variable");
  if (_set_variable_func == NULL) {
    Py_DECREF(_evaluate_func);
    Py_DECREF(_evaluator);
    throw Squeal(Squeal::recoverable,
        "Failed to import expression evaluator",
        "MAUSEvaluator::MAUSEvaluator");
  }
}

} // namespace MAUS
