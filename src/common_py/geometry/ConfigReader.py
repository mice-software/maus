"""
m. Littlefield
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

from Configuration import Configuration
import json

class Configreader(): #pylint: disable = R0903, R0902
    """
    @class configreader, Configuration File Reader Class
    
    This class reads the information in the Configuration Text File and 
    translates this into variables which can be passed between the other 
    classes.
    """
    def __init__(self):
        """
        @method Class Constructor
        
        This sets up the blank variables ready to be filled by the information
        contained in the Configuration text file.
        """
        self.cdb_dl_url = ""
        self.cdb_dl_dir = ""
        self.cdb_ul_url = ""
        self.cdb_ul_dir = ""
        self.gdmldir = ""
        self.geometrynotes = ""
        self.zipfile = None
        self.deleteoriginals = None
        self.downloaddir = ""
        self.starttime = ""
        self.stoptime = ""
        self.runnum = ""

        self.readconfig()
        
    def readconfig(self):
        """
        @method readconfig
        
        This method reads the ConfigurationDefaults File and takes the 
        information needed from the file which has certain tags.                  
        """
        inputs = Configuration().getConfigJSON(command_line_args = True)
        config_dict          = json.loads(inputs)
        self.cdb_dl_url          = config_dict['cdb_download_url']
        self.cdb_dl_dir          = config_dict['cdb_download_geometry_dir']
        self.cdb_ul_url          = config_dict['cdb_upload_url']
        self.cdb_ul_dir          = config_dict['cdb_upload_geometry_dir']
        self.maus_ul_dir         = config_dict['geometry_upload_directory']
        self.maus_dl_dir         = config_dict['geometry_download_directory']
        self.geometrynotes   = config_dict['geometry_description']
        self.zipfile         = config_dict['geometry_zip_file']
        self.deleteoriginals = config_dict['geometry_delete_originals']
        self.starttime       = config_dict['geometry_start_time']
        self.stoptime        = config_dict['geometry_stop_time']
        self.runnum          = config_dict['geometry_run_number']

def main():
    """
    Main Function
    """
    cfile = Configreader()
    cfile.readconfig()

if __name__ == "__main__":
    main()
