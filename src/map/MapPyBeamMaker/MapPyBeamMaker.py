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
BeamMaker generates beams for input into MAUS"""


import json
import ErrorHandler
import math
import random
import numpy
import numbers
import xboa.Common
import xboa.Bunch
import xboa.Hit
import copy
import beam

class MapPyBeamMaker:
    """
    """
    
    def __init__(self):
        """
        Constructor, initiate parameters          
        """
        self.beams = []
        self.particle_generator = None
        self.overwrite_existing = False
        self.binomial_n = 0
        self.binomial_p = 0.5
        self.seed = 0
        self.particle_seed = "random"

    def birth(self, argJsonConfigDocument):
        """
        Read in datacards from supplied cards file and configuration defaults
        """
        try:
            config_doc = json.loads(argJsonConfigDocument)
            self.__birth_empty_particles(config_doc["beam"])
            self.beams = []
            for beam_def in config_doc["beam"]["definitions"]:
                a_beam = beam.Beam()
                a_beam.birth(beam_def, self.particle_generator, self.seed)
                self.beams.append(a_beam)
            return True
        except:
            ErrorHandler.HandleException({}, self)
            return False


    def __birth_empty_particles(self, beam_def):
        """
        Get variables for generating empty particles to be filled with primaries
        later. Several options available:
        - binomial uses binomial_n, binomial_p to generate a random number of
        particles according to the binomial distribution. Uses the weight
        field in each beam to randomly select from multiple beam distributions.
        - counter adds a fixed number of particles defined by each of the beam's
        n_particles_per_spill field.
        - overwrite_existing overwrites the primary branch of any existing
        particles in mc branch, regardless of existing values. Uses the weight
        field in each beam to randomly select from multiple beam distributions.
        """
        self.seed = beam_def["random_seed"]
        numpy.random.seed(self.seed)
        if beam_def["particle_generator"] not in self.gen_keys:
            raise ValueError("Did not recognise particle_generator "+\
                             str(beam_def["particle_generator"])+\
                            " Should be one of "+str(self.gen_keys))
        self.particle_generator = beam_def["particle_generator"]
        if beam_def["particle_generator"] == "binomial":
            self.binomial_n = beam_def["binomial_n"]
            self.binomial_p = float(beam_def["binomial_p"])
            if self.binomial_p > 1. or self.binomial_p <= 0.:
                raise ValueError("Beam binomial_p "+str(self.binomial_p)+\
                               " should be > 0. and <= 1.")
            if self.binomial_n <= 0 :
                raise ValueError("Beam binomial_n "+str(self.binomial_n)+\
                               " should be > 0")

    def process(self, json_spill_doc):
        """
        Generate primary particles for a spill.
        - In counter mode, iterates over each beam and samples required number 
        of particles from the parent distribution.
        - In overwrite_existing or binomial mode, randomly samples from each of
        the available beam distributions according to the relative weight of
        each beam.
        """
        spill = {}
        try:
            spill = json.loads(json_spill_doc)
            self.__process_check_spill(spill)
            new_particles = self.__process_generate_multiple_primaries(spill)
            for index, particle in enumerate(new_particles):
                a_beam = self.__process_choose_beam(index)
                particle["primary"] = a_beam.make_one_primary()
        except:
            ErrorHandler.HandleException(spill, self)
        return json.dumps(spill)
            
    def __process_check_spill(self, spill):
        """
        Check that the spill has a mc branch and that it is an array type
        """
        if "mc" not in spill:
            raise KeyError("Need mc branch for processing spill")
        if type(spill["mc"]) != type([]):
            raise KeyError("mc branch should be an array type")
        return True

    def __process_generate_multiple_primaries(self, spill):
        """
        """
        spill_length = len(spill["mc"])
        if self.particle_generator == "overwrite_existing":
            return spill["mc"]
        elif self.particle_generator == "binomial":
            n_p = numpy.random.binomial(self.binomial_n, self.binomial_p)
            for i in range(n_p):
                spill["mc"].append({"primary":{}})
        elif self.particle_generator == "counter":
            for a_beam in self.beams:
                for i in range(a_beam.n_particles_per_spill):
                    spill["mc"].append({"primary":{}})
        else:
            raise RuntimeError("Didn't recognise particle_generator command "+str(self.particle_generator))
        return spill["mc"][spill_length:]

    def __process_choose_beam(self, index):
        if self.particle_generator == "counter":
            for a_beam in self.beams:
                index -= a_beam.n_particles_per_spill
                if index < 0:
                    return a_beam
        else:
            weights = [0.]
            for i, beam in enumerate(self.beams):
                weights.append(weights[i]+beam["weight"])
            dice = numpy.random.uniform(0., weights[-1])
            return self.beams[bisect.bisect_left(weights, dice)]

    def death(self):
        """ does nothing """
        return True

    gen_keys = ["binomial", "counter", "overwrite_existing"]

