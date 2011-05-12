import os.path
import libxml2
import libxslt

class Config:
    #Class initialiser
    def __init__(self, xmlin1, xsl , xmlin2=None):
        self.XMLIn1 = xmlin1
        self.XSL = xsl
        self.XMLIn2 = xmlin2
        
    # Method which parses an xml/gmdl Arguement=output file name(string)
    def XSLTParse(self, output):
        #outputExists = False
        styledoc = libxml2.parseFile(self.XSL)
        style = libxslt.parseStylesheetDoc(styledoc)
        doc = libxml2.parseFile(self.XMLIn1)
        result = style.applyStylesheet(doc, None)
        style.saveResultToFilename(output, result, 0)
        #if outputExists == False:
        #    print "Throw error output file not created!"
        style.freeStylesheet()
        doc.freeDoc()
        result.freeDoc()
        outputExists = os.path.exists(output)
        print "Merge Complete!"

    #Method which write a GDML into a MICE Module format
    def MergeToText(self, output):
        self.XSLTParse(output)

    def AppendMerge(self):
        """
        @Method which appends the MICE info to GDML

        This method reads in Merge.xsl.in, used to append the MICE info onto the end of
        a config file, and replaces the text EDIT with the name of the file from which the MICE
        info comes from. Outputs a new file, Merge.xsl, ready to be parse.

        @param Parameter 1 intialised in class intialiser (2nd xml input)
        """
        if self.XMLIn2 == None: raise TypeError("Merge input 2 not defined")
        found = False
        fin = open('Merge.xsl.in', 'r')
        fout = open('Merge.xsl', 'w')
        #check Merge.xsl.in for the word EDIT
        for line in fin.readlines():
                if line.find('EDIT')>=0:
                    found = True
                    new_line = line.replace('EDIT', self.XMLIn2)
                    print line
                    print new_line
                    print >>fout,new_line
        #throw error if Merge.xsl.in has been altered
        if found == False:
            print "Throw error: EDIT not found Merge.xsl.in has been altered"
        if self.XMLIn2==None:
            print "Throw error: No file name/path to append was given"



geometry1 = Config("fastradModel.xml", "GDML2G4MICE.xsl")
geometry2 = Config("fastradModel.xml", "Merge.xsl", "FieldInfoTest.xml")

print geometry2.XMLIn1
print geometry2.XMLIn2
print geometry2.XSL
    
geometry1.MergeToText("foobar.txt")
geometry2.AppendMerge()