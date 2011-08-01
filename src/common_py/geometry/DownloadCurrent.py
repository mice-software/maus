from GDMLtoCDB import downloader
from GDMLtoMAUSModule import gdmltomaus
from ConfigReader import configreader
from GDMLFormatter import formatter

def main():
    inputfile = configreader()
    inputfile.readconfig()
    CurrentGeometry = downloader(1)
    CurrentGeometry.downloadCurrent(inputfile.DownloadDir)
    gdmls = formatter(inputfile.DownloadDir)
    gdmls.format()
    
    #MausModules = gdmltomaus(inputfile.DownloadDir)
    #MausModules.converttomaus("location")
    

if __name__ == "__main__":
    main()