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

class MapCppTrackerReconTestCase(unittest.TestCase): # pylint: disable = R0904
    """Tests for MapCppTrackerRecon"""

    cfg = json.loads(Configuration.Configuration().getConfigJSON())
    cfg['reconstruction_geometry_filename'] = 'Stage4.dat'
    cfg['SciFiPRHelicalOn'] = 0
    cfg['SciFiStraightOn'] = 0

    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MAUS.MapCppTrackerRecon()
        cls.test_config = ""
        if maus_cpp.globals.has_instance():
            cls.test_config = maus_cpp.globals.get_configuration_cards()

    def testInit(self):
        """Check birth"""
        #success = self.mapper.birth(json.dumps(self.cfg))
        #self.assertTrue(success)

    def testGoodStraightProcess(self):
        """Check that tracker recon process produces expected
           output with good straight track data"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.cfg['reconstruction_geometry_filename'] = 'Stage4.dat'
        self.cfg['SciFiPRHelicalOn'] = 0
        self.cfg['SciFiPRStraightOn'] = 1
        print "Flags passed: "
        print self.cfg['SciFiPRHelicalOn']
        print self.cfg['SciFiPRStraightOn']
        success = self.mapper.birth(json.dumps(self.cfg))
        self.assertTrue(success)
        # Read in a spill of mc data containing 5 straight tracks
        test1 = ('%s/src/map/MapCppTrackerRecon/test_straight_digits.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        # Check the first spill (straights)
        fin.readline()
        fin.readline()
        fin.readline()
        fin.readline()
        fin.readline()
        data = fin.readline()
        result = self.mapper.process(data)
        spill_out = json.loads(result)
        self.assertTrue('recon_events' in spill_out)
        self.assertEqual(5, len(spill_out['recon_events']))
        # Check the first event
        revt = spill_out['recon_events'][0]
        self.assertTrue('sci_fi_event' in revt)
        self.assertTrue('digits' in revt['sci_fi_event'])
        self.assertEqual(32, len(revt['sci_fi_event']['digits']))
        self.assertTrue('clusters' in revt['sci_fi_event'])
        self.assertEqual(30, len(revt['sci_fi_event']['clusters']))
        self.assertTrue('spacepoints' in revt['sci_fi_event'])
        self.assertEqual(10, len(revt['sci_fi_event']['spacepoints']))
        self.assertTrue('straight_pr_tracks' in revt['sci_fi_event'])
        self.assertEqual(2, len(revt['sci_fi_event']['straight_pr_tracks']))
        self.assertTrue('helical_pr_tracks' in revt['sci_fi_event'])
        self.assertEqual(0, len(revt['sci_fi_event']['helical_pr_tracks']))

    def testGoodHelicalProcess(self):
        """Check that tracker recon  process produces expected
        output with good helical track data"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.cfg['reconstruction_geometry_filename'] = 'Stage6.dat'
        self.cfg['SciFiPRHelicalOn'] = 1
        self.cfg['SciFiStraightOn'] = 0
        success = self.mapper.birth(json.dumps(self.cfg))
        self.assertTrue(success)
        # Read in a spill of mc data containing 5 straight tracks
        # test1 = ('%s/src/map/MapCppTrackerRecon/test_helical_digits.json' %
        #          os.environ.get("MAUS_ROOT_DIR"))
        test1 = ('%s/src/map/MapCppTrackerRecon/test_helical_digits.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        # Check the first spill (helices)
        fin.readline()
        fin.readline()
        data = fin.readline()
        result = self.mapper.process(data)
        spill_out = json.loads(result)
        self.assertTrue('recon_events' in spill_out)
        self.assertEqual(1, len(spill_out['recon_events']))
        # Check the first event
        revt = spill_out['recon_events'][0]
        self.assertTrue('digits' in revt['sci_fi_event'])
        self.assertEqual(31, len(revt['sci_fi_event']['digits']))
        self.assertTrue('clusters' in revt['sci_fi_event'])
        self.assertEqual(30, len(revt['sci_fi_event']['clusters']))
        self.assertTrue('spacepoints' in revt['sci_fi_event'])
        self.assertEqual(10, len(revt['sci_fi_event']['spacepoints']))
        self.assertTrue('straight_pr_tracks' in revt['sci_fi_event'])
        self.assertEqual(0, len(revt['sci_fi_event']['straight_pr_tracks']))
        self.assertTrue('helical_pr_tracks' in revt['sci_fi_event'])
        self.assertEqual(2, len(revt['sci_fi_event']['helical_pr_tracks']))

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration,
        and checks that we can death() MapCppTrackerRecon"""
        cls.mapper = MAUS.MapCppTrackerRecon()
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        if cls.test_config != "":
            maus_cpp.globals.birth(cls.test_config)
        # Check we death() the mapper
        success = cls.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        cls.mapper = None

if __name__ == '__main__':
    unittest.main()
