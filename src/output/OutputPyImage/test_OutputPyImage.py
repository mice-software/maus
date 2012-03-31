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
        self.__file_extension = "outputpyimage"

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
        success = worker.birth("""{"image_directory":null}""")
        self.assertTrue(success, "worker.birth() failed")
        self.assertEquals(os.getcwd(), worker.directory, 
            "Unexpected worker.directory")

    def test_birth_bad_dir(self):
        """ 
        Test "birth" if given a file name as an image directory.
        @param self Object reference.
      . """
        worker = OutputPyImage()
        self.__tmpdir = tempfile.mkdtemp()
        temp_path = os.path.join(self.__tmpdir, "somefile.txt")
        temp_file = open(temp_path, 'w')
        temp_file.write('')
        temp_file.close()
        with self.assertRaisesRegexp(ValueError,
            ".*image_directory is a file.*"):
            worker.birth("""{"image_directory":"%s"}""" % temp_path)

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
        self.assertTrue(not exists(abs_path), "Directory already exists")
        success = worker.birth("""{"image_directory":"%s"}""" % abs_path)
        self.assertTrue(success, "worker.birth() failed")
        self.assertTrue(exists(abs_path), "Directory does not exist")
        self.assertEquals(abs_path, worker.directory, 
            "Unexpected worker.image_directory")

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
        self.assertTrue(exists(self.__tmpdir), "Directory does not exist")
        self.assertEquals(relative_path, worker.directory, 
            "Unexpected worker.image_directory")

    def test_save_no_images(self):
        """ 
        Test "save" with a JSON document with no "image".
        @param self Object reference.
        """
        self.__save({})

    def test_save_image_no_tag(self):
        """ 
        Test "save" with a JSON document with no "tag".
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*Missing tag.*"):
            self.__save({"image": {
                "description":"Description", 
                "image_type": self.__file_extension, 
                "data": "Data"}})

    def test_save_image_no_type(self):
        """ 
        Test "save" with a JSON document with no "image_type".
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*Missing image_type.*"):
            self.__save({"image": {
                "description":"Description", 
                "tag": "tdcadc", 
                "data": "Data"}})

    def test_save_image_no_data(self):
        """ 
        Test "save" with a JSON document with no "image" "data".
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*Missing data.*"):
            self.__save({"image": {
                "description":"Description", 
                "tag": "tdcadc", 
                "image_type": self.__file_extension}})

    def test_save_images(self):
        """ 
        Test "save" with 3 JSON documents each an "image".
        @param self Object reference.
        """
        json_str = ""
        for i in range(0, 3):
            json_doc = {"image": {
                "description":"Description", 
                "tag": "tdcadc%d" % i, 
                "image_type": self.__file_extension, 
                "data": "Data"}}
            json_str = "%s\n%s" % (json.dumps(json_doc), json_str)
        self.__worker.save(json_str)
        # Check output files...
        self.__check_result(3)

    def __save(self, json_doc):
        """
        Convert given JSON document to a string and pass to "save".
        @param self Object reference.
        @param json_doc JSON document.
        @returns JSON document string from "save".
        """
        json_str = json.dumps(json_doc)
        self.__worker.save(json_str)

    def __check_result(self, number_of_files):
        """ 
        Validate results from "save". Check that the image directory
        has the expected number of files, each with the expected
        prefix and extension.
        @param self Object reference.
        @param number_of_files Number of files to check for.
        """
        directory = join(os.getcwd(), self.__worker.directory)
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
            "Unexpected number of image files")
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
        # Clean up any temporary directories and files.
        if (self.__tmpdir != ""):
            shutil.rmtree(self.__tmpdir)
        for afile in os.listdir(os.getcwd()):
            if fnmatch.fnmatch(afile, '*.%s' % self.__file_extension):
                os.remove(afile)
            if fnmatch.fnmatch(afile, '*.json'):
                os.remove(afile)

    @classmethod
    def tearDownClass(self): # pylint: disable=C0202
        """ 
        Set the worker to None. 
        @param self Object reference.
        """
        self.__worker = None

if __name__ == '__main__':
    unittest.main()
