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

import os
import json
import types
import unittest

from Configuration import Configuration

import MAUS

class MapPyScalersAnalysisTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        self._datapath = '%s/src/input/InputCppDAQData' % \
                            os.environ.get("MAUS_ROOT_DIR")
        self._datafile = '02873'
        self._c = Configuration()

    def test_something(self):
        inputter = MAUS.InputCppDAQData(self._datapath, self._datafile)
        inputter.birth( self._c.getConfigJSON() )

        mapper = MAUS.MapPyScalersAnalysis()
        success = mapper.birth("{}")
        self.assertTrue(success)

        success = mapper.add("")
        self.assertFalse(success)

        success = mapper.add("{}")
        self.assertFalse(success)

        success = mapper.dump()
        self.assertTrue(success)

        result = mapper.process("{}")
        self.assertEqual(len(result), 2)

        mapper.death()

if __name__ == '__main__':
    unittest.main()