import os.path
import os
import sys
lib_path = os.path.abspath('../../../argparse-1.2.1')
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
    command_line_arg = parser.parse_args()
    command_line_arg = command_line_arg.__dict__
    #format the gdmls
    gdmls = formatter(command_line_arg['GDMLdir'])
    gdmls.format()
    # upload the geometry by passing the dierectory containing the files to gdmltocdb
    uploadGeometry = gdmltocdb(command_line_arg['GDMLdir'], command_line_arg['GeometryNotes'], 1)
    uploadGeometry.uploadToCDB()
    path = command_line_arg['GDMLdir'] + '/FileList.txt'
    #zip the files and store them locally
    zfile = packer(path)
    zfile.zipfile()
    #delete original GDML files as they are now zipped
    #gdmls = os.listdir(command_line_arg['GDMLdir'])
    #for fname in gdmls:
     #   path = command_line_arg['GDMLdir'] + '/' +fname
      #  os.remove(path)

if __name__ == "__main__":
    main()
