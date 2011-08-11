from ConfigReader import Configreader
from GDMLtoCDB import Downloader


def main():
    inputfile = Configreader()
    inputfile.readconfig()
    CurrentGeometry = Downloader(1)
    CurrentGeometry.get_ids(inputfile.starttime, inputfile.stoptime)

if __name__ == "__main__":
    main()