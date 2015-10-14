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

"""
InputPyRoot inputs data from a root file without converting it to json.
"""

import MAUS
import ROOT

import ErrorHandler

class InputPyRoot:
    """Reads Root documents from files/sockets

    This class inputs from a ROOT file.  The filename
    is passed as the argument to the constructor.

    """

    def __init__(self, arg_filename="", arg_number_of_events=-1):
        """Setup InputPyRoot

        \param arg_filename name of the root file.
        <b>Default: ""</b>
        \param arg_number_of_events number of events to
        read in.  If -1, read forever or until
        the end of the file.
        <b>Default: -1</b>
        """
        self._filename = arg_filename
        self._number_of_events = arg_number_of_events
        self._current_event = 0

    def birth(self, config_document = "{}"):
        """
        birth() opens the json file based on datacards

        @param json_config json string holding a dict of json parameters
        - input_root_file_name datacard controls file name to open
        @returns True on success, False on failure
        """
        try:
            if self._filename == "":
                config = json.loads(config_document)
                self._filename = config["input_root_file_name"]
            self._file = ROOT.TFile(self._filename)
            self._tree = self._file.Get("Spill")
            self._data = ROOT.MAUS.Data()
            self._tree.SetBranchAddress("data", self._data)

            if self._number_of_events == -1:
                self._number_of_events = self._tree.GetEntries()

            return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
            return False

    def emitter(self):
        """Emit data
        This is a python generator that yields
        ROOT.Maus.Data objects until the maximum 
        number of events is hit.
        """

        while self._current_event < self._number_of_events:
            self._tree.GetEntry(self._current_event)
            self._current_event += 1
            yield self._data
        return

    def death(self):
        """
        death() closes input file
        """
        self._file.Close()
        return True

