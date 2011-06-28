import os.path
import zipfile
from datetime import datetime

def main():
    from GDMLPacker import packer
    zip = packer("GDML/FileList.txt")
    zip.zipfile()

if __name__ == "__main__":
    main()

# need to re write comments and tests as it has been changed
class packer:
    """
    This class collects the files needed to be uploaded and zips them.
    """
    def __init__(self, filelist):
        """
        @Method Class Constructor

        This method is the class initialiser and takes one paramater.
        The parameter asks for the file name/path of a text file. This text file must contain
        the file names and paths of the GDML files which need to be zipped.

        @Param filelist, file name/path of the file containing the list of GDML files to be zipped.
        """
        list = []
        self.List = list
        if filelist[-4:] != ".txt":
            raise IOError("input file must be text format", "packer::__init__")
        else:
            self.FileList = filelist
        if os.path.getsize(self.FileList) == 0:
            raise StandardError("input file does not contain any information", "packer::__init__")
        else:
            fin = open(self.FileList, 'r')
            for line in fin.readlines():
                self.List.append(line.strip())
            fin.close()

    def zipfile(self, path='/home/matt/maus_littlefield/src/common_py/geometry/zippedGeoms'):
        """
        @Method zipfile, takes the collated file, containing all the GDMLs, and zips it.

        This method zips the GDML files and a text file with a list of files present. An argument of the
        output file is specified. Default is $maus/src/common_py/geometry/zippedGeoms. This method
        also names the zip file as the time and date when method is called.

        @Param Output Path, specify the output path of the zipfile
        """
        dt = str(datetime.today())
        dt = dt.replace('', 'Year', 1)
        dt = dt.replace('-', 'Mon', 1)
        dt = dt.replace('-', 'Day', 1)
        dt = dt.replace(' ', 'Hour', 1)
        dt = dt.replace(':', 'Min', 1)
        end = dt.find(':')
        dt = dt[0:end]
        zippath = path + "/" + dt +".zip"
        file = zipfile.ZipFile(zippath, 'w')
        fin = open(self.FileList, 'r')
        NumOfFiles = len(fin.readlines())
        for n in range(0, NumOfFiles):
            file.write(self.List[n], os.path.basename(self.List[n]), zipfile.ZIP_DEFLATED)
        file.write(self.FileList, os.path.basename(self.FileList), zipfile.ZIP_DEFLATED)
        file.close()
        fin.close()
