"Beamline module."

# pylint: disable=C0103,R0801

from xml.sax import parseString
from xml.sax.handler import ContentHandler

from bitarray import bitarray

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

    """

    def __init__(self):
        """
Construct a BeamLine.

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """
        super(Beamline, self).__init__("/cdb/beamline?wsdl")
        self._beamline_handler = _BeamlineHandler()

    def __str__(self):
        return "Beamline \
        \n\tset_url(string url) \
        \n\tget_status() \
        \n\tget_all_beamlines() \
        \n\tget_beamline_for_run(string run_number) \
        \n\tget_beamlines_for_dates(string start_time, string stop_time) \
        \n\tget_beamlines_for_pulses(string start_pulse, string end_pulse)"
  
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
            key = 'runType'
            value - list of strings containing the run types
        and
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

    def get_beamline_for_run(self, run_number):
        """
Get the beam line data for a given run.

@param run_number: the run number

@return a dictionary containing the beam line data set:<pre>
    key - a long containing the run number
    value - dictionary with run specific data\n
    run specific data dictionary:
        simple key value pairs
        except:
            key = 'runType'
            value - list of strings containing the run types
        and
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
        xml = self._client.getBeamlineForRun(run_number)
        return self._parse_beam_line_xml(str(xml))

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
            key = 'runType'
            value - list of strings containing the run types
        and
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
        start_time = _get_string_from_date(start_time)
        if stop_time == None:
            xml = self._client.getBeamlinesForDates(start_time, start_time)
        else:
            stop_time = _get_string_from_date(stop_time)
            xml = self._client.getBeamlinesForDates(start_time, stop_time)
        return self._parse_beam_line_xml(str(xml))
    
    def get_beamlines_for_pulses(self, start_pulse, end_pulse):
        """
Get the beam line data for the given pulse range.

@param start_pulse: pulse number
@param end_pulse: pulse number

@return a dictionary containing the beam line data set:<pre>
    key - a long containing the run number
    value - dictionary with run specific data\n
    run specific data dictionary:
        simple key value pairs
        except:
            key = 'runType'
            value - list of strings containing the run types
        and
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
        xml = self._client.getBeamlinesForPulses(start_pulse, end_pulse)
        return self._parse_beam_line_xml(str(xml))

    def _parse_beam_line_xml(self, xml):
        """ Parser for beamline data. """
        parseString(xml, self._beamline_handler)
        return self._beamline_handler.get_data()


class _BeamlineHandler(ContentHandler):

    " ContentHandler for beamline data. "

    def __init__ (self):
        ContentHandler.__init__(self)
        self._run_dict = _get_run_dict_()
        self._magnets = {}
        self._message = ""
        self._run = {}
        self._run_number = 0
        self._runs = {}

    def get_data(self):
        """
        Get a dictionary containing the parsed xml.

        @return the dictionary containing the parsed xml

        """
        return self._runs

    def startElement(self, name, attrs):
        """ Method required for ContentHandler. """
        if name == 'error':
            self._message = ""
        elif name == 'warning':
            self._message = ""
        elif name == 'runs':
            self._runs = {}
        elif name == 'run':
            self._set_run(attrs)
        elif name == 'magnet':
            self._add_magnet(attrs)
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
        elif name == 'run':
            self._run['magnets'] = self._magnets
            self._runs[self._run_number] = self._run

    def _set_run(self, attrs):
        """ Populate a run dictionary with data from the xml. """
        self._run = {}
        self._magnets = {}
        self._run_number = _get_long(attrs.get('runNumber', ''))
        self._run['runNumber'] = _get_long(attrs.get('runNumber', ''))
        self._run['startTime'] = (
            _get_date_from_string(attrs.get('startTime', '')))
        self._run['endTime'] = (
            _get_date_from_string(attrs.get('endTime', '')))
        self._run['notes'] = _check_null(str(attrs.get('notes', '')))
        self._run['optics'] = _check_null(str(attrs.get('optics', '')))
        self._run[' protonAbsorberThickness'] = (
            _get_int(attrs.get('protonAbsorberThickness', '')))
        self._run['pulseStart'] = _get_long(attrs.get('pulseStart', ''))
        self._run['pulseEnd'] = _get_long(attrs.get('pulseEnd', ''))
        self._run['step'] = _get_float(attrs.get('step', ''))
        self._run['status'] = _get_bool(attrs.get('status', ''))
        self._run['runType'] = self._get_run_type(str(attrs.get('runType', '')))
        self._run['daqTrigger'] = _check_null(str(attrs.get('daqTrigger', '')))
        self._run['daqGateWidth'] = _get_float(attrs.get('daqGateWidth', ''))
        self._run['daqVersion'] = _check_null(str(attrs.get('daqVersion', '')))
        self._run['isisTargetFrequency'] = (
            _get_long(attrs.get('isisTargetFrequency', '')))
        self._run['isisRepRate'] = _get_float(attrs.get('isisRepRate', ''))
        self._run['isisNominalBeamCurrent'] = (
            _get_float(attrs.get('isisNominalBeamCurrent', '')))
        self._run['isisNominalBeamLoss'] = (
            _get_float(attrs.get('isisNominalBeamLoss', '')))
        self._run['isisBeamPositionX'] = (
            _get_float(attrs.get('isisBeamPositionX', '')))
        self._run['isisBeamPositionY'] = (
            _get_float(attrs.get('isisBeamPositionY', '')))
        self._run['targetDepth'] = _get_float(attrs.get('targetDepth', ''))
        self._run['targetDelay'] = _get_float(attrs.get('targetDelay', ''))
        self._run['targetDriveVoltage'] = (
            _get_float(attrs.get('targetDriveVoltage', '')))

    def _add_magnet(self, attrs):
        """ Add a magnet to the magnet dictionary. """
        parameters = {}
        parameters["setCurrent"] = _get_float(attrs.get('setCurrent', ''))
        parameters["polarity"] = _check_null(str(attrs.get('polarity', '')))
        self._magnets[str(attrs.get('name', ''))] = parameters

    def _get_run_type(self, run_type):
        """ Extract a list of run types from a bit(32) variable. """
        _run_types = []
        if run_type == 'null':
            return _run_types
        run_type_array = bitarray(run_type, endian='little')
        i = 0
        while i < len(run_type_array):
            if run_type_array[i]:
                try:
                    _run_types.append(self._run_dict[i])
                except KeyError:
                    raise CdbPermanentError(
                        "ERROR - unknown run type. Unable to find run type "
                        + str(i) + " in run type dictionary")
            i = i + 1
        return _run_types

    
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

def _get_run_dict_():
    """ A dictionary containing know run types. """
    # TODO define run types
    types = {}
    types[0] = "spam"
    types[1] = "egg"
    types[2] = "chips"
    types[3] = "beans"
    types[4] = "spam"
    types[5] = "egg"
    types[6] = "chips"
    types[7] = "beans"
    types[8] = "spam"
    types[9] = "egg"
    types[10] = "chips"
    types[11] = "beans"
    types[12] = "spam"
    types[13] = "egg"
    types[14] = "chips"
    types[15] = "beans"
    types[16] = "spam"
    types[17] = "egg"
    types[18] = "chips"
    types[19] = "beans"
    types[20] = "spam"
    types[21] = "egg"
    types[22] = "chips"
    types[23] = "beans"
    types[24] = "spam"
    types[25] = "egg"
    types[26] = "chips"
    types[27] = "beans"
    types[28] = "spam"
    types[29] = "egg"
    types[30] = "chips"
    types[31] = "beans"
    return types

