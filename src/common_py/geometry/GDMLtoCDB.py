import os.path
import os
import sys
lib_path = os.path.abspath('/home/matt/maus-littlefield/build/suds')
sys.path.append(lib_path)
from suds.client import Client
from datetime import datetime
from base64 import b64encode
from base64 import b64decode

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
        @Param notes must be a string briefly describing the geometry
        @Param type 1 to set up the test server leave blank for real server
        """
        #change unit tests
        self.TextFile = None
        self.Text = ""
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
        # check if there is an existing textfile with the lis of geomtries in it
        # if there is a text file but with no geometries raise error
        NumOfFiles = len(gdmls)
        count = 0
        for fname in gdmls:
            if fname[-4:] == '.txt':
                gdmls.remove(fname)
                NumCheck = len(gdmls)
                self.TextFile = fname
                path = self.FilePath + "/" + self.TextFile
                fin = open(path, 'r')
                for lines in fin.readlines():
                    self.Text += lines
                for num in range(0, NumCheck):
                    if self.Text.find(gdmls[num]) >= 0:
                        count += 1
                if NumCheck != count:
                    raise IOError("The text file in the gdml directory should list all files in this directory.Please remove this file or write the full path locations of all the files within the directory", "gdmltocdb:__init__")
                else:
                    # if one does exist and its correct create the list of files
                    self.ListOfGeometries = path
                    fin = open(self.ListOfGeometries, 'r')
                    for line in fin.readlines():
                        self.GeometryFiles.append(line.strip())
                    fin.close()
        # if there is no text file create one and fill it with the geometries.
        if self.TextFile == None:
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
            self.UploadString += "<!--  File Name  -->\n"
        fin1.close()
        self.UploadString += "\n<!--  End of File  -->\n"
        for num in range(0, NumOfFiles):
            fin2 = open(self.GeometryFiles[num], 'r')
            for lines in fin2.readlines():
                str = lines
                self.UploadString += str
            self.UploadString += "\n<!--  End of File  -->\n"
        fin2.close()
        _dt = datetime.today()
        _gdml = b64encode(self.UploadString)
        self.Geometry.setGDML(_gdml, _dt, self.Notes)
        print self.Geometry.setGDML(_gdml, _dt, self.Notes)
	print 'Geometry Uploaded'
	

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
        
    def downloadCurrent(self, downloadPath):
        """
        @Method downloadcurrent, this method downloads the current valid geometry and writes the files

        this method decodes the uploaded geometry and acquires fromt the string this list of files contained
        within the upload. It then opens files in the ~/maus/src/common_py/geometries/Download which correspond
        to the related gdml files and write the contents to these files. 
        """
        downloadedfile = b64decode(self.Geometry.getCurrentGDML())
        files = downloadedfile.rsplit("<!--  File Name  -->")
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
        geometries = downloadedfile.rsplit("<!--  End of File  -->")
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
            path = downloadPath + "/" + filename
            fout = open(path, 'w')
            fout.write(self.GeometryFiles[num])
            fout.close()
            
def main():
 #   from GDMLtoCDB import gdmltocdb
#    from GDMLtoCDB import downloader
    geometry1 = gdmltocdb('/home/matt/workspace/Maus/testCases/GDML_fastradModel', "test geometry case", 1)
    geometry1.uploadToCDB()
    #dlgeometry = downloader(1)
    #dlgeometry.downloadCurrent()
    
if __name__ == "__main__":
    main()
