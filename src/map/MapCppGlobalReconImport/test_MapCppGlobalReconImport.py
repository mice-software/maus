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

"""Tests for MapCppGlobalReconImport"""

# pylint: disable = C0103
# pylint: disable = W0611

import os
import json
import unittest
import Configuration
import maus_cpp.converter
from _MapCppGlobalReconImport import MapCppGlobalReconImport

class MapCppGlobalImportTestCase(unittest.TestCase): # pylint: disable = R0904
    """Tests for MapCppGlobalReconImport"""
    config = json.loads(Configuration.Configuration().getConfigJSON())
    
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MapCppGlobalReconImport()

    def test_fill_Global_Event(self):
        """Check that process fills global events from detector data"""
        path_to_geom = ('%s/files/geometry/download/ParentGeometryFile.dat' %
                 os.environ.get("MAUS_ROOT_DIR"))
        self.config['simulation_geometry_filename'] = path_to_geom
        test5 = ('%s/src/map/MapCppGlobalReconImport/global_import_test.json' %
                 os.environ.get("MAUS_ROOT_DIR"))
        self.mapper.birth(json.dumps(self.config))
        fin = open(test5,'r')
        line = fin.read()
        result = self.mapper.process(line)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertTrue('recon_events' in spill_out)
        revtarray = spill_out['recon_events']
        self.assertEqual(1, len(revtarray))
        revt = revtarray[0]
        self.assertTrue('global_event' in revt)
        self.assertTrue('track_points' in revt['global_event'])
        self.assertEqual(36, len(revt['global_event']['track_points']))
        for i in revt['global_event']['track_points']:
            self.assertTrue('mapper_name' in i)
            self.assertEqual(i['mapper_name'],'MapCppGlobalReconImport')
        self.assertTrue('tracks' in revt['global_event'])
        self.assertEqual(3, len(revt['global_event']['tracks']))
        for i in revt['global_event']['tracks']:
            self.assertTrue('mapper_name' in i)
            self.assertEqual(i['mapper_name'],'MapCppGlobalReconImport')
        self.assertTrue('space_points' in revt['global_event'])
        self.assertEqual(40, len(revt['global_event']['space_points']))
        self.assertTrue('primary_chains' in revt['global_event'])
        self.assertEqual(0, len(revt['global_event']['primary_chains']))

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that we can death() MapCppGlobalReconImport"""
        cls.mapper.death()

if __name__ == '__main__':
    unittest.main()
