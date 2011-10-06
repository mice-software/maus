"""
Test class for OutputPyImage.
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

import fnmatch
import json
import os
from os.path import exists
from os.path import join
import shutil
import tempfile
import unittest

from OutputPyImage import OutputPyImage

class OutputPyImageTestCase(unittest.TestCase): # pylint: disable=C0103, R0904

    @classmethod
    def setUpClass(self): # pylint: disable=C0103, C0202
        self.__worker = OutputPyImage()
        self.__file_extension = "outputpyimage"

    def setUp(self): # pylint: disable=C0103
        self.__tmpdir = ""
        success = self.__worker.birth("""{"image_file_prefix":"prefix"}""")
        if not success:
            raise Exception('Test setUp failed', 'worker.birth() failed')

    def test_birth_default(self):
        self.assertEquals(os.getcwd(), self.__worker.directory, 
            "Unexpected worker.directory")
        self.assertEquals("prefix", self.__worker.file_prefix, 
            "Unexpected worker.file_prefix")

    def test_birth_bad_dir(self):
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
        self.__save({})

    def test_image_missing_tag(self):
        with self.assertRaisesRegexp(ValueError,
            ".*Missing tag.*"):
            self.__save({"images": [{
                "content":"Content", 
                "image_type": self.__file_extension, 
                "data": "Data"}]})

    def test_image_missing_type(self):
        with self.assertRaisesRegexp(ValueError,
            ".*Missing image_type.*"):
            self.__save({"images": [{
                "content":"Content", 
                "tag": "spill", 
                "data": "Data"}]})

    def test_image_missing_data(self):
        with self.assertRaisesRegexp(ValueError,
            ".*Missing data.*"):
            self.__save({"images": [{
                "content":"Content", 
                "tag": "spill", 
                "image_type": self.__file_extension}]})

    def test_n_images_one_doc(self):
        for i in range(0, 3):
            self.__save({"images": [{
                "content":"Content", 
                "tag": "spill%d" % i, 
                "image_type": self.__file_extension, 
                "data": ""}]})
        self.__validate_result(3)

    def test_n_images_m_docs(self):
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
        json_str = json.dumps(json_doc)
        self.__worker.save(json_str)

    def __validate_result(self, number_of_files):
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

    def tearDown(self): # pylint: disable=C0103
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
    def tearDownClass(self): # pylint: disable=C0103, C0202
        self.__worker = None

if __name__ == '__main__':
    unittest.main()
