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
import json
import os
import maus_cpp.converter

class OutputPyImage:
    """
    OutputPyImage saves image files held in a set of JSON documents.

    The input is a JSON document formatted like:

    @verbatim
    { "maus_event_type":"Image",
      "image_list": [{"keywords":["TDC", "ADC", "counts"],
               "description":"Total TDC and ADC counts to spill 2",
               "tag": "tdcadc",
               "image_type": "eps", 
               "data": "...base 64 encoded image..."}]}
    @endverbatim

    If there are no such entries in a document then it skips to the
    next.

    Each image is saved in the directory in a file named using the
    concatenation of the image file prefix and "tag". This class
    does not ensure that the file names are unique - it's up to
    the input provider to ensure that the tags are unique.

    The JSON document, minus the "data" field, is also saved in the 
    same directory and with the same file name as the image, but
    with a .json extension.

    The caller can configure the worker and specify:

    -File prefix ("image_file_prefix"). Default: "image".
    -Directory for files ("image_directory"). Default: current
     working directory. If the given directory does not exist it
     will be created. This can be absolute or relative.
    """

    def __init__(self):
        self.file_prefix = "image"
        self.directory = os.getcwd()
        self.end_of_run_directory = os.getcwd()+'/end_of_run/'
        self.last_event = {'maus_event_type':'Image', 'image_list':[]}
        self.run_number = -9999

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

        key = "image_directory"
        if key in config_doc and config_doc[key] != '':
            self.directory = config_doc[key]
        else:
            self.directory = os.getcwd()

        key = "end_of_run_image_directory"
        if key in config_doc and config_doc[key] != '':
            self.end_of_run_directory = config_doc[key]
        else:
            self.end_of_run_directory = self.directory+'/end_of_run/'

        return True

    def save(self, document):
        """
        Save image files held in a set of JSON documents. A ValueError
        is thrown if a document is found that is not valid JSON or
        that has missing "data", "tag" or "image_type" entries.
        @param document List of JSON documents separated by newline.
        """
        #json_doc = json.loads(document)
        json_doc = maus_cpp.converter.json_repr(document)
        if "maus_event_type" not in json_doc.keys():
            raise KeyError("Expected maus_event_type in json_doc")
        if json_doc["maus_event_type"] == "Image":
            if "image_list" not in json_doc:
                raise KeyError("Expected image_list in json_doc")      
            if len(json_doc["image_list"]) > 0: # could search for unique tags
                self.last_event = json_doc
            for image in json_doc["image_list"]:
                self.__handle_image(image, False)
        elif json_doc["maus_event_type"] == "RunFooter":
            self.run_number = json_doc["run_number"]
            for image in self.last_event["image_list"]:
                self.__handle_image(image, True)
        else:
            print "OutputPyImage will not process maus event of type "+\
                                                str(json_doc["maus_event_type"])

    def death(self): #pylint: disable=R0201
        """
        A no-op
        @returns True
        """
        return True

    def __handle_image(self, image, _is_end_of_run):
        """
        Helper function that does the image file save to disk
        @param json_doc the json document
        @param is_end_of_run boolean that controls destination directory; if
               we are at the end of a run, file goes to a better place
        """
        if ((not "tag" in image) or (image["tag"] == "")):
            raise ValueError("Missing tag in %s" % str(image))
        if ((not "image_type" in image) or 
            (image["image_type"] == "")):
            raise ValueError("Missing image_type in %s" % str(image))
        if (not "data" in image):
            raise ValueError("Missing data in %s" % str(image))
        image_path = self.__get_file_path(image["tag"], 
                                          image["image_type"],
                                          _is_end_of_run)
        data_file = open(image_path, "w")
        decoded_data = base64.b64decode(image["data"])
        data_file.write(decoded_data)
        data_file.close()
        json_path = self.__get_file_path(image["tag"], "json",
                                         _is_end_of_run)
        json_file = open(json_path, "w")
        json_file.write(json.dumps(image) + "\n")
        json_file.close()

    def __get_file_path(self, tag, image_type, is_end_of_run):
        """
        Get file path, derived from the directory, file name prefix,
        tag and using image_type as the file extension.
        @param self Object reference.
        @param tag File name tag.
        @param image_type Image type.
        @param is_end_of_run set to True to save in end_of_run location for
               datamover to pick up.
        @returns file path.
        """
        file_name = "%s%s.%s" % (
            self.file_prefix, 
            tag,
            image_type)
        directory = self.directory
        if is_end_of_run:
            directory = self.end_of_run_directory+"/"+str(self.run_number)
        # Rogers - coincident call to makedirs from two reducers caused crash -
        # add try ... except ...
        try:
            if not os.path.exists(directory):
                os.makedirs(directory) 
            if not os.path.isdir(directory):
                raise ValueError("image_directory is a file: %s" % directory)
        except OSError:
            pass
        file_path = os.path.join(directory, file_name)
        return file_path

