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
                         "/src/common_py/geometry/xsltScripts/GDML2G4MICE.xsl"
MM_XSL = os.environ["MAUS_ROOT_DIR"] + \
                         "/src/common_py/geometry/xsltScripts/MMTranslation.xsl"

class GDMLtomaus(): #pylint: disable = R0903
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
        self.field_file = None
        self.config_file = None
        self.material_file = None
        self.material_file_path = None
        file_list = []
        self.step_files = file_list
        if os.path.exists(path) == False:
            raise IOError('Path does not exist '+str(path))
        else:
            self.path = path
        gdmls = os.listdir(self.path)
        for fname in gdmls:
            if fname.find('materials') >= 0:
                found_file = self.path + '/' + fname
                self.material_file = found_file
                self.material_file_path = os.path.abspath(self.material_file)
            if fname == 'fastradModel.gdml' or fname == 'FastradModel.gdml':
                found_file = self.path + '/' + fname
                self.config_file = found_file
            if fname.find('Field') >= 0:
                found_file = self.path + '/' + fname
                self.field_file = found_file
            if fname.find('materials') < 0 \
               and fname.find('fastrad') < 0 \
               and fname.find('Fastrad') < 0 \
               and fname.find('Field') < 0 \
               and fname.find('Beamline') < 0 \
               and fname[-5:] == '.gdml':
                stepfile = self.path + '/' + fname
                self.step_files.append(stepfile)

    def convert_to_maus(self, output):
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
            outputfile = output + "/ParentGeometryFile.dat"
            config_file = CADImport(xmlin1 = self.config_file, \
                           xsl = CONFIGXSL, output = outputfile)
            config_file.parse_xslt()
            print "Configuration File Converted"
            length = len(self.step_files)
            for num in range(0, length):
                found_file = str(self.step_files[num])
                
                new_string = found_file.split('/')
                num_of_splits = len(new_string)
                file_name = num_of_splits - 1
                file_name = new_string[file_name]
                outputfile = output + '/' + file_name[:-4] + 'dat'
                step_file = CADImport(xmlin1 = self.step_files[num], \
                                    xsl = MM_XSL, output = outputfile)
                step_file.parse_xslt()
                step_file = None
                os.remove(self.step_files[num])
                print "Converting " + str(num+1) + \
                                        " of " + str(length) + " Geometry Files"
            os.remove(self.config_file)

def main():
    """
    Main Function.
    """
if __name__ == "__main__":
    main()
