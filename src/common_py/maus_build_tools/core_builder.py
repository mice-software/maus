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
import json
import string # pylint: disable=W0402

MAUS_ROOT_DIR = os.environ['MAUS_ROOT_DIR']
MAX_DIR_DEPTH = 10

def install_python_tests(maus_root_dir, env):
    """
    Install python test files

    Installs files tests/py_unit/test_*.py tests/py_unit/*/test_*.py. Installs
    into build/, preserving directory structure below tests/py_unit. Also 
    installs files tests/style/*.py into build/ 
    """
    target = "%s/tests/py_unit/" % maus_root_dir
    build  = "%s/build/" % maus_root_dir
    files = glob.glob(target+'test_*.py')
    env.Install(build, files)

    test_subdirs = glob.glob(target+"*")
    for dirpath, dirnames, filenames in os.walk(target): # pylint: disable=W0612
        test_files = glob.glob(dirpath+"/*.py")                   
        relative_path = dirpath[len(target):]
        env.Install(build+relative_path, test_files)


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
    # Magic to sort scons quirk when building object files first
    env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME'] = 1
    
    # Find the common_cpp and legacy cpp source files
    legacy_cpp_files = glob.glob("src/legacy/*/*cc") + \
        glob.glob("src/legacy/*/*/*cc")
    common_cpp_files = []
    for i in range(1, MAX_DIR_DEPTH):
        cpp_name = "src/common_cpp/"+"*/"*i+"*cc"
        common_cpp_files += glob.glob(cpp_name)
    
    # Set up environments for both common_cpp and legacy
    legacy_obj_env = env.Clone()
    legacy_obj_env.Append(CCFLAGS=["""-fpermissive""", """-fPIC"""])
    
    common_obj_env = env.Clone()
    common_obj_env.Append(CCFLAGS=["""-fPIC""", """-std=c++11"""])
    
    # Build the object files
    common_cpp_obj = common_obj_env.Object(common_cpp_files)
    legacy_cpp_obj = legacy_obj_env.Object(legacy_cpp_files)
   
    # Build the shared library and install 
    all_cpp_obj = common_cpp_obj + legacy_cpp_obj
    targetpath = 'src/common_cpp/libMausCpp'
    maus_cpp = common_obj_env.SharedLibrary(target = targetpath,
                                            source = all_cpp_obj,
                                            LIBS=env['LIBS'])
    env.Install("build", maus_cpp)
    #Build an extra copy with the .dylib extension for linking on OS X
    if (os.uname()[0] == 'Darwin'):
        maus_cpp_so = env.Dylib2SO(targetpath)
        # Depends == SCons global variable??
        Depends(maus_cpp_so, maus_cpp) #pylint: disable = E0602
        env.Install("build", maus_cpp_so)

def camelback_to_underscores(string_camelback):
    """
    Convert from ANameLikeThis to a_name_like_this (python module naming
    convention)
    """
    string_underscores = string_camelback[0].lower()
    for char in string_camelback[1:]:
        if char in string.ascii_uppercase:
            string_underscores += '_'+char.lower()
        else:
            string_underscores += char
    return string_underscores

def build_python_modules(env):
    """
    Build python modules

    For each *.cc file in src/common_cpp/Python build a corresponding *.so file.
    These should be python modules. Depends on libMausCpp.so so that needs to be
    built first.
    """
    target_files = glob.glob("src/py_cpp/*cc")
    target_files += glob.glob("src/py_cpp/*/*cc")
    init_all = []

    for ccpath in target_files:
        path, libname = os.path.split(ccpath)
        # strip 'Py', '.cc', make lowercase, add underscores
        libname = camelback_to_underscores(libname[2:-3])
        targetpath = os.path.join(path, libname)
        maus_cpp = env.SharedLibrary(target = targetpath,
                               source = ccpath,
                               LIBS=env['LIBS'] + ['libMausCpp'])
        init_all.append(libname)
        if not os.path.exists("build/maus_cpp"):
            os.mkdir("build/maus_cpp")
        env.Install("build/maus_cpp", maus_cpp)
        #Build an extra copy with the .dylib extension for linking on OS X
        if (os.uname()[0] == 'Darwin'):
            maus_cpp_so = env.Dylib2SO(targetpath)
            # Depends == SCons global variable??
            Depends(maus_cpp_so, maus_cpp) #pylint: disable = E0602
            env.Install("build", maus_cpp_so)
    init = open("build/maus_cpp/__init__.py", 'w')
    print >> init, '__all__ =', json.dumps(init_all)
    init.close()

def build_cpp_tests(env, module_list):
    """
    Build cpp unit tests

    Build all the unit tests for cpp code - all the stuff from tests/cpp_unit as
    tests/cpp_unit/test_cpp_unit
    """
    env.Append(LIBPATH = 'src/common_cpp')
    env.Append(CPPPATH = MAUS_ROOT_DIR)

    if 'Darwin' in os.environ.get('G4SYSTEM'):
        env.Append(LINKFLAGS=['-undefined', 'suppress','-flat_namespace'])

    test_cpp_files = glob.glob("tests/cpp_unit/*cc")+\
                     glob.glob("tests/cpp_unit/*/*cc")+\
                     glob.glob("tests/cpp_unit/*/*/*cc")+\
                     glob.glob("tests/cpp_unit/*/*/*/*cc")

    env.Program(target = 'tests/cpp_unit/test_cpp_unit', \
                source = test_cpp_files, \
                LIBS= env['LIBS'] + ['MausCpp'])#+module_list)
    env.Install('build', ['tests/cpp_unit/test_cpp_unit'])

    test_tof_files = glob.glob\
                ("tests/integration/test_simulation/test_tof/src/*cc")
    tof_mc_plotter = env.Program(target = \
                'tests/integration/test_simulation/test_tof/tof_mc_plotter', \
                source = test_tof_files, \
                LIBS= env['LIBS'] + ['MausCpp'] + module_list)
    env.Install('build', tof_mc_plotter)

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
    data_items = glob.glob(data_struct+'*.hh')
    data_items.extend(glob.glob(data_struct+'Global/*.hh'))
    data_items.extend(glob.glob(data_struct+'ImageData/*.hh'))
    data_items = [item for item in data_items if item[-7:] != '-inl.hh']
    # LinkDef.hh must be last
    data_items.sort(key = lambda x: x.find('LinkDef.hh')) 
    data_items = filter(lambda x: x[-7:] != '-inl.hh', data_items)
    dict_target = (data_struct+'/MausDataStructure.cc')
    proc_target = ['rootcint']+['-f', dict_target, '-c']
    for include in env['CPPPATH']:
        proc_target.append('-I'+include)
    proc_target += data_items
    print proc_target
    proc = subprocess.Popen(proc_target)
    proc.wait()
    os.chdir(here)


