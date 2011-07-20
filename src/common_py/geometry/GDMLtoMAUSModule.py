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

        def converttomaus(self):
            outputlocation = '/home/matt/Desktop/FastRad/Step4Maus/ConfigurationFile.txt'
            ConfigFile = CADImport(xmlin1 = self.ConfigurationFile, xsl = "GDML2G4MICE.xsl", output = outputlocation)
            ConfigFile.XSLTParse()
            length = len(self.StepFiles)
            for num in range(0, length):
                file = str(self.StepFiles[num])
                file = file.strip(self.Path)
                file = file[0:-5]
                outputlocation2 = '/home/matt/Desktop/FastRad/Step4Maus/' + file + '.txt'
                StepFile = CADImport(xmlin1 = self.StepFiles[num], xsl = "MMTranslation.xsl", output = outputlocation2)
                #StepFile.XSLTParse()
            xml = "/home/matt/Desktop/FastRad/Step4_Light_version/TDm1055m0150.gdml"
            StepFile = CADImport(xmlin1 = self.StepFiles[num], xsl = "MMTranslation.xsl", output = outputlocation2)
            StepFile.XSLTParse()





def main():
    file = gdmltomaus('/home/matt/NetBeansProjects/MAUSConfigPY/src/GDML')
    file.converttomaus()
if __name__ == "__main__":
    main()