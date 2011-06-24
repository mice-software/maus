import os.path
import os
import sys
lib_path = os.path.abspath('../../../../suds-0.3.9/')
sys.path.append(lib_path)
from suds.client import Client
from datetime import datetime
from base64 import b64encode
from base64 import b64decode

def main():
    from GDMLtoCDB import gdmltocdb
    from GDMLtoCDB import downloader
    geometry1 = gdmltocdb('/home/matt/NetBeansProjects/MAUSConfigPY/src/GDML', "test geometry case", 1)
    geometry1.uploadToCDB()
    dlgeometry = downloader(1)
    #dlgeometry.downloadCurrent()
    
if __name__ == "__main__":
    main()

class gdmltocdb:
    """
    @Class gdmltocdb, handles the uploading of geometries to the CDB

    This class writes the geometries to the database. It requires an input of the directory path which contains the GDML
    files produce by fastRad. It will search this path and find all the files within it and will write these as a list
    into a test file. This list of files and the contents of each individual file is then copied into one string
    and encoded and uploaded to the CDB.
    """
    def __init__(self, filepath, notes, testserver):
        """
        @Method Class constructor

        this method sets up the connection to the CDB and also does some preliminary set ups of the
        list of files etc needed to be uploaded. 

        @Param filepath path of the directory which contains the GDML files
        @Param type 1 to set up the test server leave blank for real server
        """
        #change unit tests
        if type(notes) != str:
            raise IOError('the note describing the geometry must be filled in', 'gdmltocdb::__init__')
        else:
            self.Notes = notes
        self.UploadString = ""
        filelist = []
        self.GeometryFiles = filelist
        self.ListOfGeometries = None
        if os.path.exists(filepath) != True:
            raise IOError("File path does not exist", "gdmltocdb::__init__")
        else:
            self.FilePath = filepath
        if testserver == None:
            self.WSDLURL = "supermouse server" # super maus wsdl needs to go here
        elif testserver == 1:
            self.WSDLURL = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl"
        else: raise StandardError("Incorrect input", "gdmltocdb::__init__")
        self.Geometry = Client(self.WSDLURL).service
        gdmls = os.listdir(self.FilePath)
        for fname in gdmls:
            if fname[-4:] != '.xml' and fname[-5:] != '.gdml':
                raise IOError("directory contains files which are not XMLs or GDMLs", "gdmltocdb::__init__")
        NumOfFiles = len(gdmls)
        path = self.FilePath + "/FileList.txt"
        fout = open(path, 'w')
        for num in range(0, NumOfFiles):
            filepath = self.FilePath + "/" + gdmls[num]
            fout.write(filepath)
            fout.write('\n')
        fout.close()
        self.ListOfGeometries = path
        fin = open(self.ListOfGeometries, 'r')
        for line in fin.readlines():
            self.GeometryFiles.append(line.strip())
        fin.close()

    def uploadToCDB(self):
        """
        @Method uploadtocdb, this method uploads the geometries to the CDB

        This method write the contents of all the gdmls and the text file which lists the gdmls
        into one string. This string is then encoded and uploaded to the CDB with a date stamp of
        when the method is called?.
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
        self.Geometry.setGDML(_gdml, _dt, self.Notes)
        print self.Geometry.setGDML(_gdml, _dt, self.Notes)

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
            str = self.ListOfGeometries[num]
            str = str.rsplit('/')
            filepos = len(str) - 1
            filename = str[filepos]
            path = "/home/matt/maus_littlefield/src/common_py/geometry/Download/" + filename
            fout = open(path, 'w')
            fout.write(self.GeometryFiles[num])
            fout.close()
