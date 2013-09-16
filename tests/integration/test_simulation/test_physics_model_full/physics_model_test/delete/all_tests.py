#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied wlen(theta_arr)-1arranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.


"""Set of tests that can be run against output data"""

import copy
import ctypes
import numpy
import math
import xboa 
import xboa.Common as Common
from xboa.Bunch import Bunch
import ROOT
from array import array

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

    _hists_energy_loss=[]

    _hists_theta=[]
 
    _hists_chi=[]
####################

class KSTest(BaseTest): #note inheritance from test pylint: disable=R0902
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
                repr(self.bins)+','+repr(self.content)+','+repr(self.n_events)+\
                ','+repr(self.pid)+','+repr(self.ks_dist)+','+\
                repr(self.ks_prob)+','+repr(self.ks_tol)+','+repr(self.n_bins)+\
                ')'

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
        
    def pdf_function_bins(c_in):
        """
        helper function for make_plot. Set to make a probability density
        function plot.
        """

        with open('MUSCAT_data.txt', 'r') as f:
           lines = f.readlines()
           bins=array('d')
          
     
           for line in lines:
            
                 app= line.split(' ')[1:]#.strip(' ')
                 bins.append(float(app[6]))

        c_out = [c_in[0]]
      
        for i in range(1, len(c_in)):
            c_out.append(c_in[i] - c_in[i-1])
            
        c_max = max(c_out)
        for i in range(len(c_out)):
            #c_out[i] /= c_max
        
            c_out[i]/=(bins[i]-bins[i-1])#prob/rad
        return c_out
    pdf_function_bins = staticmethod(pdf_function_bins)
   
    
    def pdf_function(c_in):
     
        c_out = [c_in[0]]
    
        for i in range(1, len(c_in)):
            c_out.append(c_in[i] - c_in[i-1])
        
        c_max = max(c_out)
        for i in range(len(c_out)):
            c_out[i] /= c_max
        
        return c_out
    pdf_function = staticmethod(pdf_function)
    

    def pdf_gauss(c_in):
        """
        helper function for make_plot. Set to make a probability density
        function plot.
        """

        with open('MUSCAT_data.txt', 'r') as f:
           lines = f.readlines()
           bins=array('d')
          
     
           for line in lines:
            
                 app= line.split(' ')[1:]#.strip(' ')
                 bins.append(float(app[5]))

        c_out = [c_in[0]]
      
        for i in range(1, len(c_in)):
            c_out.append(0.389*math.exp(0.564*(2*c_in[i]-1)**2))
            
            
        
        for i in range(len(c_out)):
            #c_out[i] /= c_max
            c_out[i]/=(bins[i]-bins[i-1])#prob/rad
        return c_out
    pdf_gauss = staticmethod(pdf_gauss)
    


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
    

    def get_initial_pz(test_list):
         p=172
         return p
    get_initial_pz=staticmethod(get_initial_pz)

    def get_initial_energy(test_list):
         energy=232
         return energy
    get_initial_energy=staticmethod(get_initial_energy)


    def Chi(cons,data):
        chi=0
        data=[]
        chi_array=[]
        
        
        for i, val in enumerate(data):                
            chi+=((val-(data[i]))**2)/(data[i])
            chi_array.append(chi)           
            print "sim: ", val,"data", data[i] ,"chi ", chi, "length ", len(cons)

        return chi_array
    Chi=staticmethod(Chi)



    def chi_x_prime(cons,bins):
        
        
        chi_sum=array('d')
        chi_sum=KSTest.Chi(cons)
        data=array('d')
        x_prime_array=array('d')
        chi_array=array('d')
        contents=[4.408E-05,9.44572E-05,0.00017632,0.000314857,0.000698984,0.002840015,0.019842319,0.118707573,0.34414554,0.65585446,0.881292427,0.980157681,0.997159985,0.999301016,0.999685143, 
0.99982368,0.99990554,0.99995592,1]
        data=KSTest.pdf_function(contents)
        
        
        for i, val in enumerate(cons): 
                       
            chi=((val-(data[i]))**2)/(data[i])
            chi_array.append(chi)

        canvas = xboa.Common.make_root_canvas("Theta/rad")
        canvas.SetLogy()
        x_prime_chi_2 = xboa.Common.make_root_histogram(
                  'x_prime .v. #chi^{2} per bin',
                  [], 'x_prime .v. #chi^{2} per bin', n_x_bins = 10000, xmin=0, xmax=0.1,
                  ymin=0.001,ymax=20, line_color=10)
       
        x_prime_chi_2.Draw('Y+')

        x_prime_chi_2_sum = xboa.Common.make_root_histogram(
                  'x_prime .v. #chi^{2} per bin',
                  [], 'x_prime .v. #chi^{2} per bin', n_x_bins = 10000, xmin=0, xmax=0.1,
                  ymin=0.001,ymax=20, line_color=10)
       
        x_prime_chi_2_sum.Draw('Y+')
      
        
        x_prime_chi_2=ROOT.TH1D("theta/rad","theta/rad", len(bins)-1,bins) 
        x_prime_chi_2_sum=ROOT.TH1D("theta/rad","theta/rad", len(bins)-1,bins) 
        for i, value in enumerate(chi_array):
                    x_prime_chi_2_sum.SetBinContent(i+1,chi_sum[i])
		    x_prime_chi_2.SetBinContent(i+1,value)
        x_prime_chi_2.GetYaxis().SetTitle("#chi^{2} per bin")
        x_prime_chi_2.GetXaxis().SetTitle("x'")
        x_prime_chi_2.Draw('same')
        x_prime_chi_2_sum.GetYaxis().SetTitle("#chi^{2} per bin")
        x_prime_chi_2_sum.GetXaxis().SetTitle("x'")
        #x_prime_chi_2_sum.Draw()
        return x_prime_chi_2
        canvas.Update()
    chi_x_prime = staticmethod(chi_x_prime)


    

    
    def chi_plots(ks_test_list):
        test_list=[]
        data=[]
        content=[]
        #for i, test in enumerate(ks_test_list):
            
                

          #  data=KSTest.pdf_function(test.content)

        data=KSTest.pdf_function([4.408E-05,9.44572E-05,0.00017632,0.000314857,0.000698984,0.002840015,0.019842319,0.118707573,0.34414554,0.65585446,0.881292427,0.980157681,0.997159985,0.999301016,0.999685143, 
0.99982368,0.99990554,0.99995592,1])




        name='#theta_{RMS} .v.  #chi^{2} per bin'
        canv = Common.make_root_canvas(name)
        canv.SetLogy()
        lower, upper = KSTest.hist_width(ks_test_list)

        legends=['MAUS GEANT4.9.2.p01','ICOOL','MAUS GEANT4.9.5.p04','DATA']
        
        my_ymin = KSTest._get_min_non_zero_bin(ks_test_list)
        h_start = len(BaseTest._hists_theta)
        
        hist = xboa.Common.make_root_histogram(
                  name+'-'+str(len(BaseTest._hists_theta)),
                  [], name, n_x_bins = 10000, xmin=-0.1, xmax=0.1,
                  ymin=0.0001, ymax=10, line_color=10)
        hist.Draw('Y+')
        leg =  ROOT.TLegend(0.6,0.7,0.9,0.9)
        
        

        BaseTest._hists_theta.append(hist)

        for k, test in  enumerate(ks_test_list): 
            bin_array=KSTest._get_bins(test.bins)
            #print test.bins
            theta_arr=array('d')
            
            for i in range(1,len(bin_array)):
                theta=bin_array[i-1]/(172)
                theta_arr.append(theta)
            bins=sorted(theta_arr)
            runarray=array('d',bins)  
            hist=ROOT.TH1D("theta/rad","theta/rad", len(theta_arr)-1,theta_arr)
            #print theta_arr
            if test.content[-1] != 0.:
                c_out =KSTest.pdf_function(test.content)
            
            chi_array=array('d')
            
            chi=0
            
    
        
            
            for i, val in enumerate(c_out):                
                chi+=((val-(data[i]))**2)/(data[i])
                chi_array.append(chi) 
            print chi 
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
            hist.GetYaxis().SetTitle('#chi^{2}/bin')
            hist.Draw('Psame')
            
           
            
        
            leg.AddEntry(hist, legends[k])
       
        leg.Draw('same')
        canv.SetGrid()
        canv.Update()

        canv.Update()
        return (canv, BaseTest._hists_theta[h_start:len(BaseTest._hists_theta)])
         
    chi_plots = staticmethod(chi_plots)

    
    def theta(ks_test_list,geo_list):
        """
        Plot the distributions used for the ks test.
        """
        
        code=[]
        sort_name = geo_list.name.split(' ')[1:] # strip out code name/version number
        
        sort_name = [item+' ' for item in sort_name[:-1]]+[sort_name[-1]] # spaces
        sort_name = ''.join(sort_name)
        sort_name = sort_name.replace('/', '')
        
       # data=[]
       # content=[]
       # for i, test in enumerate(ks_test_list):
           
       #     if i==2:
       #         content.append(test.content)


       # data=KSTest.pdf_function(content)

        data=KSTest.pdf_function([4.408E-05,9.44572E-05,0.00017632,0.000314857,0.000698984,0.002840015,0.019842319,0.118707573,0.34414554,0.65585446,0.881292427,0.980157681,0.997159985,0.999301016,0.999685143, 
0.99982368,0.99990554,0.99995592,1])

        name='#theta'
        canv = Common.make_root_canvas(name)
        canv.SetLogy()
        lower, upper = KSTest.hist_width(ks_test_list)
        canv.SetTitle(name)
        legends=['MAUS GEANT4.9.2.p01','ICOOL','MAUS GEANT4.9.5.p04','DATA']
        
        my_ymin = KSTest._get_min_non_zero_bin(ks_test_list)
        h_start = len(BaseTest._hists_theta)
        
        hist = xboa.Common.make_root_histogram(
                  name+'-'+str(len(BaseTest._hists_theta)),
                  [], name, n_x_bins = 10000, xmin=-0.1, xmax=0.1,
                  ymin=0.0001, ymax=10, line_color=10)
        hist.Draw('Y+')
        leg =  ROOT.TLegend(0.6,0.7,0.9,0.9)
        leg.SetHeader(sort_name)
        

        BaseTest._hists_theta.append(hist)

        for k, test in  enumerate(ks_test_list): 
            bin_array=KSTest._get_bins(test.bins)
            code_name= geo_list.name.split(':')[0]
            
            theta_arr=array('d')
            
            for i in range(1,len(bin_array)):
                theta=bin_array[i-1]/(172.)
                theta_arr.append(theta)
            bins=sorted(theta_arr)
            runarray=array('d',bins)  
            hist=ROOT.TH1D("theta/rad","theta/rad", len(theta_arr)-1,theta_arr)
          
            if test.content[-1] != 0.:
                c_out =KSTest.pdf_function(test.content)
               
             
                for i, value in enumerate(c_out):
                     
           
                    hist.SetBinContent(i+1, value)
            
           # KSTest.Chi(c_out,data)
            

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
            
          
            
        
            leg.AddEntry(hist, code_name)
        
        leg.Draw('same')
        canv.SetGrid()
        canv.Update()
       
        canv.Update()
        return (canv, BaseTest._hists_theta[h_start:len(BaseTest._hists_theta)])
         
    theta = staticmethod(theta)

    
       



    
    def energy_loss(test_list):
        
        energy_loss_arr=array('d')
        h_start = len(BaseTest._hists_energy_loss)
        contents_arr=array('f')
        contents_arr=KSTest.get_contents(test_list)
        canvas = xboa.Common.make_root_canvas("Energy Loss/MeV")
        lower, upper = KSTest.hist_width(test_list)
        my_ymin = KSTest._get_min_non_zero_bin(test_list)
        name="Energy Loss/MeV"
        hist = Common.make_root_histogram(name, [],name,n_x_bins=10000, xmin=232-upper,xmax=232-lower, ymin=my_ymin/2, line_color=10)
        hist.Draw('Y+')
        BaseTest._hists_energy_loss.append(hist)
        hist=KSTest.make_muscat(test_list)
        for k, test in enumerate(test_list):
                for  bin in test.bins:
                      energy_loss=232-bin
                      energy_loss_arr.append(energy_loss)
             
                      if test.content[-1] != 0.:
                           c_out = KSTest.pdf_function(test.content)
                
                bins=sorted(energy_loss_arr)
                runarray=array('d',bins)
                hist=ROOT.TH1D("Energy Loss/MeV","Energy Loss/MeV", len(energy_loss_arr)-1,runarray)
                for i, value in enumerate(c_out): 
                      hist.SetBinContent(i+1, value)
                hist.SetStats(True)
                
                color = k+1
                while color in [5, 10]: # yellow, white
                    color += 1
                hist.SetLineColor(color)
                hist.Draw("same") 
                BaseTest._hists_energy_loss.append(hist)
     
        canvas.Update()   
        for i in range (1, len(energy_loss_arr)):
            energy_loss_arr.append(0)
        return (canvas, BaseTest._hists_energy_loss[h_start:len(BaseTest._hists_energy_loss)])
        #return hist
    energy_loss = staticmethod(energy_loss)
    
    

   
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
        lower, upper = KSTest.hist_width(ks_test_list)
        
        
        
        my_ymin = KSTest._get_min_non_zero_bin(ks_test_list)
        h_start = len(BaseTest._hists)
        
        hist = xboa.Common.make_root_histogram(
                  name+'-'+str(len(BaseTest._hists)),
                  [], name, n_x_bins = 10000, xmin=-20, xmax=20,
                  ymin=0.0001,ymax=100, line_color=10)
        hist.Draw('Y+')

        
        

        BaseTest._hists.append(hist)
     
        for k, test in  enumerate(ks_test_list): 
            bin_array=KSTest._get_bins(test.bins)
            # test.bins
            theta_arr=array('d')
             
            for i in range(1,len(bin_array)):
                theta=bin_array[i-1]
                theta_arr.append(theta)
            bins=sorted(theta_arr)
            runarray=array('d',bins)  
            hist=ROOT.TH1D("theta/rad","theta/rad", len(theta_arr)-1,theta_arr)
            
            if test.content[-1] != 0.:
                c_out =KSTest.pdf_function(test.content)
               
                #print "c_out:", c_out
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

    


#######################


class CSTest(BaseTest):
    """
    will fit to data
    """


    def __init__(self):
        BaseTest.__init__(self)
        self.variable  = 0
        self.units     = ''
        self.bins      = []
        self.content   = []
        self.n_events  = 0
        self.pid       = -13
        self.cs_dist   = 0.
        self.cs_prob   = 0.
        self.cs_tol    = 1.
        self.n_bins    = 0
    
    def __repr__(self):
        return 'CSTest.new('+repr(self.variable)+','+repr(self.units)+','+\
                repr(self.bins)+','+repr(self.content)+','+repr(self.n_events)+\
                ','+repr(self.pid)+','+repr(self.cs_dist)+','+\
                repr(self.cs_prob)+','+repr(self.cs_tol)+','+repr(self.n_bins)+\
                ')'

    def __str__(self):
        """Summary of the CS test along with test pass information"""
        return 'fitter test with variable '+str(self.variable)+' fitter distance '+\
               str(self.cs_dist)+' probability '+str(self.cs_prob)+\
               ' tolerance '+str(self.cs_tol)+' : '+str(self.test_result())

    def new(variable, units, bins, content, n_events, pid, ks_dist, ks_prob, ks_tol, n_bins): #pylint: disable=R0913, C0301
        """
        Initialises the object
        """
        my_fit = CSTest()
        my_fit.variable  = copy.deepcopy(variable)
        my_fit.units     = copy.deepcopy(units)
        my_fit.bins      = copy.deepcopy(bins)
        my_fit.content   = copy.deepcopy(content)
        my_fit.n_events  = copy.deepcopy(n_events)
        my_fit.pid       = copy.deepcopy(pid)
        my_fit.cs_dist   = copy.deepcopy(cs_dist)
        my_fit.cs_prob   = copy.deepcopy(cs_prob)
        my_fit.cs_tol    = copy.deepcopy(cs_tol)
        my_fit.n_bins    = copy.deepcopy(n_bins)
        return my_fit
    new = staticmethod(new)

    def deepcopy(self):
        """
        Return a copy of the object
        """
        return CS.new(self.variable, self.units, self.bins, self.n_events, 
                self.content, self.pid, self.cs_dist, self.cs_prob, self.cs_tol,
                self.n_bins)

    def run_test(self, test_bunch):
        """
        Run the cs test on the bunch. Return value is a new ks_test with
        internal state updated to reflect new test data
        """
        hist   = test_bunch.histogram_var_bins(self.variable, self.bins, 
                                                                     self.units)
        cs_test_out = self.deepcopy()
        cs_test_out.n_events = len(test_bunch)
        cs_test_out.content  = [0.]
        for i in hist[0]:
            cs_test_out.content.append(i[0]+cs_test_out.content[-1])
        del cs_test_out.content[0]
        if cs_test_out.content[-1] == 0:
            cs_test_out.content[-1] = 1.
        for i in range(len(cs_test_out.content)):
            cs_test_out.content[i] /= cs_test_out.content[-1] #normalise to 1
        cs_test_out.cs_dist = 0.
        for i in range( len(cs_test_out.content) ):
            cs_dist = abs(cs_test_out.content[i] - self.content[i])
            if cs_dist > cs_test_out.cs_dist:
                cs_test_out.cs_dist = cs_dist
        if cs_test_out.n_events+self.n_events == 0: 
            cs_dist_mod = 999.
        else:
            cs_dist_mod = cs_test_out.ks_dist*\
                        ((cs_test_out.n_events*self.n_events)/\
                         (cs_test_out.n_events+self.n_events))**0.5
       # cs_test_out.cs_prob = ROOT.TMath.ChiSquared(ks_dist_mod) # pylint: disable=E1101, C0301
        return cs_test_out

    def test_result(self):
        """Return whether the test passed or failed"""
        if self.cs_prob < self.cs_tol:
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

  

    def hist_width(fit_test_list):
        """
        Return the maximum and minimum of the histogram
        """
        (lower, upper) = None, None
        for fit_test in fit_test_list:
            if lower == None or fit_test.bins[0] < lower:
                lower = fit_test.bins[0]
            if upper == None or fit_test.bins[-1] > upper:
                upper = fit_test.bins[-1]
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





####################


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

