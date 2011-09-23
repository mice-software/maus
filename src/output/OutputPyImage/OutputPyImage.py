"""
OutputPyImage saves image files held in a set of JSON documents.
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

import io
import json

class OutputPyImage:
    """
    OutputPyImage saves image files held in a set of JSON documents.
    """

    def __init__(self, arg_file = None):
        """
        A no-op
        """

    def birth(self, config_document):
        """
        A no-op
        @returns True
        """
        return True

    def save(self, document):
        """
        Save image files held in a set of JSON documents.
        TODO document 
        @param document List of JSON documents separated by newline.
        """
        document_file = io.StringIO(unicode(document))
        next_value = document_file.readline()
        while next_value != "":
            next_value = next_value.rstrip()
            if next_value != "":
                json_doc = json.loads(next_value)
                if "histograms" in json_doc:
                    for entry in json_doc["histograms"]:
                        print "Saving %s to %s" \
                            % (entry["content"], entry["file_path"])
                        data_file = open(entry["file_path"], "w")
                        data_file.write(entry["data"])
                        data_file.close()
            next_value = document_file.readline()

    def death(self):
        """
        A no-op
        @returns True
        """
        return True
