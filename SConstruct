from SCons.Script.SConscript import SConsEnvironment
import glob, os

#this is our catch-all Dev class
#it keeps track of all the variables and common functions we need
class Dev:
  mymode = ''
  debugcflags = ''
  releasecflags = ''

  #---
  # sets up the sconscript file for a given sub-project
  def Subproject(self, project):
      SConscript(env.jDev.SPath(project), exports=['project'])

  #sets up the build for a given project
  def Buildit(self, localenv, project):
     name = project.split('/')[-1]
     builddir =  '../../build/' + env.jDev.mymode
     targetpath = '../../build/' + env.jDev.mymode + '/_' + name

     #append the user's additional compile flags
     #assume debugcflags and releasecflags are defined
     if self.mymode == 'debug':
         localenv.Append(CCFLAGS=self.debugcflags)
     else:
         localenv.Append(CCFLAGS=self.releasecflags)

     #specify the build directory
     localenv.VariantDir(variant_dir=builddir, src_dir='.', duplicate=1)

     srclst = map(lambda x: builddir + '/' + x, glob.glob('*.cpp'))
     print srclst
     srclst += map(lambda x: builddir + '/' + x, glob.glob('*.i'))
     print srclst
     pgm = localenv.SharedLibrary(targetpath, source=srclst)
     env.Alias('all', pgm)  #note: not localenv

  #---- PRIVATE ----

  #---
  # return the sconscript path to use
  def SPath(self, project):
     return project + '/sconscript'


# Setup the environment.  NOTE: SHLIBPREFIX means that shared libraries don't
# have a 'lib' prefix, which is needed for python to find SWIG generated libraries
env = Environment(SHLIBPREFIX="") 

#env.Tool('swig', '%s/third_party/swig-2.0.1' % os.environ.get('MAUS_ROOT_DIR'))
env.Append(SWIGFLAGS=['-python', '-c++']) # tell SWIG to make python bindings for C++

#env.Append(PATH="%s/third_party/install/bin" % os.environ.get('MAUS_ROOT_DIR'))
env['ENV']['PATH'] =  os.environ.get('PATH')  # useful to set for root-config

# this should find swig and json
env.Append(CPPPATH=["%s/third_party/install/include" % os.environ.get('MAUS_ROOT_DIR'), \
                    "%s/third_party/install/include/python2.7" % os.environ.get('MAUS_ROOT_DIR')])

# to find JSON
env.Append(LIBPATH = ["%s/third_party/install/lib" % os.environ.get('MAUS_ROOT_DIR')])
env.Append(LIBS = ['json'])

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

def CheckCommand(context, cmd):
       context.Message('Checking for %s command... ' % cmd)
       result = WhereIs(cmd)
       context.Result(result is not None)
       return result


## autoconf-like stuff
print "Configuring..."
conf = Configure(env, custom_tests = {'CheckCommand' : CheckCommand})

# ---- check for compiler and do sanity checks
if not conf.CheckCXX():
  print('!! Your compiler and/or environment is not correctly configured.')
  Exit(0)

if not conf.CheckLib( "json" , language='C++') or not conf.CheckCXXHeader('json/json.h'):
  print( "can't find jsoncpp which is needed" );
  print ( "You may install it by running:");
  print ("MAUS_ROOT_DIR=%s ./third_party/bash/11jsoncpp.bash" % os.environ.get('MAUS_ROOT_DIR'))
  Exit(1)

if not conf.CheckLib( "stdc++" , language='C++'):
  print( "can't find stdc++ library which is needed" );
  Exit(1)

if not conf.CheckFunc('printf'):
  print('!! Your compiler and/or environment is not correctly configured.')
  Exit(0)

# check for math.h
if not conf.CheckHeader('math.h'):
  print "You need 'math.h' to compile this program"
  Exit(1)

# check for stdlib.h
if not conf.CheckCHeader('stdlib.h'):
  print "You need 'stdlib.h' to compile this program"
  Exit(1)

if not conf.CheckCXXHeader('iostream', '<>'):
  print "You need 'iostream' to compile this program"
  Exit(1)

if not conf.CheckCommand('python'):
  print "Cound't find python"
  Exit(1)

if not conf.CheckCXXHeader('Python.h'):
  print "You need 'Python.h' to compile this program"
  Exit(1)

if not conf.CheckCommand('root'):
  print "Cound't find root"
else:
  print
  print "!! Found the program 'root', so assume you want to use it with MAUS."
  print
  env['USE_ROOT'] = True

  if not conf.CheckCommand('root-config'):
    print "Cound't find root-config"
    Exit(1)
  else:
    env.ParseConfig("root-config --cflags --ldflags --libs") 

  root_libs = ['Minuit', 'Core', 'Cint', 'RIO', 'Net', 'Hist', 'Graf', 'Graf3d', 'Gpad', 'Tree', 'Rint', 'Postscript', 'Matrix', 'Physics', 'MathCore', 'Thread', 'pthread', 'm', 'dl']  # the important libraries I've found by looking at root-config output
       
  for lib in root_libs:
    if not conf.CheckLib(lib, language='c++'):
      print "You need %s to compile this program" % lib
      Exit(1)

  if not conf.CheckCXXHeader('TH1F.h'):
    print "You need 'TH1F.h' to compile this program"
    Exit(1)

  if not conf.CheckCXXHeader('TMinuit.h'):
    print "You need 'TH1F.h' to compile this program"
    Exit(1)

  #print WhereiS('root')



     # check types size!!!

env = conf.Finish()


# NOTE: do this after configure!  So we know if we have ROOT/geant4
# TODO: this should be a loop that discovers stuff
#specify all of the sub-projects in the section
env.jDev.Subproject('workers/map/CppPrint')
