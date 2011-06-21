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

import gzip

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

    def __init__(self, arg_file = gzip.GzipFile("mausput.gz", 'w')):
        """OutputPyJSON constructor

        \param arg_file arg_file is a file object.
        The default is to write uncompressed
        to the filename 'mausput'.
        """
        assert not isinstance(arg_file, str)
        self.file = arg_file


    def birth(self, config_document):
        """Birth does nothing; no setup required
        """
        return True

    def save(self, document):
        """Save single event

        This is called once per time an event needs
        to be written.

        \param document JSON document to be saved
        """
        self.file.write(document + '\n')

    def death(self):
        """Closes down OutputPyJSON
        
        Closes the file that the class has open 
        """
        self.file.close()
        return True


