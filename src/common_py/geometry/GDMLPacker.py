"""
M. Littlefield
"""
#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
# 
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

import os.path
import zipfile
from datetime import datetime

class Packer: #pylint: disable = R0903
    """
    @Class Packer This class packs the GDMLs into a zip file.
    
    This class collects the files needed to be uploaded and zips them.
    """
    def __init__(self, filelist):
        """
        @Method Class Constructor

        This method is the class initialiser and takes one paramater.
        The parameter asks for the file name/path of a text file. 
        This text file must contain the file names and paths of the GDML files 
        which need to be zipped.

        @Param filelist file name/path of the file containing the list of GDML
                        files to be zipped.
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
        @Method zipfile takes the collated file, containing all the GDMLs, 
                        and zips it.

        This method zips the GDML files and a text file with a list of files
        present. An argument of the output file is specified. Default is 
        $maus/src/common_py/geometry/zippedGeoms. This method
        also names the zip file as the time and date when method is called.

        @Param Output Path specify the output path of the zipfile
        """
        if os.path.exists(path) == False:
            raise IOError('Path does not exist!', 'Packer::zipfile')
        else:
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
        
class Unpacker: #pylint: disable = R0903
    """
    @class Unpacker This class unzips zip files.
    """
    def __init__(self, input_file, extract_path):
        """
        @method Class Constructor This is the class constructor
        
        The class constructor takes two arguments. The first is the name
        of the file which is to be unzipped. The second is the location where 
        this file will be unzipped too.
        
        @param zfile: Name of the file to be unzipped
        @param extraction path: Name of the folder where the information will 
                                be unzipped.
        """
        if input_file[-4:] != '.zip':
            raise IOError('File is not a zip file!', 'Unpacker::__init__')
        elif os.path.exists(extract_path) == False:
            raise IOError('Path does not exist!', 'Unpacker::__init__')
        else:    
            self.input_file = input_file
            self.extract_path = extract_path
        
    def unzip_file(self):
        """
        @method unzip file This method unzips the zip file.
        """
        zfile = zipfile.ZipFile(self.input_file)
        zfile.extractall(self.extract_path)

def main():
    """
    Main Function
    """

if __name__ == "__main__":
    main()
