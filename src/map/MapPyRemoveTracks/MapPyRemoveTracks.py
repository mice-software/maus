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
#  MapPyRemoveTracks removes tracks
#
#  Can either removes the all of the spill["tracks"]
#  portions of the spill or just non-muon tracks.
#  The tracks can take up significant amounts of space,
#  so this is a good idea. Or you can just tell the
#  Simulation() not to create tracks.
import json
import types

class MapPyRemoveTracks:
    def Birth(self, argJsonConfigDocument):
        self._keep_only_muons = False
        configDoc = json.loads(argJsonConfigDocument)

        key = "keep_only_muon_tracks"

        if key in configDoc:
            assert isinstance(configDoc[key], bool)
            self._keep_only_muons = configDoc[key]

        return True

    def Process(self, str):
        try:
            doc = json.loads(str)
        except ValueError:
            doc = {"errors": {"bad_json_document": "unable to do json.loads on input"} }
            return json.dumps(doc)

        if "mc" not in doc:
            if 'errors' not in doc:
                doc['errors'] = {}
            doc['errors']['missing_branch'] = 'Missing MC branch'
                
            return json.dumps(doc)
        
        assert isinstance(doc["mc"], list)
        
        for i in range(len(doc["mc"])):
            newTracks = {}
            # doc["mc"][i] is an MC particle
            if "tracks" in doc["mc"][i]:
                if self._keep_only_muons:
                    for track, value in list(doc["mc"][i]["tracks"].items()):
                        if abs(int(value["particle_id"])) == 13:
                            newTracks[track] = value
                else:
                    newTracks = {}
                doc["mc"][i]["tracks"] = newTracks

                if doc["mc"][i]["tracks"] == {}:
                    del doc["mc"][i]["tracks"]

        return json.dumps(doc)

    def Death(self):
        return True


