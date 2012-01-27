import os
import glob

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

def build_lib_maus_cpp(env):
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
      Depends(maus_cpp_so, maus_cpp)
      env.Install("build", maus_cpp_so)


def build_cpp_tests(env):
    env.Append(LIBPATH = 'src/common_cpp')
    env.Append(CPPPATH = MAUS_ROOT_DIR)

    if 'Darwin' in os.environ.get('G4SYSTEM'):
       env.Append(LINKFLAGS=['-undefined','suppress','-flat_namespace'])

    test_cpp_files = glob.glob("tests/cpp_unit/*/*cc")+\
        glob.glob("tests/cpp_unit/*cc")

    testmain = env.Program(target = 'tests/cpp_unit/test_cpp_unit', \
                               source = test_cpp_files, \
                               LIBS= env['LIBS'] + ['recpack'] + ['MausCpp'])
    env.Install('build', ['tests/cpp_unit/test_cpp_unit'])

    test_optics_files = glob.glob("tests/integration/test_optics/src/*cc")
    test_optics = env.Program(target = 'tests/integration/test_optics/optics', \
                               source = test_optics_files, \
                               LIBS= env['LIBS'] + ['MausCpp'])
    env.Install('build', test_optics)

