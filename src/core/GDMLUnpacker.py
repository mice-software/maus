import zipfile

def main():
    from GDMLUnpacker import unpacker
    testfile = unpacker("GDML.zip")
    testfile.unzipfile()

if __name__ == "__main__":
    main()

class unpacker:
    def __init__(self, zipfile):
        self.ZipFile = zipfile

    def unzipfile(self):
        file = zipfile.ZipFile(self.ZipFile, 'r')
        file.extractall("GDML") # !!!!!!!!!!! need to input a place to extract the files to here!
        file.close()