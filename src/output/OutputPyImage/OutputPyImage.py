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
import os
import uuid

class OutputPyImage:
    """
    OutputPyImage saves image files held in a set of JSON documents.

    The input is a sequence of JSON documents separated by line
    breaks e.g.:

    {"images": [{"content":"TDC and ADC counts for spill 2",
                 "tag": "spill",
                 "image_type": "eps", 
                 "data": "..."},
                {"content":"Total TDC and ADC counts to spill 2",
                 "tag": "spills",
                 "image_type": "eps", 
                 "data": "..." }]}

    If there are no such entries in a document then it skips to the
    next.

    The caller can configure the worker and specify:

    -File prefix ("image_file_prefix"). Default: auto-generated
     UUID. 
    -Directory for files ("image_directory"). Default: current
     working directory. If the given directory does not exist it
     will be created.

    Each image is saved in the directory in a file named using the
    concatenation of the image file prefix and "tag".
    """

    def __init__(self):
        self._file_prefix = ""
        self._directory = os.getcwd()

    def birth(self, config_json):
        """
        Configure worker from data cards.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True
        """
        config_doc = json.loads(config_json)

        key = "image_file_prefix"
        if key in config_doc:
            self._file_prefix = config_doc[key]
        else:
            self._file_prefix = uuid.uuid4()
        print "Image file prefix: %s" % self._file_prefix

        key = "image_directory"
        if key in config_doc:
            self._directory = config_doc[key]
            if not os.path.exists(self._directory): 
                os.makedirs(self._directory) 
        else:
            self._directory = os.getcwd()
        print "Image directory: %s" % self._directory

        return True

    def save(self, document):
        """
        Save image files held in a set of JSON documents.
        @param document List of JSON documents separated by newline.
        """
        document_file = io.StringIO(unicode(document))
        next_value = document_file.readline()
        while next_value != "":
            next_value = next_value.rstrip()
            if next_value != "":
                json_doc = json.loads(next_value)
                if "images" in json_doc:
                    for entry in json_doc["images"]:
                        file_path = self.get_file_path(entry["tag"], 
                                                       entry["image_type"])
                        print "Saving %s to %s" \
                            % (entry["content"], file_path)
                        data_file = open(file_path, "w")
                        data_file.write(entry["data"])
                        data_file.close()
            next_value = document_file.readline()

    def death(self):
        """
        A no-op
        @returns True
        """
        return True

    def get_file_path(self, tag, image_type):
        """
        Get file path, derived from the directory, file name prefix,
        tag and using image_type as the file extension.
        @param self Object reference.
        @param tag File name tag.
        @param image_type Image type.
        @returns file path.
        """
        file_name = "%s%s.%s" % (
            self._file_prefix, 
            tag,
            image_type)
        file_path = os.path.join(self._directory, file_name)
        return file_path
