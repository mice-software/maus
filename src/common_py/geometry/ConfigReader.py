"""
Configuration Text File Reader Class
M. Littlefield 02/08/11 
"""
from Configuration import Configuration
import json

#pylint: disable = R0903

class Configreader():
    """
    @class configreader, Configuration File Reader Class
    
    This class reads the information in the Configuration Text File and translates
    this into variables which can be passed between the other classes. It does 
    this using argparse.
    """
    def __init__(self):
        """
        @method Class Constructor
        
        This sets up the blank variables ready to be filled by the information
        contained in the Configuration text file.
        """
        self.gdmldir = ""
        self.geometrynotes = ""
        self.zipfile = None
        self.deleteoriginals = None
        self.downloaddir = ""
        self.starttime = ""
        self.stoptime = ""
        self.runnum = ""
        
    def readconfig(self):
        #New test for run num 
        """
        @method readconfig
        
        This method reads the ConfigurationDefaults File and takes the information needed from the
        file which has certain tags. This information is then stored into variables which can be
        passed between classes. The tags are,
        
        GeometryDirectory   = "This is where the directory to the fastrad outputted geometry goes"
        GeometryDescription = "This is where the description of the geometry goes"
        ZipFile          = "Choose 1 to create a zipfile of the geometry. 0 to do nothing"
        Delete Originals = "Choose 1 to delete the original geometry files. 0 to do nothing"
        DownloadDir = "This is where the directory which the user wishes to download geometries
                       from the CDB to goes"                      
        """
        inputs = Configuration().getConfigJSON()
        config_dict          = json.loads(inputs)
        self.gdmldir         = config_dict['GeometryDirectory']
        self.geometrynotes   = config_dict['GeometryDescription']
        self.zipfile         = config_dict['Zip_File']
        self.deleteoriginals = config_dict['Delete_Originals']
        self.downloaddir     = config_dict['DownloadDir']
        self.starttime       = config_dict['StartTime']
        self.stoptime        = config_dict['StopTime']
        self.runnum          = config_dict['RunNum']

def main():
    """
    Main Function
    """
    cfile = Configreader()
    cfile.readconfig()

if __name__ == "__main__":
    main()
