#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
test_factory contains TestFactory class used for generating new tests which can
subsequently be run against new datasets.

Conceptually, test data is self-describing, so given a bit of help (i.e. a test
runner), we can regenerate tests based on the input test data. The test_factory
module and TestFactory class generates some reference tests in the first place.

TestFactory generates both geometries and the tests associated with each
geometry. At the moment we have a one-to-many relationship of geometries and
tests, so the TestFactory job is to generate a geometry and for each geometry a
list of tests that run against that geometry.
"""


import operator
import sys

import xboa.Common as Common

import code_setup # pylint: disable=W0403
import tests # pylint: disable=W0403
import geometry # pylint: disable=W0403


MICE_CONFIGURATIONS = [ #thickness in mm, momentum in MeV/c
{'__material__':'lH2' , '__thickness__':350.,  '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':1000, '__seed__':1}, # 100k events
{'__material__':'lH2' , '__thickness__':35.,  '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':1000, '__seed__':1}, # 100k events
]
void = [
{'__material__':'lH2' , '__thickness__':350.,  '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':400., 
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':800., 
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':100.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1}, # 100k events
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':100000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':400.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':800.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'Al',   '__thickness__':0.5, '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'Be',   '__thickness__':0.5, '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'lHe',  '__thickness__':350.,  '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'STEEL304', '__thickness__':0.5, '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'Cu',   '__thickness__':5., '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'Al',   '__thickness__':0.5, '__momentum__':200.,
 '__pid__':-13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'Be',   '__thickness__':0.5, '__momentum__':200.,
 '__pid__':-13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'STEEL304', '__thickness__':0.5, '__momentum__':200.,
 '__pid__':-13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'Cu',   '__thickness__':5., '__momentum__':200.,
 '__pid__':-13, '__step__':1., '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':100.,
 '__pid__':211, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':200.,
 '__pid__':211, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':800.,
 '__pid__':211, '__step__':100., '__nev__':10000, '__seed__':1},
#about 0.1 MeV kinetic energy electron
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':0.1,'__momentum__':0.35,
 '__pid__':11, '__step__':0.1, '__nev__':10000, '__seed__':1}, 
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':0.1, '__momentum__':1.,
  '__pid__':11, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':1., '__momentum__':10.,
   '__pid__':11, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':1., '__momentum__':100.,
 '__pid__':11,  '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE', '__thickness__':1., '__momentum__':200.,
 '__pid__':11,  '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':1., '__momentum__':800.,
 '__pid__':11,  '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10., '__momentum__':200.,
 '__pid__':13,   '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10., '__momentum__':200.,
 '__pid__':-11,  '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10., '__momentum__':200.,
 '__pid__':-211, '__step__':100., '__nev__':10000, '__seed__':1},
#about 80 MeV kinetic energy proton
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':400.,
 '__pid__':2212, '__step__':100., '__nev__':10000, '__seed__':1},
#about 180 MeV kinetic energy proton
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':600.,
 '__pid__':2212, '__step__':100., '__nev__':10000, '__seed__':1},
#about 800 MeV kinetic energy proton
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':1500.,
 '__pid__':2212, '__step__':100., '__nev__':10000, '__seed__':1},
#about 8000 MeV kinetic energy proton
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':8000.,
 '__pid__':2212, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':200.,
 '__pid__':-13, '__step__':1., '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':200.,
 '__pid__':-13, '__step__':10., '__nev__':10000, '__seed__':1},
]

class TestFactory:
    """
    Class to generate reference test data from a list of substitutions.

    The reference dataset contains sufficient information to then rerun on each
    new version.
    """
    n_ks_divs = 500

    my_tests = [
      tests.KSTest.new('px', 'MeV/c', [-50.+x for x in range(n_ks_divs+1)],
                                   [0]*n_ks_divs, 10000, 1.0, 10000, 0.0, 0.01),
      tests.KSTest.new('energy', 'MeV', [202.+x/4. for x in range(n_ks_divs+1)],
                                   [0]*n_ks_divs, 10000, 1.0, 10000, 0.0, 0.01)
    ]

    def __init__(self, code):
        self.__code = code
        self.__conversions = {
                '__thickness__':self.__code.convert_position,
                '__step__':self.__code.convert_position,
                '__momentum__':self.__code.convert_momentum,
                '__material__':self.__code.convert_material,
                '__pid__':self.__code.convert_pid,
                '__nev__':(lambda x: x),
                '__seed__':(lambda x: x),
               }

    def code_convert(self, config):
        """
        In order to use a common configuration for different codes, we do
        unit and other conversions here. Conversions are
            - position units: icool uses m
            - momentum units: icool uses GeV/c
            - material names: icool and g4bl have different material names
            - pid: icool uses it's own PID codes
        """
        for key, value in config.iteritems():
            if key in self.__conversions.keys():
                config[key] = self.__conversions[key](value)
            else:
                raise(KeyError('Nothing known about key '+str(key)) )
        return config

    def make_name(self, config):
        """Make a name for this test from substitution keys"""
        _dt  = str(config['__thickness__'])
        _mat = str(config['__material__'])
        _nev = str(config['__nev__'])
        _pz  = str(config['__momentum__'])
        _pid = Common.pdg_pid_to_name[config['__pid__']]
        _dz  = str(config['__step__'])
        return str(self.__code)+': '+_dt+' mm '+_mat+' with '+_nev+' '+\
                _pz+' MeV/c '+_pid+' '+_dz+' mm steps'

    def code_parameters(self):
        """Controls how each executable will be called by MAUS"""
        code_parameters = (
            self.__code.get_executable(),
            self.__code.get_parameters(),
            self.__code.get_substitutions()
        )
        return code_parameters

    def build_geometry(self, config):
        """Build and run a set of tests for a specific geometry set up"""
        geo = geometry.Geometry()
        #first setup the geometry basics
        pid      = config['__pid__']
        geo.name = self.make_name(config)
        geo.code_model = self.code_parameters()
        geo.substitutions = self.code_convert(config)
        geo.bunch_index = self.__code.get_bunch_index()
        geo.bunch_read = self.__code.get_bunch_read_keys()
        geo.tests = self.my_tests
        #now run a batch job to get axis range right on tests
        geo.run_mc()
        bunch = geo.read_bunch()
        #adjust the geometry
        for i, test in enumerate(geo.tests):
            test.pid = pid
            bunch.conditional_remove({'pid':pid}, operator.ne)
            [xmin, xmax] = Common.min_max(
                                bunch.list_get_hit_variable([test.variable])[0])
            xmin *= Common.units[test.units]
            xmax *= Common.units[test.units]
            test.bins = [xmin+(xmax-xmin)*x/float(self.n_ks_divs) \
                                              for x in range(self.n_ks_divs+1)]
            geo.tests[i]  = test.run_test(bunch)
        print geo.name
        sys.stdout.flush()
        return geo

    def build_test_data(self, configurations):
        """
        Build and run a full reference data set from a list of configurations
        """
        fout_name = geometry.ref_data(self.__code.get_output_filename())
        fout = open(fout_name, 'w')
        print >> fout, '[\n'
        for config in configurations:
            try:
                geo = self.build_geometry(config)
                print >> fout, repr(geo),','
            except Exception: #pylint: disable=W0703
                print 'Caught exception in configuration ', config, \
                                                        'code', str(self.__code)
                sys.excepthook(*sys.exc_info())
        print >> fout, '\n]\n'
        fout.close()

def main():
    """
    Generate tests for each code listed in the program argument
    """
    code_converters = {
        'maus':code_setup.MausSetup(),
        'icool':code_setup.IcoolSetup(),
        'g4bl':code_setup.G4blSetup(),
    }
    if len(sys.argv) == 1:
        print """
Error - call using
 generate_reference_data.py <code_1> <code_2> <...>
where <code_n> is one of"""
        for key in code_converters.keys():
            print "   ", key
        print
    for arg in sys.argv[1:]:
        test_factory = TestFactory(code_converters[arg])
        test_factory.build_test_data(MICE_CONFIGURATIONS)
    sys.exit(0)

if __name__ == '__main__':
    main()
