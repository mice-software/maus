"""
Configuration Database Upload/Download Classes
M. Littlefield 02/08/11 
"""
import os
from suds.client import Client
from datetime import datetime
from base64 import b64encode
from base64 import b64decode

class GDMLtocdb:
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
        self.textfile = None
        self.text = ""
        if type(notes) != str:
            raise IOError('Geom desciprtion missing!', 'gdmltocdb::__init__')
        else:
            self.notes = notes
            self.uploadstring = ""
        filelist = []
        self.geometryfiles = filelist
        self.listofgeometries = None
        if os.path.exists(filepath) != True:
            raise IOError("File path does not exist", "gdmltocdb::__init__")
        else:
            self.filepath = filepath
        if testserver == None:
            self.wsdlurl = "supermouse server" # actual wsdl
        elif testserver == 1:
            twsdl = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl"
            self.wsdlurl = twsdl 
        else: raise StandardError("Incorrect input", "gdmltocdb::__init__")
        self.geometry = Client(self.wsdlurl).service
        gdmls = os.listdir(self.filepath)
        # check if there is an existing textfile with the lis of geomtries in it
        # if there is a text file but with no geometries raise error
        numoffiles = len(gdmls)
        count = 0
        for fname in gdmls:
            if fname[-4:] == '.txt':
                gdmls.remove(fname)
                numcheck = len(gdmls)
                self.textfile = fname
                path = self.filepath + "/" + self.textfile
                fin = open(path, 'r')
                for lines in fin.readlines():
                    self.text += lines
                for num in range(0, numcheck):
                    if self.text.find(gdmls[num]) >= 0:
                        count += 1
                if numcheck != count:
                    errormsg = "Text file doesnt list fastrad files"
                    raise IOError(errormsg, "gdmltocdb:__init__")
                else:
                    # if one does exist and its correct create the list of files
                    self.listofgeometries = path
                    fin = open(self.listofgeometries, 'r')
                    for line in fin.readlines():
                        self.geometryfiles.append(line.strip())
                    fin.close()
        # if there is no text file create one and fill it with the geometries.
        if self.textfile == None:
            path = self.filepath + "/FileList.txt"
            fout = open(path, 'w')
            for num in range(0, numoffiles):
                filepath = self.filepath + "/" + gdmls[num]
                fout.write(filepath)
                fout.write('\n')
            fout.close()
            self.listofgeometries = path
            fin = open(self.listofgeometries, 'r')
            for line in fin.readlines():
                self.geometryfiles.append(line.strip())
            fin.close()
        

    def upload_to_cdb(self):
        """
        @Method upload_to_cdb, this method uploads the geometries to the CDB

        This method write the contents of all the gdmls and the file which lists the gdmls
        into one string. This string is then encoded and uploaded to the CDB with a date stamp of
        when the method is called?.
        """
        numoffiles = len(self.geometryfiles)
        fin1 = open(self.listofgeometries, 'r')
        for lines in fin1.readlines():
            filestr = lines
            self.uploadstring += filestr
            self.uploadstring += "<!--  File Name  -->\n"
        fin1.close()
        self.uploadstring += "\n<!--  End of File  -->\n"
        for num in range(0, numoffiles):
            fin2 = open(self.geometryfiles[num], 'r')
            for lines in fin2.readlines():
                filestr = lines
                self.uploadstring += filestr
            self.uploadstring += "\n<!--  End of File  -->\n"
        fin2.close()
        _dt = datetime.today()
        _gdml = b64encode(self.uploadstring)
        self.geometry.setGDML(_gdml, _dt, self.notes)
        print self.geometry.setGDML(_gdml, _dt, self.notes)

class Downloader:
    """
    @Class Downloader, this class downloads geometries from the CDB

    This class downloads the information from the CDB and decodes and unpacks the
    information.
    """
    def __init__(self, testserver):
        """
        @Method Class constructor

        This method sets up the connection to the CDB ready to download.

        @Param testserver, If an argument of 1 is entered this will set a connection to the test CDB if left blank write to the actual CDB
        """
        self.filestr =""
        filelist = []
        self.geometryfiles = filelist
        self.listofgeometries = filelist
        if testserver == None:
            self.wsdlurl = "supermouse server"
        elif testserver == 1:
            twsdl = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl"
            self.wsdlurl = twsdl 
        else: raise StandardError("Incorrect input", "upload::__init__")
        self.geometry = Client(self.wsdlurl).service
        
    def download_current(self, downloadpath):
        """
        @Method download_current, this method downloads the current valid geometry and writes the files

        this method decodes the uploaded geometry and acquires fromt the string this list of files contained
        within the upload. It then opens files in the ~/maus/src/common_py/geometries/Download which correspond
        to the related gdml files and write the contents to these files. 
        """
        downloadedfile = b64decode(self.geometry.getCurrentGDML())
        files = downloadedfile.rsplit("<!--  File Name  -->")
        self.listofgeometries = files
        num = len(self.geometryfiles)
        num = num -1
        self.listofgeometries.pop(num)
        numoffiles = len(self.listofgeometries)
        for gfile in range(0, numoffiles):
            filestr = self.listofgeometries[gfile]
            if filestr.endswith('\n'):
                filestr = filestr.rstrip('\n')
            if filestr.startswith('\n'):
                filestr = filestr.lstrip('\n')
            self.listofgeometries[gfile] = filestr
        geometries = downloadedfile.rsplit("<!--  End of File  -->")
        numofgeoms = len(geometries)
        numofgeoms = numofgeoms - 1
        self.geometryfiles = geometries
        self.geometryfiles.pop(numofgeoms)
        self.geometryfiles.pop(0)
        for num in range(0, numoffiles):
            filestr = self.listofgeometries[num]
            filestr = filestr.rsplit('/')
            filepos = len(filestr) - 1
            filename = filestr[filepos]
            path = downloadpath + "/" + filename
            fout = open(path, 'w')
            fout.write(self.geometryfiles[num])
            fout.close()
            self.remove_first_line(path)
        
    def remove_first_line(self, file_path):
        fin = open(file_path, 'r')
        for lines in fin.readlines():
            self.filestr += lines
        self.filestr = self.filestr.lstrip()
        fin.close()
        fout = open(file_path, 'w')
        fout.write(self.filestr)
        fout.close()
        self.filestr =""
        
            
            
def main():
    """
    Main function
    """
 #   from GDMLtoCDB import gdmltocdb
#    from GDMLtoCDB import Downloader
    #test = '/home/matt/workspace/Maus/testCases/GDML_fastradModel'
    #geometry1 = GDMLtocdb(test, "test geometry case", 1)
    #geometry1.upload_to_cdb()
    dlgeometry = Downloader(1)
    #dlgeometry.download_current()
    dlgeometry.remove_first_line('/home/matt/maus-littlefield/src/common_py/geometry/Download/fastradModel.gdml')
if __name__ == "__main__":
    main()
