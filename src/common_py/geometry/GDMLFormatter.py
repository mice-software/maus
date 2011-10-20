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

import os.path
from xml.dom import minidom

#pylint: disable = C0301, R0902, R0201 

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
        self.beamline_file = None
        self.field_file = None
        self.formatted = False
        self.txtfile = ""
        self.configurationfile = None
        self.materialfile = None
        self.materialfilepath = None
        schema = '/src/common_py/geometry/GDML_3_0_0/schema/gdml.xsd'
        self.schema = os.environ['MAUS_ROOT_DIR'] + schema
        filelist = []
        self.stepfiles = filelist
        if type(path) != str:
            raise IOError('path must be a string', 'GDMLFormatter::__init__')
        else:
            self.path = path
        gdmls = os.listdir(self.path)
        for fname in gdmls:
            if fname[-5:] != '.gdml' and fname[-4:] != '.xml':
                raise IOError('Files are not GDMLs or XMLs', 'GDMLFormatter::__init__')
            if fname.find('materials') >= 0:
                materialfile = self.path + '/' + fname
                self.materialfile = materialfile
                self.materialfilepath = os.path.abspath(self.materialfile)
            if fname.find('fastrad') >= 0:
                configfile = self.path + '/' + fname
                if file != self.materialfile:
                    self.configurationfile = configfile
            if fname.find('Field') >= 0 or fname.find('field') >= 0:
                found_file = self.path + '/' + fname
                self.field_file = found_file
            if fname.find('Beamline') >= 0:
                found_file = self.path + '/' + fname
                self.beamline_file = found_file
            if fname.find('materials') < 0 and fname.find('fastrad') < 0 and fname.find('Field') < 0 and fname.find('Beamline') < 0 :
                stepfile = self.path + '/' + fname
                self.stepfiles.append(stepfile)
            if self.field_file == None:
                raise IOError('Please write a Field.gdml file which contains MAUS_info see README for details')
            
    def format_schema_location(self, gdmlfile):
        """
        @method format_schema_location
        
        This method parses the GDML file into memory and alters the location of 
        the schema location. It then rewrites the GDML with the valid schema.
        
        @param GDMLFile The name of the file which will have its schema location altered.
        """
        if gdmlfile[-5:] != '.gdml' and gdmlfile[-4:] != '.xml':
            raise IOError(gdmlfile + ' is not a gdml or xml', 'Formatter::format_schema_location')
        else:
            xmldoc = minidom.parse(gdmlfile)
            for node in xmldoc.getElementsByTagName("gdml"):
                if node.hasAttribute("xsi:noNamespaceSchemaLocation"):
                    node.attributes['xsi:noNamespaceSchemaLocation'] = self.schema
            os.remove(gdmlfile)
            fout = open(gdmlfile, 'w')
            xmldoc.writexml(fout)
       
    def merge_maus_info(self, gdmlfile):
        """
        @method merge_maus_info
        
        This method adds the field information
        to the configuration GDML.
        """
        if gdmlfile[-5:] != '.gdml' and gdmlfile[-4:] != '.xml':
            raise IOError(gdmlfile + ' is not a gdml or xml', 'Formatter::format_schema_location')
        else:
            config = minidom.parse(gdmlfile)
            field = minidom.parse(self.field_file)
            for node in field.getElementsByTagName("MICE_Information"):
                maus_info = node
            root_node = config.childNodes[2]
            root_node.insertBefore(maus_info, root_node.childNodes[9])
            fout = open(gdmlfile, 'w')
            root_node.writexml(fout)
            fout.close()
        
    def merge_run_info(self, gdmlfile):
        """
        @method merge_maus_info
        
        This method adds the field information
        to the configuration GDML.
        """
        if gdmlfile[-5:] != '.gdml' and gdmlfile[-4:] != '.xml':
            raise IOError(gdmlfile + ' is not a gdml or xml', 'Formatter::format_schema_location')
        else:    
            field = minidom.parse(gdmlfile)
            beamline = minidom.parse(self.beamline_file)
            for node in beamline.getElementsByTagName("run"):
                maus_info = node
            root_node = field.childNodes[0].childNodes[1].childNodes[1]
            root_node.insertBefore(maus_info, root_node.childNodes[0])
            fout = open(gdmlfile, 'w')
            field.writexml(fout)
            fout.close()
            os.remove(self.beamline_file)
     
    def format_materials(self, gdmlfile):
        """
        @method Format Materials
        
        This method opens a new GDML file in memory and creates a new Document
        Type Definition (DTD) which contains the correct location of the materials
        reference file produced by fastRad.
       
        @param GDMLFile The name of the file which will have its materials reference file location altered.
        """
        if gdmlfile[-5:] != '.gdml' and gdmlfile[-4:] != '.xml':
            raise IOError(gdmlfile + ' is not a gdml or xml', 'Formatter::format_materials')
        else: 
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
        if inputfile[-4:] != '.txt':
            raise IOError(inputfile + ' is not a txt file', 'Formatter::insert_materials_ref')
        else:
            fin = open(inputfile, 'r')
            gdmlfile = inputfile[:-4] + '.gdml'
            fout = open(gdmlfile, 'w')
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
        if gdmlfile[-5:] != '.gdml' and gdmlfile[-4:] != '.xml':
            raise IOError(gdmlfile + ' is not a gdml or xml', 'Formatter::format_check')
        else:
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
            if self.beamline_file != None:
                self.merge_run_info(self.field_file)
            self.merge_maus_info(self.configurationfile)
            self.format_materials(self.configurationfile)
            self.insert_materials_ref(self.txtfile)
        print "Formatted Configuration File"
        
        noofstepfiles = len(self.stepfiles)
        for num in range(0, noofstepfiles):
            self.format_check(self.stepfiles[num])
            if self.formatted == False:
                self.format_schema_location(self.stepfiles[num])
                self.format_materials(self.stepfiles[num])
                self.insert_materials_ref(self.txtfile)
            print "Formatted " + str(num+1) + " of " + str(noofstepfiles) + " Geometry Files"
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
