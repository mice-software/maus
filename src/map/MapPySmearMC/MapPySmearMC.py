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


## @class MapPyRemoveTracks.MapPyRemoveTracks
#  MapPySmearMC generates a Gaussian smear on all mc_events hits
#
#  The mc_events tree is first copied to mc_smear. Then all hits' coordinates
#  have deltas added to them from a Gaussian distribution.
#
import copy
import json
import types
import numpy

class MapPySmearMC:
    def birth(self, argJsonConfigDocument):

        return True

    def process(self, str):
        try:
            doc = json.loads(str)
        except ValueError:
            doc = {"errors": {"bad_json_document": "unable to do json.loads on input"} }
            return json.dumps(doc)

        if "mc_events" not in doc:
            if 'errors' not in doc:
                doc['errors'] = {}
            doc['errors']['missing_branch'] = 'Missing MC events branch'
                
            return json.dumps(doc)
        
        assert isinstance(doc["mc_events"], list)

        doc["mc_smeared"] = copy.deepcopy(doc["mc_events"])

        # iterate through each MC particle
        for particle_index in range(len(doc["mc_smeared"])):
          # iterate through 
          for key in doc["mc_smeared"][particle_index]:
              key_str = key.encode('ascii','ignore')
              if (key_str.find('hits') != -1):
                hit_group = doc["mc_smeared"][particle_index][key]
                for hit in hit_group:
                  position = hit["position"]
                  position["x"] += numpy.random.normal(scale=0.5)
                  position["y"] += numpy.random.normal(scale=0.5)
                  position["z"] += numpy.random.normal(scale=0.5)

                  momentum = hit["momentum"]
                  momentum["x"] += numpy.random.normal(scale=3)
                  momentum["y"] += numpy.random.normal(scale=3)
                  momentum["z"] += numpy.random.normal(scale=3)
        return json.dumps(doc)

    def death(self):
        return True


