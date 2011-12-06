"""
Test uploading and downloading a simple geometry to/from the test server

Tests bin/utilities/upload_geometry.py and bin/utilities/download_geometry.py
"""
import datetime
import sys
import os
import unittest
import subprocess

TEST_SERVER_URL = "http://rgma19.pp.rl.ac.uk:8080"
# For some odd reason, py/cdb requires an environment to be set to control the
# CDB server url - can then be changed dynamically, but only after import
os.environ["CDB_SERVER"] = TEST_SERVER_URL

import cdb
from cdb import Geometry
from cdb import GeometrySuperMouse

UL_DIR = "/cdb/geometrySuperMouse?wsdl"
DL_DIR = "/cdb/geometry?wsdl"

def test_cdb_import():
    """
    Check the cdb imported okay - make an informative error message if it failed
    """
    try:
        geom_test = Geometry()
        geom_test.set_url(TEST_SERVER_URL+DL_DIR)
        super_geom_test = GeometrySuperMouse()
        super_geom_test.set_url(TEST_SERVER_URL+UL_DIR)
    except cdb.CdbError:
        sys.excepthook(*sys.exc_info())
        raise RuntimeError("""
    The Configuration Database test server was not available for testing
    geometry upload and download. Note that this is registered as a test fail. 
    This may be a problem with your network connection (proxy? firewall?). If
    the problem persists, please contact the MAUS developers with a copy of this 
    error message.

    Upload URL:   """+TEST_SERVER_URL+UL_DIR+"""
    Download URL: """+TEST_SERVER_URL+DL_DIR+"""
    """)

def run_uploader():
    """
    Call the uploader executable, passing relevant parameters as command line
    arguments

    @returns the uploader return code
    """
    uploader = os.environ['MAUS_ROOT_DIR']+'/bin/utilities/upload_geometry.py'
    test_cache_ul = os.path.join(os.environ['MAUS_ROOT_DIR'],
                'tests/py_unit/test_geometry/testCases/testGeometry/')
    subproc = subprocess.Popen([uploader,
      '-cdb_upload_url', TEST_SERVER_URL,
      '-geometry_upload_wsdl', UL_DIR,
      '-geometry_upload_directory', test_cache_ul, 
      '-geometry_upload_note', 'Test geometry', 
      '-geometry_upload_valid_from', str(datetime.datetime.now()), 
    ])
    subproc.wait()
    return subproc.returncode

def run_downloader():
    """
    Call the downloader executable, passing relevant parameters as command line
    arguments
    """
    downloader = \
               os.environ['MAUS_ROOT_DIR']+'/bin/utilities/download_geometry.py'
    test_cache_dl = os.path.join(os.environ['MAUS_ROOT_DIR'],
                 'tmp', 'geometry_download_test')
    if not os.path.exists(test_cache_dl):
        os.mkdir(test_cache_dl)
    if not os.path.isdir(test_cache_dl):
        raise OSError('Couldnt make a directory at '+test_cache_dl+\
                                                          ' a file is blocking')
    subproc = subprocess.Popen([downloader,
      '-cdb_download_url', TEST_SERVER_URL,
      '-geometry_download_wsdl', DL_DIR,
      '-geometry_download_directory', test_cache_dl, 
      '-geometry_download_by', 'current', 
    ])
    subproc.wait()
    return subproc.returncode

def run_simulate_mice():
    """
    Try tracking a beam through and visualising
    """
    config = os.environ['MAUS_ROOT_DIR']+\
                '/tests/integration/test_utilities/test_geometry/sim_config.py'
    subproc = subprocess.Popen([
       os.environ['MAUS_ROOT_DIR']+'/bin/simulate_mice.py',
       '-configuration_file', config
    ])
    subproc.wait()
    return subproc.returncode



class TestUploadDownload(unittest.TestCase): # pylint: disable = R0904
    """
    For each of the test cases, we first upload relevant data then try
    downloading.
    """
    def test_download_by_id(self):
        """
        Check that we can upload and download by geometry id tag
        """
        ul_return_code = run_uploader()
        self.assertEqual(ul_return_code, 0)
        dl_return_code = run_downloader()
        self.assertEqual(dl_return_code, 0)
        sim_return_code = run_simulate_mice()
        self.assertEqual(sim_return_code, 0)

    def test_download_by_run_number(self):
        """
        Check that we can upload and download by run number
        """
        pass

    def test_download_by_date_time(self):
        """
        Check that we can download the latest GDML file correctly
        """
        pass

test_cdb_import()
if __name__ == "__main__":
    unittest.main()

