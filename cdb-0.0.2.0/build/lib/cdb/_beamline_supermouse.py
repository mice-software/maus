"BeamlineSuperMouse module."

from xml.sax import parseString

from cdb._base import _get_string_from_date
from cdb._beamline import Beamline
from cdb._exceptions import CdbPermanentError

__all__ = ["BeamlineSuperMouse"]

class BeamlineSuperMouse(Beamline):
    
    """
The BeamlineSuperMouse class is used to set and retrieve  beam line settings.

Beam line data is stored on a per run basis. Prior to a run commencing the
set_start_run and set_beamline methods should be called to record the initial
parameters. At the end of the set_end_run should be called to record the final
parameters. All available data can be retrieved using get_all_beamlines. A
number of methods are available to retrieve subsets of the data,
get_beamline_for_run, get_beamlines_for_dates and get_beamlines_for_pulses.

In addition there is the concept of tagged data. It is possible to tag a set of
parameters using set_beamline_tag. Tagged parameters can then be retrieved using
get_beamline_for_tag. The use of set_beamline_tag with an existing tag name will
replace the existing parameters, although the original parameters will still be
stored in the database.

    """
    
    def __init__(self, url=""):
        """
Construct a BeamlineSuperMouse.

@param url: the url of the server in the form 'http://host.domain:port'

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """

        super(BeamlineSuperMouse, self).__init__(url,
                                                 "/cdb/beamlineSuperMouse?wsdl")

    def __str__(self):
        _help_super = "\n\tset_start_run(dict run_data) \
        \n\tset_end_run(int run_number, datetime end_time, int end_pulse, \
        \n\t\tboolean status) \
        \n\tset_beamline(dict run_data, list(dict) magnets \
        \n\tset_beamline_tag(str tag, dict data, list(dict) magnets"
        return "BeamlineSuperMouse" + self._help + _help_super

    def set_start_run(self, run_data):
        """
Add the initial parameters associated with a run.

@param run_data a dictionary containing the beam line data set with the following keys/values:<pre>
    key - run_number, value - an int representing the run number
    key - start_time, value - a datetime that should represent the time the run started
    key - notes, value - a string
    key - optics, value - a string representing the beamline optics
    key - start_pulse, value - an int representing the total number of pulses at the start of run
    key - step, value - a float representing the MICE phase
    key - run_type, value - a sting
    key - daq_trigger, value - a string
    key - daq_gate_width, value - a float representing the gate width in ms
    key - daq_version, value - a string
    key - gdc_host_name, value - a string</pre>

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        try:
            run_xml = ("<startRun runNumber='" + str(run_data['run_number'])
                       + "' startTime='"
                       + _get_string_from_date(run_data['start_time'])
                       + "' notes='" + str(run_data['notes'])
                       + "' optics='" + str(run_data['optics'])
                       + "' startPulse='" + str(run_data['start_pulse'])
                       + "' step='" + str(run_data['step'])
                       + "' runType='" + str(run_data['run_type'])
                       + "' daqTrigger='" + str(run_data['daq_trigger'])
                       + "' daqGateWidth='" + str(run_data['daq_gate_width'])
                       + "' daqVersion='" + str(run_data['daq_version'])
                       + "' gdcHostName='" + str(run_data['gdc_host_name'])
                       + "' >")
            run_xml = run_xml + "</startRun>"
        except KeyError, exception:
            raise CdbPermanentError("Missing value for " + str(exception))
        xml = str(self._client.setStartRun(str(run_xml)))
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()

    def set_end_run(self, run_number, end_time, end_pulse, status):
        """
Add the parameters associated with the end of a run.

@param run_number: an int representing the run number
@param end_time: a datetime that should represent the time the run ended
@param end_pulse: an int representing the total number of pulses at the end of run
@param status: a boolean, true indicates data are analysable

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        run_xml = ("<endRun runNumber='" + str(run_number)
                   + "' endTime='" + _get_string_from_date(end_time)
                   + "' endPulse='" + str(end_pulse)
                   + "' status='" + str(int(status))   
                   + "' />")
        xml = str(self._client.setEndRun(run_xml))
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()

    def set_beamline(self, run_data, magnets):
        """
Add the initial parameters associated with a run.

@param run_data a dictionary containing the beam line data set with the following keys/values:<pre>
    key - run_number, value - an int representing the run number
    key - diffuser_thickness, value - an int representing the thickness of diffuser the in mm
    key - beam_stop, value - a boolean, true if the beam stop was in place
    key - proton_absorber_thickness, value - an int representing the thickness of the proton absorber in mm</pre>
@param magnets a list of dictionaries containing the magnet data with the following keys/values:<pre>
    key - name, value - a string
    key - set_current, value - a float
    key - polarity, value - a string</pre>

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        try:
            run_xml = ("<beamline runNumber='" + str(run_data['run_number'])
                       + "' beamStop='" 
                       + str(run_data['beam_stop'])
                       + "' diffuserThickness='" 
                       + str(run_data['diffuser_thickness'])
                       + "' protonAbsorberThickness='" 
                       + str(run_data['proton_absorber_thickness'])
                       + "' >")
            if magnets != None:
                for magnet in magnets:
                    run_xml = (run_xml + "<magnet name='" + str(magnet['name'])
                               + "' setCurrent='" + str(magnet['set_current'])
                               + "' polarity='" + str(magnet['polarity'])
                               + "'/>")
            run_xml = run_xml + "</beamline>"
        except KeyError, exception:
            raise CdbPermanentError("Missing value for " + str(exception))
        xml = str(self._client.setBeamline(str(run_xml)))
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()



    def set_beamline_tag(self, tag, data, magnets):
        """
Add a tagged set of beamline parameters.

@param tag a string containing the name of the tag 
@param data a dictionary containing the beam line data set with the following keys/values:<pre>
    key - diffuser_thickness, value - an int representing the thickness of diffuser the in mm
    key - beam_stop, value - a boolean, true if the beam stop was in place
    key - proton_absorber_thickness, value - an int representing the thickness of the proton absorber in mm</pre>
@param magnets a list of dictionaries containing the magnet data with the following keys/values:<pre>
    key - name, value - a string
    key - set_current, value - a float</pre>

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        try:
            xml = ("<tag name='" + str(tag)
                       + "' beamStop='" 
                       + str(data['beam_stop'])
                       + "' diffuserThickness='" 
                       + str(data['diffuser_thickness'])
                       + "' protonAbsorberThickness='" 
                       + str(data['proton_absorber_thickness'])
                       + "' >")
        except KeyError, exception:
            raise CdbPermanentError("Missing value for " 
                                    + str(exception))       
        try:
            if magnets != None:
                for magnet in magnets:
                    xml = (xml + "<magnet name='" + str(magnet['name'])
                               + "' setCurrent='" + str(magnet['set_current'])
                               + "'/>")
            xml = xml + "</tag>"
        except KeyError, exception:
            raise CdbPermanentError("Missing value for " + str(exception))
        print xml
        return_xml = str(self._client.setBeamlineTag(str(xml)))
        parseString(return_xml, self._status_handler)
        return self._status_handler.get_message()

