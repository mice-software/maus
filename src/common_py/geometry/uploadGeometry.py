import uploadGeometry
import os.path
import os
import sys
lib_path = os.path.abspath('../../../argparse-1.2.1')
sys.path.append(lib_path)
import argparse
from GDMLPacker import packer
from GDMLtoCDB import gdmltocdb

def main():
    parser = argparse.ArgumentParser(description='give the directory which contains the fastrad GDML files')
    parser.add_argument('GDMLdir', help = 'name the directory which contains the GDML files')
    parser.add_argument('-zip', action = 'store_true', help = 'add argument to produce a zip file of GDMLs')
    #set arguments into dict so we can use them
    obj = parser.parse_args()
    obj = obj.__dict__
    #write a text file in the GDML directory which contains a list of the files to be uploaded
    gdmls = os.listdir(obj['GDMLdir'])
    NumOfFiles = len(gdmls)
    path = obj['GDMLdir'] + "/FileList.txt"
    fout = open(path, 'w')
    for num in range(0, NumOfFiles):
        filepath = obj['GDMLdir'] + "/" + gdmls[num]
        fout.write(filepath)
        fout.write('\n')
    fout.close()
    #zip the geometry using GDML packer
    zfile = packer(path)
    zfile.zipfile(obj['GDMLdir'])
    for fname in gdmls:
        if fname[-4:] == '.zip':
            zippedfile = fname
    #pass the zipped file to gdmltocdb to upload to the database
    uploadGeometry = gdmltocdb(zippedfile, 1)
    uploadGeometry.uploadToCDB()
    

    
    
        
    

if __name__ == "__main__":
    main()
