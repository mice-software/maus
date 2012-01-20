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

import shutil
import os
from xml.dom import minidom
from geometry.ConfigReader import Configreader

SCHEMA_PATH = os.environ["MAUS_ROOT_DIR"]+\
              "/src/common_py/geometry/GDML_3_0_0/schema/gdml.xsd"

class Formatter: #pylint: disable = R0902
    """
    @Class formatter this class formats the raw outputted fastrad files so their
           schema material file paths are correct

    This class opens the files which need the correct paths added and edits the
    file in situ.
    """
    def __init__(self, path_in, path_out):
        """
        @Method Class Constructor

        This is the class constructor which sets up the variables so they point
        at the correct files ready to edit.
        @Param Path The path to the directory which contains the fastrad
                    outputted GDML files
        """
        self.g4_step_max = Configreader().g4_step_max
        self.path_in = path_in
        self.path_out = path_out
        self.beamline_file = None
        self.field_file = None
        self.configuration_file = None
        self.material_file = None
        self.stepfiles = []
        self.formatted = False
        self.txt_file = ""
        self.schema = SCHEMA_PATH
        gdmls = os.listdir(self.path_in)
        for fname in gdmls:
            if fname[-5:] != '.gdml' and fname[-4:] != '.xml':
                print fname+' not a gdml or xml file - ignored'
            elif fname.find('materials') >= 0:
                self.material_file = fname
                shutil.copy(os.path.join(self.path_in, fname), 
                        os.path.join(self.path_out, fname)   )
            elif fname.find('fastrad') >= 0:
                self.configuration_file = fname
            elif fname.find('Field') >= 0 or fname.find('field') >= 0:
                self.field_file = fname
            elif fname.find('Beamline') >= 0:
                self.beamline_file = fname
            else:
                self.stepfiles.append(fname)
        if self.field_file == None:
            raise IOError('Please write a Field.gdml file which ' + \
                        'contains MAUS_info. See README for details')
        
    def format_schema_location(self, gdmlfile):
        """
        @method format_schema_location
        
        This method parses the GDML file into memory and alters the location of 
        the schema location. It then rewrites the GDML with the valid schema.
        
        @param gdmlfile absolute path to the file which will have its schema
                        location altered.
        """
        xmldoc = minidom.parse(os.path.join(self.path_in, gdmlfile))
        for node in xmldoc.getElementsByTagName("gdml"):
            if node.hasAttribute("xsi:noNamespaceSchemaLocation"):
                node.attributes['xsi:noNamespaceSchemaLocation'] = self.schema
        fout = open(os.path.join(self.path_out, gdmlfile), 'w')
        xmldoc.writexml(fout)
        fout.close()
        
    def add_other_info(self):
        """
        @method add_other_information
        
        THis methods adds other information needed by MAUS.
        This is mainly so we can add computer specific environment
        variables into the translated gdml files.
        """
        doc = minidom.Document()
        top_node = doc.createElement("Other_Information")
        doc.appendChild(top_node)
        maus_dir = doc.createElement("GDML_Files")
        maus_dir.setAttribute("location", self.path_out)
        top_node.appendChild(maus_dir)
        g4_step = doc.createElement("G4StepMax")
        g4_step.setAttribute("Value", str(self.g4_step_max))
        top_node.appendChild(g4_step)
        field = minidom.parse(os.path.join(self.path_out, self.field_file))
        old_node = field.childNodes[0].childNodes[1].childNodes[7]
        new_node = doc.childNodes[0]
        base_node = field.childNodes[0].childNodes[1]
        base_node.replaceChild(new_node, old_node) 
        fout = open(os.path.join(self.path_out, self.field_file), 'w')
        field.writexml(fout)
        fout.close()
         
    def merge_maus_info(self, gdmlfile):
        """
        @method merge_maus_info
        
        This method adds the field information to the configuration GDML.
        """
        self.add_other_info()
        config = minidom.parse(os.path.join(self.path_in, gdmlfile)) 
        field = minidom.parse(os.path.join(self.path_out, self.field_file))
        for node in field.getElementsByTagName("MICE_Information"):
            maus_info = node
        root_node = config.childNodes[2]
        root_node.insertBefore(maus_info, root_node.childNodes[9])
        fout = open(os.path.join(self.path_out, gdmlfile), 'w')
        root_node.writexml(fout)
        fout.close()
        
    def merge_run_info(self, gdmlfile):
        """
        @method merge_maus_info
        
        This method adds the run information to the field GDML.
        """
        run_info = False
        fin = open(os.path.join(self.path_in, gdmlfile))
        for lines in fin.readlines():
            if lines.find('run') >= 0:
                run_info = True
        fin.close()
        if run_info == False:
            field = minidom.parse(os.path.join(self.path_in, gdmlfile))
            beamline_path = os.path.join(self.path_in, self.beamline_file)
            beamline = minidom.parse(beamline_path)
            for node in beamline.getElementsByTagName("run"):
                maus_info = node
            root_node = field.childNodes[0].childNodes[1].childNodes[1]
            root_node.insertBefore(maus_info, root_node.childNodes[0])
            fout = open(os.path.join(self.path_out, gdmlfile), 'w')
            field.writexml(fout)
            fout.close()
        print 'Run information merged!'
     
    def format_materials(self, gdmlfile):
        """
        @method Format Materials
        
        This method opens a new GDML file in memory and creates a new Document
        Type Definition (DTD) which contains the correct location of the
        materials reference file produced by fastRad.
       
        @param GDMLFile The name of the file which will have its materials
               reference file location altered.
        """
        impl = minidom.getDOMImplementation()
        docstr = 'gdml [<!ENTITY materials SYSTEM "' \
        + os.path.join(self.path_out, self.material_file) + '">]'
        doctype = impl.createDocumentType(docstr, None, None)
        newdoc = impl.createDocument(None, "MAUS", doctype)
        config = minidom.parse(os.path.join(self.path_out, gdmlfile))
        for node in config.getElementsByTagName("gdml"):
            rootelement = node
        for node2 in newdoc.getElementsByTagName("MAUS"):
            oldelement = node2
        newdoc.replaceChild(rootelement, oldelement)
        self.txt_file = os.path.join(self.path_out, gdmlfile[:-5] + '.txt')
        fout = open(self.txt_file, 'w')
        newdoc.writexml(fout)
        fout.close()
               
    def insert_materials_ref(self, inputfile): #pylint: disable = R0201
        """
        @method Insert Materials Reference
        
        This method opens the GDML file and replaces the materials file
        reference call which is lost during the parsing of the GDML's in other
        methods.
        
        @param GDMLFile The name of the file which will have its materials
                        reference replaced.
        """
        fin = open(inputfile, 'r')
        gdmlfile = inputfile[:-4] + '.gdml'
        fout = open(os.path.join(self.path_out, gdmlfile), 'w')
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
        fin = open(os.path.join(self.path_in, gdmlfile))
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
        self.format_check(self.configuration_file)

        if self.beamline_file != None:
            self.merge_run_info(self.field_file)
        else:
            shutil.copy(os.path.join(self.path_in, self.field_file), 
                       os.path.join(self.path_out, self.field_file))

        if self.formatted == False:
            self.format_schema_location(self.configuration_file)
            self.merge_maus_info(self.configuration_file)
            self.format_materials(self.configuration_file)
            self.insert_materials_ref(self.txt_file)
        print "Formatted Configuration File"
        
        noofstepfiles = len(self.stepfiles)
        for num in range(0, noofstepfiles):
            self.format_check(self.stepfiles[num])
            if self.formatted == False:
                self.format_schema_location(self.stepfiles[num])
                self.format_materials(self.stepfiles[num])
                self.insert_materials_ref(self.txt_file)
            print "Formatted " + str(num+1) + \
            " of " + str(noofstepfiles) + " Geometry Files"
        print "Format Complete"
        

def main():
    """
    Main Function.
    """
if __name__ == "__main__":
    main()