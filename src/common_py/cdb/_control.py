"Control module."

# pylint: disable=C0103,R0801

from xml.sax import parseString
from xml.sax.handler import ContentHandler

from cdb._base import _CdbBase
from cdb._base import _get_string_from_date
from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError


__all__ = ["Control"]


class Control(_CdbBase):

    """
The Control class is used to retrieve control parameter values.

Old control parameter values are stored for diagnostic purposes. A set of
control parameter values can be retrieved for a given time using
get_previous_settings


    """

    def __init__(self):
        """
Construct a Control.

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """
        super(Control, self).__init__("/cdb/control?wsdl")
        self._control_handler = _ControlHandler()

    def __str__(self):
        return "Control \
        \n\tset_url(string url) \
        \n\tget_status() \
        \n\tget_controls() \
        \n\tget_controls_for_crate(crate) \
        \n\tget_previous_settings(string timestamp)"

    def set_url(self, url):
        """
Set the client to use the given CDB server.

@param url: the URL of the CDB wsdl

@exception CdbTemporaryError: Unable to contact CDB server
@exception CdbPermanentError: Invalid URL

        """
        super(Control, self).set_url(url)

    def get_status(self):
        """
Get the status of the service.

@return a string containing the status of the service

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        return super(Control, self).get_status()

    def get_controls(self):
        """
Get the parameter values of the controls.

@return a list of dictionaries with each dictionary representing a channel. Each
dictionary contains key value pairs specific to the crate plus values for 
predefined keys:<pre>
    Crate
    Name
    SL
    CH</pre>
    
@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.getControls())
        return self._parse_control_xml(xml)

    def get_controls_for_crate(self, crate):
        """
Get the parameter values of the controls for the given crate.

@param crate: a string containing the name of the crate

@return a list of dictionaries with each dictionary representing a channel. Each
dictionary contains key value pairs specific to the crate plus values for 
predefined keys:<pre>
    Crate
    Name
    SL
    CH</pre>
    
@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.getControlsForCrate(crate))
        return self._parse_control_xml(xml)

    def get_previous_settings(self, timestamp):
        """
Get the parameter values of the controls as they were set at the given
timestamp.

@param timestamp: a datetime in UTC

@return a list of dictionaries with each dictionary representing a channel. Each
dictionary contains key value pairs specific to the crate plus values for 
predefined keys:<pre>
    Crate
    Name
    SL
    CH</pre>
    
@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        timestamp = _get_string_from_date(timestamp)
        xml = str(self._client.getPreviousSettings(timestamp))
        return self._parse_control_xml(xml)


    def _parse_control_xml(self, xml):
        """ Parser for control data. """
        parseString(xml, self._control_handler)
        return self._control_handler.get_data()


class _ControlHandler(ContentHandler):

    " ContentHandler for control data. "

    def __init__ (self):
        ContentHandler.__init__(self)
        self._crate_name = ""
        self._channels = []
        self._channel_data = {}
        self._message = ""

    def get_data(self):
        """
        Get a list containing the parsed xml.

        @return the list containing the parsed xml

        """
        return self._channels

    def startElement(self, name, attrs):
        """ Method required for ContentHandler. """
        if name == 'error':
            self._message = ""
        elif name == 'warning':
            self._message = ""
        elif name == 'crate':
            self._crate_name = str(attrs.get('name', ""))
        elif name == 'channel':
            self._add_channel(attrs)
        elif name == 'parameter':
            self._channel_data[str(attrs.get('name', ""))] = str(
                attrs.get('value', ""))
        return

    def characters(self, message):
        """ Method required for ContentHandler. """
        self._message = self._message + message

    def endElement(self, name):
        """ Method required for ContentHandler. """
        if name == 'error':
            raise CdbPermanentError(self._message)
        elif name == 'warning':
            raise CdbTemporaryError(self._message)
        elif name == 'channel':
            self._channels.append(self._channel_data)
        elif name == 'crate':
            self._crate_name = ""

    def _add_channel(self, attrs):
        """ Populate a dictionary with data from the xml. """
        self._channel_data = {}
        self._channel_data["Crate"] = self._crate_name
        self._channel_data["Name"] = str(attrs.get('remoteChannelName', ""))
        self._channel_data["SL"] = str(attrs.get('module', ""))
        self._channel_data["CH"] = str(attrs.get('name', ""))

