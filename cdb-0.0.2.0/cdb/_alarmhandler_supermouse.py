"AlarmHandlerSuperMouse module."

from xml.sax import parseString

from cdb._alarmhandler import AlarmHandler
from cdb._base import _get_string_from_date

__all__ = ["AlarmHandlerSuperMouse"]

class AlarmHandlerSuperMouse(AlarmHandler):
    
    """
The AlarmHandlerSuperMouse class is used to store and retrieve Alarm Handlers (ALH).

The CDB keeps track of when an ALH was in use. An ALH is associated with a tag
and retrieved via that tag, get_tagged_alh. It is also possible to associate a
new ALH with an existing tag; the old ALH is stored for diagnostic purposes.

A couple of methods are provided for diagnostics. It is possible to retrieve a
list of tags that were in use over a given time period, get_used_tags, and to
retrieve the values of an ALH that was in use at a given time, get_used_alh.


        """
    
    def __init__(self, url=""):
        """
Construct an AlarmHandlerSuperMouse.

@param url: the url of the server in the form 'http://host.domain:port'

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """
        super(AlarmHandlerSuperMouse,
              self).__init__(url, "/cdb/alarmHandlerSuperMouse?wsdl")
       
    def __str__(self):
        _help_super = (
        "\n\tset_tagged_alh(string tag, [{string name, string hihi, string hi, \
        \n\t\tstring lo, string lolo}] alarms) \
        \n\tset_tag_in_use(string tag, datetime creation_time)")
        return "AlarmHandlerSuperMouse" + self._help + _help_super

    def set_tagged_alh(self, tag, alarms):
        """
Set the values of the alarm handler with the given tag. If an alarm handler with
the tag already exists then the passed in alarm handler will become the new
alarm handler for that tag. If there is an old alarm handler that has been used,
it will be accessible via the get_used_tags and get_used_alh methods.
    
@param tag: the name of the tag
@param alarms: a list dictionaries containing the alarm handler data:<pre>
        keys: 'name', 'hihi', 'hi', 'lo', 'lolo'</pre>

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        _alarm_xml = "<alarmhandler>"
        for _alarm in alarms:
            _alarm_xml = (_alarm_xml + "<alarm>" 
                          + "<name>" + str(_alarm["name"]) + "</name>"
                          + "<hihi>" + str(_alarm["hihi"]) + "</hihi>"
                          + "<hi>" + str(_alarm["hi"]) + "</hi>"
                          + "<lo>" + str(_alarm["lo"]) + "</lo>"
                          + "<lolo>" + str(_alarm["lolo"]) + "</lolo>"
                          + "</alarm>")
        _alarm_xml = _alarm_xml + "</alarmhandler>" 
            
        xml = str(self._client.setTaggedALH(str(tag), _alarm_xml))
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()
            
    def set_tag_in_use(self, tag, creation_time):
        """
Record the fact that the alarm handler with the given tag and creation time is
now in use. Only one alarm handler maybe in use at a time. A call to this method
will supersede all previous calls to this method.
    
@param tag: the name of the tag
@param creation_time: the datetime of when the alarm handler was created in UTC

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        creation_time = _get_string_from_date(creation_time)
        xml = str(self._client.setTagInUse(str(tag), creation_time))
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()

