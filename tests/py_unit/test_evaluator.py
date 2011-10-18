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

"""Tests for src/common_py/Evaluator.py"""

import unittest

import xboa.Common

import evaluator

class TestEvaluator(unittest.TestCase): #pylint: disable = R0904
    """test evaluator"""

    def test_init(self):
        """Test evaluator initialises with math functions"""
        evaluator_ = evaluator.Evaluator()
        self.assertIn("sin", evaluator_.variables.keys())
        self.assertIn("mm", evaluator_.variables.keys())

    def test_set_variable(self):
        """Test we can set evaluator variables"""
        evaluator_ = evaluator.Evaluator()
        # check we can assign numbers
        evaluator_.set_variable("x", 21.)
        self.assertEqual(evaluator_.variables["x"], 21.)
        # check we throw an error if value can't be overloaded to float 
        self.assertRaises(ValueError, evaluator_.set_variable, "x", "value")

    def test_evaluate(self):
        """Test we can evaluate expressions okay"""
        evaluator_ = evaluator.Evaluator()
        evaluator_.set_variable("x", 21.)
        value = evaluator_.evaluate("cos(pi*x)*m")
        self.assertAlmostEqual(value, -1.*xboa.Common.units["m"])
        value = evaluator_.evaluate("sin(pi*x)")
        self.assertAlmostEqual(value, 0.)
        self.assertRaises(NameError, evaluator_.evaluate, "sin(pi*y)")

        value = evaluator_.evaluate("sin(pi*x)")
        self.assertAlmostEqual(value, 0.)

    def test_reset(self):
        """Test we can reset okay"""
        evaluator_ = evaluator.Evaluator()
        evaluator_.set_variable("x", 21.)
        self.assertAlmostEqual(evaluator_.evaluate("x"), 21.)
        evaluator_.reset()
        self.assertRaises(NameError, evaluator_.evaluate, "x")

if __name__ == '__main__':
    unittest.main()

