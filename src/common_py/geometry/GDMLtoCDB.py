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
import datetime

from geometry.ConfigReader import Configreader
from xml.etree import ElementTree

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

    def select_files(self, root, files, suffix=[]): 
        # pylint: disable = R0201, W0102, W0612
        """
        @method select_files Filter out interesting files within
        the directory "root". 
        The files of interest are given by suffix. If blank then all
        files will be assumed to be of interest.
        """
        selected_files = []

        for fname in files:
            # do concatenation her to get full path
            full_path = os.path.join(root, fname)
            
            if len(suffix)==0:
                selected_files.append(full_path)
            elif is_filetype(fname, suffix):
                selected_files.append(full_path)
        return selected_files
        
    def create_file_list(self):
        # pylint: disable = R0201, W0102, W0612
        """
        @method create_file_list Creates a text file containing a list of
                                 geometry files.
        
        Create a text file with name FILELIST containing a list of the files in
        filepath. Only files with extension *.gdml or *.xml are added. 
        """
        # if there is no text file create one and fill it with the geometries.
        files_on_disk = []
        for root, dirs, files in os.walk(self.filepath):
            if root == self.filepath:
                files_on_disk += self.select_files(root, files, ['xml', 'gdml'])
            else:
                files_on_disk += self.select_files(root, files)

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
        self.ccurl = ""
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
        self.ccurl = config.cdb_cc_download_url
        server_status = self.geometry_cdb.get_status()
        if not server_status in SERVER_OK:
            print 'Warning, server status is '+server_status 
        return self.wsdlurl
            
    def download_current(self, downloadpath, geoid=0):
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
            now = datetime.datetime.now()
            nowandone = datetime.datetime.now() + datetime.timedelta(1)
            ids = self.geometry_cdb.get_ids(now, nowandone)
            if len(ids) == 0:
                raise OSError('Geometry ID does not exist for '+str(now))
            else:
                # the latest applicable key uploaded should be the
                # first key appearing
                geoid = ids.keys()[0]
                # in case this is not the case run through the other
                # values to see if there is a later applicable
                # creation date
                sortedids = sorted(ids.items(), \
                                       key=lambda x:x[1]['created'], reverse=True)
                geoid = sortedids[0][0]
                # return geoid

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

    def download_geometry_by_run(self, run_num, download_path, geoid=0):
        """
        @Method download geometry for run 

        This method gets the geometry, for the given run, from the database then 
        passes the string to the unpack method which unpacks it.
        
        @param  run The integer run number for the desired geometry.
        @param  downloadedpath The path location where the files will be 
                               unpacked to. 
        """
        if not os.path.exists(download_path):
            raise OSError('Path '+download_path+' does not exist')
        downloaded_file = self.geometry_cdb.get_gdml_for_run(long(run_num))
        self.download_beamline_for_run(run_num, download_path, geoid)
        
        self.download_coolingchannel_for_run(run_num, download_path)
        # self.download_corrections_for_run(run_num, download_path):
        self.__write_zip_file(download_path, downloaded_file)
        
        #return the associated geometry id
        # return geoid
        

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
    
    def download_beamline_for_run(self, run_id, downloadpath, geoid=0): #pylint: disable = R0201, C0301
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
            # also want to get the geometry ID for record keeping
            downloadedmap = beamline_cdb.get_beamline_for_run(int(run_id))
            if len(downloadedmap) == 0:
                raise OSError('Beamline not found for run')
            else:
                start_time = downloadedmap[int(run_id)]['start_time']
                stop_time = downloadedmap[int(run_id)]['end_time']
                ids = self.geometry_cdb.get_ids(start_time, stop_time)
                if len(ids) == 0:
                    raise OSError('Geometry ID does not exist for run number')
                else:
                    # the latest applicable key uploaded should be the
                    # first key appearing
                    geoid = ids.keys()[0]
                    # in case this is not the case run through the other
                    # values to see if there is a later applicable
                    # creation date
                    sortedids = sorted(ids.items(), \
                                  key=lambda x:x[1]['created'], reverse=True)
                    geoid = sortedids[0][0]
                # return geoid
  
    def download_coolingchannel_for_run(self, run_id, downloadpath): 
        #pylint: disable = R0201, C0301
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
            try:
                downloaded = \
                           coolingchannel_cdb.get_coolingchannel_for_run(run_id)
                
                path = downloadpath + '/CoolingChannelInfo.gdml'
                
                downloadedfile = \
                     self.generate_coolingchannel_xml_from_string(downloaded)
                fout = open(path, 'w')
                fout.write(str(downloadedfile))
                fout.close()
                
            except RuntimeError:
                exit(1)
                #fout = open(path, 'w')
                #fout.write(str(downloaded))
                #fout.close()

    
    #def download_corrections_for_run(self, run_id, downloadpath):
        #pylint: disable = R0201, C0301
    #    """
    #    @Method download corrections for run 
    #    
    #    This method gets the detector alignment corrections, for the given run number, from the 
    #    database.
    #    
    #    @param  id The long ID run number for the desired geometry.
    #    @param  downloadedpath The path location where the files will be 
    #    unpacked to.
    #    """
    #    if os.path.exists(downloadpath) == False:
    #        raise OSError('Path '+downloadpath+' does not exist')
    #    else:
    #        correction_cdb = cdb.Corrections()
    #        try:
    #            downloaded = correction_cdb.get_corrections_for_run_xml(run_id)
    #            path = downloadpath + "/AlignmentCorrections.gdml"
    #            fout = open(path)
    #            fout.write(str(downloaded))
    #            fout.close()
    #            
    #        except RuntimeError:
    #            exit(1)
    

    def download_beamline_for_tag(self, tag, downloadpath):  #pylint: disable = R0201, C0301
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
            tag_list = beamline_cdb.list_tags()
            tag_found = [i for i, j in enumerate(tag_list) if j == tag]
            if len(tag_found) > 0:
                try:
                    downloaded = beamline_cdb.get_beamline_for_tag(tag)
                    path = downloadpath + '/Beamline.gdml'
                    downloadedfile = self.generate_beamline_xml_from_string(\
                    downloaded[tag])
                    fout = open(path, 'w')
                    fout.write(downloadedfile)
                    fout.close()
                except RuntimeError:
                    exit(1)
            else:
                print "Beamline tag does not exist. Ignoring Beamline tag."
                
    def download_coolingchannel_for_tag(self, tag, downloadpath): 
        #pylint: disable = R0201, C0301
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
            coolingchannel_cdb = cdb.CoolingChannel(self.ccurl)
            tag_list = coolingchannel_cdb.list_tags()
            tag_found = [i for i, j in enumerate(tag_list) if j == tag]
            if len(tag_found) > 0:
                try:
                    downloaded = \
                               coolingchannel_cdb.get_coolingchannel_for_tag(tag)
                    path = downloadpath + '/CoolingChannelInfo.gdml'
                    downloadedfile = \
                                   self.generate_coolingchannel_xml_from_string(downloaded)
                    fout = open(path, 'w')
                    fout.write(str(downloadedfile))
                    fout.close()
                except RuntimeError:
                    exit(1)
            else:
                print "CoolingChannel tag does not exist. Ignoring CoolingChannel tag." 
                

    def convert_dict_to_xml(self, xmldict):
        """
        @Method convert a python dictionary to an xml string
        """
        roottag = xmldict.keys()[0]
        root = ElementTree.Element(roottag)
        self.recursive_convert_dict_to_xml(root, xmldict[roottag])
        return root

    def recursive_convert_dict_to_xml(self, parent, dictitem):
        """
        @Method recursive conversion of a dictionary item to xml
        """
        assert type(dictitem) is not type([])
        
        if isinstance(dictitem, dict):
            for (tag, child) in dictitem.iteritems():
                if str(tag) == '_text':
                    parent.text = str(child)
                elif type(child) is type([]):
                    # iterate through the array and convert
                    for listchild in child:
                        elem = ElementTree.Element(tag)
                        parent.append(elem)
                        self.recursive_convert_dict_to_xml(elem, listchild)
                else:
                    elem = ElementTree.Element(tag)
                    parent.append(elem)
                    self.recursive_convert_dict_to_xml(elem, child)
        else:
            parent.text = str(dictitem)

    def generate_beamline_xml_from_string(self, xmldict): 
        #pylint: disable = R0201, C0301, C0103
        """
        @Method converts xmldict to an xml format in pieces following methods used in CDB

        This is required in absence of xml output from the CDB
        """
        xml = '<runs>'
        beam_xml  = self.run_settings_for_beamline(xmldict)
        xml = (xml + beam_xml + '</runs>')
        
        return xml
        
    def run_settings_for_beamline(self, run_data): #pylint: disable = R0201
        """
        @Method converts useful information from the dictionary to xml suitable for formatting.

        Borrowed from CDB methods.
        """
        
        run_xml = ''
        try:
            if str(run_data['beam_stop']).upper() == 'OPEN':
                beam_stop = 'true'
            elif str(run_data['beam_stop']).upper() == 'CLOSED':
                beam_stop = 'false'
            else:
                raise OSError(
                    "Value for beam_stop must be 'OPEN' or 'CLOSED' not "
                    + str(run_data['beam_stop']))
            try:
                run_xml = \
                        ("<run runNumber='" + str(run_data['run_number'])
                         + "' beamStop='" + beam_stop
                         + "' diffuserThickness='"
                         + str(run_data['diffuser_thickness'])
                         + "' protonAbsorberThickness='"
                         + str(run_data['proton_absorber_thickness'])
                         + "' ")
            except KeyError, exception:
                run_xml = ("<run runNumber='1'"
                           + " beamStop='" + beam_stop
                           + "' diffuserThickness='"
                           + str(run_data['diffuser_thickness'])
                           + "' protonAbsorberThickness='"
                           + str(run_data['proton_absorber_thickness'])
                           + "' ")
            try:
                run_xml = (run_xml + "overwrite='"
                           + str(int(run_data['overwrite'])) + "' >")
            except KeyError, exception:
                run_xml = (run_xml + "overwrite='" + str(False)
                           + "' >")
            try:
                magnets = run_data['magnets']
                
                if magnets != None:
                    for magnet in magnets:
                        if type(magnet) == type("string"):
                            # then only the set current is given.
                            # the polarity will need to be guessed at.
                            run_xml = (run_xml + "<magnet name='" + str(magnet)
                                   + "' setCurrent='" + str(magnets[magnet])
                                   + "' polarity='+1'/>")
                        else:
                            if (str(magnet['polarity']) != "-1"
                                and str(magnet['polarity']) != "0"
                                and str(magnet['polarity']) != "1"
                                and str(magnet['polarity']) != "+1"):
                                raise OSError("Polarity for "
                                              + magnet['name'] + " is "
                                              + str(magnet['polarity'])
                                              + ", it must be -1, 0 or +1")
                            run_xml = (run_xml +
                                       "<magnet name='" + str(magnet['name']) +
                                       "' setCurrent='" + str(magnet['current'])
                                      + "' polarity='" + str(magnet['polarity'])
                                       + "'/>")
            
            except KeyError, exception:
                print "No magnet values defined in tag."
            run_xml = (run_xml + "</run>")
        except KeyError, exception:
            print "Missing value for " + str(exception)
        
        return run_xml

    def generate_coolingchannel_xml_from_string(self, data):
        #pylint: disable = R0201, C0301, C0103
        """
        @Method convert string to formatable xml format

        borrowed from cdb._cooling_channel_supermouse.set_coolingchannel(data)
        """
        try:
            xml = ("<coolingchannels><coolingchannel>")
            
            for magnet in data:
                if (str(magnet['polarity']) != "-1"
                    and str(magnet['polarity']) != "0"
                    and str(magnet['polarity']) != "1"
                    and str(magnet['polarity']) != "+1"):
                    raise OSError("Polarity for " + magnet['name']
                                            + " is " + str(magnet['polarity'])
                                            + ", it must be -1, 0 or +1")
                xml = xml + "<magnet name='" + str(magnet['name']) + "' "
                xml = xml + "mode='" + str(magnet['mode']) + "' "
                xml = xml + "polarity='" + str(magnet['polarity']) + "'>"
                
                coils = magnet['coils']
                for coil in coils:
                    xml = xml + "<coil name='" + str(coil['name']) + "' "
                    xml = (xml + "calibration='"
                           + str(coil['calibration']) + "' ")
                    xml = xml + "ilim='" + str(coil['ilim']) + "' "
                    xml = xml + "iset='" + str(coil['iset']) + "' "
                    xml = xml + "rate='" + str(coil['rate']) + "' "
                    xml = (xml + "stability='" + str(coil['stability'])
                           + "' ")
                    xml = xml + "vlim='" + str(coil['vlim']) + "'/>"
                    
                xml = xml + "</magnet>"
            xml = xml + "</coolingchannel></coolingchannels>"
        except KeyError, exception:
            raise OSError("Missing value for " + str(exception))
        return xml
