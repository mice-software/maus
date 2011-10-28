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

class CADImport: #pylint: disable = R0903
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
             #pylint: disable = R0912, R0913
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
        @Method parse_xslt to parse an XML(GDML) to text or another XML(GDML)

        This method will execute an XSLT stylesheet and produce either a text
        or another XML(GDML) according to the request.
        """
        styledoc = libxml2.parseFile(self.xsl)
        style = libxslt.parseStylesheetDoc(styledoc)
        doc = libxml2.parseFile(self.xml_in_1)
        result = style.applyStylesheet(doc, None)
        style.saveResultToFilename(self.output, result, 0)
        style.freeStylesheet()
        doc.freeDoc()
        result.freeDoc()

def main():
    """
    Main Function
    """
if __name__ == '__main__':
    main()
