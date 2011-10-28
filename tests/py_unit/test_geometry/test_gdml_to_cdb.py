"""
M.Littlefield
"""
#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
# 
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

import unittest
import os
from geometry.GDMLtoCDB import GDMLtocdb
from geometry.GDMLtoCDB import Downloader
from cdb import GeometrySuperMouse
import urllib2


class  test_gdml_to_cdb(unittest.TestCase): #pylint: disable = C0103, R0904
    """
    class test_gdml_to_cdb
    
    This class tests the uploading class
    (GDMLtocdb) of GDMLtoCDB.
    """
    def internet_on_uploader(self):
        """
        method internet_on_uploader
        
        This method checks there is an internet 
        connection as the tests require internet.
        """
        try:
            response=urllib2.urlopen('http://google.com',timeout=1)
            return True
        except urllib2.URLError as err: pass
        return False

    def setUp(self): #pylint: disable = C0103
        """
        method set_up
        
        This method sets up GDMLtoCDB objects 
        ready for testing.
        """
        if self.internet_on_uploader() == False:
            print "No connection to CDB. Can't set up"
        else:
            self.constructor = None
            path = '/src/common_py/geometry/testCases/testGeometry'
            self.testcase = os.environ['MAUS_ROOT_DIR'] + path
            self.testnote = 'This is the unit test'
            self.test_gdml_to_cdb = GDMLtocdb(self.testcase, self.testnote, 1)
        
    def test_constructor(self):
        """
        method test_constructor
        
        This method tests the constructor by inputting
        invalid arguments and tests whether errors are 
        raised.
        """
        if self.internet_on_uploader() == False:
            print "No connection to CDB. Unit test " + \
                      "test_constructor not carried out"
        else:
            try:
                err = 'this is not an int'
                self.constructor = GDMLtocdb(self.testcase, self.testnote, err)
                self.assertTrue(False, 'Should have raised an error')
            except:
                pass #pylint: disable = W0702
            
            try:
                self.constructor = GDMLtocdb(self.testcase, 2, 1)
                self.assertTrue(False, 'Should have raised an error')
            except:
                pass #pylint: disable = W0702
                      
            try:
                self.constructor = GDMLtocdb(2, self.testnote, 1)
                self.assertTrue(False, 'Should have raised an error')
            except:
                pass #pylint: disable = W0702
        
    def test_set_up_server(self):
        """
        method test_set_upserver
        
        This method ensures the server is set up.
        """
        if self.internet_on_uploader() == False:
            print "No connection to CDB. Unit test " + \
                    "test_set_up_server not carried out"
        else:
            wsdl_url = self.test_gdml_to_cdb.set_up_server()
            wsdl = 'http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl'
            err = 'WSDL URL not set up, test_gdmltocdb::test_set_up_server'
            self.assertEqual(wsdl_url, wsdl, err)
            
    def test_check_file_list(self):
        """
        method test_check_file_list
        
        This method ensures an error is raised when
        an invalid argument is passed.
        """
        if self.internet_on_uploader() == False:
            print "No connection to CDB. Unit test " + \
                  "test_check_file_list not carried out"
        else:
            path = '/src/common_py/geometry/testCases/testGDMLtoCDB'
            test_file_list = os.environ['MAUS_ROOT_DIR'] + path
            try:
                self.constructor = GDMLtocdb(test_file_list, self.testnote, 1)
                self.assertTrue(False, 'should have raised an error')
            except:
                pass #pylint: disable = W0702
            
    def test_create_file_list(self):
        """
        method test_create_file_list
        
        This method ensures that a file list is created
        when this method is called. The file list is the text
        file which lists the names of the files to be
        uploaded. In this case it is the test geometry.
        """
        if self.internet_on_uploader() == False:
            print "No connection to CDB. Unit test " + \
                 "test_create_file_list not carried out"
        else:
            path = '/src/common_py/geometry/testCases/testCreateFileList'
            test_create_file_list = os.environ['MAUS_ROOT_DIR'] + path
            self.constructor = GDMLtocdb(test_create_file_list, \
                                                self.testnote, 1)
            find_file = os.listdir(test_create_file_list)
            for fname in find_file:
                if fname == 'FileList.txt':
                    found_file = fname
            err = 'FileList.txt not created'
            self.assertEqual(found_file, 'FileList.txt', err)
            path = test_create_file_list + '/FileList.txt'
            os.remove(path)
            
    def test_upload_to_cdb(self):
        """
        method test_upload_to_cdb
        
        This method tests the upload_to_cdb method
        and check to make sure the test geometry
        is uploaded. 
        """
        if self.internet_on_uploader() == False:
            print "No connection to CDB. Unit test " + \
                    "test_upload_to_cdb not carried out"
        else:
            path = '/src/common_py/geometry/testCases/testUpload.not_zip'
            test_upload = os.environ['MAUS_ROOT_DIR'] + path
            try:
                self.test_gdml_to_cdb.upload_to_cdb(test_upload)
                self.assertTrue(False, 'Should have raised an error')
            except:
                pass #pylint: disable = W0702
            
            zfile = '/src/common_py/geometry/testCases/testGeometry.zip'
            path = os.environ['MAUS_ROOT_DIR'] + zfile
            self.test_gdml_to_cdb.upload_to_cdb(path)
            response = self.test_gdml_to_cdb.server_status
            err = "GDML not inserted into CDB"
            self.assertEqual(response, "Inserted GDML, 13648 bytes", err)
            
    def tearDown(self): #pylint: disable = C0103
        """
        method tearDown
        
        This method removes the FileList.txt which
        is created in setUp. This file causes problems for
        other tests.
        """
        if self.internet_on_uploader() == False:
            print "No tear down required"
        else:
            f_path = '/src/common_py/geometry/testCases/testGeometry/' + /
                                                            'FileList.txt'
            path = os.environ['MAUS_ROOT_DIR'] + f_path
            os.remove(path) 
            
    
class test_downloader(unittest.TestCase): #pylint: disable = C0103, R0904
    """
    class test_downloader
    
    This class tests the Downloader class from
    GDMLtoCDB.py.
    """
    def set_up(self):
        """
        method set_up
        
        This method creates a Downloader object
        ready for testing.
        """
        self.constructor = None
        self.test_downloader = Downloader(1) #pylint: disable = W0201
        
    def internet_on_downloader(self):
        try:
            response=urllib2.urlopen('http://google.com',timeout=1)
            return True
        except urllib2.URLError as err: pass
        return False
   
    def test_constructor(self):
        """
        method test_constructor
        
        This method test the constructor by passing
        invalid arguments and seeing if errors are raised 
        as they should be. 
        """
        if self.internet_on_downloader() == False:
            print "No connection to CDB. Unit test " + \
                      "test_constructor not carried out"
        else:
            try:
                self.constructor = Downloader('no int') #pylint: disable = W0201
                self.assertTrue(False, 'Should have raised an error')
            except:
                pass #pylint: disable = W0702
            
    def test_download_current(self):
        """
        method test_download_current
        
        This method checks to see if errors are raised 
        when false arguments are entered.
        """
        if self.internet_on_downloader() == False:
            print "No connection to CDB. Unit test " + \
                 "test_download_current not carried out"
        else:
            try:
                self.test_downloader.download_current('Not a Path')
                self.assertTrue(False, 'Should have raised an error')
            except:
                pass #pylint: disable = W0702
        
    def test_download_geometry_for_id(self):
        """
        method test_download_geomtry_for_id
        
        This method checks to see if errors are raised 
        when false arguments are entered.
        """
        if self.internet_on_downloader() == False:
            print "No connection to CDB. Unit test " + \
        "test_download_geometry_for_id not carried out"
        else:
            try:
                num = str(14)
                self.test_downloader.download_geometry_for_id(num, 'Not a Path')
                self.assertTrue(False, 'Should have raised an error')
            except:
                pass #pylint: disable = W0702
            try:
                num = int(14)
                maus = os.environ['MAUS_ROOT_DIR']
                self.test_downloader.download_geometry_for_id(num, maus)
                self.assertTrue(False, 'Should have raised an error')
            except:
                pass #pylint: disable = W0702
            
    def test_get_ids(self):
        """
        method test_get_ids
        
        This method calls this method 
        on the testserver and checks that 
        the correct id num is returned.
        """
        if self.internet_on_downloader() == False:
            print "No connection to CDB. Unit test " + \
                      "test_get_ids not carried out"
        else:
            result = Downloader(1).get_ids("2011-09-08 09:00:00", \
                                             "2011-09-09 09:00:00")
            err = "get_ids did not return the correct id"
            self.assertEqual(result, "383", err)
            
    def test_download_beamline_for_run(self):
        """
        method test_beamline_geomtry_for_run
        
        This method checks to see if errors are raised 
        when false arguments are entered.
        """
        if self.internet_on_downloader() == False:
            print "No connection to CDB. Unit test " + \
        "test_download_beamline_for_run not carried out"
        else:
            try:
                num = str(14)
                self.test_downloader.download_beamline_for_run \
                                            (num, 'Not a Path')
                self.assertTrue(False, 'Should have raised an error')
            except:
                pass #pylint: disable = W0702
            try:
                num = int(14)
                maus = os.environ['MAUS_ROOT_DIR']
                self.test_downloader.download_beamline_for_run(num, maus)
                self.assertTrue(False, 'Should have raised an error')
            except:
                pass #pylint: disable = W0702   
            
if __name__ == '__main__':
    unittest.main()
