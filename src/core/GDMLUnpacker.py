import zipfile

def main():
    from GDMLUnpacker import unpacker
    testfile = unpacker("GDML.zip")
    testfile.unzip()

if __name__ == "__main__":
    main()

class unpacker:
    def __init__(self, zipfile):
        self.ZipFile = zipfile

    def unzip(self):
        file = zipfile.ZipFile(self.ZipFile, 'r')
        file.extractall()
        file.close()



        