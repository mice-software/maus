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

import ROOT
import glob
import sys
import os

mrd = os.environ['MAUS_ROOT_DIR']

sys.path.insert(0, mrd+'/tests/integration/test_simulation/test_physics_model_full')# adds this to path
sys.path.insert(0, mrd+'/tests/integration/plots/test_physics_model_full')# adds this to path

#imports files, if code imported it is not run
import plotter
import geometry
import all_tests
import code_setup
from factory import TestFactory

import Configuration  # MAUS configuration (datacards) module
import maus_cpp.globals as maus_globals # MAUS C++ calls
import maus_cpp.field as field # MAUS field map calls
import xboa.Common as Common # xboa post-processor library


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

LH2_ONLY_CONFIGURATIONS = [ # icool_elms recommends step size of 5 mm
{'__material__':'lH2' , '__thickness__':350.,  '__momentum__':200.,
 '__pid__':-13, '__step__':5., '__nev__':100000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':400., 
 '__pid__':-13, '__step__':5., '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':800., 
 '__pid__':-13, '__step__':5., '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':200., 
 '__pid__':-13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':200., 
 '__pid__':-13, '__step__':1., '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':200., 
 '__pid__':-13, '__step__':10., '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':350.,  '__momentum__':200., 
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
]

LH2_CONFIGURATION = [ # icool_elms recommends step size of 5 mm
{'__material__':'lH2' , '__thickness__':576.,  '__momentum__':207.,
 '__pid__':13, '__step__':.1, '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':576.,  '__momentum__':207., 
 '__pid__':13, '__step__':0.5, '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':576.,  '__momentum__':207., 
 '__pid__':13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':576.,  '__momentum__':207., 
 '__pid__':13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':576.,  '__momentum__':207., 
 '__pid__':13, '__step__':30.0, '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':576.,  '__momentum__':207., 
 '__pid__':13, '__step__':50., '__nev__':10000, '__seed__':1},
{'__material__':'lH2',  '__thickness__':576.,  '__momentum__':207., 
 '__pid__':13, '__step__':100., '__nev__':10000, '__seed__':1},
]


LITHIUM_HYDRIDE_CONFIGURATION= [ 
{'__material__':'LITHIUM_HYDRIDE' , '__thickness__':63.,  '__momentum__':207.,
 '__pid__':13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':63.,  '__momentum__':207., 
 '__pid__':13, '__step__':0.5, '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':63.,  '__momentum__':207., 
 '__pid__':13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':63.,  '__momentum__':207., 
 '__pid__':13, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':63.,  '__momentum__':207., 
 '__pid__':13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':63.,  '__momentum__':207., 
 '__pid__':13, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'LITHIUM_HYDRIDE',  '__thickness__':63.,  '__momentum__':207., 
 '__pid__':13, '__step__':100.0, '__nev__':10000, '__seed__':1},
]

BE_CONFIGURATION= [ 
{'__material__':'Be' , '__thickness__':23,  '__momentum__':207.,
 '__pid__':13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':23,  '__momentum__':207., 
 '__pid__':13, '__step__':0.5, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':23,  '__momentum__':207., 
 '__pid__':13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':23,  '__momentum__':207., 
 '__pid__':13, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':23,  '__momentum__':207., 
 '__pid__':13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':23,  '__momentum__':207., 
 '__pid__':13, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':23,  '__momentum__':207., 
 '__pid__':13, '__step__':100., '__nev__':10000, '__seed__':1},
]

FE_CONFIGURATION= [ 
{'__material__':'Fe' , '__thickness__':1.1,  '__momentum__':207.,
 '__pid__':13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'Fe',  '__thickness__':1.1,  '__momentum__':207., 
 '__pid__':13, '__step__':0.5, '__nev__':10000, '__seed__':1},
{'__material__':'Fe',  '__thickness__':1.1,  '__momentum__':207., 
 '__pid__':13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'Fe',  '__thickness__':1.1,  '__momentum__':207., 
 '__pid__':13, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'Fe',  '__thickness__':1.1,  '__momentum__':207., 
 '__pid__':13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'Fe',  '__thickness__':1.1,  '__momentum__':207., 
 '__pid__':13, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'Fe',  '__thickness__':1.1,  '__momentum__':207., 
 '__pid__':13, '__step__':100., '__nev__':10000, '__seed__':1},
]

AL_CONFIGURATION= [ 
{'__material__':'Al' , '__thickness__':5.77,  '__momentum__':207.,
 '__pid__':13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'Al',  '__thickness__':5.77,  '__momentum__':207., 
 '__pid__':13, '__step__':0.5, '__nev__':10000, '__seed__':1},
{'__material__':'Al',  '__thickness__':5.77,  '__momentum__':207., 
 '__pid__':13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'Al',  '__thickness__':5.77,  '__momentum__':207., 
 '__pid__':13, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'Al',  '__thickness__':5.77,  '__momentum__':207., 
 '__pid__':13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'Al',  '__thickness__':5.77,  '__momentum__':207., 
 '__pid__':13, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'Al',  '__thickness__':5.77,  '__momentum__':207., 
 '__pid__':13, '__step__':100., '__nev__':10000, '__seed__':1},
]

CU_CONFIGURATION= [ 
{'__material__':'Cu' , '__thickness__':0.93,  '__momentum__':207.,
 '__pid__':13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'Cu',  '__thickness__':0.93,  '__momentum__':207., 
 '__pid__':13, '__step__':0.8, '__nev__':10000, '__seed__':1},
{'__material__':'Cu',  '__thickness__':0.93,  '__momentum__':207., 
 '__pid__':13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'Cu',  '__thickness__':0.93,  '__momentum__':207., 
 '__pid__':13, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'Cu',  '__thickness__':0.93,  '__momentum__':207., 
 '__pid__':13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'Cu',  '__thickness__':0.93,  '__momentum__':207., 
 '__pid__':13, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'Cu',  '__thickness__':0.93,  '__momentum__':207., 
 '__pid__':13, '__step__':100., '__nev__':10000, '__seed__':1},
]

LI_CONFIGURATION= [ 
{'__material__':'Li' , '__thickness__':100.58,  '__momentum__':207.,
 '__pid__':13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'Li',  '__thickness__':100.58,  '__momentum__':207., 
 '__pid__':13, '__step__':0.8, '__nev__':10000, '__seed__':1},
{'__material__':'Li',  '__thickness__':100.58,  '__momentum__':207., 
 '__pid__':13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'Li',  '__thickness__':100.58,  '__momentum__':207., 
 '__pid__':13, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'Li',  '__thickness__':100.58,  '__momentum__':207., 
 '__pid__':13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'Li',  '__thickness__':100.58,  '__momentum__':207., 
 '__pid__':13, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'Li',  '__thickness__':100.58,  '__momentum__':207., 
 '__pid__':13, '__step__':100., '__nev__':10000, '__seed__':1},
]

C_CONFIGURATION= [ 
{'__material__':'C' , '__thickness__':13.85,  '__momentum__':207.,
 '__pid__':13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'C',  '__thickness__':13.85,  '__momentum__':207., 
 '__pid__':13, '__step__':0.8, '__nev__':10000, '__seed__':1},
{'__material__':'C',  '__thickness__':13.85,  '__momentum__':207., 
 '__pid__':13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'C',  '__thickness__':13.85,  '__momentum__':207., 
 '__pid__':13, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'C',  '__thickness__':13.85,  '__momentum__':207., 
 '__pid__':13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'C',  '__thickness__':13.85,  '__momentum__':207., 
 '__pid__':13, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'C',  '__thickness__':13.85,  '__momentum__':207., 
 '__pid__':13, '__step__':100., '__nev__':10000, '__seed__':1},
]

PROTON_CONFIGURATION= [ 
{'__material__':'Be' , '__thickness__':0.000702,  '__momentum__':207,
 '__pid__':2212, '__step__':0.0001, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.000702,  '__momentum__':207., 
 '__pid__':2212, '__step__':0.0005, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.000702,  '__momentum__':207., 
 '__pid__':2212, '__step__':0.001, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.000702,  '__momentum__':207., 
 '__pid__':2212, '__step__':0.005, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.000702,  '__momentum__':207., 
 '__pid__':2212, '__step__':0.01, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.000702,  '__momentum__':207., 
 '__pid__':2212, '__step__':0.05, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.000702,  '__momentum__':207., 
 '__pid__':2212, '__step__':0.1, '__nev__':10000, '__seed__':1},
]


TI_CONFIGURATION= [ 
{'__material__':'Ti' , '__thickness__':2.31,  '__momentum__':207.,
 '__pid__':2212, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'Ti',  '__thickness__':2.31,  '__momentum__':207., 
 '__pid__':2212, '__step__':0.5, '__nev__':10000, '__seed__':1},
{'__material__':'Ti',  '__thickness__':2.31,  '__momentum__':207., 
 '__pid__':2212, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'Ti',  '__thickness__':2.31,  '__momentum__':207., 
 '__pid__':2212, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'Ti',  '__thickness__':2.31,  '__momentum__':207., 
 '__pid__':2212, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'Ti',  '__thickness__':2.31,  '__momentum__':207., 
 '__pid__':2212, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'Ti',  '__thickness__':2.31,  '__momentum__':207., 
 '__pid__':2212, '__step__':100., '__nev__':10000, '__seed__':1},
]




NI_CONFIGURATION= [ 
{'__material__':'Ni' , '__thickness__':0.92,  '__momentum__':207.,
 '__pid__':-13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'Ni',  '__thickness__':0.92,  '__momentum__':207., 
 '__pid__':-13, '__step__':0.5, '__nev__':10000, '__seed__':1},
{'__material__':'Ni',  '__thickness__':0.92,  '__momentum__':207., 
 '__pid__':-13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'Ni',  '__thickness__':0.92,  '__momentum__':207., 
 '__pid__':-13, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'Ni',  '__thickness__':0.92,  '__momentum__':207., 
 '__pid__':-13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'Ni',  '__thickness__':0.92,  '__momentum__':207., 
 '__pid__':-13, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'Ni',  '__thickness__':0.92,  '__momentum__':207., 
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
]

SI_CONFIGURATION= [ 
{'__material__':'Si' , '__thickness__':6.08,  '__momentum__':207.,
 '__pid__':13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'Si',  '__thickness__':6.08,  '__momentum__':207., 
 '__pid__':13, '__step__':0.5, '__nev__':10000, '__seed__':1},
{'__material__':'Si',  '__thickness__':6.08,  '__momentum__':207., 
 '__pid__':13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'Si',  '__thickness__':6.08,  '__momentum__':207., 
 '__pid__':13, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'Si',  '__thickness__':6.08,  '__momentum__':207., 
 '__pid__':13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'Si',  '__thickness__':6.08,  '__momentum__':207., 
 '__pid__':13, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'Si',  '__thickness__':6.08,  '__momentum__':207., 
 '__pid__':13, '__step__':100., '__nev__':10000, '__seed__':1},
]

CR_CONFIGURATION= [ 
{'__material__':'Cr' , '__thickness__':1.3,  '__momentum__':207.,
 '__pid__':-13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'Cr',  '__thickness__':1.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':0.5, '__nev__':10000, '__seed__':1},
{'__material__':'Cr',  '__thickness__':1.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'Cr',  '__thickness__':1.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'Cr',  '__thickness__':1.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'Cr',  '__thickness__':1.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'Cr',  '__thickness__':1.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
]


P_CONFIGURATION= [ 
{'__material__':'P' , '__thickness__':6.3,  '__momentum__':207.,
 '__pid__':-13, '__step__':0.1, '__nev__':10000, '__seed__':1},
{'__material__':'P',  '__thickness__':6.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':0.5, '__nev__':10000, '__seed__':1},
{'__material__':'P',  '__thickness__':6.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':1.0, '__nev__':10000, '__seed__':1},
{'__material__':'P',  '__thickness__':6.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':5.0, '__nev__':10000, '__seed__':1},
{'__material__':'P',  '__thickness__':6.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':10.0, '__nev__':10000, '__seed__':1},
{'__material__':'P',  '__thickness__':6.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':50.0, '__nev__':10000, '__seed__':1},
{'__material__':'P',  '__thickness__':6.3,  '__momentum__':207., 
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
]


BE_PLUG_CONFIGURATIONS = [
{'__material__':'Be',   '__thickness__':100., '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'Be',   '__thickness__':100., '__momentum__':200.,
 '__pid__':2212, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'Be',   '__thickness__':100., '__momentum__':400.,
 '__pid__':2212, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'Be',   '__thickness__':100., '__momentum__':800.,
 '__pid__':2212, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'Be',   '__thickness__':100., '__momentum__':200.,
 '__pid__':211, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'Be',   '__thickness__':100., '__momentum__':200.,
 '__pid__':-11, '__step__':100., '__nev__':10000, '__seed__':1},
{'__material__':'Be',   '__thickness__':200., '__momentum__':200.,
 '__pid__':-13, '__step__':100., '__nev__':10000, '__seed__':1},
]

TIMS_CONFIGURATION= [ 
{'__material__':'lH2' , '__thickness__':576.,  '__momentum__':207.,
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'Li',  '__thickness__':100.,  '__momentum__':207., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':23.,  '__momentum__':207., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'C',  '__thickness__':13.85,  '__momentum__':207., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'Al',  '__thickness__':5.77,  '__momentum__':207., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'Ti',  '__thickness__':2.31,  '__momentum__':207., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'Cu',  '__thickness__':0.93,  '__momentum__':207., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'Fe',  '__thickness__':1.1,  '__momentum__':207., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
]

MUSCAT_CONFIGURATION= [ 
{'__material__':'lH2' , '__thickness__':109.,  '__momentum__':172.,
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'Li',  '__thickness__':12.78,  '__momentum__':172., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':3.73,  '__momentum__':172., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'C',  '__thickness__':2.50,  '__momentum__':172., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'Al',  '__thickness__':1.5,  '__momentum__':172., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
{'__material__':'Fe',  '__thickness__':0.24,  '__momentum__':172., 
 '__pid__':13, '__step__':1, '__nev__':10000, '__seed__':1},
]



NEW2_CONFIGURATION= [ 
{'__material__':'lH2' , '__thickness__':109,  '__momentum__':172.,
 '__pid__':13, '__step__':1, '__nev__':100000, '__seed__':1},
]

BE_FOIL_CONFIGURATION= [ 
{'__material__':'Be' , '__thickness__':0.000702,  '__momentum__':11.,
 '__pid__':2212, '__step__':0.000001, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.000702,  '__momentum__':11., 
 '__pid__':2212, '__step__':0.000005, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.000702,  '__momentum__':11., 
 '__pid__':2212, '__step__':0.00001, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.000702,  '__momentum__':11., 
 '__pid__':2212, '__step__':0.00005, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.000702,  '__momentum__':11., 
 '__pid__':2212, '__step__':0.0001, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.000702,  '__momentum__':11., 
 '__pid__':2212, '__step__':0.0005, '__nev__':10000, '__seed__':1},
{'__material__':'Be',  '__thickness__':0.0007023,  '__momentum__':11., 
 '__pid__':2212, '__step__':0.001, '__nev__':10000, '__seed__':1},
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
{'__material__':'STEEL304', '__thickness__':0.5, '__mo10000mentum__':200.,
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
                    [0]*N_KS_DIVS, 10000, 1.0, 10000,0.0,0.01, N_KS_DIVS),
   # all_tests.KSTest.new('pz', 'MeV/c', [-50.+x for x in range(N_KS_DIVS+1)],
                   #  [0]*N_KS_DIVS, 10000, 1.0, 10000, 0.0, 0.01, N_KS_DIVS),
  # all_tests.KSTest.new('energy', 'MeV', [202.+x/4. for x in range(N_KS_DIVS+1)],
                 #   [0]*N_KS_DIVS, 10000, 1.0, 10000, 0.0, 0.01, N_KS_DIVS),
  
 
]

def main():
    """
    Generate tests for each code listed in the program argument
    """
    code_converters = {
        'maus':code_setup.MausSetup,
        'icool':code_setup.IcoolSetup,
        'icool_elms':code_setup.IcoolElmsSetup,
        'g4bl':code_setup.G4blSetup,
        'muon1':code_setup.Muon1Setup,
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
        if arg == 'muon1' or arg == 'icool_elms':
            test_factory.build_test_data(LH2_ONLY_CONFIGURATIONS)
        else:
            test_factory.build_test_data(MUSCAT_CONFIGURATION)
    sys.exit(0)

if __name__ == '__main__':
    main()
