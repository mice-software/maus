"""
GDMLtoCDB contains the classes that handle uploading and downloading to the cdb

GDMLtoCDB contains two classes:
- Uploader handles uploading to the configuration database
- Downloader handles downloading from the configuration database
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

import os
import cdb

from geometry.ConfigReader import Configreader

GEOMETRY_ZIPFILE = 'geometry.zip'
FILELIST = 'FileList.txt'
SERVER_OK = ['okay', 'OK']

def is_filetype(file_name, extensions):
    """
    return true if the file extension in extensions is one of the items in the
    iterable extensions. File extensions are specified like ['zip', 'xml'] etc 
    """
    for suffix in extensions:
        slen = file_name.rfind('.')
        if slen != -1 and file_name[slen+1:] == suffix:
            return True
    return False

#change notes no longer encode decode
class Uploader: #pylint: disable = R0902
    """
    @Class Uploader handles the uploading of geometries to the CDB

    This class writes the geometries to the database. It requires an input of 
    the directory path which contains the GDML files produce by fastRad. It 
    will search this path and find all the files within it and will write these 
    as a list into a test file. This list of files and the contents of each 
    individual file is saved to a zip file by another class
    and then encoded and uploaded to the CDB.
    """
    def __init__(self, filepath, notes, textfile = None):
        """
        @Method Class constructor This method sets up the necessaries to upload
                                  to the database

        This method sets up some class variables and then calls other methods 
        which set up the connection to the database and also organise the GDMLs 
        into an order ready to upload.

        @Param filepath path of the directory which contains the GDML files
        @Param notes must be a string which briefly describes the geometry
        @Param textfile path to a text file containing list of geometries to be
                        uploaded; default is None, in which case geometries are
                        generated automatically
        """
        self.config = Configreader()
        self.wsdlurl = self.config.cdb_upload_url+\
                                                self.config.geometry_upload_wsdl
        print '"'+self.wsdlurl+'"'
        self.geometry_cdb = cdb.GeometrySuperMouse(self.wsdlurl)
        self.textfile = textfile
        self.text = ""
        if type(notes) != str:
            raise IOError('Geometry description missing or not a string')
        else:
            self.notes = notes
        filelist = []
        self.geometryfiles = filelist
        if os.path.exists(filepath) != True:
            raise IOError("File path "+str(filepath)+" does not exist")
        else:
            self.filepath = filepath
        self.set_up_server()
        if self.textfile == None:
            self.create_file_list()
        self.check_file_list()
        
    def set_up_server(self):
        """
        @method set_up_server This method sets up the connection to the CDB
        
        This method sets up a connection to either the supermouse server or
        the test server depending on whether this is specified by __init__.
        """
        #self.wsdlurl = self.config.cdb_upload_url+\
        #                    self.config.geometry_upload_wsdl
        #self.geometry_cdb.set_url(self.wsdlurl)
        server_status = self.geometry_cdb.get_status()
        if not server_status in SERVER_OK:
            print 'Warning, server status is '+server_status
        return self.wsdlurl
    
    def create_file_list(self):
        """
        @method create_file_list Creates a text file containing a list of
                                 geometry files.
        
        Create a text file with name FILELIST containing a list of the files in
        filepath. Only files with extension *.gdml or *.xml are added. 
        """
        # if there is no text file create one and fill it with the geometries.
        files_on_disk = []
        for fname in sorted(os.listdir(self.filepath)):
            if is_filetype(fname, ['xml', 'gdml']):
                files_on_disk.append(os.path.join(self.filepath, fname))

        filelist_path = os.path.join(self.filepath, FILELIST)
        fout = open(filelist_path, 'w')
        for fname in files_on_disk:
            fout.write(fname+'\n')
        fout.close()

    def check_file_list(self):
        """
        @method check_file_list 
        
        Returns true if everything in FILELIST can be found in filepath. Else
        returns false
        """
        files_on_disk = sorted(os.listdir(self.filepath))
        filelist_path = os.path.join(self.filepath, FILELIST)
        fin = open(filelist_path)
        files_in_filelist = [str.rstrip(x) for x in sorted(fin.readlines())]
        fin.close()
        for fname in files_in_filelist:
            if fname not in files_on_disk:
                return False
        return True

    def upload_to_cdb(self, zipped_file):
        """
        @Method upload_to_cdb, this method uploads the geometries to the CDB

        This method write the contents of all the gdmls and the file which lists
        the gdmls into one string. This string is then encoded and uploaded to 
        the CDB with a date stamp of when the method is called.
        """
        if zipped_file[-4:] != '.zip':
            raise IOError('Argument is not a zip file')
        else:
            _dt = self.config.geometry_upload_valid_from
            fin = open(zipped_file, 'r')
            _gdml = fin.read()
            #self.geometry_cdb = cdb.GeometrySuperMouse()
            self.geometry_cdb.set_gdml(_gdml, _dt, self.notes)
            
class Downloader: #pylint: disable = R0902
    """
    @Class Downloader, this class downloads geometries from the CDB

    This class downloads the information from the CDB and decodes and unpacks 
    the information.
    """
    def __init__(self):
        """
        @Method Class constructor

        This method sets up the connection to the CDB ready to download.

        @Param testserver, If an argument of 1 is entered this will set a 
        connection to the test CDB if left blank write to the actual CDB
        """
        self.times = []
        self.id_nums = []
        self.filestr = ""
        filelist = []
        self.geometryfiles = filelist
        self.listofgeometries = filelist
        self.wsdlurl = ""
        self.geometry_cdb = None
        self.set_up_server()

    def set_up_server(self):
        """
        @method set_up_server This method sets up the connection to the CDB
        
        This method sets up a connection to either the supermouse server or
        the test server depending on whether this is specified by __init__.
        """
        config = Configreader()
        self.wsdlurl = config.cdb_download_url+config.geometry_download_wsdl
        self.geometry_cdb = cdb.Geometry()
        self.geometry_cdb.set_url(self.wsdlurl)
        server_status = self.geometry_cdb.get_status()
        if not server_status in SERVER_OK:
            print 'Warning, server status is '+server_status 
        return self.wsdlurl
            
    def download_current(self, downloadpath):
        """
        @Method download_current, this method downloads the current valid 
                                  geometry and writes the files

        this method decodes the uploaded geometry and acquires from the 
        string this list of files contained within the upload. It then opens 
        files in the ~/maus/src/common_py/geometries/Download which correspond
        to the related gdml files and write the contents to these files.
        
        @param  downloadedpath The path location where the files will be 
                               unpacked to. 
        """
        if os.path.exists(downloadpath) == False:
            raise OSError('Path '+str(downloadpath)+' does not exist')
        else:
            downloadedfile = self.geometry_cdb.get_current_gdml()
            self.__write_zip_file(downloadpath, downloadedfile)

    def download_geometry_by_id(self, id_num, download_path):
        """
        @Method download geometry for ID 

        This method gets the geometry, for the given ID, from the database then 
        passes the string to the unpack method which unpacks it.
        
        @param  id The integer ID number for the desired geometry.
        @param  downloadedpath The path location where the files will be 
                               unpacked to. 
        """
        if not os.path.exists(download_path):
            raise OSError('Path '+download_path+' does not exist')
        else:
            downloaded_file = self.geometry_cdb.get_gdml_for_id(id_num)
            self.__write_zip_file(download_path, downloaded_file)

    def download_geometry_by_run(self, run_num, download_path):
        """
        @Method download geometry for ID 

        This method gets the geometry, for the given ID, from the database then 
        passes the string to the unpack method which unpacks it.
        
        @param  id The integer ID number for the desired geometry.
        @param  downloadedpath The path location where the files will be 
                               unpacked to. 
        """
        if not os.path.exists(download_path):
            raise OSError('Path '+download_path+' does not exist')
        downloaded_file = self.geometry_cdb.get_gdml_for_run(long(run_num))
        self.download_beamline_for_run(run_num, download_path)
        self.__write_zip_file(download_path, downloaded_file)

    def __write_zip_file(self, path_to_file, output_string): #pylint: disable = R0201, C0301
        """
        Write string to file path_to_file+GEOMETRY_ZIPFILE in zip format
        """
        zip_file = os.path.join(path_to_file, GEOMETRY_ZIPFILE)
        fout = open(zip_file, 'w')
        fout.write(output_string)
        fout.close()
            
    def get_ids(self, start_time, stop_time = None):
        """
        @method get IDs
        
        This method queries the database for the list of geometries and prints
        to the screen their ID numbers and their descriptions.
        
        @param start_time The datetime of which you wish the query to start must
                          be in UTC.
        @param stop_time The datetime of which you wish the query to stop must
                         be in UTC. Can be blank.
        """
        id_dict = self.geometry_cdb.get_ids(start_time, stop_time)
        id_number = sorted(id_dict.keys())[-1]
        print "Using geometry ID " + str(id_number) + \
                           " valid from " + str(id_dict[id_number]['validFrom'])
        return str(id_number)
    
    def download_beamline_for_run(self, run_id, downloadpath): #pylint: disable = R0201, C0301
        """
        @Method download geometry for run 

        This method gets the geometry, for the given run number, from the 
        database then passes the string to the unpack method which unpacks it.
        
        @param  id The long ID run number for the desired geometry.
        @param  downloadedpath The path location where the files will be 
                               unpacked to.
        """
        if os.path.exists(downloadpath) == False:
            raise OSError('Path '+downloadpath+' does not exist')
        else:        
            beamline_cdb = cdb.Beamline()
            downloadedfile = beamline_cdb.get_beamline_for_run_xml(run_id)
            path = downloadpath + '/Beamline.gdml'
            fout = open(path, 'w')
            fout.write(downloadedfile)
            fout.close()

    
    def download_coolingchannel_for_run(self, run_id, downloadpath): #pylint: disable = R0201, C0301
        """
        @Method download geometry for run 

        This method gets the geometry, for the given run number, from the 
        database then passes the string to the unpack method which unpacks it.
        
        @param  id The long ID run number for the desired geometry.
        @param  downloadedpath The path location where the files will be 
                               unpacked to.
        """
        if os.path.exists(downloadpath) == False:
            raise OSError('Path '+downloadpath+' does not exist')
        else:        
            coolingchannel_cdb = cdb.CoolingChannel()
            downloadedfile = coolingchannel_cdb.get_coolingchannel_for_run_xml(run_id)
            path = downloadpath + '/CoolingChannel.gdml'
            fout = open(path, 'w')
            fout.write(downloadedfile)
            fout.close()

