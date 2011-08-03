"""
Executable file to upload geometries to the CDB
M. Littlefield 02/08/11 
"""
import os.path
from GDMLPacker import packer
from GDMLtoCDB import gdmltocdb
from GDMLFormatter import Formatter
from ConfigReader import Configreader

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
    #format the GDMLs so the paths to the schema and materials file are correct
    gdmls = Formatter(inputfile.gdmldir)
    gdmls.format()
    #upload the geometry
    uploadgeometry = GDMLtocdb(inputfile.gdmldir, inputfile.geometrynotes, 1)
    uploadgeometry.upload_to_cdb()
    #delete the text file produced by uploadtoCDB()
    path = inputfile.gdmldir + '/FileList.txt'
    #zip the files and store them locally if selected
    if inputfile.zipfile == True:
        zfile = Packer(path)
        zippedgeoms = '/src/common_py/geometry/zippedGeoms'
        zippath = os.environ['MAUS_ROOT_DIR'] + zippedgeoms
        zfile.zipfile(zippath)
    #delete original GDML files as they are now zipped if selected
    os.remove(path)
    if inputfile.deleteoriginals == True:
        gdmls = os.listdir(inputfile.gdmldir)
        for fname in gdmls:
            path = inputfile.gdmldir + '/' +fname
            os.remove(path)


if __name__ == "__main__":
    main()
