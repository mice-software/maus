import os
import glob
import shutil

import SCons
import SCons.Script.SConscript as SConscript
from SCons.Script.SConscript import SConsEnvironment # pylint: disable-msg=F0401

MAUS_ROOT_DIR = os.environ['MAUS_ROOT_DIR']

#this is our catch-all Dev class
class ModuleBuilder:
    def __init__(self, scons_environment):
        self.env = scons_environment

    def build_modules(self):
        directories = []
        types = ["input", "map", "reduce", "output", "util"]
        for my_type in types:
            directories += glob.glob(MAUS_ROOT_DIR+"/src/"+my_type+"/*")

        stuff_to_import = []

        for directory in directories:
            directory = directory[len(MAUS_ROOT_DIR)+1:]
            parts = directory.split("/")
            assert len(parts) == 3
            assert parts[0] == 'src'
            assert parts[1] in types

            if 'Py' in parts[2] and 'Cpp' not in parts[2]:
                print 'Found Python module: %s' % parts[2]
                files = glob.glob('%s/*.py' % directory) 
                self.env.Install("build", files)

                stuff_to_import.append(parts[2])

            if (parts[2][0:6] == 'MapCpp') or (parts[2][0:8] == 'InputCpp') or (parts[2][0:9] == 'ReduceCpp') or (parts[2][0:7] == 'UtilCpp'):
                print 'Found C++ module: %s' % parts[2]
                self.Subproject(directory)
                stuff_to_import.append(parts[2])
        return stuff_to_import

    def build_maus_lib(self, filename, stuff_to_import):
        file_to_import = open(filename, 'w')

        file_to_import.write("import ROOT\n")
        file_to_import.write("ROOT.PyConfig.IgnoreCommandLineOptions = True\n")
        file_to_import.write("\n")

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
    # sets up the sconscript file for a given sub-project
    def Subproject(self, project):
        SConscript(self.env.jDev.SPath(project), exports=['project']) # pylint: disable-msg=E0602

    #sets up the build for a given project
    def Buildit(self, localenv, project, \
                    use_root, use_g4, use_unpacker = False):
        if use_root and not self.env['USE_ROOT']:
            if not self.env.GetOption('clean'):
                print "The worker", project, \
                    "requires ROOT which is disabled. Skipping..."
            return False

        if use_g4 and not self.env['USE_G4']:
            if not self.env.GetOption('clean'):
                print "The worker", project, \
                    "requires Geant4 which is disabled. Skipping..."
            return False

        if use_unpacker and not self.env['USE_UNPACKER']:
            if not self.env.GetOption('clean'):
                print "The worker", project, \
                    "requires Unpacker which is disabled. Skipping..."
            return False

        name = project.split('/')[-1]
        builddir = 'build'
        targetpath = os.path.join('build', '_%s' % name)

        #append the user's additional compile flags
        #assume debugcflags and releasecflags are defined
        if use_root and use_g4:
            localenv.Append(LIBS=['MausCpp'])
            localenv.Append(LIBPATH = "%s/src/common_cpp" % MAUS_ROOT_DIR)

        #specify the build directory
        localenv.VariantDir(variant_dir=builddir, src_dir='.', duplicate=0)
        localenv.Append(CPPPATH='.')

        full_build_dir = os.path.join(MAUS_ROOT_DIR, builddir)

        srclst = map(lambda x: builddir + '/' + x, glob.glob('*.cc'))
        srclst += map(lambda x: builddir + '/' + x, glob.glob('*.i'))
        pgm = localenv.SharedLibrary(targetpath, source=srclst)

        if (os.uname()[0] == 'Darwin'):
          lib_so = self.env.Dylib2SO(targetpath)
          Depends(lib_so, pgm)
          self.env.Install(full_build_dir, lib_so)

        tests = glob.glob('test_*.py')

        self.env.Install(full_build_dir, "build/%s.py" % name)
        self.env.Install(full_build_dir, pgm)
        self.env.Install(full_build_dir, tests)
        self.env.Alias('all', pgm)  #note: not localenv

        return True

    #---- PRIVATE ----

    #---
    # return the sconscript path to use
    def SPath(self, project):
        return project + '/sconscript'

    def cleanup_extras(self):

        print("In cleaning mode")
        for root, dirs, files in os.walk('%s/build' % os.environ['MAUS_ROOT_DIR']):
            for basename in files:
                filename = os.path.join(root, basename)
                print 'Removing:', filename
                if os.path.isfile(filename):
                    os.remove(filename)
            for basename in dirs:
                dirname = os.path.join(root, basename)
                if os.path.isdir(dirname):
                    shutil.rmtree(dirname) 


