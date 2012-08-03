#This file is a part of g4mice
#
#g4mice is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#g4mice is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with g4mice in the doc folder.  If not, see 
#<http://www.gnu.org/licenses/>.
#

"""
generate_reference_data contains a list of data and few functions for
generating reference tests and geometry sets
"""


#import test_physics_model_tools
import code_comparison as cc
import os
import xboa.Common as Common
import operator
import sys
import exceptions

#TODO: pylint:disable=W0511
# * This is a bit of a mess and needs cleaning up. Should separate the data from
#   the (generic) code. Probably do this when I want to add another set of
#   tests. One way to do it would be to make a geometry_factory class that takes
#   a list of substitutions.
#
# * For the actual physics list tests, I should add a test on emittance change,
#   on output moments, on mean energy loss vs pdg formula, on mcs vs pdg
#   formula, probably some other stuff
#

MICE_CONFIGURATIONS = [ #thickness in mm, momentum in MeV/c
{'__material__':'lH2' , '__thickness__':350.,  '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
]
void = [
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
    Class to generate reference test data from a list of substitutions. The 
    reference dataset contains sufficient information to then rerun on each new
    version.
    """
    maus   = 'maus'
    g4bl   = 'g4bl'
    icool  = 'icool'
    g4mice = 'g4mice_simulation'
    codes  = (maus, g4bl, icool, g4mice)

    n_ks_divs = 500

    tests = [
      cc.KSTest.new('px',     'MeV/c', [-50.+x for x in range(n_ks_divs+1)],
                                   [0]*n_ks_divs, 10000, 1.0, 10000, 0.0, 0.01),
      cc.KSTest.new('energy', 'MeV', [202.+x/4. for x in range(n_ks_divs+1)],
                                   [0]*n_ks_divs, 10000, 1.0, 10000, 0.0, 0.01)
    ]

    #list of configuration substitutions
    pos_keys = ['__thickness__', '__step__'] #dimensions of position
    mom_keys = ['__momentum__'] #dimensions of momentum
    mat_keys = ['__material__'] #materials (encoding between ICOOL and G4)
    pid_keys = ['__pid__'] #pid (different encoding between ICOOL and G4)
    no_keys  = ['__nev__','__seed__'] #not code dependent

    g4mice_material_to_icool = {
    'lH2':'LH', 'LITHIUM_HYDRIDE':'LIH', 'Al':'AL', 'Be':'BE', 'lHe':'LHE',
    'STEEL304':'FE','Cu':'CU'
    }

    g4mice_material_to_g4bl = {
    'lH2':'LH2', 'LITHIUM_HYDRIDE':'LITHIUM_HYDRIDE', 'Al':'Al', 'Be':'Be',
    'lHe':'lHe', 'STEEL304':'Fe','Cu':'Cu'
    }

    def __init__(self, code):
        if not code in self.codes:
            raise NotImplementedError(
                                   str(code)+" must be one of "+str(self.codes))
        self.__code = code
        try:
            os.mkdir(self.target(''))
        except OSError:
            pass

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
            if   key in self.pos_keys:
                config[key] = self.position(value)  
            elif key in self.mom_keys:
                config[key] = self.momentum(value)  
            elif key in self.mat_keys:
                config[key] = self.material(value)  
            elif key in self.pid_keys:
                config[key] = self.pid(value)  
            elif key in self.no_keys:
                pass  
            else: raise(KeyError('Nothing known about key '+str(key)) )
        return config

    def material(self, value):
        """Convert material with MAUS name given by value to different codes"""
        if self.__code == self.g4mice or self.__code == self.maus:
            return value
        if self.__code == self.g4bl:
            return self.g4mice_material_to_g4bl[value] 
        if self.__code == self.icool:
            return self.g4mice_material_to_icool[value]

    def position(self, value):
        """Convert position with units mm to different codes"""
        if self.__code == self.g4bl or self.__code == self.g4mice or \
           self.__code == self.maus:
            return value
        if self.__code == self.icool:
            return value/Common.units['m']

    def momentum(self, value):
        """Convert momentum with units MeV/c to different codes"""
        if self.__code == self.g4bl or self.__code == self.g4mice or \
           self.__code == self.maus:
            return value
        if self.__code == self.icool:
            return value/Common.units['GeV/c']

    def pid(self, value):
        """Convert from PDG PID to different code units"""
        if self.__code == self.g4bl or self.__code == self.g4mice or \
           self.__code == self.maus:
            return value
        if self.__code == self.icool:
            return Common.pdg_pid_to_icool[value]

    def make_name(self, config):
        """Make a name for this test from substitution keys"""
        _dt  = str(config['__thickness__'])
        _mat = str(config['__material__'])
        _nev = str(config['__nev__'])
        _pz  = str(config['__momentum__'])
        _pid = Common.pdg_pid_to_name[config['__pid__']]
        _dz  = str(config['__step__'])
        return self.__code+': '+_dt+' mm '+_mat+' with '+_nev+' '+\
                _pz+' MeV/c '+_pid+' '+_dz+' mm steps'

    def bunch_read(self):
        """Function call for xboa to read in output data set"""
        bunch_read = {
          self.maus:('maus_root_virtual_hit', 'maus_output.root'),
          self.g4mice:('g4mice_virtual_hit', 'Sim.out.gz'),
          self.icool: ('icool_for009', 'for009.dat'),
          self.g4bl: ('icool_for009', 'for009_g4bl.txt'),
        }
        return bunch_read[self.__code]

    def bunch_index(self):
        """Index of the relevant station containing bunch data to be tested"""
        bunch_index = {self.maus:2, self.g4mice:2, self.icool:5, self.g4bl:2}
        return bunch_index[self.__code]

    def source(self, file_name):
        """Add a prefix pointing to the directory of source files"""
        source_dir = os.path.expandvars("$MAUS_ROOT_DIR/tests/integration/"+
            "test_simulation/test_physics_model_full/test_physics_model_files/")
        return os.path.join(source_dir, self.__code, file_name)

    def target(self, file_name):
        """Add a suffix pointing to the directory of output files"""
        target_dir = self.__code
        return os.path.join(target_dir, file_name)


    def code_parameters(self):
        """Controls how each executable will be called by MAUS"""
        code_parameters = {
          self.maus:(os.path.join('$MAUS_ROOT_DIR', 'bin', 'simulate_mice.py'),
            ['--configuration_file', self.target('configuration.py')],
            {self.source('configuration.in'):self.target('configuration.py'),
             self.source('MaterialBlock.in'):self.target('MaterialBlock.dat')}),
          self.g4mice:(
              os.path.join('$MICESRC','Applications','Simulation','Simulation'),
            [self.target('cards')],
            {self.source('cards.in'):self.target('cards'),
             self.source('MaterialBlock.in'):self.target('MaterialBlock.dat')}),
          self.icool: (os.path.join('$ICOOL'),[],
                  {self.source('for001.in'):self.target('for001.dat')}),
          self.g4bl: (os.path.join('$G4BL'),[self.target('g4bl_deck')],
                  {self.source('g4bl_deck.in'):self.target('g4bl_deck')}),
        }
        return code_parameters[self.__code]

    def build_geometry(self, config, tests):
        """Build and run a set of tests for a specific geometry set up"""
        if not self.__code in self.codes:
            raise(NotImplementedError(
                                'Nothing known about code '+str(self.__code)))
        geo = cc.Geometry()
        #first setup the geometry basics
        pid      = config['__pid__']
        geo.name = self.make_name(config)
        geo.code_model = self.code_parameters()
        geo.substitutions = self.code_convert(config)
        geo.bunch_index = self.bunch_index()
        geo.bunch_read = self.bunch_read()
        geo.tests = tests
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

    def build_reference_data(self, configurations):
        """
        Build and run a full reference data set from a list of configurations
        """
        fout = open(self.__code+'_ref_data.dat', 'w')
        print >> fout, '[\n'
        for config in configurations:
            try:
                geo = self.build_geometry(config, self.tests)
                print >> fout, repr(geo),','
            except Exception: #pylint: disable=W0703
                print 'Caught exception in configuration ', config, \
                                                            'code', self.__code
                sys.excepthook(*sys.exc_info())
        print >> fout, '\n]\n'
        fout.close()

def main():
    """
    Generate tests for each code listed in the program argument
    """
    for arg in sys.argv:
        test_factory = TestFactory(arg)
        test_factory.build_reference_data(MICE_CONFIGURATIONS)
    sys.exit(0)

if __name__ == '__main__':
    main()
