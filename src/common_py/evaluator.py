# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#

"""
Workaround for memory leak in CLHEP Evaluator (redmine issue #42)
"""

import copy
import math
import xboa.Common

class Evaluator:
    def __init__(self):
        self.variables = {}
        self.reset()

    def evaluate(self, formula):
        """
        Evaluate a string expression given by formula
        """
        out = eval(formula, {"__builtins__":None}, self.variables)
        return out

    def set_variable(self, name, value):
        """
        Set a variable:value pair

        \item name string giving the variable name
        \item value gives the variable value - should be able to evaluate as a
              float
        """
        self.variables[str(name)] = float(value)

    def reset(self):
        """
        Remove all variables and start from scratch
        """
        self.variables = {}
        self.variables = copy.deepcopy(xboa.Common.units)
        my_math = ['acos', 'acosh', 'asin', 'asinh', 'atan', 'atan2', 'atanh',
                   'ceil', 'copysign', 'cos', 'cosh', 'degrees', 'e', 'erf',
                   'erfc', 'exp', 'expm1', 'fabs', 'factorial', 'floor',
                   'fmod', 'frexp', 'fsum', 'gamma', 'hypot', 'isinf',
                   'isnan', 'ldexp', 'lgamma', 'log', 'log10', 'log1p',
                   'modf', 'pi', 'pow', 'radians', 'sin', 'sinh',
                   'sqrt', 'tan', 'tanh', 'trunc']
        for item in my_math:
            self.variables[item] = math.__dict__[item]


