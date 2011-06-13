import os.path
import os
import sys
lib_path = os.path.abspath('../../../suds-0.3.9/')
sys.path.append(lib_path)
from suds.client import Client
from datetime import datetime
from base64 import b64encode
from base64 import b64decode
import zipfile

def main():
    from GDMLtoCDB import gdmltocdb
    from GDMLtoCDB import downloader
    geometry1 = gdmltocdb("GDML.zip", 1)
    #geometry1.uploadToCDB()
    dlgeometry = downloader(1)
    dlgeometry.downloadCurrent()
    
if __name__ == "__main__":
    main()


class gdmltocdb:
    """
    @Class gdmltocdb, handles the uploading and downloading of geometries to the CDB>

    This class writes the geometries to the database. It requires an input of the zipped file of
    gdmls which also includes a text file which has the names of the gdmls zipped as well.
    In order to write to the datbase the geometry files within the zip file are re written into a
    single string and encoded before being uploaded to the CDB. This string is then
    """

    def __init__(self, file, testserver):
        """
        @Method Class constructor

        this method sets up the connection to the CDB and also does some preliminary set ups of the
        list of files etc needed to be uploaded. This is taken from the zip file produced by GDMLPacker

        @Param file name/path of the zip file
        @Param type 1 to set up the test server leave blank for real server
        """
        self.UploadString = ""
        filelist = []
        self.GeometryFiles = filelist
        self.ListOfGeometries = None
        self.File = file
        if testserver == None:
            self.WSDLURL = "supermouse server"
        elif testserver == 1:
            self.WSDLURL = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl"
        else: raise StandardError("Incorrect input", "upload::__init__")
        self.Geometry = Client(self.WSDLURL).service
        zfile = zipfile.ZipFile(self.File, 'r')
        zfile.extractall("GDML")
        path = "GDML"
        dirList = os.listdir(path)
        for fname in dirList:
            if fname[-4:] == ".txt":
                self.ListOfGeometries = fname
        fin = open(self.ListOfGeometries, 'r')
        for line in fin.readlines():
            self.GeometryFiles.append(line.strip())
        fin.close()

    def uploadToCDB(self):
        """
        @Method uploadtocdb, this method uploads the geometries to the CDB

        this method takes the files with in the zip file and re write them into a single string.
        this string is then encoded and uploaded to the database with a date stamp.
        """
        NumOfFiles = len(self.GeometryFiles)
        fin1 = open(self.ListOfGeometries, 'r')
        for lines in fin1.readlines():
            str = lines
            self.UploadString += str
            self.UploadString += "<-file name->\n"
        fin1.close()
        self.UploadString += "\nThis line is the file seperator!!\n"
        for num in range(0, NumOfFiles):
            fin2 = open(self.GeometryFiles[num], 'r')
            for lines in fin2.readlines():
                str = lines
                self.UploadString += str
            self.UploadString += "\nThis line is the file seperator!!\n"
        fin2.close()
        _dt = datetime.today()
        _gdml = b64encode(self.UploadString)
        self.Geometry.setGDML(_gdml, _dt)

class downloader:
    """
    @Class downloader, this class downloads geometries from the CDB

    This class downloads the information from the CDB and decodes and unpacks the
    information.
    """
    def __init__(self, testserver):
        """
        @Method Class constructor

        This method sets up the connection to the CDB ready to download.

        @Param testserver, If an argument of 1 is entered this will set a connection to the test CDB if left blank write to the actual CDB
        """
        filelist = []
        self.GeometryFiles = filelist
        self.ListOfGeometries = filelist
        if testserver == None:
            self.WSDLURL = "supermouse server"
        elif testserver == 1:
            self.WSDLURL = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl"
        else: raise StandardError("Incorrect input", "upload::__init__")
        self.Geometry = Client(self.WSDLURL).service
        
    def downloadCurrent(self):
        """
        @Method downloadcurrent, this method downloads the current valid geometry and writes the files

        this method decodes the uploaded geometry and acquires fromt the string this list of files contained
        within the upload. It then opens files in the ~/maus/src/common_py/geometries/Download which correspond
        to the related gdml files and write the contents to these files. 
        """
        downloadedfile = b64decode(self.Geometry.getCurrentGDML())
        files = downloadedfile.rsplit("<-file name->")
        self.ListOfGeometries = files
        Num = len(self.GeometryFiles)
        Num = Num -1
        self.ListOfGeometries.pop(Num)
        NumOfFiles = len(self.ListOfGeometries)
        for n in range(0, NumOfFiles):
            str = self.ListOfGeometries[n]
            if str.endswith('\n'):
                str = str.rstrip('\n')
            if str.startswith('\n'):
                str = str.lstrip('\n')
            self.ListOfGeometries[n] = str
        geometries = downloadedfile.rsplit("This line is the file seperator!!")
        NumOfGeoms = len(geometries)
        NumOfGeoms = NumOfGeoms - 1
        self.GeometryFiles = geometries
        self.GeometryFiles.pop(NumOfGeoms)
        self.GeometryFiles.pop(0)
        for num in range(0, NumOfFiles):
            path = "Download/" + self.ListOfGeometries[num]
            fout = open(path, 'w')
            fout.write(self.GeometryFiles[num])
            fout.close()