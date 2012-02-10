"Beamline module."

from xml.sax import parseString
from xml.sax.handler import ContentHandler

from cdb._base import _CdbBase
from cdb._base import _get_date_from_string
from cdb._base import _get_string_from_date
from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError


__all__ = ["Beamline"]


class Beamline(_CdbBase):
    
    """
The Beamline class is used to retrieve data about beam line settings.

Beam line data is stored on a per run basis. All available data can be retrieved
using get_all_beamlines. A number of methods are available to retrieve subsets
of the data, get_beamline_for_run, get_beamlines_for_dates and
get_beamlines_for_pulses.

In addition there is the concept of tagged data. It is possible to tag a set of
parameters using set_beamline_tag. Tagged parameters can then be retrieved using
get_beamline_for_tag. The use of set_beamline_tag with an existing tag name will
replace the existing parameters, although the original parameters will still be
stored in the database.

    """

    def __init__(self, url="",
                 wsdl_dir="/cdb/beamline?wsdl"):
        """
Construct a BeamLine.

@param url: the url of the server in the form 'http://host.domain:port'
@param wsdl_dir: the path to the wsdl on the server

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """
        super(Beamline, self).__init__(url, wsdl_dir)
        self._beamline_handler = _BeamlineHandler()
        self._help = "\n\tset_url(string url) \
        \n\tget_status() \
        \n\tget_all_beamlines() \
        \n\tget_all_beamlines_xml() \
        \n\tget_beamline_for_run(int run_number) \
        \n\tget_beamline_for_run_xml(int run_number) \
        \n\tget_beamlines_for_dates(datetime start_time, datetime stop_time) \
        \n\tget_beamlines_for_dates_xml(datetime start_time, datetime stop_time) \
        \n\tget_beamlines_for_pulses(int start_pulse, int end_pulse) \
        \n\tget_beamlines_for_pulses_xml(int start_pulse, int end_pulse)\
        \n\tget_beamline_for_tag(str tag)"

    def __str__(self):
        return "Beamline" + self._help
  
    def set_url(self, url):
        """
Set the client to use the given CDB server.

@param url: the URL of the CDB wsdl

@exception CdbTemporaryError: Unable to contact CDB server
@exception CdbPermanentError: Invalid URL

        """
        super(Beamline, self).set_url(url)

    def get_status(self):
        """
Get the status of the service.

@return a string containing the status of the service

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        return super(Beamline, self).get_status()

    def get_all_beamlines(self):
        """
Get the beam line data for all runs.

@return a dictionary containing the beam line data set:<pre>
    key - a long containing the run number
    value - dictionary with run specific data\n
    run specific data dictionary:
        simple key value pairs
        except:
            key = 'magnets'
            value - dictionary with magnet data\n
            magnet data dictionary:
                key - a string containing the name of magnet
                value - a dictionary containing magnet specific data\n
                magnet specific data dictionary:
                    simple key value pairs</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        return self._parse_beam_line_xml(str(self._client.getAllBeamlines()))

    def get_all_beamlines_xml(self):
        """
Get the beam line data for all runs.

@return a string containing the beam line data set as XML

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        return str(self._client.getAllBeamlines())

    def get_beamline_for_run(self, run_number):
        """
Get the beam line data for a given run.

@param run_number: an int representing the run number

@return a dictionary containing the beam line data set:<pre>
    key - a long containing the run number
    value - dictionary with run specific data\n
    run specific data dictionary:
        simple key value pairs
        except:
            key = 'magnets'
            value - dictionary with magnet data\n
            magnet data dictionary:
                key - a string containing the name of magnet
                value - a dictionary containing magnet specific data\n
                magnet specific data dictionary:
                    simple key value pairs</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        return self._parse_beam_line_xml(
            self.get_beamline_for_run_xml(run_number))

    def get_beamline_for_run_xml(self, run_number):
        """
Get the beam line data for a given run.

@param run_number: an int representing the run number

@return a string containing the beam line data set as XML

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        return str(self._client.getBeamlineForRun(str(run_number)))

    def get_beamlines_for_dates(self, start_time, stop_time=None):
        """
Get the beam line data for the given time period.

N.B. if the stop_time is None then only the data that were valid at the
start_time will be returned.

@param start_time: a datetime in UTC
@param stop_time: a datetime in UTC. May be None.

@return a dictionary containing the beam line data set:<pre>
    key - a long containing the run number
    value - dictionary with run specific data\n
    run specific data dictionary:
        simple key value pairs
        except:
            key = 'magnets'
            value - dictionary with magnet data\n
            magnet data dictionary:
                key - a string containing the name of magnet
                value - a dictionary containing magnet specific data\n
                magnet specific data dictionary:
                    simple key value pairs</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        return self._parse_beam_line_xml(
            self.get_beamlines_for_dates_xml(start_time, stop_time))

    def get_beamlines_for_dates_xml(self, start_time, stop_time=None):
        """
Get the beam line data for the given time period.

N.B. if the stop_time is None then only the data that were valid at the
start_time will be returned.

@param start_time: a datetime in UTC
@param stop_time: a datetime in UTC. May be None.

@return a string containing the beam line data set as XML

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        start_time = _get_string_from_date(start_time)
        if stop_time == None:
            xml = self._client.getBeamlinesForDates(start_time, start_time)
        else:
            stop_time = _get_string_from_date(stop_time)
            xml = self._client.getBeamlinesForDates(start_time, stop_time)
        return str(xml)

    def get_beamlines_for_pulses(self, start_pulse, end_pulse=None):
        """
Get the beam line data for the given pulse range.

N.B. if the end_pulse is None then only the data that were valid at the
start_pulse will be returned.

@param start_pulse: an int representing pulse number
@param end_pulse: an int representing pulse number. May be None.

@return a dictionary containing the beam line data set:<pre>
    key - a long containing the run number
    value - dictionary with run specific data\n
    run specific data dictionary:
        simple key value pairs
        except:
            key = 'magnets'
            value - dictionary with magnet data\n
            magnet data dictionary:
                key - a string containing the name of magnet
                value - a dictionary containing magnet specific data\n
                magnet specific data dictionary:
                    simple key value pairs</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        if end_pulse == None:
            end_pulse = start_pulse
        return self._parse_beam_line_xml(self.get_beamlines_for_pulses_xml
            (start_pulse, end_pulse))

    def get_beamlines_for_pulses_xml(self, start_pulse, end_pulse=None):
        """
Get the beam line data for the given pulse range.

N.B. if the end_pulse is None then only the data that were valid at the
start_pulse will be returned.

@param start_pulse: an int representing pulse number
@param end_pulse: an int representing pulse number. May be None.

@return a string containing the beam line data set as XML

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        if end_pulse == None:
            end_pulse = start_pulse
        return str(self._client.getBeamlinesForPulses(str(start_pulse),
            str(end_pulse)))
        
        
    def get_beamline_for_tag(self, tag):
        """
Get the beam line data for the given tag.

@param tag: a string containing the tag name

@return a dictionary containing the beam line data set:<pre>
    key - a long containing the run number
    value - dictionary with run specific data\n
     specific data dictionary:
        simple key value pairs
        except:
            key = 'magnets'
            value - dictionary with magnet data\n
            magnet data dictionary:
                key - a string containing the name of magnet
                value - a dictionary containing magnet specific data\n
                magnet specific data dictionary:
                    simple key value pairs</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = self._client.getBeamlineForTag(tag)
        return self._parse_beam_line_xml(str(xml))
    
        
    def _parse_beam_line_xml(self, xml):
        """ Parser for beamline data. """
        parseString(xml, self._beamline_handler)
        return self._beamline_handler.get_data()


class _BeamlineHandler(ContentHandler):
#pylint: disable-msg=R0902

    " ContentHandler for beamline data. "

    def __init__ (self):
        ContentHandler.__init__(self)
        self._magnets = {}
        self._message = ""
        self._run = {}
        self._run_number = 0
        self._runs = {}
        self._tag = {}
        self._tag_name = ""
        self._tags = {}
        self.run_xml = False
        self.tag_xml = False

    def get_data(self):
        """
        Get a dictionary containing the parsed XML.

        @return the dictionary containing the parsed XML

        """
        if self.run_xml:
            return self._runs
        else:
            return self._tags

    def startElement(self, name, attrs): #pylint: disable-msg=C0103
        """ Method required for ContentHandler. """
        if name == 'error':
            self._message = ""
        elif name == 'warning':
            self._message = ""
        elif name == 'runs':
            self._runs = {}
            self.run_xml = True
            self.tag_xml = False
        elif name == 'tags':
            self._tag = {}
            self.run_xml = False
            self.tag_xml = True
        elif name == 'tag':
            self._set_tag(attrs)
        elif name == 'run':
            self._set_run(attrs)
        elif name == 'magnet':
            self._add_magnet(attrs)
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
        elif name == 'run':
            self._run['magnets'] = self._magnets
            self._runs[self._run_number] = self._run
        elif name == 'tag':
            self._tag['magnets'] = self._magnets
            self._tags[self._tag_name] = self._tag
            
    def _set_tag(self, attrs):
        """ Populate a run dictionary with data from the XML. """
        self._tag = {}
        self._magnets = {}
        self._tag_name = str(attrs.get('name', ''))
        self._tag['proton_absorber_thickness'] = (
            _get_int(attrs.get('protonAbsorberThickness', '')))
        self._tag['beam_stop'] = (_get_bool(attrs.get('beamStop', '')))
        self._tag['diffuser_thickness'] = (
            _get_int(attrs.get('diffuserThickness', '')))
        
    def _set_run(self, attrs):
        """ Populate a run dictionary with data from the XML. """
        self._run = {}
        self._magnets = {}
        self._run_number = _get_long(attrs.get('runNumber', ''))
        self._run['run_number'] = _get_long(attrs.get('runNumber', ''))
        self._run['start_time'] = (
            _get_date_from_string(attrs.get('startTime', '')))
        self._run['end_time'] = (
            _get_date_from_string(attrs.get('endTime', '')))
        self._run['notes'] = _check_null(str(attrs.get('notes', '')))
        self._run['optics'] = _check_null(str(attrs.get('optics', '')))
        self._run['proton_absorber_thickness'] = (
            _get_int(attrs.get('protonAbsorberThickness', '')))
        self._run['start_pulse'] = _get_long(attrs.get('startPulse', ''))
        self._run['end_pulse'] = _get_long(attrs.get('endPulse', ''))
        self._run['step'] = _get_float(attrs.get('step', ''))
        self._run['status'] = _get_bool(attrs.get('status', ''))
        self._run['run_type'] = _check_null(str(attrs.get('runType', '')))
        self._run['daq_trigger'] = _check_null(str(attrs.get('daqTrigger', '')))
        self._run['daq_gate_width'] = _get_float(attrs.get('daqGateWidth', ''))
        self._run['daq_version'] = _check_null(str(attrs.get('daqVersion', '')))
        self._run['beam_stop'] = (_get_bool(attrs.get('beamStop', '')))
        self._run['diffuser_thickness'] = (
            _get_int(attrs.get('diffuserThickness', '')))
        self._run['gdc_host_name'] = (
            _check_null(str(attrs.get('gdcHostName', ''))))

    def _add_magnet(self, attrs):
        """ Add a magnet to the magnet dictionary. """
        if self.run_xml:
            parameters = {}
            parameters["set_current"] = _get_float(attrs.get('setCurrent', ''))
            parameters["polarity"] = _get_int(str(attrs.get('polarity', '')))
            self._magnets[str(attrs.get('name', ''))] = parameters
        else:
            self._magnets[str(attrs.get('name', ''))] = (
                _get_float(attrs.get('setCurrent', '')))
    
def _check_null(string):
    """ If string is 'null' convert it to None. """
    if string == 'null':
        string = None
    return string

def _get_bool(string):
    """ If string is 'true' convert it to True. """
    if string == "true":
        return True
    return False

def _get_float(string):
    """ Convert string to float """
    try:
        float_attr = float(string)
    except ValueError:
        return None
    return float_attr
    
def _get_int(string):
    """ Convert string to int """
    try:
        int_attr = int(string)
    except ValueError:
        return None
    return int_attr

def _get_long(string):
    """ Convert string to long """
    try:
        long_attr = long(string)
    except ValueError:
        return None
    return long_attr

