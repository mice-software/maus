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

#include <Python.h>

#include <map>
#include <string>

namespace MAUS {

/** Wrapper class for ROOT TFormula to make it look like a CLHEP Evaluator
 *
 *  Workaround for memory leak in CLHEP Evaluator (redmine issue #42)
 *
 */
class MAUSEvaluator {
  public:
    MAUSEvaluator();

    ~MAUSEvaluator();

    void set_variable(std::string name, double value);

    double evaluate(std::string function);

    void reset();

    void free();

  private:
    std::map<std::string, double> _parameters;
    PyObject* _evaluator;
    PyObject* _evaluate_func;
    PyObject* _set_variable_func;
};

} // namespace MAUS

#endif

