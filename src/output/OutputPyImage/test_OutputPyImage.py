"""
Tests for OutputPyImage module.
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

# pylint: disable=C0103

import fnmatch
import json
import os
from os.path import exists
from os.path import join
import shutil
import tempfile
import unittest

from OutputPyImage import OutputPyImage

class OutputPyImageTestCase(unittest.TestCase): # pylint: disable=R0904
    """
    Test class for OutputPyImage.
    """

    @classmethod
    def setUpClass(self): # pylint: disable=C0202
        """ 
        Prepare for test by setting up worker and file extension.
        @param self Object reference.
        """
        self.__worker = OutputPyImage()
        self.here = os.getcwd()
        self.__file_extension = "outputpyimage"
        self.working_dir = os.path.expandvars(
                                        "$MAUS_ROOT_DIR/tmp/test_OutputPyImage")
        if exists(self.working_dir):
            shutil.rmtree(self.working_dir)
        os.makedirs(self.working_dir)
        os.chdir(self.working_dir)

    def setUp(self):
        """ 
        Invoke "birth" and check for success.
        @param self Object reference.
        """
        self.__tmpdir = ""
        success = self.__worker.birth("""{"image_file_prefix":"prefix"}""")
        if not success:
            raise Exception('Test setUp failed', 'worker.birth() failed')

    def test_birth_default(self):
        """ 
        Check default configuration after "birth" is called.
        @param self Object reference.
        """
        self.assertEquals(os.getcwd(), self.__worker.directory, 
            "Unexpected worker.directory")
        self.assertEquals("prefix", self.__worker.file_prefix, 
            "Unexpected worker.file_prefix")

    def test_birth_none_dir(self):
        """ 
        Check default configuration after "birth" is called.
        @param self Object reference.
        """
        worker = OutputPyImage()
        self.assertEquals(os.getcwd(), worker.directory)
        self.assertEquals(worker.directory+'/end_of_run/',
                          worker.end_of_run_directory)
        success = worker.birth(json.dumps({"image_directory":None,
                                           "end_of_run_image_directory":None}))
        self.assertTrue(success, "worker.birth() failed")
        self.assertEquals(os.getcwd(), worker.directory)
        self.assertEquals(worker.directory+'/end_of_run/',
                          worker.end_of_run_directory)

    def test_birth_bad_dir(self):
        """ 
        Test "birth" if given a file name as an image directory.
        @param self Object reference.
      . """
        worker = OutputPyImage()
        self.__tmpdir = tempfile.mkdtemp(prefix=os.getcwd())
        temp_path = os.path.join(self.__tmpdir, "somefile.txt")
        temp_file = open(temp_path, 'w')
        temp_file.write('')
        temp_file.close()
        with self.assertRaisesRegexp(ValueError,
            ".*image_directory is a file.*"):
            worker.birth("""{"image_directory":"%s"}""" % temp_path)
            worker.save(json.dumps({"maus_event_type":"Image", "image_list": [{
                "description":"Description", 
                "tag": "_tdcadc", 
                "image_type": self.__file_extension, 
                "data": "Data"}]}))

    def test_birth_abs_dir(self):
        """ 
        Test "birth" if given an absolute path to a directory as 
        an image directory.
        @param self Object reference.
      . """
        worker = OutputPyImage()
        self.__tmpdir = tempfile.mkdtemp()
        relative_path = join('a', 'b', 'c', 'd')
        abs_path = join(self.__tmpdir, relative_path)
        abs_path_end = join(abs_path, 'e', 'f')
        self.assertTrue(not exists(abs_path), "Directory already exists")
        success = worker.birth(json.dumps({"image_directory":abs_path,
                                    "end_of_run_image_directory":abs_path_end}))
        self.assertTrue(success, "worker.birth() failed")
        worker.save(json.dumps({"maus_event_type":"Image", "image_list": [{
            "description":"Description", 
            "tag": "_tdcadc", 
            "image_type": self.__file_extension, 
            "data": "Data"}]}))
        self.assertTrue(exists(abs_path))
        self.assertEquals(abs_path, worker.directory)
        worker.save(json.dumps({"maus_event_type":"RunFooter",
                               "run_number":1111}))
        self.assertTrue(exists(abs_path_end))
        self.assertEquals(abs_path_end, worker.end_of_run_directory)

    def test_birth_relative_dir(self):
        """ 
        Test "birth" if given a relative path to a directory as 
        an image directory.
        @param self Object reference.
      . """
        worker = OutputPyImage()
        relative_path = "test_output_py_image"
        self.__tmpdir = join(os.getcwd(), relative_path)
        self.assertTrue(not exists(self.__tmpdir), "Directory already exists")
        success = worker.birth("""{"image_directory":"%s"}""" % relative_path)
        self.assertTrue(success, "worker.birth() failed")
        worker.save(json.dumps({"maus_event_type":"Image", "image_list":[{
            "description":"Description", 
            "tag": "_tdcadc", 
            "image_type": self.__file_extension, 
            "data": "Data"}]}))
        self.assertTrue(exists(self.__tmpdir), "Directory does not exist")
        self.assertEquals(relative_path, worker.directory, 
            "Unexpected worker.image_directory")

    def test_save_no_images(self):
        """ 
        Test "save" with a JSON document with no "image".
        @param self Object reference.
        """
        self.__save({'maus_event_type':'Image', "image_list":[]})


    def test_save_no_event_type(self):
        """ 
        Test "save" with a JSON document with no "maus_event_type".
        @param self Object reference.
        """
        try:
            self.__save({})
            self.assertTrue(False)
        except KeyError:
            pass

    def test_save_image_no_tag(self):
        """ 
        Test "save" with a JSON document with no "tag".
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*Missing tag.*"):
            self.__save({"maus_event_type":"Image", "image_list": [{
                "description":"Description", 
                "image_type": self.__file_extension, 
                "data": "Data"}]})

    def test_save_image_no_type(self):
        """ 
        Test "save" with a JSON document with no "image_type".
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*Missing image_type.*"):
            self.__save({"maus_event_type":"Image", "image_list": [{
                "description":"Description", 
                "tag": "tdcadc", 
                "data": "Data"}]})

    def test_save_image_no_data(self):
        """ 
        Test "save" with a JSON document with no "image" "data".
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*Missing data.*"):
            self.__save({"maus_event_type":"Image", "image_list": [{
                "description":"Description", 
                "tag": "tdcadc", 
                "image_type": self.__file_extension}]})

    def test_save_images(self):
        """ 
        Test "save" with 3 JSON documents each an "image".
        @param self Object reference.
        """
        json_doc = {"maus_event_type":"Image", "image_list": [{
            "description":"Description", 
            "tag": "tdcadc%d" % i, 
            "image_type": self.__file_extension, 
            "data": "Data"} for i in range(3)]}
        self.__worker.save(json.dumps(json_doc))
        # Check output files...
        self.__check_result(3, join(os.getcwd(), self.__worker.directory))

    def test_save_end_of_run(self):
        """ 
        Test "save" with 3 JSON documents each an "image".
        @param self Object reference.
        """
        json_doc = {"maus_event_type":"Image", "image_list": [{
            "description":"Description", 
            "tag": "tdcadc%d" % i, 
            "image_type": self.__file_extension, 
            "data": "Data"} for i in range(3)]}
        self.__worker.save(json.dumps(json_doc))
        run_footer = json.dumps({"maus_event_type":"RunFooter",
                                 "run_number":1111})
        self.__worker.save(run_footer)
        self.__check_result(3, join(os.getcwd(),
                            self.__worker.end_of_run_directory, '1111'))

    def __save(self, json_doc):
        """
        Convert given JSON document to a string and pass to "save".
        @param self Object reference.
        @param json_doc JSON document.
        @returns JSON document string from "save".
        """
        json_str = json.dumps(json_doc)
        self.__worker.save(json_str)

    def __check_result(self, number_of_files, directory):
        """ 
        Validate results from "save". Check that the image directory
        has the expected number of files, each with the expected
        prefix and extension.
        @param self Object reference.
        @param number_of_files Number of files to check for.
        """
        image_files = []
        json_files = []
        for afile in os.listdir(directory):
            if fnmatch.fnmatch(afile, '*.%s' % self.__file_extension):
                image_files.append(afile)
            if fnmatch.fnmatch(afile, '*.json'):
                json_files.append(afile)
        image_files = sorted(image_files)
        json_files = sorted(json_files)
        self.assertEquals(number_of_files, len(image_files), 
            "Unexpected number of image files %i/%i"% (len(image_files),
                                                               number_of_files))
        self.assertEquals(number_of_files, len(json_files), 
            "Unexpected number of JSON files")
        for i in range(0, number_of_files):
            self.assertEquals(image_files[i], "prefixtdcadc%d.%s" % 
                                        (i, self.__file_extension))
        for i in range(0, number_of_files):
            self.assertEquals(json_files[i], "prefixtdcadc%d.json" % i)
            json_file = open(json_files[i])
            try:
                content = json_file.read()
                json.loads(content) 
            except: # pylint: disable=W0702
                self.fail("Invalid JSON for file %s: %s" \
                    % (json_files[i], content))

    def tearDown(self):
        """
        Invoke "death" and remove any temporary directories and image
        files. 
        @param self Object reference.
        """
        success = self.__worker.death()
        if not success:
            raise Exception('Test setUp failed', 'worker.death() failed')

    @classmethod
    def tearDownClass(self): # pylint: disable=C0202
        """ 
        Set the worker to None. 
        @param self Object reference.
        """
        self.__worker = None
        os.chdir(self.here)

if __name__ == '__main__':
    unittest.main()
