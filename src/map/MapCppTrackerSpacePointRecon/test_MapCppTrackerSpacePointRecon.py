# This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

"""Tests for MapCppTrackerRecon"""

# pylint: disable = C0103

import os
import json
import unittest
import Configuration
import MAUS
import maus_cpp.globals
import maus_cpp.converter
import maus_cpp.mice_module

# pylint: disable = R0904
class MapCppTrackerSpacePointReconTestCase(unittest.TestCase):
    """Tests for MapCppTrackerSpacePointRecon"""

    cfg = json.loads(Configuration.Configuration().getConfigJSON())

    helical_geom = "TrackerTest.dat"
    straight_geom = "TrackerTest_NoField.dat"
    cfg['simulation_geometry_filename'] = helical_geom
    cfg['reconstruction_geometry_filename'] = helical_geom

    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""

        cls.mappers = [MAUS.MapCppTrackerClusterRecon(), \
          MAUS.MapCppTrackerSpacePointRecon()]
        cls.test_config = ""
        if maus_cpp.globals.has_instance():
            cls.test_config = maus_cpp.globals.get_configuration_cards()
            maus_cpp.globals.death()

    def testInit(self):
        """Check birth"""
        #success = self.mapper.birth(json.dumps(self.cfg))
        #self.assertTrue(success)

    def testGoodStraightProcess(self):
        """Check that the mapper process function produces expected
           output with good straight track data"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.cfg['simulation_geometry_filename'] = self.straight_geom
        self.cfg['reconstruction_geometry_filename'] = self.straight_geom
        self.cfg['SciFiPRHelicalOn'] = 0
        self.cfg['SciFiPRStraightOn'] = 1
        maus_cpp.globals.birth(json.dumps(self.cfg))
        maus_cpp.globals.set_reconstruction_mice_modules(
            maus_cpp.mice_module.MiceModule(self.straight_geom))

        for m in self.mappers:
            m.birth(json.dumps(self.cfg))

        # Read in a spill of mc data containing 5 straight tracks
        test1 = \
          ('%s/tests/test_data/scifi_test_data/test_straight_digits.json' %
          os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        fin.readline()
        fin.readline()
        fin.readline()
        data = fin.readline()

        for m in self.mappers:
            data = m.process(data)

        spill_out = maus_cpp.converter.json_repr(data)
        self.assertTrue('recon_events' in spill_out)
        self.assertEqual(1, len(spill_out['recon_events']))
        # Check the first event
        revt = spill_out['recon_events'][0]
        self.assertTrue('sci_fi_event' in revt)
        self.assertTrue('digits' in revt['sci_fi_event'])
        self.assertEqual(31, len(revt['sci_fi_event']['digits']))
        self.assertTrue('clusters' in revt['sci_fi_event'])
        self.assertEqual(30, len(revt['sci_fi_event']['clusters']))
        self.assertTrue('spacepoints' in revt['sci_fi_event'])
        self.assertEqual(10, len(revt['sci_fi_event']['spacepoints']))

        maus_cpp.globals.death()

    def testGoodHelicalProcess(self):
        """Check that the mapper process function produces expected
        output with good helical track data"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.cfg['simulation_geometry_filename'] = self.helical_geom
        self.cfg['reconstruction_geometry_filename'] = self.helical_geom
        self.cfg['SciFiPRHelicalOn'] = 1
        self.cfg['SciFiStraightOn'] = 0

        maus_cpp.globals.birth(json.dumps(self.cfg))
        maus_cpp.globals.set_reconstruction_mice_modules(
            maus_cpp.mice_module.MiceModule(self.helical_geom))

        for m in self.mappers:
            m.birth(json.dumps(self.cfg))

        # Read in a spill of mc data containing 5 straight tracks
        test1 = ('%s/tests/test_data/scifi_test_data/test_helical_digits.json' %
                os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        fin.readline()
        fin.readline()
        data = fin.readline()

        for m in self.mappers:
            data = m.process(data)

        spill_out = maus_cpp.converter.json_repr(data)
        self.assertTrue('recon_events' in spill_out)
        self.assertEqual(1, len(spill_out['recon_events']))
        # Check the first event
        revt = spill_out['recon_events'][0]
        self.assertTrue('digits' in revt['sci_fi_event'])
        self.assertEqual(34, len(revt['sci_fi_event']['digits']))
        self.assertTrue('clusters' in revt['sci_fi_event'])
        self.assertEqual(30, len(revt['sci_fi_event']['clusters']))
        self.assertTrue('spacepoints' in revt['sci_fi_event'])
        self.assertEqual(10, len(revt['sci_fi_event']['spacepoints']))

        maus_cpp.globals.death()

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration,
        and checks that we can death() MapCppTrackerSpacePointRecon"""
        cls.mapper = MAUS.MapCppTrackerSpacePointRecon()
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        if cls.test_config != "":
            maus_cpp.globals.birth(cls.test_config)
        # Check we death() the mapper
        cls.mapper.death()
        cls.mapper = None

if __name__ == '__main__':
    unittest.main()
