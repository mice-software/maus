"Geometry module."

# pylint: disable=C0103,R0801

from base64 import b64decode
from xml.sax import parseString
from xml.sax import SAXParseException
from xml.sax.handler import ContentHandler

from cdb._base import _CdbBase
from cdb._base import _get_date_from_string
from cdb._base import _get_string_from_date
from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError

__all__ = ["Geometry"]


class Geometry(_CdbBase):

    """
The Geometry class is used to retrieve a GDML data set.

To store GDML data sets use the GeometrySuperMouse class. Previous versions of
the GDML data sets are also stored. The current GDML data set can be retrieved
using get_current_gdml. The GDML data set associated with a run can be retrieved
using get_gdml_for_run. Each GDML data set has a unique id. A list of ids for
data sets that were valid at a given time or over a time range can be retrieve
using get_ids and the GDML data set associated with an id can be retrieved using
get_gdml_for_id. The time a GDML data set was valid from is defined by the user
when storing the GDML data set.

    """

    def __init__(self):
        """
Construct a Geometry.

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """
        super(Geometry, self).__init__("/cdb/geometry?wsdl")
        self._geometry_handler = _GeometryHandler()

    def __str__(self):
        return "Geometry \
        \n\tset_url(string url) \
        \n\tget_status() \
        \n\tget_current_gdml() \
        \n\tget_gdml_for_id(string id) \
        \n\tget_gdml_for_run(string run_number) \
        \n\tget_ids(string start_time, string stop_time)"
        
    def set_url(self, url):
        """
Set the client to use the given CDB server.

@param url: the URL of the CDB wsdl

@exception CdbTemporaryError: Unable to contact CDB server
@exception CdbPermanentError: Invalid URL

        """
        super(Geometry, self).set_url(url)

    def get_status(self):
        """
Get the status of the service.

@return a string containing the status of the service

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        return super(Geometry, self).get_status()
        
    def get_current_gdml(self):
        """
Get the current GDML data set.

The current GDML is the one with the latest validFromTime. If there are more
than one GDML with the the same latest validFromTime then the one with the
latest creation time is returned.

@return a string containing the GDML data set

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        _gdml = b64decode(self._client.getCurrentGDML())
        try:
            # check for error or warning tag
            parseString(_gdml, self._status_handler)
        except SAXParseException:
            pass # no tag found so as expected could not parse gdml
        return _gdml

    def get_gdml_for_id(self, id_):
        """
Get the GDML data set for the given id.

@param id_: an int containing the id of the GDML

@return a string containing the GDML data set

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        _gdml = b64decode(self._client.getGDMLForId(id_))
        try:
            # check for error or warning tag
            parseString(_gdml, self._status_handler)
        except SAXParseException:
            pass # no tag found and could not parse gdml
        return _gdml

    def get_gdml_for_run(self, run_number):
        """
Get the GDML data set that was valid when the run represented by the run number
occurred.

If more than one GDML data set was valid then the one with the latest creation
time is returned.

@param run_number: a long containing the number of a run

@return a string containing the GDML data set

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        _gdml = b64decode(self._client.getGDMLForRun(run_number))
        try:
            # check for error or warning tag
            parseString(_gdml, self._status_handler)
        except SAXParseException:
            pass # no tag found and could not parse gdml
        return _gdml

    def get_ids(self, start_time, stop_time=None):
        """
Return a list of ids of the GDML data sets that were valid during the given time
period.

N.B. if the stop_time is None then only the ids that were valid at the
start_time will be returned.

@param start_time: a datetime in UTC
@param stop_time: a datetime in UTC. May be None. 

@return a dictionary containing the geometry ids data set:<pre>
    key - id
    value - dictionary with id specific data\n
    id specific data dictionary:
        key - validFrom
            a datetime from when the gdml id was valid
        key - created
            a datetime of when the gdml id was created</pre>

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        start_time = _get_string_from_date(start_time)
        if stop_time == None:
            xml = str(self._client.getIds(start_time))
        else:
            stop_time = _get_string_from_date(stop_time)
            xml = str(self._client.getIds(start_time, stop_time))
        parseString(xml, self._status_handler)
        return self._parse_geometry_xml(str(xml))

    def _parse_geometry_xml(self, xml):
        """ Parser for geometry data. """
        parseString(xml, self._geometry_handler)
        return self._geometry_handler.get_data()


class _GeometryHandler(ContentHandler):

    " ContentHandler for beamline data. "

    def __init__ (self):
        ContentHandler.__init__(self)
        self._message = ""
        self._id = {}
        self._id_number = 0
        self._ids = {}

    def get_data(self):
        """
        Get a dictionary containing the parsed xml.
        
        @return the dictionary containing the parsed xml
        
        """
        return self._ids

    def startElement(self, name, attrs):
        """ Method required for ContentHandler. """
        if name == 'error':
            self._message = ""
        elif name == 'warning':
            self._message = ""
        elif name == 'ids':
            self._ids = {}
        elif name == 'id':
            self._set_id(attrs)
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
        elif name == 'id':
            self._ids[self._id_number] = self._id

    def _set_id(self, attrs):
        """ Populate a _id dictionary with data from the xml. """
        self._id = {}
        self._id_number = int(attrs.get('name', ''))
        self._id['validFrom'] = (
            _get_date_from_string(attrs.get('validFrom', '')))
        self._id['created'] = (
            _get_date_from_string(attrs.get('created', '')))

