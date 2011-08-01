from suds.client import Client
from datetime import datetime
from base64 import b64encode
from base64 import b64decode

wsdlUrl = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl"
geometry = Client(wsdlUrl).service
print Client(wsdlUrl)
print geometry.getStatus()
print geometry.getIds("2011-07-29 09:00:00.0", "2011-07-31 09:00:00.0" )
_dt = "2011-05-25 12:00:00.0"

_gdml = b64encode("qwerty2")

#print geometry.setGDML(_gdml, _dt)
#print b64decode(geometry.getCurrentGDML())


#wsdlUrl = "http://rgma19.pp.rl.ac.uk:8080/cdb/beamline?wsdl"
#beamline = Client(wsdlUrl).service
#_beamlineXml = str(beamline.getBeamlineForRun(2879))
#print _beamlineXml



#x = Client(wsdlUrl)
#print x
