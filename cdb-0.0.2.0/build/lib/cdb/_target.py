"Target module."

from xml.sax import parseString
from xml.sax.handler import ContentHandler

from cdb._base import _CdbBase
from cdb._base import _get_date_from_string
from cdb._base import _get_string_from_date
from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError


__all__ = ["Target"]


class Target(_CdbBase):

    """
The Target class is used to retrieve target data.

Target data is stored on a per event basis. All available data can be retrieved
using get_all_targets. A number of methods are available to retrieve subsets of
the data, get_target_for_run, get_target_for_event, get_targets_for_dates and
get_targets_for_pulses.


    """

    def __init__(self, url="",
                 wsdl_dir="/cdb/target?wsdl"):
        """
Construct a Target.

@param url: the url of the server in the form 'http://host.domain:port'
@param wsdl_dir: the path to the wsdl on the server

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """
        super(Target, self).__init__(url, wsdl_dir)
        self._target_handler = _TargetHandler()
        self._help = "\n\tset_url(string url) \
        \n\tget_status() \
        \n\tget_all_targets() \
        \n\tget_target_for_run(int run_number) \
        \n\tget_target_for_event(int event_number) \
        \n\tget_targets_for_dates(datetime start_time, datetime stop_time) \
        \n\tget_targets_for_pulses(int start_pulse, int end_pulse)"

    def __str__(self):
        return "Target" + self._help

    def set_url(self, url):
        """
Set the client to use the given CDB server.

@param url: the URL of the CDB wsdl

@exception CdbTemporaryError: Unable to contact CDB server
@exception CdbPermanentError: Invalid URL

        """
        super(Target, self).set_url(url)

    def get_status(self):
        """
Get the status of the service.

@return a string containing the status of the service

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        return super(Target, self).get_status()
    
    def get_all_targets(self):
        """
Get the target data for all events.

@return a dictionary containing the target data set:<pre>
    key - a long containing the event number
    value - dictionary with event specific data</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.getAllTargets())
        print xml
        print self._parse_target_xml(xml)
        return self._parse_target_xml(xml)
    
    
    def get_target_for_run(self, run_number):
        """
Get the target data for the given run number.

@param run_number:  a long containing the number of a run

@return a dictionary containing the target data set:<pre>
    key - a long containing the event number
    value - dictionary with event specific data</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.getTargetForRun(run_number))
        return self._parse_target_xml(xml)
    
    def get_target_for_event(self, event_number):
        """
Get the target data for the given event number.

@param event_number:  a long containing the number of a event

@return a dictionary containing the target data set:<pre>
    key - a long containing the event number
    value - dictionary with event specific data</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.getTargetForEvent(event_number))
        return self._parse_target_xml(xml)
        
    def get_targets_for_dates(self, start_time, stop_time=None):
        """
Get the target data  for the given time period.

N.B. if the stop_time is None then only the data that were valid at the
start_time will be returned.

@param start_time: a datetime in UTC
@param stop_time: a datetime in UTC. May be None.

@return a dictionary containing the target data set:<pre>
    key - a long containing the event number
    value - dictionary with event specific data</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        start_time = _get_string_from_date(start_time)
        if stop_time == None:
            xml = str(self._client.getTargetsForDates(start_time, start_time))
        else:
            stop_time = _get_string_from_date(stop_time)
            xml = str(self._client.getTargetsForDates(start_time, stop_time))
        return self._parse_target_xml(xml)
    
    def get_targets_for_pulses(self, start_pulse, end_pulse=None):
        """
Get the target data for the given pulse range.

N.B. if the end_pulse is None then only the data that were valid at the
start_pulse will be returned.

@param start_pulse: an int representing pulse number
@param end_pulse: an int representing pulse number. May be None.

@return a dictionary containing the target data set:<pre>
    key - a long containing the event number
    value - dictionary with event specific data</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        if end_pulse == None:
            end_pulse = start_pulse
        xml = str(self._client.getTargetsForPulses(start_pulse, end_pulse))
        return self._parse_target_xml(xml)   

    def _parse_target_xml(self, xml):
        """ Parser for target data. """
        parseString(xml, self._target_handler)
        return self._target_handler.get_data()


class _TargetHandler(ContentHandler):

    " ContentHandler for target data. "

    def __init__ (self):
        ContentHandler.__init__(self)
        self._message = ""
        self._event = {}
        self._events = {}
        
    def get_data(self):
        """
        Get a list containing the parsed xml.

        @return the list containing the parsed xml

        """
        return self._events

    def startElement(self, name, attrs): #pylint: disable-msg=C0103
        """ Method required for ContentHandler. """
        if name == 'error':
            self._message = ""
        elif name == 'warning':
            self._message = ""
        elif name == 'events':
            self._events = {}
        elif name == 'event':
            self._set_event(attrs)
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

    def _set_event(self, attrs):
        """ Populate a event dictionary with data from the XML. """
        self._event = {}
        self._event['start_time'] = (
            _get_date_from_string(attrs.get('startTime', '')))
        self._event['end_time'] = (
            _get_date_from_string(attrs.get('endTime', '')))
        self._event['start_pulse'] = _get_long(attrs.get('startPulse', ''))
        self._event['end_pulse'] = _get_long(attrs.get('endPulse', ''))
        self._event['delay'] = _get_float(attrs.get('delay', ''))
        self._event['depth'] = _get_float(attrs.get('depth', ''))
        self._event['drive_voltage '] = (_get_float
                                         (attrs.get('driveVoltage', '')))
        self._events[long(attrs.get('eventNumber', ''))] = self._event
        
        
def _get_float(string):
    """ Convert string to float """
    try:
        float_attr = float(string)
    except ValueError:
        return None
    return float_attr

def _get_long(string):
    """ Convert string to long """
    try:
        long_attr = long(string)
    except ValueError:
        return None
    return long_attr

