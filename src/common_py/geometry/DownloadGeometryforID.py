"""
Download geometry for an ID exectuable
M.Littlefield
"""
import os
from geometry.GDMLtoCDB import Downloader
from geometry.GDMLtoMAUSModule import GDMLtomaus
from geometry.ConfigReader import Configreader
from geometry.GDMLFormatter import Formatter
from geometry.GDMLPacker import Unpacker

def main():
    """
    This is the executable which searches the database for
    an ID which is valid for the time period given as an 
    argument. It then downloads this geometry and translates
    it into MAUS Module format. 
    """
    inputs = Configreader()
    inputs.readconfig()
    current_geometry = Downloader(1)
    if inputs.stoptime == None:
        id_num = current_geometry.get_ids(inputs.starttime)
        current_geometry.download_geometry_for_id(id_num, inputs.downloaddir)
    else:
        id_num = current_geometry.get_ids(inputs.starttime, inputs.stoptime)
        current_geometry.download_geometry_for_id(id_num, inputs.downloaddir)
    path = inputs.downloaddir + '/Geometry.zip'
    zipped_geom = Unpacker(path, inputs.downloaddir)
    zipped_geom.unzip_file()
    os.remove(path)
    os.remove(inputs.downloaddir + '/FileList.txt')
    gdmls = Formatter(inputs.downloaddir)
    gdmls.format()
    maus_modules = GDMLtomaus(inputs.downloaddir)
    maus = os.environ['MAUS_ROOT_DIR']
    outputlocation = maus + '/src/common_py/geometry/Download' 
    maus_modules.convert_to_maus(outputlocation)
    print "Download Complete!"

if __name__ == "__main__":
    main()