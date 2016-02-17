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

"""Tests for MapCppTOFSpacePoints"""

#pylint: disable = C0103

import os
import json
import unittest
from Configuration import Configuration
import maus_cpp.converter
import MAUS

class MapCppTOFSpacePointsTestCase(unittest.TestCase): #pylint: disable = R0904
    """Tests for MapCppTOFSpacePoints"""

    @classmethod
    def setUpClass(cls): #pylint: disable = C0103
        """Construct a MapCppTOFSpacePoints"""
        cls.mapper = MAUS.MapCppTOFSpacePoints()
        cls.c = Configuration()

    def test_empty(self):
        """Check against configuration is empty"""
        self.assertRaises(ValueError, self.mapper.birth, "")
        result = self.mapper.process("")
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppTOFSpacePoints" in doc["errors"])

    def test_init(self):
        """Check can handle empty spill"""
        test_configuration = self.c.getConfigJSON()
        test_conf_json = json.loads(test_configuration)
        test_conf_json['Enable_t0_correction'] = False
        test_conf_json['Enable_triggerDelay_correction'] = False
        test_conf_json['Enable_timeWalk_correction'] = False
        test_conf_json['TOF_findTriggerPixelCut'] = 1.001
        test_conf_json['TOF_makeSpacePointCut'] = 1.001
        #print testConfJSON
        test_configuration = json.dumps(test_conf_json)
        self.mapper.birth(test_configuration)

    def test_no_data(self):
        """Check for case where we have no data"""
        test1 = ('%s/src/map/MapCppTOFSpacePoints/noDataTest.txt' %
                                                os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertFalse('tof_space_points' in spill_out)

    def __get_space_point(self, spill, tof, event): # pylint: disable=R0201
        """Return the space point for given tof and particle event"""
        return spill['recon_events'][event]['tof_event']\
                                            ['tof_space_points']['tof'+str(tof)]

    @unittest.skip("Disabled pending adjusting for new TOF JSON Processor")
    def __test_process_tof_0(self, spill_out):
        """Check the MapCppTOFSpacePoints process function for tof0"""
        self.assertFalse(self.__get_space_point(spill_out, 0, 0))
        self.assertFalse(self.__get_space_point(spill_out, 0, 2))
        self.assertFalse(self.__get_space_point(spill_out, 0, 3))
        part_ev1_tof0 = self.__get_space_point(spill_out, 0, 1)
        self.assertEqual(len(part_ev1_tof0), 2)
        # test the calculations
        sp_time = (-11.025 -10.075 -11.025 -11.075)/4.
        sp_time_recon = part_ev1_tof0[0]['time']
        sp_charge = 5+6
        sp_charge_recon = part_ev1_tof0[0]['charge']
        sp_charge_prod = 2+3
        sp_charge_prod_recon = part_ev1_tof0[0]['charge_product']
        sp_dt = (-11.025 -10.075)/2. - (-11.025 -11.075)/2.
        sp_dt_recon = part_ev1_tof0[0]['dt']
        self.assertEqual(sp_time, sp_time_recon)
        self.assertEqual(sp_dt, sp_dt_recon)
        self.assertEqual(sp_charge, sp_charge_recon)
        self.assertEqual(sp_charge_prod, sp_charge_prod_recon)

    def __test_process_tof_1(self, spill_out):
        """Check the MapCppTOFSpacePoints process function for tof1"""
        self.assertFalse(self.__get_space_point(spill_out, 1, 2))
        self.assertFalse(self.__get_space_point(spill_out, 1, 3))
        part_ev0_tof1 = self.__get_space_point(spill_out, 1, 0)
        part_ev1_tof1 = self.__get_space_point(spill_out, 1, 1)
        self.assertEqual(len(part_ev0_tof1), 2)
        self.assertEqual(len(part_ev1_tof1), 1)

    def __test_process_tof_2(self, spill_out):
        """Check the MapCppTOFSpacePoints process function for tof2"""
        self.assertFalse(self.__get_space_point(spill_out, 2, 0))
        self.assertFalse(self.__get_space_point(spill_out, 2, 1))
        self.assertFalse(self.__get_space_point(spill_out, 2, 3))
        part_ev2_tof2 = self.__get_space_point(spill_out, 2, 2)
        self.assertEqual(len(part_ev2_tof2), 1)

    def test_process(self):
        """Check the MapCppTOFSpacePoints process function"""
        test2 = ('%s/src/map/MapCppTOFSpacePoints/processTest.json' %
                                                os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2, 'r')
        data = fin.read()
        # test with some crazy events.
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        # test the output for each tof
        self.__test_process_tof_0(spill_out)
        self.__test_process_tof_1(spill_out)
        self.__test_process_tof_2(spill_out)

    @classmethod
    def tearDownClass(cls): #pylint: disable = C0103
        cls.mapper.death()
        cls.mapper = None

if __name__ == '__main__':
    unittest.main()
