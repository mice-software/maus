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
SConscript file controls the MAUS build

SConscript file replaces the standard makefile for the MAUS library. SConscript
uses scons library to handle builds. We have three submodules
- environment_tools contains tools for modifying the environment
- module_builder contains tools for handling modules (input, output, map, reduce) 
- core_builder contains tools for handling the MAUS core (common_cpp, common_py,
  legacy)
"""

import glob
import os
import shutil

import maus_build_tools.environment_tools
import maus_build_tools.core_builder
import maus_build_tools.module_builder

from SCons.Script.SConscript import SConsEnvironment

# check that the environment has been sourced (i.e. MAUS_ROOT_DIR and 
# MAUS_THIRD_PARTY exist)
(maus_root_dir, maus_third_party) = \
                  maus_build_tools.environment_tools.get_environment_variables()

# Setup the scons environment.
env = maus_build_tools.environment_tools.get_environment()

# Setup the module builder
SConsEnvironment.jDev = maus_build_tools.module_builder.ModuleBuilder(env)

# Cleanup anything missed by scons
if env.GetOption('clean'):
    SConsEnvironment.jDev.cleanup_extras()

# make sure the sconscripts can get to the variables - don't need to export
# anything but 'env'. This needs to be done at global scope I think.
Export('env')

# add 'CheckCommand' to check that we can run exe - could just call directly...
conf = Configure(env, custom_tests = {'CheckCommand' : \
                              maus_build_tools.environment_tools.check_command})

# check libraries exist
for lib in ['compiler', 'python', 'gsl', 'root', 'clhep', 'geant4', 'recpack',
           'gtest', 'unpacker']:
    maus_build_tools.environment_tools.set_lib(conf, env, lib)

# build the maus cpp core library (libMausCpp.so)
maus_build_tools.core_builder.build_lib_maus_cpp(env)
# build the cpp tests (test_cpp_unit)
maus_build_tools.core_builder.build_cpp_tests(env)
# install the python tests (pure python, no build to do)
maus_build_tools.core_builder.install_python_tests(maus_root_dir, env)
# build the modules - inputters, mappers, reducers, outputters
stuff_to_import = SConsEnvironment.jDev.build_modules()
# build the MAUS python library (MAUS.py)
SConsEnvironment.jDev.build_maus_lib('%s/build/MAUS.py' % maus_root_dir, stuff_to_import)



