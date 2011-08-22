"""
GDML Packer class
M. Littlefield 02/08/11 
"""
import os.path
import zipfile
from datetime import datetime

# need to re write comments and tests as it has been changed
class Packer:
    #pylint: disable = R0903
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
        alist = []
        self.list = alist
        if filelist[-4:] != ".txt":
            raise IOError("input file must be text format", "Packer::__init__")
        else:
            self.filelist = filelist
        if os.path.getsize(self.filelist) == 0:
            raise StandardError("input file is blank", "Packer::__init__")
        else:
            fin = open(self.filelist, 'r')
            for line in fin.readlines():
                self.list.append(line.strip())
            fin.close()

    def zipfile(self, path):
        """
        @Method zipfile, takes the collated file, containing all the GDMLs, and zips it.

        This method zips the GDML files and a text file with a list of files present. An argument of the
        output file is specified. Default is $maus/src/common_py/geometry/zippedGeoms. This method
        also names the zip file as the time and date when method is called.

        @Param Output Path, specify the output path of the zipfile
        """
        timenow = str(datetime.today())
        timenow = timenow.replace('', 'Year', 1)
        timenow = timenow.replace('-', 'Mon', 1)
        timenow = timenow.replace('-', 'Day', 1)
        timenow = timenow.replace(' ', 'Hour', 1)
        timenow = timenow.replace(':', 'Min', 1)
        end = timenow.find(':')
        timenow = timenow[0:end]
        zippath = path + "/" + timenow +".zip"
        zfile = zipfile.ZipFile(zippath, 'w')
        fin = open(self.filelist, 'r')
        numoffiles = len(fin.readlines())
        for num in range(0, numoffiles):
            geomlist = self.list[num]
            base = os.path.basename(self.list[num])
            zfile.write(geomlist, base, zipfile.ZIP_DEFLATED)
        base = os.path.basename(self.filelist)
        zfile.write(self.filelist, base, zipfile.ZIP_DEFLATED)
        zfile.close()
        fin.close()
        return zippath
    
class Unpacker:
    def __init__(self, file, extract_path):
        #may need a little test
        self.file = file
        self.extract_path = extract_path
    
    def unzip_file(self):
        zfile = zipfile.ZipFile(self.file)
        zfile.extractall(self.extract_path)

def main():
    """
    Main Function
    """
    test = "/home/matt/workspace/Maus/testCases/GDML_fastradModel/FileList.txt"
    filetozip = Packer(test)
    path = '/home/matt/maus-littlefield/src/common_py/geometry/zippedGeoms'
    filetozip.zipfile(path)

if __name__ == "__main__":
    main()
