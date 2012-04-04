"""
OutputPyFile writes output to disk.
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

import json
import os

import ErrorHandler

class OutputPyFile:
    """
    OutputPyFile saves JSON documents into files. 

    Each successive document overwrites the previous one, unless
    output_file_auto_number is set to True.

    The caller can configure the worker and specify:

    -Directory for files ("output_file_directory"). Default: current
     working directory. If the given directory does not exist it
     will be created. This can be absolute or relative.
    -File name ("output_file_name") Name for output files. Default
     "output_file".
    -File extension ("output_file_extension") Extension for output
     files. Default "json".
    -Auto-number ("output_file_auto_number"). Default: false. Flag
     that determines if files are autonumbered. If false then
     successive documents overwrite the previous one. If true then
     output files are named "output_file000001.json", 
     "output_file000002.json" ...
    """

    def __init__(self):
        """
        Constructor.
        @param self Object reference.
        @param file_name Default file name.
        """
        self.directory = os.getcwd()
        self.file_name = "output_file"
        self.file_extension = "json"
        self._auto_number = -1

    def birth(self, config_json):
        """
        Configure worker from data cards. An OSError is thrown if
        there are any problems in creating the directory. A ValueError
        is thrown if the directory is a file.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True if configuration succeeded. 
        """
        config_doc = json.loads(config_json)
        key = "output_file_directory"
        if key in config_doc:
            directory = config_doc[key]
            if (directory == None):
                directory = os.getcwd()
            elif not os.path.exists(directory):
                os.makedirs(directory) 
            elif (not os.path.isdir(directory)):
                raise ValueError("output_file_directory is a file: %s" % 
                    directory)
            self.directory = directory
        else:
            self.directory = os.getcwd()

        key = "output_file_name"
        if key in config_doc:
            self.file_name = config_doc["output_file_name"]

        key = "output_file_extension"
        if key in config_doc:
            self.file_extension = config_doc["output_file_extension"]

        key = "output_file_auto_number"
        if key in config_doc:
            self._auto_number = 0
        else:
            self._auto_number = -1

        return True

    def save(self, document):
        """
        Save JSON document.
        @param self Object reference.
        @param document JSON document.
        """
        if (self._auto_number > -1):
            file_name = "%s%06d.%s" % \
                (self.file_name, self._auto_number, self.file_extension)
            self._auto_number += 1
        else:
            file_name = "%s.%s" % (self.file_name, self.file_extension)
        file_path = os.path.join(self.directory, file_name)
        try:
            json_file = open(file_path, 'w')
            json_file.write(document.rstrip() + '\n')
            json_file.close()
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)

    def death(self): #pylint: disable=R0201
        """
        A no-op
        @returns True
        """
        return True
