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


import os
import unittest
import json
import geometry
from Configuration import Configuration
from geometry.LocationFit import  ElementRotationTranslation


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
        test_locationfit = '/tests/py_unit/test_geometry/testCases/testLocationFit/'
        
        data_dir = self.maus + test_locationfit
        # Initialize the fitting algorithm
        self.fittest = ElementRotationTranslation(data_dir)
        
        # define the expected resolution for resampling the data
        self.sigmax = self.sigmay = self.sigmaz = 5. # 5mm position resolution
        self.sthetax = self.sthetay = self.sthetaz = 0.5 # 200 mrad
        self.drnoise = 10. # mm position resolution
        
        
    def testFits(self):
        """
        test_location_fit::testFits test for the basic functions of the fit.
        """
        # Get the data from the GDML file as
        # required by the configuration
        fourPoints = 'MICE_Information/Detector_Information/EMR'
        try:
            self.assertTrue(not self.fittest.extractDatafromGDML(fourPoints),"Data successfully read")
        except:
            pass
        # Get the data from the fitting object
        data = self.fittest.datapoints
        init = self.fittest.initguess
        
        # Basic, unaltered, fit
        result = self.fittest.FitQP()
        try:
            self.assertTrue(len(result)!=3,"Fit successfully completed")
        except:
            pass
        
        # Setup and attempt a fit that will produce nonsense
        
        # Fixed corrections of a significant magnitude
        dx =  3 * self.sigmax
        dy = -3 * self.sigmay
        dz =  2 * self.sigmaz
        dtx = 3 * self.sthetax
        dty =-3 * self.sthetay
        dtz = 2 * self.sthetaz
        par0 = [dx, dy, dz, dtx, dty, dtz]
        newdatapoints = []
        # Rotate the data based on the random variations
        signshift = -1.0
        for point in data:
            try: 
                rotpoint = self.fittest.ApplyRotations(point[0], par0)
                self.assertEqual(rotpoint, None, "Result should be filled")
            except: #pylint: disable = W0702 
                pass #pylint: disable = W0702 
            # also apply shifts in position
            rotpoint[0] += 3.0 * signshift * self.drnoise
            signshift *= -1.0
            rotpoint[1] += 3.0 * signshift * self.drnoise
            signshift *= -1.0
            rotpoint[2] += 3.0 * signshift * self.drnoise
            signshift *= -1.0
            newdatapoints.append([rotpoint, point[1]])
        self.fittest.datapoints = newdatapoints
        try:
            result = self.fittest.FitQP()
            success = result[2] / self.fittest.ndof \
                      < self.fittest.tolerance
            self.assertTrue(success,"Fit should be outside of tolerance")
        except:
            pass

if __name__ == '__main__':
    unittest.main()
