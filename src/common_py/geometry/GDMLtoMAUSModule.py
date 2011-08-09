import os
from CADImport import CADImport

class GDMLtomaus():
        
        def __init__(self, path):
            self.ConfigurationFile = None
            self.MaterialFile = None
            self.MaterialFilePath = None
            list = []
            self.StepFiles = list
            if type(path) != str:
                raise IOError('path must be a string to the directory which holds the GDMLs', 'GDMLFormatter::__init__')
            else:
                self.Path = path
            gdmls = os.listdir(self.Path)
            for fname in gdmls:
                if fname.find('materials') >= 0:
                    file = self.Path + '/' + fname
                    self.MaterialFile = file
                    self.MaterialFilePath = os.path.abspath(self.MaterialFile)
                if fname.find('fastrad') >= 0 and fname != self.MaterialFile:
                    file = self.Path + '/' + fname
                    self.ConfigurationFile = file
                if fname.find('materials') < 0 and fname.find('fastrad') < 0 :
                    stepfile = self.path + '/' + fname
                    self.stepfiles.append(stepfile)

        def convert_to_maus(self, output):
            outputfile = output + "/Configuration.txt"
            configurationxsl = os.environ["MAUS_ROOT_DIR"] + "/src/common_py/geometry/xsltScripts/GDML2G4MICE.xsl"
            ConfigFile = CADImport(xmlin1 = self.ConfigurationFile, xsl = configurationxsl, output = outputfile)
            ConfigFile.XSLTParse()
            length = len(self.StepFiles)
            for num in range(0, length):
                file = str(self.StepFiles[num])
                file = file.strip(self.Path)
                outputfile = output + '/' + file + 'txt'
                MMxsl = os.environ["MAUS_ROOT_DIR"] + "/src/common_py/geometry/xsltScripts/MMTranslation.xsl"
                StepFile = CADImport(xmlin1 = self.StepFiles[num], xsl = '/home/matt/maus-littlefield/src/common_py/geometry/xsltScripts/MMTranslation.xsl', output = outputfile)
                StepFile.XSLTParse()
                StepFile = None
                os.remove(self.StepFiles[num])
            os.remove(self.ConfigurationFile)
            
def main():
    file = GDMLtomaus('/home/matt/maus-littlefield/src/common_py/geometry/Download')
    file.convert_to_maus('/home/matt/maus-littlefield/src/common_py/geometry/Download/')
if __name__ == "__main__":
    main()