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

import base64
import io
import json
import os
import uuid

class OutputPyImage:
    """
    OutputPyImage saves image files held in a set of JSON documents.

    The input is a sequence of JSON documents separated by line
    breaks e.g.:

    @verbatim
    {"images": [{"content":"TDC and ADC counts for spill 2",
                 "tag": "spill",
                 "image_type": "eps", 
                 "data": "...base 64 encoded image..."},
                {"content":"Total TDC and ADC counts to spill 2",
                 "tag": "spills",
                 "image_type": "eps", 
                 "data": "...base 64 encoded image..."}]}
    @endverbatim

    If there are no such entries in a document then it skips to the
    next.

    The caller can configure the worker and specify:

    -File prefix ("image_file_prefix"). Default: auto-generated
     UUID. 
    -Directory for files ("image_directory"). Default: current
     working directory. If the given directory does not exist it
     will be created. This can be absolute or relative.

    Each image is saved in the directory in a file named using the
    concatenation of the image file prefix and "tag". This class
    does not ensure that the file names are unique - it's up to
    the input provider to ensure that the tags are unique.
    """

    def __init__(self):
        self.file_prefix = ""
        self.directory = os.getcwd()

    def birth(self, config_json):
        """
        Configure worker from data cards. An OSError is thrown if
        there are any problems in creating the directory. A ValueError
        is thrown if the directory is a file.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True
        """
        config_doc = json.loads(config_json)

        key = "image_file_prefix"
        if key in config_doc:
            self.file_prefix = config_doc[key]
        else:
            self.file_prefix = uuid.uuid4()

        key = "image_directory"
        if key in config_doc:
            directory = config_doc[key]
            if not os.path.exists(directory):
                os.makedirs(directory) 
            elif (not os.path.isdir(directory)):
                raise ValueError("image_directory is a file: %s" % 
                    directory)
            self.directory = directory
        else:
            self.directory = os.getcwd()

        return True

    def save(self, document):
        """
        Save image files held in a set of JSON documents. A ValueError
        is thrown if a document is found that is not valid JSON or
        that has missing "data", "tag" or "image_type" entries.
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
                        if ((not "tag" in entry) or (entry["tag"] == "")):
                            raise ValueError("Missing tag in %s")
                        if ((not "image_type" in entry) or 
                            (entry["image_type"] == "")):
                            raise ValueError("Missing image_type in %s")
                        if (not "data" in entry):
                            raise ValueError("Missing data in %s")
                        file_path = self.__get_file_path(entry["tag"], 
                                                         entry["image_type"])
                        data_file = open(file_path, "w")
                        decoded_data = base64.b64decode(entry["data"])
                        data_file.write(decoded_data)
                        data_file.close()
            next_value = document_file.readline()

    def death(self): #pylint: disable=R0201
        """
        A no-op
        @returns True
        """
        return True

    def __get_file_path(self, tag, image_type):
        """
        Get file path, derived from the directory, file name prefix,
        tag and using image_type as the file extension.
        @param self Object reference.
        @param tag File name tag.
        @param image_type Image type.
        @returns file path.
        """
        file_name = "%s%s.%s" % (
            self.file_prefix, 
            tag,
            image_type)
        file_path = os.path.join(self.directory, file_name)
        return file_path
