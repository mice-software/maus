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


## @class MapPyCreateSpill.MapPyCreateSpill
"""  MapPyCreateSpill creates N empty particles in a spill"""
#
#
#  @author Timothy Carlisle <t.carlisle1@physics.ox.ac.uk>
import json
#import types


class MapPyCreateSpill:
    """__"""
    
    def __init__(self):
        
        self.n_particles = 0
        
    def birth(self, arg_json_config_document):
        """ Birth """
        
        config_doc = json.loads(arg_json_config_document)
       # print  "doc " + arg_json_config_document
        key = "CreateSpill_Nparticles"
        
        if key in config_doc:
            assert isinstance(config_doc[key], int)
            self.n_particles  = config_doc[key]
                    
        return True
        
    def process(self, spill):
        """ Generate spill Stuff """
#        print "json check" +  spill
        try:
            spill = json.loads(spill)
        except ValueError:
            spill = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return json.dumps(spill)
      
        print " "
        if "mc" in spill:
            if 'errors' not in spill:
                spill['errors'] = {}
                spill['errors']['pre-existing_branch'] = 'pre_existing MCbranch'
                return json.dumps(spill)
            
        spill["mc"] = []
            
        for i in range(self.n_particles):
            spill["mc"].append({})

        return json.dumps(spill)
    
    def death(self):
        """DEATH"""
        return True

    
