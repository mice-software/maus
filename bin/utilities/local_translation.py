#!/usr/bin/env python

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
import shutil
import geometry
from geometry.GDMLtoMAUSModule import GDMLtomaus
from geometry.GDMLFormatter import Formatter

def main():
    """
    This is the code for the executable file which downloads the current valid
    geometry. It takes the arguments of the download directory. It will download
    the geometry from the database and translates the geometry into MAUS Module
    format.
    """
    if os.path.exists('/home/matt/maus_littlefield/tmp/SIM_FILES') == False:
        os.mkdir('/home/matt/maus_littlefield/tmp/SIM_FILES')
    gdml_files = Formatter('/home/matt/StepFiles/EMR/small_version/', '/home/matt/maus_littlefield/tmp/SIM_FILES/')
    #gdml_files.format()
    
    new_gdml_files = GDMLtomaus('/home/matt/maus_littlefield/tmp/SIM_FILES/')
    new_gdml_files.convert_to_maus('/home/matt/maus_littlefield/tmp/SIM_FILES/')
    
    print 'Translation Complete'

if __name__ == "__main__":
    main()