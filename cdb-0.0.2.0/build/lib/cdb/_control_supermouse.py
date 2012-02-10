"ControlSuperMouse module."

from xml.sax import parseString

from cdb._control import Control

__all__ = ["ControlSuperMouse"]

class ControlSuperMouse(Control):
    
    """
The ControlSuperMouse class is used to set and retrieve control parameter values.

Old control parameter values are stored for diagnostic purposes. A set of
control parameter values can be retrieved for a given time using
get_previous_settings

    """
    
    def __init__(self, url=""):
        """
Construct a ControlSuperMouse.

@param url: the url of the server in the form 'http://host.domain:port'

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """
        
        super(ControlSuperMouse, self).__init__(url,
                                                "/cdb/controlSuperMouse?wsdl")

    def __str__(self):
        _help_super = (
        "\n\tset_parameter(int crate, int module, int channel, \
        \n\t\tstring parameter_name, string parameter_value) \
        \n\tupdate_parameter(int crate, int module, int channel,\
        \n\t\tstring parameter_name, string parameter_value)")
        return "ControlSuperMouse" + self._help + _help_super

    def set_parameter(self, crate, module, channel, parameter_name,
                      parameter_value):
        #pylint: disable-msg=R0913
        """
Set the parameter value of an individual parameter for the given channel. A
timestamp is associated with the parameter. N.B. There must NOT already be a
value set for the parameter otherwise an error will be reported.
    
@param crate: the number of the crate
@param module: the number of the module
@param channel: the number of the channel
@param parameter_name: a string containing the name of the parameter
@param parameter_value: a string containing the value to set 

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.setParameter(str(crate), str(module),
                                               str(channel),
                                               str(parameter_name),
                                               str(parameter_value)))
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()
            
    def update_parameter(self, crate, module, channel, parameter_name,
                         parameter_value):
        #pylint: disable-msg=R0913
        """
Update the parameter value of an individual parameter for the given channel. The
timestamp associated with the parameter is updated to the current time.N.B.
There must already be a value set for the parameter otherwise an error will be
reported.
    
@param crate: the number of the crate
@param module: the number of the module
@param channel: the number of the channel
@param parameter_name: a string containing the name of the parameter
@param parameter_value: a string containing the value to set 

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.updateParameter(str(crate), str(module),
                                               str(channel),
                                               str(parameter_name),
                                               str(parameter_value)))
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()

