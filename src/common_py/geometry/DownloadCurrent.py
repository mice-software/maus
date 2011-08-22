import os
from geometry.GDMLtoCDB import Downloader
from geometry.GDMLtoMAUSModule import GDMLtomaus
from geometry.ConfigReader import Configreader
from geometry.GDMLFormatter import Formatter
from geometry.GDMLPacker import Unpacker

def main():
    inputfile = Configreader()
    inputfile.readconfig()
    CurrentGeometry = Downloader(1)
    CurrentGeometry.download_current(inputfile.downloaddir)
    path = inputfile.downloaddir + '/Geometry.zip'
    zipped_geom = Unpacker(path, inputfile.downloaddir)
    zipped_geom.unzip_file()
    os.remove(path)
    os.remove(inputfile.downloaddir + '/FileList.txt')
    gdmls = Formatter(inputfile.downloaddir)
    gdmls.format()
    MausModules = GDMLtomaus(inputfile.downloaddir)
    outputlocation = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/Download' 
    MausModules.convert_to_maus(outputlocation)
    print "Download Complete!"

if __name__ == "__main__":
    main()