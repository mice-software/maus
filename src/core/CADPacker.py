import zipfile

def main():
    from Packer import packer
    filelist1 = packer("geom1.txt", "filltobezipped.txt")
    filelist1.gdmlToOneFile()

if __name__ == "__main__":
    main()

class packer:
    def __init__(self, filelist, zipfile):
        self.ZipFileName = zipfile
        list = []
        self.List = list
        self.FileList = filelist
        fin = open(self.FileList, 'r')
        for line in fin.readlines():
            self.List.append(line.strip())
     
    def gdmlToOneFile(self):
        fin1 = open(self.FileList, 'r')
        NumOfFiles = len(fin1.readlines())
        for n in range(0, NumOfFiles):
            file = self.List[n]
            fin2 = open(file)
            for lines in fin2.readlines():
                str = lines
                print str