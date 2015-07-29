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

"""Module to write output to disk
"""

import gzip
import json
import maus_cpp.converter

import ErrorHandler

class OutputPyJSON:
    """OutputPyJSON writes JSON files to python files

    This class outputs to a python file object that
    is passed as the only argument 'arg_file' to the
    constructor.  This allows the class to output
    to any Python file object:
    - an uncompressed ASCII file where each line
    represents an event.  One can use the
    command 'python -mjson.tool' to view the
    events in a more human-readable fashion. For
    example 'cat filename | python -mjson.tool';
    arg_file=open('filename', 'w')
    - a gzip-compressed file that can be
    decompressed either with InputPyJSON within
    MAUS or by the Linux tools gunzip/gzip;
    arg_file=GzipFile('filename', 'wb')
    see http://docs.python.org/library/gzip.html
    - a socket;
    http://docs.python.org/library/socket.html
    - etc...
    .

    """

    def __init__(self, arg_file = None):
        """OutputPyJSON constructor

        \param arg_file arg_file is a file object.
        The default is to take inputs from configuration at birth.
        """
        self.file = arg_file


    def birth(self, config_document):
        """If file is not defined, take file from config_document

        \param config_document json configuration. Reads output_json_file_type
        to define type and output_json_file_name to define file name. Possible
        types are "gzip" or "text"
        """
        try:
            if self.file == None:
                config = json.loads(config_document)
                fname = config["output_json_file_name"]
                if config["output_json_file_type"] == "gzip":
                    self.file = gzip.GzipFile(fname, 'w')
                if config["output_json_file_type"] == "text":
                    self.file = open(fname, 'w')
            return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
            return False

    def save(self, document):
        """Save single event

        This is called once per time an event needs
        to be written.

        \param document JSON document to be saved
        """
        try:
            doc_str = maus_cpp.converter.string_repr(document)
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
            return False

        try:
            self.file.write(doc_str.rstrip() + '\n')
            return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
            return False

        # remove references to data if any
        try:
            maus_cpp.converter.del_data_repr(document)
        except: #pylint: disable=W0702
            pass

    def death(self):
        """Closes down OutputPyJSON
        
        Closes the file that the class has open 
        """
        try:
            if self.file != None:
                self.file.close() # can this make an exception? Catch anyway...
            return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
            return False

