import numpy
import xboa
import xboa.Hit
import xboa.Bunch
import sys

class Beam():
    def __init__(self):
        self.beam_matrix = numpy.diag([1.]*6)
        self.beam_mean = numpy.zeros((6))
        self.transverse_mode = "pencil"
        self.longitudinal_mode = "pencil"
        self.coupling_mode = "pencil"
        self.reference = xboa.Hit.Hit()
        self.n_particles_per_spill = 0
        self.weight = 0.
        self.t_dist = ""
        self.t_start = 0.
        self.t_end = 0.
        self.momentum_defined_by = "energy"
        self.seed = 0

    def birth(self, beam_def, particle_generator, random_seed):
        self.seed = random_seed
        self.__birth_particle_generator(beam_def, particle_generator)
        self.__birth_reference_particle(beam_def)
        self.__birth_transverse_ellipse(beam_def["transverse"])
        self.__birth_longitudinal_ellipse(beam_def["longitudinal"])
        self.__birth_trans_long_coupling(beam_def["coupling"])
        self.__birth_beam_mean()

    def __birth_particle_generator(self, beam_def, particle_generator):
        """
        """
        if particle_generator == "counter":
            self.n_particles_per_spill = beam_def["n_particles_per_spill"]
            if self.n_particles_per_spill < 1:
                raise ValueError("Must have positive number of particles per spill")
        elif particle_generator in ["overwrite_existing", "binomial"]:
            self.weight = beam_def["weight"]
            if self.weight <= 0.:
                raise ValueError("Weight of a beam must be > 0.")

    def __birth_reference_particle(self, beam_definition):
        try:
            self.reference = xboa.Hit.Hit.new_from_maus_object('maus_primary', beam_definition['reference'], 0)
        except:
            #sys.excepthook(*sys.exc_info())
            raise ValueError("Failed to parse reference particle "+str(beam_definition['reference']))

    def __birth_transverse_ellipse(self, beam_def):
        """
        Use algorithms in xboa to build an ellipse from input cards
        """
        self.transverse_mode = beam_def["transverse_mode"]
        ref = self.reference
        trans_matrix = numpy.diag([1.]*4)
        if self.transverse_mode == 'pencil':
            pass # ones is fine then
        elif self.transverse_mode == 'penn':
            #emittance_t, mass, beta_t, alpha_t, p, Ltwiddle_t, bz, q
            trans_matrix = xboa.Bunch.Bunch.build_penn_ellipse(
                beam_def['emittance_4d'], ref['mass'], beam_def['beta_4d'],
                beam_def['alpha_4d'], ref['p'],
                beam_def['normalised_angular_momentum'],
                beam_def['bz'], ref['charge'])
        elif self.transverse_mode == "twiss":
            self.__birth_twiss_ellipse(beam_def)
        elif self.transverse_mode == "constant_solenoid":
            # k_s is solenoid focussing strength: cite Penn MUCOOL note 71
            k_s = (ref['charge']*Common.units['c_light']/2.*ref['bz']/ref['p'])
            beta_4d = (1.+beam_def['normalised_angular_momentum'])**0.5/k_s
            alpha_4d = 0.
            trans_matrix = xboa.Bunch.build_penn_ellipse(
              beam_def['emittance_4d'], ref['mass'], beta_4d, alpha_4d,
              ref['p'], beam_def['normalised_angular_momentum'], beam_def['bz'],
              ref['charge'])
        else:
            raise ValueError('Did not recognise beam transverse mode '+str(self.transverse_mode))
        for i in range(4):
            for j in range(4):
                  self.beam_matrix[i, j] = trans_matrix[i, j]

    def __birth_twiss_ellipse(self, beam_def):
        """
        """
        ref = self.reference
        trans_matrix = numpy.diag([1.]*4)
        geometric = False # define like x,x' (True) or x,px (False)
        trans_matrix_x = xboa.Bunch.Bunch.build_ellipse_2d(beam_def['beta_x'],
            beam_def['alpha_x'], beam_def['emittance_x'], ref['p'], ref['mass'],
            geometric)
        trans_matrix_y = xboa.Bunch.Bunch.build_ellipse_2d(beam_def['beta_y'],
            beam_def['alpha_y'], beam_def['emittance_y'], ref['p'], ref['mass'],
            geometric)
        for i in range(2):
            for j in range(2):
                trans_matrix[i, j] = trans_matrix_x[i, j]
                trans_matrix[i+2, j+2] = trans_matrix_y[i, j]
        return trans_matrix

    def __birth_longitudinal_ellipse(self, beam_def):
        """
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
            long_matrix = xboa.Bunch.Bunch.build_ellipse_2d(beam_def['beta_l'], beam_def['alpha_l'], beam_def['emittance_l'], ref['p'], ref['mass'], False)
        elif self.longitudinal_mode == 'gaussian':
            long_matrix[0, 0] = beam_def['sigma_t']**2.
            long_matrix[1, 1] = beam_def['sigma_'+self.momentum_defined_by]**2.
            if beam_def['sigma_t'] <= 0.:
                raise ValueError("sigma_t "+str(long_matrix[0, 0])+
                                 " must be > 0")
            if beam_def['sigma_'+self.momentum_defined_by] <= 0.:
                raise ValueError('sigma_'+self.momentum_defined_by+ \
                                 " "+str(long_matrix[0, 0])+" must be > 0")
        elif self.longitudinal_mode == 'uniform_time' or \
             self.longitudinal_mode == 'sawtooth_time':
            self.t_dist = self.longitudinal_mode
            self.t_start = beam_def['t_start']
            self.t_end = beam_def['t_end']
            long_matrix[1, 1] = beam_def['sigma_'+self.momentum_defined_by]
            if beam_def['sigma_'+self.momentum_defined_by] <= 0.:
                raise ValueError('sigma_'+self.momentum_defined_by+ \
                                 " "+str(long_matrix[0, 0])+" must be > 0")
        else:
            raise ValueError("Did not recognise beam longitudinal_mode "+str(self.longitudinal_mode))

        for i in range(2):
            for j in range(2):
                  self.beam_matrix[i+4, j+4] = long_matrix[i, j]

    def __birth_trans_long_coupling(self, beam_def):
        if not (beam_def["coupling_mode"] == "none"):
            raise NotImplementedError("No transverse - longitudinal coupling implemented")

    def __birth_beam_mean(self):
        for i, key in enumerate(Beam.array_keys):
            self.beam_mean[i] = self.reference[key]
        self.beam_mean[5] = self.reference[self.momentum_defined_by]

    def make_one_primary(self):
        particle_array = numpy.random.multivariate_normal(self.beam_mean, self.beam_matrix)
        if self.transverse_mode == "pencil":
            for i in range(4):
                particle_array[i] = self.beam_mean[i]
        if self.longitudinal_mode == "pencil":
            particle_array[4] = self.beam_mean[4]
            particle_array[5] = self.beam_mean[5]
        # time distribution is special
        if self.t_dist == "sawtooth_time":
            t_rand = numpy.random.triangular(self.t_start, self.t_end, 2.*self.t_end-self.t_start)
            if t_rand > self.t_end:
                t_rand = 2.*self.t_end-t_rand
            particle_array[4] = t_rand
        if self.t_dist == "uniform_time":
            particle_array[4] = numpy.random.uniform(self.t_start, self.t_end)
        return self.__process_array_to_primary(particle_array, self.reference["pid"], self.momentum_defined_by)

    def __process_array_to_primary(self, particle_array, pid, longitudinal_variable):
        """
        """
        hit = xboa.Hit.Hit()
        for i, key in enumerate(Beam.array_keys):
            hit[key] = particle_array[i]
        hit["pid"] = pid
        hit["mass"] = xboa.Common.pdg_pid_to_mass[abs(pid)]
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
        else:
            raise KeyError("Did not recognise longitudinal variable "+\
                           str(longitudinal_variable))
        primary = hit.get_maus_dict('maus_primary')[0]
        primary["position"]["z"] = self.reference["z"]
        primary["random_seed"] = self.seed
        return primary

    array_keys = ["x", "px", "y", "py", "t"]
    momentum_keys = ['p', 'pz', 'energy']

