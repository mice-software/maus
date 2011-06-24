import os.path
import os
import sys
lib_path = os.path.abspath('../../../../argparse-1.2.1')
sys.path.append(lib_path)
import argparse
from GDMLPacker import packer
from GDMLtoCDB import gdmltocdb
from GDMLFormatter import formatter


def main():
    parser = argparse.ArgumentParser(description='give the directory which contains the fastrad GDML files')
    parser.add_argument('GDMLdir', help = 'name the directory which contains the GDML files')
    parser.add_argument('GeometryNotes', help = 'please give a brief description of the geometry')
    #set arguments into dict so we can use them
    obj = parser.parse_args()
    obj = obj.__dict__
    #format the gdmls
    gdmls = formatter(obj['GDMLdir'])
    gdmls.format()
    # upload the geometry by passing the dierectory containing the files to gdmltocdb
    uploadGeometry = gdmltocdb(obj['GDMLdir'], obj['GeometryNotes'], 1)
    uploadGeometry.uploadToCDB()
    path = obj['GDMLdir'] + '/FileList.txt'
    #zip the files and store them locally
    zfile = packer(path)
    zfile.zipfile()
    #delete original GDML files as they are now zipped
    #gdmls = os.listdir(obj['GDMLdir'])
    #for fname in gdmls:
     #   path = obj['GDMLdir'] + '/' +fname
      #  os.remove(path)

if __name__ == "__main__":
    main()
