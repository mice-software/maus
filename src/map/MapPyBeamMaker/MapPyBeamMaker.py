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


## @class MappyBeamMaker.MappyBeamMaker
"""BeamMaker generates a matched beam suitable for
intput in the upstream tracker in ICOOL format beam file"""


# @authors  Timothy Carlisle <t.carlisle1@physics.ox.ac.uk>
#& based on work by Mark Rayner <m.rayner1@physics.ox.ac.uk> for G4MICE

import json
#import types
import math
import random
import numpy
import numbers

class MapPyBeamMaker:
    """__"""

    def __init__(self):
        self._emittance4d = 0
        self._central_pz = 0
        self._sigma_pz = 0
        self._sigma_time = 0
        
    def birth(self, argJsonConfigDocument):
        """Get DataCards"""
        configDoc = json.loads(argJsonConfigDocument)
       
        key = "emittance4D"
        result = True
        if key in configDoc:
            assert isinstance(configDoc[key], numbers.Real)
            self._emittance4d = configDoc[key]
        else:
            result = False

        key = "centralPz"

        if key in configDoc:
            assert isinstance(configDoc[key],numbers.Real)
            self._central_pz = configDoc[key]
        else:
            result = False
            #return False

        key = "sigmaPz"

        if key in configDoc:
            assert isinstance(configDoc[key],numbers.Real)
            self._sigma_pz = configDoc[key]
        else:
            result = False
            #return False

        key = "sigmaTime"

        if key in configDoc:
            assert isinstance(configDoc[key], numbers.Real)
            self._sigma_time = configDoc[key]
        else:
            result = False
            #return False

  
        return result #true


    def process(self, str):
        """Generate beam covariance matrix, particles & coordinates"""
        try:
            spill = json.loads(str)
        except ValueError:
            spill = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return json.dumps(spill)
               
        
        if "mc" not in spill:
            if 'errors' not in spill:
                spill['errors'] = {}
            spill['errors']['no spill'] = 'no spill'
            return json.dumps(spill)
               
        if "particle_id" in  spill: #or "particle_id"
            if 'errors' not in spill:
                spill['errors'] = {}
            spill['errors']['pre-existing_branch'] = 'pre-existing MC branch'
            return json.dumps(spill)
   
      
        b_z = 4.#  Tesla
        kappa =  0.15 * ( (b_z) / ( self._central_pz ) ) # m^-1
        beta = 1. / math.fabs( kappa )
        gamma = 2. / beta
       # print "beta = " + str(beta*100)  + " mm" 
        mass = 105.658367 # MeV/c              ## defined centrally?

        s_xx = self._emittance4d * beta * mass / self._central_pz
        s_pp = self._emittance4d * gamma * mass * self._central_pz
        cross_term = mass * self._emittance4d

        # build 4x4 beam covariance matrix
        s_matrix = numpy.matrix([ [s_xx, 0, 0, - cross_term],
                 [0, s_pp, cross_term, 0],
                          [0, cross_term, s_xx, 0],
                                  [ - cross_term, 0, 0, s_pp] ])

        det_s = numpy.linalg.det(s_matrix)
 
        s_inv = s_matrix.I

        #What are our limits?
        _xmax = 5. * math.sqrt( s_xx )
        _pmax = 5. * math.sqrt( s_pp )
        
        #Calculate mean transverse amplitude (c.f. Penn pp. 5)
        _mean_transverse_amp = (4. * mass * self._emittance4d)/ self._central_pz
        
        #now loop over particles
        for i in range (0, len(spill["mc"]), 1): 
            particle = {}
            done = False
    
            # Consult for the format:
            # http://micewww.pp.rl.ac.uk/projects/
            # maus/wiki/DataStructure#MC-Particles
            while ( done == False ):
            
                _xpos = random.uniform(-_xmax, _xmax)
                _ypos  = random.uniform(-_xmax, _xmax)

                _px = random.uniform(-_pmax, _pmax)
                _py = random.uniform(-_pmax, _pmax)

                _time = random.gauss(0., self._sigma_time)
                _pz = random.gauss(self._central_pz, self._sigma_pz)
               
                zeta = numpy.matrix( [_xpos, _px, _ypos, _py] )
#          
                argument = (zeta * s_inv * zeta.T)

                if (math.exp(- 0.5 * argument ) > random.uniform(0., 1.)):
                    done = True
                         
            position = {}
            position["x"] = _xpos
            position["y"] = _ypos

            # ZoffSet?
            position["z"] = 0.
          
            particle["position"] = position
            
            unit_momentum = {}   
            unit_momentum["x"] =  _px #random.uniform(-_pmax, _pmax) 
            unit_momentum["y"] =  _py #random.uniform(-_pmax, _pmax)
            unit_momentum["z"] =  _pz #random.gauss(_central_pz, _sigma_pz)

            total_momentum = math.sqrt(pow(_px,2) + pow(_py,2) + pow(_pz,2))
            
            particle["unit_momentum"] = unit_momentum
            
            particle["energy"] =  math.sqrt(math.pow(total_momentum,2) + math.pow(mass,2))
            # MeV
            
            particle["particle_id"] = 13# PDG PID
          
            spill["mc"][i]= particle
            #print spill
        return json.dumps(spill)
                
    def death(self):
        """DEATH"""
    
        



