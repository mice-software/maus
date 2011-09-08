"""
Download geometry for an Run Num exectuable
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
    This Executable downloads the geometry for 
    a given run number. 
    """
    inputs = Configreader()
    inputs.readconfig()
    current_geometry = Downloader(1)
    #Download beamline information
    #Donwload geometry which is valid for time of run
    #id_num = current_geometry.get_ids(current_geometry.times[0], current_geometry.times[1]) !!!!! no current valid geometries
    #id_num = current_geometry.get_ids('2011-09-08 14:30:00', '2011-09-08 14:45:00')
    #current_geometry.download_geometry_for_id(id_num, inputs.downloaddir)
    current_geometry.download_current(inputs.downloaddir)
    path = inputs.downloaddir + '/Geometry.zip'
    zipped_geom = Unpacker(path, inputs.downloaddir)
    zipped_geom.unzip_file()
    os.remove(path)
    os.remove(inputs.downloaddir + '/FileList.txt')
    current_geometry.download_beamline_for_run(inputs.runnum, inputs.downloaddir) #this needs to move when current geoms are valid
    gdmls = Formatter(inputs.downloaddir)
    gdmls.format()
    maus_modules = GDMLtomaus(inputs.downloaddir)
    maus = os.environ['MAUS_ROOT_DIR']
    outputlocation = maus + '/src/common_py/geometry/Download' 
    maus_modules.convert_to_maus(outputlocation)
    print "Download Complete!"

if __name__ == "__main__":
    main()