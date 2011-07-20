import os.path
import os

def main():
    from GDMLFormatter import formatter
    gdmls = formatter('GDML')
    gdmls.format()

if __name__ == '__main__':
    main()

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
            self.NewLine = ""
            self.OldLine = ""
            self.Text = ""
            self.ConfigurationFile = None
            self.MaterialFile = None
            self.MaterialFilePath = None
            list = []
            self.StepFiles = list
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
                if fname.find('Step') >= 0:
                    file = self.Path + '/' + fname
                    self.StepFiles.append(file)

        def lineReplace(self, file, oldline, newline):
            """
            @Method Line Replace, this method opens the file and finds the line of interest and replaces that line with the new line

            @Param File, name of the file which the line will be replaced in
            @Param oldline, name of the string which is in the line needed to be replaced
            @Param newline, the new string to be placed in the file
            """
            self.OldLine = oldline
            self.NewLine = newline
            File = file
            fin = open(File, 'r')
            for line in fin.readlines():
                if line.find(self.OldLine) >= 0:
                     self.OldLine = line
            fin.close()
            path = self.Path + File
            fin = open(File, 'r')
            for lines in fin.readlines():
                self.Text += lines
            fin.close()
            fout = open(File, "w")
            fout.write(self.Text.replace(self.OldLine, self.NewLine))
            self.Text = ""
            fout.close()

        def format(self):
            """
            @Method format, this method formats the GDML files by replacing the paths of the materials file and GDML schema with the correct paths
            """
            newline1 = '<!ENTITY materials SYSTEM "'+ self.MaterialFilePath +'">'
            self.lineReplace(self.ConfigurationFile, "ENTITY materials SYSTEM", newline1)
            newline2 = 'xsi:noNamespaceSchemaLocation="GDML_3_0_0/schema/gdml.xsd" xmlns:gdml="http://cern.ch/2001/Schemas/GDML">'
            self.lineReplace(self.ConfigurationFile, "xsi:noNamespaceSchemaLocation=", newline2)
            NumOfStepFiles = len(self.StepFiles)
            for num in range(0, NumOfStepFiles):
                newline3 = '<!ENTITY materials SYSTEM "'+ self.MaterialFilePath +'">'
                self.lineReplace(self.StepFiles[num], "ENTITY materials SYSTEM", newline3)
                newline4 = 'xsi:noNamespaceSchemaLocation="GDML_3_0_0/schema/gdml.xsd" xmlns:gdml="http://cern.ch/2001/Schemas/GDML">'
                self.lineReplace(self.StepFiles[num], "xsi:noNamespaceSchemaLocation=", newline4)



