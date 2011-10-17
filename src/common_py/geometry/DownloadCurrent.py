"""
Download Current Geometry exectuable
M. Littlefield
"""
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