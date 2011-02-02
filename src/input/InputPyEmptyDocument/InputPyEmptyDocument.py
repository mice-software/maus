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


## @class InputPyEmptyDocument.InputPyEmptyDocument
#  InputPyEmptyDocument lets MAUS start with empty spills
#
#  This class is useful for either debugging or for simulating
#  spills since it loads the mostly empty JSON document into
#  MAUS which is only useful if something like a Simulation
#  mapper is used to create all the data.  The documents this 
#  yields contain only the spill number.  
#
#  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk> 

import json

class InputPyEmptyDocument:

    ## Setup InputPyEmptyDocument
    #
    # Runs for argNumberOfEvents, then halts. If
    # argNumberOfEvents is -1, then runs forever.
    def __init__(self, argNumberOfEvents=10000):
        assert isinstance(argNumberOfEvents, int)
        self._numberOfEvents = argNumberOfEvents

    ## Birth sets spill counter to zero
    def Birth(self):
        self._currentSpill= 0

    ## Emits JSON documents with only event number
    def Emitter(self):
        if self._numberOfEvents < 0: # the infinite case 
            while 1:
                yield """{ "spill_number" : %d }""" % self._currentSpill
                self._currentSpill= self._currentSpill+ 1
        else: # the finite case 
            for i in range(self._numberOfEvents):
                yield """{ "spill_number" : %d }""" % self._currentSpill
                self._currentSpill= self._currentSpill+ 1

    ## Death() Does nothing        
    def Death(self):
        pass
