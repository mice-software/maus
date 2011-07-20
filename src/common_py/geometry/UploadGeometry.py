import os.path
import os
import sys
lib_path = os.path.abspath('../../../argparse-1.2.1')
sys.path.append(lib_path)
import argparse
from GDMLPacker import packer
from GDMLtoCDB import gdmltocdb
from GDMLFormatter import formatter
from ConfigReader import configreader

def main():
    inputfile = configreader()
    inputfile.readconfig()
    #format the gdmls
    gdmls = formatter(inputfile.GDMLDir)
    gdmls.format()
    # upload the geometry by passing the dierectory containing the files to gdmltocdb
    uploadGeometry = gdmltocdb(inputfile.GDMLDir, inputfile.GeometryNotes, 1)
    uploadGeometry.uploadToCDB()
    path = inputfile.GDMLDir + '/FileList.txt'
    #zip the files and store them locally if selected
    if inputfile.ZipFile == True:
        zfile = packer(path)
        zfile.zipfile()
    #delete original GDML files as they are now zipped if selected
    if inputfile.DeleteOriginals == True:
        gdmls = os.listdir(inputfile.GDMLDir)
        for fname in gdmls:
            path = command_line_arg['GDMLdir'] + '/' +fname
            os.remove(path)


if __name__ == "__main__":
    main()
