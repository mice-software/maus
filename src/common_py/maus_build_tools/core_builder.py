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
Tools to build core library (libMausCpp) and core library unit tests
"""

import os
import glob
import shutil
import SCons # pylint: disable=F0401

MAUS_ROOT_DIR = os.environ['MAUS_ROOT_DIR']

def install_python_tests(maus_root_dir, env):
    """
    Install python test files

    Installs files tests/py_unit/test_*.py tests/py_unit/*/test_*.py. Installs
    into build/, preserving directory structure below tests/py_unit. Also 
    installs files tests/style/*.py into build/ 
    """
    style = "%s/tests/style/" % maus_root_dir
    target = "%s/tests/py_unit/" % maus_root_dir
    build  = "%s/build/" % maus_root_dir
    files = glob.glob(target+'test_*.py')+glob.glob(style+'*.py')
    env.Install(build, files)

    test_subdirs = glob.glob(target+"*")
    for subdir in test_subdirs:
        if os.path.isdir(subdir):
            pos = len(target)
            subdir_mod = subdir[pos:]
            test_files = glob.glob(subdir+"/test_*.py")                   
            env.Install(build+subdir_mod, test_files)

def duplicate_dylib_as_so(target, source, env): # pylint: disable=W0613
    """
    Wrapper for shutil.copyfile for dylib2so tool

    @param source - the file source
    @param target - the target file
    @param env - the SCons environment
    """
    print "Duplicating %s as %s." % (source[0], target[0])
    shutil.copyfile(str(source[0]), str(target[0]))
    return None


DYLIB2SO = SCons.Builder.Builder(action = duplicate_dylib_as_so,
                                 suffix = '.so',
                                 src_suffix = '.dylib')

def build_lib_maus_cpp(env):
    """
    Build main cpp library

    Build libMausCpp.so shared object - containing all the code in 
    src/common_cpp/* and src/legacy/*
    """

    env.Append(BUILDERS = {'Dylib2SO' : DYLIB2SO}) # pylint: disable-msg=E0602

    common_cpp_files = glob.glob("src/legacy/*/*cc") + \
        glob.glob("src/legacy/*/*/*cc") + \
        glob.glob("src/common_cpp/*/*cc") + \
        glob.glob("src/common_cpp/*/*/*cc")

    targetpath = 'src/common_cpp/libMausCpp'
    maus_cpp = env.SharedLibrary(target = targetpath,
                                 source = common_cpp_files,
                                 LIBS=env['LIBS'] + ['recpack'])
    env.Install("build", maus_cpp)
    #Build an extra copy with the .dylib extension for linking on OS X
    if (os.uname()[0] == 'Darwin'):
        maus_cpp_so = env.Dylib2SO(targetpath)
        # Depends == SCons global variable??
        env.Depends(maus_cpp_so, maus_cpp) #pylint: disable = E0602
        env.Install("build", maus_cpp_so)


def build_cpp_tests(env):
    """
    Build cpp unit tests

    Build all the unit tests for cpp code - all the stuff from tests/cpp_unit as
    tests/cpp_unit/test_cpp_unit and all the stuff from
    tests/integration/test_optics/src as optics
    """
    env.Append(LIBPATH = 'src/common_cpp')
    env.Append(CPPPATH = MAUS_ROOT_DIR)

    if 'Darwin' in os.environ.get('G4SYSTEM'):
        env.Append(LINKFLAGS=['-undefined', 'suppress','-flat_namespace'])

    test_cpp_files = glob.glob("tests/cpp_unit/*/*cc")+\
        glob.glob("tests/cpp_unit/*cc")

    env.Program(target = 'tests/cpp_unit/test_cpp_unit', \
                source = test_cpp_files, \
                LIBS= env['LIBS'] + ['recpack'] + ['MausCpp'])
    env.Install('build', ['tests/cpp_unit/test_cpp_unit'])

    test_optics_files = glob.glob("tests/integration/test_optics/src/*cc")
    test_optics = env.Program(target = 'tests/integration/test_optics/optics', \
                               source = test_optics_files, \
                               LIBS= env['LIBS'] + ['MausCpp'])
    env.Install('build', test_optics)

