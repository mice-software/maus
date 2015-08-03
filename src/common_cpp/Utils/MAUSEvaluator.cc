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

#include <iostream>

#include "Utils/Exception.hh"

#include "src/common_cpp/Utils/MAUSEvaluator.hh"

namespace MAUS {

MAUSEvaluator::MAUSEvaluator() : _evaluator_mod(NULL),
                                 _evaluator(NULL),
                                 _evaluate_func(NULL),
                                 _set_variable_func(NULL)  {
    Py_Initialize();
    this->reset();
}

MAUSEvaluator::~MAUSEvaluator() {
    clear();
}

void MAUSEvaluator::set_variable(std::string name, double value) {
    PyObject *py_arg = NULL, *py_value = NULL;
    // argument to set_variable
    py_arg = Py_BuildValue("(sd)", name.c_str(), value);
    if (py_arg == NULL) {
        PyErr_Clear();
        throw(Exception(Exception::recoverable,
              "Failed to resolve arguments to set_variable",
              "MAUSEvaluator::evaluate"));
    }
    if (_set_variable_func != NULL && PyCallable_Check(_set_variable_func)) {
        // py_value should be None (return from Evaluator.set_variable)
        py_value = PyObject_CallObject(_set_variable_func, py_arg);
    }
    // check for error
    if (py_value == NULL) {
        PyErr_Clear();
        Py_DECREF(py_arg);
        throw(Exception(Exception::recoverable,
                     "Failed to parse variable "+name,
                     "MAUSEvaluator::evaluate"));
    }
    // clean up
    Py_DECREF(py_value);
    Py_DECREF(py_arg);
}

double MAUSEvaluator::evaluate(std::string function) {
    PyErr_Clear();
    PyObject *py_arg(NULL), *py_value(NULL);
    // argument to evalute
    py_arg = Py_BuildValue("(s)", function.c_str());
    if (py_arg == NULL) {
        PyErr_Clear();
        throw(Exception(Exception::recoverable,
                   "Failed to build function "+function,
                   "MAUSEvaluator::evaluate"));
    }
    // run the evaluator to calculate function value
    if (_evaluate_func != NULL && PyCallable_Check(_evaluate_func))  {
        py_value = PyObject_CallObject(_evaluate_func, py_arg);
    }
    if (py_value == NULL) {
        if (PyErr_Occurred())
            PyErr_Print();
        Py_DECREF(py_arg);
        throw(Exception(Exception::recoverable,
                   "MAUS failed to evaluate expression \""+function+"\"",
                   "MAUSEvaluator::evaluate"));
    }

    // now put transform py_value into C++ double
    double value = 0.;
    if (!PyArg_Parse(py_value, "d", &value)) {
        if (PyErr_Occurred())
            PyErr_Print();
        throw(MAUS::Exception(MAUS::Exception::recoverable,
                   "Failed to evaluate expression \""+function+"\"",
                   "MAUSEvaluator::evaluate"));
    }
    // clean up
    Py_DECREF(py_value);
    Py_DECREF(py_arg);
    // return
    return value;
}

void MAUSEvaluator::clear() {
  // clear any memory allocations
  if (_set_variable_func != NULL) {
      Py_DECREF(_set_variable_func);
      _set_variable_func = NULL;
  }
  if (_evaluate_func != NULL) {
      Py_DECREF(_evaluate_func);
      _evaluate_func = NULL;
  }
  if (_evaluator != NULL) {
      Py_DECREF(_evaluator);
      _evaluator = NULL;
  }
  if (_evaluator_mod != NULL) {
      Py_DECREF(_evaluator_mod);
      _evaluator_mod = NULL;
  }
}

void MAUSEvaluator::reset() {
  // check that we don't have anything allocated already
  clear();
  // NOTE: I don't throw MAUS::Exceptions here because I'm nervous about
  // set up/tear down order
  // initialise evaluator module
  _evaluator_mod = PyImport_ImportModule("evaluator");
  if (_evaluator_mod == NULL) {
    std::cerr << "Failed to import evaluator module" << std::endl;
    return;
  }

  // initialise an evaluator object
  // evaluator_mod_dict is evaluator.__dict__ is a borrowed reference
  PyObject* evaluator_mod_dict = PyModule_GetDict(_evaluator_mod);
  if (evaluator_mod_dict != NULL) {
    // evaluator_init is a Evaluator.__init__ borrowed reference
    PyObject* evaluator_init = PyDict_GetItemString
                                              (evaluator_mod_dict, "Evaluator");
    if (PyCallable_Check(evaluator_init)) {
        _evaluator = PyObject_Call(evaluator_init, NULL, NULL);
    }
  }
  if (_evaluator == NULL) {
    clear();
    std::cerr << "Failed to instantiate evaluator" << std::endl;
    return;
  }

  // get the evaluator object evaluate() function
  _evaluate_func = PyObject_GetAttrString(_evaluator, "evaluate");
  if (_evaluate_func == NULL) {
    clear();
    std::cerr << "Failed to find evaluate function" << std::endl;
    return;
  }

  // get the evaluator object set_variable()() function
  _set_variable_func = PyObject_GetAttrString(_evaluator, "set_variable");
  if (_set_variable_func == NULL) {
    clear();
    std::cerr << "Failed to find set_variables function" << std::endl;
    return;
  }
}

} // namespace MAUS
