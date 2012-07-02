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
import subprocess

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
            test_files = glob.glob(subdir+"/*.py")                   
            env.Install(build+subdir_mod, test_files)

def build_lib_maus_cpp(env):
    """
    Build main cpp library

    Build libMausCpp.so shared object - containing all the code in 
    src/common_cpp/* and src/legacy/*
    """
    common_cpp_files = glob.glob("src/legacy/*/*cc") + \
        glob.glob("src/legacy/*/*/*cc") + \
        glob.glob("src/common_cpp/*/*cc") + \
        glob.glob("src/common_cpp/*/*/*cc")

    targetpath = 'src/common_cpp/libMausCpp'
    maus_cpp = env.SharedLibrary(target = targetpath,
                                 source = common_cpp_files,
                                 LIBS=env['LIBS'])
    env.Install("build", maus_cpp)
    #Build an extra copy with the .dylib extension for linking on OS X
    if (os.uname()[0] == 'Darwin'):
        maus_cpp_so = env.Dylib2SO(targetpath)
        # Depends == SCons global variable??
        Depends(maus_cpp_so, maus_cpp) #pylint: disable = E0602
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
                LIBS= env['LIBS'] + ['MausCpp'])
    env.Install('build', ['tests/cpp_unit/test_cpp_unit'])

    test_optics_files = glob.glob("tests/integration/test_optics/src/*cc")
    test_optics = env.Program(target = 'tests/integration/test_optics/optics', \
                               source = test_optics_files, \
                               LIBS= env['LIBS'] + ['MausCpp'])
    env.Install('build', test_optics)

def build_data_structure(env):
    """
    Build the data structure library

    ROOT needs a .so containing the data structure
    class symbols and this needs to be ported to wherever the data structure is
    called. We build this by calling root and letting it do it's dynamic linker 
    stuff
    """
    maus_root_dir = os.environ['MAUS_ROOT_DIR']
    data_struct = os.path.join(maus_root_dir, 'src/common_cpp/DataStructure/') 
    here = os.getcwd()
    os.chdir(maus_root_dir)
    data_items = glob.glob(data_struct+'/*.hh')
    data_items = [item for item in data_items if item[-7:] != '-inl.hh']
    # LinkDef.hh must be last
    data_items.sort(key = lambda x: x.find('LinkDef.hh')) 
    dict_target = (data_struct+'/MausDataStructure.cc')
    proc_target = ['rootcint']+['-f', dict_target, '-c']
    for include in env['CPPPATH']:
        proc_target.append('-I'+include)
    proc_target += data_items
    print proc_target
    proc = subprocess.Popen(proc_target)
    proc.wait()
    os.chdir(here)


