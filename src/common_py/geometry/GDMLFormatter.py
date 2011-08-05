"""
GDML Formatting Class
M. Littlefield 02/08/11 
"""
import os.path
from xml.dom import minidom

class Formatter:
    """
    @Class formatter this class formats the raw outputted fastrad files so there schema material file paths are correct

    This class opens the files which need the correct paths added and edits the file in situ.
    """
    def __init__(self, path):
        """
        @Method Class Constructor

        This is the class constructor which sets up the variables so they point at the correct files ready to edit.

        @Param Path The path to the directory which contains the fastrad outputted GDML files
        """
        self.formatted = False
        self.txtfile = ""
        self.configurationfile = None
        self.materialfile = None
        self.materialfilepath = None
        schema = '/src/common_py/geometry/gdmlSchema/GDML_3_0_0/schema/gdml.xsd'
        self.schema = os.environ['MAUS_ROOT_DIR'] + schema
        filelist = []
        self.stepfiles = filelist
        if type(path) != str:
            raise IOError('path must be a string', 'GDMLFormatter::__init__')
        else:
            self.path = path
        gdmls = os.listdir(self.path)
        for fname in gdmls:
            if fname.find('materials') >= 0:
                materialfile = self.path + '/' + fname
                self.materialfile = materialfile
                self.materialfilepath = os.path.abspath(self.materialfile)
            if fname.find('fastrad') >= 0:
                configfile = self.path + '/' + fname
                if file != self.materialfile:
                    self.configurationfile = configfile
            if fname != self.configurationfile  and fname != self.materialfile:
                stepfile = self.path + '/' + fname
                self.stepfiles.append(stepfile)
        length = len(self.stepfiles)
        for num in range(0, length):
            if self.stepfiles[num] == self.configurationfile:
                file1 = self.stepfiles[num]
            if self.stepfiles[num] == self.materialfile:
                file2 = self.stepfiles[num]
        self.stepfiles.remove(file1)
        self.stepfiles.remove(file2)

    def format_schema_location(self, gdmlfile):
        """
        @method format_schema_location
        
        This method parses the GDML file into memory and alters the location of 
        the schema location. It then rewrites the GDML with the valid schema.
        
        @param GDMLFile The name of the file which will have its schema location altered.
        """
        xmldoc = minidom.parse(gdmlfile)
        for node in xmldoc.getElementsByTagName("gdml"):
            if node.hasAttribute("xsi:noNamespaceSchemaLocation"):
                node.attributes['xsi:noNamespaceSchemaLocation'] = self.schema
        os.remove(gdmlfile)
        fout = open(gdmlfile, 'w')
        xmldoc.writexml(fout)

    def format_materials(self, gdmlfile):
        """
        @method Format Materials
        
        This method opens a new GDML file in memory and creates a new Document
        Type Definition (DTD) which contains the correct location of the materials
        reference file produced by fastRad.
       
        @param GDMLFile The name of the file which will have its materials reference file location altered.
        """
        self.txtfile = "" 
        impl = minidom.getDOMImplementation()
        docstr = 'gdml [<!ENTITY materials SYSTEM "' + self.materialfile + '">]'
        doctype = impl.createDocumentType(docstr, None, None)
        newdoc = impl.createDocument(None, "MAUS", doctype)
        config = minidom.parse(gdmlfile)
        for node in config.getElementsByTagName("gdml"):
            rootelement = node
        for node2 in newdoc.getElementsByTagName("MAUS"):
            oldelement = node2
        newdoc.replaceChild(rootelement, oldelement)
        self.txtfile = gdmlfile[:-5] + '.txt'
        fout = open(self.txtfile, 'w')
        newdoc.writexml(fout)
        fout.close()
        os.remove(gdmlfile)
        
    def insert_materials_ref(self, inputfile):
        """
        @method Insert Materials Reference
        
        This method opens the GDML file and replaces the materials file
        reference call which is lost during the parsing of the GDML's
        in other methods.
        
        @param GDMLFile The name of the file which will have its materials reference replaced.
        """
        fin = open(inputfile, 'r')
        gdmlfile = inputfile[:-4] + '.gdml'
        fout = open(gdmlfile, 'w')
        contents = []
        for line in fin.readlines():
            if line.find('<!-- Materials definition CallBack -->')>=0:
                matdef = '<!-- Materials definition CallBack -->'
                new_line = line.replace(matdef, matdef+'&materials;')
                print >> fout, new_line
            else:
                print >> fout, line
        print >> fout, '<!-- Formatted for MAUS -->'
        fin.close()
        fout.close()
        os.remove(inputfile)
        
    def format_check(self, gdmlfile):
        """
        @method Format Check
        
        This method checks to see whether the file passed to it
        has already been formatted but looking for the string
        <!-- Formatted for MAUS --> which will have been put in the file
        if it had already been formatted. This is to stop multiple 
        materials reference calls being added as it cannot be added by parsing
        the GDML.
        
        @param GDML File The file to be checked.
        """
        self.formatted = False
        fin = open(gdmlfile, 'r')
        for lines in fin.readlines():
            if lines.find('<!-- Formatted for MAUS -->') >= 0:
                print gdmlfile + ' already formatted!'
                self.formatted = True
        fin.close()
    
        
    def format(self):
        """
        @method Format
        
        This method calls all the other methods to format the 
        necessary parts of the file. It will run through all of the
        fastRad outputted files within the folder location given to
        the class constructor.
        """
        self.format_check(self.configurationfile)
        if self.formatted == False:
            self.format_schema_location(self.configurationfile)
            self.format_materials(self.configurationfile)
            self.insert_materials_ref(self.txtfile)
        
        noofstepfiles = len(self.stepfiles)
        for num in range(0, noofstepfiles):
            self.format_check(self.stepfiles[num])
            if self.formatted == False:
                self.format_schema_location(self.stepfiles[num])
                self.format_materials(self.stepfiles[num])
                self.insert_materials_ref(self.txtfile)
        print "Format Complete!"
            
                    
def main():
    """
    Main Function
    """
    location = '/src/common_py/geometry/Download'
    gdmls = Formatter(os.environ['MAUS_ROOT_DIR']+location)
    gdmls.format()

if __name__ == '__main__':
    main()