"Cabling module."

from xml.sax import parseString
from xml.sax.handler import ContentHandler

from cdb._base import _CdbBase
from cdb._base import _get_string_from_date
from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError


__all__ = ["Cabling"]


class Cabling(_CdbBase):

    """
The Cabling class is used to retrieve cabling data.

For the control system a channel is described by its location with reference to
a crate and module. For the detectors and trackers a channel is described by its
location with reference to computer id and geo number. Each channel has data
associated with it. Old versions of the data are stored for diagnostic purposes.
Data can be retrieved for a given time using get_cabling_for_date and for a
given run using get_cabling_for_run.


    """

    def __init__(self, url="",
                 wsdl_dir="/cdb/cabling?wsdl"):
        """
Construct a Cabling.

@param url: the url of the server in the form 'http://host.domain:port'
@param wsdl_dir: the path to the wsdl on the server

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """
        super(Cabling, self).__init__(url, wsdl_dir)
        self._cabling_handler = _CablingHandler()
        self._help = "\n\tset_url(string url) \
        \n\tget_status() \
        \n\tget_cabling_for_date(string device, datetime timestamp) \
        \n\tget_cabling_for_run(string device, int run_number) \
        \n\tget_current_cabling(string device) \
        \n\tlist_devices()"

    def __str__(self):
        return "Cabling" + self._help

    def set_url(self, url):
        """
Set the client to use the given CDB server.

@param url: the URL of the CDB wsdl

@exception CdbTemporaryError: Unable to contact CDB server
@exception CdbPermanentError: Invalid URL

        """
        super(Cabling, self).set_url(url)

    def get_status(self):
        """
Get the status of the service.

@return a string containing the status of the service

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        return super(Cabling, self).get_status()

    def get_cabling_for_date(self, device, timestamp):
        """
Get the cabling data of the given device, where the name of the device may be
that of a detector, tracker or control, that was valid at the given timestamp.

@param device: a string containing the name of the detector, tracker or control
@param timestamp: a datetime in UTC

@return a list of dictionaries
    
@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        timestamp = _get_string_from_date(timestamp)
        xml = str(self._client.getCablingForDate(str(device), timestamp))
        return self._parse_cabling_xml(xml)

    def get_cabling_for_run(self, device, run_number):
        """
Get the cabling data of the given device, where the name of the device may be
that of a detector, tracker or control, that was valid for the given run number.

@param device: a string containing the name of the detector, tracker or control
@param run_number:  a long containing the number of a run

@return a list of dictionaries

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.getCablingForRun(device, run_number))
        return self._parse_cabling_xml(xml)

    def get_current_cabling(self, device):
        """
Get the cabling data of the given device, where the name of the device may be
that of a detector, tracker or control.

@param device: a string containing the name of the detector, tracker or control

@return a list of dictionaries
    
@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.getCurrentCabling(device))
        return self._parse_cabling_xml(xml)
    
    def list_devices(self):
        """
Get a list of known devices. These are the device names that are recognised by this API.

@return a list of device names
    
@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.listDevices())
        return self._parse_cabling_xml(xml)
    

    def _parse_cabling_xml(self, xml):
        """ Parser for cabling data. """
        parseString(xml, self._cabling_handler)
        return self._cabling_handler.get_data()


class _CablingHandler(ContentHandler):
    " ContentHandler for cabling data. "

    def __init__ (self):
        ContentHandler.__init__(self)
        self._device_type = ""
        self._vlsb_computer_id = ""
        self._vlsb_geo_number = ""
        self._crate = ""
        self._module = ""
        self._data = []
        self._message = ""

    def get_data(self):
        """
        Get a list containing the parsed xml.

        @return the list containing the parsed xml

        """
        return self._data

    def startElement(self, name, attrs): #pylint: disable-msg=C0103
        """ Method required for ContentHandler. """
        if name == 'error':
            self._message = ""
        elif name == 'warning':
            self._message = ""
        elif name == 'cabling':
            self._reset()
            self._device_type = str(attrs.get('type', ""))
        elif name == 'vlsbComputerId':
            self._vlsb_computer_id = int(attrs.get('number', ""))
        elif name == 'vlsbGeoNumber':
            self._vlsb_geo_number = int(attrs.get('number', ""))
        elif name == 'vlsbChannel':
            if self._device_type == "TRACKER":
                self._add_vlsb_channel(attrs)
        elif name == 'crate':
            self._crate = int(attrs.get('number', ""))
        elif name == 'module':
            self._module = int(attrs.get('number', ""))            
        elif name == 'channel':
            if self._device_type == "CONTROL":
                self._add_control_channel(attrs)
        elif name == 'device':
            self._data.append(str(attrs.get('name', "")))
        return

    def characters(self, message):
        """ Method required for ContentHandler. """
        self._message = self._message + message

    def endElement(self, name): #pylint: disable-msg=C0103
        """ Method required for ContentHandler. """
        if name == 'error':
            raise CdbPermanentError(self._message)
        elif name == 'warning':
            raise CdbTemporaryError(self._message)

    def _add_vlsb_channel(self, attrs):
        """ Populate a dictionary with data from the xml. """
        _cable_data = {}
        _cable_data["vlsb_computer_id"] = self._vlsb_computer_id
        _cable_data["vlsb_geo_number"] = self._vlsb_geo_number
        _cable_data["vlsb_channel"] = int(attrs.get('number', ""))
        _cable_data["tracker_no"] = int(attrs.get('trackerNumber', ""))
        _cable_data["station"] = int(attrs.get('station', ""))
        _cable_data["plane"] = int(attrs.get('plane', ""))
        _cable_data["channel"] = int(attrs.get('channel', ""))
        self._data.append(_cable_data)

    def _add_control_channel(self, attrs):
        """ Populate a dictionary with data from the xml. """
        _cable_data = {}
        _cable_data["crate"] = self._crate
        _cable_data["module"] = self._module
        _cable_data["channel"] = int(attrs.get('number', ""))
        _cable_data["name"] = str(attrs.get('name', ""))
        self._data.append(_cable_data)

    def _reset(self):
        """ Reset self values. """
        self._device_type = ""
        self._vlsb_computer_id = ""
        self._vlsb_geo_number = ""
        self._crate = ""
        self._module = ""
        self._data = []
        self._message = ""

