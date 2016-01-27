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
import libxml2
from xml.dom import minidom
from geometry.ConfigReader import Configreader

SCHEMA_PATH = os.environ["MAUS_ROOT_DIR"]+\
              "/src/common_py/geometry/GDML_3_0_0/schema/gdml.xsd"

class Formatter: #pylint: disable = R0902, R0912, R0914, R0915, C0103
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
        self.tof_0_file_number     = Configreader().tof_0_file_number
        self.tof_1_file_number     = Configreader().tof_1_file_number
        self.tof_2_file_number     = Configreader().tof_2_file_number
        self.ckov1_file_number      = Configreader().ckov1_file_number
        self.ckov2_file_number      = Configreader().ckov2_file_number
        self.kl_file_number        = Configreader().kl_file_number
        self.emr_file_number       = Configreader().emr_file_number
        self.tracker0_file_number  = Configreader().tracker0_file_number
        self.tracker1_file_number  = Configreader().tracker1_file_number
        self.absorber0_file_number = Configreader().absorber0_file_number
        self.absorber1_file_number = Configreader().absorber1_file_number
        self.absorber2_file_number = Configreader().absorber2_file_number
        self.using_field_map = Configreader().using_field_map
        self.path_in = path_in
        self.path_out = path_out.rstrip('/') + '/'
        self.beamline_file = None
        self.coolingChannel_file = None
        self.maus_information_file = None
        self.configuration_file = None
        self.material_file = None
        self.tracker_file = None
        self.detmodulefiles = []
        self.modulefiles = []
        self.stepfiles = []
        self.formatted = False
        self.txt_file = ""
        self.schema = SCHEMA_PATH
        self.usegdml = False
        gdmls = os.listdir(self.path_in)
        #if self.path_in == self.path_out:
        #    paths_equal = True
        for fname in gdmls:
            print fname
            if fname[-5:] != '.gdml' and fname[-4:] != '.xml' and \
                    fname.find('.table') < 0:
                print fname+' not a gdml or xml file - ignored'
            elif fname.find('materials') >= 0:
                self.material_file = fname
                shutil.copy(os.path.join(self.path_in, fname), 
                        os.path.join(self.path_out, fname)   )
            elif fname.find('fastrad') >= 0 or \
                     fname.find('Fastrad') >= 0 or \
                     fname.find('Step_IV')>=0:
                self.configuration_file = fname
            elif fname.find('Cooling_Channel') >= 0:
                self.tracker_file = fname
            elif fname.find('Maus_Information') >= 0 \
                     or fname.find('maus_information') >= 0 \
                     or fname.find('Field') >= 0 \
                     or fname.find('field') >= 0:
                print 'Found ', fname
                self.maus_information_file = fname
            elif fname.find('Beamline') >= 0:
                self.beamline_file = fname
            elif fname.find('CoolingChannelInfo') >= 0:
                self.coolingChannel_file = fname
                print 'Found ', fname
            elif fname.find('Quad') >= 0 \
                     or fname.find('Dipole') >= 0 \
                     or fname.find('Solenoid') >= 0 \
                     or fname.find('AFC') >= 0:
                self.modulefiles.append(fname)
            elif fname.find('KL') >= 0 \
                     or fname.find('Tracker') >= 0:
                self.detmodulefiles.append(fname)
            else:
                self.stepfiles.append(fname)
        if self.maus_information_file == None:
            self.maus_information_file = 'Maus_Information.gdml'
            maus_information_location = os.environ['MAUS_ROOT_DIR'] + \
            '/tests/py_unit/test_geometry/testCases/mausModuleSource'
            shutil.copy(os.path.join(maus_information_location, \
                                               self.maus_information_file), 
                        os.path.join(self.path_in, self.maus_information_file) )
            print 'Maus_Information file not found' + \
                  'Maus_Information.gdml has been copied from ' + \
                  maus_information_location
        else:
            info = libxml2.parseFile(os.path.join(self.path_in,
                                                  self.maus_information_file))
            if len(info.xpathEval(\
                "MICE_Information/Other_Information/Module")) > 0:
                self.usegdml = True
                    
    def format_schema_location(self, gdmlfile):
        """
        @method format_schema_location
        
        This method parses the GDML file into memory and alters the location of 
        the schema location. It then rewrites the GDML with the valid schema.
        
        @param gdmlfile absolute path to the file which will have its schema
                        location altered.
        """
        # ignore the field maps.
        if gdmlfile.find('.table') < 0:
            xmldoc = minidom.parse(os.path.join(self.path_in, gdmlfile))
            for node in xmldoc.getElementsByTagName("gdml"):
                if node.hasAttribute("xsi:noNamespaceSchemaLocation"):
                    node.attributes['xsi:noNamespaceSchemaLocation'] = \
                        self.schema
                    # print "Schema location corrected for ",gdmlfile
            fout = open(os.path.join(self.path_out, gdmlfile), 'w')
            xmldoc.writexml(fout)
            fout.close()
        else:
            shutil.copy(os.path.join(self.path_in, gdmlfile), \
                        os.path.join(self.path_out, gdmlfile))

    def format_gdml_locations(self, gdmlfile):
        """
        @method format_gdml_locations

        This method parses the GDML file into memoty and alters the location
        of the file names to absolute paths.
        """
        docfile = os.path.join(self.path_out, gdmlfile)
        print "Reviewing content of ", docfile
        xmldoc = libxml2.parseFile(docfile)
        for vol in xmldoc.xpathEval("gdml/structure/volume/physvol"):
            if len(vol.xpathEval("file")) > 0:
                filenode = vol.xpathEval("file")[0]
                filename = filenode.prop("name")
                if filename.find(self.path_out) == -1:
                    # add the path to the name
                    newname = os.path.join(self.path_out, filename)
                    # print "Replacing ", filename, " with ", newname
                    filenode.setProp("name", newname)
        gfile = open(os.path.join(self.path_out, gdmlfile),'w')
        xmldoc.saveTo(gfile)
        gfile.close()
        xmldoc.freeDoc()

    def correct_gdml_locations(self, gdmlfile):
        """
        @method format_gdml_locations

        This method parses the GDML file into memoty and alters the location
        of the file names to absolute paths.
        """
        docfile = os.path.join(self.path_out, gdmlfile)
        # print "Reviewing content of ",docfile
        xmldoc = libxml2.parseFile(docfile)
        for vol in xmldoc.xpathEval("gdml/structure/volume/physvol"):
            if len(vol.xpathEval("file")) > 0:
                filenode = vol.xpathEval("file")[0]
                filename = filenode.prop("name")
                if filename.find(self.path_in) == -1:
                    # add the path to the name
                    newname = os.path.join(self.path_out, filename)
                    # print "Replacing ",filename," with ",newname
                    filenode.setProp("name", newname)
        # print "Saving to",os.path.join(self.path_out, gdmlfile)
        f = open(os.path.join(self.path_out, gdmlfile),'w')
        xmldoc.saveTo(f)
        f.close()
        xmldoc.freeDoc()
                 
    def add_other_info(self): #pylint: disable = R0914, R0915, C0301
        """
        @method add_other_information
        
        THis methods adds other information needed by MAUS.
        This is mainly so we can add computer specific environment
        variables into the translated gdml files.
        """
        maus_information = minidom.parse(os.path.join(self.path_out, \
                                                    self.maus_information_file))
        file_numbers = None
        for node in maus_information.getElementsByTagName("FileNumbers"):
            file_numbers = node
        doc = minidom.Document()
        top_node = doc.createElement("Other_Information")
        doc.appendChild(top_node)
        maus_dir = doc.createElement("GDML_Files")
        path = self.path_out
        maus_dir.setAttribute("location", path)
        maus_dir.setAttribute("basefile", self.configuration_file)
        top_node.appendChild(maus_dir)
        g4_step = doc.createElement("G4StepMax")
        g4_step.setAttribute("Value", str(self.g4_step_max))
        top_node.appendChild(g4_step)
        file_numbers = doc.createElement("FileNumbers")
        top_node.appendChild(file_numbers)
        tof_0_file_number = doc.createElement("Tof0FileNumber")
        tof_0_file_number.setAttribute("number", str(self.tof_0_file_number))
        file_numbers.appendChild(tof_0_file_number)
        tof_1_file_number = doc.createElement("Tof1FileNumber")
        tof_1_file_number.setAttribute("number", str(self.tof_1_file_number))
        file_numbers.appendChild(tof_1_file_number)
        
        tof_2_file_number = doc.createElement("Tof2FileNumber")
        tof_2_file_number.setAttribute("number", str(self.tof_2_file_number))
        file_numbers.appendChild(tof_2_file_number)
        
        ckov1_file_number = doc.createElement("Ckov1FileNumber")
        ckov1_file_number.setAttribute("number", str(self.ckov1_file_number))
        file_numbers.appendChild(ckov1_file_number)
        ckov2_file_number = doc.createElement("Ckov2FileNumber")
        ckov2_file_number.setAttribute("number", str(self.ckov2_file_number))
        file_numbers.appendChild(ckov2_file_number)
        
        kl_file_number = doc.createElement("KLFileNumber")
        kl_file_number.setAttribute("number", str(self.kl_file_number))
        file_numbers.appendChild(kl_file_number)
        
        emr_file_number = doc.createElement("EMRFileNumber")
        emr_file_number.setAttribute("number", str(self.emr_file_number))
        file_numbers.appendChild(emr_file_number)
        
        tracker0_file_number = doc.createElement("Tracker0FileNumber")
        tracker0_file_number.setAttribute("number", 
                                          str(self.tracker0_file_number))
        file_numbers.appendChild(tracker0_file_number)
        tracker1_file_number = doc.createElement("Tracker1FileNumber")
        tracker1_file_number.setAttribute("number", 
                                          str(self.tracker1_file_number))
        file_numbers.appendChild(tracker1_file_number)
        
        absorber0_file_number = doc.createElement("Absorber0FileNumber")
        absorber0_file_number.setAttribute("number", 
                                           str(self.absorber0_file_number))
        file_numbers.appendChild(absorber0_file_number)
        absorber1_file_number = doc.createElement("Absorber1FileNumber")
        absorber1_file_number.setAttribute("number", 
                                           str(self.absorber1_file_number))
        file_numbers.appendChild(absorber1_file_number)
        absorber2_file_number = doc.createElement("Absorber2FileNumber")
        absorber2_file_number.setAttribute("number", 
                                           str(self.absorber2_file_number))
        file_numbers.appendChild(absorber2_file_number)

        if file_numbers != None:
            top_node.appendChild(file_numbers)
        maus_information = minidom.parse(os.path.join(self.path_out, \
                                                   self.maus_information_file))
        old_node = maus_information.childNodes[0].childNodes[7]
        new_node = doc.childNodes[0]
        base_node = maus_information.childNodes[0]
        base_node.replaceChild(new_node, old_node)
        fout = open(os.path.join(self.path_out, self.maus_information_file),'w')
        maus_information.writexml(fout)
        fout.close()
         
    def merge_maus_info(self, gdmlfile):
        """
        @method merge_maus_info
        
        This method adds the maus_information information to the 
        configuration GDML.
        """
        self.add_other_info()
        config = minidom.parse(os.path.join(self.path_out, gdmlfile))
        # print "config file ", os.path.join(self.path_out, gdmlfile)
        # print "information file ", os.path.join(self.path_out, \
        #                                         self.maus_information_file)
        maus_information = minidom.parse(os.path.join(self.path_out, \
                                                    self.maus_information_file))
        for node in maus_information.getElementsByTagName("MICE_Information"):
            maus_info = node
        #     print str(node)
        # print str(config)
        root_node = config.childNodes[2]
        root_node.insertBefore(maus_info, root_node.childNodes[9])
        fout = open(os.path.join(self.path_out, gdmlfile), 'w')
        root_node.writexml(fout)
        fout.close()
        
    def merge_run_info(self, gdmlfile):
        """
        @method merge_run_info
        
        This method adds the run information to the maus_information_file GDML.
        """
        run_info = False
        fin = open(os.path.join(self.path_in, gdmlfile))
        for lines in fin.readlines():
            if lines.find('run') >= 0 or lines.find('runs') >= 0:
                run_info = True
        fin.close()
        if run_info == False:
            maus_information = \
                             minidom.parse(os.path.join(self.path_in, gdmlfile))
            beamline_path = os.path.join(self.path_in, self.beamline_file)
            beamline = minidom.parse(beamline_path)
            for node in beamline.getElementsByTagName("run"):
                run_info = node
            if type(run_info) == bool:
                raise IOError("Run number you have selected is not on the CDB")
            else:
                root_node = maus_information.childNodes[0].childNodes[1]
                root_node.insertBefore(run_info, root_node.childNodes[0])
                fout = open(os.path.join(self.path_out, gdmlfile), 'w')
                maus_information.writexml(fout)
                fout.close()
            
        print 'Run information merged!'
        
    def merge_cooling_channel_info(self, gdmlfile):
        """
        @method merge_maus_info
        
        This method adds the run information to the maus_information_file GDML.
        """
        run_info = False
        fin = open(os.path.join(self.path_out, gdmlfile))
        #for lines in fin.readlines():
        #    if lines.find('run') >= 0 or lines.find('runs') >= 0:
        #        run_info = True
        fin.close()
        if run_info == False:
            maus_information = \
                             minidom.parse(os.path.join(self.path_out, \
                                                            gdmlfile))
            coolingChannel_path = os.path.join(self.path_in, \
                                               self.coolingChannel_file)
            # print coolingChannel_path
            coolingChannel = minidom.parse(coolingChannel_path)
            for node in coolingChannel.getElementsByTagName("coolingchannel"):
                cc_info = node
            if type(cc_info) == bool:
                raise IOError("Run number you have selected is not on the CDB")
            else:
                root_node = maus_information.childNodes[0].childNodes[1]
                root_node.insertBefore(cc_info, root_node.childNodes[0])
                fout = open(os.path.join(self.path_out, gdmlfile), 'w')
                maus_information.writexml(fout)
                fout.close()
            
        print 'Cooling channel information merged!'
     
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
            self.merge_run_info(self.maus_information_file)
        else:
            shutil.copy(os.path.join(self.path_in, self.maus_information_file), 
                       os.path.join(self.path_out, self.maus_information_file))
        
        if self.coolingChannel_file != None:
            self.merge_cooling_channel_info(self.maus_information_file)


        if self.formatted == False:
            print self.configuration_file
            self.format_schema_location(self.configuration_file)
            self.merge_maus_info(self.configuration_file)
            self.format_materials(self.configuration_file)
            self.format_gdml_locations(self.configuration_file)
            self.insert_materials_ref(self.txt_file)
        print "Formatted Configuration File"
        
        if self.tracker_file != None:
            self.format_check(self.tracker_file)
            if self.formatted == False:
                print self.tracker_file
                self.format_schema_location(self.tracker_file)
                self.merge_maus_info(self.tracker_file)
                self.format_materials(self.tracker_file)
                self.format_gdml_locations(self.tracker_file)
                self.insert_materials_ref(self.txt_file)
            print "Formatted cooling channel and trackers"
        for module in self.modulefiles:
            self.format_check(module)
            if self.formatted == False:
                self.format_schema_location(module)
                self.merge_maus_info(module)
                self.format_materials(module)
                self.format_gdml_locations(module)
                self.insert_materials_ref(self.txt_file)
            print "Formatted module files"

        self.stepfiles.extend(self.detmodulefiles)
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

        
    def addDiffuserIrises(self):
        """
        @ Module addDiffuserIrises

        Add the irises to the output gdml file
        """
        
        mausInfo = libxml2.parseFile(os.path.join(self.path_out, \
                                                  self.maus_information_file))
        
        mausInfoIn = libxml2.parseFile(os.path.join(self.path_in, \
                                                  self.maus_information_file))
        runInfo = mausInfo.xpathEval(\
            "MICE_Information/Configuration_Information/run")
        print runInfo
        diffuserSetting = []
        if len(runInfo) > 0: 
            diffuserSetting.append(runInfo[0].prop("diffuserThickness"))
            print "Will use diffuser setting ", diffuserSetting[0]
        else:
            print "Will use the default diffuser."
        # check the possible keys for suspected
        modulekeys = mausInfoIn.xpathEval(\
            "MICE_Information/Other_Information/Module")
        keynode = next(x for x in \
                       modulekeys if x.prop('name').find('SolenoidUS') >= 0 \
                       or x.prop('name').find('Cooling_Channel') >= 0)
        key = keynode.prop('name')
        modrange = [float(keynode.prop('zmin')), float(keynode.prop('zmax'))]
        diffuser = mausInfo.xpathEval(\
            "MICE_Information/Detector_Information/Diffuser")

        targetname = os.path.join(self.path_in, key+".gdml")
        print targetname
        target = libxml2.parseFile(targetname)
        vol = next(x for x in target.xpathEval("gdml/structure/volume") 
                   if x.prop("name").find(key + "_structvol") >= 0)

        for iris in diffuser:
            # extract the iris number, position and rotation
            inum = int(iris.xpathEval("Iris")[0].prop("name"))
            pos = [float(iris.xpathEval("Position")[0].prop("x")), \
                   float(iris.xpathEval("Position")[0].prop("y")), \
                   float(iris.xpathEval("Position")[0].prop("z"))]
            
            print key, pos[2], modrange
            pos[2] = pos[2] - (modrange[1] + modrange[0])/2.
            print key, pos[2], modrange
            posUnit = iris.xpathEval("Position")[0].prop("unit")
            rot = [float(iris.xpathEval("Rotation")[0].prop("x")), \
                   float(iris.xpathEval("Rotation")[0].prop("y")), \
                   float(iris.xpathEval("Rotation")[0].prop("z"))]
            rotUnit = iris.xpathEval("Rotation")[0].prop("unit")
            newNode = libxml2.newNode("physvol")
            fileNode = libxml2.newNode("file")
            if len(diffuserSetting) > 0: # test of whether the statement exists.
                print "Using diffuser setting ", int(diffuserSetting[0])
                if (inum == 1 and int(diffuserSetting[0]) % 2 == 0) or \
                       (inum == 2 and (int(diffuserSetting[0]) % 4 == 0 or \
                                     int(diffuserSetting[0]) % 4 == 1)) or \
                       (inum == 3 and (int(diffuserSetting[0]) < 4 or
                                     (int(diffuserSetting[0]) >= 8 and \
                                      int(diffuserSetting[0]) < 12))) or \
                       (inum == 4 and int(diffuserSetting[0]) < 8):
                    newNode.setProp("name", "iris" + str(inum))
                    fileNode.setProp("name", "iris" + str(inum) + "_open.gdml")
                else:
                    newNode.setProp("name", "iris" + str(inum))
                    fileNode.setProp("name", "iris" + str(inum) + \
                                     "_closed.gdml")
            else:
                if inum == 2 or inum == 3:
                    print "Iris ", inum, " default closed introduced."
                    newNode.setProp("name", "iris" + str(inum))
                    fileNode.setProp("name", "iris" + str(inum) + \
                                     "_closed.gdml")
                else:
                    print "Iris ", inum, " default open introduced."
                    newNode.setProp("name", "iris" + str(inum))
                    fileNode.setProp("name", "iris" + str(inum) + "_open.gdml")
            newNode.addChild(fileNode)
            posNode = libxml2.newNode("position")
            posNode.setProp("name", "posRef_iris_" + str(inum))
            posNode.setProp("x", str(pos[0]))
            posNode.setProp("y", str(pos[1]))
            posNode.setProp("z", str(pos[2]))
            posNode.setProp("unit", posUnit)
            newNode.addChild(posNode)
            rotNode = libxml2.newNode("rotation")
            rotNode.setProp("name", "rotRef_iris_"+str(inum))
            rotNode.setProp("x", str(rot[0]))
            rotNode.setProp("y", str(rot[1]))
            rotNode.setProp("z", str(rot[2]))
            rotNode.setProp("unit", rotUnit)
            newNode.addChild(rotNode)
            vol.addChild(newNode)
            
        tgtfile = open(targetname, 'w')
        target.saveTo(tgtfile)
        tgtfile.close()
        target.freeDoc()
        mausInfo.freeDoc()

    def maus_info_to_gdml(self):
        """
        To be used when synthesising the gdml files for direct use to
        define the geometry. Reformats the information file to use gdml
        headers.
        """
        self.add_other_info()
        infofile = os.path.join(self.path_out, self.maus_information_file)
        info = libxml2.parseFile(infofile)
        doc = libxml2.parseDoc("""<?xml version="1.0"?>
        <gdml>
        <define/>
        <materials/>
        <solids/>
        <structure/>
        <setup/>
        </gdml>""")
        root = doc.xpathEval("gdml")[0]
        root.addChildList(info.xpathEval("MICE_Information")[0].copyNode(1))

        f = open(infofile,"w")
        doc.saveTo(f)
        f.close()

        doc.freeDoc()
        info.freeDoc()

    def formatForGDML(self):
        """
        @method FormatForGDML
        
        This method calls all the other methods to format the 
        necessary parts of the file. It will run through all of the
        fastRad outputted files within the folder location given to
        the class constructor.
        """
        # self.format_check(self.configuration_file)
        
        if self.beamline_file != None:
            self.merge_run_info(self.maus_information_file)
        else:
            shutil.copy(os.path.join(self.path_in, self.maus_information_file), 
                       os.path.join(self.path_out, self.maus_information_file))
        self.add_other_info()
        if self.coolingChannel_file != None:
            self.merge_cooling_channel_info(self.maus_information_file)
            
            
        # if self.formatted == False:
        print self.configuration_file
        self.format_schema_location(self.configuration_file)
        self.correct_gdml_locations(self.configuration_file)
        self.addDiffuserIrises()
        # self.merge_maus_info(self.configuration_file)
        print "Formatted Configuration File"
        self.maus_info_to_gdml()
        if self.tracker_file != None:
            self.format_check(self.tracker_file)
            if self.formatted == False:
                print self.tracker_file
                self.format_schema_location(self.tracker_file)
                self.correct_gdml_locations(self.tracker_file)
                # print "Formatted cooling channel and trackers"
        for module in self.modulefiles:
            self.format_check(module)
            if self.formatted == False:
                self.format_schema_location(module)
                self.correct_gdml_locations(module)
                # print "Formatted module file ",module
        for module in self.detmodulefiles:
            # self.format_check(module)
            if self.formatted == False:
                self.format_schema_location(module)
                self.correct_gdml_locations(module)
                # print "Formatted detector module file ",module
        noofstepfiles = len(self.stepfiles)
        for num in range(0, noofstepfiles):
            self.format_schema_location(self.stepfiles[num])
            # print "Formatted step file ",self.stepfiles[num]
            # self.correct_gdml_locations(self.stepfiles[num])
            
        
        print "Format Complete"

def main():
    """
    Main Function.
    """
if __name__ == "__main__":
    main()
