#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

"""Set of tests that can be run against output data"""

import copy
import ctypes

import xboa 
import xboa.Common as Common
from xboa.Bunch import Bunch
import ROOT

#TODO: pylint:disable=W0511
# * For the actual physics list tests, I should add a test on emittance change,
#   on output moments, on mean energy loss vs pdg formula, on mcs vs pdg
#   formula, probably some other stuff


class Test:
    """
    Test object is an abstract type that contains enough information to run
    a test against some bunch of particle data

    Geometry object runs the Monte Carlo and returns a xboa.Bunch.Bunch object
    which contains particle data.
    """
    def __init__(self):
        """Initialise the test to 0."""
        pass

    def __repr__(self):
        """Represent the test as a string. Should enforce 
           some_test == eval(repr(some_test))"""
        raise(NotImplementedError(__name__+\
                                    " not implemented for this test class"))

    def __str__(self):
        """Summary information on the test"""
        raise(NotImplementedError(__name__+\
                                    " not implemented for this test class"))

    def deepcopy(self):
        """Make a copy of the test, copying memory over also. """
        raise(NotImplementedError(__name__+\
                                    " not implemented for this test class"))

    def run_test(self, bunch):
        """
        Returns a new reference test object whose internal state reflects 
        the data in the bunch
        """
        raise(NotImplementedError(__name__+\
                                    " not implemented for this test class"))

    def test_result(self):
        """
        Return 'pass', 'fail' or 'warn' to indicate whether the test passed,
        failed, or generated a warning (e.g. library not available for 
        testing)
        """
        raise(NotImplementedError(__name__+\
                                    " not implemented for this test class"))


    def make_plots(test_list):
        """
        Make plots if appropriate from each of the tests in test_list. Else 
        throw NotImplemented.
        """
        raise(NotImplementedError(__name__+\
                                    " not implemented for this test class"))
    make_plots = staticmethod(make_plots)

####################

class KSTest(Test): #note inheritance from test pylint: disable=R0902
    """
    ks test object is a summary of information generated and used for ks test. 
    Idea is to make a summary information such that we don't need to store round 
    large datasets to test against.
      variable = string name of the (Hit) variable for testing. Should be one of 
                 Hit.get() type variables.
      units    = string name of the units the variable is stored in
      bins     = list of bin edges
      content  = numpy array of bin contents (weight)
      n_events = number of events that was used to generate the data
      tolerance = ks tolerance (probability); below this tolerance, test is 
                  presumed to have failed
      ks_dist  = calculated maximum kolmogorov-smirnov distance
      ks_prob  = calculated kolmogorov-smirnov probability that the ks test is
                 the same as some reference data
    """
    def __init__(self):
        Test.__init__(self)
        self.variable  = ''
        self.units     = ''
        self.bins      = []
        self.content   = []
        self.n_events  = 0
        self.pid       = -13
        self.ks_dist   = 0.
        self.ks_prob   = 0.
        self.ks_tol    = 1.
    
    def __repr__(self):
        return 'KSTest.new('+repr(self.variable)+','+repr(self.units)+','+\
                repr(self.bins)+','+repr(self.content)+','+repr(self.n_events)+\
                ','+repr(self.pid)+','+repr(self.ks_dist)+','+\
                repr(self.ks_prob)+','+repr(self.ks_tol)+')'

    def __str__(self):
        """Summary of the KS test along with test pass information"""
        return 'ks test with variable '+str(self.variable)+' ks distance '+\
               str(self.ks_dist)+' probability '+str(self.ks_prob)+\
               ' tolerance '+str(self.ks_tol)+' : '+str(self.test_result())

    def new(variable, units, bins, content, n_events, pid, ks_dist, ks_prob, ks_tol): #pylint: disable=R0913, C0301
        """
        Initialises the object
        """
        my_ks_test = KSTest()
        my_ks_test.variable  = copy.deepcopy(variable)
        my_ks_test.units     = copy.deepcopy(units)
        my_ks_test.bins      = copy.deepcopy(bins)
        my_ks_test.content   = copy.deepcopy(content)
        my_ks_test.n_events  = copy.deepcopy(n_events)
        my_ks_test.pid       = copy.deepcopy(pid)
        my_ks_test.ks_dist   = copy.deepcopy(ks_dist)
        my_ks_test.ks_prob   = copy.deepcopy(ks_prob)
        my_ks_test.ks_tol    = copy.deepcopy(ks_tol)
        return my_ks_test
    new = staticmethod(new)

    def deepcopy(self):
        """
        Return a copy of the object
        """
        return KSTest.new(self.variable, self.units, self.bins, self.n_events, 
                self.content, self.pid, self.ks_dist, self.ks_prob, self.ks_tol)

    def run_test(self, test_bunch):
        """
        Run the ks test on the bunch. Return value is a new ks_test with
        internal state updated to reflect new test data
        """
        hist   = test_bunch.histogram_var_bins(self.variable, self.bins, 
                                                                     self.units)
        ks_test_out = self.deepcopy()
        ks_test_out.n_events = len(test_bunch)
        ks_test_out.content  = [0.]
        for i in hist[0]:
            ks_test_out.content.append(i[0]+ks_test_out.content[-1])
        del ks_test_out.content[0]
        if ks_test_out.content[-1] == 0:
            ks_test_out.content[-1] = 1.
        for i in range(len(ks_test_out.content)):
            ks_test_out.content[i] /= ks_test_out.content[-1] #normalise to 1
        ks_test_out.ks_dist = 0.
        for i in range( len(ks_test_out.content) ):
            ks_dist = abs(ks_test_out.content[i] - self.content[i])
            if ks_dist > ks_test_out.ks_dist:
                ks_test_out.ks_dist = ks_dist
        if ks_test_out.n_events+self.n_events == 0: 
            ks_dist_mod = 999.
        else:
            ks_dist_mod = ks_test_out.ks_dist*\
                        ((ks_test_out.n_events*self.n_events)/\
                         (ks_test_out.n_events+self.n_events))**0.5
        ks_test_out.ks_prob = ROOT.TMath.KolmogorovProb(ks_dist_mod) # pylint: disable=E1101, C0301
        return ks_test_out

    def test_result(self):
        """Return whether the test passed or failed"""
        if self.ks_prob < self.ks_tol:
            return 'fail'
        else:
            return 'pass'

    def cdf_function(contents):
        """
        helper function for make_plot. Set to make a cumulative density
        function plot
        """
        return contents
    cdf_function = staticmethod(cdf_function)
        
    def pdf_function(c_in):
        """
        helper function for make_plot. Set to make a probability density
        function plot.
        """
        c_out = [c_in[0]]
        for i in range(1, len(c_in)):
            c_out.append(c_in[i] - c_in[i-1])
        c_max = max(c_out)
        for i in range(len(c_out)):
            c_out[i] /= c_max
        return c_out
    pdf_function = staticmethod(pdf_function)

    def hist_width(ks_test_list):
        """
        Return the maximum and minimum of the histogram
        """
        (lower, upper) = -1, -1
        for ks_test in ks_test_list:
            index = 0
            data_max = max(ks_test.content) #Probably 1
            while ks_test.content[index] < 0.01*data_max and \
                  index < len(ks_test.content):
                index += 1
            if ks_test.bins[index] < lower or lower < 0:
                lower = ks_test.bins[index]
            while ks_test.content[index] < 0.99*data_max and \
                  index < len(ks_test.content):
                index += 1
            if ks_test.bins[index] > upper or upper < 0:
                upper = ks_test.bins[index+1]
        return lower, upper
    hist_width = staticmethod(hist_width)

    def __get_bins(bins):
        """
        Get ctypes array from a list of bins
        """
        bin_array = ctypes.c_double*len(bins)
        my_array = bin_array()
        for i in range(len(bins)):
            my_array[i] = ctypes.c_double(bins[i])
        return my_array
    __get_bins = staticmethod(__get_bins)

    def make_plots(ks_test_list):
        """
        Plot the distributions used for the ks test.
        """
        name = ks_test_list[0].variable
        if ks_test_list[0].units != '':
            name += ' ['+ks_test_list[0].units+']'
        canv = Common.make_root_canvas(name)

        bad_colors = [5, 10] #5 is yellow, 10 is white

        lower, upper = KSTest.hist_width(ks_test_list)

        hist = xboa.Common.make_root_histogram(
                  name+'-'+str(len(Common._hist_persistent)),
                  [], name, n_x_bins = 10000, xmin=lower, xmax=upper,
                  line_color=10)
        hist.Draw()
        # ack need complicated bin widths
        for k, test in  enumerate(ks_test_list): 
            bin_array = KSTest.__get_bins(test.bins)
            hist = ROOT.TH1D(name+'-'+str(len(Common._hist_persistent)), 
                             ';'+name, len(test.bins)-1, bin_array)
            if test.content[-1] != 0.:
                c_out = KSTest.pdf_function(test.content)
                for i, value in enumerate(c_out): 
                    hist.SetBinContent(i+1, value)
            Common._hist_persistent.append(hist)
            color = k+1
            while color in bad_colors: 
                color += 1
            hist.SetLineColor(color)
            hist.SetStats(False)
            hist.Draw('same')
        canv.Update()
        return (canv, h_start, len(Common._hist_persistent))
    make_plots = staticmethod(make_plots)

#######################

class HitEqualityTest(Test):
    """
    Hit equality test checks that two hits are equal within some floating point
    tolerance.
      bunch           = check that hits in the bunch are the same as those in 
                        the test bunch
      float_tolerance = floating point tolerance for the test
      test_result     = string, either 'pass' or 'fail' depending on whether 
                        test passed or failed
    """
    def __init__(self):
        """Initialise an empty equality test"""
        Test.__init__(self)
        self.bunch     = Bunch()
        self.tolerance = xboa.Common.float_tolerance    
        self.test_out  = 'fail'

    def __repr__(self):
        """Return a string; eval(repr(test) ) will return an identical test"""
        return 'hit_equality_test.new('+repr(self.tolerance)+\
                repr(self.test_out)+repr(self.bunch)+')'

    def __str__(self):
        return 'hit_equality_test with bunch of: '+str(self.bunch)+\
               ' tolerance: '+str(self.tolerance)+' result: '+\
               str(self.test_out)

    def new(tolerance, test_result, bunch):
        """initialise a new test"""
        a_test = HitEqualityTest()
        a_test.bunch     = bunch
        a_test.tolerance = tolerance
        a_test.test_out  = test_result    
        return a_test
    new = staticmethod(new)

    def run_test(self, test_bunch):
        """
        Compare test_bunch with stored bunch. return an updated copy of self 
        with test bunch and updated test_result
        """
        het_out = copy.deepcopy(self)
        het_out.test_out = 'pass'
        het_out.bunch       = test_bunch
        if not self.bunch.hit_equality(test_bunch, self.tolerance):
            het_out.test_out = 'fail'

    def test_result(self):
        """
        Return test result ('pass' or 'fail') depending on whether the test
        passed or failed
        """
        return self.test_out

    def deepcopy(self):
        """Return a copy of self (allocating new memory)"""
        return HitEqualityTest.new(self.tolerance, self.test_out, self.bunch)

    def make_plots(test_list):
        """No plots to make - so does nothing"""
        pass
    make_plots = staticmethod(make_plots)

