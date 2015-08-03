"""
R. Bayes
"""
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

#pylint: disable=C0103, R0903, W0232, E0611, R0914, R0915, W0612

import unittest
import os
# import geometry
# from geometry.ConfigReader import Configreader
from geometry.LocationFit import  ElementRotationTranslation
from ROOT import TRandom3
from ROOT import TH1D, TFile


class test_location_fit(unittest.TestCase): #pylint: disable = C0103, R0904
    """
    class test_location_fit This class tests LocationFit.py

    the is the unit test which verifies LocationFit.py is working correctly
    """
    def setUp(self): #pylint: disable = C0103
        """
        test_location_fit::setUp This sets some variables for the tests
        """
        self.maus = os.environ['MAUS_ROOT_DIR']
        self.constructor = None
        # test_locationfit = \
        #     'tests/py_unit/test_geometry/testCases/testLocationFit/'
        # fourPoints = 'MICE_Information/Detector_Information/EMR'
        self.fittest = ElementRotationTranslation()

def testFittingPulls(): #pylint disable=C0324
    """
    check fits and pull distributions
    """
    # Initialize the random number generator
    rand = TRandom3()
    # Intialize histograms for the fit validation
    hx = TH1D("hpullx",
              ";Position Pull X (mm);Entries per 0.02#sigma",
              300,-10.0,10.0)
    hy = TH1D("hpully",
              ";Position Pull Y (mm);Entries per 0.02#sigma",
              300,-10.0,10.0)
    hz = TH1D("hpullz",
              ";Position Pull Z (mm);Entries per 0.02#sigma",
              300,-10.0,10.0)
    hthx = TH1D("hpullthx",
                ";Rotation Pull X (mm);Entries per 0.02#sigma",
                300,-10.0,10.0)
    hthy = TH1D("hpullthy",
                ";Rotation Pull Y (mm);Entries per 0.02#sigma",
                300,-10.0,10.0)
    hthz = TH1D("hpullthz",
                ";Rotation Pull Z (mm);Entries per 0.02#sigma",
                300,-10.0,10.0)
    # Initialize the fitting algorithm
    base = os.path.expandvars("${MAUS_ROOT_DIR}/tests/py_unit/test_geometry"+\
                              "/testCases/testLocationFit/")
    fittest = ElementRotationTranslation(base)
    # Get the data from the GDML file as
    # required by the configuration
    fourPoints = 'MICE_Information/Detector_Information/EMR'
    isgood = fittest.extractDatafromGDML(fourPoints)
    # Get the data from the fitting object
    data = fittest.datapoints
    init = fittest.initguess
    # define the expected resolution for resampling the data
    sigmax = sigmay = sigmaz = 5. # 5mm position resolution
    sthetax = sthetay = sthetaz = 0.5 # 200 mrad
    drnoise = 10. # mm position resolution
    # Loop over a finite number of resampled fits
    i = 0
    while i < 1000:
        # Sample the random distributions
        dx = rand.Gaus(0, sigmax)
        dy = rand.Gaus(0, sigmay)
        dz = rand.Gaus(0, sigmaz)
        dtx = rand.Gaus(0, sthetax)
        dty = rand.Gaus(0, sthetay)
        dtz = rand.Gaus(0, sthetaz)
        par0 = [dx, dy, dz, dtx, dty, dtz]
        newdatapoints = []
        # Rotate the data based on the random variations
        for point in data:
            rotpoint = fittest.ApplyRotations(point[0], par0)
            # also apply random noise in positions
            rotpoint[0] += rand.Gaus(0, drnoise)
            rotpoint[1] += rand.Gaus(0, drnoise)
            rotpoint[2] += rand.Gaus(0, drnoise)
            newdatapoints.append([rotpoint, point[1]])
        fittest.datapoints = newdatapoints
        # Run the fit and extract the result
        result = fittest.FitQP()
        # self.assertTrue(len(result) == 3)
        # self.assertTrue(result[2]/
        #              (3*(len(newdatapoints) - len(par0))) < 3)
        # Now fill the histograms with the pulls
        hx.Fill( (result[0][0] - (init[0] + dx)) / result[1][0] )
        hy.Fill( (result[0][1] - (init[1] + dy)) / result[1][1] )
        hz.Fill( (result[0][2] - (init[2] + dz)) / result[1][2] )
        hthx.Fill( (result[0][3] - dtx) / result[1][3] )
        hthy.Fill( (result[0][4] - dty) / result[1][4] )
        hthz.Fill( (result[0][5] - dtz) / result[1][5] )
        # iterate the counter
        i += 1

    # Check the behaviour of the pull histogram
    print 'Mean of x position is ', hx.GetMean(), ' with width ', \
          hx.GetRMS()
    print 'Mean of y position is ', hy.GetMean(), ' with width ', \
          hy.GetRMS()
    print 'Mean of z position is ', hz.GetMean(), ' with width ', \
          hz.GetRMS()
    print 'Mean of x rotation is ', hthx.GetMean(), ' with width ', \
          hthx.GetRMS()
    print 'Mean of y rotation is ', hthy.GetMean(), ' with width ', \
          hthy.GetRMS()
    print 'Mean of z rotation is ', hthz.GetMean(), ' with width ', \
          hthz.GetRMS()
        
    f = TFile("SurveyFitPull.root","recreate")
    hx.Write()
    hy.Write()
    hz.Write()
    hthx.Write()
    hthy.Write()
    hthz.Write()
    f.Close()

if __name__ == "__main__":
    testFittingPulls()
