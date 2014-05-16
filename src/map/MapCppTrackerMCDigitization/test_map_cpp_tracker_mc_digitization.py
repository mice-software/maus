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

"""
Test for MapCppTrackerMCDigitization
"""
# pylint: disable=C0103
# pylint: disable=R0904
# pylint: disable=R0201

import json
import unittest
import os
from Configuration import Configuration
import maus_cpp.converter

from MAUS import MapCppTrackerMCDigitization

class MapCppTrackerMCDigitizationTestCase(unittest.TestCase):
    """ The MapCppTrackerRecon test.
    Member functions are:

    - bool birth(std::string argJsonConfigDocument);
    - bool death();
    - std::string process(std::string document);
    - void digitization(SciFiSpill &spill, Json::Value &root);
    - void fill_digits_vector(Json::Value &digits, SciFiSpill &a_spill);
    - void cluster_recon(SciFiEvent &evt);
    - void spacepoint_recon(SciFiEvent &evt);
    - void save_to_json(SciFiEvent &evt);
    - void print_event_info(SciFiEvent &event);
    Functions added for testing purpose only:
    - JsonToString(Json::Value json_in);
    - ConvertToJson(std::string jsonString);

    The functions listed above are not elementar; the rely upon calls
    to other functions, for which we have cpp unit tests
    (check the tests/cpp_unit/Recon/ directory).

    This python test will work over the process
    """
    @classmethod
    def setUpClass(self):  # pylint: disable-msg=C0103, C0202
        """ Class Initializer.
            The set up is called before each test function
            is called.
        """
        self.mapper = MapCppTrackerMCDigitization()
        conf = json.loads(Configuration().getConfigJSON())
        geom = os.getenv("MAUS_ROOT_DIR")+"/src/map/"+\
               "MapCppTrackerMCDigitization/MapCppTrackerMCDigitizationTest.dat"
        conf["simulation_geometry_filename"] = geom
        # Test whether the configuration files were loaded correctly at birth
        self.mapper.birth(json.dumps(conf))

    def test_death(self):
        """ Test to make sure death occurs """
        self.mapper.death()

    def test_process(self):
        """ Test of the process function """
        root_dir = os.environ.get("MAUS_ROOT_DIR")
        assert root_dir != None
        assert os.path.isdir(root_dir)
        _filename = \
        '%s/src/map/MapCppTrackerMCDigitization/maus_output.json' % root_dir
        assert os.path.isfile(_filename)
        _file = open(_filename, 'r')
        # First line is just info, use to check we get errors
        line_1 = _file.readline().rstrip()
        output_1 = self.mapper.process(line_1)
        self.assertTrue("errors" in maus_cpp.converter.json_repr(output_1))
        # Line 2 is run header; throw away
        _file.readline()
        # Line 3 is good data
        line_3 = _file.readline()
        output_3 = self.mapper.process(line_3)
        spill_out = maus_cpp.converter.json_repr(output_3)
        self.assertTrue("recon_events" in spill_out)
        # Check the digits have been made
        revt = spill_out['recon_events'][0]
        self.assertTrue('sci_fi_event' in revt)
        self.assertTrue('digits' in revt['sci_fi_event'])
        print revt['sci_fi_event']['digits']
        self.assertEqual(34, len(revt['sci_fi_event']['digits']))
        # Close file
        _file.close()

    @classmethod
    def tear_down_class(self): # pylint: disable-msg=C0202
        """___"""
        self.mapper.death()

if __name__ == '__main__':
    unittest.main()
