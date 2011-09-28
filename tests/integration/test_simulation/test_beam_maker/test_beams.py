import unittest
import os
import subprocess

import ROOT

import xboa.Bunch
from xboa.Bunch import Bunch
import xboa.Common as Common

MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
SIM_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "simulate_mice.py")
PLOT_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                                    "plots", "beam_maker")

BIN_P = 0.1
BIN_N = 20
N_SPILLS = 1000
WEIGHTS_TO_PID = {-11:0.08, 211:0.02, -13:0.90}

class beam_test(unittest.TestCase):
    """
    Some tests on beam we push up to the high level integration tests - this
    checks that we can parse the beam configuration through all the simulation
    bureaucracy correctly and also that the generated distributions are correct.
    """
    def test_defaults(self):
        """
        Check that the default beam parameters run and produce some number of
        primary hits > 0
        """
        bunch = Bunch.new_from_read_builtin('maus_primary', 'simulation_defaults.out')
        self.assertTrue(len(bunch) > 0)

    def test_binomial(self):
        """
        Check that we can generate a binomial distribution for event number
        """
        # make a dict of bunches of xboa.Hits separated by event (spill) number
        bunch_dict = Bunch.new_dict_from_read_builtin('maus_primary', \
                          'simulation_binomial.out', 'event_number')
        bunch_weights = []
        ref_binomial_x = []
        ref_binomial_y = []
        for event, bunch in bunch_dict.iteritems():
            bunch_weights.append(bunch.bunch_weight())
        canvas = Common.make_root_canvas("")
        hist = Common.make_root_histogram("generated distribution",\
             bunch_weights, "bunch weights", BIN_N+1, xmin=-0.5, xmax=BIN_N+0.5)
        hist.Fill(0, 1000-hist.GetSumOfWeights()) # xboa ignores empty spills
        hist.Draw()

        test_hist = ROOT.TH1D("test_hist", "reference distribution", \
                               BIN_N+1, -0.5, BIN_N+0.5)
        for i in range(BIN_N+1):
            test_hist.SetBinContent(i,\
                    (ROOT.TMath.BinomialI(BIN_P, BIN_N, i-1)-\
                     ROOT.TMath.BinomialI(BIN_P, BIN_N, i))*N_SPILLS)
            print i, test_hist.GetBinContent(i), hist.GetBinContent(i)
        test_hist.SetLineStyle(2)
        test_hist.Draw("SAME")
        ks_value = test_hist.KolmogorovTest(hist)
        print "ks_value",ks_value
        self.assertGreater(ks_value, 1e-3)
        canvas.Update()
        canvas.Print(PLOT_DIR+"/binomial_distribution_test.png")

    def test_weighting(self):
        """
        Check that the weight of each sub-beam is close to the expected weight
        """
        # make a dict of bunches of xboa.Hits separated by event (spill) number
        bunch_dict = Bunch.new_dict_from_read_builtin('maus_primary', \
                          'simulation_binomial.out', 'pid')
        test_weights = {}
        sum_weights = 0
        for pid, bunch in bunch_dict.iteritems():
            test_weights[pid] = bunch.bunch_weight()
            sum_weights += test_weights[pid]
        for pid, weight in test_weights.iteritems():
            print pid, weight, WEIGHTS_TO_PID[pid]*sum_weights, sum_weights**0.5
            self.assertLess(abs(weight - WEIGHTS_TO_PID[pid]*sum_weights), \
                            sum_weights**0.5 )

    def test_gaussian(self):
        """
        Check that the weight of each sub-beam is close to the expected weight
        """
        # make a dict of bunches of xboa.Hits separated by event (spill) number
        bunch = Bunch.new_dict_from_read_builtin('maus_primary', \
                          'simulation_binomial.out', 'pid')[-13]
        for key, value in {'energy':285.0, ''}.iteritems():
        bunch.mean()

    def test_sawtooth_time(self):
        pass

    def test_uniform_time(self):
        pass


def run_simulations():
    try:
        os.mkdir(PLOT_DIR)
    except OSError:
        pass # dir already exists

    ps = subprocess.Popen([SIM_PATH, '-configuration_file', \
                           'default_beam_config.py'])
    ps.wait()
    os.rename('simulation.out', 'simulation_defaults.out')

    ps = subprocess.Popen([SIM_PATH, '-configuration_file', \
                           'binomial_beam_config.py'])
    ps.wait()
    os.rename('simulation.out', 'simulation_binomial.out')

if __name__ == "__main__":
    #run_simulations()
    unittest.main()

