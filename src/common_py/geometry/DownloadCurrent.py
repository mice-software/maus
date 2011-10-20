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
from geometry.GDMLtoCDB import Downloader
from geometry.GDMLtoMAUSModule import GDMLtomaus
from geometry.ConfigReader import Configreader
from geometry.GDMLFormatter import Formatter
from geometry.GDMLPacker import Unpacker

def main():
    """
    This is the code for the executable file which downloads
    the current valid geometry. It takes the arguments of the 
    download directory. It will download the geometry from the
    database and translates the geometry into MAUS Module format.
    """
    #Read Config Arguments
    inputfile = Configreader()
    inputfile.readconfig()
    #Download file
    current_geometry = Downloader(1)
    current_geometry.download_current(inputfile.downloaddir)
    #Unzip file
    path = inputfile.downloaddir + '/Geometry.zip'
    zipped_geom = Unpacker(path, inputfile.downloaddir)
    zipped_geom.unzip_file()
    os.remove(path)
    os.remove(inputfile.downloaddir + '/FileList.txt')
    #Format Files
    gdmls = Formatter(inputfile.downloaddir)
    gdmls.format()
    #Convert to MAUS Modules
    maus_modules = GDMLtomaus(inputfile.downloaddir)
    maus = os.environ['MAUS_ROOT_DIR']
    outputlocation = maus + '/src/common_py/geometry/Download' 
    maus_modules.convert_to_maus(outputlocation)
    print "Download Complete!"

if __name__ == "__main__":
    main()
