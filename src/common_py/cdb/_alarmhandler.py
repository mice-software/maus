"AlarmHandler module."

# pylint: disable=C0103,R0801

from xml.sax import parseString
from xml.sax.handler import ContentHandler

from cdb._base import _CdbBase
from cdb._base import _get_date_from_string
from cdb._base import _get_string_from_date
from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError

__all__ = ["AlarmHandler"]


class AlarmHandler(_CdbBase):

    """
The AlarmHandler class is used to retrieve Alarm Handlers (ALH).

The CDB keeps track of when an ALH was in use. An ALH is associated with a tag
and retrieved via that tag, get_tagged_alh. It is also possible to associate a
new ALH with an existing tag; the old ALH is stored for diagnostic purposes.

A couple of methods are provided for diagnostics. It is possible to retrieve a
list of tags that were in use over a given time period, get_used_tags, and to
retrieve the values of an ALH that was in use at a given time, get_used_alh.


        """

    def __init__(self):
        """
Construct an AlarmHandler.

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """
        super(AlarmHandler, self).__init__("/cdb/alarmHandler?wsdl")
        self._alh_handler = _AlarmHandlerHandler()

    def __str__(self):
        return "AlarmHandler \
        \n\tset_url(string url) \
        \n\tget_status() \
        \n\tget_tagged_alh(string tag) \
        \n\tget_used_alh(string timestamp) \
        \n\tget_used_tags(string start_time, string stop_time) \
        \n\tlist_tags()"

    def set_url(self, url):
        """
Set the client to use the given CDB server.

@param url: the URL of the CDB wsdl

@exception CdbTemporaryError: Unable to contact CDB server
@exception CdbPermanentError: Invalid URL

        """
        super(AlarmHandler, self).set_url(url)       

    def get_status(self):
        """
Get the status of the service.

@return a string containing the status of the service

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        return super(AlarmHandler, self).get_status()

    def get_tagged_alh(self, tag):
        """
Get the values of the AlarmHandler for the given tag with the greatest creation
time.

@param tag: a string containing the name of the tag

@return a dictionary containing the alarm handler data set:<pre>
    key - 'tag'
    value - a string containing the tag name
    key - 'creationtime'
    value - a datetime containing the creation time of the tag
    key - 'alarms'
    value - dictionary with alarm specific data\n
    alarm specific data dictionary:
        keys: 'hihi', 'hi', 'lo', 'lolo'</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.getTaggedALH(tag))
        return self._parse_alh_xml(xml)

    def get_used_alh(self, timestamp):
        """
Get the AlarmHandler that was in use for the given timestamp.

@param timestamp: a datetime in UTC

@return a dictionary containing the alarm handler data set:<pre>
    key - 'tag'
    value - a string containing the tag name
    key - 'creationtime'
    value - a datetime containing the creation time of the tag
    key - 'alarms'
    value - dictionary with alarm specific data\n
    alarm specific data dictionary:
        keys: 'hihi', 'hi', 'lo', 'lolo'</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.getUsedALH(timestamp))
        return self._parse_alh_xml(xml)

    def get_used_tags(self, start_time, stop_time=None):
        """
Get a list of tags that were in use for the given time period.

This will include any tags that were in use at the start time. NB one start time
will be before that of the selected start time. The results contain the name of
the tag and the time that it was set as in use. Implicitly a tag stopped being
in use when the next tag was set as in use. It is possible for the same tag to
be repeated consecutively and point to different AlarmHandlers. The details of
an individual AlarmHandler can be found using the get_used_alh method.

@param start_time: a datetime in UTC
@param stop_time: a datetime in UTC. May be None.

@return a dictionary containing the tags data set:<pre>
    key - a datetime containing the creation time
    value - a string containing the tag name</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        start_time = _get_string_from_date(start_time)
        if stop_time == None:
            xml = str(self._client.getUsedTags(start_time, start_time))
        else:
            stop_time = _get_string_from_date(stop_time)
            xml = str(self._client.getUsedTags(start_time, stop_time))
        return self._parse_tag_xml(xml)

    def list_tags(self):
        """
Return a list of known tags. If there are no tags then no &lt;tag&gt; elements
will be returned.

@return a list of strings containing the names of the tags

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = str(self._client.listTags())
        return self._parse_tag_list_xml(xml)

    def _parse_alh_xml(self, xml):
        """ Parser for alh data. """
        parseString(xml, self._alh_handler)
        return self._alh_handler.get_alh()

    def _parse_tag_xml(self, xml):
        """ Parser for alh data. """
        parseString(xml, self._alh_handler)
        return self._alh_handler.get_tags()

    def _parse_tag_list_xml(self, xml):
        """ Parser for alh data. """
        parseString(xml, self._alh_handler)
        return self._alh_handler.get_tag_list()


class _AlarmHandlerHandler(ContentHandler):

    " ContentHandler for alarm handler data. "

    def __init__ (self):
        ContentHandler.__init__(self)
        self._name = ""
        self._alh = {}
        self._alarms = {}
        self._alarm = {}
        self._message = ""
        self._tags = {}
        self._tag_names = []

    def get_alh(self):
        """
        Get a dictionary containing the parsed xml.

        @return the dictionary containing the parsed xml

        """
        return self._alh

    def get_tags(self):
        """
        Get a dictionary containing the parsed xml.

        @return the dictionary containing the parsed xml

        """
        return self._tags

    def get_tag_list(self):
        """
        Get a list containing the parsed xml.

        @return the list containing the parsed xml

        """
        return self._tag_names

    def startElement(self, name, attrs):
        """ Method required for ContentHandler. """
        if name == 'error':
            self._message = ""
        elif name == 'warning':
            self._message = ""
        elif name == 'alarmhandler':
            self._add_alh(attrs)
        elif name == 'alarm':
            self._alarm = {}
            self._name = ""
        elif name == 'name':
            self._message = ""
        elif name == 'hihi':
            self._message = ""
        elif name == 'hi':
            self._message = ""
        elif name == 'lo':
            self._message = ""
        elif name == 'lolo':
            self._message = ""
        elif name == 'tags':
            self._tags = {}
            self._tag_names = []
        elif name == 'tag':
            self._add_tag(attrs)
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
        elif name == 'alarmhandler':
            self._alh["alarms"] = self._alarms
        elif name == 'alarm':
            self._alarms[self._name] = self._alarm
        elif name == 'name':
            self._name = str(self._message)
        elif name == 'hihi':
            self._alarm["hihi"] = float(self._message)
        elif name == 'hi':
            self._alarm["hi"] = float(self._message)
        elif name == 'lo':
            self._alarm["lo"] = float(self._message)
        elif name == 'lolo':
            self._alarm["lolo"] = float(self._message)

    def _add_alh(self, attrs):
        """ Populate a dictionary with data from the xml. """
        self._alh = {}
        self._alarms = {}
        self._alh["creationtime"] = (
            _get_date_from_string(attrs.get('creationtime', "")))
        self._alh["tag"] = str(attrs.get('tag', ""))

    def _add_tag(self, attrs):
        """ Populate a dictionary with data from the xml. """
        self._tag_names.append(str(attrs.get('name', "")))
        try:
            self._tags[_get_date_from_string(attrs.get('starttime', ""))] = (
                str(attrs.get('name', "")))
        except ValueError:
            # starttime might not be present
            pass

