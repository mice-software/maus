import os
from GDMLtoCDB import Downloader
from GDMLtoMAUSModule import GDMLtomaus
from ConfigReader import Configreader
from GDMLFormatter import Formatter

def main():
    inputfile = Configreader()
    inputfile.readconfig()
    CurrentGeometry = Downloader(1)
    CurrentGeometry.download_geometry_for_run(inputfile.starttime, inputfile.downloaddir)
    gdmls = Formatter(inputfile.downloaddir)
    gdmls.format()
    MausModules = GDMLtomaus(inputfile.downloaddir)
    outputlocation = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/Download' 
    MausModules.convert_to_maus(outputlocation)
    print "Download Complete!"

if __name__ == "__main__":
    main()