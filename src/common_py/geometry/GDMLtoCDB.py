"""
Configuration Database Upload/Download Classes
M. Littlefield 02/08/11 
"""
import os
from suds.client import Client
from datetime import datetime
from base64 import b64encode
from base64 import b64decode

#pylint: disable = C0301, R0902

class GDMLtocdb:
    """
    @Class GDMLtocdb handles the uploading of geometries to the CDB

    This class writes the geometries to the database. It requires an input of the directory path which contains the GDML
    files produce by fastRad. It will search this path and find all the files within it and will write these as a list
    into a test file. This list of files and the contents of each individual file is saved to a zip file by another class
    and then encoded and uploaded to the CDB.
    """
    def __init__(self, filepath, notes, testserver = None):
        """
        @Method Class constructor This method sets up the necessaries to upload to the database

        This method sets up some class variables and then calls other methods which set up
        the connection to the database and also organise the GDMLs into an order ready to
        upload.

        @Param filepath path of the directory which contains the GDML files
        @Param notes must be a string which briefly describes the geometry
        @Param testserver enter 1 to set up the test server leave blank for real server
        """
        self.wsdlurl = None
        self.geometry = None
        self.textfile = None
        self.text = ""
        if type(testserver) != int:
            raise IOError('Test Server argument must be 1 or 0 or None', 'GDMLtocdb::__init__')
        else: 
            self.testserver = testserver
        if type(notes) != str:
            raise IOError('Geom desciprtion missing!', 'GDMLtocdb::__init__')
        else:
            self.notes = notes
        filelist = []
        self.geometryfiles = filelist
        self.listofgeometries = None
        if os.path.exists(filepath) != True:
            raise IOError("File path does not exist", "gdmltocdb::__init__")
        else:
            self.filepath = filepath
        self.set_up_server()
        self.check_file_list()
        self.create_file_list()
        
    def set_up_server(self):
        """
        @method set_up_server This method sets up the connection to the CDB
        
        This method sets up a connection to either the supermouse server or
        the test server depending on whether this is specified by __init__.
        """
        if self.testserver == None:
            self.wsdlurl = "supermouse server" # actual wsdl
        elif self.testserver == 1:
            twsdl = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl"
            self.wsdlurl = twsdl 
        else: 
            raise StandardError("Incorrect input", "gdmltocdb::__init__")
        self.geometry = Client(self.wsdlurl).service
        print "Configuration Database Status is"
        print self.geometry.getStatus()
    
    def check_file_list(self):
        """
        @method check_file_list This method searches the designated path for a text file and checks if it contains the list of geometries.
        
        This method goes through the designated path given by __init__ and 
        checks to see if there is a text file. It then checks this file to
        if it contains a list of the geometries also in the designated path.
        It raises an error if they don't match. If there isn't a text file
        it stores the files within the path in a list ready for the text
        file to be created.
        """
        gdmls = os.listdir(self.filepath)
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
                    self.listofgeometries = path
                    fin = open(self.listofgeometries, 'r')
                    for line in fin.readlines():
                        self.geometryfiles.append(line.strip())
                    fin.close()
    
    def create_file_list(self):
        """
        @method create_file_list This method creates a text file which contains a list of geometries.
        
        This method will create a text file which contains a list
        of the GDMLs in the path given by __init__. This is in 
        preparation for uploading. If there is already a text file
        it does nothing. 
        """
        # if there is no text file create one and fill it with the geometries.
        gdmls = os.listdir(self.filepath)
        numoffiles = len(gdmls)
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
        

    def upload_to_cdb(self, zipped_file):
        """
        @Method upload_to_cdb, this method uploads the geometries to the CDB

        This method write the contents of all the gdmls and the file which lists the gdmls
        into one string. This string is then encoded and uploaded to the CDB with a date stamp of
        when the method is called.
        """
        _dt = datetime.today()
        fin = open(zipped_file, 'r')
        f_contents = fin.read()
        _gdml = b64encode(f_contents)
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
        self.id_nums = []
        self.filestr = ""
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
        
        @param  downloadedpath The path location where the files will be unpacked to. 
        """
        downloadedfile = b64decode(self.geometry.getCurrentGDML())
        zip_path = downloadpath + '/Geometry.zip'
        fout = open(zip_path, 'w')
        fout.write(downloadedfile)
        fout.close()

    def download_geometry_for_id(self, id_num, downloadpath):
        """
        @Method download geometry for ID 

        This method gets the geometry, for the given ID, from the database then passes the 
        string to the unpack method which unpacks it.
        
        @param  id The integer ID number for the desired geometry.
        @param  downloadedpath The path location where the files will be unpacked to. 
        """
        downloadedfile = b64decode(self.geometry.getGDMLForId(id_num))
        zip_path = downloadpath + '/Geometry.zip'
        fout = open(zip_path, 'w')
        fout.write(downloadedfile)
        fout.close()
        
    def get_ids(self, start_time, stop_time = None):
        """
        @method get IDs
        
        This method queries the database for the list of geometries and prints
        to the screen their ID numbers and their descriptions.
        
        @param start_time The datetime of which you wish the query to start must be in UTC.
        @param stop_time The datetime of which you wish the query to stop must be in UTC. Can be blank.
        """
        id_list = self.geometry.getIds(start_time, stop_time)
        ids = id_list.split(' ')
        length = len(ids)
        temp = ""
        for num in range(0, length):
            temp = str(ids[num])
            if temp.find('name=') >= 0:
                self.id_nums.append(temp)
            temp = ""
        last_id = len(self.id_nums)
        valid_id = str(self.id_nums[last_id-1])
        valid_id = valid_id.strip('name=')
        valid_id_list = eval(valid_id)
        valid_id = int(valid_id_list)
        return valid_id
        
    def download_geometry_for_run(self, run_id, downloadpath):
        """
        @Method download geometry for run 

        This method gets the geometry, for the given run number, from the database then passes the 
        string to the unpack method which unpacks it.
        
        @param  id The long ID run number for the desired geometry.
        @param  downloadedpath The path location where the files will be unpacked to. 
        """        
        downloadedfile = b64decode(self.geometry.getGDMLForRun(run_id))
        zip_path = downloadpath + '/Geometry.zip'
        fout = open(zip_path, 'w')
        fout.write(downloadedfile)
        fout.close()
        
            
def main():
    """
    Main function
    """
if __name__ == "__main__":
    main()
