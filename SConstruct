from SCons.Script.SConscript import SConsEnvironment # pylint: disable-msg=F0401
import glob
import os

def my_exit(code = 1):
    """Internal routine to exit

    Currently this is its own function to allow easy disabling or a pylint
    warning.
    """
    Exit(code) # pylint: disable-msg=E0602

maus_root_dir = os.environ.get('MAUS_ROOT_DIR')
if not maus_root_dir:
    print('!! Could not find the $MAUS_ROOT_DIR environmental variable')
    print('!! Did you try running: "source env.sh"?')
    my_exit(1)

#this is our catch-all Dev class
class Dev:
    cflags = ''
    ecflags = ''

    # sets up the sconscript file for a given sub-project
    def Subproject(self, project):
        SConscript(env.jDev.SPath(project), exports=['project']) # pylint: disable-msg=E0602

    #sets up the build for a given project
    def Buildit(self, localenv, project, \
                    use_root, use_g4, use_unpacker = False):
        if use_root and not env['USE_ROOT']:
            if not env.GetOption('clean'):
                print "The worker", project, \
                    "requires ROOT which is disabled. Skipping..."
            return False

        if use_g4 and not env['USE_G4']:
            if not env.GetOption('clean'):
                print "The worker", project, \
                    "requires Geant4 which is disabled. Skipping..."
            return False

        if use_unpacker and not env['USE_UNPACKER']:
            if not env.GetOption('clean'):
                print "The worker", project, \
                    "requires Unpacker which is disabled. Skipping..."
            return False

        name = project.split('/')[-1]
        builddir = 'build'
        targetpath = os.path.join('build', '_%s' % name)

        #append the user's additional compile flags
        #assume debugcflags and releasecflags are defined
        localenv.Append(CCFLAGS=self.cflags)
        if use_root and use_g4:
            localenv.Append(LIBS=['simulate'])

        #specify the build directory
        localenv.VariantDir(variant_dir=builddir, src_dir='.', duplicate=0)
        localenv.Append(CPPPATH='.')

        srclst = map(lambda x: builddir + '/' + x, glob.glob('*.cpp'))
        srclst += map(lambda x: builddir + '/' + x, glob.glob('*.i'))
        pgm = localenv.SharedLibrary(targetpath, source=srclst)

        tests = glob.glob('test_*.py')

        full_build_dir = os.path.join(maus_root_dir, builddir)

        env.Install(full_build_dir, "build/%s.py" % name)
        env.Install(full_build_dir, pgm)
        env.Install(full_build_dir, tests)
        env.Alias('all', pgm)  #note: not localenv

        return True

    #---- PRIVATE ----

    #---
    # return the sconscript path to use
    def SPath(self, project):
        return project + '/sconscript'

def CheckCommand(context, cmd):
    context.Message('Checking for %s command... ' % cmd)
    result = WhereIs(cmd) # pylint: disable-msg=E0602
    context.Result(result is not None)
    return result

## autoconf-like stuff
def set_cpp(conf, env):
    """
    Sanity checks that the compiler is installed correctly
    """

    if not conf.CheckCXX():
        print('!! Your compiler or environment is not correctly configured.')
        my_exit(1)

    if not conf.CheckLib( "json" , language='C++') or\
            not conf.CheckCXXHeader('json/json.h'):

        print( "!! Can't find jsoncpp which is needed." )
        print ( "You may install it by running:")
        print ("     MAUS_ROOT_DIR=%s ./third_party/bash/52jsoncpp.bash"\
                   % maus_root_dir)
        my_exit(1)

    if not conf.CheckLib( "stdc++" , language='C++'):
        my_exit(1)

    if not conf.CheckFunc('printf'):
        print('!! Your compiler or environment is not correctly configured.')
        my_exit(1)

    if not conf.CheckHeader('math.h'):
        my_exit(1)

    if not conf.CheckCHeader('stdlib.h'):
        my_exit(1)

    if not conf.CheckCXXHeader('iostream', '<>'):
        my_exit(1)

def set_python(conf, env):
    EnsurePythonVersion(2, 7) # pylint: disable-msg=E0602

    if not conf.CheckCommand('python'):
        my_exit(1)

    if not conf.CheckCXXHeader('Python.h'):
        print "You need 'Python.h' to compile this program"
        print "If you want to install python locally, run:"
        print ("     MAUS_ROOT_DIR=%s ./third_party/bash/01python.bash" \
                   % maus_root_dir)
        my_exit(1)

    if not conf.CheckCommand('swig'):
        print "Cound't find swig.  If you want it, then run:"
        print ("     MAUS_ROOT_DIR=%s ./third_party/bash/02swig.bash" \
                   % maus_root_dir)
        my_exit(1)

    conf.env.Append(LIBS = ['python2.7'])

def set_gsl(conf, env):
    if not conf.CheckLib('gslcblas'):
        print "Cound't find GSL (required for ROOT).  If you want it, then run:"
        print ("     MAUS_ROOT_DIR=%s ./third_party/bash/20gsl.bash" \
                   % maus_root_dir)
        my_exit(1)
    else:
        conf.env.Append(LIBS = ['gslcblas'])

    if not conf.CheckLib('gsl'):
        print "Cound't find GSL (required for ROOT).  If you want it, then run:"
        print ("     MAUS_ROOT_DIR=%s ./third_party/bash/20gsl.bash" \
                   % maus_root_dir)
        my_exit(1)

def get_root_libs():
    """ROOT libs

    Important libraries for ROOT to run.  Worth checking for.  The list is not
    exhaustive but just for sanity checks.
    """
    return ['Cint', \
                'Core', \
                'Gpad', \
                'Graf', \
                'Graf3d', \
                'Hist', \
                'MathCore', \
                'Matrix', \
                'Minuit', \
                'Net', \
                'Physics', \
                'Postscript', \
                'RIO', \
                'Rint', \
                'Thread', \
                'Tree', \
                'dl', \
                'm', \
                'pthread']

def set_root(conf, env):
    if not conf.CheckCommand('root'):
        print "Cound't find root.  If you want it, after installing GSL, run:"
        print ("     MAUS_ROOT_DIR=%s ./third_party/bash/21root.bash" \
                   % maus_root_dir)
        print ("where you can install GSL by running:")
        print ("     MAUS_ROOT_DIR=%s ./third_party/bash/20gsl.bash" \
                   % maus_root_dir)
        my_exit(1)

    else:
        print
        print "!! Found the program 'root', so MAUS will use it."
        print
        env['USE_ROOT'] = True

        if not conf.CheckCommand('root-config'):
            my_exit(1)

        conf.env.ParseConfig("root-config --cflags --ldflags --libs")

        root_libs = get_root_libs()

        for lib in root_libs:
            if not conf.CheckLib(lib, language='c++'):
                my_exit(1)

        if not conf.CheckCXXHeader('TH1F.h'):
            my_exit(1)

        if not conf.CheckCXXHeader('TMinuit.h'):
            my_exit(1)

def set_clhep(conf, env):
    if not conf.CheckLib('CLHEP', language='c++'):
        print "Cound't find CLHEP (required for geant4).  If you want it, then run:"
        print ("      MAUS_ROOT_DIR=%s ./third_party/bash/30clhep.bash" % maus_root_dir)
        my_exit(1)

    conf.env.Append(LIBS = ['CLHEP'])

def get_g4_libs():
    return [ 'G4FR', \
             'G4RayTracer', \
             'G4Tree', \
             'G4UIGAG', \
             'G4UIbasic', \
             'G4UIcommon', \
             'G4VRML', \
             'G4baryons', \
             'G4biasing', \
             'G4bosons', \
             'G4brep', \
             'G4csg', \
             'G4cuts', \
             'G4decay', \
             'G4detector', \
             'G4detscorer', \
             'G4detutils', \
             'G4digits', \
             'G4emhighenergy', \
             'G4emlowenergy', \
             'G4empolar', \
             'G4emstandard', \
             'G4emutils', \
             'G4error_propagation', \
             'G4event', \
             'G4geomBoolean', \
             'G4geombias', \
             'G4geomdivision', \
             'G4geometrymng', \
             'G4geomtext', \
             'G4gflash', \
             'G4globman', \
             'G4graphics_reps', \
             'G4had_im_r_matrix', \
             'G4had_lll_fis', \
             'G4had_mod_man', \
             'G4had_mod_util', \
             'G4had_muon_nuclear', \
             'G4had_neu_hp', \
             'G4had_preequ_exciton', \
             'G4had_string_diff', \
             'G4had_string_frag', \
             'G4had_string_man', \
             'G4had_theo_max', \
             'G4hadronic_HE', \
             'G4hadronic_LE', \
             'G4hadronic_RPG', \
             'G4hadronic_ablation', \
             'G4hadronic_abrasion', \
             'G4hadronic_bert_cascade', \
             'G4hadronic_binary', \
             'G4hadronic_body_ci', \
             'G4hadronic_coherent_elastic', \
             'G4hadronic_deex_evaporation', \
             'G4hadronic_deex_fermi_breakup', \
             'G4hadronic_deex_fission', \
             'G4hadronic_deex_gem_evaporation', \
             'G4hadronic_deex_handler', \
             'G4hadronic_deex_management', \
             'G4hadronic_deex_multifragmentation', \
             'G4hadronic_deex_photon_evaporation', \
             'G4hadronic_deex_util', \
             'G4hadronic_em_dissociation', \
             'G4hadronic_hetcpp_evaporation', \
             'G4hadronic_hetcpp_utils', \
             'G4hadronic_incl_cascade', \
             'G4hadronic_interface_ci', \
             'G4hadronic_iso', \
             'G4hadronic_leading_particle', \
             'G4hadronic_mgt', \
             'G4hadronic_proc', \
             'G4hadronic_qgstring', \
             'G4hadronic_qmd', \
             'G4hadronic_radioactivedecay', \
             'G4hadronic_stop', \
             'G4hadronic_util', \
             'G4hadronic_xsect', \
             'G4hepnumerics', \
             'G4hits', \
             'G4intercoms', \
             'G4ions', \
             'G4leptons', \
             'G4magneticfield', \
             'G4materials', \
             'G4mctruth', \
             'G4mesons', \
             'G4modeling', \
             'G4muons', \
             'G4navigation', \
             'G4optical', \
             'G4parameterisation', \
             'G4partadj', \
             'G4partman', \
             'G4partutils', \
             'G4phys_builders', \
             'G4phys_lists', \
             'G4procman', \
             'G4readout', \
             'G4run', \
             'G4scoring', \
             'G4shortlived', \
             'G4specsolids', \
             'G4track', \
             'G4tracking', \
             'G4transportation', \
             'G4visHepRep', \
             'G4visXXX', \
             'G4vis_management', \
             'G4volumes', \
             'G4xrays']


def set_geant4(conf, env):
    if 'G4INSTALL' not in os.environ or (not os.path.exists(os.environ.get('G4INSTALL'))):
        print "Cound't find geant4.  If you want it, after installing CLHEP, then run:"
        print ("      MAUS_ROOT_DIR=%s ./third_party/bash/31geant4.bash" % maus_root_dir)
        my_exit(1)
    else:
        print
        print "!! Found the package 'geant4', so assume you want to use it with MAUS."
        print
        env['USE_G4'] = True

        env.Append(LIBPATH = ["%s/%s" % (os.environ.get('G4LIB'), os.environ.get('G4SYSTEM'))])
        env.Append(CPPPATH=[os.environ.get("G4INCLUDE")])

        if not conf.CheckCXXHeader('G4EventManager.hh'):
            my_exit(1)

        # removing this line (and using the append(libs) one below, because this is messy and breaks/seg-faults.
        #    conf.env.ParseConfig('%s/liblist -m %s < %s/libname.map'.replace('%s', os.path.join(os.environ.get('G4LIB'), os.environ.get('G4SYSTEM'))))


        env.Append(LIBS=get_g4_libs())

        for lib in get_g4_libs():
            if not conf.CheckLib(lib, language='c++'):
                my_exit(1)

def set_recpack(conf, env):
    if not conf.CheckLib('recpack', language='c++') or\
       not conf.CheckCXXHeader('recpack/RecpackManager.h'):
        my_exit(1)

def set_gtest(conf, env):
    if not conf.CheckLib('gtest', language='c++'):
        my_exit(1)
    if not conf.CheckCXXHeader('gtest/gtest.h'):
        my_exit(1)

def set_unpacker(conf, env):
    if (not conf.CheckLib('MDunpack', language='c++') or \
        not  conf.CheckCXXHeader('MDevent.h')):
        print
        print "!! Unpacker module not found, you will not be able to use the RealData module."
        print
    else:
        env["USE_UNPACKER"] = True


# Setup the environment.  NOTE: SHLIBPREFIX means that shared libraries don't
# have a 'lib' prefix, which is needed for python to find SWIG generated libraries
env = Environment(SHLIBPREFIX="") # pylint: disable-msg=E0602

if os.path.isfile('.use_llvm_with_maus'):
    env['CC'] = "llvm-gcc"
    env['CXX'] = "llvm-g++"

env.Tool('swig', '%s/third_party/swig-2.0.1' % maus_root_dir)
env.Append(SWIGFLAGS=['-python', '-c++']) # tell SWIG to make python bindings for C++

#env.Append(PATH="%s/third_party/install/bin" % maus_root_dir)
env['ENV']['PATH'] =  os.environ.get('PATH')  # useful to set for root-config
env['ENV']['LD_LIBRARY_PATH'] = os.environ.get('LD_LIBRARY_PATH')
env['ENV']['DYLD_LIBRARY_PATH'] = os.environ.get('DYLD_LIBRARY_PATH')

libs = str(os.environ.get('LD_LIBRARY_PATH'))+':'+str(os.environ.get('DYLD_LIBRARY_PATH'))

# to find third party libs, includes
env.Append(LIBPATH =  libs.split(':') + ["%s/build" % maus_root_dir])

env.Append(CPPPATH=["%s/third_party/install/include" % maus_root_dir, \
                      "%s/third_party/install/include/python2.7" % maus_root_dir, \
                      "%s/third_party/install/include/root" % maus_root_dir, \
                      "%s/src/common" % maus_root_dir, ""])

env['USE_G4'] = False
env['USE_ROOT'] = False
env['USE_UNPACKER'] = False

#put all .sconsign files in one place
env.SConsignFile()

#we can put variables right into the environment, however
#we must watch out for name clashes.
SConsEnvironment.jDev = Dev()

#get the mode flag from the command line
#default to 'release' if the user didn't specify
env.jDev.mymode = ARGUMENTS.get('mode', 'release')  # pylint: disable-msg=E0602

#check if the user has been naughty: only 'debug' or 'release' allowed
if not (env.jDev.mymode in ['debug', 'release']):
    print "Error: expected 'debug' or 'release', found: " + env.jDev.mymode
    my_exit(1)

#tell the user what we're doing
print '**** Compiling in ' + env.jDev.mymode + ' mode...'

env.jDev.debugcflags = [ '-W1', '-GX', '-D_DEBUG']   #extra compile flags for debug
env.jDev.releasecflags = ['-O2', '-DNDEBUG',]         #extra compile flags for release

#make sure the sconscripts can get to the variables
#don't need to export anything but 'env'
Export('env') # pylint: disable-msg=E0602

#### Target: Documentation
#dox = env.Command('does_not_exist3', 'doc/Doxyfile',
#                  'doxygen doc/Doxyfile && cd doc/html')
#env.Alias('doc', [dox])

#python_executable = '%s/third_party/install/bin/python' % maus_root_dir
#if os.path.isfile(python_executable):
#  unit = env.Command('does_not_exist2', 'build', '%s -m unittest discover -b -v build' % python_executable)
#  env.Alias('unittest', [unit])

print "Configuring..."
# Must have long32 & long64 for the unpacking library
env.Append(CCFLAGS=["""-Dlong32='int'""", """-Dlong64='long long'"""])
conf = Configure(env, custom_tests = {'CheckCommand' : CheckCommand}) # pylint: disable-msg=E0602
set_cpp(conf, env)
set_python(conf, env)
set_gsl(conf, env)
set_root(conf, env)
set_clhep(conf, env)
set_geant4(conf, env)
set_recpack(conf, env)
set_gtest(conf, env)
set_unpacker(conf, env)

# check types size!!!
env = conf.Finish()
if 'configure' in COMMAND_LINE_TARGETS: # pylint: disable-msg=E0602
    my_exit(0)


# NOTE: do this after configure!  So we know if we have ROOT/geant4
#specify all of the sub-projects in the section
if env['USE_G4'] and env['USE_ROOT']:
    env.Append(CCFLAGS=['-g','-pg'])
    env.Append(LINKFLAGS='-pg')

    common_cpp_files = glob.glob("src/common/*/*cc") + \
        glob.glob("src/common/*/*/*cc") + \
        glob.glob("src/common/*/*cpp") + \
        glob.glob("src/common/*/*/*cpp")

    simulate = env.SharedLibrary(target = 'src/common/libsimulate',
                                 source = common_cpp_files,
                                 LIBS=env['LIBS'] + ['recpack'])
    env.Install("build", simulate)

    env.Append(LIBPATH = 'src/common/')
    env.Append(CPPPATH = maus_root_dir)

    if 'Darwin' in os.environ.get('G4SYSTEM'):
       env.Append(LINKFLAGS=['-undefined','suppress','-flat_namespace'])

    test_cpp_files = glob.glob("tests/cpp_unit/*/*cpp")+\
        glob.glob("tests/cpp_unit/*cpp")
    test_cpp_files += glob.glob("tests/cpp_unit/*/*cc")+\
        glob.glob("tests/cpp_unit/*cc")

    testmain = env.Program(target = 'tests/cpp_unit/test_cpp_unit', \
                               source = test_cpp_files, \
                               LIBS= env['LIBS'] + ['recpack'] + ['simulate'])
    env.Install('build', ['tests/cpp_unit/test_cpp_unit'])

directories = []
types = ["input", "map", "reduce", "output"]
for my_type in types:
    directories += glob.glob("src/%s/*" % my_type)

stuff_to_import = []

for directory in directories:
    parts = directory.split("/")
    assert len(parts) == 3
    assert parts[0] == 'src'
    assert parts[1] in types

    if 'Py' in parts[2] and 'Cpp' not in parts[2]:
        print 'Found Python module: %s' % parts[2]
        files = glob.glob('%s/test_*.py' % directory) +  ["%s/%s.py" % (directory, parts[2])]
        env.Install("build", files)

        stuff_to_import.append(parts[2])

    if (parts[2][0:6] == 'MapCpp') or (parts[2][0:8] == 'InputCpp'):
        print 'Found C++ module: %s' % parts[2]
        env.jDev.Subproject(directory)
        stuff_to_import.append(parts[2])

file_to_import = open('%s/build/MAUS.py' % maus_root_dir, 'w')

file_to_import.write("try:\n")
file_to_import.write("     from Go import Go\n")
file_to_import.write("except ImportError:\n")
file_to_import.write("     print 'failed to import Go'\n")
file_to_import.write("\n")

for single_stuff in stuff_to_import:
    file_to_import.write("try:\n")
    file_to_import.write("     from %s import %s\n" % (single_stuff, single_stuff))
    file_to_import.write("except ImportError:\n")
    file_to_import.write("     print 'failed to import %s'\n" % single_stuff)
    file_to_import.write("\n")

file_to_import.close()

files = glob.glob('tests/unit/test_*')+glob.glob('tests/style/*.py')
env.Install("build", files)

env.Install("build", "tests/unit/test_cdb")
env.Install("build", "tests/unit/suds")

env.Alias('install', ['%s/build' % maus_root_dir])
