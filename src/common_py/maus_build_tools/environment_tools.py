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
Tools for setting up the environment for SCons

A whole bunch of tools for setting up each of the external c++ libraries from
third_party - adding libraries, plus any extra directives for the path, etc.
These can be called individually, by calling set_*(conf, env) functions, or by
calling set_lib(lib_name, conf, env) where lib_name is a string naming the 
library.

A few helper functions also in there.
"""

import sys
import os
import shutil

import SCons # pylint: disable=F0401

def my_exit(code = 1):
    """Internal routine to exit

    Currently this is its own function to allow easy disabling or a python
    warning.
    """
    sys.exit(code) # pylint: disable-msg=E0602

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

DYLIB2SO = SCons.Builder.Builder(action = duplicate_dylib_as_so, suffix = '.so',
                   src_suffix = '.dylib')

def which(program):
    """
    Return the path to a program, or None if the program cannot be
    called

    @param program the name of the program
    """
    def is_exe(fpath):
        """
        Return true if fpath exists and is executable
        """
        return os.path.exists(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)  # pylint: disable=W0612
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

def check_command(context, cmd):
    """
    Wrapper for which function for putting into the SCons configuration
    """
    context.Message('Checking for %s command... ' % cmd)
    result = which(cmd) # pylint: disable-msg=E0602
    context.Result(result is not None)
    return result

def get_environment_variables():
    """
    Return environment variables or exit if they can't be found
    """
    maus_root_dir = os.environ.get('MAUS_ROOT_DIR')
    if not maus_root_dir:
        print('!! Could not find the $MAUS_ROOT_DIR environmental variable')
        print('!! Did you try running: "source env.sh"?')
        my_exit(1)

    maus_third_party = os.environ.get('MAUS_THIRD_PARTY')
    if not maus_third_party:
        print('Could not find the $MAUS_THIRD_PARTY environmental variable')
        print('Setting maus_third_party to current working directory')
        maus_third_party = maus_root_dir
    return (maus_root_dir, maus_third_party)

def get_environment():
    """
    Set up the SCons environment

    Setup the environment with bindings for SWIG, llvm, Darwin, some 
    third_party_libs

    @returns the scons environment
    """
 
   #  NOTE: SHLIBPREFIX means that shared libraries don't
    # have a 'lib' prefix, which is needed for python to find SWIG generated
    # libraries
    env = SCons.Environment.Environment(SHLIBPREFIX="",
                 BUILDERS = {'Dylib2SO' : DYLIB2SO}) # pylint: disable-msg=E0602
    if os.getenv('CC') != None:
        env['CC'] = os.getenv("CC")
    if os.getenv('CXX') != None:
        env['CXX'] = os.getenv("CXX")

    env.Tool \
          ('swig', '%s/third_party/swig-2.0.1' % os.environ['MAUS_THIRD_PARTY'])
    # tell SWIG to make python bindings for C++
    env.Append(SWIGFLAGS=['-python', '-c++']) 

    # useful to set for root-config
    env['ENV']['PATH'] =  os.environ.get('PATH')
    env['ENV']['LD_LIBRARY_PATH'] = os.environ.get('LD_LIBRARY_PATH')
    env['ENV']['DYLD_LIBRARY_PATH'] = os.environ.get('DYLD_LIBRARY_PATH')

    libs = str(os.environ.get('LD_LIBRARY_PATH'))+':'+\
                                        str(os.environ.get('DYLD_LIBRARY_PATH'))

    # Additions for those who wish to use colorgcc
    env['ENV']['HOME'] =  os.environ.get('HOME')
    env['ENV']['TERM'] =  os.environ.get('TERM')

    # to find third party libs, includes
    env.Append(LIBPATH =  libs.split(':') + \
                                  ["%s/build" % os.environ['MAUS_THIRD_PARTY']])

    maus_root = os.environ['MAUS_ROOT_DIR']
    maus_third = os.environ['MAUS_THIRD_PARTY']
    env.Append(CPPPATH=["%s/third_party/install/include" % maus_third,
                        "%s/third_party/install/include/python2.7" % maus_third,
                        "%s/third_party/install/include/root" % maus_third,
                        "%s/src/legacy" % maus_root,
                        "%s/src/common_cpp" % maus_root,
                        "%s/" % maus_root])
    env['USE_G4'] = False
    env['USE_ROOT'] = False
    env['USE_UNPACKER'] = False

    #put all .sconsign files in one place
    env.SConsignFile()
    return env


def set_lib(conf, env, lib):
    """
    Check that appropriate library files can be found then add them to the
    include and load variables
    """
    if lib not in LIBS.keys():
        raise KeyError('Library '+str(lib)+' is not registered for '+\
                       'linking. Should be one of '+str(LIBS.keys()))
    set_lib_func = LIBS[lib]
    set_lib_func(conf, env)

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

        print( "ERROR: Couldn't find jsoncpp." )
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

    env.Append(CCFLAGS=["""-Wall""",
                        """-Dlong32='int'""",
                        """-DdatePointer='long'""",
                        """-fdiagnostics-color=always""",
                        """-std=c++11"""])

    cpp_extras(env)

def cpp_extras(env):
    """
    Sets compilation to include coverage, debugger, profiler depending on 
    environment variable flags. Following controls are enabled:
        if maus_lcov is set, sets gcov flags (for subsequent use in lcov); also
        disables inlining
        if maus_debug is set, sets debug flag -g
        if maus_gprof is set, sets profiling flag -pg
        if maus_no_optimize is not set and none of the others are set, sets
            optimise flag -O3
    """
    lcov = 'maus_lcov' in os.environ and os.environ['maus_lcov'] != '0'
    debug = 'maus_debug' in os.environ and os.environ['maus_debug'] != '0'
    gprof = 'maus_gprof' in os.environ and os.environ['maus_gprof'] != '0'
    # optimise by default
    optimise = not ('maus_no_optimize' in os.environ 
                  and os.environ['maus_no_optimize'] != '0')
    assert_active = 'maus_assert_active' in os.environ \
                    and os.environ['maus_assert_active'] != '0'
    
    if lcov:
        env.Append(LIBS=['gcov'])
        env.Append(CCFLAGS=["""-fprofile-arcs""", """-ftest-coverage""",
                            """-fno-inline""", """-fno-default-inline"""])

    if debug:
        env.Append(CCFLAGS=["""-g"""])

    if gprof: # only works on pure c code (i.e. unit tests)
        env.Append(CCFLAGS=["""-pg"""])
        env.Append(LINKFLAGS=["""-pg"""])

    if not (lcov or debug or gprof) and optimise:
        env.Append(CCFLAGS=["""-O3"""])        

    if not assert_active: # disable debug flags
        env.Append(CCFLAGS=["""-DNDEBUG"""])

def set_python(conf, env): # pylint: disable=W0613
    """
    Setup python
    """
    if sys.version[0:3] != '2.7':
        my_exit(1)
    
    if not conf.CheckCommand('python'):
        my_exit(1)

    if not conf.CheckCXXHeader('Python.h'):
        print "ERROR: Couldn't find Python.h"
        my_exit(1)

    if not conf.CheckCommand('swig'):
        print "ERROR: Cound't find swig."
        my_exit(1)

    conf.env.Append(LIBS = ['python2.7'])

def set_gsl(conf, env): # pylint: disable=W0613
    """
    Setup gsl
    """
    conf.env.Append(LIBS = ['gsl'])
    conf.env.Append(LIBS = ['gslcblas'])
    if not conf.CheckLib('gsl'):
        print "ERROR: Cound't find GSL"
        my_exit(1)
    if not conf.CheckLib('gslcblas'):
        print "ERROR: Cound't find GSL (required for ROOT)."
        my_exit(1)

def set_numpy(conf, env): # pylint: disable=W0613
    """
    Setup numpy
    """
    conf.env.Append(LIBS = ['npymath'])
    if not conf.CheckLib('npymath'):
        print "ERROR: Cound't find Numpy library"
        my_exit(1)
    # note other includes require Python
    if not conf.CheckCXXHeader('numpy/utils.h'):
        print "ERROR: Cound't find Numpy includes"
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
                'Gui', \
                'MathCore', \
                'Minuit', \
                'Hist', \
                'Matrix', \
                'Spectrum',\
                'Net', \
                'Physics', \
                'Postscript', \
                'PyROOT', \
                'RIO', \
                'Rint', \
                'Thread', \
                'Tree', \
                'dl', \
                'm', \
                'pthread',
                'rt']

def set_root(conf, env): # pylint: disable=W0613
    """
    Setup root
    """
    if not conf.CheckCommand('root'):
        print "Fatal - couldn't find root library."
        my_exit(1)

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

def set_clhep(conf, env): # pylint: disable=W0613
    """
    Setup clhep
    """
    if not conf.CheckLib('CLHEP', language='c++'):
        print "ERROR: Cound't find CLHEP."
        my_exit(1)

    conf.env.Append(LIBS = ['CLHEP'])

def set_xercesc(conf, env): # pylint: disable=W0613
    """
    Setup numpy
    """
    conf.env.Append(LIBS = ['xerces-c'])
    if not conf.CheckLib('xerces-c'):
        print "ERROR: Cound't find Xerces-c library"
        my_exit(1)

def get_g4_libs(): # pylint: disable=W0511
    """
    List of geant4 libraries
    """
    if os.environ['G4VERS'] == 'geant4.9.2.p04':
        return [ 'G4FR', 
             'G4RayTracer',
             'G4Tree',
             'G4UIGAG',
             'G4UIbasic',
             'G4UIcommon',
             'G4VRML',
             'G4baryons',
             'G4biasing',
             'G4bosons',
             'G4brep',
             'G4csg',
             'G4cuts',
             'G4decay',
             'G4detector',
             'G4detutils',
             'G4detscorer',
             'G4digits',
             'G4emlowenergy',
             'G4empolar',
             'G4emstandard',
             'G4emutils',
             'G4error_propagation',
             'G4event',
             'G4geomBoolean',
             'G4geombias',
             'G4geomdivision',
             'G4geometrymng',
             'G4geomtext',
             'G4gflash',
             'G4globman',
             'G4graphics_reps',
             'G4had_lll_fis',
             'G4had_mod_man',
             'G4had_mod_util',
             'G4had_neu_hp',
             'G4had_preequ_exciton',
             'G4had_string_diff',
             'G4had_string_frag',
             'G4had_string_man',
             'G4had_theo_max',
             'G4hadronic_HE',
             'G4hadronic_LE',
             'G4hadronic_RPG',
             'G4hadronic_ablation',
             'G4hadronic_abrasion',
             'G4hadronic_bert_cascade',
             'G4hadronic_binary',
             'G4had_im_r_matrix',
             'G4hadronic_deex_fermi_breakup',
             'G4hadronic_deex_handler',
             'G4hadronic_deex_evaporation',
             'G4hadronic_deex_fission',
             'G4hadronic_deex_gem_evaporation',
             'G4hadronic_deex_management',
             'G4hadronic_deex_multifragmentation',
             'G4hadronic_deex_photon_evaporation',
             'G4hadronic_deex_util',
             'G4hadronic_em_dissociation',
             'G4hadronic_hetcpp_evaporation',
             'G4hadronic_hetcpp_utils',
             'G4hadronic_incl_cascade',
             'G4hadronic_interface_ci',
             'G4hadronic_body_ci',
             'G4hadronic_iso',
             'G4hadronic_leading_particle',
             'G4hadronic_mgt',
             'G4hadronic_proc',
             'G4hadronic_qgstring',
             'G4hadronic_qmd',
             'G4hadronic_radioactivedecay',
             'G4hadronic_stop',
             'G4hadronic_util',
             'G4hadronic_xsect',
             'G4hepnumerics',
             'G4hits',
             'G4intercoms',
             'G4ions',
             'G4leptons',
             'G4magneticfield',
             'G4materials',
             'G4mctruth',
             'G4mesons',
             'G4modeling',
             'G4muons',
             'G4navigation',
             'G4optical',
             'G4parameterisation',
             'G4partadj',
             'G4partman',
             'G4partutils',
             'G4phys_builders',
             'G4had_muon_nuclear',
             'G4hadronic_coherent_elastic',
             'G4emhighenergy.so',
             'G4phys_lists',
             'G4procman',
             'G4readout',
             'G4run',
             'G4scoring',
             'G4shortlived',
             'G4specsolids',
             'G4track',
             'G4tracking',
             'G4transportation',
             'G4visHepRep',
             'G4visXXX', #pylint: disable=W0511
             'G4vis_management',
             'G4volumes',
             'G4xrays']
    else:
        return ['G4analysis',
            'G4error_propagation',
            'G4geometry',
            'G4graphics_reps',
            'G4materials',
            'G4particles',
            'G4processes',
            'G4run',
            'G4Tree',
            'G4visXXX', # pylint: disable = W0511
            'G4clhep',
            'G4event',
            'G4global',
            'G4intercoms',
            'G4modeling',
            'G4persistency',
            'G4RayTracer',
            'G4tracking',
            'G4visHepRep',
            'G4VRML',
            'G4digits_hits',
            'G4FR',
            'G4GMocren',
            'G4interfaces',
            'G4parmodels',
            'G4physicslists',
            'G4readout',
            'G4track',
            'G4vis_management',
            'G4zlib',
            ]


def set_geant4(conf, env):
    """
    Setup geant4

    Nb: sometimes geant4 builds some but not all libraries - so we really do
    need to check for every one.
    """
    g4_include = os.getenv('G4INCLUDE')
    if g4_include != '' and g4_include != None:
        env.Append(CPPPATH=[g4_include])
    if not conf.CheckCXXHeader('Geant4/G4EventManager.hh'):
        my_exit(1)

    env.Append(LIBS=get_g4_libs())

    for lib in get_g4_libs():
        if not conf.CheckLib(lib, language='c++'):
            my_exit(1)

    geant4_extras(env, conf)

def set_gtest(conf, env): # pylint: disable=W0613
    """
    Setup google tests (framework for cpp tests)
    """
    if not conf.CheckLib('gtest', language='c++'):
        my_exit(1)
    if not conf.CheckCXXHeader('gtest/gtest.h'):
        my_exit(1)

# Must have long32 & long64 for the unpacking library
def set_unpacker(conf, env):
    """
    Setup unpacker
    """
    maus_unpacker_version = os.environ.get('MAUS_UNPACKER_VERSION')
    if not maus_unpacker_version:
        print('!! Could not find $MAUS_UNPACKER_VERSION environment')
        print('!! Did you try running: "source env.sh"?')
        my_exit(1)
    if maus_unpacker_version != 'StepI' and maus_unpacker_version != 'StepIV':
        print('!! Unknown $MAUS_UNPACKER_VERSION %s' % maus_unpacker_version)
        my_exit(1)
    if maus_unpacker_version == 'StepIV':
        env.Append(CCFLAGS=["""-D_STEPIV_DATA"""])
    maus_unpack_lib = 'MDUnpack_'+maus_unpacker_version
    if (not conf.CheckLib(maus_unpack_lib, language='c++') or \
        not  conf.CheckCXXHeader('unpacking/MDevent.h')):
        print
        print "!! Unpacker module not found, you will not be able to use the "+\
              "RealData module."
        print
    else:
        env["USE_UNPACKER"] = True


def geant4_extras(env, conf):
    """
    Sets compilation to include geant4 opengl bindings

    If maus_opengl environment variable is set and is not equal to 0, add
    G4VIS_USE_OPENGLX and G4VIS_USE_OPENGLXM to the CCFLAGS and G4OpenGL to
    the libraries. Note that these libraries are not built by default.
    """
    opengl = 'maus_opengl' in os.environ and os.environ['maus_opengl'] != '0'
    if opengl:
        env.Append(CCFLAGS=["""-DG4VIS_USE_OPENGLX"""])
        env.Append(CCFLAGS=["""-DG4VIS_USE_OPENGLXM"""])
        geant_vis = 'G4OpenGL'
        env.Append(LIBS=[geant_vis])
        if not conf.CheckLib(geant_vis, language='c++'):
            print """
              Could not find G4OpenGL library. Build this library using
              $MAUS_ROOT_DIR/third_party/bash/32geant4_extras.bash
              Note that you need to have valid open gl and open gl xm development
              libraries installed.
              """
            my_exit(1)

LIBS = {
    'compiler':set_cpp,
    'python':set_python,
    'gsl':set_gsl,
    'numpy':set_numpy,
    'root':set_root,
    'clhep':set_clhep,
    'xercesc':set_xercesc,
    'geant4':set_geant4,
    'gtest':set_gtest,
    'unpacker':set_unpacker,
}
