import os.path
import libxml2
import libxslt

def main():
    from CADImport import CADImport
    geometry1 = CADImport("fastradModel.xml", "GDML2G4MICE.xsl", "", "OUTPUTFILE.txt")
    geometry2 = CADImport("fastradModel.xml", "GDML2G4MICE.xsl")
    geometry3 = CADImport("fastradModel.xml", "Merge.xsl", "FieldInfoTest.xml")
    geometry1.XSLTParse()
    geometry3.AppendMerge()
    

if __name__ == '__main__':
    main()


class CADImport:
    """
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
        @Method Class initialiser

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
        #if xmlin1 == None: raise StandardError("xmlin1 must be defined", "CADImport::__init__")
        #else:
        self.XMLIn1 = xmlin1
        if xsl == None: raise StandardError("xsl must be defined", "CADImport::__init__")
        else: self.XSL = xsl
        if xmlin2 == "": self.XMLIn2 = None
        else: self.XMLIn2 = xmlin2
        if output == "": self.Output = None
        else: self.Output = output
        if mergein == None: self.MergeIn = 'Merge.xsl.in'
        else: self.MergeIn = mergein
        if mergeout == None: self.MergeOut = 'Merge.xsl'
        else: self.MergeOut = mergeout
        
    def XSLTParse(self):
        """
        @Method XSLTParse, to parse an XML(GDML) to text or another XML(GDML)

        This method will execute an XSLT stylesheet and produce either a text
        or another XML(GDML) according to the request.

        """
        if os.path.exists(self.Output) == None: raise StandardError("Output file name not defined", "CADImport::XSLTParse")
        #outputExists = False
        styledoc = libxml2.parseFile(self.XSL)
        style = libxslt.parseStylesheetDoc(styledoc)
        doc = libxml2.parseFile(self.XMLIn1)
        result = style.applyStylesheet(doc, None)
        style.saveResultToFilename(self.Output, result, 0)
        #outputExists = os.path.exists(output)
        #if outputExists == False: raise StandardError("GDML not parsed!", "CADImport::XSLTParse")
        style.freeStylesheet()
        doc.freeDoc()
        result.freeDoc()

    def AppendMerge(self):
        """
        @Method which appends the MICE info to GDML

        This method reads in Merge.xsl.in, used to append the MICE info onto the end of
        a config file, and replaces the text EDIT with the name of the file from which the MICE
        info comes from. Outputs a new file, Merge.xsl, ready to be parse.

        @param Parameter 1 intialised in class intialiser (2nd xml input)
        """
        if self.XMLIn2 == None: raise TypeError("Merge input 2 not defined", "CADImport::AppendMerge")
        found = False
        fin = open(self.MergeIn, 'r')
        fout = open(self.MergeOut, 'w')
        #check Merge.xsl.in for the word EDIT
        for line in fin.readlines():
                if line.find('EDIT')>=0:
                    found = True
                    new_line = line.replace('EDIT', self.XMLIn2)
                    print >>fout,new_line
                else:
                    print >>fout,line
        #throw error if Merge.xsl.in has been altered
        if found == False: raise StandardError("EDIT not found in Merge.xsl.in, this file has been altered", "CADImport::AppendMerge")