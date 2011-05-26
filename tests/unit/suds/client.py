"Mimic calls to suds.client."

# pylint: disable=R0903,C0103,W0613

from re import search
from urllib2 import URLError
from httplib import InvalidURL

from . import _xml

class Client():
    "Mimic suds.client Client class."

    def __init__(self, url):
        if search('URLError$', url):
            raise URLError("")
        elif search('TransportError$', url):
            raise TransportError()
        elif search('ValueError$', url):
            raise ValueError()
        elif search('InvalidURL$', url):
            raise InvalidURL()
        elif search('error$', url):
            self.service = _Service(2)
        elif search('warn$', url):
            self.service = _Service(1)
        else:
            self.service = _Service(0)
    
class _Service():
    "Mimic a suds service."

    def __init__(self, status):
        self.status = status

    def getStatus(self):
        """Mimic soap call getStatus. Return a status dependent on how the class
        was initialised."""
        if self.status == 0:
            return _xml.STATUS_OK
        elif self.status == 1:
            return _xml.STATUS_WARN
        elif self.status == 2:
            return _xml.STATUS_ERROR

#ALARMHANDLER
    def getTaggedALH(self, tag):
        "Mimic soap call getTaggedALH."
        if self.status == 0:
            return _xml.ALH_TAGGED_ALH
        return self.getStatus()

    def getUsedALH(self, timestamp):
        "Mimic soap call getUsedALH."
        if self.status == 0:
            return _xml.ALH_USED_ALH
        return self.getStatus()

    def getUsedTags(self, start_time, stop_time):
        "Mimic soap call getUsedTags."
        if self.status == 0:
            return _xml.ALH_USED_TAGS
        return self.getStatus()

    def listTags(self):
        "Mimic soap call getUsedTags."
        if self.status == 0:
            return _xml.ALH_LIST_TAGS
        return self.getStatus()

# BEAMLINE
    def getAllBeamlines(self):
        "Mimic soap call getAllBeamlines."
        if self.status == 0:
            return _xml.BL_RUNS
        return self.getStatus()

    def getBeamlinesForDates(self, start_time, stop_time):
        "Mimic soap call getBeamlinesForDates."
        if self.status == 0:
            return _xml.BL_DATES
        return self.getStatus()

    def getBeamlinesForPulses(self, start_pulse, stop_pulse):
        "Mimic soap call getBeamlinesForPulses."
        if self.status == 0:
            return _xml.BL_PULSES
        return self.getStatus()

    def getBeamlineForRun(self, run_number):
        "Mimic soap call getBeamlineForRun."
        if self.status == 0:
            return _xml.BL_RUN
        return self.getStatus()
    
# CONTROL
    def getControls(self):
        "Mimic soap call getControls."
        if self.status == 0:
            return _xml.C_CONTROLS
        return self.getStatus()

    def getControlsForCrate(self, crate):
        "Mimic soap call getControlsForCrate."
        if self.status == 0:
            return _xml.C_CONTROLS_FOR_CRATE
        return self.getStatus()

    def getPreviousSettings(self, timestamp):
        "Mimic soap call getPreviousSettings."
        if self.status == 0:
            return _xml.C_CONTROLS_FOR_PREVIOUS
        return self.getStatus()

# GEOMETRY

    def get_geo_status(self):
        """Mimic soap call getStatus. Return a status dependent on how the class
        was initialised."""
        if self.status == 0:
            return _xml.G_STATUS_OK
        elif self.status == 1:
            return _xml.G_STATUS_WARN
        elif self.status == 2:
            return _xml.G_STATUS_ERROR

    def getCurrentGDML(self):
        "Mimic soap call getCurrentGDML."
        if self.status == 0:
            return _xml.G_GDML_CURRENT
        return self.get_geo_status()

    def getGDMLForId(self, id_):
        "Mimic soap call getGDMLForId."
        if self.status == 0:
            return _xml.G_GDML_FOR_ID
        return self.get_geo_status()

    def getGDMLForRun(self, run):
        "Mimic soap call getGDMLForRun."
        if self.status == 0:
            return _xml.G_GDML_FOR_RUN
        return self.get_geo_status()

    def getIds(self, start_time, stop_time=None):
        "Mimic soap call getIds."
        if self.status == 0:
            return _xml.G_IDS
        return self.getStatus()


class TransportError(Exception):
    "Mimic suds TransportError class."

    def __init__(self):
        super(TransportError, self).__init__()

