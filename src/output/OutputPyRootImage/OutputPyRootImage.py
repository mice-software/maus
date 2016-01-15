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

# ROOT false positives
# pylint: disable=E1101
# pylint: disable=R0902

"""
OutputPyRootImage saves image files held as a ROOT.MAUS.ImageData object.
"""

import json
import os

import ROOT

class OutputPyRootImage:
    """
    OutputPyRootImage saves image files held in a ROOT.MAUS.ImageData object.

    Each image is saved in the directory in a file named using the
    concatenation of the image file prefix and "tag". This class
    does not ensure that the file names are unique - it's up to
    the input provider to ensure that the tags are unique.

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
        self.process_count = 0
        self.refresh_rate = None
        self.last_event = None
        self.run_number = None
        self.spill_number = None
        self.image_types = []
        self.supported_types = \
            ["ps", "eps", "gif", "jpg", "jpeg", "pdf", "svg", "png"]

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
        if 'reduce_plot_refresh_rate' in config_json:
            self.refresh_rate = config_doc["reduce_plot_refresh_rate"]
        if 'image_file_prefix' in config_json:
            self.file_prefix = config_doc["image_file_prefix"]
        if 'image_directory' in config_json:
            self.directory = config_doc["image_directory"]
        if 'end_of_run_image_directory' in config_json:
            self.end_of_run_directory = config_doc["end_of_run_image_directory"]
        if 'image_types' in config_json:
            self.image_types = config_doc["image_types"]

    def save(self, image_data):
        """
        Save images. 
          - @param image_data: image data object. OutputPyRootImage does nothing
            if type is not ROOT.MAUS.ImageData.
        
        """
        if type(image_data) == ROOT.MAUS.ImageData:
            is_end_of_run = False
            self.last_event = image_data
            self.run_number = image_data.GetImage().GetRunNumber()
        elif type(image_data) == ROOT.MAUS.RunFooterData:
            is_end_of_run = True
        else:
            print "OutputPyRootImage ignoring event of type "+\
                                                          str(type(image_data))
            return
        image = self.last_event.GetImage()
        if image.GetSpillNumber() != self.spill_number:
            self.spill_number = image.GetSpillNumber()
            self.process_count += 1
        elif not is_end_of_run:
            return
        if (self.process_count % self.refresh_rate) and not is_end_of_run:
            return
        for canvas_wrap in image.GetCanvasWrappers():
            canvas = canvas_wrap.GetCanvas()
            file_path = self._get_file_path(canvas_wrap.GetFileTag(),
                                            is_end_of_run)
            for file_type in self.image_types:
                if file_type not in self.supported_types:
                    print """OutputPyRootImage ignoring wrong
                          image type: %s""" % file_type
                else:
                    canvas.SaveAs(file_path+file_type)

    def death(self): #pylint: disable=R0201
        """
        A no-op
        @returns None
        """
        return

    def _get_file_path(self, tag, is_end_of_run):
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
        file_name = "%s%s." % (
            self.file_prefix, 
            tag)
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

