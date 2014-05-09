#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even 
#  the implied wlen(theta_arr)-1arranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.


"""Set of tests that can be run against output data"""

import ROOT
import xboa 
import xboa.Common as Common
import copy
import ctypes
from xboa.Bunch import Bunch
from array import array
from physics_model_test.geometry import Geometry #pylint:disable=W0611, F0401
#import physics_model_test.runner
#TODO: pylint:disable=W0511
# * For the actual physics list tests, I should add a test on emittance change,
#   on output moments, on mean energy loss vs pdg formula, on mcs vs pdg
#   formula, probably some other stuff


class BaseTest:
    """
    Test object is an abstract type that contains enough information to run
    a test against some bunch of particle data

    Geometry object runs the Monte Carlo and returns a xboa.Bunch.Bunch object
    which contains particle data.

    _hists is a list of histograms - for plotting multiple tests on the same
    canvas (used by test_plot)
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

    _hists = []

    _hists_energy_loss = []

    _hists_theta = []
 
    _hists_chi = []
####################

class KSTest(BaseTest): #pylint:disable=R0902
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
        BaseTest.__init__(self)
        self.variable  = 0
        self.units     = ''
        self.bins      = []
        self.content   = []
        self.n_events  = 0
        self.pid       = -13
        self.ks_dist   = 0.
        self.ks_prob   = 0.
        self.ks_tol    = 1.
        self.n_bins    = 0
     
    
    def __repr__(self):
        return 'KSTest.new('+repr(self.variable)+','+repr(self.units)+','+\
                repr(self.bins)+','+repr(self.content)+','+\
                repr(self.n_events)+','+\
                repr(self.pid)+','+\
                repr(self.ks_dist)+','+repr(self.ks_prob)+','+\
                repr(self.ks_tol)+','+repr(self.n_bins)+')'

    def __str__(self):
        """Summary of the KS test along with test pass information"""
        return 'ks test with variable '+str(self.variable)+' ks distance '+\
               str(self.ks_dist)+' probability '+str(self.ks_prob)+\
               ' tolerance '+str(self.ks_tol)+' : '+str(self.test_result())

    def new(variable, units, bins, content, n_events, pid, ks_dist, ks_prob, ks_tol, n_bins): #pylint: disable=R0913, C0301
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
        my_ks_test.n_bins    = copy.deepcopy(n_bins)

        return my_ks_test
    new = staticmethod(new)


    def deepcopy(self):
        """
        Return a copy of the object
        """
        return KSTest.new(self.variable, self.units, self.bins, self.n_events, 
                self.content, self.pid, self.ks_dist, self.ks_prob, self.ks_tol,
                self.n_bins)

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
        if self.ks_prob < 0.001:
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
        """calculates pdf function"""
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
        (lower, upper) = None, None
        for ks_test in ks_test_list:
            if lower == None or ks_test.bins[0] < lower:
                lower = ks_test.bins[0]
            if upper == None or ks_test.bins[-1] > upper:
                upper = ks_test.bins[-1]
        return lower, upper
    hist_width = staticmethod(hist_width)

    def _get_bins(bins):
        """
        Get ctypes array from a list of bins
        """
        bin_array = ctypes.c_double*len(bins)
        my_array = bin_array()
        for i in range(len(bins)):
            my_array[i] = ctypes.c_double(bins[i])
     
        return my_array
    _get_bins = staticmethod(_get_bins)

    def _get_min_non_zero_bin(test_list, float_tolerance=1e-9):
        """
        Get the content of the minimum bin that is not zero
        """
        running_min = 1.
        for test in test_list:
            for bin_cont in test.content:
                if bin_cont < running_min and bin_cont > float_tolerance:
                    running_min = bin_cont
        return running_min
    _get_min_non_zero_bin = staticmethod(_get_min_non_zero_bin)
    

    def make_plots(ks_test_list):
        """
        Plot the distributions used for the ks test.
        """        
        name = ks_test_list[0].variable
        print name
        if ks_test_list[0].units != '':
            name += ' ['+ks_test_list[0].units+']'
            print name
        canv = Common.make_root_canvas(name)
        canv.SetLogy()
       # lower, upper = KSTest.hist_width(ks_test_list)
        
        
        
        #my_ymin = KSTest._get_min_non_zero_bin(ks_test_list)
        h_start = len(BaseTest._hists)
        
        hist = xboa.Common.make_root_histogram(
                  name+'-'+str(len(BaseTest._hists)),
                  [], name, n_x_bins = 10000, xmin = -20, xmax = 20,
                  ymin = 0.0001, ymax = 100, line_color = 10)
        hist.Draw('Y+')

        BaseTest._hists.append(hist)
     
        for k, test in  enumerate(ks_test_list): 
            bin_array = KSTest._get_bins(test.bins)
            theta_arr = array('d')
             
            for i in range(1, len(bin_array)):
                theta = bin_array[i-1]
                theta_arr.append(theta)
            
          
            hist = ROOT.TH1D("theta", "theta/rad", len(theta_arr)-1, theta_arr)#pylint:disable=E1101, C0301
            
            if test.content[-1] != 0.:
                c_out = KSTest.pdf_function(test.content)
        
                for i, value in enumerate(c_out):
                     
           
                    hist.SetBinContent(i+1, value)
           
                
            BaseTest._hists.append(hist)
            color = k+1
            while color in [5, 10]: # yellow, white
                color += 1
            hist.SetLineColor(color)
            hist.SetStats(False)
            hist.SetMarkerStyle(20)
            hist.SetMarkerColor(color)
            hist.Draw('Psame')
   
        canv.Update()
        

        return (canv, BaseTest._hists[h_start:len(BaseTest._hists)])
         
    make_plots = staticmethod(make_plots)


class Chi2Test(BaseTest): #pylint:disable=R0902
    """ defines class for getting chi squared distribution"""
    def __init__(self):
        BaseTest.__init__(self)
        self.variable  = 0
        self.units     = ''
        self.bins      = []
        self.content   = []
        self.n_events  = 0
        self.pid       = -13
        self.errors    = []
        self.chi2   = 0.
        self.chi2_prob   = 0.
        self.chi2_tol    = 1.
        self.n_bins    = 0
     
    
    def __repr__(self):
        return 'Chi2Test.new('+repr(self.variable)+','+repr(self.units)+','+\
                repr(self.bins)+','+repr(self.content)+','+\
                repr(self.n_events)+','+\
                repr(self.pid)+','+repr(self.errors)+','+repr(self.chi2)+','+\
                repr(self.chi2_prob)+','+repr(self.chi2_tol)+','+\
                repr(self.n_bins)+')'

    def __str__(self):
        """Summary of the Chi2 test along with test pass information"""
        return 'chi2 test with variable '+str(self.variable)+' chi2 distance '+\
               str(self.chi2)+' probability '+str(self.chi2_prob)+\
               ' tolerance '+str(self.chi2_tol)+' : '+str(self.test_result())

    def new(variable, units, bins, content, n_events, pid, errors, chi2, chi2_prob, chi2_tol, n_bins): #pylint: disable=R0913, C0301
        """
        Initialises the object
        """
        my_chi2_test = Chi2Test()
        my_chi2_test.variable  = copy.deepcopy(variable)
        my_chi2_test.units     = copy.deepcopy(units)
        my_chi2_test.bins      = copy.deepcopy(bins)
        my_chi2_test.content   = copy.deepcopy(content)
        my_chi2_test.n_events  = copy.deepcopy(n_events)
        my_chi2_test.pid       = copy.deepcopy(pid)
        my_chi2_test.errors    = copy.deepcopy(errors)
        my_chi2_test.chi2   = copy.deepcopy(chi2)
        my_chi2_test.chi2_prob   = copy.deepcopy(chi2_prob)
        my_chi2_test.chi2_tol    = copy.deepcopy(chi2_tol)
        my_chi2_test.n_bins    = copy.deepcopy(n_bins)

        return my_chi2_test
    new = staticmethod(new)


    def deepcopy(self):
        """
        Return a copy of the object
        """
        return Chi2Test.new(self.variable, self.units, self.bins, \
                self.n_events, self.content, self.pid,self.errors, \
                self.chi2, self.chi2_prob, self.chi2_tol, \
                self.n_bins)

    def cdf_function(contents):
        """
        helper function for make_plot. Set to make a cumulative density
        function plot
        """
        return contents
    cdf_function = staticmethod(cdf_function)
        
    def pdf_function(c_in):
        """ calculates pdf function """
        c_out = [c_in[0]]
    
        for i in range(1, len(c_in)):
            c_out.append(c_in[i] - c_in[i-1])
        
        c_max = max(c_out)
        for i in range(len(c_out)):
            if c_out[i] != 0:
                c_out[i] /= c_max
        
        return c_out
    pdf_function = staticmethod(pdf_function)
    
    def hist_width(chi2_test_list):
        """
        Return the maximum and minimum of the histogram
        """
        (lower, upper) = None, None
        for chi2_test in chi2_test_list:
            if lower == None or chi2_test.bins[0] < lower:
                lower = chi2_test.bins[0]
            if upper == None or chi2_test.bins[-1] > upper:
                upper = chi2_test.bins[-1]
        return lower, upper
    hist_width = staticmethod(hist_width)

    def _get_bins(bins):
        """
        Get ctypes array from a list of bins
        """
        bin_array = ctypes.c_double*len(bins)
        my_array = bin_array()
        for i in range(len(bins)):
            my_array[i] = ctypes.c_double(bins[i])
     
        return my_array
    _get_bins = staticmethod(_get_bins)

    def _get_min_non_zero_bin(test_list, float_tolerance=1e-9):
        """
        Get the content of the minimum bin that is not zero
        """
        running_min = 1.
        for test in test_list:
            for bin_cont in test.content:
                if bin_cont < running_min and bin_cont > float_tolerance:
                    running_min = bin_cont
        return running_min
    _get_min_non_zero_bin = staticmethod(_get_min_non_zero_bin)
    

    

    def get_data():
        """ imports MUSCAT data from file """
        data = []
        
        data_file  = open("muscat_data.dat")
       
        test_data_list = eval(data_file.read())
       
      
        data = Chi2Test.pdf_function(test_data_list[0].content)
           
        
        return data
        
    get_data = staticmethod(get_data)


    def run_test(self, test_bunch): #pylint:disable=R0902, R0914
        """ runs chis squared test"""
        data_file  = open("MUSCAT_data.dat")
        test_data_list = eval(data_file.read())
       
        hist   = test_bunch.histogram_var_bins(self.variable, \
                                                self.bins, self.units)
                                                                   
        chi2_test_out = self.deepcopy()
      
        chi2_test_out.n_events = len(test_bunch)
        chi2_test_out.content  = [0.]
        
       
        for i in hist[0]:
            chi2_test_out.content.append(i[0]+chi2_test_out.content[-1])
        del chi2_test_out.content[0]
        if chi2_test_out.content[-1] == 0:
            chi2_test_out.content[-1] = 1.
        
  
       
        chi2_test_out.chi2 = 0.
        chi = 0
        for i in range(len(chi2_test_out.content)):
            chi2_test_out.content[i] /= chi2_test_out.content[-1]
        if chi2_test_out.content[-1] != 0.:
            
            sim_pdf = [chi2_test_out.n_events*x \
                      for x in Chi2Test.pdf_function(chi2_test_out.content)]
        
      
        for i, geo in enumerate(test_data_list):#pylint:disable=W0612
            for test in test_data_list[i].tests:
                chi = 0
                data_pdf = [test.n_events*x \
                             for x in Chi2Test.pdf_function(test.content)]
                print "DATA PDF", data_pdf
                weight_sim = 0
                weight_data = 0
                for i, simulation in enumerate(sim_pdf):#pylint:disable=W0612
                    weight_sim += sim_pdf[i]
            
                for i, data in enumerate(data_pdf):#pylint:disable=W0612
                    weight_data += data_pdf[i]
          
                
               
                for i, data in enumerate(data_pdf):#pylint:disable=W0612
               
                      
                    chi += (1/(weight_sim*weight_data))*\
                            ((weight_data*sim_pdf[i]-\
                            weight_sim*data_pdf[i])**2)\
                            /(sim_pdf[i]+data_pdf[i])
                print "CHI", chi
     
                
                    
   
        if chi > 200:
            chi2_test_out.chi2 = chi
       
        return chi2_test_out
  
            

    def test_result(self):
        """Return whether the test passed or failed"""
        if self.chi2_prob < self.chi2_tol:
            return 'fail'
        else:
            return 'pass'

 
    def make_plots(chi2_test_list):
        """
        Plot the distributions used for the chi2 test.
        """
       
        
        mom = 172

        
        
        name = '#theta'
        canv = Common.make_root_canvas(name)
        canv.SetLogy()
       # lower, upper = Chi2Test.hist_width(chi2_test_list)
        canv.SetTitle(name)

        
       # my_ymin = Chi2Test._get_min_non_zero_bin(chi2_test_list)
        h_start = len(BaseTest._hists_theta)
        
        hist = xboa.Common.make_root_histogram(
                  name+'-'+str(len(BaseTest._hists_theta)),
                  [] , name , n_x_bins = 10000 , xmin = -0.1 , xmax = 0.1 ,
                  ymin = 0.0001 , ymax = 10 , line_color = 10)
        hist.Draw('Y+')
      

        BaseTest._hists_theta.append(hist)

        for k, test in  enumerate(chi2_test_list): 
          
            bin_array = Chi2Test._get_bins(test.bins)
           
           
            theta_arr = array('d')
            
            for i in range(1 , len(bin_array)):
                theta = bin_array[i-1]/(mom)
                theta_arr.append(theta)
       
    
            hist = ROOT.TH1D("theta", "theta", len(theta_arr)-1, theta_arr) #pylint:disable=E1101, C0301
          
          
            if test.content[-1] != 0.:
                c_out = Chi2Test.pdf_function(test.content)
               
            
                for i, value in enumerate(c_out):
                     
           
                    hist.SetBinContent(i+1, value)
            

            BaseTest._hists_theta.append(hist)
            color = k+1
            while color in [5, 10]: # yellow, white
                color += 1
            hist.SetLineColor(color)
            hist.SetStats(False)
            hist.SetMarkerStyle(20)
            hist.SetMarkerColor(color)
            hist.GetXaxis().SetTitle('#theta_{RMS}')
            hist.Draw('Psame')
                 
        canv.SetGrid()
        canv.Update()
       
        canv.Update()
        return (canv, BaseTest._hists_theta[h_start:len(BaseTest._hists_theta)])
         
    make_plots = staticmethod(make_plots)



class Chi2Test(BaseTest): #pylint:disable=R0902
    """ defines class for getting chi squared distribution"""
    def __init__(self):
        BaseTest.__init__(self)
        self.variable  = 0
        self.units     = ''
        self.bins      = []
        self.content   = []
        self.n_events  = 0
        self.pid       = -13
        self.errors    = []
        self.chi2   = 0.
        self.chi2_prob   = 0.
        self.chi2_tol    = 1.
        self.n_bins    = 0
     
    
    def __repr__(self):
        return 'Chi2Test.new('+repr(self.variable)+','+repr(self.units)+','+\
                repr(self.bins)+','+repr(self.content)+','+\
                repr(self.n_events)+','+\
                repr(self.pid)+','+repr(self.errors)+','+repr(self.chi2)+','+\
                repr(self.chi2_prob)+','+repr(self.chi2_tol)+','+\
                repr(self.n_bins)+')'

    def __str__(self):
        """Summary of the Chi2 test along with test pass information"""
        return 'chi2 test with variable '+str(self.variable)+' chi2 distance '+\
               str(self.chi2)+' probability '+str(self.chi2_prob)+\
               ' tolerance '+str(self.chi2_tol)+' : '+str(self.test_result())

    def new(variable, units, bins, content, n_events, pid, errors, chi2, chi2_prob, chi2_tol, n_bins): #pylint: disable=R0913, C0301
        """
        Initialises the object
        """
        my_chi2_test = Chi2Test()
        my_chi2_test.variable  = copy.deepcopy(variable)
        my_chi2_test.units     = copy.deepcopy(units)
        my_chi2_test.bins      = copy.deepcopy(bins)
        my_chi2_test.content   = copy.deepcopy(content)
        my_chi2_test.n_events  = copy.deepcopy(n_events)
        my_chi2_test.pid       = copy.deepcopy(pid)
        my_chi2_test.errors    = copy.deepcopy(errors)
        my_chi2_test.chi2   = copy.deepcopy(chi2)
        my_chi2_test.chi2_prob   = copy.deepcopy(chi2_prob)
        my_chi2_test.chi2_tol    = copy.deepcopy(chi2_tol)
        my_chi2_test.n_bins    = copy.deepcopy(n_bins)

        return my_chi2_test
    new = staticmethod(new)


    def deepcopy(self):
        """
        Return a copy of the object
        """
        return Chi2Test.new(self.variable, self.units, self.bins, \
                self.n_events, self.content, self.pid,self.errors, \
                self.chi2, self.chi2_prob, self.chi2_tol, \
                self.n_bins)

    def cdf_function(contents):
        """
        helper function for make_plot. Set to make a cumulative density
        function plot
        """
        return contents
    cdf_function = staticmethod(cdf_function)
        
    def pdf_function(c_in):
        """ calculates pdf function """
        c_out = [c_in[0]]
    
        for i in range(1, len(c_in)):
            c_out.append(c_in[i] - c_in[i-1])
        
        c_max = max(c_out)
        for i in range(len(c_out)):
            if c_out[i] != 0:
                c_out[i] /= c_max
        
        return c_out
    pdf_function = staticmethod(pdf_function)
    
    def hist_width(chi2_test_list):
        """
        Return the maximum and minimum of the histogram
        """
        (lower, upper) = None, None
        for chi2_test in chi2_test_list:
            if lower == None or chi2_test.bins[0] < lower:
                lower = chi2_test.bins[0]
            if upper == None or chi2_test.bins[-1] > upper:
                upper = chi2_test.bins[-1]
        return lower, upper
    hist_width = staticmethod(hist_width)

    def _get_bins(bins):
        """
        Get ctypes array from a list of bins
        """
        bin_array = ctypes.c_double*len(bins)
        my_array = bin_array()
        for i in range(len(bins)):
            my_array[i] = ctypes.c_double(bins[i])
     
        return my_array
    _get_bins = staticmethod(_get_bins)

    def _get_min_non_zero_bin(test_list, float_tolerance=1e-9):
        """
        Get the content of the minimum bin that is not zero
        """
        running_min = 1.
        for test in test_list:
            for bin_cont in test.content:
                if bin_cont < running_min and bin_cont > float_tolerance:
                    running_min = bin_cont
        return running_min
    _get_min_non_zero_bin = staticmethod(_get_min_non_zero_bin)
    

    

    def get_data():
        """ imports MUSCAT data from file """
        data = []
        
        data_file  = open("MUSCAT_data.dat")
        test = Chi2Test.new('px','MeV/c',[-19.8, -16.13, -12.97, -10.27, -7.96, -5.97, -4.25, -2.79, -1.54, -0.468, 0.468, 1.54, 2.79, 4.25, 5.97, 7.96, 10.27, 12.97, 16.13, 19.8],[
1.22149E-05,4.88597E-05,0.000103827,0.000207654,0.00053135,0.001691768,0.008654281,0.071561191,0.311584643,0.688415357,0.928438809,0.991345719,0.998308232,0.99946865,
0.999792346,0.999896173,0.99995114,0.999987785,1],10000,13,[0.005,0.004,0.003,0.006,0.012,0.061,0.39,0.41,1.5,2.7,1.5,0.41,0.39,0.061,0.012,0.006,0.003,0.004,0.005], 1.0,0.0,0.01,20)#Chi2Test.new('px','MeV/c',     [-19.8,-16.13,-12.97,-10.27,-7.96,-5.97,-4.25,-2.79,-1.54,-0.468,0.468,1.54,2.79,4.25,5.97,7.96,10.27,12.97,16.13,19.8],[4.408E-05,9.44572E-05,0.00017632,0.000314857,0.000698984,0.002840015,0.019842319,0.118707573,0.34414554,0.65585446,0.881292427,0.980157681,0.997159985,0.999301016,0.999685143, 
#0.99982368,0.99990554,0.99995592,1],2694511,13,[0.005,0.004,0.003,0.006,0.012,0.061,0.39,0.41,1.5,2.7,1.5,0.41,0.39,0.061,0.012,0.006,0.003,0.004,0.005],0,1,1,1)
        #a_geometry = eval(data_file.read())
        #print type(a_geometry)
        #print a_geometry[0].tests
        #data_f = a_geometry.tests
        data = Chi2Test.pdf_function(test.content)
           
        
        return data
        
    get_data = staticmethod(get_data)


###################################################################################
    
    def chi_plots(chi2_test_list):
       
        data=[]
        data=Chi2Test.get_data()
        
        name='#theta .v.  #chi^{2} '
        canv = Common.make_root_canvas(name)
        canv.SetLogy()
        lower, upper = Chi2Test.hist_width(chi2_test_list)
        leg = ROOT.TLegend(0.0, 0.1, 0.4, 0.90)
        legends = ["GEANT4.9.2.p04", "GEANT4.9.5.p01", "ICOOL.3.30", "MuScat Data"]
        
        my_ymin = Chi2Test._get_min_non_zero_bin(chi2_test_list)
        h_start = len(BaseTest._hists_theta)
        
        hist = xboa.Common.make_root_histogram(
                  name+'-'+str(len(BaseTest._hists_theta)),
                  [], name, n_x_bins = 10000, xmin=-0.1, xmax=0.1,
                  ymin=0.0001, ymax=10, line_color=10)
        hist.Draw('Y+')
       
        errors=array('d')
        
        BaseTest._hists_theta.append(hist)
   
        for k, test in  enumerate(chi2_test_list): 
            bin_array=Chi2Test._get_bins(test.bins)
            

            
            weight=test.n_events/10000
            
            theta_arr=array('d')
   
            for i in range(1,len(bin_array)):
                theta=bin_array[i-1]/(172)
                theta_arr.append(theta)
                
            bins=sorted(theta_arr)
            runarray=array('d',bins)  
            hist=ROOT.TH1D("theta/rad","theta/rad", len(theta_arr)-1,theta_arr)
            
            if test.content[-1] != 0.:
                c_out =Chi2Test.pdf_function(test.content)
            
            chi_array=array('d')
            
            chi=0
            weight_sim = 0
            weight_data = 0
            for i, simulation in enumerate(c_out):#pylint:disable=W0612
                weight_sim += simulation
            
    
            for i, d in enumerate(data):#pylint:disable=W0612
                    weight_data += d
            for i, val in enumerate(c_out): 

                #30214.5 , 8644317.91
                chi+=(1/(30214.5*8644317.91))*\
                            ((8644317.91*val-30214.5*data[i])**2)/(val+data[i])
                print "value", val, "data", data[i], "chi", chi
                chi_array.append(chi) 
            print "WOOOWWWWW", chi_array
            print "CHIARRAY", chi
            for i, value in enumerate(chi_array):
                     
           
                    hist.SetBinContent(i+1, value)
           
     
            
            BaseTest._hists_theta.append(hist)
            color = k+1
            while color in [5, 10]: # yellow, white
                color += 1
            hist.SetLineColor(color)
            hist.SetStats(False)
            hist.SetMarkerStyle(20)
            hist.SetMarkerColor(color)
            hist.GetXaxis().SetTitle('#theta_{RMS}')
            hist.Draw('Psame')
          
            
        
        
        canv.SetGrid()
        canv.Update()
       
        canv.Update()
        return (canv, BaseTest._hists_theta[h_start:len(BaseTest._hists_theta)])
         
         
    chi_plots = staticmethod(chi_plots)
    



    def run_test(self, test_bunch): #pylint:disable=R0902, R0914
        """ runs chis squared test"""
        data_file  = open("MUSCAT_data.dat")
        test_data_list = eval(data_file.read())
        chi_arr=[]
        hist   = test_bunch.histogram_var_bins(self.variable, \
                                                self.bins, self.units)
                                                                   
        chi2_test_out = self.deepcopy()
      
        chi2_test_out.n_events = len(test_bunch)
        chi2_test_out.content  = [0.]
   
       
        for i in hist[0]:
            chi2_test_out.content.append(i[0]+chi2_test_out.content[-1])
        del chi2_test_out.content[0]
        if chi2_test_out.content[-1] == 0:
            chi2_test_out.content[-1] = 1.
        
  
       
        chi2_test_out.chi2 = 0.
        chi = 0
        for i in range(len(chi2_test_out.content)):
            chi2_test_out.content[i] /= chi2_test_out.content[-1]
        if chi2_test_out.content[-1] != 0.:
            
            sim_pdf = [chi2_test_out.n_events*x \
                      for x in Chi2Test.pdf_function(chi2_test_out.content)]
        
      
        for i, geo in enumerate(test_data_list):#pylint:disable=W0612
            for test in test_data_list[i].tests:
                chi = 0
                data_pdf = [test.n_events*x \
                             for x in Chi2Test.pdf_function(test.content)]
                print "DATA PDF", data_pdf
                weight_sim = 0
                weight_data = 0
                for i, simulation in enumerate(sim_pdf):#pylint:disable=W0612
                    weight_sim += sim_pdf[i]
            
                for i, data in enumerate(data_pdf):#pylint:disable=W0612
                    weight_data += data_pdf[i]
          
                
               
                for i, data in enumerate(data_pdf):#pylint:disable=W0612
               
                      
                    chi += (1/(weight_sim*weight_data))*\
                            ((weight_data*sim_pdf[i]-\
                            weight_sim*data_pdf[i])**2)\
                            /(sim_pdf[i]+data_pdf[i])
                chi_arr.append(chi)
              
                print "array ", chi_arr
           
        #-------P-VALUE---------#
        for i, sim in enumerate(sim_pdf):
            PValue = 0
            PValue = 1 - sim
            print "P-Value is: ", PValue
    
       #########################          
   
        if chi > 200:
            chi2_test_out.chi2 = chi
       
        return chi2_test_out
  
            

    def test_result(self):
        """Return whether the test passed or failed"""
        if self.chi2_prob < self.chi2_tol:
            return 'fail'
        else:
            return 'pass'

 
    def make_plots(chi2_test_list):
        """
        Plot the distributions used for the chi2 test.
        """
       
        
        mom = 172

        
        
        name = '#theta'
        canv = Common.make_root_canvas(name)
        canv.SetLogy()
       # lower, upper = Chi2Test.hist_width(chi2_test_list)
        canv.SetTitle(name)

        
       # my_ymin = Chi2Test._get_min_non_zero_bin(chi2_test_list)
        h_start = len(BaseTest._hists_theta)
        
        hist = xboa.Common.make_root_histogram(
                  name+'-'+str(len(BaseTest._hists_theta)),
                  [] , name , n_x_bins = 10000 , xmin = -0.1 , xmax = 0.1 ,
                  ymin = 0.0001 , ymax = 10 , line_color = 10)
        hist.Draw('Y+')
      

        BaseTest._hists_theta.append(hist)

        for k, test in  enumerate(chi2_test_list): 
          
            bin_array = Chi2Test._get_bins(test.bins)
           
           
            theta_arr = array('d')
            
            for i in range(1 , len(bin_array)):
                theta = bin_array[i-1]/(mom)
                theta_arr.append(theta)
                
            hist = ROOT.TH1D("theta", "theta", len(theta_arr)-1, theta_arr) #pylint:disable=E1101, C0301
          
          
            if test.content[-1] != 0.:
                c_out = Chi2Test.pdf_function(test.content)
               
            
                for i, value in enumerate(c_out):
                     
           
                    hist.SetBinContent(i+1, value)
            

            BaseTest._hists_theta.append(hist)
            color = k+1
            while color in [5, 10]: # yellow, white
                color += 1
            hist.SetLineColor(color)
            hist.SetStats(False)
            hist.SetMarkerStyle(20)
            hist.SetMarkerColor(color)
            hist.GetXaxis().SetTitle('#theta_{RMS}')
            hist.Draw('Psame')
                 
        canv.SetGrid()
        canv.Update()
       
        canv.Update()
        return (canv, BaseTest._hists_theta[h_start:len(BaseTest._hists_theta)])
         
    make_plots = staticmethod(make_plots)








class HitEqualityTest(BaseTest):
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
        BaseTest.__init__(self)
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

