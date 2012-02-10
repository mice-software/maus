"Base module for cdb."
#pylint: disable-msg=R0201

import time
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

import cdb

__all__ = []


class _CdbBase(object):

    "Base class for cdb."

    def __init__(self, url, wsdl_dir):
        if url == "":
            _cdb_wsdl = _get_cdb_server_name() + wsdl_dir
        else:
            _cdb_wsdl = url + wsdl_dir
        try:
            self._client = Client(_cdb_wsdl).service
            self._client.getStatus()
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
            self._client.getStatus()
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
        try:
            xml = str(self._client.getStatus())
        except (URLError, TransportError):
            raise CdbTemporaryError("Unable to contact CDB server")
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()
    
    def get_name(self):
        """
Get the name of the server.

@return a string containing the name of the server

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: An unexpected internal error

        """
        try:
            xml = str(self._client.getName())
        except (URLError, TransportError):
            raise CdbTemporaryError("Unable to contact CDB server")
        parseString(xml, self._status_handler)
        return self._status_handler.get_message()
    
    def get_version(self):
        """
Get the version number of the API.

@return a string containing the version number of the API

        """
        return cdb.__version__


class _StatusHandler(ContentHandler):

    " ContentHandler for status data. "

    def __init__ (self):
        ContentHandler.__init__(self)
        self.message = ""
        
    def get_message(self):
        """ Get the message extracted from the xml. """
        return self.message

    def startElement(self, name, attrs): #pylint: disable-msg=C0103
        """ Method required for ContentHandler. """
        if name == 'error':
            self.message = ""
        elif name == 'warning':
            self.message = ""
        elif name == 'ok':
            self.message = ""
        elif name == 'serverName':
            self.message = ""
        return

    def characters(self, message):
        """ Method required for ContentHandler. """
        self.message = self.message + message

    def endElement(self, name): #pylint: disable-msg=C0103
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
    _default_cdb_server = "http://cdb.mice.rl.ac.uk"
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
    return _cdb_server

def _get_date_from_string(timestamp):
    """ Convert from a string to a python datetime. """
    _dt = None
    if timestamp == 'null':
        return _dt
    
    # Python 2.4 is still in use in the control room
    # TODO remove when control room updates python
    
    if hasattr(datetime, 'strptime'):
        #python 2.6
        strptime = datetime.strptime
    else:
        #python 2.4 equivalent
        strptime = (lambda date_string, format:
                    datetime(*(time.strptime(date_string, format)[0:6])))

    try:
        _dt = strptime(timestamp, "%Y-%m-%d %H:%M:%S.%f")
    except ValueError:
        #python 2.6 thown if no %f
        try:
            _dt = strptime(timestamp, "%Y-%m-%d %H:%M:%S")
        except ValueError, exception:
            raise CdbPermanentError(
                "ERROR - timestamp should be in the format \
                yyyy-mm-dd hh:mm:ss[.fffffffff]", exception) 
    except KeyError:
        #python 2.4 does not know about %f
        try:
            _dt = strptime(timestamp, "%Y-%m-%d %H:%M:%S")
        except ValueError, exception:
            raise CdbPermanentError(
                "ERROR - timestamp should be in the format \
                yyyy-mm-dd hh:mm:ss[.fffffffff]", exception) 

# TODO END OF HACK
    
#    try:
#        _dt = datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S.%f")
#    except ValueError:
#        try:
#            _dt = datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S")
#        except ValueError, exception:
#            raise CdbPermanentError(
#                "ERROR - timestamp should be in the format \
#                yyyy-mm-dd hh:mm:ss[.fffffffff]", exception)
            
    return _dt

def _get_string_from_date(timestamp):
    """ Convert from a python datetime to a string. """
    try:
        _dt = datetime.strftime(timestamp, "%Y-%m-%d %H:%M:%S.%f")
    except TypeError:
        # be kind and see if time stamp was already a string representation of a
        # datetime
        _get_date_from_string(timestamp)
        return timestamp
    return _dt

