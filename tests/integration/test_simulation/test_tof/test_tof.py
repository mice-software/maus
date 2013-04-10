"""
Test TOF simulation and reconstruction
"""

import os
import unittest
import subprocess #pylint: disable=E1101
from ROOT import TFile, TH1, TH1F, TH1D #pylint: disable=E0611,W0611

REF_PLOT_FILE = "tof_mc_plots_ref.root"
TEST_PLOT_FILE = "tof_mc_plots.root"
TEST_ROOT_FILE = "test_tof_mc.root"

def run_simulation(output_root):
    """
    Runs tof simulation
    """
    mrd = os.environ["MAUS_ROOT_DIR"]
    os.chdir(os.getenv('MAUS_ROOT_DIR')+\
             '/tests/integration/test_simulation/test_tof')
    print 'Running TOF simulation'
    log = os.path.expandvars('$MAUS_ROOT_DIR/tmp/log.tof')
    print 'logging in ', log,
    config_file = os.path.join(mrd,
      "tests/integration/test_simulation/test_tof/config.py")
    simulation = os.path.join(mrd, "bin/simulate_mice.py")
    call_options = [simulation,
                    "-configuration_file", config_file,
                    "-output_root_file_name", output_root,
                    "-verbose_level", "1",
                   ]

    log = open(
            os.path.join(mrd, "tmp/test_simulation_tof.log"), 'w'
          )
    proc = subprocess.Popen(call_options, stdout=log, stderr=subprocess.STDOUT)
    proc.wait() # pylint: disable=E1101
    return proc.returncode # pylint: disable=E1101

def run_plotter(input_root, output_root):
    """
    Runs tof plotter
    Make plots and write them to root file
    """
    mrd = os.environ["MAUS_ROOT_DIR"]
    os.chdir(os.getenv('MAUS_ROOT_DIR')+\
             '/tests/integration/test_simulation/test_tof')
    log = open(
            os.path.join(mrd, "tmp/test_simulation_tof.log"), 'a'
          )
    plotter = os.path.join("./tof_mc_plotter")
    call_options = [plotter, input_root, output_root,]
    proc = subprocess.Popen(call_options, stdout=log, stderr=subprocess.STDOUT)
    proc.wait() #pylint: disable=E1101
    return proc.returncode #pylint: disable=E1101

def compare_plots(plotname):
    """
    Compare output with reference plot 
    """
    fileref = TFile(REF_PLOT_FILE,"r")
    refplotname = plotname+"_ref"
    h1 = fileref.Get(refplotname) # pylint: disable=C0103
    filein = TFile(TEST_PLOT_FILE,"r")
    h2 = filein.Get(plotname) # pylint: disable=C0103
    kval = h1.KolmogorovTest(h2,"UOND")
    return kval

class TestTof(unittest.TestCase): # pylint: disable=R0904
    """
    Run tof simulation and validate the output
    """

    def test_tof(self):
        """
        Check that we can run the simulation, make plots
        and compare the plots against reference plots
        """
        mrd = os.environ["MAUS_ROOT_DIR"]
        output_root = os.path.join(mrd, "tmp/", TEST_ROOT_FILE)
        return_code = run_simulation(output_root)
        self.assertEqual(return_code, 0)
        input_root = output_root
        output_root = TEST_PLOT_FILE
        return_code = run_plotter(input_root, output_root)
        self.assertEqual(return_code, 0)
        plotname = "tof01"
        kthresh = 0.8
        return_code = compare_plots(plotname)
        self.assert_(return_code > kthresh)

if __name__ == '__main__':
    unittest.main()

