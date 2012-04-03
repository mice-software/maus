"""
OutputPyFile saves JSON documents into a text file.
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

import ErrorHandler

class OutputPyFile:
    """
    OutputPyFile saves JSON documents into a text file. Each successive
    document overwrites the previous one, unless output_file_auto_number
    is set to True.

    The caller can configure the worker and specify:

    -File name ("output_file_prefix") Prefix for output files. This can
     be an absolute or relative path. Default "output_file" which
     yields an output file "output_file.json".
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
        """
        self._file_prefix = "output_file"
        self._auto_number = -1

    def birth(self, config_json):
        """
        Configure worker from data cards.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True if configuration succeeded. 
        """
        config_doc = json.loads(config_json)
        key = "output_file_prefix"
        if key in config_doc:
            self._file_prefix = config_doc["output_file_prefix"]
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
            file_name = "%s%06d.json" \
                % (self._file_prefix, self._auto_number)
            self._auto_number += 1
        else:
            file_name = "%s.json" % self._file_prefix
        try:
            json_file = open(file_name, 'w')
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
