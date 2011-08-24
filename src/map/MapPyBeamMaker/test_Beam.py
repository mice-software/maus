import unittest
import Beam
import xboa.Hit
import xboa.Common
import copy
import numpy

MU_MASS = xboa.Common.pdg_pid_to_mass[13]

TEST_PRIM_P = {
    "position":{"x":1.0, "y":2.0, "z":3.0},
    "momentum":{"x":4.0, "y":5.0, "z":6.0},
    "particle_id":2212,
    "energy":1000.0,
    "time":2.0,
    "random_seed":0
}

TEST_PRIM_MU = {
    "position":{"x":0.0, "y":0.0, "z":3.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13,
    "energy":226.0,
    "time":0.0,
    "random_seed":0
}

TEST_PRIM_F1 = {
    "position":{"x":0.0, "y":0.0, "z":0.0},
    "momentum":{"x":0.0, "y":0.0, "z":0.0}, # momentum vector = 0
    "particle_id":-13,
    "energy":226.0,
    "time":0.0,
    "random_seed":0
}

TEST_PRIM_F2 = {
    "position":{"x":0.0, "y":0.0, "z":0.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13,
    "energy":100.0, # energy < mass
    "time":0.0,
    "random_seed":0
}

TEST_REF = {'local_weight': 1.0, 'energy': 1000.0, 'pid': 2212, 'ey': 0.0, 
'ex': 0.0, 'ez': 0.0, 'e_dep': 0.0, 'pz': 236.51264524548657, 'event_number': 0,
'px': 157.67509683032438, 'py': 197.09387103790547, 'charge': 0.0, 'station': 0,
'global_weight': 1.0, 'status': 0, 'proper_time': 0.0, 'path_length': 0.0,
'particleNumber': 0, 'particle_number': 0, 'bx': 0.0, 'by': 0.0, 'bz': 0.0,
'sz': 0.0, 'sy': 0.0, 'sx': 0.0, 'eventNumber': 0, 'mass': 938.271996, 't': 2.0,
'y': 2.0, 'x': 1.0, 'z': 3.0}

TEST_PENN = {"transverse_mode":"penn", "emittance_4d":6., "beta_4d":333.,
             "alpha_4d":1., "normalised_angular_momentum":1.,
             "bz":4.e-3}

TEST_PENN_F1 = {"transverse_mode":"penn", "emittance_4d":-6., "beta_4d":333.,
             "alpha_4d":1., "normalised_angular_momentum":1.,
             "bz":4.e-3}

TEST_PENN_F2 = {"transverse_mode":"penn", "emittance_4d":6., "beta_4d":-333.,
             "alpha_4d":1., "normalised_angular_momentum":1.,
             "bz":4.e-3}

TEST_TWISS = {"transverse_mode":"twiss",
              'beta_x':333.0, 'alpha_x':1.,'emittance_x':6.,
              'beta_y':200.0, 'alpha_y':-1., 'emittance_y':2.}

TEST_TWISS_F1 = {"transverse_mode":"twiss",
              'beta_x':-333.0, 'alpha_x':1., 'emittance_x':6.,
              'beta_y':200.0, 'alpha_y':-1., 'emittance_y':2.}

TEST_TWISS_F2 = {"transverse_mode":"twiss",
              'beta_x':333.0, 'alpha_x':1., 'emittance_x':-6.,
              'beta_y':200.0, 'alpha_y':-1., 'emittance_y':2.}

TEST_TWISS_L = {"longitudinal_mode":"twiss", "momentum_variable":"p",
                "beta_l":10., "alpha_l":-1., "emittance_l":0.1}

TEST_TWISS_L_F1 = {"longitudinal_mode":"twiss", "momentum_variable":"p",
                "beta_l":-10., "alpha_l":-1., "emittance_l":0.1}

TEST_TWISS_L_F2 = {"longitudinal_mode":"twiss", "momentum_variable":"p",
                "beta_l":10., "alpha_l":-1., "emittance_l":-0.1}

TEST_GAUSSIAN_L = {"longitudinal_mode":"gaussian", "momentum_variable":"pz",
                "sigma_t":1.e6, "sigma_pz":100.}

TEST_GAUSSIAN_L_F1 = {"longitudinal_mode":"gaussian", "momentum_variable":"pz",
                "sigma_t":-1.e6, "sigma_pz":100.}

TEST_GAUSSIAN_L_F2 = {"longitudinal_mode":"gaussian", "momentum_variable":"pz",
                "sigma_t":1.e6, "sigma_pz":-100.}

TEST_SAWTOOTH_T = {"longitudinal_mode":"sawtooth_time",
                   "momentum_variable":"energy",
                   "sigma_energy":100.,
                   "t_start":-100.,
                   "t_end":-75.}

TEST_UNIFORM_T = {"longitudinal_mode":"uniform_time",
                   "momentum_variable":"energy",
                   "sigma_energy":100.,
                   "t_start":-100.,
                   "t_end":750.}

TEST_UNIFORM_T_F1 = {"longitudinal_mode":"uniform_time",
                   "momentum_variable":"energy",
                   "sigma_energy":-100.,
                   "t_start":-100.,
                   "t_end":750.}

TEST_BIRTH = {
  "weight":0.5,
  "random_seed":10,
  "reference":TEST_PRIM_MU,
  "transverse":TEST_PENN,
  "longitudinal":TEST_TWISS_L,
  "coupling":{"coupling_mode":"none"}
}

class test_Beam(unittest.TestCase):
    @classmethod
    def setUp(cls):
        cls.beam._Beam__birth_reference_particle({"reference":TEST_PRIM_MU})

    def test_birth_particle_generator(self):
        beam = Beam.Beam()
        test_gen = ["n_particles_per_spill"]
        beam._Beam__birth_particle_generator({"n_particles_per_spill":1}, "counter")       
        beam._Beam__birth_particle_generator({"weight":2.}, "binomial")
        beam._Beam__birth_particle_generator({"weight":2.}, "overwrite_existing")
        try:
            beam._Beam__birth_particle_generator({"n_particles_per_spill":0}, "counter")
            raise RuntimeError("Should have made a ValueError")
        except ValueError:
            pass
        try:
            beam._Beam__birth_particle_generator({"weight":0.}, "binomial")
            raise RuntimeError("Should have made a ValueError")
        except ValueError:
            pass
        try:
            beam._Beam__birth_particle_generator({"weight":-1.}, "overwrite_existing")
            raise RuntimeError("Should have made a ValueError")
        except ValueError:
            pass
        
    def test_birth_particle_generator(self):
        self.assertRaises(ValueError, self.beam._Beam__birth_particle_generator,
                          {"n_particles_per_spill":0}, "counter")
        self.beam._Beam__birth_particle_generator({"n_particles_per_spill":1},
                                                                      "counter")
        self.assertEqual(self.beam.n_particles_per_spill, 1)
        self.assertRaises(ValueError, self.beam._Beam__birth_particle_generator,
                          {"weight":0}, "overwrite_existing")
        self.beam._Beam__birth_particle_generator({"weight":2.}, "binomial")
        self.assertAlmostEqual(self.beam.weight, 2.)


    def test_birth_reference_particle(self):
        self.beam_no_ref._Beam__birth_reference_particle({"reference":TEST_PRIM_P}) 
        self.assertEqual(
                self.beam_no_ref.reference,
                xboa.Hit.Hit.new_from_dict(TEST_REF) )
        test_broken = copy.deepcopy(TEST_PRIM_P)
        test_broken["position"] = 0
        self.assertRaises( ValueError,
                           self.beam_no_ref._Beam__birth_reference_particle,
                           ({"reference":test_broken}) )
        self.beam_no_ref._Beam__birth_reference_particle({"reference":TEST_PRIM_MU}) 
        self.assertEqual( self.beam_no_ref.reference['pid'], -13)

    def test_birth_transverse_no_mode(self):
        self.assertRaises(KeyError, self.beam._Beam__birth_transverse_ellipse, {})
        self.assertRaises(ValueError, self.beam._Beam__birth_transverse_ellipse,
                                                      {"transverse_mode":"bob"})

    def test_birth_transverse_pencil(self):
        self.beam._Beam__birth_transverse_ellipse({"transverse_mode":"pencil"})
        self.assertTrue( numpy.all(numpy.equal(
                                   self.beam.beam_matrix[0:4, 0:4],
                                   numpy.diag([1.]*4)
                       )))
        self.assertEqual( self.beam.transverse_mode, "pencil" )

    def test_birth_transverse_penn(self):
        test = \
        [[1.05668599e+03, -6.33950201e+02,  0.00000000e+00,  3.77222626e-01],
         [-6.33950201e+02, 7.60666714e+02, -3.77222626e-01,  0.00000000e+00],
         [0.00000000e+00, -3.77222626e-01,  1.05668599e+03, -6.33950201e+02],
         [3.77222626e-01,  0.00000000e+00, -6.33950201e+02,  7.60666714e+02]]
        test_array = numpy.array(test)
        self.beam._Beam__birth_transverse_ellipse(TEST_PENN)
        self.assertRaises(ValueError,
              self.beam._Beam__birth_transverse_ellipse, TEST_PENN_F1)
        self.assertRaises(ValueError,
              self.beam._Beam__birth_transverse_ellipse, TEST_PENN_F2)
        self.assertEqual( self.beam.transverse_mode, "penn" )
        for i in test.shape[0]:
            for j in test.shape[1]:
                self.assertAlmostEqual(test[i, j] == \
                                       self.beam.beam_matrix[i, j])

    def test_birth_transverse_const_sol(self):
        #self.beam._Beam__birth_transverse_ellipse(TEST_CONSTANT_SOL)
        self.assertTrue(False) # check beam ellipse is correct (I think not)

    def test_birth_transverse_twiss(self):
        self.assertRaises(ValueError, 
              self.beam._Beam__birth_transverse_ellipse, TEST_TWISS_F1)
        self.assertRaises(ValueError, 
              self.beam._Beam__birth_transverse_ellipse, TEST_TWISS_F2)
        self.assertEqual( self.beam.transverse_mode, "twiss" )
        self.assertTrue(False) # check beam ellipse is correct

    def test_birth_longitudinal_no_mode(self):
        self.assertRaises( KeyError,
                                self.beam._Beam__birth_longitudinal_ellipse, {})
        self.assertRaises( KeyError,
                           self.beam._Beam__birth_longitudinal_ellipse, 
                           {"longitudinal_mode":"pencil"})
        self.assertRaises( ValueError,
                           self.beam._Beam__birth_longitudinal_ellipse, 
                           {"longitudinal_mode":"pencil", 
                            "momentum_variable":"bob"})
        self.assertRaises( ValueError,
                           self.beam._Beam__birth_longitudinal_ellipse, 
                           {"longitudinal_mode":"bob", "momentum_variable":"p"})

    def test_birth_longitudinal_pencil(self):
        for mom_var in ["p", "pz", "energy"]:
          self.beam._Beam__birth_longitudinal_ellipse(
                {"longitudinal_mode":"pencil", "momentum_variable":mom_var})
        self.assertTrue( numpy.all(numpy.equal(self.beam.beam_matrix, numpy.diag([1.]*6) )) )
        self.assertEqual( self.beam.longitudinal_mode, "pencil" )

    def test_birth_longitudinal_twiss(self):
        self.beam._Beam__birth_longitudinal_ellipse(TEST_TWISS_L)
        self.assertEqual( self.beam.longitudinal_mode, "twiss" )
        self.assertRaises(ValueError,
                          self.beam._Beam__birth_longitudinal_ellipse,
                          TEST_TWISS_L_F1)
        self.assertRaises(ValueError,
                          self.beam._Beam__birth_longitudinal_ellipse,
                          TEST_TWISS_L_F2)
        self.assertTrue(False) # check beam ellipse is correct

    def test_birth_longitudinal_gaussian(self):
        self.beam._Beam__birth_longitudinal_ellipse(TEST_GAUSSIAN_L)
        self.assertEqual( self.beam.longitudinal_mode, "gaussian" )
        self.assertTrue( numpy.all(numpy.equal(self.beam.beam_matrix,
                                            numpy.diag([1.]*4+[1.e12, 1.e4])
                       )))
        self.assertRaises(ValueError,
                          self.beam._Beam__birth_longitudinal_ellipse,
                          TEST_GAUSSIAN_L_F1)
        self.assertRaises(ValueError,
                          self.beam._Beam__birth_longitudinal_ellipse,
                          TEST_GAUSSIAN_L_F2)


    def test_birth_longitudinal_t_dist(self):
        self.beam._Beam__birth_longitudinal_ellipse(TEST_UNIFORM_T)
        self.assertEqual(self.beam.longitudinal_mode, "uniform_time")
        self.assertAlmostEqual(self.beam.t_start, TEST_UNIFORM_T["t_start"])
        self.assertAlmostEqual(self.beam.t_end, TEST_UNIFORM_T["t_end"])
        self.assertRaises(ValueError, 
                          self.beam._Beam__birth_longitudinal_ellipse,
                          TEST_UNIFORM_T_F1)

        self.beam._Beam__birth_longitudinal_ellipse(TEST_SAWTOOTH_T)
        self.assertEqual(self.beam.longitudinal_mode, "sawtooth_time")
        self.assertAlmostEqual(self.beam.t_start, TEST_SAWTOOTH_T["t_start"])
        self.assertAlmostEqual(self.beam.t_end, TEST_SAWTOOTH_T["t_end"])

    def test_birth_trans_long_coupling(self):
        self.beam._Beam__birth_trans_long_coupling({"coupling_mode":"none"})
        self.assertRaises(NotImplementedError,
                          self.beam._Beam__birth_trans_long_coupling,
                          {"coupling_mode":"bob"})

    def test_birth_beam_mean(self):
       for mom_var in ["p", "pz", "energy"]:
          self.beam._Beam__birth_longitudinal_ellipse(
                {"longitudinal_mode":"pencil", "momentum_variable":mom_var})
          self.beam._Beam__birth_beam_mean()
          for i, key in enumerate(['x', 'px', 'y', 'py', 't']):
              self.assertAlmostEqual(self.beam.reference[key],
                                     self.beam.beam_mean[i])
          self.assertAlmostEqual(self.beam.reference[mom_var],
                                 self.beam.beam_mean[5])

    def test_birth(self):
        beam = Beam.Beam()
        beam.birth(TEST_BIRTH, "binomial", 2)

    def test_process_array_to_primary(self):
        mean = numpy.array([10., 20., 30., 40., 50., 600.])
        mass = xboa.Common.pdg_pid_to_mass[13]
        self.beam.seed = 10
        primary = self.beam._Beam__process_array_to_primary(mean, 13, 'p')
        self.assertAlmostEqual(primary["position"]["x"], 10.)
        self.assertAlmostEqual(primary["position"]["y"], 30.)
        self.assertAlmostEqual(primary["position"]["z"], 3.) # from ref
        self.assertAlmostEqual(primary["momentum"]["x"], 20.)
        self.assertAlmostEqual(primary["momentum"]["y"], 40.)
        self.assertAlmostEqual(primary["momentum"]["z"],
                          (600.**2.-20**2.-40.**2.)**0.5)
        self.assertAlmostEqual(primary["energy"], (600.**2.+mass**2.)**0.5)
        self.assertAlmostEqual(primary["time"], 50.)
        self.assertEqual(primary["particle_id"], 13)
        self.assertEqual(primary["random_seed"], 10)

        primary = self.beam._Beam__process_array_to_primary(mean, 13, 'pz')
        self.assertAlmostEqual(primary["momentum"]["x"], 20.)
        self.assertAlmostEqual(primary["momentum"]["y"], 40.)
        self.assertAlmostEqual(primary["momentum"]["z"], 600.)
        self.assertAlmostEqual(primary["energy"],
                          (600.**2.+20.**2.+40.**2.+mass**2.)**0.5)

        primary = self.beam._Beam__process_array_to_primary(mean, -13, 'energy')
        self.assertAlmostEqual(primary["momentum"]["x"], 20.)
        self.assertAlmostEqual(primary["momentum"]["y"], 40.)
        self.assertAlmostEqual(primary["momentum"]["z"],
              (600.**2.-xboa.Common.pdg_pid_to_mass[13]**2.-20.**2-40.**2)**0.5)
        self.assertAlmostEqual(primary["energy"], 600.)

    def test_make_one_primary_gaussian_dist(self):
        beam = Beam.Beam()
        beam.birth(TEST_BIRTH, "binomial", 2)
        bunch = xboa.Bunch.Bunch()
        for i in range(1000):
            primary = beam.make_one_primary()
            hit = xboa.Hit.Hit.new_from_maus_object('maus_primary', primary, 0)           
            hit.check()

    def test_make_one_primary_pencil(self):
        beam = Beam.Beam()
        test_birth = {
          "weight":0.5,
          "random_seed":10,
          "reference":TEST_PRIM_MU,
          "transverse":{"transverse_mode":"pencil"},
          "longitudinal":{"longitudinal_mode":"pencil","momentum_variable":"p"},
          "coupling":{"coupling_mode":"none"},
        }
        beam.birth(test_birth, "binomial", 2)
        for i in range(10):
            primary = beam.make_one_primary()
            self.assertEqual(beam.reference,
                  xboa.Hit.Hit.new_from_maus_object('maus_primary', primary, 0))

    def test_make_one_primary_sawtooth(self):
        beam = Beam.Beam()
        test_birth = {
          "weight":0.5,
          "random_seed":10,
          "reference":TEST_PRIM_MU,
          "transverse":{"transverse_mode":"pencil"},
          "longitudinal":TEST_SAWTOOTH_T,
          "coupling":{"coupling_mode":"none"},
        }
        beam.birth(test_birth, "binomial", 2)
        for i in range(1000):
            primary = beam.make_one_primary()
            hit = xboa.Hit.Hit.new_from_maus_object('maus_primary', primary, 0)
            self.assertGreater(hit['t'], TEST_SAWTOOTH_T["t_start"])
            self.assertLess(hit['t'], TEST_SAWTOOTH_T["t_end"])
            self.assertGreater(hit['energy'], xboa.Common.pdg_pid_to_mass[13])
            hit.check()
                  
    def test_make_one_primary_uniform(self):
        beam = Beam.Beam()
        test_birth = {
          "weight":0.5,
          "random_seed":10,
          "reference":TEST_PRIM_MU,
          "transverse":{"transverse_mode":"pencil"},
          "longitudinal":TEST_UNIFORM_T,
          "coupling":{"coupling_mode":"none"},
        }
        beam.birth(test_birth, "binomial", 2)
        for i in range(1000):
            primary = beam.make_one_primary()
            hit = xboa.Hit.Hit.new_from_maus_object('maus_primary', primary, 0)
            self.assertGreater(hit['t'], TEST_UNIFORM_T["t_start"])
            self.assertLess(hit['t'], TEST_UNIFORM_T["t_end"])
            self.assertGreater(hit['energy'], xboa.Common.pdg_pid_to_mass[13])
            hit.check()

    beam = Beam.Beam()
    beam_no_ref = Beam.Beam()

if __name__=="__main__":
    unittest.main()


