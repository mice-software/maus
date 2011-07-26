import os
import os.path
from xml.dom import minidom

class formatter:
        """
        @Class formatter this class formats the raw outputted fastrad files so there schema material file paths are correct

        This class opens the files which need the correct paths added and edits the file in situ.
        """
        def __init__(self, path):
            """
            @Method Class Constructor

            This is the class constructor which sets up the variables so they point at the correct files ready to edit.

            @Param Path The path to the directory which contains the fastrad outputted GDML files
            """
            self.TextFile = ""
            self.ConfigurationFile = None
            self.MaterialFile = None
            self.MaterialFilePath = None
            self.Schema = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/gdmlSchema/GDML_3_0_0/schema/gdml.xsd'
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
                if fname.find('fastrad') >= 0:
                    file = self.Path + '/' + fname
                    if file != self.MaterialFile:
                        self.ConfigurationFile = file
                if fname != self.ConfigurationFile  and fname != self.MaterialFile:
                    file = self.Path + '/' + fname
                    self.StepFiles.append(file)
            """length = len(self.StepFiles)
            for num in range(0, length):
                if self.StepFiles[num] == self.ConfigurationFile:
                    file1 = self.StepFiles[num]
                if self.StepFiles[num] == self.MaterialFile:
                    file2 = self.StepFiles[num]
                if self.StepFiles[num][-4:] == '.txt':
                    self.StepFiles.remove(num)
                else:
                    self.TextFile = None
            self.StepFiles.remove(file1)
            self.StepFiles.remove(file2)
            if self.TextFile != None:
                self.StepFiles.remove(self.TextFile)
                """
        

        def formatSchemaLocation(self, file):
            xmldoc = minidom.parse(file)
            for node in xmldoc.getElementsByTagName("gdml"):
                if node.hasAttribute("xsi:noNamespaceSchemaLocation"):
                    node.attributes['xsi:noNamespaceSchemaLocation'] = self.Schema
            os.remove(file)
            fout = open(file, 'w')
            xmldoc.writexml(fout)
            fout.close()

        def formatMaterials(self, file): 
            impl = minidom.getDOMImplementation()
            str = 'gdml [<!ENTITY materials SYSTEM "' + self.MaterialFile + '">]'
            docType = impl.createDocumentType(str, None, None)
            NewDoc = impl.createDocument(None, "MAUS", docType)
            Config = minidom.parse(file)
            for node in Config.getElementsByTagName("gdml"):
                RootEle = node
            for node2 in NewDoc.getElementsByTagName("MAUS"):
                OldEle = node2
            NewDoc.replaceChild(RootEle, OldEle)
            os.remove(file)
            fout = open(file, 'w')
            NewDoc.writexml(fout)
            fout.close()
            
        def insertMaterialsRef(self, file):
            fin = open(file, 'r')
            LinesInFile = []
            for lines in fin.readlines():
                if lines.find('')

                
                        
        def format(self):
            self.formatMaterials(self.ConfigurationFile)
            self.formatSchemaLocation(self.ConfigurationFile)
            self.insertMaterialsRef(self.ConfigurationFile)
            NoOfStepFiles = len(self.StepFiles)
            #for num in range(0, NoOfStepFiles):
             #   self.formatSchemaLocation(self.StepFiles[num])
            
def main():
    gdmls = formatter('/home/matt/workspace/Maus/testCases/testGeometry')
    gdmls.format()

if __name__ == '__main__':
    main()