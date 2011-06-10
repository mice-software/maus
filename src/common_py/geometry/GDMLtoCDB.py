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
    geometry1 = gdmltocdb("GDML.zip", 1)
    #geometry1.uploadToCDB()
    geometry1.downloadCurrent()


if __name__ == "__main__":
    main()


class gdmltocdb:
    def __init__(self, file, testserver):
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
        NumOfFiles = len(self.GeometryFiles)
        for num in range(0, NumOfFiles):
            fin = open(self.GeometryFiles[num], 'r')
            for lines in fin.readlines():
                str = lines
                self.UploadString += str
            self.UploadString += "\nThis line is the file seperator!!\n"
        fin.close()
        _dt = datetime.today()
        _gdml = b64encode(self.UploadString)
        self.Geometry.setGDML(_gdml, _dt)

    def downloadCurrent(self):
        NumOfFiles = len(self.GeometryFiles)
        downloadedfile = b64decode(self.Geometry.getCurrentGDML())
        for num in range(0, NumOfFiles):
            path = "Download/" + self.GeometryFiles[num]
            fout = open(path, 'w')
            str_list = []
            str_list = downloadedfile.rsplit("This line is the file seperator!!")
            fout.write(str_list[num])
            fout.close()