"""
R. Bayes

LocationFit contains a class that manipulates the measurements of the
detector positions to determine their offset and rotation with respect
to a set of reference points defining the location of the detector
within the CAD geometry coordinate system.

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


from Configuration import Configuration
import json
import ROOT
import libxml2
from math import sin, cos
from array import array

class ElementRotationTranslation: #pylint: disable = R0903, R0902
    """
    @class ElementRotationTranslation: This class fits for translations and rotations of
    a set of reference points in the detector system to place the detector in
    the hall coordinate system.

    Two different sets of points, with the same ordering, needs to be provided to run the
    fitting algorithm. The configuration file is used to indicate the input files.

    Output still to be determined.
    """
    # initialize class
    def __init__(self):
        # initialize input data arrays
        self.refpoints = []
        self.datapoints = []
        # access configuration file
        inputs = Configuration().getConfigJSON(command_line_args = True)
        config_dict = json.loads(inputs)
        # extract data files from configuration files
        self.DataFile = config_dict['survey_measurement_record'] # 'Rotation_test.dat
        self.Reffile  = config_dict['survey_reference_position'] # 'Rotation_test.reference'
        self.UseGDML  = config_dict['use_gdml_source']
        if self.UseGDML:
             self.GDMLFile = config_dict['survey_measurement_record']
        self.result = []

    def execute(self):
        """
        @method execute: A subroutine to execute the fit based on a set of input data points.
        """
        isgood = True
        # extract the measured data
        # if the GDML file is to be used, a list of files associated with the
        # survey points must be provided. The order must match the reference points. 
        if self.UseGDML:   isgood = self.extractDatafromGDML()
        # otherwise use a list of data points provided
        elif not self.extractData():     isgood = False
        # extract the detector reference points  
        if not self.extractRefPoints():  isgood = False
        # make sure that the set of data and reference points are matched
        elif not self.sensibleDataSet(): isgood = False
        # if all checks out then run the fit itself
        if isgood:
            self.result = self.FitQP()
            print self.result
            # if self.FitQP() == 0: isgood = True
            # else: isgood = False
            if len(self.result) ==  2 and self.UseGDML:
                self.writeResulttoGDML()
        else: #
            print "Fit abandoned"

        
    
        return isgood
    
    def extractData(self):
        """
        @method extractData: Get the survey point positions from a text file.
        """
        isgood = True
        datafile = open(self.DataFile)
        for line in datafile:
            if line[0] == '#': continue
            m = line.split()
            if len(m) != 6:
                print 'Error: data invalid for the following line:'
                print line
                isgood = False
                return isgood
            # otherwise the prospective data point is good.
            # write the file content to the datapoints array
            # print m
            data   = array('d',(float(m[0]), float(m[1]), float(m[2])))
            errors = array('d',(float(m[3]), float(m[4]), float(m[5])))
            self.datapoints.append([data, errors])
            
        return isgood

    def extractDatafromGDML(self):
        """
        @method extractData: Get the survey point positions from a GDML file.
        """
        isgood = True
        datafile = open(self.DataFile)
        gdmlfile = libxml2.parsefile(self.GDMLFile)
        for line in datafile:
            if line[0] == '#': continue
            m = line.split()
            if len(m) != 1:
                print 'Error: expected one arguement on the following line:'
                print line
                isgood = False
                return isgood
            # Otherwise search for the position for
            # the indicated file name. This is the measured position
            # of the survey point.
            data = array('d', (0., 0., 0.) )
            error = array('d', (1.0, 1.0, 1.0) ) # assume a 1 mm resolution
            for q in gdmlfile.xpathEval("gdml/structure/volume/physvol/position"):
                if q.prop("name") == line:
                    data[0] = q.prop('x')
                    data[1] = q.prop('y')
                    data[2] = q.prop('z')
            self.datapoints.append([data, error])
        return isgood
    
    def extractRefPoints(self):
        """
        @method extractRefPoints: Get the detector reference point positions from a text file.
        """
        isgood = True
        if False:  # self.UseCDB:
            print 'Error: CDB record of survey reference has not yet been implemented'
            isgood = False
            return isgood
        else: # read from data file
            reffile = open(self.Reffile)
            for line in reffile:
                if line[0] == '#': continue
                m = line.split()
                if len(m) != 3:
                    print 'Error: reference points invalid for the following line:'
                    print line
                    isgood = False
                    return isgood
                # otherwise the prospective reference point is good.
                # write the file content to the refpoints array
                data = array('d',(float(m[0]), float(m[1]), float(m[2])))
                self.refpoints.append(data)
        return isgood
    
    def sensibleDataSet(self):

        """ @method sensibleDataSet: Need to assume that the data is
        composed of a matched set of measured points and reference
        points because we cannot assume that the two are in the same
        coordinate system.
        
        Should check that there are the same number of elements in the two arrays
        """
        isgood = True
        if len(self.refpoints) != len(self.datapoints):
            isgood = False
            print 'Warning: input arrays will be shortened for consistency.'
            print '    This may result in an incompatability of the input data and reference points.'
            # Shorten the longer array
            if len(self.refpoints) > len(self.datapoints):
                # check the difference in length
                diff = len(self.refpoints) - len(self.datapoints)
                # remove the elements from the array
                for i in range(-diff, -1):
                    self.refpoints.pop(i)
            else:
                # check the difference in length
                diff = len(self.datapoints) - len(self.refpoints)
                # remove the elements from the arra
                for i in range(-diff, -1):
                    self.datapoints.pop(i)
                
        return isgood
        
    def FitQP(self):
        """
        @method FitQP: This method uses the ROOT Minuit algorithms
        to minimize the rotations and translations of the detector.
        """
        gMinuit = ROOT.TMinuit(5)
        gMinuit.SetFCN( self.fcn )

        arglist = array( 'd', 10*[0] )
        ierflag = ROOT.Long(1982)

        arglist[0] = 1
        gMinuit.mnexcm( "SET ERR", arglist, 1, ierflag )

        # Starting values and step sizes for parameters
        vstart = array( 'd', ( 0., 0., 0., 0., 0., 0.) )
        step   = array( 'd', (0.01, 0.01, 0.01, 0.001, 0.001, 0.001) )
        gMinuit.mnparm( 0, "dx",      vstart[0], step[0], 0, 0, ierflag )
        gMinuit.mnparm( 1, "dy",      vstart[1], step[1], 0, 0, ierflag )
        gMinuit.mnparm( 2, "dz",      vstart[2], step[2], 0, 0, ierflag )
        gMinuit.mnparm( 3, "theta_x", vstart[3], step[3], 0, 0, ierflag )
        gMinuit.mnparm( 4, "theta_y", vstart[4], step[4], 0, 0, ierflag )
        gMinuit.mnparm( 5, "theta_z", vstart[5], step[5], 0, 0, ierflag )

        # Minimization step #
        arglist[0] = 500
        arglist[1] = 1.
        gMinuit.mnexcm( "MIGRAD", arglist, 2, ierflag )

        # print results
        amin, edm, errdef = ROOT.Double(0.18), ROOT.Double(0.19), ROOT.Double(0.20)
        nvpar, nparx, icstat = ROOT.Long(1981), ROOT.Long(1983), ROOT.Long(1986)
        gMinuit.mnstat( amin, edm, errdef, nvpar, nparx, icstat )
        gMinuit.mnprin( 3, amin )

        # Get the output parameters
        param, errors = array( 'd' ), array( 'd' )
        temp, terr  = ROOT.Double(0.), ROOT.Double(0.)
        for i in range(6):
            gMinuit.GetParameter(i, temp, terr)
            param.append(temp)
            errors.append(terr)
            
        return [param, errors]
    

    
    def ApplyRotations(self, data, pars):
        """
        A somewhat general method of applying translations and rotations with the matrices explicitly written.

        Inputs are the space point in 3 Dimensions and the rotation/translation parameters.
        """
        x0 = array('d', (pars[0], pars[1], pars[2]))
        cx = cos(pars[3])
        cy = cos(pars[4])
        cz = cos(pars[5])
        sx = sin(pars[3])
        sy = sin(pars[4])
        sz = sin(pars[5])

        xi = data
        # print 'x0[0] = ', x0[0],', cosy = ', cy,', cosz = ', cz,'xi[0] = ', xi[0],', sinz = ', sz, 'xi[1] = ', xi[1],', siny = ', sy,'xi[2] = ', xi[2] 
        xf = array('d', xi)
        xf[0] = x0[0] + cy*cz*xi[0] - cy*sz*xi[1] + sy*xi[2]
        xf[1] = x0[1] + (cx*sz + sx*sy*cz)*xi[0] + (cx*cz - sx*sy*sz)*xi[1] - sx*cy*xi[2]
        xf[2] = x0[2] + (sx*sz - cx*sy*cz)*xi[0] + (sx*cz + cx*sy*sz)*xi[1] - cx*cz*xi[2]

        return xf

    def fcn( self, npar, gin, f, par, iflag ):
        npoints = len(self.refpoints)

        # calculate chisquare of translated points
        chi2, delta2, sigma2 = 0., 0., 0.
        for i in range(npoints):
            rotatedpoints = self.ApplyRotations(self.refpoints[i], par)
            
            for j in range(len(rotatedpoints)):
                delta2 += (self.datapoints[i][0][j] - rotatedpoints[j]) * (self.datapoints[i][0][j] - rotatedpoints[j])
                sigma2 += self.datapoints[i][1][j] * self.datapoints[i][1][j]
                
            chi2 += delta2 / sigma2 
            delta2, sigma2 = 0., 0.

        f[0] = chi2

    def writeResulttoGDML(self):
        """
        @method writeResulttoGDML: A method to write out the result of
        a fit between a set of survey measurements and the reference points in
        the detector frame to a GDML file describing the experimental geometry.
        """
        isgood = True
        datafile = open(self.DataFile)
        gdmlfile = libxml2.parsefile(self.GDMLFile)
        for line in datafile:
            if line[0] == '#': continue
            m = line.split()
            if len(m) != 1:
                print 'Error: expected one arguement on the following line:'
                print line
                isgood = False
                return isgood
            # Otherwise search for the position for
            # the indicated file name. This is the measured position
            # of the survey point.
            data = array('d', (0., 0., 0.) )
            error = array('d', (1.0, 1.0, 1.0) ) # assume a 1 mm resolution
            for q in gdmlfile.xpathEval("gdml/structure/volume/physvol/position"):
                if q.prop("name") == line:
                    x0 = q.prop('x')
                    q.setProp('x', x0 + self.result[0][0])
                    y0 = q.prop('y')
                    q.setProp('y', y0 + self.result[0][1])
                    z0 = q.prop('z')
                    q.setProp('z', z0 + self.result[0][2])
                    rotNode = libxml2.newNode("rotation")
                    rotName = line.replace('pos','rot')
                    rotNode.setProp('name',rotName)
                    rotNode.setProp('x', self.result[0][3])
                    rotNode.setProp('y', self.result[0][4])
                    rotNode.setProp('z', self.result[0][5])
        newfile = self.GDMLFile.replace('.gdml','_ed.gdml')
        f = open(newfile,'w')
        gdmlfile.saveTo(f)
        f.close
        gdmlFile.freeDoc()
        
        return isgood        
                
def testfit():
    """
    A simple test of the algorithm developed to fit for rotations in the elements of the geometry.
    """
    # Instatiate the class
    fitter = ElementRotationTranslation()
    fitter.execute()

if __name__=="__main__":
    testfit()
