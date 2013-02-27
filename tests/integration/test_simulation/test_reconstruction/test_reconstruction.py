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
"""test_reconstruction.py"""
# import sys
import unittest
import os
import subprocess
import json
import xboa.Common as Common

def run_simulation(output_json):
    """
    Run the simulation taking geometry file, output filename and simulation file
    as args
    """
    mrd = os.environ["MAUS_ROOT_DIR"]
    config_file = os.path.join(mrd,
      "tests/integration/test_simulation/test_reconstruction/config.py"
    )

    simulation = os.path.join(mrd, "bin/simulate_mice.py")
    call_options = [simulation,
                    "-configuration_file", config_file,
                    "-simulation_geometry_filename", "Stage4.dat",
                    "-output_json_file_name", output_json,
                    "-verbose_level", "0",
                   ]

    log = open(
            os.path.join(mrd, "tmp/test_mc_reconstruction.log"), 'w'
          )
    proc = subprocess.Popen(call_options, stdout=log, stderr=subprocess.STDOUT)
    proc.wait()
    return output_json

class TestReconstruction(unittest.TestCase):#pylint: disable =R0904
    """TestReconstruction"""
    def test_tof_reconstruction(self):
        """
        Check that we can reconstruct at each of the relevant detectors
        """
        mrd = os.environ["MAUS_ROOT_DIR"]
        output_json = os.path.join(mrd, "tmp/test_mc_reconstruction.json")
        run_simulation(output_json)
        json_in = open(output_json).readlines()
        det_list = ["tof0", "tof1", "tof2"]
        n_events = 0
        det = {}
        for detector in det_list:
            det[detector] = []
        for spill in json_in:
            spill = json.loads(spill)
            if "mc" in spill:
                n_events += len(spill["mc"])
                for detector in det_list:
                    for sp_list in spill["space_points"][detector]:
                        if type(sp_list) != type(None):
                            det[detector] += [sp["time"] for sp in sp_list]
            else:
                print spill
        for detector in det_list:
            name = detector+"_mc_time-of-flight"
            canvas = Common.make_root_canvas(name)
            hist = Common.make_root_histogram(name, det[detector], 'ns', 100)
            hist.SetStats(True)
            hist.Draw()
            plot_out = os.path.join \
                             (mrd, "tests", "integration", "plots", name+".png")
            canvas.Print(plot_out)
            self.assertGreater(len(det[detector])/float(n_events), 0.90)

if __name__ == '__main__':
    unittest.main()

