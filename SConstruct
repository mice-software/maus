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
     builddir =  '../build/' + env.jDev.mymode
     targetpath = '../build/' + env.jDev.mymode + '/_' + name

     #append the user's additional compile flags
     #assume debugcflags and releasecflags are defined
     if self.mymode == 'debug':
         localenv.Append(CCFLAGS=self.debugcflags)
     else:
         localenv.Append(CCFLAGS=self.releasecflags)

     #specify the build directory
     localenv.VariantDir(variant_dir=builddir, src_dir='.', duplicate=0)

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

env = Environment(SWIGFLAGS=['-python'], SHLIBPREFIX="")

env.Append(CPPPATH="/usr/include/python2.6")

env['ENV']['PATH'] = os.environ.get('PATH')

if os.environ.get('USE_G4') == 'yes':
   env['USE_G4'] = True
else:
   env['USE_G4'] = False

if os.environ.get('USE_ROOT') == 'yes':
   env['USE_ROOT'] = True
else: 
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

#specify all of the sub-projects in the section
env.jDev.Subproject('workers/cpp/CppPrint')

## autoconf
if 'configure' in COMMAND_LINE_TARGETS:
   conf = Configure(env)

   if not conf.CheckHeader('Python.h'):
      print "You need 'Python.h' to compile this program"
      Exit(1)

   env = conf.Finish()