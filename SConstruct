from SCons.Script.SConscript import SConsEnvironment
import glob, os

#this is our catch-all Dev class
class Dev:
  cflags = '-g'
  ecflags = ''

  #---
  # sets up the sconscript file for a given sub-project
  def Subproject(self, project):
      SConscript(env.jDev.SPath(project), exports=['project'])

  #sets up the build for a given project
  def Buildit(self, localenv, project, use_root, use_g4):
    if use_root and not env['USE_ROOT']:
      if not env.GetOption('clean'):
        print "The worker", project, "requires ROOT which is disabled. Skipping..."
      return
    if use_g4 and not env['USE_G4']:
      if not env.GetOption('clean'):
        print "The worker", project, "requires Geant4 which is disabled. Skipping..."
      return
    print 'found', project
    builddir = 'build'
    name = project.split('/')[-1]
    targetpath = os.path.join('build', '_%s' % name)

    #append the user's additional compile flags
    #assume debugcflags and releasecflags are defined
    localenv.Append(CCFLAGS=self.cflags)
    if use_root and use_g4:
      localenv.Append(LIBS=['simulate'])
      pass

    #specify the build directory
    localenv.VariantDir(variant_dir=builddir, src_dir='.', duplicate=0)
    localenv.Append(CPPPATH='.')

    srclst = map(lambda x: builddir + '/' + x, glob.glob('*.cpp'))
    print srclst
    srclst += map(lambda x: builddir + '/' + x, glob.glob('*.i'))
    pgm = localenv.SharedLibrary(targetpath, source=srclst)#, LIBS=['simulate'])

    tests = glob.glob('test_*.py')
    print "tests:", tests
    
    env.Install(os.path.join(os.environ.get('MAUS_ROOT_DIR'), builddir) , "build/%s.py" % name)
    env.Install(os.path.join(os.environ.get('MAUS_ROOT_DIR'), builddir) , pgm)
    env.Install(os.path.join(os.environ.get('MAUS_ROOT_DIR'), builddir) , tests)
    env.Alias('all', pgm)  #note: not localenv

  #---- PRIVATE ----

  #---
  # return the sconscript path to use
  def SPath(self, project):
     return project + '/sconscript'


# Setup the environment.  NOTE: SHLIBPREFIX means that shared libraries don't
# have a 'lib' prefix, which is needed for python to find SWIG generated libraries
env = Environment(SHLIBPREFIX="") 

env.Tool('swig', '%s/third_party/swig-2.0.1' % os.environ.get('MAUS_ROOT_DIR'))
env.Append(SWIGFLAGS=['-python', '-c++']) # tell SWIG to make python bindings for C++

#env.Append(PATH="%s/third_party/install/bin" % os.environ.get('MAUS_ROOT_DIR'))
env['ENV']['PATH'] =  os.environ.get('PATH')  # useful to set for root-config

# to find third party libs, includes
env.Append(LIBPATH = ["%s/third_party/install/lib" % os.environ.get('MAUS_ROOT_DIR'),\
                        "%s/build" % os.environ.get('MAUS_ROOT_DIR')])
env.Append(CPPPATH=["%s/third_party/install/include" % os.environ.get('MAUS_ROOT_DIR'), \
                      "%s/third_party/install/include/python2.7" % os.environ.get('MAUS_ROOT_DIR'), \
                      "%s/commonCpp" % os.environ.get('MAUS_ROOT_DIR'), ])

env['USE_G4'] = False
env['USE_ROOT'] = False

#put all .sconsign files in one place
env.SConsignFile()

#we can put variables right into the environment, however
#we must watch out for name clashes.
SConsEnvironment.jDev = Dev()

#get the mode flag from the command line
#default to 'release' if the user didn't specify
env.jDev.mymode = ARGUMENTS.get('mode', 'release')   #holds current mode

#check if the user has been naughty: only 'debug' or 'release' allowed
if not (env.jDev.mymode in ['debug', 'release']):
   print "Error: expected 'debug' or 'release', found: " + env.jDev.mymode
   Exit(1)

#tell the user what we're doing
print '**** Compiling in ' + env.jDev.mymode + ' mode...'

env.jDev.debugcflags = [ '-W1', '-GX', '-D_DEBUG']   #extra compile flags for debug
env.jDev.releasecflags = ['-O2', '-DNDEBUG',]         #extra compile flags for release

#make sure the sconscripts can get to the variables
#don't need to export anything but 'env'
Export('env')

#### Target: Documentation
dox = env.Command('does_not_exist3', 'doc/Doxyfile',
                  'doxygen doc/Doxyfile && cd doc/html')
env.Alias('doc', [dox])

#unit = env.Command('does_not_exist2', 'build',
#                   'python -m unittest discover -b -v build')
#env.Alias('unittest', [unit])

def CheckCommand(context, cmd):
       context.Message('Checking for %s command... ' % cmd)
       result = WhereIs(cmd)
       context.Result(result is not None)
       return result

## autoconf-like stuff
if not env.GetOption('clean'):
  print "Configuring..."
  conf = Configure(env, custom_tests = {'CheckCommand' : CheckCommand})

  # ---- check for compiler and do sanity checks
  if not conf.CheckCXX():
    print('!! Your compiler and/or environment is not correctly configured.')
    Exit(0)

  if not conf.CheckLib( "json" , language='C++') or not conf.CheckCXXHeader('json/json.h'):
    print( "can't find jsoncpp which is needed" );
    print ( "You may install it by running:");
    print ("     MAUS_ROOT_DIR=%s ./third_party/bash/11jsoncpp.bash" % os.environ.get('MAUS_ROOT_DIR'))
    Exit(1)

  if not conf.CheckLib( "stdc++" , language='C++'):
    Exit(1)

  if not conf.CheckFunc('printf'):
    print('!! Your compiler and/or environment is not correctly configured.')
    Exit(0)

  if not conf.CheckHeader('math.h'):
    Exit(1)

  if not conf.CheckCHeader('stdlib.h'):
    Exit(1)

  if not conf.CheckCXXHeader('iostream', '<>'):
    Exit(1)

  if not conf.CheckCommand('python'):
    Exit(1)

  if not conf.CheckCXXHeader('Python.h'):
    print "You need 'Python.h' to compile this program"
    print "If you want to install python locally, run:"
    print ("     MAUS_ROOT_DIR=%s ./third_party/bash/03python.bash" % os.environ.get('MAUS_ROOT_DIR'))
    Exit(1)

  if not conf.CheckCommand('swig'):
    print "Cound't find swig.  If you want it, then run:"
    print ("     MAUS_ROOT_DIR=%s ./third_party/bash/10swig.bash" % os.environ.get('MAUS_ROOT_DIR'))
    Exit(1)

  if not conf.CheckLib('gslcblas'):
    print "Cound't find GSL (required for ROOT).  If you want it, then run:"
    print ("      MAUS_ROOT_DIR=%s ./third_party/bash/20gsl.bash" % os.environ.get('MAUS_ROOT_DIR'))
    if 'REQUIREALL' in os.environ:
      print "When running with 'REQUIREALL' all possible dependcies must exist"
      Exit(1)
  else:
    conf.env.Append(LIBS = ['gslcblas'])

  if not conf.CheckLib('gsl'):
    print "Cound't find GSL (required for ROOT).  If you want it, then run:"
    print ("      MAUS_ROOT_DIR=%s ./third_party/bash/20gsl.bash" % os.environ.get('MAUS_ROOT_DIR'))
    if 'REQUIREALL' in os.environ:
      print "When running with 'REQUIREALL' all possible dependcies must exist"
      Exit(1)

  if not conf.CheckCommand('root'):
    print "Cound't find root.  If you want it, after installing GSL, then run:"
    print ("      MAUS_ROOT_DIR=%s ./third_party/bash/21root.bash" % os.environ.get('MAUS_ROOT_DIR'))
    if 'REQUIREALL' in os.environ:
      print "When running with 'REQUIREALL' all possible dependcies must exist"
      Exit(1)

  else:
    print
    print "!! Found the program 'root', so assume you want to use it with MAUS."
    print
    env['USE_ROOT'] = True

    if not conf.CheckCommand('root-config'):
      Exit(1)

    conf.env.ParseConfig("root-config --cflags --ldflags --libs") 
      
    root_libs = ['Core', 'Cint', 'RIO', 'Net', 'Hist', 'Graf', 'Graf3d', 'Gpad', 'Tree', 'Rint', 'Postscript', 'Matrix', 'Physics', 'MathCore', 'Thread', 'pthread', 'm', 'dl', 'Minuit']  # the important libraries I've found by looking at root-config output
       
    for lib in root_libs:
      if not conf.CheckLib(lib, language='c++'):
        Exit(1)

    if not conf.CheckCXXHeader('TH1F.h'):
      Exit(1)

    if not conf.CheckCXXHeader('TMinuit.h'):
      Exit(1)

  if not conf.CheckLib('CLHEP', language='c++'):
    print "Cound't find CLHEP (required for geant4).  If you want it, then run:"
    print ("      MAUS_ROOT_DIR=%s ./third_party/bash/22clhep.bash" % os.environ.get('MAUS_ROOT_DIR'))
    if 'REQUIREALL' in os.environ:
      print "When running with 'REQUIREALL' all possible dependcies must exist"
      Exit(1)

  if 'G4INSTALL' not in os.environ or (not os.path.exists(os.environ.get('G4INSTALL'))):
    print "Cound't find geant4.  If you want it, after installing CLHEP, then run:"
    print ("      MAUS_ROOT_DIR=%s ./third_party/bash/23geant4.bash" % os.environ.get('MAUS_ROOT_DIR'))
    if 'REQUIREALL' in os.environ:
      print "When running with 'REQUIREALL' all possible dependcies must exist"
      Exit(1)
  else:
    print
    print "!! Found the package 'geant4', so assume you want to use it with MAUS."
    print
    env['USE_G4'] = True

    env.Append(LIBPATH = ["%s/%s" % (os.environ.get('G4LIB'), os.environ.get('G4SYSTEM'))])
    env.Append(CPPPATH=[os.environ.get("G4INCLUDE")])

    if not conf.CheckCXXHeader('G4EventManager.hh'):
      Exit(1)

    # removing this line (and using the append(libs) one below, because this is messy and breaks/seg-faults.
    #    conf.env.ParseConfig('%s/liblist -m %s < %s/libname.map'.replace('%s', os.path.join(os.environ.get('G4LIB'), os.environ.get('G4SYSTEM'))))

    geant4_libs = ['G4error_propagation', 'G4mctruth', 'G4readout', 'G4phys_lists', 'G4gflash', 'G4phys_builders', 'G4visHepRep', 'G4biasing', 'G4geomtext', 'G4FR', 'G4brep', 'G4RayTracer', 'G4Tree', 'G4VRML', 'G4visXXX', 'G4vis_management', 'G4geomBoolean', 'G4hadronic_radioactivedecay', 'G4decay', 'G4UIbasic', 'G4UIGAG', 'G4UIcommon', 'G4emhighenergy', 'G4partadj', 'G4muons', 'G4empolar', 'G4emlowenergy', 'G4xrays', 'G4hadronic_coherent_elastic', 'G4emstandard', 'G4hadronic_binary', 'G4partutils', 'G4run', 'G4had_theo_max', 'G4had_muon_nuclear', 'G4scoring', 'G4hadronic_interface_ci', 'G4hadronic_abrasion', 'G4transportation', 'G4hadronic_bert_cascade', 'G4hadronic_hetcpp_evaporation', 'G4hadronic_proc', 'G4hadronic_stop', 'G4hadronic_em_dissociation', 'G4hadronic_mgt', 'G4hadronic_qmd', 'G4hadronic_hetcpp_utils', 'G4hadronic_iso', 'G4hadronic_ablation', 'G4optical', 'G4had_preequ_exciton', 'G4hadronic_incl_cascade', 'G4hadronic_deex_handler', 'G4had_lll_fis', 'G4hadronic_deex_evaporation', 'G4had_neu_hp', 'G4had_string_diff', 'G4hadronic_leading_particle', 'G4hadronic_deex_gem_evaporation', 'G4hadronic_deex_fission', 'G4hadronic_deex_fermi_breakup', 'G4detutils', 'G4hadronic_deex_photon_evaporation', 'G4hadronic_deex_multifragmentation', 'G4had_string_frag', 'G4hadronic_HE', 'G4parameterisation', 'G4hadronic_qgstring', 'G4had_string_man', 'G4had_im_r_matrix', 'G4hadronic_deex_management', 'G4hadronic_LE', 'G4hadronic_body_ci', 'G4hadronic_RPG', 'G4hadronic_deex_util', 'G4shortlived', 'G4geomdivision', 'G4hadronic_xsect', 'G4had_mod_util', 'G4detscorer', 'G4had_mod_man', 'G4hadronic_util', 'G4modeling', 'G4event', 'G4geombias', 'G4specsolids', 'G4mesons', 'G4tracking', 'G4emutils', 'G4leptons', 'G4bosons', 'G4ions', 'G4baryons', 'G4cuts', 'G4procman', 'G4csg', 'G4digits', 'G4detector', 'G4hepnumerics', 'G4track', 'G4hits', 'G4navigation', 'G4magneticfield', 'G4partman', 'G4volumes', 'G4geometrymng', 'G4materials', 'G4graphics_reps', 'G4intercoms', 'G4globman']

    env.Append(LIBS=geant4_libs) 

    for lib in geant4_libs:
      if not conf.CheckLib(lib, language='c++'):
        Exit(1)

     # check types size!!!

  env = conf.Finish()
  if 'configure' in COMMAND_LINE_TARGETS:
    os.sys.exit(0)


  # NOTE: do this after configure!  So we know if we have ROOT/geant4
  # TODO: this should be a loop that discovers stuff
  #specify all of the sub-projects in the section
  if env['USE_G4'] and env['USE_ROOT']:
    env.Append(CCFLAGS=['-g','-pg'])
    
    commonCppFiles = glob.glob("commonCpp/*/*cc") + glob.glob("commonCpp/*/*/*cc") + glob.glob("commonCpp/*/*cpp") + glob.glob("commonCpp/*/*/*cpp")
    simulate = env.SharedLibrary(target = 'commonCpp/libsimulate', source = commonCppFiles, LIBS=['recpack'] +  env['LIBS'])
    env.Install("build", simulate)


  directories = []
  directories += glob.glob("components/input/*")
  directories += glob.glob("components/map/*")
  directories += glob.glob("components/reduce/*")
  for directory in directories:

    parts = directory.split("/")
    assert len(parts) == 3
    
    if parts[2][0:7] == "InputPy":  
      print 'Found Python input: %s' % parts[2]
      env.Install("build", "%s/%s.py" % (directory, parts[2]))

    if parts[2][0:5] == "MapPy":
      print 'Found Python mapper: %s' % parts[2]
      files = glob.glob('%s/test_*' % directory) +  ["%s/%s.py" % (directory, parts[2])]
      env.Install("build", files)

    if parts[2][0:8] == "ReducePy":
      print 'Found Python reducer: %s' % parts[2]
      env.Install("build", "%s/%s.py" % (directory, parts[2]))

    if parts[2][0:6] == 'MapCpp':
      print 'Found C++ mapper: %s' % parts[2] 
      env.jDev.Subproject(directory)

    
    

  env.Alias('install', ['%s/build' % os.environ.get('MAUS_ROOT_DIR')])

