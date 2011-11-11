"""
Test gdml uploader and downloader. Note that I switch from cdb library to cdb
mock at run time so that we don't have any messy network calls in unit tests.
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
import geometry.GDMLtoCDB as GDMLtocdb
import test_cdb_mockup as test_cdb_mockup
GDMLtocdb.cdb = test_cdb_mockup # use cdb mockup library

class  TestUploader(unittest.TestCase): #pylint: disable = C0103, R0904
    """
    TestUploader
    
    This class tests the uploading class
    (GDMLtocdb) of GDMLtoCDB.
    """

    def setUp(self): #pylint: disable = C0103
        """
        TestUploader::set_up
        
        This method sets up GDMLtoCDB objects 
        ready for testing.
        """
        self.constructor = None
        path = '/tests/py_unit/test_geometry/testCases/'
        self.testcase = os.environ['MAUS_ROOT_DIR'] + path
        self.testgeom = self.testcase+'testGeometry'
        self.testnote = 'This is the unit test'
        self.test_gdml_to_cdb = \
                               GDMLtocdb.Uploader(self.testgeom, self.testnote)
        
    def test_is_filetype(self):
        """
        TestUploader::test_is_filetype
        """
        self.assertTrue(GDMLtocdb.is_filetype('bob.zip', ['zip']))
        self.assertFalse(GDMLtocdb.is_filetype('bobzip', ['zip']))
        self.assertFalse(GDMLtocdb.is_filetype('bob.zipa', ['zip']))
        self.assertFalse(GDMLtocdb.is_filetype('bob.azip', ['zip']))
        self.assertFalse(GDMLtocdb.is_filetype('zip', ['zip']))
        self.assertTrue(GDMLtocdb.is_filetype('bob.zip', ['fred', 'zip']))
        

    def test_constructor(self):
        """
        TestUploader::test_constructor
        
        This method tests the constructor by inputting
        invalid arguments and tests whether errors are 
        raised.
        """
        try:
            err = 'this is not an int'
            self.constructor = \
                          GDMLtocdb.Uploader(self.testgeom, self.testnote, err)
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702
        
        try:
            self.constructor = GDMLtocdb.Uploader(self.testgeom, 2, 1)
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702
                  
        try:
            self.constructor = GDMLtocdb.Uploader(2, self.testnote, 1)
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702
        
    def test_set_up_server(self):
        """
        TestUploader::test_set_upserver
        
        This method ensures the server is set up.
        """
        wsdl_url = self.test_gdml_to_cdb.set_up_server()
        wsdl = 'http://cdb.mice.rl.ac.uk/cdb/geometrySuperMouse?wsdl'
        self.assertEqual(wsdl_url, wsdl)
            
    def test_check_file_list(self):
        """
        TestUploader::test_check_file_list
        
        This method ensures an error is raised when
        an invalid argument is passed.
        """
        test_file_list = self.testcase+'/testGDMLtoCDB'
        try:
            self.constructor = \
                            GDMLtocdb.Uploader(test_file_list, self.testnote, 1)
            self.assertTrue(False, 'should have raised an error')
        except:
            pass #pylint: disable = W0702
            
    def test_create_file_list(self):
        """
        TestUploader::test_create_file_list
        
        This method ensures that a file list is created
        when this method is called. The file list is the text
        file which lists the names of the files to be
        uploaded. In this case it is the test geometry.
        """
        test_create_file_list = self.testcase+'/testCreateFileList'
        self.constructor = GDMLtocdb.Uploader(test_create_file_list, \
                                                                  self.testnote)
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
        TestUploader::test_upload_to_cdb
        
        This method tests the upload_to_cdb method
        and check to make sure the test geometry
        is uploaded. 
        """
        test_upload = self.testcase+'/testUpload.not_zip'
        try:
            self.test_gdml_to_cdb.upload_to_cdb(test_upload)
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702
        
        path = self.testcase+'/testGeometry.zip'
        self.test_gdml_to_cdb.upload_to_cdb(path)
        response = self.test_gdml_to_cdb.server_status
        self.assertEqual(response, "some_text")
            
    def tearDown(self): #pylint: disable = C0103
        """
        TestUploader::tearDown
        
        This method removes the FileList.txt which
        is created in setUp. This file causes problems for
        other tests.
        """
        pass
            
    
class TestDownloader(unittest.TestCase): #pylint: disable = R0904
    """
    class test_downloader
    
    This class tests the Downloader class from
    GDMLtoCDB.py.
    """
    def set_up(self):
        """
        TestDownloader::set_up
        
        This method creates a Downloader object
        ready for testing.
        """
        self.constructor = None
        self.test_downloader = GDMLtocdb.Downloader(1) #pylint: disable = W0201
        
    def test_constructor(self):
        """
        TestDownloader::test_constructor
        
        This method test the constructor by passing
        invalid arguments and seeing if errors are raised 
        as they should be. 
        """
        try:
            self.constructor = GDMLtocdb.Downloader('no int') #pylint: disable = W0201, C0301
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702
            
    def test_download_current(self):
        """
        TestDownloader::test_download_current
        
        This method checks to see if errors are raised 
        when false arguments are entered.
        """
        try:
            self.test_downloader.download_current('Not a Path')
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702
        
    def test_download_geometry_for_id(self):
        """
        TestDownloader::test_download_geomtry_for_id
        
        This method checks to see if errors are raised 
        when false arguments are entered.
        """
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
        TestDownloader::test_get_ids
        
        This method calls this method 
        on the testserver and checks that 
        the correct id num is returned.
        """
        result = GDMLtocdb.Downloader().get_ids("2011-09-08 09:00:00", \
                                         "2011-09-09 09:00:00")
        self.assertEqual(result, "3004")
            
    def test_download_beamline_for_run(self):
        """
        TestDownloader::test_beamline_geomtry_for_run
        
        This method checks to see if errors are raised 
        when false arguments are entered.
        """
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
