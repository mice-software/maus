import sys
import datetime

from urlparse import urlparse

from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError

"""
CDB mock up classes - for mocking the python CDB interface (i.e. without
requiring network access for unit tests, and able to set the CDB to be offline,
online, etc)
"""



# This controls whether the CDB is okay. Should be either None (meaning CDB is
# okay), CdbPermanentError (meaning some insoluble problem like malformed URL)
# or CdbTemporaryError (meaning the server is slow to respond)
CDB_STATUS = None # none, CdbPermanentError, CdbTemporaryError

class Beamline():
    def __init__(self, wsdl_dir="/cdb/beamline?wsdl"):
        """Raise a permanent error if CDB_STATUS is not None"""
        if CDB_STATUS != None:
            raise CdbPermanentError("error")

class Geometry():
    """Mock up of config db geometry downloader"""

    def __init__(self, wsdl_dir="/cdb/geometry?wsdl"):
        """Raise a permanent error if CDB_STATUS is not None"""
        if CDB_STATUS != None:
            raise CdbPermanentError("error")

    def set_url(self, url):
        """Inherits from CdbBase"""
        if CDB_STATUS != None:
            raise CDB_STATUS
        try:
            urlparse(url)
        except:
            sys.excepthook(*sys.exc_info())
            raise CdbPermanentError("malformed url")

    def get_status(self):
        """Return a string with status of cdb"""
        if CDB_STATUS != None:
            raise CDB_STATUS
        return "okay"

    def get_current_gdml(self):
        """
        Return a string like 'gdml_data ' or raise CDB_STATUS if not None
        """
        return self.__get_gdml()

    def get_gdml_for_id(self, id_):
        """
        Return a string like 'gdml_data repr(id_)' or raise CDB_STATUS if
        not None
        """
        return self.__get_gdml(id_)

    def get_gdml_for_run(self, run_number):
        """
        Return a string like 'gdml_data repr(run_number)' or raise CDB_STATUS if
        not None
        """
        return self.__get_gdml(run_number)

    def get_ids(self, start_time, stop_time=None):
        """
        Return a dict of key:value pairs like run_ids:run_tag, where run_tag is
        a dict in itself with format specified in cdb.Geometry() proper.

        Raise CDB_STATUS if not None
        """
        if CDB_STATUS != None:
            raise CDB_STATUS
        return {
          1002:{"validFrom":datetime.datetime.min,"created":datetime.datetime.today(), "notes":"1"},
          2003:{"validFrom":datetime.datetime.today(),"created":datetime.datetime.min, "notes":"2"},
          3004:{"validFrom":datetime.datetime.max,"created":datetime.datetime.max, "notes":"3"},
        }        

    def __get_gdml(self, some_value=""):
        """
        Return a string like 'gdml_data repr(some_value) or raise CDB_STATUS if
        not None
        """
        if CDB_STATUS != None:
            raise CDB_STATUS
        return "gdml_data "+repr(some_value)

class GeometrySuperMouse(Geometry):
    """
    Mock up of config db geometry uploader. Inherit from Geometry so that we
    get the interface right. 
    """
    def set_gdml(self, gdml, valid_from_time, notes=""):
        """Raise CDB_STATUS"""
        if CDB_STATUS != None:
            raise CDB_STATUS
        return "some_text"

