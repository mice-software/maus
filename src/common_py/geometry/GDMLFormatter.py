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
            self.Formatted = False
            self.txtfile = ""
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
            length = len(self.StepFiles)
            for num in range(0, length):
                if self.StepFiles[num] == self.ConfigurationFile:
                    file1 = self.StepFiles[num]
                if self.StepFiles[num] == self.MaterialFile:
                    file2 = self.StepFiles[num]
            self.StepFiles.remove(file1)
            self.StepFiles.remove(file2)

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
            self.txtfile = "" 
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
            self.txtfile = file[:-5] + '.txt' #NEED to change for gdmls
            fout = open(self.txtfile, 'w')
            NewDoc.writexml(fout)
            fout.close()
            os.remove(file)
            
        def insertMaterialsRef(self, file):
            fin = open(file, 'r')
            gdmlfile = file[:-4] + '.gdml'
            fout = open(gdmlfile, 'w')
            for line in fin.readlines():
                if line.find('<!-- Materials definition CallBack -->')>=0:
                    new_line = line.replace('<!-- Materials definition CallBack -->', '<!-- Materials definition CallBack --> &materials;')
                    print >>fout,new_line
                else:
                    print >>fout,line
            print >>fout, '<!-- Formatted for MAUS -->'
            fin.close()
            fout.close()
            os.remove(file)
            
        def formatCheck(self, file):
            self.Formatted = False
            fin = open(file, 'r')
            for lines in fin.readlines():
                if lines.find('<!-- Formatted for MAUS -->') >= 0:
                    print file + ' already formatted! No formatting will be done.'
                    self.Formatted = True
            fin.close()
        
            
        def format(self):
            self.formatCheck(self.ConfigurationFile)
            if self.Formatted == False:
                self.formatSchemaLocation(self.ConfigurationFile)
                self.formatMaterials(self.ConfigurationFile)
                self.insertMaterialsRef(self.txtfile)
            
            NoOfStepFiles = len(self.StepFiles)
            for num in range(0, NoOfStepFiles):
                self.formatCheck(self.StepFiles[num])
                if self.Formatted == False:
                    self.formatSchemaLocation(self.StepFiles[num])
                    self.formatMaterials(self.StepFiles[num])
                    self.insertMaterialsRef(self.txtfile)
            print "Format Complete!"
            
                    
def main():
    gdmls = formatter('/home/matt/maus-littlefield/src/common_py/geometry/testCases/testGeometry')
    gdmls.format()

if __name__ == '__main__':
    main()