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
"""InputPyEmptyDocument.py"""

import ROOT
import libMausCpp # pylint: disable=W0611

class InputPyEmptyDocument:
    """Lets MAUS start with empty spills
    
    This class is useful for either debugging or for simulating
    spills since it loads the mostly empty JSON document into
    MAUS which is only useful if something like a Simulation
    mapper is used to create all the data.  The documents this
    yields contain only the spill number.

    """

    def __init__(self, arg_number_of_events=10000):
        """
        Setup InputPyEmptyDocument

        Runs for arg_number_of_events, then halts. If
        arg_number_of_events is -1, then runs forever.
        """
        assert isinstance(arg_number_of_events, int)
        self._number_of_events = arg_number_of_events
        self._current_spill = 0

    def birth(self, json_config = "{}"): #pylint:disable=W0613
        """
        birth() does nothing
        """
        self._current_spill = 0
        return True

    def emitter(self):
        """
        Emits JSON documents with only event number
        """
        data = ROOT.MAUS.Data() # pylint: disable=E1101
        if self._number_of_events < 0: # the infinite case
            while 1:
                #yield """{ "spill_number" : %d }""" % self._current_spill
                spill = ROOT.MAUS.Spill() # pylint: disable=E1101
                spill.SetSpillNumber(self._current_spill)
                self._current_spill = self._current_spill + 1
                #data.SetEventType("Spill")
                data.SetSpill(spill)
                yield data
        else: # the finite case
            for i in range(self._number_of_events):
                #yield """{ "spill_number" : %d }""" % i
                self._current_spill = i
                spill = ROOT.MAUS.Spill() # pylint: disable=E1101
                spill.SetSpillNumber(self._current_spill)
                #data.SetEventType("Spill")
                data.SetSpill(spill)
                yield data
    def death(self):#pylint:disable=R0201
        """
        death() does nothing
        """
        return True
