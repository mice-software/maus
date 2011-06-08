import zipfile

def main():
    from GDMLUnpacker import unpacker
    testfile = unpacker("GDML.zip")
    testfile.unzipfile()

if __name__ == "__main__":
    main()

class unpacker:
    """
    This class unzips the zip file containing the GDMLs.
    """
    def __init__(self, zipfile):
        """
        @Method Class constructor

        This method constructs a class object.

        @Param Zip File, the name/path of the zip file.
        """
        self.ZipFile = zipfile

    def unzipfile(self):
        """
        @Method unzipfile, this method takes the required zip file and unzips it.
        """
        file = zipfile.ZipFile(self.ZipFile, 'r')
        file.extractall("GDML") # !!!!!!!!!!! need to input a place to extract the files to here!
        file.close()