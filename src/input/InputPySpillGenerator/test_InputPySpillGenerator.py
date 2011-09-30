#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

#pylint: disable = R0904, C0103

"""
Tests InputPySpillGenerator
"""

import json
import unittest

from InputPySpillGenerator import InputPySpillGenerator

class InputPySpillGeneratorTestCase(unittest.TestCase):
    """Tests InputPySpillGenerator
    """

    def test_birth_bad(self):
        """test bad input number of spills
        """
        test_conf_1 = unicode('{"spill_generator_number_of_spills":"bob"}')
        test_conf_2 = unicode("{}")
        test_conf_3 = unicode('{"spill_generator_number_of_spills":-1}')
        self.assertFalse(InputPySpillGenerator().birth(test_conf_1))
        self.assertFalse(InputPySpillGenerator().birth(test_conf_2))
        self.assertFalse(InputPySpillGenerator().birth(test_conf_3))
    
    def test_generate100(self):
        """test generate 100 events
        """
        my_input = InputPySpillGenerator()
        test_conf = unicode('{"spill_generator_number_of_spills":100}')
        self.assertTrue(my_input.birth(test_conf))
        n_spills = 0
        for doc in my_input.emitter():
            json_doc = json.loads(doc)
            self.assertTrue(json_doc == {})
            n_spills += 1
        self.assertEqual(n_spills, 100)
        self.assertTrue(my_input.death())

    def test_generate0(self):
        """should be able to generate 0 events (and return nothing)
        """
        my_input = InputPySpillGenerator()
        test_conf = unicode('{"spill_generator_number_of_spills":0}')
        self.assertTrue(my_input.birth(test_conf))
        for doc in my_input.emitter():
            raise RuntimeError("Shouldn't generate documents here: "+str(doc))

if __name__ == '__main__':
    unittest.main()
