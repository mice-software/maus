import os.path
import zipfile

def main():
    from GDMLPacker import packer
    filelist1 = packer("geom1.txt", "filltobezipped.txt")
    filelist1.gdmlToOneFile()
    filelist1.zipfile()

if __name__ == "__main__":
    main()

class packer:
    """
    This class will read in a text file containing a list of GDML files which need to be zipped
    in order to upload them to the CDB. First the file contents need to be writen into one file
    of string format. This file is then zipped.
    """
    def __init__(self, filelist, zipfile):
        """
        @Method Class Initialiser

        This method is the class initialiser and takes two parameters.
        The first parameter is the file name/path of the text file containing the file names/paths
        which are needed to be zipped. The second parameter takes the name of which the user would like
        to call the file which will be placed in the zip file.

        @Param filelist, file name/path of the file containing the list of GDML files to be zipped.
        @Param zipfile,  the name of the file which will be zipped.
        """
        self.ZipFileName = zipfile
        list = []
        self.List = list
        self.FileList = filelist
        fin = open(self.FileList, 'r')
        for line in fin.readlines():
            self.List.append(line.strip())
     
    def gdmlToOneFile(self):
        """
        @Method gdmlToOneFile, takes mutliple GDML files and places them into one file.
        
        This method is a preprocessor which collates all the information
        taken from the files in the file list and appends them all into one file.
        """
        fin1 = open(self.FileList, 'r')
        NumOfFiles = len(fin1.readlines())
        fout = open(self.ZipFileName, 'w')
        for n in range(0, NumOfFiles):
            file = self.List[n]
            fin2 = open(file)
            for lines in fin2.readlines():
                str = lines
                print >>fout, str
            print >>fout, "*********************** END OF ", self.List[n], "**************************"
        fin1.close()
        fin2.close()

    def zipfile(self):
        """
        @Method zipfile, takes the collated file, containing all the GDMLs, and zips it.

        This method take the collated GDML file and produces a zipped file.
        """
        print self.ZipFileName
        name = self.ZipFileName
        file = zipfile.ZipFile("GDML.zip", 'w')
        file.write(name, os.path.basename(name), zipfile.ZIP_DEFLATED)
        file.close()