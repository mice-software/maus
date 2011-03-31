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


class OutputPyDoNothing:
    """OutputPyDoNothing doesn't save output

    OutputPyDoNothing doesn't actually save anything,
    which is useful for running forever in the control
    room and also for load tests.

    """

    def birth(self):
        """Birth does nothing; no setup required
        """
        pass # do nothing

    def save(self, document):
        """Save single event to nowhere

        This is called once per time an event wants
        to be written.  But this just tosses the
        event away in this case.

        \param document JSON document to be saved
        """
        pass # do nothing

    def death(self):
        """Closes down OutputPyDoNothing

        Does nothing.
        """
        pass  #  do nothing


