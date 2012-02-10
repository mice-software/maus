"StateMachineSuperMouse module."

from xml.sax import parseString

from cdb._statemachine import StateMachine

__all__ = ["StateMachineSuperMouse"]

class StateMachineSuperMouse(StateMachine):
    
    """
The StateMachineSuperMouse class is used to set and retrieve  state machine settings.

The state of an individual system can be retrieved using get_current_state and
the state of the entire system can be retrieved with get_current_state_machine.
Old versions of the data are stored for diagnostic purposes. Data can be
retrieved for a given time using get_state_machine_for_date and for a given run
using get_state_machine_for_run.

    """
    
    def __init__(self, url=""):
        """
Construct a StateMachineSuperMouse.

@param url: the url of the server in the form 'http://host.domain:port'

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """

        super(StateMachineSuperMouse,
              self).__init__(url, "/cdb/stateMachineSuperMouse?wsdl")

    def __str__(self):
        _help_super = "\n\tset_state(string system, string state)" 
        return "StateMachineSuperMouse" + self._help + _help_super

    def set_state(self, system, state):
        """
Set the state of the given system to the given state.

@param system: a string containing the system name
@param state: a string containing the state to set

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.setState(str(system), str(state)))
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()

