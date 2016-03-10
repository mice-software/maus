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

import os
from Configuration import Configuration
import json
import ROOT
import libxml2
from math import sin, cos, pi
from array import array

class ElementRotationTranslation: #pylint: disable = R0903, R0902, C0103, E1101
    """
    @class ElementRotationTranslation: This class fits for translations and rotations of
    a set of reference points in the detector system to place the detector in
    the hall coordinate system.

    Two different sets of points, with the same ordering, needs to be provided to run the
    fitting algorithm. The configuration file is used to indicate the input files.

    Output still to be determined.
    """
    # initialize class
    def __init__(self, base_dir=''): #pylint: disable = R0903, R0902, C0103
        """
        Initialize arrays and arguments
        """
        # initialize input data arrays
        self.refpoints  = []
        self.datapoints = []
        self.initguess  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
        # access configuration file
        inputs = Configuration().getConfigJSON(command_line_args = True)
        config_dict = json.loads(inputs)
        # get the gdml directory
        self.dl_dir = base_dir
        if base_dir == '':
            self.dl_dir = os.path.join(
                config_dict['geometry_download_directory'],'gdml')
            
        # 
        # extract data files from configuration files
        # 
        self.GDMLFile = ''
        self.XMLFile = ''
        self.DataFile = ''
        self.RefFile  = ''
        # Override methods
        self.DataFile = config_dict['survey_measurement_record']
        self.RefFile = config_dict['survey_reference_position']
        
        self.UseGDML  = config_dict['use_gdml_source']
        self.tempList = config_dict['survey_target_detectors']
        # Fitting tolerance. Reject fit if chi^2/ndof > value
        self.tolerance = 3 # this should be read from the CDB files
        # detector positions to be fit.
        self.sfTarget = []
        # default ndof. 6 parameters in fit
        self.ndof = -6.0
        self.datafile = None
        self.gdmlfile = None
        if self.UseGDML:
            self.defineGDMLVariables()
            
        self.result = []

    def defineGDMLVariables(self):
        """
        setup gdml file, parse and verify it
        """
        # The survey information is in the CAD geometry
        # file which should have one of these names
        fnames = ['fastradModel.gdml', 'FastradModel.gdml', 'Step_IV.gdml']
        for name in fnames:
            tempfile = os.path.join(self.dl_dir, name)
            if os.path.exists(tempfile):
                self.GDMLFile = tempfile
                # break on the first qualifing instance.
                break
        # An override method --- if necessary
        if self.DataFile != "":
            self.GDMLFile = self.DataFile
        # The default source of reference information for the fit.
        self.XMLFile  = os.path.join(self.dl_dir,'Maus_Information.gdml')
        if not os.path.exists(self.XMLFile):
            self.XMLFile = ''
        # An override method --- if necessary
        if self.RefFile != "":
            self.XMLFile  = self.RefFile
        # print 
        print self.GDMLFile, self.XMLFile
        self.datafile = libxml2.parseFile(self.XMLFile)
        self.gdmlfile = libxml2.parseFile(self.GDMLFile)
        # get the survey target detectors
        # The list of active detectors should appear in
        # the Maus_Information file under the following path
        basepath = 'MICE_Information/Detector_Information/'
        # This is a list of all possible detectors (which
        # have GDML geometries).
        detectors = ['TOF0', 'TOF1', 'TOF2', \
                     'KL', 'Ckov1', 'Ckov2', 'EMR', \
                     'Tracker0', 'Tracker1', 'LH2', \
                     'Disk_LiH', 'Disk_PE', 'Wedge_LiH_45', \
                     'Wedge_LiH_90']
        # Evaluate the set of entries consistant with the path
        detector_search = self.datafile.xpathEval(basepath+'/*')
        # initialize the container for the detector target
        self.sfTarget = []
        # loop over the path elements
        for elem in detector_search:
            # loop over the potential detector names
            for det in detectors:
                # check for matches between the list
                if det == elem.name:
                    # add the detector name to the list
                    self.sfTarget.append(det)
        if not len(self.sfTarget):
            # check for an alternate list if no detectors match. 
            self.sfTarget = self.tempList

    def execute(self): #pylint: disable = R0903, R0902, C0103
        """
        @method execute: Fit for all detectors indicated by the survey target list in the config file
        """
        
        # A check (possibly redundant) to see if we will do anything
        if not len(self.sfTarget):
            return False
        # Run over all elements in the surveyed detectors
        isgood = True
        basepath = 'MICE_Information/Detector_Information/'
        for target in self.sfTarget:
            targetpath = os.path.join(basepath, target)
            if target.find('TOF') >= 0:
                targetpath = os.path.join(basepath, 'TOF/'+target)
            elif target.find('Ckov') >= 0:
                targetpath = os.path.join(basepath, 'Cherenkov/'+target)
            elif target.find('Tracker') >=0:
                targetpath = os.path.join(basepath, 'Tracker/'+target)
            if not self.FitForDetector(targetpath, target+'.gdml'):
                isgood = False
                print 'Fit Failed for '+target
        self.writefile()        
        return isgood

    def FitForDetector(self, detectorPath, detectorFile):
        #pylint: disable = R0903, R0902, C0103
        """
        @method FitForDetectors: A subroutine to execute the fit based on a set of input data points.
        """
        isgood = True
        self.ndof = -6
        # extract the measured data
        # if the GDML file is to be used, a list of files associated with the
        # survey points must be provided. Order must match reference points. 
        if self.UseGDML:
            isgood = self.extractDatafromGDML(detectorPath)
        # otherwise use a list of data points provided
        else:
            if not self.extractData():
                isgood = False
            # extract the detector reference points  
            if not self.extractRefPoints():
                isgood = False
            # make sure that the set of data and reference points are matched
            elif not self.sensibleDataSet():
                isgood = False
        # if all checks out then run the fit itself
        if isgood:
            self.result = self.FitQP()
            # print  self.result
            # if self.FitQP() == 0: isgood = True
            # else: isgood = False
        if len(self.result) == 3 and self.UseGDML:
            if not self.writeResulttoGDML(isgood, detectorPath, detectorFile):
                print "Result not recorded to "+detectorPath
                exit(1)
                # print  "GDML corrected"
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
            if line[0] == '#':
                continue
            m = line.split()
            if len(m) != 6:
                print 'Error: data invalid for the following line:'
                print  line
                isgood = False
                return isgood
            # otherwise the prospective data point is good.
            # write the file content to the datapoints array
            # print m
            data   = array('d',(float(m[0]), float(m[1]), float(m[2])))
            errors = array('d',(float(m[3]), float(m[4]), float(m[5])))
            self.datapoints.append([data, errors])
            
        return isgood

    def extractDatafromGDML(self, path): #pylint: disable = C0301
        """
        @method extractData: Get the survey point positions from a GDML file.
        """
        isgood = True
        # path = "DetectorInformation/TOF/TOF0"
        print path
        det = self.datafile.xpathEval(path)
        # if len(det)==0:
        for q in self.gdmlfile.xpathEval(\
            "gdml/structure/volume/physvol/position"):
            
            if q.prop("name") == det[0].prop("gdml_posref"):
                self.initguess[0] =  float(q.prop('x'))
                self.initguess[1] =  float(q.prop('y'))
                self.initguess[2] =  float(q.prop('z'))
        # print  "Initial guess for detector location is ", self.initguess
        # clear all previous reference and data points
        self.refpoints  = []
        self.datapoints = []
        surveylist = self.datafile.xpathEval(path + "/Survey")
        if len(surveylist)==0:
            isgood = False
            print 'No survey points identified for '+path
            return isgood
        
        for node in surveylist:
            # Extract the expected survey point position from the xml file
            temp = array('d', (float(node.prop('x')), \
                               float(node.prop('y')), \
                              -float(node.prop('z'))))
            # print  "Location of survey point in detector coordinates: ", temp
            self.refpoints.append(temp)
            
            # Search for the position for
            # the indicated file name. This is the measured position
            # of the survey point.
            data = array('d', (0., 0., 0.) )
            error = array('d', (1.0, 1.0, 1.0) ) # assume a 1 mm resolution
            ql = self.gdmlfile.xpathEval(
                "gdml/structure/volume/physvol/position")
            for q in ql:
                # print  q.prop("name"), node.prop("gdml_posref")
                if q.prop("name") == node.prop("gdml_posref"):
                    data[0] = float(q.prop('x'))
                    data[1] = float(q.prop('y'))
                    data[2] = float(q.prop('z'))
                    break
            # print node.prop('err')
            error[0] = float(node.prop('err'))
            error[1] = float(node.prop('err'))
            error[2] = float(node.prop('err'))
            # print  "Survey point in global coordinates: ", data
            
            self.datapoints.append([data, error])
            # also add to degrees of freedom
            self.ndof += 3
        if self.ndof <= 0:
            print "Not enough degrees of freedom for fit"
            isgood = False
        return isgood
    
    def extractRefPoints(self):
        """
        @method extractRefPoints: Get the detector reference point positions from a text file.
        To be used for debugging purposes.
        """
        isgood = True
        reffile = open(self.RefFile)
        for line in reffile:
            if line[0] == '#':
                continue
            m = line.split()
            if len(m) != 3:
                print 'Error: reference points invalid for the line:'
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
            print '     This may result in an incompatability of the input '
            print '     data and reference points.'
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
        
    def FitQP(self): #pylint: disable = E1101, R0914
        """
        @method FitQP: This method uses the ROOT Minuit algorithms
        to minimize the rotations and translations of the detector.
        """
        gMinuit = ROOT.TMinuit(5)
        gMinuit.SetFCN( self.fcn )
        gMinuit.SetPrintLevel(-1) # Shut up Minuit - AD 2016-02-11

        arglist = array( 'd', 10*[0] )
        ierflag = ROOT.Long(1982)

        arglist[0] = 1
        gMinuit.mnexcm( "SET ERR", arglist, 1, ierflag )

        # Starting values and step sizes for parameters
        vstart = self.initguess # array( 'd', ( 0., 0., 0., 0., 0., 0.) )
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
        # pylint: disable=W0612
        amin, edm, errdef = \
              ROOT.Double(0.18), ROOT.Double(0.19), ROOT.Double(0.20)
        nvpar, nparx, icstat = \
               ROOT.Long(1981), ROOT.Long(1983), ROOT.Long(1986)
        # Far too noisy, commenting out - AD 2016-02-11
        # gMinuit.mnstat( amin, edm, errdef, nvpar, nparx, icstat )
        # gMinuit.mnprin( 3, amin )

        # Get the output parameters
        param, errors = array( 'd' ), array( 'd' )
        temp, terr  = ROOT.Double(0.), ROOT.Double(0.)
        for i in range(6):
            gMinuit.GetParameter(i, temp, terr)
            param.append(temp)
            errors.append(terr)
        return [param, errors, amin]
    

    
    def ApplyRotations(self, data, pars): #pylint: disable = R0201
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
        
        xf = array('d', xi)
        xf[0] = x0[0] + cy*cz*xi[0] - cy*sz*xi[1] + sy*xi[2]
        xf[1] = x0[1] + (cx*sz + sx*sy*cz)*xi[0] + \
                (cx*cz - sx*sy*sz)*xi[1] - sx*cy*xi[2]
        xf[2] = x0[2] + (sx*sz - cx*sy*cz)*xi[0] + \
                (sx*cz + cx*sy*sz)*xi[1] - cx*cz*xi[2]

        return xf

    def fcn( self, npar, gin, f, par, iflag ):
        #pylint: disable = W0613, R0913, C0111
        npoints = len(self.refpoints)

        # calculate chisquare of translated points
        chi2, delta2, sigma2 = 0., 0., 0.
        for i in range(npoints):
            rotatedpoints = self.ApplyRotations(self.refpoints[i], par)
            
            for j in range(len(rotatedpoints)):
                delta2 += (self.datapoints[i][0][j] - rotatedpoints[j]) \
                          * (self.datapoints[i][0][j] - rotatedpoints[j])
                sigma2 += self.datapoints[i][1][j] * self.datapoints[i][1][j]
                
            chi2 += delta2 / sigma2 
            delta2, sigma2 = 0., 0.

        f[0] = chi2

    def writeResulttoGDML(self, isgood, path, newFile):
        """
        @method writeResulttoGDML: A method to write out the result of
        a fit between a set of survey measurements and the reference points in
        the detector frame to a GDML file describing the experimental geometry.
        """
        
        # path = "Detector_Information/TOF/TOF0"
        det = self.datafile.xpathEval(path)
        if len(det)==0:
            print "Path not found in Information file."
            return False
        else:
            PosRefName = det[0].prop("gdml_posref")
        # print  "Write results of "+path+" to GDML file in "+PosRefName
        
        if self.result[2] / self.ndof > self.tolerance :
            print "Fit Tolerance test failed: chi-square = ", self.result[2], \
                  " for ", self.ndof, " degrees of freedom."
            isgood = False
            
        nodefound = 0
        for node in self.gdmlfile.xpathEval("gdml/structure/volume/physvol"):
            # print  "testing"+node.xpathEval("position")[0].prop("name")
            if node.xpathEval("position")[0].prop("name") == PosRefName:
                # print  "Reference Found"
                if newFile and \
                       os.path.exists(os.path.join(self.dl_dir, newFile)):
                    filepath = node.xpathEval("file")[0]
                    filepath.setProp('name', newFile)
                if isgood: # the fit has been conducted and the result recorded
                    q = node.xpathEval("position")[0]
                    q.setProp('x', str(self.result[0][0]))
                    q.setProp('y', str(self.result[0][1]))
                    q.setProp('z', str(self.result[0][2]))
                    line    = PosRefName
                    rotName = line.replace('pos','rot')
                    # print  "Position corrected"
                    nodefound = 1
                    if len(node.xpathEval("rotation")):
                        r = node.xpathEval("rotation")[0]
                        nodefound = 1
                        r.setProp('x', str(self.result[0][3]))
                        r.setProp('y', str(self.result[0][4]))
                        r.setProp('z', str(self.result[0][5]))
                    else:
                        # if len(node.xpathEval("rotationref"))==1:
                        #     rotref = node.xpathEval("rotationref")[0]
                        #     rotref.unlinkNode()
                        #     rotref.freeNode()
                        rotNode = libxml2.newNode("rotation")
                        rotNode.setProp('name', rotName)
                        rotNode.setProp('x', str(self.result[0][3] * 180/pi))
                        rotNode.setProp('y', str(self.result[0][4] * 180/pi))
                        rotNode.setProp('z', str(self.result[0][5] * 180/pi))
                        rotNode.setProp('unit','degree')
                        q.addNextSibling(rotNode)
                        # print  "Rotation Written"
                break
                # print q.next()
        if nodefound == 0:
            isgood = False
            print "Target node "+PosRefName+" in path "+ path \
                  +" not found in GDML file." 
        # newfile = self.GDMLFile.replace('.gdml','_ed.gdml')
        return isgood

    def writefile(self): #pylint: disable = C0111, W0104
        f = open(self.GDMLFile,'w')
        self.gdmlfile.saveTo(f)
        # print  self.gdmlfile
        f.close()
        self.gdmlfile.freeDoc()
        

                
def testfit():
    """
    A simple test of the algorithm developed to fit for rotations in the elements of the geometry.
    """
    # Instatiate the class
    fitter = ElementRotationTranslation()
    fitter.execute()

if __name__ == "__main__":
    testfit()
