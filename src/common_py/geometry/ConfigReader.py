import os.path
import os
import sys
lib_path = os.path.abspath('../../../argparse-1.2.1')
sys.path.append(lib_path)
import argparse

def main():
    from ConfigReader import configreader
    file = configreader()
    file.readconfig()

if __name__ == "__main__":
    main()

class configreader():

    def __init__(self):
        self.GDMLDir = ""
        self.GeometryNotes = ""
        self.ZipFile = None
        self.DeleteOriginals = None



    def readconfig(self):
        parser = argparse.ArgumentParser(description='The file name/path of the configuration File')
        parser.add_argument('Config File', help = 'The file name/path of the configuration File')
        #set arguments into dict so we can use them
        config_file = parser.parse_args()
        config_file = config_file.__dict__
        fin = open(config_file['Config File'], 'r')
        for lines in fin.readlines():
            if lines.find('GeometryDirectory') >= 0:
                varStart = lines.find('=') + 1
                self.GDMLDir = lines[varStart:]
                self.GDMLDir = self.GDMLDir.replace('"', '')
                self.GDMLDir = self.GDMLDir.strip()
            if lines.find('GeometryDescription') >= 0:
                varStart = lines.find('=') + 1
                self.GeometryNotes = lines[varStart:]
                self.GeometryNotes = self.GeometryNotes.replace('"', '')
                self.GeometryNotes = self.GeometryNotes.strip()
            if lines.find('ZipFile') >= 0:
                zipfile = lines
                if zipfile.find('1') >= 0:
                    self.ZipFile = True
                else:
                    self.ZipFile = False
            if lines.find('Delete Originals') >= 0:
                deleteoriginals = lines
                if deleteoriginals.find('1') >= 0:
                    self.DeleteOriginals = True
                else:
                    self.DeleteOriginals = False
        return self.GDMLDir
        return self.GeometryNotes
        return self.ZipFile
        return self.DeleteOriginals