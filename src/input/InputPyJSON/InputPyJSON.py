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


## @class InputPyJSON.InputPyJSON 
#  InputPyJSON reads JSON documents from files/sockets
#
#  This class inputs from a python file object that
#  is passed as the only argument 'argFile' to the
#  constructor.  This allows the class to input
#  from any Python file object:
#   - an uncompressed ASCII file where each line
#     represents an spill document.  One can use
#     the command 'python -mjson.tool' to view the
#     spills in a more human-readable fashion. For
#     example 'cat filename | python -mjson.tool';
#     <b>argFile=open('filename', 'w')</b>
#   - a gzip-compressed file that can be
#     decompressed either with InputPyJSON within
#     MAUS or by the Linux tools gunzip/gzip;
#     <b>argFile=GzipFile('filename', 'w')</b>;
#     see http://docs.python.org/library/gzip.html
#   - a socket;
#     http://docs.python.org/library/socket.html
#   - etc...
#   .
#
#  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk> 

import json

class InputPyJSON:

    ## Setup InputPyJSON
    #
    # \param argFile python file object. 
    #        <b>Default: open('mausput','r')</b>
    # \param argNumberOfEvents number of events to
    #        read in.  If -1, read forever or until
    #        the end of the file.
    #        <b>Default: -1</b>
    def __init__(self, argFile=None, argNumberOfEvents=-1):
        if argFile == None:
            argFile = open('mausput','r')
        self._file = argFile

        self._numberOfEvents = argNumberOfEvents
        self._currentEvent = 0

    ## Birth() does nothing
    def Birth(self):
        pass
        
    ## Emit JSON documents
    #
    # This is a python generator that yields
    # JSON documents until the end of the file
    # or until the maximum number of events is hit.
    def Emitter(self):
        # this returns an empty string if
        # there is nothing to read, hence
        # while loop ahead
        nextValue = self._file.readline()

        while nextValue != '':
            # remove \n and whitespace
            nextValue = nextValue.rstrip()

            # see if EOF is reached
            if not nextValue:
                self._file.close()
                return

            # check that it's a valid JSON document
            json.loads(nextValue)

            # have we gone past the number of events we wanted?
            if self._numberOfEvents >= 0 and self._currentEvent >= self._numberOfEvents:
                return

            # yield the current event (google 'python generators' if confused)
            yield nextValue
            self._currentEvent += 1
            nextValue = self._file.readline()


    ## Death() closes input file
    def Death(self):
        self._file.close()
        
