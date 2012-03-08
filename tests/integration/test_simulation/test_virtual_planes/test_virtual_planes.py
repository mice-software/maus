#!/usr/bin/env python2.7

import unittest
import os
import subprocess
import xboa.Bunch

def run_simulation(geometry):
    mrd = os.environ["MAUS_ROOT_DIR"]
    sim_file = os.path.join(mrd,
      "tests/integration/test_simulation/test_virtual_planes/",
      geometry
    )
    output_json = os.path.join(mrd, "tmp/test_virtual_planes_"+geometry+".json")

    simulation = os.path.join(mrd, "bin/simulate_mice.py")
    call_options = [simulation,
                    "-simulation_geometry_filename", sim_file,
                    "-output_json_file_name", output_json
                   ]

    log = open(
            os.path.join(mrd, "tmp/test_virtual_planes_"+geometry+".log"), 'w'
          )    
    proc = subprocess.Popen(call_options, stdout=log, stderr=subprocess.STDOUT)
    proc.wait()
    return output_json

class VirtualPlaneTestCase(unittest.TestCase):
    def test_default_virtual(self):
        filename = run_simulation("VirtualPlanesTestDefaults.dat")
        virts = xboa.Bunch.Bunch.new_from_read_builtin\
                                          ("maus_virtual_hit", filename)
        print virts
        self.assertEqual(len(virts.hits()), 2)
        self.assertEqual(virts[0]['z'], 100.)
        self.assertEqual(virts[1]['x'], 101.)

if __name__ == '__main__':
    unittest.main()
