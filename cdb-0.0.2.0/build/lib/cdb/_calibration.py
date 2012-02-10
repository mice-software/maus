"Calibration module."

from xml.sax import parseString
from xml.sax.handler import ContentHandler

from cdb._base import _CdbBase
from cdb._base import _get_string_from_date
from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError


__all__ = ["Calibration"]


class Calibration(_CdbBase):

    """
The Calibration class is used to retrieve calibration data.

For the detectors and trackers data is described by its location with reference
to to bank, board and channel. Old versions of the data are stored for
diagnostic purposes. Data can be retrieved for a given time using
get_calibration_for_date and for a given run using get_calibration_for_run.


    """

    def __init__(self, url="",
                 wsdl_dir="/cdb/calibration?wsdl"):
        """
Construct a Calibration.

@param url: the url of the server in the form 'http://host.domain:port'
@param wsdl_dir: the path to the wsdl on the server

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """
        super(Calibration, self).__init__(url, wsdl_dir)
        self._calibration_handler = _CalibrationHandler()
        self._help = "\n\tset_url(string url) \
        \n\tget_status() \
        \n\tget_calibration_for_date(string device, datetime timestamp) \
        \n\tget_calibration_for_run(string device, int run_number) \
        \n\tget_current_calibration(string device) \
        \n\tlist_devices()"
        
    def __str__(self):
        return "Calibration" + self._help

    def set_url(self, url):
        """
Set the client to use the given CDB server.

@param url: the URL of the CDB wsdl

@exception CdbTemporaryError: Unable to contact CDB server
@exception CdbPermanentError: Invalid URL

        """
        super(Calibration, self).set_url(url)

    def get_status(self):
        """
Get the status of the service.

@return a string containing the status of the service

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        return super(Calibration, self).get_status()

    def get_calibration_for_date(self, device, timestamp):
        """
Get the calibration data of the given device, where the name of the device may be
that of a detector or tracker, that was valid at the given timestamp.

@param device: a string containing the name of the detector or tracker
@param timestamp: a datetime in UTC

@return a list of dictionaries
    
@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        timestamp = _get_string_from_date(timestamp)
        xml = str(self._client.getCalibrationForDate(str(device), timestamp))
        return self._parse_calibration_xml(xml)

    def get_calibration_for_run(self, device, run_number):
        """
Get the calibration data of the given device, where the name of the device may be
that of a detector or tracker, that was valid for the given run number.

@param device: a string containing the name of the detector or tracker
@param run_number:  a long containing the number of a run

@return a list of dictionaries

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.getCalibrationForRun(device, run_number))
        return self._parse_calibration_xml(xml)

    def get_current_calibration(self, device):
        """
Get the calibration data of the given device, where the name of the device may be
that of a detector or tracker.

@param device: a string containing the name of the detector or tracker

@return a list of dictionaries
    
@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.getCurrentCalibration(device))
        return self._parse_calibration_xml(xml)
    
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
        return self._parse_calibration_xml(xml)
    

    def _parse_calibration_xml(self, xml):
        """ Parser for calibration data. """
        parseString(xml, self._calibration_handler)
        return self._calibration_handler.get_data()


class _CalibrationHandler(ContentHandler):
    " ContentHandler for calibration data. "

    def __init__ (self):
        ContentHandler.__init__(self)
        self._device_type = ""
        self._board = ""
        self._bank = ""
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
        elif name == 'calibration':
            self._reset()
            self._device_type = str(attrs.get('type', ""))
        elif name == 'device':
            self._data.append(str(attrs.get('name', "")))
        elif name == 'board':
            self._board = int(attrs.get('number', ""))
        elif name == 'bank':
            self._bank = int(attrs.get('number', ""))
        elif name == 'channel':
            if self._device_type == "TRACKER":
                self._add_tracker(attrs)
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

    def _add_tracker(self, attrs):
        """ Populate a dictionary with data from the xml. """
        _cable_data = {}
        _cable_data["bank"] = self._board
        _cable_data["board"] = self._bank
        _cable_data["channel"] = int(attrs.get('number', ""))
        _cable_data["adc_pedestal"] = float(attrs.get('adcPedestal', ""))
        _cable_data["adc_gain"] = float(attrs.get('adcGain', ""))
        _cable_data["tdc_pedestal"] = float(attrs.get('tdcPedestal', ""))
        _cable_data["tdc_slope"] = float(attrs.get('tdcSlope', ""))
        self._data.append(_cable_data)

    def _reset(self):
        """ Reset self values. """
        self._device_type = ""
        self._board = ""
        self._bank = ""
        self._data = []
        self._message = ""


