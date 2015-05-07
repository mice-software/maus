# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,


# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#

"""
Check that the extraction and processing of a set of GDML files will work
"""

import time
import unittest
import subprocess
import os
import shutil

from geometry.CADModuleExtraction import CADModuleExtraction
from geometry.GDMLFormatter import Formatter
from geometry.GDMLtoMAUSModule import GDMLtomaus


# pylint: disable = C0103, R0904, R0915, R0912, C0202

TMP_DIR = os.environ.get("MAUS_TMP_DIR")
ROOT_DIR = os.environ.get("MAUS_ROOT_DIR")
SOURCEGDML = os.path.join(ROOT_DIR, \
                          "tests/integration/test_utilities/"+\
                          "test_gdml_processing/examples/"+\
                          "Step_IV_Raw/Step_IV.gdml") 
SOURCEINFO = os.path.join(ROOT_DIR, \
                          "tests/integration/test_utilities/"+\
                          "test_gdml_processing/examples/"+\
                          "Step_IV_Raw/Maus_Information.gdml") 
DSTNTN_DIR = os.path.join(TMP_DIR, "test_gdml_extraction")
OUT_PARENT_GDML = "Step_IV"
FORMAT_GDML_DIR = os.path.join(TMP_DIR, "test_gdml_formatted")
PARENT_MODULE_FILE = os.path.join(FORMAT_GDML_DIR, \
                                  "ParentGeometryFile.dat")



def run_extraction():
    """
    Generate a set of files suitable for placement on the CDB
    """
    if os.path.exists(DSTNTN_DIR):
        shutil.rmtree(DSTNTN_DIR)
    os.mkdir(DSTNTN_DIR)
    ext = CADModuleExtraction(SOURCEGDML, SOURCEINFO, \
                              DSTNTN_DIR, OUT_PARENT_GDML)

    ext.EditMaterials()
    ext.select_module_by_position()
    
    gdmls = [x for x in os.listdir(DSTNTN_DIR) \
             if x.find("_ed.gdml") >= 0]
    
    for edfile in gdmls:
        os.remove(os.path.join(DSTNTN_DIR, edfile))
    
        

def run_formatting():
    """
    Format the output files 
    """
    if os.path.exists(FORMAT_GDML_DIR):
        shutil.rmtree(FORMAT_GDML_DIR)
    os.mkdir(FORMAT_GDML_DIR)
    
    gdml = Formatter(DSTNTN_DIR, FORMAT_GDML_DIR)
    
    gdml.formatForGDML()
    
    maus_modules = GDMLtomaus(FORMAT_GDML_DIR)
    maus_modules.generate_parent(FORMAT_GDML_DIR)
    
    
    
def run_test_sim():
    """
    Run a short simulation of the channel
    """
    
    test_dir = os.path.expandvars("${MAUS_ROOT_DIR}/bin/simulate_mice.py")
    print test_dir
    out_file = os.path.join(FORMAT_GDML_DIR, "maus_gdml_test.root")
    args = [test_dir, \
            '-simulation_geometry_filename', PARENT_MODULE_FILE, \
            '-output_root_file_name', out_file]
    
    proc = subprocess.Popen(args, stdin=subprocess.PIPE)
    while proc.poll() == None:
        proc.communicate('\n')
        time.sleep(1)
    proc.wait()
    return proc.returncode
    


class GDMLProcessingTestCase(unittest.TestCase):
    """
    Test that GDML extraction and processing operates properly
    """
    @classmethod
    def test_gdml_processing(cls):
        """
        @Method test_gdml_processing

        Generate a set of gdml files, format the files, and run a quick simulation with those files.
        """
        run_extraction()
        run_formatting()
        run_test_sim()
               

if __name__ == "__main__":
    unittest.main()
