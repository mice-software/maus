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

"""
High level tests for the beam maker - some pushed back from unit tests because
they are too slow. Check that we can generate appropriate distributions and that
the simulate_mice.py executable interfaces correctly.

Use Kolmogorov Smirnov test to compare distributions a lot of the time...
"""

import unittest
import os
import subprocess
import math

import ROOT
import numpy

from xboa.bunch import Bunch #pylint: disable=F0401
import xboa.common as common #pylint: disable=F0401


MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
TMP_PATH = os.path.join(MAUS_ROOT_DIR, "tmp")
SIM_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "simulate_mice.py")
CONV_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "utilities", "root_to_json.py")
PLOT_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                                    "plots", "beam_maker")
TEST_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                        "test_simulation", "test_beam_maker")
DEF_SIM = os.path.join(TMP_PATH, 'simulation_defaults.out')
BIN_SIM = os.path.join(TMP_PATH, 'simulation_binomial.out')
NAN_SIM = os.path.join(TMP_PATH, 'simulation_nan.out')
AP_SIM = os.path.join(TMP_PATH, 'simulation_ap_corr.out')
BIN_P = 0.1
BIN_N = 20
N_SPILLS = 1000
WEIGHTS_TO_PID = {-11:0.08, 211:0.02, -13:0.90}

SETUP_DONE = False

def make_plot_dir():
    """
    Make a directory to contain plots from this test
    """
    try:
        os.mkdir(PLOT_DIR)
    except OSError:
        pass # dir already exists

def run_simulations():
    """
    Run simulation to generate some data. We only want to do this once, so I
    pull it out into a separate part of the test.
    """
    out = open(TMP_PATH+'/test_beam_maker_output.out', 'w')

    # run simulation
    subproc = subprocess.Popen([SIM_PATH, '-configuration_file', \
                           os.path.join(TEST_DIR, 'default_beam_config.py')], \
                           stdout = out)
    subproc.wait()
    # run format conversion
    subproc = subprocess.Popen([CONV_PATH, '-configuration_file', \
                           os.path.join(TEST_DIR, 'default_beam_config.py'),
                          '-output_json_file_name', DEF_SIM], \
                           stdout = out)
    subproc.wait()

    # run simulation
    subproc = subprocess.Popen([SIM_PATH, '-configuration_file', \
                           os.path.join(TEST_DIR, 'binomial_beam_config.py')], \
                           stdout = out)
    subproc.wait()
    # run format conversion
    subproc = subprocess.Popen([CONV_PATH, '-configuration_file', \
                           os.path.join(TEST_DIR, 'binomial_beam_config.py'),
                          '-output_json_file_name', BIN_SIM], \
                           stdout = out)
    subproc.wait()
    
    # run simulation
    subproc = subprocess.Popen([SIM_PATH, '-configuration_file', \
                           os.path.join(TEST_DIR, 'nan_beam_config.py')], \
                           stdout = out)
    subproc.wait()
    # run format conversion
    subproc = subprocess.Popen([CONV_PATH, '-configuration_file', \
                           os.path.join(TEST_DIR, 'nan_beam_config.py'),
                          '-output_json_file_name', NAN_SIM], \
                           stdout = out)
    subproc.wait()

    # run simulation
    subproc = subprocess.Popen([SIM_PATH, '-configuration_file', \
                           os.path.join(TEST_DIR, 'ap_corr_beam_config.py')], \
                           stdout = out)
    subproc.wait()
    # run format conversion
    subproc = subprocess.Popen([CONV_PATH, '-configuration_file', \
                           os.path.join(TEST_DIR, 'ap_corr_beam_config.py'),
                          '-output_json_file_name', AP_SIM], \
                           stdout = out)
    subproc.wait()

    out.close()

class BeamMakerTest(unittest.TestCase): # pylint: disable = R0904
    """
    Some tests on beam we push up to the high level integration tests - this
    checks that we can parse the beam configuration through all the simulation
    bureaucracy correctly and also that the generated distributions are correct.
    """
    def setUp(self): #pylint: disable = C0103
        """
        run tests the first time the class is instantiated
        """
        global SETUP_DONE # pylint: disable = W0603
        if not SETUP_DONE:
            make_plot_dir()
            run_simulations()
            SETUP_DONE = True

    def test_defaults(self):
        """
        Check that the default beam parameters run and produce some number of
        primary hits > 0
        """
        bunch = Bunch.new_from_read_builtin('maus_json_primary', DEF_SIM)
        self.assertTrue(len(bunch) > 0)

    def test_binomial(self):
        """
        Check that we can generate a binomial distribution for event number
        """
        # make a dict of bunches of xboa.Hits separated by event (spill) number
        bunch_dict = Bunch.new_dict_from_read_builtin('maus_json_primary', \
                          BIN_SIM, 'spill')
        bunch_weights = []
        for bunch in bunch_dict.values():
            bunch_weights.append(bunch.bunch_weight())
        canvas = common.make_root_canvas("")
        hist = common.make_root_histogram("generated distribution", \
             bunch_weights, "bunch weights", BIN_N+1, xmin=-0.5, xmax=BIN_N+0.5)
        hist.Fill(0, 1000-hist.GetSumOfWeights()) # xboa ignores empty spills
        hist.Draw()

        test_hist = ROOT.TH1D("test_hist", "reference distribution", BIN_N+1, -0.5, BIN_N+0.5)  # pylint: disable = E1101, C0301
        for i in range(BIN_N+1):
            test_hist.SetBinContent(i, \
                    (ROOT.TMath.BinomialI(BIN_P, BIN_N, i-1)- ROOT.TMath.BinomialI(BIN_P, BIN_N, i))*N_SPILLS) # pylint: disable = E1101, C0301
            print i, test_hist.GetBinContent(i), hist.GetBinContent(i)
        test_hist.SetLineStyle(2)
        test_hist.Draw("SAME")
        ks_value = test_hist.KolmogorovTest(hist)
        canvas.Update()
        canvas.Print(PLOT_DIR+"/binomial_distribution_test.png")
        print "binomial ks_value", ks_value
        self.assertGreater(ks_value, 1e-3)

    def test_weighting(self):
        """
        Check that the weight of each sub-beam is close to the expected weight
        """
        # make a dict of bunches of xboa.Hits separated by event (spill) number
        bunch_dict = Bunch.new_dict_from_read_builtin('maus_json_primary', \
                          BIN_SIM, 'pid')
        test_weights = {}
        sum_weights = 0
        for pid, bunch in bunch_dict.iteritems():
            test_weights[pid] = bunch.bunch_weight()
            sum_weights += test_weights[pid]
        for pid, weight in test_weights.iteritems():
            print pid, weight, WEIGHTS_TO_PID[pid]*sum_weights, sum_weights**0.5
            self.assertLess(abs(weight - WEIGHTS_TO_PID[pid]*sum_weights), \
                            sum_weights**0.5 )

    def __cmp_matrix(self, ref_matrix, test_matrix):
        """Compare two numpy matrices"""
        self.assertEqual(ref_matrix.shape, test_matrix.shape)
        msg = "\nReference:\n"+str(ref_matrix)+\
              "\nIn Test:\n"+str(test_matrix)
        for i in range(ref_matrix.shape[0]):
            for j in range(test_matrix.shape[1]):
                ref = ref_matrix[i, j]/(ref_matrix[i, i]/ref_matrix[j, j])**0.5
                tst = test_matrix[i, j]/(ref_matrix[i, i]/ref_matrix[j, j])**0.5
                msg_ = msg+"\n "+str(i)+" "+str(j)+" "+str(ref)+" "+str(tst)
                diff = ref/tst-1 < 0.1 or 2.*abs(ref-tst)/(ref+tst) < 0.1
                self.assertTrue(diff, msg_)


    def test_gaussian(self):
        """
        Check that the weight of each sub-beam is close to the expected weight
        """
        # make a dict of bunches of xboa.Hits separated by event (spill) number
        bunch = Bunch.new_dict_from_read_builtin('maus_json_primary', \
                          BIN_SIM, 'pid')[-13]
        for key, value in {'energy':226.0, 'z':3.0, 'x':0., 'y':0., \
                           'px':0., 'py':0.}.iteritems():
            sigma = 5.*bunch.moment([key, key])**0.5\
                /float(bunch.bunch_weight())**0.5
            self.assertLess(abs(bunch.mean([key])[key]-value), sigma+1.)
        covs = bunch.covariance_matrix(['x', 'px', 'y', 'py'])
        test = numpy.array(\
        [[1.05668599e+03, -6.33950201e+02,  0.00000000e+00,  6.34327423e+02],
         [-6.33950201e+02, 1.14145263e+03, -6.34327423e+02,  0.00000000e+00],
         [0.00000000e+00, -6.34327423e+02,  1.05668599e+03, -6.33950201e+02],
         [6.34327423e+02,  0.00000000e+00, -6.33950201e+02,  1.14145263e+03]]
        )
        msg_ = "Determinants:"+str(numpy.linalg.det(test))+ \
                              str(numpy.linalg.det(covs))
        self.assertLess(\
              2.*abs(numpy.linalg.det(test) - numpy.linalg.det(covs))/ \
                    (numpy.linalg.det(test) + numpy.linalg.det(covs)), 0.2,
              msg=msg_)
        self.__cmp_matrix(test, covs)

    def test_no_nan(self):
        """
        Check that no nans appear
        """
        bunch = Bunch.new_from_read_builtin('maus_json_primary', NAN_SIM)
        self.assertAlmostEqual(bunch.bunch_weight(), 1000.)
        for hit in bunch:
            self.assertFalse(math.isnan(hit['energy']))
        canvas, hist = bunch.root_histogram('kinetic_energy', 'MeV', xmin=0.,
                                                          xmax=150.)
        cmp_hist = ROOT.TH1D("test", "test", hist.GetNbinsX(), 0., 150.) # pylint: disable = E1101, C0301
        for h_bin in range(1, hist.GetNbinsX()+1):
            sigma = 25.
            norm = 2.*bunch.bunch_weight()/sigma/(2.*math.pi)**0.5
            norm *= 150./hist.GetNbinsX()
            my_x = hist.GetBinCenter(h_bin)
            my_y = math.exp(-my_x**2./2./sigma**2.)*norm
            cmp_hist.Fill(my_x, my_y)
        cmp_hist.SetLineStyle(2)
        cmp_hist.Draw("SAME")
        ks_value = cmp_hist.KolmogorovTest(hist)
        print "nan energy ks_value", ks_value
        canvas.Update()
        canvas.Print(PLOT_DIR+"/nan_energy_distribution_test.png")
        self.assertGreater(ks_value, 1e-3)

    def test_sawtooth_time(self):
        """
        Check that beam maker generates sawtooth t distribution correctly
        """
        bunch = Bunch.new_dict_from_read_builtin('maus_json_primary', \
                          BIN_SIM, 'pid')[-13]
        canvas, hist = bunch.root_histogram('t', 'ns', xmin=-1e6, xmax=1e6)
        cmp_hist = ROOT.TH1D("test", "test", hist.GetNbinsX(), -1e6, 1e6) # pylint: disable = E1101, C0301
        for h_bin in range(1, hist.GetNbinsX()+1):
            my_x = hist.GetBinCenter(h_bin)
            my_y = 2.*hist.GetSumOfWeights()*h_bin/hist.GetNbinsX()**2
            cmp_hist.Fill(my_x, my_y)
        cmp_hist.SetLineStyle(2)
        cmp_hist.Draw("SAME")
        ks_value = cmp_hist.KolmogorovTest(hist)
        print "sawtooth t ks_value", ks_value
        canvas.Update()
        canvas.Print(PLOT_DIR+"/sawtooth_time_distribution_test.png")
        self.assertGreater(ks_value, 1e-3)

    def test_uniform_time(self):
        """
        Check that beam maker generates uniform t distribution correctly
        """
        bunch = Bunch.new_dict_from_read_builtin('maus_json_primary', \
                          BIN_SIM, 'pid')[-11]
        canvas, hist = bunch.root_histogram('t', 'ns', xmin=-2e6, xmax=1e6)
        cmp_hist = ROOT.TH1D("uniform_tst", "test", hist.GetNbinsX(), -2e6, 1e6) # pylint: disable = E1101, C0301
        for _bin in range(1, hist.GetNbinsX()+1):
            my_x = hist.GetBinCenter(_bin)
            my_y = hist.GetSumOfWeights()/float(hist.GetNbinsX())
            print my_x, my_y
            cmp_hist.Fill(my_x, my_y)
        cmp_hist.SetLineStyle(2)
        cmp_hist.Draw("SAME")
        ks_value = cmp_hist.KolmogorovTest(hist)
        print "uniform t ks_value", ks_value
        canvas.Update()
        canvas.Print(PLOT_DIR+"/uniform_time_distribution_test.png")
        self.assertGreater(ks_value, 1e-3)

    # def test_ap_corr(self):
        # """
        # Check that beam maker generates uniform t distribution correctly
        # """
        # print "loading ap"
        # bunch = Bunch.new_from_read_builtin('maus_json_primary', AP_SIM)
        # print "doing ap"
        # ref_p = 100. # update momentum in datacards as well
        # p_list = [hit['pz']-ref_p for hit in bunch]
        # amp_list = [bunch.get_hit_variable(hit, 'amplitude x y') \
        #                                                        for hit in bunch]
        # p_a_list = [ref_p*amp/mom for mom, amp in zip(p_list, amp_list)]
        # p_a_mean = sum(p_a_list)/len(p_a_list)
        # p_a_sigma = sum([p_a**2-p_a_mean**2 for p_a in p_a_list])/len(p_a_list)
        # print "mean ratio:", p_a_mean, "sigma ratio:", p_a_sigma
        # self.assertLess(abs(p_a_mean-0.001), 0.0002)
        # self.assertLess(abs(p_a_sigma), 1e-5)
        # canvas = common.make_root_canvas("a-p correlation")
        # canvas.Draw()
        # hist, graph = common.make_root_graph('a-p correlation',
        #                                     p_list, "dp_{z} [MeV/c]",
        #                                     amp_list, "A_{trans} [mm]")
        # hist.Draw()
        # graph.SetMarkerStyle(7)
        # graph.Draw('p')
        # graph.Fit("pol1")
        # canvas.Update()
        # canvas.Print(PLOT_DIR+"/beam_ap_corr_test.png")

    setup_done = False

if __name__ == "__main__":
    unittest.main()

