"""
Configuration Text File Reader Class
M. Littlefield 02/08/11 
"""
import argparse

class Configreader():
    #pylint: disable = W0101, R0903
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

    def readconfig(self):
        """
        @method readconfig
        
        This method reads the Configuration Text File and takes the information needed from the
        file which has certain tags. This information is then stored into variables which can be
        passed between classes. The tags are,
        
        GeometryDirectory   = "This is where the directory to the fastrad outputted geometry goes"
        GeometryDescription = "This is where the description of the geometry goes"
        ZipFile          = "Choose 1 to create a zipfile of the geometry. 0 to do nothing"
        Delete Originals = "Choose 1 to delete the original geometry files. 0 to do nothing"
        DownloadDir = "This is where the directory which the user wishes to download geometries
                       from the CDB to goes"
                       
        These tags must be in the Configuration Text File otherwise this class will not read them.  
        """
        explanation = 'The file name/path of the configuration File'
        parser = argparse.ArgumentParser(description= explanation)
        helpexplanation = 'The file name/path of the configuration File'
        parser.add_argument('Config File', help = helpexplanation)
        config_file = parser.parse_args()
        config_file = config_file.__dict__
        fin = open(config_file['Config File'], 'r')
        for lines in fin.readlines():
            if lines.find('GeometryDirectory') >= 0:
                varstart = lines.find('=') + 1
                self.gdmldir = lines[varstart:]
                self.gdmldir = self.gdmldir.replace('"', '')
                self.gdmldir = self.gdmldir.strip()
            if lines.find('GeometryDescription') >= 0:
                varstart = lines.find('=') + 1
                self.geometrynotes = lines[varstart:]
                self.geometrynotes = self.geometrynotes.replace('"', '')
                self.geometrynotes = self.geometrynotes.strip()
            if lines.find('ZipFile') >= 0:
                zipfile = lines
                if zipfile.find('1') >= 0:
                    self.zipfile = True
                else:
                    self.zipfile = False
            if lines.find('Delete Originals') >= 0:
                deleteoriginals = lines
                if deleteoriginals.find('1') >= 0:
                    self.deleteoriginals = True
                else:
                    self.deleteoriginals = False
            if lines.find('DownloadDir') >= 0:
                varstart = lines.find('=') + 1
                self.downloaddir = lines[varstart:]
                self.downloaddir = self.downloaddir.replace('"', '')
                self.downloaddir = self.downloaddir.strip()
            if lines.find('StartTime') >= 0:
                varstart = lines.find('=') + 1
                self.starttime = lines[varstart:]
                self.starttime = self.starttime.replace('"', '')
                self.starttime = self.starttime.strip()
            if lines.find('StopTime') >= 0:
                varstart = lines.find('=') + 1
                self.stoptime = lines[varstart:]
                self.stoptime = self.stoptime.replace('"', '')
                self.stoptime = self.stoptime.strip()
        return self.gdmldir
        return self.geometrynotes
        return self.zipfile
        return self.deleteoriginals
        return self.downloaddir
        return self.starttime
        return self.stoptime

def main():
    """
    Main Function
    """
    cfile = Configreader()
    cfile.readconfig()

if __name__ == "__main__":
    main()
