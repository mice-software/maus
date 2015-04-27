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

Call this build script by doing (with third party libraries built)
    source env.sh
    scons
scons has a good man entry. A few useful switches are
    - scons -c cleanup (delete built files)
    - scons -s silent (gcc errors and warnings only)
    - scons -j2 multiprocess with e.g. 2 cores

SConscript file replaces the standard makefile for the MAUS library. SConscript
uses scons library to handle builds. We have three submodules
- environment_tools contains tools for modifying the environment
- module_builder contains tools for handling modules (input, output, map, 
  reduce) 
- core_builder contains tools for handling the MAUS core (common_cpp, common_py,
  legacy)
"""

# this import fails if calling as a python script - call with scons
from SCons.Script.SConscript import SConsEnvironment # pylint: disable=F0401

import maus_build_tools.environment_tools 
import maus_build_tools.core_builder
import maus_build_tools.module_builder

def setup_environment():
    """
    Setup the environment

    Setup the module builder; Check for the existence of third party libraries
    and add them to the includes.
    """
    # check that the environment has been sourced (i.e. MAUS_ROOT_DIR and 
    # MAUS_THIRD_PARTY exist)
    (_maus_root_dir, _maus_third_party) = \
                 maus_build_tools.environment_tools.get_environment_variables()
    # Setup the scons environment.
    env = maus_build_tools.environment_tools.get_environment()
    # make sure the sconscripts can get to the variables - don't need to export
    # anything but 'env'. This needs to be done at global scope I think.
    Export('env') # pylint: disable=E0602
    # add 'CheckCommand' to check that we can run exe - could just call directly
    tests = {'CheckCommand' : maus_build_tools.environment_tools.check_command}
    conf = Configure(env, tests) # pylint: disable=E0602
    # Setup the module builder
    SConsEnvironment.jDev = maus_build_tools.module_builder.ModuleBuilder(env,
                                                                          conf)
    Export('conf') # pylint: disable=E0602
    # check libraries exist; add them into the environment
    for lib in ['compiler', 'python', 'gsl', 'numpy', 'root', 'clhep',
		'xercesc', 'geant4', 'gtest', 'unpacker']:
        maus_build_tools.environment_tools.set_lib(conf, env, lib)
    return _maus_root_dir, env

def build_libraries(maus_root_dir, env, skip_ds):
    """
    Build the maus libraries

    Build core libraries, cpp tests, python tests, python utilities, modules
    """
    if not skip_ds:
        # build the data structure (separate library so we can port it to
        # external deps)
        maus_build_tools.core_builder.build_data_structure(env)
    # build the maus cpp core library (libMausCpp.so)
    maus_build_tools.core_builder.build_lib_maus_cpp(env)
    # install the python tests (pure python, no build to do)
    maus_build_tools.core_builder.install_python_tests(maus_root_dir, env)
    # build the modules - inputters, mappers, reducers, outputters
    (cpp_libs, python_libs) = SConsEnvironment.jDev.register_modules()
    # build the MAUS python library (MAUS.py)
    maus_build_tools.module_builder.build_maus_lib \
                      ('%s/build/MAUS.py' % maus_root_dir, cpp_libs+python_libs)
    # build the cpp tests (test_cpp_unit)
    maus_build_tools.core_builder.build_cpp_tests(env, cpp_libs)
    # build python utilities
    maus_build_tools.core_builder.build_python_modules(env)

def main():
    """
    main function - should be called by SCons
    """
    maus_root_dir_, env_ = setup_environment()

    # SCons can forget to clean stuff if the source file no longer exists
    if env_.GetOption('clean'):
        maus_build_tools.module_builder.cleanup_extras()

    skip_ds = ARGUMENTS.get('skip_ds', False) # pylint: disable=E0602
    build_libraries(maus_root_dir_, env_, skip_ds)

main()

