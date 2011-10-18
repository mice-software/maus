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

#ifndef SRC_COMMON_CPP_UTILS_MAUSEVALUATOR
#define SRC_COMMON_CPP_UTILS_MAUSEVALUATOR

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

#include <map>
#include <string>

namespace MAUS {

/** Wrapper class for python evaluator to evaluate expressions
 *
 *  Workaround for memory leak in CLHEP Evaluator (redmine issue #42)
 *
 *  Note - really doesn't like being initialised statically; will make
 *  segmentation fault in the destructor - I guess python clean up can happen
 *  first. Took a while to figure that out!
 */
class MAUSEvaluator {
  public:
    /** Initialise python and call reset() to set up parameters
     */
    MAUSEvaluator();

    /** Calls clear() to Py_DECREF any python objects (and set to NULL)
     */
    ~MAUSEvaluator();

    /** Set a variable for evaluation in expressions
     *
     *  Enables to add a symbol that can be used when evaluating expressions.
     *  The python interpreter will substitute <value> for <name>. Throw a
     *  Squeal if the name was not valid.
     */
    void set_variable(std::string name, double value);

    /** Evaluate the expression
     *
     *  Evaluate function. Any python standard operators are allowed in addition
     *  to the common functions in python math module. Also xboa units are
     *  allowed. However, all other python global expressions have been 
     *  disabled.
     */
    double evaluate(std::string function);

    /** Clear the evaluator and re-initialise
     *
     *  Initialises with python math functions and xboa units in the variables
     *  table. Clears any other variables that have been added with e.g.
     *  set_variable function
     */
    void reset();

  private:
    void clear();

    PyObject* _evaluator_mod;
    PyObject* _evaluator;
    PyObject* _evaluate_func;
    PyObject* _set_variable_func;
};

} // namespace MAUS

#endif

