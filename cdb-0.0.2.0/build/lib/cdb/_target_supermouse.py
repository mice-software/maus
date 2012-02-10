"TargetSuperMouse module."

from xml.sax import parseString

from cdb._base import _get_string_from_date
from cdb._target import Target
from cdb._exceptions import CdbPermanentError

__all__ = ["TargetSuperMouse"]

class TargetSuperMouse(Target):
    
    """
The TargetSuperMouse class is used to set and retrieve  target settings.

Target data is stored on a per event basis. Prior to an event commencing the
set_start_event method should be called to record the initial parameters. At the end
of the event set_end_event should be called to record the final parameters. All available
data can be retrieved using get_all_targets. A number of methods are available
to retrieve subsets of the data, get_target_for_run,
get_target_for_event, get_targets_for_dates and get_targets_for_pulses.


    """
    
    def __init__(self, url=""):
        """
Construct a TargetSuperMouse.

@param url: the url of the server in the form 'http://host.domain:port'

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """

        super(TargetSuperMouse, self).__init__(url,
                                                 "/cdb/targetSuperMouse?wsdl")

    def __str__(self):
        _help_super = "\n\tset_start_event(event_data(dict)) \
        \n\tset_end_event(int event_number, datetime end_time, int end_pulse)" 
        return "TargetSuperMouse" + self._help + _help_super

    def set_start_event(self, event_data):
        """
Add the initial parameters associated with a event.
@param event_data a dictionary containing the beam line data set with the following keys/values:<pre>
    key - event_number, value - an int representing the event number
    key - start_time, value - a datetime that should represent the time the event started
    key - start_pulse, value - an int representing the total number of pulses at the start of event
    key - delay, value - a float representing the target delay
    key - depth, value - a float representing the target depth
    key - drive_voltage, value - a float representing the target drive voltage
</pre>

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        try:
            event_xml = ("<startEvent eventNumber='" 
                         + str(event_data['event_number'])
                         + "' startTime='" 
                         + _get_string_from_date(event_data['start_time'])
                         + "' delay='" + str(event_data['delay'])
                         + "' depth='" + str(event_data['depth'])
                         + "' driveVoltage='" + str(event_data['drive_voltage'])
                         + "' startPulse='" + str(event_data['start_pulse'])
                         + "' >")
            event_xml = event_xml + "</startEvent>"
        except KeyError, exception:
            raise CdbPermanentError("Missing value for " + str(exception)) 
        xml = str(self._client.setStartEvent(str(event_xml)))
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()

    def set_end_event(self, event_number, end_time, end_pulse):
        """
Add the parameters associated with the end of a event.

@param event_number: an int representing the event number
@param end_time: a datetime that should represent the time the event ended
@param end_pulse: an int representing the total number of pulses at the end of event

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        event_xml = ("<endEvent eventNumber='" + str(event_number)
                     + "' endTime='" + _get_string_from_date(end_time)
                     + "' endPulse='" + str(end_pulse)
                     + "' />")
        xml = str(self._client.setEndEvent(event_xml))
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()

