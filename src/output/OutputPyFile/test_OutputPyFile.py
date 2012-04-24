"""
Tests for OutputPyFile module.
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

from OutputPyFile import OutputPyFile

class OutputPyFileTestCase(unittest.TestCase): # pylint: disable=R0904
    """
    Test class for OutputPyFile.
    """

    def setUp(self):
        """ 
        Create a worker.
        @param self Object reference.
        """
        self.__worker = OutputPyFile()
        self.__tmpdir = ""

    def test_birth_default(self):
        """ 
        Check default configuration after "birth" is called.
        @param self Object reference.
        """
        success = self.__worker.birth("""{}""")
        if not success:
            raise Exception('Test setUp failed', 'worker.birth() failed')
        self.assertEquals(os.getcwd(), self.__worker.directory, 
            "Unexpected worker.directory")
        self.assertEquals("output_file", self.__worker.file_name, 
            "Unexpected worker.file_name")
        self.assertEquals("json", self.__worker.file_extension, 
            "Unexpected worker.file_extension")

    def test_birth_none_dir(self):
        """ 
        Test "birth" with "null" "output_file_directory".
        @param self Object reference.
        """
        success = self.__worker.birth("""{"output_file_directory":null}""")
        self.assertTrue(success, "worker.birth() failed")
        self.assertEquals(os.getcwd(), self.__worker.directory, 
            "Unexpected worker.directory")

    def test_birth_bad_dir(self):
        """ 
        Test "birth" if given a file name as a directory.
        @param self Object reference.
      . """
        self.__tmpdir = tempfile.mkdtemp()
        temp_path = os.path.join(self.__tmpdir, "somefile.txt")
        temp_file = open(temp_path, 'w')
        temp_file.write('')
        temp_file.close()
        with self.assertRaisesRegexp(ValueError,
            ".*output_file_directory is a file.*"):
            self.__worker.birth("""{"output_file_directory":"%s"}""" \
                % temp_path)

    def test_birth_abs_dir(self):
        """ 
        Test "birth" if given an absolute path to a directory as 
        a directory.
        @param self Object reference.
      . """
        self.__tmpdir = tempfile.mkdtemp()
        relative_path = join('a', 'b', 'c', 'd')
        abs_path = join(self.__tmpdir, relative_path)
        self.assertTrue(not exists(abs_path), "Directory already exists")
        success = self.__worker.birth("""{"output_file_directory":"%s"}""" \
            % abs_path)
        self.assertTrue(success, "worker.birth() failed")
        self.assertTrue(exists(abs_path), "Directory does not exist")
        self.assertEquals(abs_path, self.__worker.directory, 
            "Unexpected worker.directory")

    def test_birth_relative_dir(self):
        """ 
        Test "birth" if given a relative path to a directory as 
        a directory.
        @param self Object reference.
      . """
        relative_path = "test_output_py_file"
        self.__tmpdir = join(os.getcwd(), relative_path)
        self.assertTrue(not exists(self.__tmpdir), "Directory already exists")
        success = self.__worker.birth("""{"output_file_directory":"%s"}""" \
            % relative_path)
        self.assertTrue(success, "worker.birth() failed")
        self.assertTrue(exists(self.__tmpdir), "Directory does not exist")
        self.assertEquals(relative_path, self.__worker.directory, 
            "Unexpected worker.directory")

    def test_birth_name(self):
        """ 
        Test "birth" with "output_file_name".
        @param self Object reference.
        """
        success = \
            self.__worker.birth("""{"output_file_name":"test_name"}""")
        self.assertEquals("test_name", self.__worker.file_name, 
            "Unexpected worker.file_name")

    def test_birth_extension(self):
        """ 
        Test "birth" with "output_file_extension".
        @param self Object reference.
        """
        success = \
            self.__worker.birth("""{"output_file_extension":"test_ext"}""")
        self.assertEquals("test_ext", self.__worker.file_extension, 
            "Unexpected worker.file_extension")

    def test_save_images_auto_number(self):
        """ 
        Test "save" with 3 JSON documents using auto-numbering..
        @param self Object reference.
        """
        birth_doc = {}
        birth_doc["output_file_name"] = "test_outputpyfile_auto"
        birth_doc["output_file_extension"] = "json"
        birth_doc["output_file_auto_number"] = True
        success = self.__worker.birth(json.dumps(birth_doc))
        expected = []
        for i in range(0, 3):
            name = '%s%06d.%s' % \
                (self.__worker.file_name, i, self.__worker.file_extension)
            json_doc = {"test_output_py_file": i}
            json_str = json.dumps(json_doc)
            self.__worker.save(json_str)
            expected.append((name, json_str + "\n"))
        self.__check_result(expected)

    def test_save_images(self):
        """ 
        Test "save" with 3 JSON documents.
        @param self Object reference.
        """
        birth_doc = {}
        birth_doc["output_file_name"] = "test_outputpyfile"
        birth_doc["output_file_extension"] = "json"
        success = self.__worker.birth(json.dumps(birth_doc))
        for i in range(0, 3):
            json_doc = {"test_output_py_file": i}
            json_str = json.dumps(json_doc)
            self.__worker.save(json_str)
        # Check output file.
        name = '%s.%s' % \
            (self.__worker.file_name, self.__worker.file_extension)
        self.__check_result([(name, json_str + "\n")])

    def __save(self, json_doc):
        """
        Convert given JSON document to a string and pass to "save".
        @param self Object reference.
        @param json_doc JSON document.
        @returns JSON document string from "save".
        """
        json_str = json.dumps(json_doc)
        self.__worker.save(json_str)

    def __check_result(self, expected):
        """ 
        Validate results from "save". Check that the output directory
        has the expected file contents..
        @param self Object reference.
        @param expected List of tuples each of which contains a file name
        and the expected contents of the file.
        """
        directory = join(os.getcwd(), self.__worker.directory)
        for (name, content) in expected:
            path = join(directory, name)
            self.assertTrue(exists(path), "File %s does not exist" \
                % name)
            data_file = open(path)
            actual_content = data_file.read()
            self.assertEquals(content, actual_content, 
                "Unexpected content for %s" % path)

    def tearDown(self):
        """
        Invoke "death" and remove any temporary directories and files. 
        @param self Object reference.
        """
        # Clean up any temporary directories and files.
        if (self.__tmpdir != ""):
            shutil.rmtree(self.__tmpdir)
        pattern = '%s*.%s' % \
            (self.__worker.file_name, self.__worker.file_extension)
        for afile in os.listdir(os.getcwd()):
            if fnmatch.fnmatch(afile, '*.%s' % self.__worker.file_extension):
                os.remove(afile)
        success = self.__worker.death()
        if not success:
            raise Exception('Test setUp failed', 'worker.death() failed')

if __name__ == '__main__':
    unittest.main()
