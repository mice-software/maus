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
from geometry.CADImport import CADImport

CONFIGXSL = os.environ["MAUS_ROOT_DIR"] + \
        "/src/common_py/geometry/xsltScripts/ParentFileTranlsation.xsl"
CONFIGXSL2 = os.environ["MAUS_ROOT_DIR"] + \
        "/src/common_py/geometry/xsltScripts/ParentFileTranslation.xsl"
MINCONFIGXSL = os.environ["MAUS_ROOT_DIR"] + \
        "/src/common_py/geometry/xsltScripts/ParentFileCreation.xsl"
TRACKERXSL = os.environ["MAUS_ROOT_DIR"] + \
         "/src/common_py/geometry/xsltScripts/CoolingChannelTranslation.xsl"
MODULEXSL = os.environ["MAUS_ROOT_DIR"] + \
         "/src/common_py/geometry/xsltScripts/MagnetModuleTranslation.xsl"
MM_XSL = os.environ["MAUS_ROOT_DIR"] + \
                         "/src/common_py/geometry/xsltScripts/MMTranslation.xsl"
DET_GDML = ['TOF0.gdml', 'TOF1.gdml', 'TOF2.gdml', \
            'Tracker0.gdml', 'Tracker1.gdml',\
            'KL.gdml', 'Ckov1.gdml', 'Ckov2.gdml', 'Disk_LiH.gdml',\
            'XeAbs.gdml', 'HeAbs.gdml', 'NeAbs.gdml',\
            'Wedge_LiH_90.gdml', 'Wedge_LiH_45.gdml', 'LH2.gdml','EMR.gdml',\
            'HeWindow.gdml', 'HeVolDSCap.gdml', 'HeVolUSCap.gdml',\
            'iris1_closed.gdml', 'iris2_open.gdml', 'iris3_open.gdml',\
            'iris3_closed.gdml', 'iris4_open.gdml',\
            'iris2_closed.gdml', 'iris1_open.gdml',\
            'iris4_closed.gdml']

class GDMLtomaus(): #pylint: disable = R0902, R0903
    """
    @class GDMLtomaus This class converts GDMLs to MAUS Modules
    
    This class converts GDMLs to MAUS Modules by applying a pre written
    XSLT over the GDML. This produces the necessary MAUS Modules and the 
    original GDMLs are then deleted.
    """
    def __init__(self, path):
        """
        @method Class Constructor This is the class constructor which gets the
                                  file ready to be converted.
            
        This method searches through the path, given by the argument, 
        and sorts out the files within the path into variables which 
        are then used by the next method to convert them to MAUS
        Modules.
            
        @param Path This is the path to the folder which contains the GDMLS to 
                    be converted.            
        """
        self.maus_information_file = None
        self.config_file = None
        self.material_file = None
        self.material_file_path = None
        self.tracker_file = None
        module_list = []
        self.module_files = module_list
        file_list = []
        self.step_files = file_list
        if os.path.exists(path) == False:
            raise IOError('Path does not exist '+str(path))
        else:
            self.path = path
        gdmls = os.listdir(self.path)
        # print gdmls
        for fname in gdmls:
            if fname.find('materials') >= 0:
                found_file = os.path.join(self.path, fname)
                self.material_file = found_file
                self.material_file_path = os.path.abspath(self.material_file)
            if fname == 'fastradModel.gdml' or \
                   fname == 'FastradModel.gdml' or \
                   fname == 'Step_IV.gdml':
                found_file = os.path.join(self.path, fname)
                self.config_file = found_file
            if fname == 'SpectrometerSolenoidUS.gdml' or \
                   fname == 'Cooling_Channel.gdml':
                found_file = os.path.join(self.path, fname)
                self.tracker_file = found_file
            if fname.find('Maus_Information') >= 0:
                found_file = os.path.join(self.path, fname)
                self.maus_information_file = found_file
            if fname == 'Quad456.gdml' or \
                   fname == 'Quad789.gdml' or \
                   fname == 'Dipole2.gdml' or \
                   fname == 'SpectrometerSolenoidDS.gdml' or \
                   fname == 'AFC.gdml':
                found_file = os.path.join(self.path, fname)
                self.module_files.append(found_file)
            if fname.find('materials') < 0 \
               and fname.find('fastrad') < 0 \
               and fname.find('Fastrad') < 0 \
               and fname.find('Maus_Information') < 0 \
               and fname.find('Beamline') < 0 \
               and fname.find('CoolingChannelInfo') < 0 \
               and fname[-5:] == '.gdml' \
               and fname.find('Step_IV') < 0 \
               and fname.find('SolenoidUS') < 0\
               and fname.find('SolenoidDS') < 0\
               and fname.find('Cooling_Channel') < 0\
               and fname.find('Quad456') < 0\
               and fname.find('Quad789') < 0\
               and fname.find('Dipole2.gdml') < 0\
               and fname.find('AFC') < 0:
                
                stepfile = os.path.join(self.path, fname)
                self.step_files.append(stepfile)

    def convert_to_maus(self, output): #pylint: disable = R0914
        """
        @method convert_to_maus This method converts the GDMLs to MAUS Modules.
        
        This method applies the necessary XSLT scripts to the GDMLs.
        The configuration file uses the GDML2G4MICE.xsl and the individual
        component files use MMTranslation.xsl XSLTs to convert to the correct
        MAUS Modules.
        
        @param Output This is the path to the directory where you wish the MAUS 
                      Modules to be placed. 
        """
        if os.path.exists(output) == False:
            raise IOError('Output path doesnt exist '+str(output))
        else:
            outputfile1 = os.path.join(output, "ParentGeometryFile.dat")
            # outputfile3 = os.path.join(output, "RotatedGeometryFile.dat")
            
            if str(self.tracker_file).find('Cooling_Channel') >= 0 or \
               str(self.tracker_file).find('TrackerSolenoidUS') >= 0:
                
                outputfile2 = os.path.join(output, self.tracker_file[:-4]+"dat")
                config_file = CADImport(xmlin1 = str(self.config_file), \
                                        xsl = str(CONFIGXSL2), \
                                        output = str(outputfile1))
                config_file.parse_xslt()
                tracker_file = CADImport(xmlin1 = str(self.tracker_file), \
                                         xsl = str(TRACKERXSL), \
                                         output = str(outputfile2))
                tracker_file.parse_xslt()
                # os.remove(self.tracker_file)
                print "Applying translation assuming a secondary mother volume"
            else:
                config_file = CADImport(xmlin1 = str(self.config_file), \
                                        xsl = str(CONFIGXSL), \
                                        output = str(outputfile1))
                config_file.parse_xslt()
                print "Applying single config file translation"
            # rotated_file = CADImport(xmlin1 = str(self.config_file), \
            #         xsl = str(ROTATEDXSL), output = str(outputfile3))
            # rotated_file.parse_xslt()
            
            print "Configuration File Converted"
            length = len(self.module_files)
            for fnum in range(0, length):
                found_file = str(self.module_files[fnum])
                print found_file
                new_string = found_file.split('/')
                file_name = new_string[-1]
                outputfile = output + '/' + file_name[:-4] + 'dat'
                module_file = CADImport(xmlin1 = str(self.module_files[fnum]), \
                                 xsl = str(MODULEXSL), output = str(outputfile))
                module_file.parse_xslt()
                module_file = None
                os.remove(self.module_files[fnum])
                
            length = len(self.step_files)
            for num in range(0, length):
                found_file = str(self.step_files[num])
                
                new_string = found_file.split('/')
                num_of_splits = len(new_string)
                file_name = num_of_splits - 1
                file_name = new_string[file_name]
                outputfile = output + '/' + file_name[:-4] + 'dat'
                step_file = CADImport(xmlin1 = str(self.step_files[num]), \
                                    xsl = str(MM_XSL), output = str(outputfile))
                direct_translation = 0
                for det_file in DET_GDML:
                    if file_name == det_file:
                        direct_translation = 1
                if direct_translation:
                    step_file.translate_gdml()
                else:
                    step_file.parse_xslt()
                # if file_name.find('Tracker') >= 0:
                #     step_file.TrackerPlaneParametrization()
                step_file = None
                os.remove(self.step_files[num])
                print "Converting " + str(num+1) + \
                      " of " + str(length)
            
            
            os.remove(self.config_file)
            print "Files saved to " + str(output)

    def generate_parent(self, output): #pylint: disable = R0914
        """
        @method generate_parent This method generates a parent file
        
        This method applies the necessary XSLT scripts to the GDMLs.
                
        @param Output This is the path to the directory where you wish the MAUS 
                      Modules to be placed. 
        """
        if os.path.exists(output) == False:
            raise IOError('Output path doesnt exist '+str(output))
        else:
            outputfile1 = os.path.join(output, "ParentGeometryFile.dat")
            # outputfile3 = os.path.join(output, "RotatedGeometryFile.dat")
            
            config_file = CADImport(xmlin1 = str(self.maus_information_file), \
                                    xsl = str(MINCONFIGXSL), \
                                    output = str(outputfile1))
            config_file.parse_xslt()
            print "Applying minimal config file translation"
            # rotated_file = CADImport(xmlin1 = str(self.config_file), \
            #         xsl = str(ROTATEDXSL), output = str(outputfile3))
            # rotated_file.parse_xslt()
            
            print "Configuration File Converted"
            length = len(self.module_files)
            for fnum in range(0, length):
                found_file = str(self.module_files[fnum])
                # print found_file
                new_string = found_file.split('/')
                file_name = new_string[-1]
                outputfile = output + '/' + file_name[:-4] + 'dat'
                module_file = CADImport(xmlin1 = str(self.module_files[fnum]), \
                                        xsl = str(MODULEXSL), \
                                        output = str(outputfile))
                module_file.parse_xslt()
                module_file = None

            length = len(self.step_files)
            for num in range(0, length):
                found_file = str(self.step_files[num])
                new_string = found_file.split('/')
                num_of_splits = len(new_string)
                file_name = num_of_splits - 1
                file_name = new_string[file_name]
                outputfile = output + '/' + file_name[:-4] + 'dat'
                step_file = CADImport(xmlin1 = str(self.step_files[num]), \
                                      xsl = str(MM_XSL), \
                                      output = str(outputfile))
                for det_file in DET_GDML:
                    if file_name == det_file:
                        step_file.translate_gdml()
                # ignore all other step files.
                        
            print "Files saved to " + str(output)

def main():
    """
    Main Function.
    """
if __name__ == "__main__":
    main()
