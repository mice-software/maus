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

import os.path
from geometry.GDMLPacker import Packer
from geometry.GDMLtoCDB import GDMLtocdb
from geometry.ConfigReader import Configreader

def main():
    """
    Executable File
    
    This file is the executable file which uploads the chosen geometry to the Configuration Database.
    It takes a command line argument of a txt file which contains variables which will aid the uploading of the 
    geometry. The main variables are the path to the folder which contains ONLY the fastrad outputted files. The 
    second variable is the string which describes that geometry.   
    """
    #configreader class takes the arguments from the txt file so we can use them
    inputfile = Configreader()
    inputfile.readconfig()
    #upload the geometry
    uploadgeometry = GDMLtocdb(inputfile.gdmldir, inputfile.geometrynotes, 1)
    path = inputfile.gdmldir + '/FileList.txt'
    zfile = Packer(path)
    zippedgeoms = '/src/common_py/geometry/zippedGeoms'
    zippath = os.environ['MAUS_ROOT_DIR'] + zippedgeoms
    upload_file = zfile.zipfile(zippath)
    uploadgeometry.upload_to_cdb(upload_file)
    #delete the text file produced by uploadtoCDB()
    os.remove(path)
    #delete zip file if specified
    if inputfile.zipfile == 0:
        os.remove(upload_file)
    #delete original GDML files as they are now zipped if selected
    if inputfile.deleteoriginals == 1:
        gdmls = os.listdir(inputfile.gdmldir)
        for fname in gdmls:
            path = inputfile.gdmldir + '/' +fname
            os.remove(path)


if __name__ == "__main__":
    main()
