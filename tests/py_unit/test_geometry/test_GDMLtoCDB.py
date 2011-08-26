import unittest
import os
from geometry.GDMLtoCDB import GDMLtocdb
from geometry.GDMLtoCDB import Downloader

class  Test_GDMLtoCDB(unittest.TestCase):
    
    def setUp(self):
        self.constructor = None
        self.testcase = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry'
        self.testnote = 'This is the unit test'
        self.test_gdml_to_cdb = GDMLtocdb(self.testcase, self.testnote, 1)
        
    def test_constructor(self):
        try:
            self.constructor = GDMLtocdb(self.testcase, self.testnote, 'this is not an int')
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass
        
        try:
            self.constructor = GDMLtocdb(self.testcase, 2, 1)
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass
                  
        try:
            self.constructor = GDMLtocdb(2, self.testnote, 1)
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass
        
    def test_set_up_server(self):
        wsdl_url = self.test_gdml_to_cdb.set_up_server()
        self.assertEqual(wsdl_url, 'http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl', 'WSDL URL not set up, test_gdmltocdb::test_set_up_server')
        #write the same for actual server
        
    def test_check_file_list(self):
        test_file_list = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGDMLtoCDB'
        try:
            self.constructor = GDMLtocdb(test_file_list, self.testnote, 1)
            self.assertTrue(False, 'should have raised an error')
        except:
            pass
        
    def test_create_file_list(self):
        test_create_file_list = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testCreateFileList'
        self.constructor = GDMLtocdb(test_create_file_list, self.testnote, 1)
        find_file = os.listdir(test_create_file_list)
        for fname in find_file:
            if fname == 'FileList.txt':
                found_file = fname
        self.assertEqual(found_file, 'FileList.txt', 'FileList.txt not created, test_GDMLtoCDB::test_create_file_list')
        path = test_create_file_list + '/FileList.txt'
        os.remove(path)
        
    def test_upload_to_cdb(self):
        test_upload = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testUpload.not_zip'
        try:
            self.test_gdml_to_cdb.upload_to_cdb(test_upload)
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass
        
        #should figure out some more test to mock suds and check calls are being made.
    
    def tearDown(self):
        self.testcase = None
        self.testnote = None
        self.test_gdml_to_cdb =None

class test_Downloader(unittest.TestCase):
    
    def setUp(self):
        self.constructor = None
        self.test_downloader = Downloader(1)
        
    def test_constructor(self):
        try:
            self.constructor = Downloader('not an int')
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass
        

if __name__ == '__main__':
    unittest.main()

