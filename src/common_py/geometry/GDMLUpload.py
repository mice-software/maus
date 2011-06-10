import os.path
import os
import sys
lib_path = os.path.abspath('../../../suds-0.3.9/')
sys.path.append(lib_path)
from suds.client import Client
from datetime import datetime
from base64 import b64encode
from base64 import b64decode

def main():
    from GDMLUpload import upload
    geometry1 = upload("GDML.zip", 1)
    #geometry1.uploadToCDB()
    #geometry1.downloadCurrent()


if __name__ == "__main__":
    main()


class upload:
    def __init__(self, file, testserver):
        self.File = file
        if testserver == None:
            self.WSDLURL = "supermouse server"
        elif testserver == 1:
            self.WSDLURL = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl"
        else: raise StandardError("Incorrect input", "upload::__init__")
        self.Geometry = Client(self.WSDLURL).service

    def uploadToCDB(self):
        _dt = datetime.today()
        _gdml = b64encode(self.File)
        print self.Geometry.getStatus()
        file = zipfile.ZipFile(self.ZipFile, 'r')
        file.extractall("GDML")
        #self.Geometry.setGDML(f.read(), _dt)
        #print self.Geometry.setGDML(_gdml, _dt)

    def downloadCurrent(self):
        downloadedfile = b64decode(self.Geometry.getCurrentGDML())
        print downloadedfile
        



    """
    wsdlUrl = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl"
    geometry = Client(wsdlUrl).service
    print geometry.getStatus()
    _dt = "2011-05-25 12:00:00.0"

    _gdml = b64encode("qwerty2")

    print geometry.setGDML(_gdml, _dt)
    print b64decode(geometry.getCurrentGDML())


    wsdlUrl = "http://rgma19.pp.rl.ac.uk:8080/cdb/beamline?wsdl"
    beamline = Client(wsdlUrl).service
    _beamlineXml = str(beamline.getBeamlineForRun(2879))
    print _beamlineXml


    x = Client(wsdlUrl)
    print x
    """
