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


## Virtual planes show when a particle crossed a z-plane
#
#  @class MapPyVirtualPlanes.MapPyVirtualPlanes
#  This class saves the state vector of a muon that crosses some imaginary plane
#  in (x,y) where the beam axis is 'z'.  This class is meant to be comparable to
#  the G4MICE VirtualPlanes application.  One *could* use tracks to do the same
#  thing, but this processor tries to do the reduction of tracks such that not
#  much room is used on the disk.  
#
import json
import types
import os

class MapPyVirtualPlanes:
    def Birth(self, argJsonConfigDocument):
        configJSON = json.loads(argJsonConfigDocument)

        if 'VirtualPlanes_z' in configJSON and\
                isinstance(configJSON['VirtualPlanes_z'], list):
            self.planes = configJSON['VirtualPlanes_z']
            return True
        return False

    def Process(self, str):
        ##
        ## Sanity checks
        ##

        # Is JSON document good?
        try:
            doc = json.loads(str)
        except ValueError:
            doc = {"errors": {"MapPyVirtualPlanes": "unable to do json.loads on input"} }
            return json.dumps(doc)

        # Is there MC data?
        if "mc" not in doc:
            if 'errors' not in doc:
                doc['errors'] = {}
            doc['errors']['MapPyVirtualPlanes'] = 'Missing MC branch'

            return json.dumps(doc)

        assert isinstance(doc["mc"], list)

        ##
        ## The logic.
        ##
        virtualPlanes = {}
        for particle in doc["mc"]:
            if "tracks" in particle:
                for track, value in particle['tracks'].items():

                    # verify initial position is before all the planes
                    if not all(map(lambda x: x > value["initial_position"]["z"], self.planes)):
                        print 'infront'
                        continue
                    else:
                        print 'behind'

                    # this assumes linearly going through tracks
                    for step in value["steps"]:
                        for plane in self.planes:
                            if step["position"]["z"] > plane and plane not in virtualPlanes:
                                virtualPlanes[plane] = [track, step]
                                


        doc["virtual_planes"] = virtualPlanes
        return json.dumps(doc)

    def Death(self):
        return True



