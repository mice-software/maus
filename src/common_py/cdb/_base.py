"Base module for cdb."

# pylint: disable=C0103,R0801

from datetime import datetime
from httplib import InvalidURL
from os import environ
from urllib2 import URLError
from xml.sax import parseString
from xml.sax.handler import ContentHandler

from suds.client import Client
from suds.client import TransportError

from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError

__all__ = []


class _CdbBase(object):

    "Base class for cdb."

    def __init__(self, wsdl_dir):
        _cdb_wsdl = _get_cdb_server_name() + wsdl_dir
        try:
            self._client = Client(_cdb_wsdl).service
        except (URLError, TransportError):
            raise CdbTemporaryError("Unable to contact CDB server at " + 
            _cdb_wsdl)
        except (ValueError, InvalidURL):
            raise CdbPermanentError("Invalid CDB server URL: " + _cdb_wsdl)
        self._status_handler = _StatusHandler()

    def set_url(self, url):
        """
Set the client to use the given CDB server.

@param url: the URL of the CDB wsdl

@exception CdbTemporaryError: Unable to contact CDB server
@exception CdbPermanentError: Invalid URL

        """
        try:
            self._client = Client(url).service
        except (URLError, TransportError):
            raise CdbTemporaryError("Unable to contact CDB server at " + url)
        except (ValueError, InvalidURL):
            raise CdbPermanentError("Invalid CDB server URL: " + url)

    def get_status(self):
        """
Get the status of the service.

@return a string containing the status of the service

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        xml = str(self._client.getStatus())
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()


class _StatusHandler(ContentHandler):

    " ContentHandler for status data. "

    def __init__ (self):
        ContentHandler.__init__(self)
        self.message = ""
        
    def get_message(self):
        """ Get the message extracted from the xml. """
        return self.message

    def startElement(self, name, attrs):
        """ Method required for ContentHandler. """
        if name == 'error':
            self.message = ""
        elif name == 'warning':
            self.message = ""
        elif name == 'ok':
            self.message = ""
        return

    def characters(self, message):
        """ Method required for ContentHandler. """
        self.message = self.message + message

    def endElement(self, name):
        """ Method required for ContentHandler. """
        if name == 'error':
            raise CdbPermanentError(self.message)
        elif name == 'warning':
            raise CdbTemporaryError(self.message)


def _get_props():
    """ Get a dictionary containing the properties from the props file. """
    try:
        _prop_file = file(r"/opt/mice/etc/cdb-client/cdb.props", "rU")
    except IOError:
        return dict()
    _prop_dict = dict()
    for _prop_line in _prop_file:
        _prop_def = _prop_line.strip()
        if len(_prop_def) == 0:
            continue
        if _prop_def[0] in ('!', '#'):
            continue
        _punctuation = [ _prop_def.find(c) for c in ':= ' ] + [ len(_prop_def) ]
        _found = min([ pos for pos in _punctuation if pos != -1 ])
        _name = _prop_def[:_found].rstrip()
        _value = _prop_def[_found:].lstrip(":= ").rstrip()
        _prop_dict[_name] = _value
    _prop_file.close()
    return _prop_dict

def _get_cdb_server_name():
    """ Get the address of the CDB server. """
    _default_cdb_server = "http://micewww.pp.rl.ac.uk:4443"
    _cdb_server = None

    try:
        _cdb_server = environ['CDB_SERVER']
    except KeyError:
        _cdb_server = None
    if _cdb_server == None:
        _props = _get_props()
        _cdb_server = _props.get("cdb.server")
    if _cdb_server == None:
        _cdb_server = _default_cdb_server
        _cdb_server = _default_cdb_server
    return _cdb_server

def _get_date_from_string(timestamp):
    """ Convert from a string to a python datetime. """
    _dt = None
    try:
        _dt = datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S.%f")
    except ValueError:
        _dt = datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S")
    return _dt

def _get_string_from_date(timestamp):
    """ Convert from a python datetime to a string. """
    try:
        _dt = datetime.strftime(timestamp, "%Y-%m-%d %H:%M:%S.%f")
    except TypeError, exception:
        raise CdbPermanentError("ERROR - timestamp should be of type datetime",
                                exception)
    return _dt

