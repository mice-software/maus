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

The idea is to make the tests fairly independent. Note that I did end up with
some test-dependent code in build_geometry (need to fettle the axes for KSTest)
"""


import operator
import sys

import xboa.Common as Common

from physics_model_test import geometry

class TestFactory:
    """
    Class to generate reference test data from a list of substitutions.

    The reference dataset contains sufficient information to then rerun on each
    new version.
    """
    def __init__(self, code, my_tests):
        self._code = code
        self._tests  = my_tests
        self._conversions = {
                '__thickness__':self._code.convert_position,
                '__step__':self._code.convert_position,
                '__momentum__':self._code.convert_momentum,
                '__material__':self._code.convert_material,
                '__pid__':self._code.convert_pid,
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
            if key in self._conversions.keys():
                config[key] = self._conversions[key](value)
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
        return str(self._code)+': '+_dt+' mm '+_mat+' with '+_nev+' '+\
                _pz+' MeV/c '+_pid+' '+_dz+' mm steps'

    def code_parameters(self):
        """Controls how each executable will be called by MAUS"""
        code_parameters = (
            self._code.get_executable(),
            self._code.get_parameters(),
            self._code.get_substitutions()
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
        geo.bunch_index = self._code.get_bunch_index()
        geo.bunch_read = self._code.get_bunch_read_keys()
        geo.tests = self._tests
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
            test.bins = [xmin+(xmax-xmin)*x/float(test.n_bins) \
                                              for x in range(test.n_bins+1)]
            geo.tests[i]  = test.run_test(bunch)
        print geo.name
        sys.stdout.flush()
        return geo

    def build_test_data(self, configurations):
        """
        Build and run a full reference data set from a list of configurations
        """
        fout_name = geometry.ref_data(self._code.get_output_filename())
        fout = open(fout_name, 'w')
        print >> fout, '[\n'
        for config in configurations:
            try:
                geo = self.build_geometry(config)
                print >> fout, repr(geo),','
            except Exception: #pylint: disable=W0703
                print 'Caught exception in configuration ', config, \
                                                        'code', str(self._code)
                sys.excepthook(*sys.exc_info())
        print >> fout, '\n]\n'
        fout.close()

