"""
M. Littlefield
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
import libxml2
import libxslt
import math
import numpy

class CADImport: #pylint: disable = R0903, C0103
    """
    @Class CADImport, GDML parser class.
    
    This class has been designed to parse XML(GDML) files in a number of ways.
    The first way applies an XSLT stylesheet to a GDML file which re writes the
    file into MICE module format. The output can be a text file. The second 
    parsing method is where the XSLT stylesheet is applied to two GDMLs and 
    appends the MICE Information of one GDML to the end of the geometry GMDL 
    file, taken from fastRad in order to merge geometrical information and
    field information needed for G4. This outputs the a GDML file.
    """

    def __init__(self, xmlin1, xsl=None, xmlin2=None, \
             output=None, mergein=None, mergeout=None): 
             #pylint: disable = R0912, R0913, C0103
        """
        @Method Class constructor

        This method initialises the class and takes 6 parameters. 
        The later 4 parameters are optional.
        All arguments should be file names or paths.

        @param xmlin1   first xml file name/path, used to apply an XSLT to.
        @param xsl      xslt file name/path, xslt stylesheet used to 
                        re write xml(GDML)
        @param xmlin2   second xml file name/path,  
                        used to hold the MICE info to append to geometry
        @param output   output file name/path
        @param mergein  template XSLT for append, this is the file name/path  
                        to Merge.xsl.in which is the stylesheet used to append
                        the MICE info to geometry info. This file must be 
                        altered to set the putout file name/path.
        @param mergeout file name/path to be inserted into merge.xsl.in.
        """
        if type(xmlin1) != str:
            raise TypeError(xmlin1 + " must be a string(file name/path)")
        if xmlin1[-4:] != '.xml' and xmlin1[-5:] != '.gdml':
            raise IOError(xmlin1 + " must be an xml or gdml file")
        else: 
            self.xml_in_1 = xmlin1
            self.geodir = os.path.dirname(xmlin1)

        if xsl == None: 
            self.xsl = None        
        elif type(xsl) != str:
            raise TypeError(xsl + " must be a string(file name/path")
        elif xsl[-4:] != '.xsl':
            raise IOError(xsl + " must be an xsl file")
        else: 
            self.xsl = xsl
        
        if xmlin2 == None: 
            self.xml_in_2 = None
        elif type(xmlin2) != str:
            raise TypeError(xmlin2 + " must be a string(file name/path)")
        elif xmlin2[-4:] != '.xml' and xmlin2[-5:] != '.gdml':
            raise IOError(xmlin2 + " must be an xml or gdml file")
        else: 
            self.xml_in_2 = xmlin2
            
        if output == None: 
            self.output = None
        elif type(output) != str:
            raise TypeError(output + " must be a string(file name/path)")
        else: 
            self.output = output

        if mergein == None: 
            maus_root = os.environ['MAUS_ROOT_DIR'] 
            self.merge_in = maus_root + \
            '/src/common_py/geometry/xsltScripts/Merge.xsl.in'
        elif type(mergein) != str:
            raise TypeError(mergein + " input must be a string(file name/path)")
        else: 
            self.merge_in = mergein

        if mergeout == None: 
            path = os.environ['MAUS_ROOT_DIR'] + \
            '/src/common_py/geometry/xsltScripts/Merge.xsl'
            self.merge_out = path
        elif type(mergeout) != str:
            raise TypeError(mergeout + "output must be a string filename/path)")
        elif mergeout[-4:] != '.xsl':
            raise IOError(mergeout + " must be an xsl file")
        else: 
            self.merge_out = mergeout


    def parse_xslt(self):
        """
        @Method parse_xslt to parse an XML(GDML) to text or another XML (GDML)

        This method will execute an XSLT stylesheet and produce either a text
        or another XML(GDML) according to the request.
        """
        print self.xml_in_1, self.xsl, self.output
        styledoc = libxml2.parseFile(self.xsl)
        style = libxslt.parseStylesheetDoc(styledoc)
        doc = libxml2.parseFile(self.xml_in_1)
        result = style.applyStylesheet(doc, None)
        style.saveResultToFilename(self.output, result, 0)
        style.freeStylesheet()
        doc.freeDoc()
        result.freeDoc()


    def translate_gdml(self):
        #pylint: disable = R0912, R0913, C0103
        '''
        
        @Method translate_gdml to parse an XML(GDML) to a MICE
        modules text file.
        
        This python algorithm explicitly translates a gdml
        compliant file into a MICE module compliant file. The
        reason for this approach is that xslt appears to be an
        extremely limiting protocol in terms of making use of
        internal references that are used in the GDML schema --- a
        schema must exist somewhere but it seems simpler to use
        libxml2 python bindings directly.
        
        '''
        # Parse the target gdml file
        datafile = libxml2.parseFile(self.xml_in_1)
        # extract the top level object.
        world = datafile.xpathEval("gdml/setup/world")
        # get the name of the name of the world volume (there can be only one)
        moduleName = world[0].prop("ref")
        # initialize result record
        result = []
        # recursively generate a MICE module based on the world volume
        self.AccessModule(datafile, result, moduleName)
        result.append("}\n")
        # free the memory allocated to the xml file 
        datafile.freeDoc()
        # write the resulting MICE module to a text file.
        mmfile = open(self.output, 'w+')
        for l in result:
            mmfile.write(l)
        mmfile.close()

    def AppendTubeSolid(self, datafile, result, solid):
        #pylint: disable = R0201, R0912, R0913, R0914, R0915, C0103
        '''
        @Method AppendTubeSolid to generate a MICE module from a gdml tube solid
        '''
        tube       = datafile.xpathEval("gdml/solids/tube")
        for elem in tube:
            # Extract the properties of a tube corresponding
            # to the solid reference
            if elem.prop('name') == solid:
                if elem.prop('rmin'):
                    result.append('Volume Tube\n')
                    result.append('Dimensions '+\
                                  elem.prop('rmin')+' '+\
                                  elem.prop('rmax')+' '+\
                                  elem.prop('z')+' '+\
                                  elem.prop('lunit')+'\n')
                else:
                    result.append('Volume Cylinder\n')
                    result.append('Dimensions '+\
                                  elem.prop('rmax')+' '+\
                                  elem.prop('z')+' '+\
                                  elem.prop('lunit')+'\n')
                break

    def AppendBoxSolid(self, datafile, result, solid):
        #pylint: disable = R0201, R0912, R0913, R0914, R0915, C0103
        '''
        @Method AppendBoxSolid to generate a MICE module from a gdml box solid
        '''
        box        = datafile.xpathEval("gdml/solids/box")
        for elem in box:
            # Extract the properties of a box corresponding
            # to the solid reference
            if elem.prop('name') == solid:
                result.append('Volume Box\n')
                result.append('Dimensions '+elem.prop('x')+\
                              ' '+elem.prop('y')+' '+\
                              elem.prop('z')+' '+\
                              elem.prop('lunit')+'\n')
                break

    def AppendTrdSolid(self, datafile, result, solid):
        #pylint: disable = R0201, R0912, R0913, R0914, R0915, C0103
        '''
        @Method AppendTrdSolid to generate a MICE module from a gdml trd solid
        '''
        trd      = datafile.xpathEval("gdml/solids/trd")
        for elem in trd:
            if elem.prop("name") == solid:
                result.append('Volume Trapezoid\n')
                result.append('PropertyDouble TrapezoidWidthX1 '+\
                              elem.prop('x1')+'\n')
                result.append('PropertyDouble TrapezoidWidthX2 '+\
                              elem.prop('x2')+'\n')
                result.append('PropertyDouble TrapezoidHeightY1 '+\
                              elem.prop('y1')+'\n')
                result.append('PropertyDouble TrapezoidHeightY2 '+\
                              elem.prop('y2')+'\n')
                result.append('PropertyDouble TrapezoidLengthZ '+\
                              elem.prop('z')+'\n')
                break
            
    def AppendSphereSolid(self, datafile, result, solid):
        #pylint: disable = R0201, R0912, R0913, R0914, R0915, C0103
        '''
        @Method AppendSphereSolid to generate a MICE module from a gdml sphere solid
        '''
        sphere  = datafile.xpathEval("gdml/solids/sphere")
        for elem in sphere:
            if elem.prop('name') == solid:
                result.append('Volume Sphere\n')
                result.append('Dimensions '+elem.prop('rmin')+' '+\
                              elem.prop('rmax')+' '+\
                              elem.prop('lunit')+'\n')
                break
    def AppendTorusSolid(self, datafile, result, solid):
        #pylint: disable = R0201, R0912, R0913, R0914, R0915, C0103
        '''
        @Method AppendTorusSolid to generate a MICE module from a gdml torus solid
        '''
        torus  = datafile.xpathEval("gdml/solids/torus")
        for elem in torus:
            # look for a torus object with a name
            # matching the volume
            if elem.prop('name') == solid:
                result.append('Volume Torus\n')
                result.append('PropertyDouble TorusInnerRadius '\
                              +elem.prop('rmin') +' '+elem.prop('lunit')+'\n')
                result.append('PropertyDouble TorusOuterRadius '\
                              +elem.prop('rmax') +' '+elem.prop('lunit')+'\n')
                result.append('PropertyDouble TorusRadiusOfCurvature '\
                              +elem.prop('rtor')+' '+elem.prop('lunit')+'\n')
                result.append('PropertyDouble TorusAngleStart '\
                              +elem.prop('startphi') +' '\
                              +elem.prop('aunit')+'\n')
                result.append('PropertyDouble TorusOpeningAngle '\
                              +elem.prop('deltaphi')+' '\
                              +elem.prop('aunit')+'\n')

    def AppendPolyconeSolid(self, datafile, result, solid, name):
        #pylint: disable = R0912, R0913, R0914, R0915, C0103
        '''
        @Method AppendPolyconeSolid to generate a MICE module from a gdml polycone solid
        '''
        # For a polycone, a data file should be written locally
        # based on the listed vertices
        polycone  = datafile.xpathEval("gdml/solids/polycone")
        for elem in polycone:
            # look for a polycone object with a name
            # matching the volume
            if elem.prop('name') == solid:
                result.append('Volume Polycone\n')
                points = []
                zplane = elem.xpathEval('zplane')
                # loop over the z planes of the object.
                for p in zplane:
                    point = []
                    point.append(float(p.prop('z')))
                    point.append(float(p.prop('rmin')))
                    point.append(float(p.prop('rmax')))
                    points.append(point)
                    # Create a text files containing the z plane positions
                    # particular to the volume.
                    f = open(self.output[:-4]+"_"+name+\
                             '.txt','w')
                    f.write(" NumberOfPoints \t" + str(len(points)) +"\n")
                    f.write(" Units    mm \n")
                    f.write(" Z        RInner       ZOuter\n")
                    for p in points:
                        f.write(str(p[0])+"\t"+\
                                str(p[1])+"\t"+str(p[2])+"\n")
                        # close the text file
                    f.close()

    def AppendIntersectionSolid(self, datafile, result, solid):
        #pylint: disable = R0912, R0913, R0914, R0915, C0103
        '''
        @Method AppendIntersectionSolid to generate a MICE module from a gdml intersection solid
        '''
        intersection = datafile.xpathEval("gdml/solids/intersection")
        for elem in intersection:
            # if the solid name matches the element name, we leave
            # the solid definition to the auxiliary elements
            if elem.prop('name') == solid:
                result.append('Volume Boolean\n')
                result.append('PropertyString BaseModule '+\
                              os.path.join(self.geodir,\
                                elem.xpathEval('first')[0].prop('ref'))+\
                                '.dat\n')
                # This gets fun now... The base module must be defined 
                # independent of the # existance of a GDML volume. 
                # For this reason using access module will not
                # work because that uses a reference from a volume.
                self.AccessBooleanModule(datafile, \
                                         elem.xpathEval('first')[0].prop('ref'))
                result.append('PropertyString BooleanModule1 '+\
                              os.path.join(self.geodir,\
                                elem.xpathEval('second')[0].prop('ref'))+\
                                '.dat\n')
                self.AccessBooleanModule(datafile, \
                              elem.xpathEval('second')[0].prop('ref'))
                result.append(\
                             'PropertyString BooleanModule1Type Intersection\n'
                             )
                result.append('PropertyHep3Vector BooleanModule1Pos '+\
                              elem.xpathEval('position')[0].prop('x')+' '+\
                              elem.xpathEval('position')[0].prop('y')+' '+\
                              elem.xpathEval('position')[0].prop('z')+' '+\
                              elem.xpathEval('position')[0].prop('unit')+'\n')
                result.append('PropertyHep3Vector BooleanModule1Rot '+\
                              elem.xpathEval('rotation')[0].prop('x')+' '+\
                              elem.xpathEval('rotation')[0].prop('y')+' '+\
                              elem.xpathEval('rotation')[0].prop('z')+' '+\
                              elem.xpathEval('rotation')[0].prop('unit')+'\n')
                break

    def AppendSubtractionSolid(self, datafile, result, solid):
        #pylint: disable = R0912, R0913, R0914, R0915, C0103
        '''
        @Method AppendSubtractionSolid to generate a MICE module from a gdml subtraction solid
        '''
        subtraction = datafile.xpathEval("gdml/solids/subtraction")
        for elem in subtraction:
            if elem.prop('name') == solid:
                result.append('Volume Boolean\n')
                result.append('PropertyString BaseModule '+
                              os.path.join(self.geodir,\
                                elem.xpathEval('first')[0].prop('ref'))+
                                '.dat\n')
                self.AccessBooleanModule(datafile, \
                                         elem.xpathEval('first')[0].prop('ref'))
                result.append('PropertyString BooleanModule1 '+
                              os.path.join(self.geodir,\
                                elem.xpathEval('second')[0].prop('ref'))+
                                '.dat\n')
                self.AccessBooleanModule(datafile, \
                              elem.xpathEval('second')[0].prop('ref'))
                result.append('PropertyString BooleanModule1Type Subtraction\n')
                result.append('PropertyHep3Vector BooleanModule1Pos '+\
                              elem.xpathEval('position')[0].prop('x')+' ' +\
                              elem.xpathEval('position')[0].prop('y')+' '+\
                              elem.xpathEval('position')[0].prop('z')+' '+\
                              elem.xpathEval('position')[0].prop('unit')+'\n')
                result.append('PropertyHep3Vector BooleanModule1Rot '+\
                              elem.xpathEval('rotation')[0].prop('x')+' '+\
                              elem.xpathEval('rotation')[0].prop('y')+' '+\
                              elem.xpathEval('rotation')[0].prop('z')+' '+\
                              elem.xpathEval('rotation')[0].prop('unit')+'\n')
                break

    def AccessBooleanModule(self, datafile, moduleName):
        #pylint: disable = R0912, R0913, R0914, R0915, C0103
        '''
        @Method AccessBoolean Module to recursively generate a MICE module from a gdml file
        '''
        result = []
        result.append("Module "+moduleName+'\n')
        result.append("{\n")
        # loop over all solids in the datafile to find the indicated module name
        # This is done internal to the append solid subroutines.
        self.AppendBoxSolid(datafile, result,  moduleName)
        self.AppendTubeSolid(datafile, result,  moduleName)
        self.AppendTrdSolid(datafile, result,  moduleName)
        self.AppendSphereSolid(datafile, result, moduleName)
        self.AppendTorusSolid(datafile, result,  moduleName)
        self.AppendIntersectionSolid(datafile, result, moduleName)
        self.AppendSubtractionSolid(datafile, result, moduleName)
        result.append("}\n")
        f = open(os.path.join(self.geodir, moduleName + '.dat'), 'w')
        for line in result:
            f.write(line)
        f.close()
        
    def AccessModule(self, datafile, result, moduleName, volumenumber=-1):
        #pylint: disable = R0912, R0913, R0914, R0915, C0103
        '''
        @Method AccessModule to recursively generate a MICE module from a gdml file
        '''
        # Record the module name
        if volumenumber > -1:
            result.append("Module "+moduleName+str(volumenumber)+'\n')
        else:
            result.append("Module "+moduleName+'\n')
        result.append("{\n")
        # loop over all volumes in the datafile to find the target volume
        system = datafile.xpathEval("gdml/structure/volume")
        for vol in system:
            foundSensDet = ''
            if vol.prop('name') == moduleName:
                # find the reference to the solid used to generate the volume
                solid      = vol.xpathEval("solidref")[0].prop('ref')
                # This solid corresponds to a box, tube,
                # or other, that we do not know a priori
                # Try each in turn.
                self.AppendBoxSolid(datafile, result, solid)
                self.AppendTubeSolid(datafile, result, solid)
                self.AppendTrdSolid(datafile, result, solid)
                self.AppendSphereSolid(datafile, result, solid)
                self.AppendTorusSolid(datafile, result, solid)
                self.AppendPolyconeSolid(datafile, result, solid, moduleName)
                self.AppendIntersectionSolid(datafile, result, solid)
                self.AppendSubtractionSolid(datafile, result, solid)

                aux        = vol.xpathEval('auxiliary')
                
                unit = "cm"
                # Get the auxilary elements of the volume
                
                for elem in aux:
                    elemtype, value = \
                              elem.prop('auxtype'), elem.prop('auxvalue')
                    # Extract units for double types
                    if elemtype == 'unit':
                        unit = value
                    # Extract unitless double elements
                    elif elemtype =='RedColour' \
                           or elemtype == 'BlueColour'\
                           or elemtype == 'GreenColour' \
                           or elemtype == 'G4StepMax' \
                           or elemtype == 'Thickness[mm]' \
                           or elemtype == 'DiameterOfPmt[mm]'\
                           or elemtype == 'DiameterOfGate[mm]'\
                           or elemtype == 'XoffsetOfPmt[mm]'\
                           or elemtype == 'ZoffsetOfPmt[mm]'\
                           or elemtype == 'TrapezoidWidthX1'\
                           or elemtype == 'TrapezoidWidthX2'\
                           or elemtype == 'TrapezoidHeightY1'\
                           or elemtype == 'TrapezoidHeightY2'\
                           or elemtype == 'TrapezoidLengthZ'\
                           or elemtype == 'NbOfBars':
                        result.append('PropertyDouble '+elemtype+' '+value+"\n")
                    # Extract double typed elements with units
                    elif elemtype == 'ActiveRadius' \
                           or elemtype == 'Pitch'\
                           or elemtype == 'FibreDiameter' \
                           or elemtype == 'CoreDiameter'\
                           or elemtype == 'CentralFibre' \
                           or elemtype == 'GlueThickness'\
                           or elemtype == 'FibreLength' \
                           or elemtype == 'FibreSpacingY'\
                           or elemtype == 'FibreSpacingZ'\
                           or elemtype == 'OpticsMaterialLength'\
                           or elemtype == 'BarWidth'\
                           or elemtype == 'BarHeight'\
                           or elemtype == 'BarLength'\
                           or elemtype == 'HoleRad'\
                           or elemtype == 'Gap'\
                           or elemtype == 'FiberCladdingExtRadius':
                        result.append('PropertyDouble '+elemtype+' '\
                                      +value+' '+unit+'\n')
                    # Extract integer elements
                    elif elemtype == 'numPlanes' \
                             or elemtype == 'Station' or elemtype == 'numPMTs' \
                             or elemtype == 'Plane' or elemtype == 'Tracker' \
                             or elemtype == 'Cell' \
                             or elemtype == 'Slab'\
                             or elemtype == 'CkovPmtNum':
                        result.append('PropertyInt '+elemtype+' '+value+"\n")
                    # Extract string typed elements
                    elif elemtype == 'SensitiveDetector' \
                             or elemtype == 'Detector' \
                             or elemtype == 'G4Detector' \
                             or elemtype == 'BaseModule' \
                             or elemtype == 'BooleanModule1' \
                             or elemtype == 'BooleanModule1Type'\
                             or elemtype == 'BooleanModule2' \
                             or elemtype == 'BooleanModule2Type'\
                             or elemtype == 'FileName'\
                             or elemtype == 'PolyconeType':
                        # An exception for the case of polycone objects
                        if value == "PolyconeProfile":
                            value = self.output[:-4]+'_'+vol.prop('name')+\
                                    '.txt'
                        if elemtype == 'SensitiveDetector':
                            foundSensDet = value
                            
                        result.append('PropertyString '+elemtype+' '+value+"\n")
                    # Extract boolean elements
                    elif elemtype == ' Invisible'\
                             or elemtype == 'UseDaughtersInOptics'\
                             or elemtype == 'AddWLSFiber':
                        result.append('PropertyBool '+elemtype+' '\
                                      +value+"\n")
                    # Extract selected vector typed elements
                    elif elemtype == 'Pmt1PosX':
                        result.append('PropertyHep3Vector Pmt1Pos '\
                                      +value+' 0.0 0.0 cm\n')
                    elif elemtype == 'Pmt2PosX':
                        result.append('PropertyHep3Vector Pmt2Pos '\
                                      +value+' 0.0 0.0 cm\n')
                    elif elemtype == 'Pmt1PosY':
                        result.append('PropertyHep3Vector Pmt1Pos 0.0 '\
                                      +value+' 0.0 cm\n')
                    elif elemtype == 'Pmt2PosY':
                        result.append('PropertyHep3Vector Pmt2Pos 0.0 '\
                                      +value+' 0.0 cm\n')
                    elif elemtype == 'PMT0Pos':
                        result.append('PropertyHep3Vector PMT0Pos '\
                                      +value+' 0.0 0.0 cm\n')
                    elif elemtype == 'PMT2Pos':
                        result.append('PropertyHep3Vector PMT2Pos '\
                                      +value+' 0.0 0.0 cm\n')
                    elif elemtype == 'PMT1Pos':
                        result.append('PropertyHep3Vector PMT1Pos 0.0 '\
                                      +value+' 0.0 cm\n')
                    elif elemtype == 'PMT3Pos':
                        result.append('PropertyHep3Vector PMT3Pos 0.0 '\
                                      +value+' 0.0 cm\n')
                    # Specialized vector types
                    elif elemtype == 'BooleanModule1Pos' \
                         or elemtype == 'BooleanModule1Rot' \
                         or elemtype == 'BooleanModule2Pos' \
                         or elemtype == 'BooleanModule2Rot' \
                         or elemtype == 'Phi' or elemtype == 'Theta':
                        result.append('PropertyHep3Vector '+\
                                      elemtype+' '+value+"\n")
                    elif elemtype == "PlaceModule" and value == 'Virtual':
                        result.append('Module Virtual\n')
                        result.append(' {\n')
                        result.append('     Volume None\n')
                        result.append('     Position 0.0 0.0 0.0 mm\n')
                        result.append('     Rotation 0.0 0.0 0.0 degree\n')
                        result.append(\
                              '     PropertyString SensitiveDetector Virtual\n'\
                                     )
                        result.append(
                              '     PropertyString IndependentVariable z\n'\
                                     )
                        result.append(' }\n')
                        
                # Get the material reference     
                material = vol.xpathEval("materialref")
                result.append("PropertyString Material "+\
                              material[0].prop("ref")+"\n")
                # Access the physical volumes defined within the parent
                physvol = vol.xpathEval("physvol")
                # print "found Sens Det", foundSensDet
                if foundSensDet == 'SciFi' or foundSensDet == 'KL' \
                       or moduleName.find('KLGlue') >= 0:
                    continue
                for elem in physvol:
                    vol_path = elem.xpathEval("volumeref")
                    file_path = elem.xpathEval("file")
                    vol_name = ""
                    # Access the daughter volume
                    if len(vol_path)==1:
                        vol_name = vol_path[0].prop('ref')
                        self.AccessModule(datafile, result, vol_name, \
                                          volumenumber)
                    # if there is no volume check if there is a file reference
                    elif len(file_path)==1:
                        newfile = file_path[0].prop("name")
                        newfile = os.path.join(self.geodir, newfile[:-5])
                        result.append("Module "+newfile+".dat\n")
                        result.append("{\n")
                    # get the postion of the daughter
                    position = elem.xpathEval("position")[0]
                    result.append("Position "+\
                                  position.prop('x')+" "+\
                                  position.prop('y')+' '+\
                                  position.prop('z')+" "+\
                                  position.prop('unit') + '\n')
                    # get the rotation of the daughter
                    rotlist = elem.xpathEval("rotation")
                    # Either a rotation is defined or a reference
                    # to the identity rotation.
                    if len(rotlist):
                        rotation = elem.xpathEval("rotation")[0]
                        result.append("Rotation "+\
                                      rotation.prop('x')+" "+\
                                      rotation.prop('y')+' '+\
                                      rotation.prop('z')+" "+\
                                      rotation.prop('unit')+'\n')
                    else:
                        # If there is no rotation defined use the
                        # null rotation.
                        result.append("Rotation 0.0 0.0 0.0 degree\n")
                    # Bookkeeping for the MICE modules
                    if volumenumber > -1:
                        # If ending a replica volume add the replica number
                        result.append("} // End "+vol_name\
                                      +str(volumenumber)+"\n\n")
                    else:
                        # Add the volume identity at the end for readability
                        result.append("} // End "+vol_name+"\n\n")
                # To simplify the definition of replicated volumes replicavol
                # is available
                replicavol = vol.xpathEval("replicavol")
                for instance in replicavol:
                    # get the volume reference 
                    basename = instance.xpathEval("volumeref")[0].prop('ref')
                    # How many volumes are there?
                    nvolumes = float(instance.prop('number'))
                    # On what axis is the volume replicated
                    replica   = instance.xpathEval("replicate_along_axis")
                    # What is the change in position of the replica
                    width = replica[0].xpathEval("width")[0].prop('value')
                    deltapos  = float(width)
                    # What is the unit of the change in position
                    deltaunit = replica[0].xpathEval("width")[0].prop('unit')
                    # Some math to position the replicas in the MICE module
                    repOffset = deltapos * math.floor(nvolumes / 2.0)
                    minOffset = -repOffset
                    maxOffset = repOffset
                    # Add an offset specific to odd and even numbers of elements
                    if int(nvolumes) % 2 == 0:
                        minOffset += deltapos * 0.5
                        maxOffset += deltapos * 0.5
                    elif int(nvolumes) % 2 ==1:
                        maxOffset += deltapos 
                    volumenumber = 0
                    # Write the MICE module for each replica volume
                    for pos in numpy.arange(minOffset, maxOffset, deltapos):
                        # Write the volume from the file.
                        self.AccessModule(datafile, result, \
                                          basename, volumenumber)
                        # Add the volume number to the output stream
                        result.append("PropertyInt Slab "+\
                                      str(volumenumber)+"\n")
                        # Write the position of the volume
                        if replica[0].xpathEval("direction")[0].prop('x') \
                               == '1':
                            result.append("Position "+\
                                          str(pos)+" 0.0 0.0 "+deltaunit+"\n")
                        elif replica[0].xpathEval("direction")[0].prop('y') \
                                 == '1':
                            result.append("Position 0.0 "+\
                                          str(pos)+" 0.0 "+deltaunit+"\n")
                        # Assume that there is a null rotation... is this right
                        result.append("Rotation 0.0 0.0 0.0 degree\n")
                        result.append("} // End"+basename+\
                                      str(volumenumber)+"\n\n")
                        volumenumber += 1

    def TrackerPlaneParametrization(self):
        #pylint: disable = R0912, R0913, R0914, R0915, C0103, C0301
        '''
        
        @Method TrackerPlaneParametrization to add plane
        parametrization elements to the Tracker GDML files

        '''
        viewNames = ['TrackerViewU',
                     'TrackerViewV',
                     'TrackerViewW',
                     'trackerViewUStation5Tracker0']
        print self.xml_in_1
        for viewName in viewNames:
            # Parse the target gdml file
            datafile = libxml2.parseFile(self.xml_in_1)
            # extract the path to the solid definitions
            solids    = datafile.xpathEval("gdml/solids")
            # extract the path to the structure definitions
            structure = datafile.xpathEval("gdml/structure")
            # extract the information for the SciFiPlane Module
            # from the tracker view (W)
            volumes = structure[0].xpathEval("volume")
            auxlist = []
            solid_ref = ''
            mater_ref = ''
            doubletThickness = 0.0
            trackerRadius    = 0.0
            volumefound = 0
            print viewName
            for instance in volumes:
                print instance.prop('name')
                # find the tracker view (W)
                if instance.prop('name').find(viewName) >= 0:
                    volumefound = 1
                    # extract the auxiliary list
                    auxlist = instance.xpathEval("auxiliary")
                    # Get the solid reference name
                    solid_ref = instance.xpathEval("solidref")
                    # Get the material reference name
                    mater_ref = instance.xpathEval("materialref")
            if volumefound == 0:
                print 'Warning: did not find volume ', viewName
                continue
            # solids = structure[0].xpathEval("solid")
            print solid_ref[0].prop('ref')
            for instance in solids[0].xpathEval("tube"):
                # get the tracker view solid
                if instance.prop('name') == solid_ref[0].prop('ref'):
                    print instance.prop('name'), instance.prop('rmax'), \
                          instance.prop('z')
                    doubletThickness = float(instance.prop('z'))
                    trackerRadius   = float(instance.prop('rmax'))
            material = mater_ref[0].prop("ref")
            if len(auxlist)==0:
                print 'Failure in extraction of auxiliary data list'
                return 
            # Now that the list has been captured the contents can be sorted
            # into useful variables
            activeRadius  = 0.0
            fibrePitch    = 0.0
            fibreDiameter = 0.0
            coreDiameter  = 0.0
            centralFibre  = 0.0
            for elem in auxlist:
                if elem.prop('auxtype') == 'ActiveRadius':
                    activeRadius = float(elem.prop('auxvalue'))
                if elem.prop('auxtype') == 'Pitch':
                    fibrePitch = float(elem.prop('auxvalue'))
                if elem.prop('auxtype') == 'FibreDiameter':
                    fibreDiameter = float(elem.prop('auxvalue'))
                if elem.prop('auxtype') == 'CoreDiameter':
                    coreDiameter = float(elem.prop('auxvalue'))
                if elem.prop('auxtype') == 'CentralFibre':
                    centralFibre = float(elem.prop('auxvalue'))
            print "ActiveRadius = ", activeRadius, ", Pitch = ", fibrePitch, \
                  ", FibreDiameter = ", fibreDiameter, ", CoreDiameter = ",\
                  coreDiameter, ",CentralFibre = ", centralFibre
            datafile.freeDoc()
            # the important variables are now loaded and the new volumes
            # can be defined. Start by defining the plane solid.
            # Define a new volume
            doc = libxml2.newDoc("1.0")
            root = libxml2.newNode("gdml")
            doc.setRootElement(root)
            # Register the gdml namespace
            ns = root.newNs('http://www.w3.org/2001/XMLSchema-instance', 'xsi')
            root.setNs(ns)
            root.setNsProp(ns, 'noNamespaceSchemaLocation', \
                           "http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd")
            defNode = libxml2.newNode('define')
            root.addChild(defNode)
            matNode = libxml2.newNode('materials')
            # Hydrogen
            
            H1 = libxml2.newNode("isotope")
            H1.setProp("name", 'H10')
            H1.setProp('N', '1')
            H1.setProp('Z', '1')
            atomH1 = libxml2.newNode("atom")
            atomH1.setProp("unit", "g/mole")
            atomH1.setProp("value", "1.00782503081372")
            H1.addChild(atomH1)
            matNode.addChild(H1)
            H2 = libxml2.newNode("isotope")
            H2.setProp("name",'H20')
            H2.setProp('N','2')
            H2.setProp('Z','1')
            atomH2 = libxml2.newNode("atom")
            atomH2.setProp("unit", "g/mole")
            atomH2.setProp("value", "2.01410199966617")
            H2.addChild(atomH2)
            matNode.addChild(H2)
            H0 = libxml2.newNode("element")
            H0.setProp("name","H0")
            h0frach1 = libxml2.newNode("fraction")
            h0frach1.setProp("n", "0.999885")
            h0frach1.setProp("ref", "H10")
            H0.addChild(h0frach1)
            h0frach2 = libxml2.newNode("fraction")
            h0frach2.setProp("n", "0.000115")
            h0frach2.setProp("ref", "H20")
            H0.addChild(h0frach2)
            matNode.addChild(H0)
            #Carbon
            C12 = libxml2.newNode("isotope")
            C12.setProp("name", 'C12')
            C12.setProp('N', '12')
            C12.setProp('Z', '6')
            atomC12 = libxml2.newNode("atom")
            atomC12.setProp("unit", "g/mole")
            atomC12.setProp("value", "12")
            C12.addChild(atomC12)
            matNode.addChild(C12)
            C13 = libxml2.newNode("isotope")
            C13.setProp("name", 'C13')
            C13.setProp('N', '13')
            C13.setProp('Z', '6')
            atomC13 = libxml2.newNode("atom")
            atomC13.setProp("unit", "g/mole")
            atomC13.setProp("value", "13.0034")
            C13.addChild(atomC13)
            matNode.addChild(C13)
            C0 = libxml2.newNode("element")
            C0.setProp("name", "C0")
            c0frach12 = libxml2.newNode("fraction")
            c0frach12.setProp("n", "0.9893")
            c0frach12.setProp("ref", "C12")
            C0.addChild(c0frach12)
            c0frach13 = libxml2.newNode("fraction")
            c0frach13.setProp("n", "0.0107")
            c0frach13.setProp("ref", "C13")
            C0.addChild(c0frach13)
            matNode.addChild(C0)
            # now define polystyrene as a material
            matpoly = libxml2.newNode("material")
            matpoly.setProp("name", "POLYSTYRENE")
            matpoly.setProp("state", "solid")
            MEE = libxml2.newNode("MEE")
            MEE.setProp("unit", "eV")
            MEE.setProp('value', '68.7')
            matpoly.addChild(MEE)
            dens = libxml2.newNode('D')
            dens.setProp('unit', 'g/cm3')
            dens.setProp('value', '1.06')
            matpoly.addChild(dens)
            fracH0 = libxml2.newNode('fraction')
            fracH0.setProp('n', '0.077418')
            fracH0.setProp('ref', 'H0')
            matpoly.addChild(fracH0)
            fracC0 = libxml2.newNode('fraction')
            fracC0.setProp('n', '0.922582')
            fracC0.setProp('ref', 'C0')
            matpoly.addChild(fracC0)
            matNode.addChild(matpoly)
            # copy all material nodes to the new document
            # for oldnode in datafile.xpathEval('gdml/materials'):
            #      matNode.addChild(oldnode)
            root.addChild(matNode)
            # datafile.freeDoc()
            # Now create a solid node based on the above data
            solidNode = libxml2.newNode('solids')
            # A node to define the solid for the scifi plane
            planeNode = libxml2.newNode("tube")
            planeNode.setProp("name", viewName + "Doublet_simple")
            planeNode.setProp("rmax", str(trackerRadius))
            planeNode.setProp("z", str(doubletThickness))
            planeNode.setProp("lunit", "cm")
            planeNode.setProp("aunit", "degree")
            planeNode.setProp("deltaphi", "360")
            # add the plane to the set of solid objects
            solidNode.addChild(planeNode)
            # create a dummy volume to facilitate the creation of a rotated solid
            sphereNode = libxml2.newNode("sphere")
            sphereNode.setProp("name", "dummy_solid")
            sphereNode.setProp("rmax", str(coreDiameter/2.))
            sphereNode.setProp("lunit", "mm")
            sphereNode.setProp("deltaphi", "10.0")
            sphereNode.setProp("deltatheta", "10.0")
            sphereNode.setProp("aunit", "degree")
            solidNode.addChild(sphereNode)
            # Combine and rotate the tracker plane
            subnode = libxml2.newNode("subtraction")
            subnode.setProp("name", viewName + "Doublet_solid")
            firstnode = libxml2.newNode("first")
            firstnode.setProp("ref", viewName + "Doublet_simple")
            subnode.addChild(firstnode)
            secondnode = libxml2.newNode("second")
            secondnode.setProp("ref", "dummy_solid")
            subnode.addChild(secondnode)
            firstpos = libxml2.newNode("firstposition")
            firstpos.setProp("name", "firstpos")
            firstpos.setProp("x", "0.0")
            firstpos.setProp("y", "0.0")
            firstpos.setProp("z", "0.0")
            firstpos.setProp("unit", "mm")
            firstrot = libxml2.newNode("firstrotation")
            firstrot.setProp("name", "firstpos")
            firstrot.setProp("x", "-90.0")
            firstrot.setProp("y", "0.0")
            firstrot.setProp("z", "0.0")
            subnode.addChild(firstrot)
            solidNode.addChild(subnode)
            # Now create a temporary scifi object
            sciFibreNode = libxml2.newNode("tube")
            sciFibreNode.setProp("name", viewName + "DoubletCores_solid")
            sciFibreNode.setProp("rmax", str(coreDiameter/2.))
            sciFibreNode.setProp("z", "1.0")
            sciFibreNode.setProp("lunit", "mm")
            sciFibreNode.setProp("aunit", "degree")
            sciFibreNode.setProp("deltaphi", "360")
            # add the fibre to the set of solid objects
            solidNode.addChild(sciFibreNode)
            root.addChild(solidNode)
            # now to define the detector structure
            structNode = libxml2.newNode("structure")
            # define the fibre volume
            fibreVol = libxml2.newNode("volume")
            fibreVol.setProp("name", viewName + "DoubletCores")
            matreffV = libxml2.newNode("materialref")
            matreffV.setProp("ref", material)
            fibreVol.addChild(matreffV)
            solreffV = libxml2.newNode("solidref")
            solreffV.setProp("ref", viewName + "DoubletCores_solid")
            fibreVol.addChild(solreffV)
            # create an auxiliary object to contain the visualization and
            # sensitive detector information
            # auxdet1 = libxml2.newNode("auxiliary")
            # auxdet1.setProp("auxtype","SensitiveDetector")
            # auxdet1.setProp("auxvalue","SciFi")
            # fibreVol.addChild(auxdet1)
            auxvis1 = libxml2.newNode("auxiliary")
            auxvis1.setProp("auxtype", "Invisible")
            auxvis1.setProp("auxvalue", "1")
            fibreVol.addChild(auxvis1)
            auxstep1 = libxml2.newNode("auxiliary")
            auxstep1.setProp("auxtype", "G4StepMax")
            auxstep1.setProp("auxvalue", "0.1")
            fibreVol.addChild(auxstep1)
            structNode.addChild(fibreVol)
            # define the plane volume
            planeVol = libxml2.newNode("volume")
            planeVol.setProp("name", viewName + "Doublet")
            matrefpV = libxml2.newNode("materialref")
            matrefpV.setProp("ref", material)
            planeVol.addChild(matrefpV)
            solrefpV = libxml2.newNode("solidref")
            solrefpV.setProp("ref", viewName + "Doublet_solid")
            planeVol.addChild(solrefpV)
            paramVol = libxml2.newNode("paramvol")
            numFibres = math.floor(2.0 * activeRadius / (0.5 * fibrePitch))
            paramVol.setProp("ncopies", str(int(numFibres)))
            volrefpV = libxml2.newNode("volumeref")
            volrefpV.setProp("ref", viewName + "DoubletCores")
            paramVol.addChild(volrefpV)
            ppspV = libxml2.newNode("parameterised_position_size")
            paramNode = []
            posNode   = []
            # rotNode   = []
            tubedim   = []
            for i in range(1, int(numFibres)+1):
                paramNode.append(libxml2.newNode("parameters"))
                paramNode[-1].setProp("number", str(i))
                posNode.append(libxml2.newNode("position"))
                posNode[-1].setProp("name", "fibrePos" + str(i))
                fp = fibrePitch/fibreDiameter
                xpos = i * (fibreDiameter*fp/2.) \
                       - (activeRadius-fibreDiameter/2.)
                posNode[-1].setProp("x", str(xpos))
                posNode[-1].setProp("z", "0.0")
                spacing = math.sqrt(fibreDiameter*fibreDiameter*(1-fp*fp/4.))
                zpos = 0.5*spacing
                if i % 2:
                    zpos = -0.5*spacing
                posNode[-1].setProp("y", str(zpos))
                #rotNode.append(libxml2.newNode("rotation"))
                #rotNode[-1].setProp("x", str(90.0))
                #rotNode[-1].setProp("y", str(0))
                #rotNode[-1].setProp("z", str(0))
                #rotNode[-1].setProp("unit","degree")
                paramNode[-1].addChild(posNode[-1])
                # paramNode[-1].addChild(rotNode[-1])
                tubedim.append(libxml2.newNode("tube_dimensions"))
                tubedim[-1].setProp("InR", "0.0")
                tubedim[-1].setProp("OutR", str(coreDiameter/2.))
                if activeRadius > math.fabs(xpos):
                    hz = math.sqrt(activeRadius*activeRadius - xpos*xpos)
                tubedim[-1].setProp("hz", str(2.0*hz))
                tubedim[-1].setProp("lunit", "mm")
                tubedim[-1].setProp("StartPhi", "0.0")
                tubedim[-1].setProp("DeltaPhi", "360.0")
                tubedim[-1].setProp("aunit", "degree")
                paramNode[-1].addChild(tubedim[-1])
                ppspV.addChild(paramNode[-1])
            paramVol.addChild(ppspV)
            planeVol.addChild(paramVol)
            # create an auxiliary object to contain the visualization and
            # sensitive detector information
            auxvis2 = libxml2.newNode("auxiliary")
            auxvis2.setProp("auxtype", "Invisible")
            auxvis2.setProp("auxvalue", "1")
            planeVol.addChild(auxvis2)
            auxstep2 = libxml2.newNode("auxiliary")
            auxstep2.setProp("auxtype", "G4StepMax")
            auxstep2.setProp("auxvalue", "0.1")
            planeVol.addChild(auxstep2)
            structNode.addChild(planeVol)
            root.addChild(structNode)
            # Now the setup node needs to be added
            setup = libxml2.newNode("setup")
            setup.setProp("name", "Default")
            setup.setProp("version", "1.0")
            world = libxml2.newNode("world")
            world.setProp("ref", viewName + "Doublet")
            setup.addChild(world)
            root.addChild(setup)
            
            ofile = open(viewName + "_Doublet.gdml","w")
            doc.saveTo(ofile)
            ofile.close()
            doc.freeDoc()
        
