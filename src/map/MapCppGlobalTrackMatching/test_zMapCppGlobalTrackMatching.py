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

"""Tests for MapCppGlobalTrackMatching"""

# pylint: disable = C0103
# File is renamed because of an issue in the testing framework with geometries
# not being reinitialized properly, that makes the test execution-order
# dependent

import os
import json
import unittest
import Configuration
import MAUS
import maus_cpp.converter
from _MapCppGlobalTrackMatching import MapCppGlobalTrackMatching

class MapCppGlobalTMTestCase(unittest.TestCase): # pylint: disable = R0904
    """Tests for MapCppGlobalTrackMatching"""

    cfg = json.loads(Configuration.Configuration().getConfigJSON())
    geom = (os.environ.get("MAUS_ROOT_DIR") +
            "/tests/cpp_unit/Recon/Global/TestGeometries/SimpleBeamline.dat")
    cfg['simulation_geometry_filename'] = geom
    cfg['reconstruction_geometry_filename'] = geom

    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MapCppGlobalTrackMatching()
        cls.test_config = ""
        if maus_cpp.globals.has_instance():
            cls.test_config = maus_cpp.globals.get_configuration_cards()
            maus_cpp.globals.death()
    
    def test_empty(self):
        """Check can handle empty configuration"""
        self.assertRaises(ValueError, self.mapper.birth, "",)
        result = self.mapper.process("")
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppGlobalTrackMatching" in doc["errors"])

    def test_no_data(self):
        """Check that nothing happens in absence of data"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.cfg['simulation_geometry_filename'] = self.geom
        self.cfg['reconstruction_geometry_filename'] = self.geom
        maus_cpp.globals.birth(json.dumps(self.cfg))
        maus_cpp.globals.set_reconstruction_mice_modules(
            maus_cpp.mice_module.MiceModule(self.geom))
        self.mapper.birth(json.dumps(self.cfg))
        test1 = ('%s/src/map/MapCppGlobalTrackMatching/nodata.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertFalse('global_event' in spill_out)
        maus_cpp.globals.death()

    def test_invalid_json_process(self):
        """Check process with an invalid json input"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.cfg['simulation_geometry_filename'] = self.geom
        self.cfg['reconstruction_geometry_filename'] = self.geom
        maus_cpp.globals.birth(json.dumps(self.cfg))
        maus_cpp.globals.set_reconstruction_mice_modules(
            maus_cpp.mice_module.MiceModule(self.geom))
        self.mapper.birth(json.dumps(self.cfg))
        test3 = ('%s/src/map/MapCppGlobalTrackMatching/invalid.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test3,'r')
        data = fin.read()
        result = self.mapper.process(data)
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("MapCppGlobalTrackMatching" in doc["errors"])
        maus_cpp.globals.death()

    def test_fill_Global_Event(self): # pylint: disable = R0914
        """Check that process makes global tracks from TOF and tracker data"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.cfg['simulation_geometry_filename'] = self.geom
        self.cfg['reconstruction_geometry_filename'] = self.geom
        self.cfg['track_matching_pid_hypothesis'] = "kMuPlus"
        maus_cpp.globals.birth(json.dumps(self.cfg))
        maus_cpp.globals.set_reconstruction_mice_modules(
            maus_cpp.mice_module.MiceModule(self.geom))
        self.mapper.birth(json.dumps(self.cfg))
        test4 = ('%s/src/map/MapCppGlobalTrackMatching/testdata.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test4,'r')
        line = fin.read()
        result = self.mapper.process(line)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertEqual(result.GetSpill().GetReconEvents().size(), 1)
        global_event = result.GetSpill().GetReconEvents()[0].GetGlobalEvent()
        self.assertEqual(global_event.get_primary_chains().size(), 3)
        self.assertEqual(global_event.GetThroughPrimaryChains().size(), 1)
        self.assertEqual(global_event.GetUSPrimaryChains().size(), 1)
        self.assertEqual(global_event.GetDSPrimaryChains().size(), 1)
        through_chain = global_event.GetThroughPrimaryChains()[0]
        self.assertEqual(through_chain.GetUSDaughter(), global_event.GetUSPrimaryChains()[0])
        self.assertEqual(through_chain.GetDSDaughter(), global_event.GetDSPrimaryChains()[0])
        self.assertEqual(through_chain.GetMatchedTracks().size(), 1)
        self.assertEqual(through_chain.GetUSDaughter().GetMatchedTracks().size(), 1)
        self.assertEqual(through_chain.GetDSDaughter().GetMatchedTracks().size(), 1)
        constituent_tracks = through_chain.GetMatchedTracks()[0].GetConstituentTracks()
        self.assertEqual(constituent_tracks[0], through_chain.GetUSDaughter().GetMatchedTracks()[0])
        self.assertEqual(constituent_tracks[1], through_chain.GetDSDaughter().GetMatchedTracks()[0])
        self.assertEqual(through_chain.GetMatchedTracks()[0].GetTrackPoints().size(), 7)
        self.assertEqual(through_chain.GetUSDaughter().GetMatchedTracks()[0].GetTrackPoints().size(), 3)
        self.assertEqual(through_chain.GetDSDaughter().GetMatchedTracks()[0].GetTrackPoints().size(), 4)
        maus_cpp.globals.death()

    def test_reset_geometry(self):
        """Attempt at a hack to fix issues with geometry not being reset"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.cfg['simulation_geometry_filename'] = "Test.dat"
        self.cfg['reconstruction_geometry_filename'] = "Test.dat"
        maus_cpp.globals.birth(json.dumps(self.cfg))
        maus_cpp.globals.set_reconstruction_mice_modules(
            maus_cpp.mice_module.MiceModule("Test.dat"))
        maus_cpp.globals.death()

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration,
        and checks that we can death() MapCppGlobalTrackMatching"""
        cls.mapper = MAUS.MapCppGlobalTrackMatching()
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        if cls.test_config != "":
            maus_cpp.globals.birth(cls.test_config)
        else:
            maus_cpp.globals.birth(json.dumps(MapCppGlobalTMTestCase.cfg))
        # Check we death() the mapper
        cls.mapper.death()
        cls.mapper = None

if __name__ == '__main__':
    unittest.main()
