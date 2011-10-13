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
        Invoke worker's birth method and check for success. 
        @param self Object reference.
        """
        self.__tmpdir = ""
        success = self.__worker.birth("""{"image_file_prefix":"prefix"}""")
        if not success:
            raise Exception('Test setUp failed', 'worker.birth() failed')

    def test_birth_default(self):
        """ 
        Test worker's default values after birth is called.
        @param self Object reference.
        """
        self.assertEquals(os.getcwd(), self.__worker.directory, 
            "Unexpected worker.directory")
        self.assertEquals("prefix", self.__worker.file_prefix, 
            "Unexpected worker.file_prefix")

    def test_birth_bad_dir(self):
        """ 
        Test worker's birth method throws an exception if given 
        a file name as an image directory.
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
        Test worker's birth method with an absolute directory
        path as an image directory.
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
        Test worker's birth method with a relative directory
        path as an image directory.
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

    def test_no_images(self):
        """ 
        Test worker's save method with no JSON documents. 
        @param self Object reference.
        """
        self.__save({})

    def test_image_missing_tag(self):
        """ 
        Test worker's save method with a JSON document with no images.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*Missing tag.*"):
            self.__save({"images": [{
                "content":"Content", 
                "image_type": self.__file_extension, 
                "data": "Data"}]})

    def test_image_missing_type(self):
        """ 
        Test worker's save method with a JSON document with an image
        that has no image type.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*Missing image_type.*"):
            self.__save({"images": [{
                "content":"Content", 
                "tag": "spill", 
                "data": "Data"}]})

    def test_image_missing_data(self):
        """ 
        Test worker's save method with a JSON document with an image
        that has no image data.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*Missing data.*"):
            self.__save({"images": [{
                "content":"Content", 
                "tag": "spill", 
                "image_type": self.__file_extension}]})

    def test_n_images_one_doc(self):
        """ 
        Test worker's save method with a JSON document with N
        image entries.
        @param self Object reference.
        """
        for i in range(0, 3):
            self.__save({"images": [{
                "content":"Content", 
                "tag": "spill%d" % i, 
                "image_type": self.__file_extension, 
                "data": ""}]})
        self.__validate_result(3)

    def test_n_images_m_docs(self):
        """ 
        Test worker's save method with M JSON documents each with N
        image entries.
        @param self Object reference.
        """
        json_str = ""
        for i in range(0, 3):
            json_doc = {"images": [{
                "content":"Content", 
                "tag": "spill%d" % i, 
                "image_type": self.__file_extension, 
                "data": "Data"}]}
            json_str = "%s\n%s" % (json.dumps(json_doc), json_str)
        self.__worker.save(json_str)
        self.__validate_result(3)

    def __save(self, json_doc):
        """
        Convert JSON document to string and pass to worker's save
        method.
        @param self Object reference.
        @param json_doc JSON document.
        """
        json_str = json.dumps(json_doc)
        self.__worker.save(json_str)

    def __validate_result(self, number_of_files):
        """ 
        Check that the worker's image directory has a specific
        number of files with the expected prefix and file
        extension.
        @param self Object reference.
        @param number_of_files Number of files to check for.
        """
        directory = join(os.getcwd(), self.__worker.directory)
        files = []
        for afile in os.listdir(directory):
            if fnmatch.fnmatch(afile, '*.%s' % self.__file_extension):
                files.append(afile)
        files = sorted(files)
        self.assertEquals(number_of_files, len(files), 
            "Unexpected number of files")
        for i in range(0, number_of_files):
            self.assertEquals(files[i], "prefixspill%d.%s" % 
                                        (i, self.__file_extension))

    def tearDown(self):
        """
        Invoke worker's death method and remove any temporary 
        directories and image files.
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

    @classmethod
    def tearDownClass(self): # pylint: disable=C0202
        """ 
        Set the worker to None. 
        @param self Object reference.
        """
        self.__worker = None

if __name__ == '__main__':
    unittest.main()
