#!/usr/bin/env python

"""
Example to load a ROOT file
"""

import os
import subprocess
import unittest

# basic PyROOT definitions
import ROOT 

# definitions of MAUS data structure for PyROOT
import libMausCpp #pylint: disable = W0611

class FineGrainedProductionThresholdTest(unittest.TestCase):
    """
    Generates some data and then attempts to load it and make a simple histogram
    """
    @classmethod
    def run_simulation(cls):
        print "Running simulation"
        log = os.path.expandvars("${MAUS_ROOT_DIR}/tmp/test_prod_threshold.log")
        lout = open(log, "w")
        sim = os.path.expandvars("${MAUS_ROOT_DIR}/bin/simulate_mice.py")
        config =  os.path.expandvars("${MAUS_ROOT_DIR}/tests/integration/"+\
                                  "test_simulation/test_physics_model_brief/"+\
                                  "fine_grained_production_threshold_config.py")
        proc = subprocess.Popen(["python", sim, "--configuration_file", config],
                                stdout=lout, stderr=subprocess.STDOUT)
        proc.wait()

    @classmethod
    def load_root_file(cls):
        my_file_name = os.path.expandvars(
              "${MAUS_ROOT_DIR}/tmp/test_fine_grained_production_threshold.root"
        )
        print "Loading ROOT file", my_file_name
        root_file = ROOT.TFile(my_file_name, "READ") # pylint: disable = E1101
        print "Setting up data tree"
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)
        for i in range(tree.GetEntries()):
            tree.GetEntry(i)
            spill = data.GetSpill()
            for j in range(spill.GetMCEvents().size()):
                event = spill.GetMCEvents()[j]
                track_0 = event.GetTracks()[0]
                for x_position, region in cls.geometries.iteritems():
                    if abs(x_position - track_0.GetInitialPosition().x()) < 1.:
                        print "Tracks for region", region
                for k in range(event.GetTracks().size()):
                    track = event.GetTracks()[k]
                    print "    event:", j, "track:", k, \
                          "x0:", track.GetInitialPosition().x(), \
                          "z0:", track.GetInitialPosition().z(), \
                          "pid:", track.GetParticleId()
        root_file.Close()

    def test_fine_grained_prod_threshold_test(self):
        self.run_simulation()
        self.load_root_file()

    geometries = {1500.0:"ThresholdInt", 1000.0:"Threshold", 500.:"Threshold",
                  0.0:"NoCut", -500.:"NoThreshold", -1000.:None,
                  -1500.:"NegativeCut"}

if __name__ == "__main__":
    unittest.main()

