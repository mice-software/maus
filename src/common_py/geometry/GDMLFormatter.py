import xml.dom
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
                    node.attributes['xsi:noNamespaceSchemaLocation'] = "/home/matt/Desktop/Matt/CAD2G4/GDML_fastradModel/GDML_3_0_0/schema/gdml.xsd"
            os.remove(file)
            fout = open(file, 'w')
            xmldoc.writexml(fout)
            fout.close()

        def formatMaterials(self, file):
            Materials = minidom.parse(self.MaterialFile)
            for node in Materials.getElementsByTagName("materials"):
                MaterialsDef = node
            Config = minidom.parse(file)
            x = Config.createElement("materials")
            #Config.childNodes[1].childNodes[1].appendChild(x)
            #for node in Config.getElementsByTagName("define"):
                #gdml.appendChild(MaterialsDef)
            print Config.toxml()
            """
            for node2 in Config.getElementsByTagName("define"):
                    Define = node2
                for node3 in Config.getElementsByTagName("solids"):
                    Solids = node3
                for node4 in Config.getElementsByTagName("structure"):
                    Structure = node4
                fin = open(file, 'r')
                for lines in fin.readlines():
                    if lines.find("setup") >= 0:
                        for node5 in Config.getElementsByTagName("setup"):
                            Setup = node5
                
            doc = minidom.Document()
            gdml = doc.createElement("gdml")
            doc.appendChild(gdml)
            define = doc.createElement("define")
            gdml.appendChild(Define)
            print doc.toxml()
            """                               
            #uri = xml.dom.XML_NAMESPACE
            #xmldoc = minidom.getDOMImplementation()
            #NewDoc = minidom.DOMImplementation.createDocument(xml.dom.XML_NAMESPACE)
            #print NewDoc.toxml()
            """
            impl = minidom.getDOMImplementation()
            newdoc = impl.createDocument(None, "gdml", None)
            top_element = newdoc.documentElement
            text = newdoc.createTextNode('Some textual content.')
            xmldoc = minidom.parse(file)
            for node in xmldoc.getElementsByTagName("gdml"):
                text = node
            top_element.appendChild(text)
            xmldoc = minidom.parse(self.MaterialFile)
            for node in xmldoc.getElementsByTagName("material"):
                text = node
            top_element.appendChild(text)
            print newdoc.toprettyxml()
            """

            

        def format(self):
            self.formatMaterials(self.ConfigurationFile)
            self.formatSchemaLocation(self.ConfigurationFile)
            NoOfStepFiles = len(self.StepFiles)
            #for num in range(0, NoOfStepFiles):
             #   self.formatSchemaLocation(self.StepFiles[num])
            





"""
            def lineReplace(self, file, oldline, newline):
            
            @Method Line Replace, this method opens the file and finds the line of interest and replaces that line with the new line

            @Param File, name of the file which the line will be replaced in
            @Param oldline, name of the string which is in the line needed to be replaced
            @Param newline, the new string to be placed in the file
            
            self.OldLine = oldline
            self.NewLine = newline
            File = file
            fin = open(File, 'r')
            for line in fin.readlines():
                if line.find(self.OldLine) >= 0:
                     self.OldLine = line
            fin.close()
            fin = open(File, 'r')
            for lines in fin.readlines():
                self.Text += lines
            fin.close()
            fout = open(File, "w")
            fout.write(self.Text.replace(self.OldLine, self.NewLine))
            self.Text = ""
            fout.close()


        def format(self):

            @Method format, this method formats the GDML files by replacing the paths of the materials file and GDML schema with the correct paths
   
            newline1 = '<!ENTITY materials SYSTEM "'+ self.MaterialFilePath +'">]>'
            self.lineReplace(self.ConfigurationFile, "ENTITY materials SYSTEM", newline1)
            newline2 = 'xsi:noNamespaceSchemaLocation="/home/matt/Desktop/Matt/CAD2G4/GDML_fastradModel/GDML_3_0_0/schema/gdml.xsd" >'
            self.lineReplace(self.ConfigurationFile, "xsi:noNamespaceSchemaLocation=", newline2)
            NumOfStepFiles = len(self.StepFiles)
            for num in range(0, NumOfStepFiles):
                newline3 = '<!ENTITY materials SYSTEM "'+ self.MaterialFilePath +'">'
                self.lineReplace(self.StepFiles[num], "ENTITY materials SYSTEM", newline3)
                newline4 = 'xsi:noNamespaceSchemaLocation="GDML_3_0_0/schema/gdml.xsd">'
                self.lineReplace(self.StepFiles[num], "xsi:noNamespaceSchemaLocation=", newline4)
    """

def main():
    gdmls = formatter('/home/matt/workspace/Maus/testCases/testGeometry')
    gdmls.format()

if __name__ == '__main__':
    main()