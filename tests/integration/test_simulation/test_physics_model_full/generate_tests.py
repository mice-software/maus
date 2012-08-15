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
Generates tests for subsequent call by test_runner
"""

import sys
import xboa.Common as Common
from physics_model_test import all_tests
from physics_model_test import code_setup
from physics_model_test.factory import TestFactory

def _ke2p(kinetic_energy):
    """
    Convert proton kinetic energy into momentum
    """
    mass = Common.pdg_pid_to_mass[2212]
    momentum = ((kinetic_energy+mass)**2.-mass**2.)**0.5
    return momentum

TEST_CONFIGURATIONS = [ #thickness in mm, momentum in MeV/c
{'__material__':'lH2',   '__thickness__':100., '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
]

LH2_ONLY_CONFIGURATIONS = [
{'__material__':'lH2' , '__thickness__':350.,  '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':400., 
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':800., 
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':200., 
 '__pid__':-13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':200., 
 '__pid__':-13, '__step__':1., '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':200., 
 '__pid__':-13, '__step__':10., '__nev__':10000, '__seed__':1},
]

MICE_CONFIGURATIONS = [
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':100.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':10.,  '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
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
]+LH2_ONLY_CONFIGURATIONS

ERIT_CONFIGURATIONS = [
{'__material__':'C' , '__thickness__':0.0001,  '__momentum__':_ke2p(7.),
 '__pid__':2212, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
{'__material__':'C' , '__thickness__':0.0001,  '__momentum__':_ke2p(8.),
 '__pid__':2212, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
{'__material__':'C' , '__thickness__':0.0001,  '__momentum__':_ke2p(9.),
 '__pid__':2212, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
{'__material__':'C' , '__thickness__':0.0001,  '__momentum__':_ke2p(10.),
 '__pid__':2212, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
{'__material__':'C' , '__thickness__':0.0001,  '__momentum__':_ke2p(11.),
 '__pid__':2212, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
{'__material__':'C' , '__thickness__':0.0001,  '__momentum__':_ke2p(12.),
 '__pid__':2212, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
{'__material__':'C' , '__thickness__':0.0001,  '__momentum__':_ke2p(13.),
 '__pid__':2212, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
{'__material__':'C' , '__thickness__':0.0001,  '__momentum__':_ke2p(14.),
 '__pid__':2212, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
{'__material__':'C' , '__thickness__':0.0001,  '__momentum__':_ke2p(15.),
 '__pid__':2212, '__step__':100., '__nev__':100000, '__seed__':1}, # 100k events
]

N_KS_DIVS = 500
# test factory dynamically decides on n_bins, bin width
TESTS = [
  all_tests.KSTest.new('px', 'MeV/c', [-50.+x for x in range(N_KS_DIVS+1)],
                   [0]*N_KS_DIVS, 10000, 1.0, 10000, 0.0, 0.01, N_KS_DIVS),
  all_tests.KSTest.new('energy', 'MeV', [202.+x/4. for x in range(N_KS_DIVS+1)],
                   [0]*N_KS_DIVS, 10000, 1.0, 10000, 0.0, 0.01, N_KS_DIVS)
]

def main():
    """
    Generate tests for each code listed in the program argument
    """
    code_converters = {
        'maus':code_setup.MausSetup,
        'icool':code_setup.IcoolSetup,
        'g4bl':code_setup.G4blSetup,
    }
    print sys.argv
    if len(sys.argv) == 1:
        print """
Error - call using
 generate_reference_data.py <code_1> <code_2> <...>
where <code_n> is one of"""
        for key in code_converters.keys():
            print "   ", key
        print
    for arg in sys.argv[1:]:
        test_factory = TestFactory(code_converters[arg](), TESTS)
        test_factory.build_test_data(ERIT_CONFIGURATIONS)
    sys.exit(0)

if __name__ == '__main__':
    main()
