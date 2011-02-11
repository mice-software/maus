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


## @class OutputPyDoNothing.OutputPyDoNothing
#  OutputPyDoNothing doesn't save output
#
#  OutputPyDoNothing doesn't actually save anything, 
#  which is useful for running forever in the control
#  room and also for load tests.
#
#  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk> 

class OutputPyDoNothing:
    ## Birth does nothing; no setup required
    def Birth(self):
        pass

    ## Save single event
    #
    #  This is called once per time an event needs
    #  to be written.
    #
    # \param document JSON document to be saved
    #
    def Save(self, document):
        pass

    ## Closes down OutputPyDoNothing
    #
    # Closes the file that the class has open
    def Death(self):
        pass  #  do nothing


