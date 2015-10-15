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
Responsible for initialising parent distribution and sampling single particles
for beam generation.
"""

import math
import numpy
import xboa
import xboa.bunch
import xboa.common
import xboa.hit
# 32 bit long; note that CLHEP max for the seed is a 32 bit unsigned int which
# goes to 4294967295 in both 32 bit and 64 bit.
NUMPY_RAND_MAX = 2147483647 

class Beam(): # pylint: disable=R0902
    """
    @brief Beam holds functions necessary to make a single MAUS beam and add
    particles to the spill.

    A beam is typically defined by a reference trajectory, which is the beam
    centroid; and a beam ellipse that defines the alignment of the beam in 
    position-momentum space around the beam. Additional functions are provided
    for "pencil" beams, i.e. those with no distribution and special "uniform"
    and "sawtooth" time distributions, that are intended to more realistically
    reflect the time distribution of the MICE beam.

    Beam holds the methods to set up the centroid of the beam (reference
    trajectory) and the associated distributions; and to make primary particles.
    Also stores either number of particles that will be created or relative
    likelihood that this beam will fire a particle, depending on how multiple
    beams are sampled (set in MapPyBeamMaker class).

    When sampling from a multivariate Gaussian distribution, the beam is defined
    by a beam ellipse \f$\mathbf{B}\f$ that is the locus of points 
    \f[\vec{u}\mathbf{B}^{-1} \vec{u}^{T} = 1\f]
    where \f$\vec{u}\f$ is a vector going like 
    \f$(x, px, y, py, time, \mathcal{P})\f$
    relative to the mean. \f$\mathcal{P}\f$ is a momentum-type variable. Either
    total momentum, energy or pz are allowed here depending on the variable
    momentum_defined_by.

    The matrix \f$\mathbf{B}\f$ is then a 6x6 matrix known (for gaussian
    distributions) as the covariance matrix as the resultant distribution will 
    have covariances given by
    \f[Cov(u_i, u_j) = B_{ij}\f]
    We allow the beam ellipse to be defined in a number of ways. For transverse
    modes we have either Twiss, where the ellipse is defined by
    \f[\mathbf{B_\perp} = m \left(\begin{array}{cccc}
    \epsilon_x \beta_x/p   & -\epsilon_x\alpha_x & 0 & 0 \\
    -\epsilon_x\alpha_x & \epsilon_x\gamma_x p  & 0 & 0 \\
    0 & 0  & \epsilon_y\beta_y/p  & -\epsilon_y\alpha_y \\
    0 & 0 & -\epsilon_y\alpha_y & \epsilon_y\gamma_y p \\
    \end{array}\right),\f]
    Penn, where the ellipse is defined 
    \f[\mathbf{B_\perp} = m\epsilon_\perp \left(\begin{array}{cccc}
    \beta_\perp/p   & -\alpha_\perp & 0 & -\mathcal{L}+\beta_\perp B_0/2p \\
    -\alpha_\perp & \gamma_\perp p  & \mathcal{L}-\beta_\perp B_0/2p & 0 \\
    0 & \mathcal{L}-\beta_\perp B_0/2p  & \beta_\perp/p  & -\alpha_\perp \\
    -\mathcal{L}+\beta_\perp B_0/2p & 0 & -\alpha_\perp & \gamma_\perp p \\
    \end{array}\right),\f]
    or constant_solenoid, where the ellipse is defined as in Penn above, except
    with
    \f$\alpha_\perp=0\f$ and 
    \f[\frac{\beta_\perp}{k_s}\sqrt{1+\mathcal{L}}\f]
    where \f$k_s\f$ is the focussing strength in a solenoid, given by
    \f[k_s = \frac{qc}{2 b_z p}\f]
    This final mode gives a beam with constant beam ellipse in a constantly
    focussing solenoid.

    In longitudinal phase space we have either Gaussian, where the ellipse is
    defined by 
    \f[\mathbf{B_l} = \left(\begin{array}{cc}
    \sigma_t^2   & 0\\
    0 & \sigma_\mathcal{P}^2 \\
    \end{array}\right),\f]    or Twiss, where the ellipse is defined by
    \f[\mathbf{B_l} = m\epsilon_l \left(\begin{array}{cc}
    \beta_l/p   & -\alpha_l\\
    -\alpha_l & \gamma_l p \\
    \end{array}\right).\f]
    Additionally, as mentioned above we can override the beam ellipse definition
    with explicit time distributions.

    Coupling between longitudinal and transverse (e.g. dispersion) has yet to be 
    implemented.
    """

    def __init__(self):
        """
        Initialises to empty settings - pencil beam and no particles. Member
        data as follows:
            - beam_matrix 
            - beam_mean defines the centroid of the beam ellipse 
            \f$\vec{u}_0\f$, so that generated values will be 
            \f$\vec{u}+\vec{u}_0\f$
            - reference is an xboa hit type that describes also the centroid of
            the beam ellipse
            - momentum_defined_by 
            - transverse_mode Defines the way the transverse beam ellipse is
            generated (or not for pencil beams)
            - longitudinal_mode Defines the way the longitudinal beam ellipse
            is generated
            - coupling_mode Defines any coupling between longitudinal and
            transverse (e.g. dispersion). Disabled for now.
            - n_particles_per_spill number of particles per spill when in
            counter mode
            - weight relative probability of a particle being sampled from this
            distribution when in sampling mode
            - t_dist time distribution
            - t_start start time of the time distribution
            - t_end end time of the time distribution
            - momentum_defined_by defines the longitudinal momentum-type 
              variable
        """
        self.beam_matrix = numpy.diag([1.]*6)
        self.beam_mean = numpy.zeros((6))
        self.transverse_mode = "pencil"
        self.longitudinal_mode = "pencil"
        self.coupling_mode = "none"
        self.reference = xboa.hit.Hit()
        self.n_particles_per_spill = 0
        self.weight = 0.
        self.t_dist = ""
        self.t_start = 0.
        self.t_end = 0.
        self.momentum_defined_by = "energy"
        self.beam_seed = 0
        self.particle_seed_algorithm = ""
        #############################################
        self.beam_polarisation = "flat"
        self.beam_mean_x = 1.0
        self.beam_mean_y = 2.0
        self.beam_mean_z = 3.0
        self.beam_sigma_x = 4.0
        self.beam_sigma_y = 5.0
        self.beam_sigma_z = 6.0
        #############################################
        self.a_p_correlation = {}

    def birth(self, beam_def, particle_generator, random_seed):
        """
        Construct the beam ellipes, reference particle and other beam data
            - beam_def dictionary holding a data tree that defines the beam (see
            configuration docs).
            - particle_generator string that controls the way in which multiple
            beams gather data
            - random_seed seed used for the random number generator. Note that
            beam doesn't set the seed; this has to be done at a higher level as
            numpy only stores one seed.
            - particle_seed seed used for the random number generator.
        """
        self.beam_seed = random_seed
        self.__birth_particle_generator(beam_def, particle_generator)
        self.__birth_reference_particle(beam_def)
        self.__birth_transverse_ellipse(beam_def["transverse"])
        self.__birth_longitudinal_ellipse(beam_def["longitudinal"])
        self.__birth_trans_long_coupling(beam_def["coupling"])
        self.__birth_beam_mean()
        self.__birth_beam_polarisation(beam_def)
        self.__birth_a_p_correlation(beam_def)
 
    def __birth_particle_generator(self, beam_def, particle_generator):
        """
        Get n_particles_per_spill or weight from the particle generator
        """
        if particle_generator == "counter":
            self.n_particles_per_spill = beam_def["n_particles_per_spill"]
            if self.n_particles_per_spill < 1:
                raise ValueError\
                            ("Must have positive number of particles per spill")
        elif particle_generator in ["overwrite_existing", "binomial"]:
            self.weight = beam_def["weight"]
            if self.weight <= 0.:
                raise ValueError("Weight of a beam must be > 0.")
        self.particle_seed_algorithm = beam_def["random_seed_algorithm"]
        if self.particle_seed_algorithm == "incrementing_random":
            self.beam_seed = self.__random_unsigned()
        if self.particle_seed_algorithm not in self.seed_keys:
            raise ValueError("random_seed_algorithm "+\
                  self.particle_seed_algorithm+" should be one of "+\
                  str(self.seed_keys))

    def __birth_reference_particle(self, beam_definition):
        """Setup the reference particle - of type primary"""
        self.reference = xboa.hit.factory.MausJsonHitFactory.\
                                      hit_from_maus_object(
                                                   'primary',
                                                   beam_definition['reference'],
                                                   0
                                      )

    def __birth_transverse_ellipse(self, beam_def):
        """
        Use algorithms in xboa to build a transverse ellipse from input cards
        """
        self.transverse_mode = beam_def["transverse_mode"]
        ref = self.reference
        trans_matrix = numpy.diag([1.]*4)
        if self.transverse_mode == 'pencil':
            pass # ones is fine then
        elif self.transverse_mode == 'penn':
            #emittance_t, mass, beta_t, alpha_t, p, Ltwiddle_t, bz, q
            trans_matrix = xboa.bunch.Bunch.build_penn_ellipse(
                beam_def['emittance_4d'], ref['mass'], beam_def['beta_4d'],
                beam_def['alpha_4d'], ref['p'],
                beam_def['normalised_angular_momentum'],
                beam_def['bz'], ref['charge'])
        elif self.transverse_mode == "twiss":
            trans_matrix = self.__birth_twiss_ellipse(beam_def)
        elif self.transverse_mode == "constant_solenoid":
            # k_s is solenoid focussing strength: cite Penn MUCOOL note 71
            k_s = abs(ref['charge']*xboa.common.constants['c_light']/\
                                                     2.*beam_def['bz']/ref['p'])
            if abs(beam_def['bz']) < 1e-9:
                raise ZeroDivisionError("Cannot define constant_solenoid "+\
                                        "beam for solenoid with 0. field")
            if abs(ref['charge']) < 1e-9:
                raise ZeroDivisionError("Cannot define constant_solenoid "+\
                                        "beam for reference with 0. charge")
            if abs(ref['p']) < 1e-9:
                raise ZeroDivisionError("Cannot define constant_solenoid "+\
                                        "beam for reference with 0. momentum")
            beta_4d = (1.+beam_def['normalised_angular_momentum'])**0.5/k_s
            alpha_4d = 0.
            trans_matrix = xboa.bunch.Bunch.build_penn_ellipse(
              beam_def['emittance_4d'], ref['mass'], beta_4d, alpha_4d,
              ref['p'], beam_def['normalised_angular_momentum'], beam_def['bz'],
              ref['charge'])
        else:
            raise ValueError('Did not recognise beam transverse mode '+\
                             str(self.transverse_mode))
        for i in range(4):
            for j in range(4):
                self.beam_matrix[i, j] = trans_matrix[i, j]

    def __birth_twiss_ellipse(self, beam_def):
        """
        Use algorithms in xboa to build 2d transverse ellipses and combine into
        4d ellipse
        """
        ref = self.reference
        trans_matrix = numpy.diag([1.]*4)
        geometric = False # define like x,x' (True) or x,px (False)
        trans_matrix_x = xboa.bunch.Bunch.build_ellipse_2d(beam_def['beta_x'],
            beam_def['alpha_x'], beam_def['emittance_x'], ref['p'], ref['mass'],
            geometric)
        trans_matrix_y = xboa.bunch.Bunch.build_ellipse_2d(beam_def['beta_y'],
            beam_def['alpha_y'], beam_def['emittance_y'], ref['p'], ref['mass'],
            geometric)
        for i in range(2):
            for j in range(2):
                trans_matrix[i, j] = trans_matrix_x[i, j]
                trans_matrix[i+2, j+2] = trans_matrix_y[i, j]
        return trans_matrix

    def __birth_longitudinal_ellipse(self, beam_def):
        """
        Use algorithms in xboa to build 2d longitudinal ellipse
        """
        self.longitudinal_mode = beam_def["longitudinal_mode"]
        ref = self.reference
        long_matrix = numpy.diag([1., 1.])
        self.momentum_defined_by = beam_def['momentum_variable']
        if self.momentum_defined_by not in self.momentum_keys:
            raise ValueError('momentum_variable '+str(self.momentum_defined_by)+
                  ' not recognised - should be one of '+str(self.momentum_keys))
        if self.longitudinal_mode == 'pencil':
            pass
        elif self.longitudinal_mode == 'twiss':
            long_matrix = xboa.bunch.Bunch.build_ellipse_2d(
                          beam_def['beta_l'],
                          beam_def['alpha_l'], beam_def['emittance_l'],
                          ref['p'], ref['mass'], False)
        elif self.longitudinal_mode == 'gaussian':
            long_matrix[0, 0] = beam_def['sigma_t']**2.
            long_matrix[1, 1] = beam_def['sigma_'+self.momentum_defined_by]**2.
            corr_key = 'cov(t,'+self.momentum_defined_by+')'
            if corr_key in beam_def:
                long_matrix[0, 1] = beam_def[corr_key]
                long_matrix[1, 0] = long_matrix[0, 1]
            if numpy.linalg.det(long_matrix) <= 0:
                raise ValueError('longitudinal matrix\n'+str(long_matrix)+\
                                 'must be positive definite.')
        elif self.longitudinal_mode == 'uniform_time' or \
             self.longitudinal_mode == 'sawtooth_time':
            self.t_dist = self.longitudinal_mode
            self.t_start = beam_def['t_start']
            self.t_end = beam_def['t_end']
            long_matrix[1, 1] = beam_def['sigma_'+self.momentum_defined_by]**2.
            if beam_def['sigma_'+self.momentum_defined_by] <= 0.:
                raise ValueError('sigma_'+self.momentum_defined_by+ \
                                 " "+str(long_matrix[0, 0])+" must be > 0")
        else:
            raise ValueError("Did not recognise beam longitudinal_mode "+\
                             str(self.longitudinal_mode))
        for i in range(2):
            for j in range(2):
                self.beam_matrix[i+4, j+4] = long_matrix[i, j]

    def __birth_trans_long_coupling(self, beam_def): # pylint: disable=R0201
        """
        Placeholder for future development
        """
        if not (beam_def["coupling_mode"] == "none"):
            raise NotImplementedError\
                           ("No transverse - longitudinal coupling implemented")

    def __birth_a_p_correlation(self, beam_def):
        """
        Setup amplitude momentum correlation
        """
        self.a_p_correlation = {}
        if "a-p_correlation" not in beam_def.keys():
            return
        self.a_p_correlation = beam_def["a-p_correlation"]
        if 'momentum_variable' not in self.a_p_correlation:
            self.a_p_correlation['momentum_variable'] = self.momentum_defined_by
        mom_var = self.a_p_correlation['momentum_variable']
        if mom_var not in self.momentum_keys:
            raise KeyError("Did not recognise a-p_correlation "+\
                                       "momentum_variable '"+str(mom_var)+"'")
        if 'magnitude' not in self.a_p_correlation:
            raise KeyError("Need to define 'magnitude' in beam definition "+\
                  "amplitude momentum correlation")
        # check we can convert to a float
        self.a_p_correlation['magnitude'] = \
                                        float(self.a_p_correlation['magnitude'])

    def __birth_beam_mean(self):
        """
        Setup the beam mean from reference coordinates
        """
        for i, key in enumerate(Beam.array_keys):
            self.beam_mean[i] = self.reference[key]
        self.beam_mean[5] = self.reference[self.momentum_defined_by]
    
    def __birth_beam_polarisation(self, beam_def):
        """
        Setup the beam polarisation
        """
        self.beam_polarisation = {}
        try:
            self.beam_polarisation = beam_def["beam_polarisation"]
        except KeyError:
            self.beam_polarisation['polarisation_mode'] = 'flat'

        if self.beam_polarisation['polarisation_mode'] == 'flat':
            
            self.beam_mean_x = 0.
            self.beam_mean_y = 0.
            self.beam_mean_z = 0.
            self.beam_sigma_x = 1.
            self.beam_sigma_y = 1.
            self.beam_sigma_z = 1.
            #if flat backward compatibility
        if self.beam_polarisation['polarisation_mode'] == \
           'gaussian_unit_vectors':
            self.beam_mean_x = self.beam_polarisation['beam_mean_x']
            self.beam_mean_y = self.beam_polarisation['beam_mean_y']
            self.beam_mean_z = self.beam_polarisation['beam_mean_z']
            self.beam_sigma_x = self.beam_polarisation['beam_sigma_x']
            self.beam_sigma_y = self.beam_polarisation['beam_sigma_y']
            self.beam_sigma_z = self.beam_polarisation['beam_sigma_z']
       
    def make_one_primary(self):
        """
        Make a primary particle.

        Returns an dict formatted for insertion into the json tree as a primary.
        See json data tree documentation for further information

        Note that if the dice give us a particle that has "negative" total
        momentum we throw again.
        """
        hit = xboa.hit.Hit.new_from_dict({'pid':-13, "energy":float('nan')})
        while math.isnan(hit["energy"]) or math.isnan(hit["pz"]) or \
              hit["energy"] < hit["mass"]:
            particle_array = self.__process_get_particle_array()
            hit = self.__process_array_to_hit(particle_array,
                                self.reference["pid"], self.momentum_defined_by)
            hit = self.__process_a_p_correlation(hit)
        spin = self.__process_beam_polarisation()
        primary = self.__process_hit_to_primary(hit)
        primary["spin"] = {"x":spin[0], "y":spin[1], "z":spin[2]}
        return primary

    def __process_beam_polarisation(self):
        
        """
        Generates a particle array for gaussians
        """
        beam_mean_matrix = numpy.array([self.beam_mean_x,
                                        self.beam_mean_y,
                                        self.beam_mean_z])
        beam_sigma_matrix = numpy.array([[((self.beam_sigma_x)**2) , 0., 0.],
                                         [0., ((self.beam_sigma_y)**2), 0.],
                                         [0., 0., ((self.beam_sigma_z)**2)]])
        particle_array = numpy.random.multivariate_normal(beam_mean_matrix,
                                                          beam_sigma_matrix)
      
        try:
            array_norm = particle_array/\
           (particle_array[0]**2+particle_array[1]**2+particle_array[2]**2)**0.5
        except ZeroDivisionError:
            array_norm = numpy.array([0., 0., 1.])
        
        return  array_norm
                
    def __process_get_particle_array(self):
        """
        Generate a particle array like x,px,y,py,time,<p>
        """
        particle_array = numpy.random.multivariate_normal\
                                              (self.beam_mean, self.beam_matrix)
        if self.transverse_mode == "pencil":
            for i in range(4): 
                particle_array[i] = self.beam_mean[i]
        if self.longitudinal_mode == "pencil":
            particle_array[4] = self.beam_mean[4]
            particle_array[5] = self.beam_mean[5]
        # time distribution is special
        if self.t_dist == "sawtooth_time":
            t_rand = numpy.random.triangular\
                          (self.t_start, self.t_end, 2.*self.t_end-self.t_start)
            if t_rand > self.t_end:
                t_rand = 2.*self.t_end-t_rand
            particle_array[4] = t_rand
        if self.t_dist == "uniform_time":
            particle_array[4] = numpy.random.uniform(self.t_start, self.t_end)
        return particle_array

    def __process_array_to_hit(self, particle_array, pid,
                                                         longitudinal_variable):
        """
        Convert from an array like x,px,y,py,time,<p> to a primary dict that can
        be included in json data tree
        """
        hit = xboa.hit.Hit()
        for i, key in enumerate(Beam.array_keys):
            hit[key] = particle_array[i]
        hit["pid"] = pid
        hit["mass"] = xboa.common.pdg_pid_to_mass[abs(pid)]
        if longitudinal_variable == "p":
            hit["pz"] = (particle_array[5]**2-\
                        particle_array[1]**2-particle_array[3]**2)**0.5
            hit.mass_shell_condition("energy")
        elif longitudinal_variable == "pz":
            hit["pz"] = particle_array[5]
            hit.mass_shell_condition("energy")
        elif longitudinal_variable == "energy":
            hit["energy"] = particle_array[5]
            hit.mass_shell_condition("pz")
        if self.reference["pz"] != 0.:
            hit["pz"] *= self.reference['pz']/abs(self.reference['pz'])
        return hit

    def __process_hit_to_primary(self, hit):
        """
        Convert an xboa hit into a MAUS primary
        """
        primary = hit.get_maus_dict('maus_json_primary')[0]
        primary["position"]["z"] = self.reference["z"]
        primary["random_seed"] = self.__process_get_seed()
        if math.isnan(primary["energy"]) or math.isinf(primary["energy"]):
            raise IOError("Bad energy for primary\n"+str(primary))
        return primary

    def __process_get_seed(self):
        """
        Get the random seed for the next particle

        - if particle_seed = beam_seed, use beam_seed for all particles
        - if particle_seed = random, assign seed as a random unsigned int
        - if particle_seed = incrementing or incrementing_random, assign
                             beam_seed to the first particle, beam_seed+1 to the
                             second particle, etc 
        """
        if self.particle_seed_algorithm == "beam_seed":
            return self.beam_seed
        if self.particle_seed_algorithm == "random":
            return self.__random_unsigned()
        if self.particle_seed_algorithm == "incrementing" or \
           self.particle_seed_algorithm == "incrementing_random":
            random_seed = self.beam_seed
            self.beam_seed += 1
            return random_seed
        #raise NotImplementedError("Did not recognise seed algorithm "+\
                  #                str(self.particle_seed_algorithm))

    def __process_a_p_correlation(self, hit):
        """
        Introduce a correlation between particle amplitude and momentum
        (or energy)

        Calculate the particle amplitude A based on the transverse matrix
        defined in "transverse", like
                            A = U V^{-1} U^{T}
        Add to "momentum_variable" V an amount given by
                            V = V+M*V*A
        where M is self.a_p_corr["magnitude"]. See the MAUS user docs for
        latexed version of this.
        """
        if self.a_p_correlation == {}:
            return hit
        space = ["x", "px", "y", "py"]
        cov_inv = numpy.linalg.inv(self.beam_matrix[0:4, 0:4])
        cov_det = numpy.linalg.det(self.beam_matrix[0:4, 0:4])
        ps_vec = numpy.matrix([hit[var] for var in space])-self.beam_mean[0:4]
        amplitude = (ps_vec*cov_inv*ps_vec.transpose())[0, 0]
        amplitude *= cov_det**0.25*hit['mass']**2
        mom_var = self.a_p_correlation["momentum_variable"]
        mom = hit[mom_var]
        mom = mom + self.a_p_correlation["magnitude"]*mom*amplitude
        hit[mom_var] = mom
        if mom_var == 'p':
            hit.mass_shell_condition('energy')
        elif mom_var == 'pz':
            hit.mass_shell_condition('energy')
        elif mom_var == 'energy':
            hit.mass_shell_condition('p')
        return hit

    array_keys = ["x", "px", "y", "py", "t"]
    momentum_keys = ['p', 'pz', 'energy']

    seed_keys = ["beam_seed", "random", "incrementing", "incrementing_random"]

  

    def __random_unsigned(self): # pylint: disable=R0201
        """
        Return a random unsigned int
        """
        return numpy.random.randint(0, 2147483647)



