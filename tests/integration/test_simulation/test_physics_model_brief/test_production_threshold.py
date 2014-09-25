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

# pylint: disable = R0904
class ProductionThresholdTest(unittest.TestCase):
    """
    Generates some data and then attempts to load it and make a simple histogram
    """
    @classmethod
    def run_simulation(cls):
        """
        Run the simulation
        """
        print "Running simulation"
        log = os.path.expandvars(
                          "${MAUS_ROOT_DIR}/tmp/test_production_threshold.log")
        lout = open(log, "w")
        sim = os.path.expandvars("${MAUS_ROOT_DIR}/bin/simulate_mice.py")
        config =  os.path.expandvars("${MAUS_ROOT_DIR}/tests/integration/"+\
                                  "test_simulation/test_physics_model_brief/"+\
                                  "production_threshold_config.py")
        proc = subprocess.Popen(["python", sim, "--configuration_file", config],
                                stdout=lout, stderr=subprocess.STDOUT)
        proc.wait()

    def load_root_file(self):
        """
        Load the root file and run the tests
        """
        my_file_name = os.path.expandvars(
              "${MAUS_ROOT_DIR}/tmp/test_production_threshold.root"
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
                self.check_event(spill.GetMCEvents()[j])
        root_file.Close()

    def check_event(self, event):
        """
        Check a given event has the correct production threshold
        """
        track_0 = event.GetTracks()[0]
        for x_position, region in self.geometries.iteritems():
            pids = [track.GetParticleId() for track in event.GetTracks()]
            if abs(x_position - track_0.GetInitialPosition().x()) < 1.:
                print "Pids for region", region, pids
                if region == "ThresholdInt" or region == "Threshold":
                    # cuts are too high for gamma production
                    self.assertTrue(22 not in pids) 
                    self.assertTrue(11 in pids)
                    self.assertTrue(-13 in pids)
                elif region == "NoThreshold" or region == "NoCut" or \
                   region == None:
                    # production_threshold cut is too high for e+-
                    # production
                    self.assertTrue(22 not in pids)
                    self.assertTrue(11 not in pids)
                    self.assertTrue(-13 in pids)
                elif region == "NegativeCut":
                    # G4 defaults do produce gammas
                    self.assertTrue(22 in pids)
                    self.assertTrue(11 in pids)
                    self.assertTrue(-13 in pids)
                elif region == "KL1":
                    self.assertTrue(22 not in pids)
                    self.assertTrue(11 not in pids)
                    self.assertTrue(-13 in pids)
                elif region == "KL2":
                    self.assertTrue(22 in pids)
                    self.assertTrue(11 in pids)
                    self.assertTrue(-13 in pids)


    def test_production_threshold(self):
        """
        test_production_threshold: Run the simulation and check the outputs
        """
        self.run_simulation()
        self.load_root_file()

    geometries = {3500.0:"KL1", 2500.0:"KL2",
                  1500.0:"ThresholdInt", 1000.0:"Threshold", 500.:"Threshold",
                  0.0:"NoCut", -500.:"NoThreshold", -1000.:None,
                  -1500.:"NegativeCut"}

if __name__ == "__main__":
    unittest.main()

