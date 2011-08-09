"""
CAD Import Class
M Littlefield
"""
import os
import libxml2
import libxslt

#pylint: disable = C0301, R0913, R0912

class CADImport:
    """
    @Class CADImport, GDML parser class.
    
    This class has been designed to parse XML(GDML) files in a number of ways.
    The first way applies an XSLT stylesheet to a GDML file and re writes the
    file into MICE module format. The output can be a text file. The second parsing
    method is where the XSLT stylesheet is applied to two GDMLs and appends the MICE
    Information of one GDML to the end of the geometry GMDL file, taken from fastRad
    in order to merge geometrical information and field information needed for G4. This outputs the
    a GDML file.
    """

    def __init__(self, xmlin1, xsl , xmlin2=None, output=None, mergein=None, mergeout=None):
        """
        @Method Class constructor

        This method initialises the class and takes 7 parameters. The later 4 parameters are optional
        but if one of the latter needs to be initialised a blank "" needs to be placed to set preceeding 
        params to None. All arguments should be file names or paths.

        @param xmlin1,   first xml file name/path, used to apply an XSLT to.
        @param xsl,      xslt file name/path, xslt stylesheet used to re write xml(GDML)
        @param xmlin2,   second xml file name/path, used to hold the MICE info to append to geometry
        @param output,   output file name/path
        @param mergein,  template XSLT for append, this is the file name/path to Merge.xsl.in which is the stylesheet
                         used to append the MICE info to geometry info. This file must be altered to set the putout file name/path.
        @param mergeout, file name/path to be inserted into merge.xsl.in.
        """
        if type(xmlin1) != str:
            raise TypeError(xmlin1 + " must be a string(file name/path)")
        if xmlin1[-4:] != '.xml' and xmlin1[-5:] != '.gdml':
            raise IOError(xmlin1 + " must be an xml or gdml file")
        else: self.xml_in_1 = xmlin1
        
        if type(xsl) != str:
            raise TypeError(xsl + " must be a string(file name/path")
        if xsl[-4:] != '.xsl':
            raise IOError(xsl + " must be an xml or gdml file")
        else: self.xsl = xsl
        
        if xmlin2 == None: 
            self.xml_in_2 = None
        elif type(xmlin2) != str:
            raise TypeError(xmlin2 + " must be a string(file name/path)")
        elif xmlin2[-4:] != '.xml' and self.xml_in_2[-5:] != '.gdml':
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
            self.merge_in = maus_root + '/src/common_py/geometry/xsltScripts/Merge.xsl.in'
        elif type(mergein) != str:
            raise TypeError(mergein + " input must be a string(file name/path)")
        else: 
            self.merge_in = mergein

        if mergeout == None: 
            self.merge_out = 'Merge.xsl'
        elif type(mergeout) != str:
            raise TypeError(mergeout + "output must be a string filename/path)")
        elif mergeout[-4:] != '.xsl':
            raise IOError(mergeout + " must be an xsl file")
        else: 
            self.merge_out = mergeout

    def parse_xslt(self):
        """
        @Method parse_xslt, to parse an XML(GDML) to text or another XML(GDML)

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

    def append_merge(self):
        """
        @Method which appends the MICE info to GDML

        This method reads in Merge.xsl.in, used to append the MICE info onto the end of
        a config file, and replaces the text EDIT with the name of the file from which the MICE
        info comes from. Outputs a new file, Merge.xsl, ready to be parse.

        @param Parameter 1 intialised in class intialiser (2nd xml input)
        """
        fin = open(self.merge_in, 'r')
        fout = open(self.merge_out, 'w')
        for line in fin.readlines():
            if line.find('EDIT')>=0:
                new_line = line.replace('EDIT', self.xml_in_2)
                print >> fout, new_line
            else:
                print >> fout, line


def main():
    """
    Main Function
    """
    print 'Main Function to be written'

if __name__ == '__main__':
    main()
