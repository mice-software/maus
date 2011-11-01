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
import unittest
from Configuration import Configuration
import MAUS

from MapCppTOFSpacePoints import MapCppTOFSpacePoints

class MapCppTOFSpacePointsTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.mapper = MAUS.MapCppTOFSpacePoints()
        self.c = Configuration()

    def test_empty(self):
        result = self.mapper.birth("")
        self.assertFalse(result)
        result = self.mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_init(self):
        testConfiguration = self.c.getConfigJSON()
        testConfJSON = json.loads(testConfiguration)
        testConfJSON['Enable_t0_correction'] = False
        testConfJSON['Enable_triggerDelay_correction'] = False
        testConfJSON['Enable_timeWalk_correction'] = False
        testConfJSON['TOF_findTriggerPixelCut'] = 1.001
        testConfJSON['TOF_makeSpacePiontCut'] = 1.001
        #print testConfJSON
        testConfiguration = json.dumps(testConfJSON)
        success = self.mapper.birth(testConfiguration)
        #success = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(success)

    def test_no_data(self):
        test1 = '%s/src/map/MapCppTOFSpacePoints/noDataTest.txt' %os.environ.get("MAUS_ROOT_DIR")
        f = open(test1,'r')
        data = f.read()
        # test with no data.
        result = self.mapper.process(data)
        spill_out = json.loads(result)
        self.assertFalse('space_points' in spill_out)

    def test_process(self):
        test2 = '%s/src/map/MapCppTOFSpacePoints/processTest.txt' %os.environ.get("MAUS_ROOT_DIR")
        f = open(test2,'r')
        data = f.read()
        # test with some crazy events.
        result = self.mapper.process(data)
        spill_in = json.loads(data)
        spill_out = json.loads(result)
        # test the tof0 output
        n_part_events = len(spill_out['space_points']['tof0'])
        self.assertEqual(n_part_events,4)
        self.assertFalse(spill_out['space_points']['tof0'][0])
        self.assertFalse(spill_out['space_points']['tof0'][2])
        self.assertFalse(spill_out['space_points']['tof0'][3])
        partEv1_tof0 = spill_out['space_points']['tof0'][1]
        self.assertEqual(len(partEv1_tof0),2)
        # test the calculations
        sp_time = (-11.025 -10.075 -11.025 -11.075)/4.
        sp_time_recon = partEv1_tof0[0]['time']
        sp_charge = 5+6
        sp_charge_recon = partEv1_tof0[0]['charge']
        sp_charge_prod = 2+3
        sp_charge_prod_recon = partEv1_tof0[0]['charge_product']
        sp_dt = (-11.025 -10.075)/2. - (-11.025 -11.075)/2.
        sp_dt_recon = partEv1_tof0[0]['dt']
        self.assertEqual(sp_time,sp_time_recon)
        self.assertEqual(sp_dt,sp_dt_recon)
        self.assertEqual(sp_charge,sp_charge_recon)
        self.assertEqual(sp_charge_prod,sp_charge_prod_recon)
        # test the tof1 output
        n_part_events = len(spill_out['space_points']['tof1'])
        self.assertEqual(n_part_events,4)
        self.assertFalse(spill_out['space_points']['tof1'][2])
        self.assertFalse(spill_out['space_points']['tof1'][3])
        partEv0_tof1 = spill_out['space_points']['tof1'][0]
        partEv1_tof1 = spill_out['space_points']['tof1'][1]
        self.assertEqual(len(partEv0_tof1),2)
        self.assertEqual(len(partEv1_tof1),1)
        # test the tof2 output
        n_part_events = len(spill_out['space_points']['tof2'])
        self.assertEqual(n_part_events,4)
        self.assertFalse(spill_out['space_points']['tof2'][0])
        self.assertFalse(spill_out['space_points']['tof2'][1])
        self.assertFalse(spill_out['space_points']['tof2'][3])
        partEv2_tof2 = spill_out['space_points']['tof2'][2]
        self.assertEqual(len(partEv2_tof2),1)

    @classmethod
    def tearDownClass(self):
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()