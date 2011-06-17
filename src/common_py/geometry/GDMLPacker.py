import os.path
import zipfile
from datetime import datetime

def main():
    from GDMLPacker import packer
    filelist1 = packer("geom1.txt")
    filelist1.zipfile()

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

        This method is the class initialiser and takes two parameters.
        The first parameter is the file name/path of the text file containing the file names/paths
        which are needed to be zipped. The second parameter takes the name of which the user would like
        to call the file which will be placed in the zip file.

        @Param filelist, file name/path of the file containing the list of GDML files to be zipped.
        @Param zipfile,  the name of the zip file.
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

        This method take the collated GDML file and produces a zipped file.
        """
        #add more notes on new argument
        dt = str(datetime.today())
        zippath = path + "/" + dt +".zip"
        file = zipfile.ZipFile(zippath, 'w')
        fin = open(self.FileList, 'r')
        NumOfFiles = len(fin.readlines())
        for n in range(0, NumOfFiles):
            file.write(self.List[n], os.path.basename(self.List[n]), zipfile.ZIP_DEFLATED)
        file.write(self.FileList, os.path.basename(self.FileList), zipfile.ZIP_DEFLATED)
        file.close()
        fin.close()