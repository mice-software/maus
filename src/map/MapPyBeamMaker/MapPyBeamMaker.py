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
BeamMaker generates beams for input into MAUS
"""

### TO DO:
# Humm, I think the file IO should be done in the sub-beam

import json
import ErrorHandler
import numpy
import bisect
import beam
import os
from xboa.hit import Hit #pylint: disable=F0401

class MapPyBeamMaker: #pylint: disable=R0902
    """
    MapPyBeamMaker generates primaries for simulation in Geant4

    MapPyBeamMaker creates a set of primary particles depending on the
    particle_generator field of the beam branch of the json configuration
    document
    - binomial creates a set of particles according to the binomial distribution
    and samples from random parent distributions according to the 'weight' field
    in each beam
    - counter samples a fixed number of particles from each beam
    - overwrite_existing overwrites existing particles and samples from random
    parent distributions with probability of a given parent distribution
    assigned by 'weight' field
    - file reads beam particles from input beam file
    The number of beam particles in a spill is determined by
    nbm_particles_per_spill specified in the config.
    - use_beam_file must be set to True in config if file input
    If use_beam_file=True and particle_generator != "file",
    then particle_generator is reset to "file" in this class

    Each beam is defined in the "definitions" array of the beam branch.
    """
    def __init__(self): 
        """
        Constructor; initialises parameters to 0. Member data as follows:
        - beams list of Beam classes
        - particle_generator string that controls the way particles are
        generated
        - binomial_n, binomial_p control binomial distribution; define binomial
        distribution asthe discrete probability distribution of the number of
        successes in a sequence of binomial_n independent yes/no experiments,
        each of which yields success with probability p.
        - seed random seed used for generating particles (and generating monte
        carlo seeds in some instances)
        - use_beam_file set to F, set it to True in config if beam input
        """
        self.beams = []
        self.particle_generator = None
        self.binomial_n = 0
        self.binomial_p = 0.5
        self.seed = 0
        self.use_beam_file = False
        self.beam_file = None
        self.beam_file_format = None
        self.beam_file_insert_position = {'x':0.0, 'y':0.0, 'z':0.0}
        self.bm_fh = None
        self.file_particles_per_spill = 0
        self.beam_seed = 0
        self.g4bl_generator = False
        self.random_seed = 0

    def birth(self, json_configuration):
        """
        Read in datacards from supplied cards file and configuration defaults.

        @param json_configuration unicode string containing the json
        configuration

        Several options available:
        - binomial uses binomial_n, binomial_p to generate a random number of
        particles according to the binomial distribution. Uses the weight
        field in each beam to randomly select from multiple beam distributions.
        - counter adds a fixed number of particles defined by each of the beam's
        n_particles_per_spill field.
        - overwrite_existing overwrites the primary branch of any existing
        particles in mc branch, regardless of existing values. Uses the weight
        field in each beam to randomly select from multiple beam distributions.
        - file reads beam particles from input beam file
        The number of beam particles in a spill is determined by
        nbm_particles_per_spill specified in the config.
        """
        try:
            config_doc = json.loads(json_configuration)
            self.__birth_empty_particles(config_doc["beam"])
            self.beams = []

            # if use_beam_file, then verify the file can be opened
            # get the file handle, and skip over headers and comments
            if self.use_beam_file:
                return self.__check_beam_file()

            for beam_def in config_doc["beam"]["definitions"]:
                a_beam = beam.Beam()
                a_beam.birth(beam_def, self.particle_generator, self.seed)
                self.beams.append(a_beam)
            return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
            return False


    def __birth_empty_particles(self, beam_def):
        """
        Get variables for generating empty particles to be filled with primaries
        later. 
        """
        self.seed = beam_def["random_seed"]
        self.beam_seed = self.seed
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
        self.use_beam_file = beam_def["particle_generator"] == "file"
        if self.use_beam_file:
            self.beam_file = os.path.expandvars(beam_def["beam_file"])
            self.beam_file_format = beam_def["beam_file_format"]
            self.file_particles_per_spill = beam_def["file_particles_per_spill"]
            if "beam_start_position" in beam_def :
              self.beam_file_insert_position = beam_def["beam_start_position"]
            
    def __check_beam_file(self):
        """
        Check the input beam file
        Skip over header lines (if any) so that xboa.Hit.read is happy
        If no predefined header lines, check for comment lines and skip
        """

        if self.particle_generator != 'file' :
            print "Requested file-input but generator is not 'file'"
            print "Resetting it.."
            self.particle_generator = 'file'

        # try opening the beam file
        # construct path
        try:
            self.bm_fh = open(self.beam_file,'r')
        except IOError as err:
            raise IOError(err)

        # skip over header lines in beam file
        _nhead = {'icool_for009':3, 
                 'icool_for003':2, 
                 'g4beamline_bl_track_file':0,
                 'g4mice_special_hit':0,
                 'g4mice_virtual_hit':0,
                 'zgoubi':0, 
                 'turtle':0, 
                 'madx':0,
                 'mars_1':0, 
                 'maus_virtual_hit':0, 
                 'maus_primary':0
        }
        _nskip = 0
        while _nskip < _nhead[self.beam_file_format] :
            self.bm_fh.readline()
            _nskip = _nskip + 1

        # for formats with no predefined header lines,
        #  make sure we are not sitting on a comment line
        skipped_comments = False
        while not skipped_comments:
            # mark the position in the file and read
            # if it is not a comment line, then rewind
            fpos = self.bm_fh.tell()
            bline = self.bm_fh.readline()
            if '#' not in bline:
                # just read a non-comment line; rewind to start of line
                self.bm_fh.seek(fpos)
                skipped_comments = True
        return True


    def process(self, json_spill_doc):
        """
        Generate primary particles for a spill.

        @param json_spill_doc unicode string containing the json spill

        - In counter mode, iterates over each beam and samples required number 
        of particles from the parent distribution.
        - In overwrite_existing or binomial mode, randomly samples from each of
        the available beam distributions according to the relative weight of
        each beam.

        @returns a string with the json spill
        """
        spill = {}
        try:
            spill = json.loads(json_spill_doc)
            spill = self.__process_check_spill(spill)
            new_particles = self.__process_gen_empty(spill)
            for index, particle in enumerate(new_particles):
                # if beam IO, then read hits from file and fill spill
                if (self.use_beam_file):
                    spill_hit = Hit.new_from_read_builtin(self.beam_file_format,
                                                        self.bm_fh)
                    primary = spill_hit.get_maus_dict('maus_json_primary')[0]
                    self.beam_seed = self.beam_seed + 1
                    primary["random_seed"] = self.beam_seed
                    primary['position']['x'] += self.beam_file_insert_position['x']
                    primary['position']['y'] += self.beam_file_insert_position['y']
                    primary['position']['z'] += self.beam_file_insert_position['z']
                    particle["primary"] = primary
                else:
                    a_beam = self.__process_choose_beam(index)
                    particle["primary"] = a_beam.make_one_primary()
                if "spin" not in particle["primary"]:
                    particle["primary"]["spin"] = {"x":0., "y":0., "z":1.}
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException(spill, self)
        return json.dumps(spill)
            
    def __process_check_spill(self, spill): #pylint: disable=R0201
        """
        Check that the spill has a mc branch and that it is an array type. If
        no branch, make one. If branch is of wrong type, raise a KeyError.
        Returns a copy of the spill
        """
        if "mc_events" not in spill:
            spill["mc_events"] = []
        if type(spill["mc_events"]) != type([]):
            raise KeyError("mc_events branch should be an array type")
        return spill

    def __process_gen_empty(self, spill):
        """
        Generate empty primaries
        """
        spill_length = len(spill["mc_events"])
        if self.particle_generator == "overwrite_existing":
            for particle in spill["mc_events"]:
                particle["primary"] = {}
            return spill["mc_events"]
        elif self.particle_generator == "binomial":
            n_p = numpy.random.binomial(self.binomial_n, self.binomial_p)
            for i in range(n_p): #pylint: disable=W0612
                spill["mc_events"].append({"primary":{}})
        elif self.particle_generator == "counter":
            for a_beam in self.beams:
                for i in range(a_beam.n_particles_per_spill):
                    spill["mc_events"].append({"primary":{}})
        elif self.particle_generator == "file":
            for i in range(self.file_particles_per_spill):
                spill["mc_events"].append({"primary":{}})
        else:
            raise RuntimeError("Didn't recognise particle_generator command "+\
                               str(self.particle_generator))
        return spill["mc_events"][spill_length:]

    def __process_choose_beam(self, index):
        """
        Choose the beam from which to sample
        """
        if self.particle_generator == "counter":
            for a_beam in self.beams:
                index -= a_beam.n_particles_per_spill
                if index < 0:
                    return a_beam
        else:
            weights = [0.]
            for i, a_beam in enumerate(self.beams):
                weights.append(weights[i]+a_beam.weight)
            dice = numpy.random.uniform(0., weights[-1])
            return self.beams[bisect.bisect_left(weights, dice)-1]

    def death(self): #pylint: disable=R0201
        """Closes beam file; returns true"""
        if (self.bm_fh):
            self.bm_fh.close()
        return True

    gen_keys = ["binomial", "counter", "overwrite_existing", "file"]
