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

import unittest
import os
import subprocess
import xboa.Bunch

def run_simulation(geometry, configuration):
    """
    Run the simulation taking geometry file, output filename and simulation file
    as args
    """
    name = geometry.split('.')[0]
    mrd = os.environ["MAUS_ROOT_DIR"]
    sim_file = os.path.join(mrd,
      "tests/integration/test_simulation/test_virtual_planes/",
      geometry
    )
    config_file = os.path.join(mrd,
      "tests/integration/test_simulation/test_virtual_planes/",
      configuration
    ) 
    output_json = os.path.join(mrd, "tmp/test_virtual_planes_"+name+".json")

    simulation = os.path.join(mrd, "bin/simulate_mice.py")
    call_options = [simulation,
                    "-configuration_file", config_file,
                    "-simulation_geometry_filename", sim_file,
                    "-output_json_file_name", output_json,
                    "-verbose_level", "1",
                   ]

    log = open(
            os.path.join(mrd, "tmp/test_virtual_planes_"+name+".log"), 'w'
          )
    proc = subprocess.Popen(call_options, stdout=log, stderr=subprocess.STDOUT)
    proc.wait()
    return output_json

class VirtualPlaneTestCase(unittest.TestCase):
    def test_virtual_defaults(self):
        """
        Check that we readout virtual hits at the correct z position (and not
        for primaries that don't track across the virtual plane
        """
        filename = run_simulation("VirtualPlanesTestDefaults.dat",
                                  "defaults.config")
        virts = xboa.Bunch.Bunch.new_from_read_builtin\
                                          ("maus_virtual_hit", filename)
        self.assertEqual(len(virts.hits()), 3)
        self.assertAlmostEqual(virts[0]['z'], 100.)
        self.assertLess(virts[1]['pz'], 0.) # allow backwards defaults to true
        self.assertAlmostEqual(virts[2]['x'], 101.)

    def test_virtual_options(self):
        """
        Check that the various options on the virtual hit work okay
        """
        filename = run_simulation("VirtualPlanesTestOptions.dat",
                                  "options.config")
        virts = xboa.Bunch.Bunch.new_from_read_builtin\
                                          ("maus_virtual_hit", filename)
        self.assertEqual(len(virts.hits()), 1)
        self.assertAlmostEqual(virts[0]['z'], 100.)

    def test_virtual_indie_u(self):
        """
        Check that independent variable u can be set okay
        """
        filename = run_simulation("VirtualPlanesTestIndieU.dat",
                                  "indie.config")
        virts = xboa.Bunch.Bunch.new_from_read_builtin\
                                          ("maus_virtual_hit", filename)
        self.assertEqual(len(virts.hits()), 1)
        self.assertAlmostEqual(virts[0]['z'], 80.)
        self.assertAlmostEqual(virts[0]['x'], -10.)

    def test_virtual_indie_t(self):
        """
        Check that independent variable t can be set okay
        """
        filename = run_simulation("VirtualPlanesTestIndieT.dat",
                                  "indie.config")
        virts = xboa.Bunch.Bunch.new_from_read_builtin\
                                          ("maus_virtual_hit", filename)
        self.assertEqual(len(virts.hits()), 1)
        self.assertAlmostEqual(virts[0]['t'], 10.)

    def test_virtual_indie_tau(self):
        """
        Check that independent variable tau can be set okay
        """
        filename = run_simulation("VirtualPlanesTestIndieTau.dat",
                                  "indie.config")
        virts = xboa.Bunch.Bunch.new_from_read_builtin\
                                          ("maus_virtual_hit", filename)
        self.assertEqual(len(virts.hits()), 2)
        self.assertAlmostEqual(virts[0]['proper_time'], virts[1]['proper_time'])

if __name__ == '__main__':
    unittest.main()
