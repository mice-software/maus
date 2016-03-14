"""
CDB mock up classes - for mocking the python CDB interface (i.e. without
requiring network access for unit tests, and able to set the CDB to be offline,
online, etc at will).

Note that I have only implemented those classes required for testing geometry so
far. Feel free to extend (or improve the mocking of existing stuff as required)
"""
import sys
import datetime

from urlparse import urlparse
from cdb._exceptions import CdbPermanentError

# This controls whether the CDB is okay. Should be either None (meaning CDB is
# okay), CdbPermanentError (meaning some insoluble problem like malformed URL)
# or CdbTemporaryError (meaning the server is slow to respond/down)
CDB_STATUS = None # none, CdbPermanentError, CdbTemporaryError

class Beamline(): #pylint: disable = R0903
    """
    Mock beamline - no functions defined yet
    """
    def __init__(self, wsdl_dir="/cdb/beamline?wsdl"): #pylint: disable = W0613
        """Raise a permanent error if CDB_STATUS is not None"""
        if CDB_STATUS != None:
            raise CdbPermanentError("error") #pylint: disable = E0702

    def get_beamline_for_run(self, run_number):#pylint: disable =R0201,W0613
        """Raise CDB_Status if CDB_STATUS is not None. Else return toy dict"""
        if CDB_STATUS != None:
            raise CDB_STATUS#pylint: disable =E0702
        return {run_number : Beamline.bl_dict}

    def get_beamline_for_run_xml(self, run_number):#pylint: disable =R0201,W0613
        """Raise CDB_Status if CDB_STATUS is not None. Else return toy xml"""
        if CDB_STATUS != None:
            raise CDB_STATUS#pylint: disable =E0702
        return Beamline.bl_xml

    bl_xml = """
    <test>
    <test1>bob</test1>
    <test2>fred</test2>
    </test>
    """

    bl_dict = {"test1":"bob", "test2":"fred", "start_time":2010, "end_time":2020}

class Geometry():
    """Mock up of config db geometry downloader"""

    def __init__(self, wsdl_dir="/cdb/geometry?wsdl"): #pylint: disable = W0613
        """Raise a permanent error if CDB_STATUS is not None"""
        if CDB_STATUS != None:
            raise CdbPermanentError("error") #pylint: disable = E0702

    def set_url(self, url): #pylint: disable = R0201
        """Inherits from CdbBase"""
        if CDB_STATUS != None:
            raise CDB_STATUS #pylint: disable = E0702
        try:
            urlparse(url)
        except:
            sys.excepthook(*sys.exc_info())
            raise CdbPermanentError("malformed url")

    def get_status(self): #pylint: disable = R0201
        """Return a string with status of cdb"""
        if CDB_STATUS != None:
            raise CDB_STATUS #pylint: disable = E0702
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

    def get_ids(self, start_time, stop_time=None): #pylint: disable = R0201, W0613, C0301
        """
        Return a dict of key:value pairs like run_ids:run_tag, where run_tag is
        a dict in itself with format specified in cdb.Geometry() proper.

        Raise CDB_STATUS if not None
        """
        if CDB_STATUS != None:
            raise CDB_STATUS #pylint: disable = E0702
        return {
          1002:{"validFrom":datetime.datetime.min,
                "created":datetime.datetime.today(), "notes":"1"},
          2003:{"validFrom":datetime.datetime.today(),
                "created":datetime.datetime.min, "notes":"2"},
          3004:{"validFrom":datetime.datetime.max,
                "created":datetime.datetime.max, "notes":"3"},
        }        

    def __get_gdml(self, some_value=""): #pylint: disable = R0201
        """
        Return a string like 'gdml_data repr(some_value) or raise CDB_STATUS if
        not None
        """
        if CDB_STATUS != None:
            raise CDB_STATUS #pylint: disable = E0702
        return "gdml_data "+repr(some_value)

class GeometrySuperMouse(Geometry):
    """
    Mock up of config db geometry uploader. Inherit from Geometry so that we
    get the interface right. 
    """
    def set_gdml(self, gdml, valid_from_time, notes=""): #pylint: disable = R0201, W0613, C0301
        """Raise CDB_STATUS"""
        if CDB_STATUS != None:
            raise CDB_STATUS #pylint: disable = E0702
        return "some_text"

