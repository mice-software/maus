import os
from CADImport import CADImport

class gdmltomaus():
        
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
                if fname != self.ConfigurationFile  and fname != self.MaterialFile:
                    file = self.Path + '/' + fname
                    self.StepFiles.append(file)
            length = len(self.StepFiles)
            for num in range(0, length):
                if self.StepFiles[num] == self.ConfigurationFile:
                    file1 = self.StepFiles[num]
                if self.StepFiles[num] == self.MaterialFile:
                    file2 = self.StepFiles[num]
            self.StepFiles.remove(file1)
            self.StepFiles.remove(file2)

        def converttomaus(self, output):
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
                StepFile = CADImport(xmlin1 = '/home/matt/maus-littlefield/src/common_py/geometry/testCases/testGeometry/Step_2.gdml', xsl = '/home/matt/maus-littlefield/src/common_py/geometry/xsltScripts/MMTranslation.xsl', output = outputfile)
                StepFile.XSLTParse()
                StepFile = None
            StepFile = CADImport(xmlin1 = self.StepFiles[num], xsl = MMxsl, output = outputfile)
            StepFile.XSLTParse()
            

def main():
    file = gdmltomaus('/home/matt/maus-littlefield/src/common_py/geometry/testCases/testGeometry')
    file.converttomaus('/home/matt/maus-littlefield/src/common_py/geometry/Download/MM')
if __name__ == "__main__":
    main()