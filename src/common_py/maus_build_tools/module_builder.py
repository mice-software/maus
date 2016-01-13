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
Tools to build the MAUS modules that are used to e.g. reconstruct an individual
detector
"""

# TO-DO (Rogers): Just get rid of the "this module requires that library stuff"
#                I don't really see the point

import os
import glob
import shutil

import SCons.Script.SConscript as SConscript # pylint: disable=F0401

MAUS_ROOT_DIR = os.environ['MAUS_ROOT_DIR']

#this is our catch-all Dev class
class ModuleBuilder:
    """
    SCons "Dev" class responsible for build subprojects, in this case each of
    the submodules are a "subproject"
    
    sconscript files call these functions
    """

    def __init__(self, scons_environment, scons_configuration):
        """
        Initialise the ModuleBuilder
        """
        self.conf = scons_configuration
        self.env = scons_environment
        self.project_list = []
        self.dependency_dict = {}
        self.built_list = []
        self.local_environments = {}

    # sets up the sconscript file for a given sub-project
    def subproject(self, project, env, conf):#pylint: disable=C0103, R0201,W0613
        """
        Build the Subproject with name project
        """
        #pylint: disable=E1102
        SConscript(sconscript_path(project),
                   exports=['project', 'env', 'conf']) # pylint: disable=E0602, C0301

    # Determine if the project uses SWIG to build, and pass to either
    # build_project_swig or build_project_capi
    def build_project(self, localenv, project, dependencies=None): #pylint: disable=R0914, C0301
        """
        Add a particular subproject to the build list

        Buildit is called by SConscript files to request that we build a module.
        @param localenv the local environment set up in the sconscript file
        @param project the name of the project (string)
        @param dependencies list of projects on which this project depends
        @return True on success
        """
        name = project.split('/')[-1]
        potential_swig_file = os.path.join(MAUS_ROOT_DIR, project, name + '.i')
        if os.path.isfile(potential_swig_file):
            self.build_project_swig(localenv, project, dependencies)
        else:
            self.build_project_capi(localenv, project, dependencies)

    #sets up the build for a given project, using SWIG
    def build_project_swig(self, localenv, project, dependencies=None): #pylint: disable=R0914, C0301
        """
        Add a particular subproject to the build list

        Buildit is called by SConscript files to request that we build a module.
        @param localenv the local environment set up in the sconscript file
        @param project the name of the project (string)
        @param dependencies list of projects on which this project depends
        @return True on success
        """
        if dependencies == None:
            dependencies = [] # can cause evil if we use [] as default value

        name = project.split('/')[-1]
        builddir = 'build'
        # ack - build the module twice, once for lib*.so and once for _*.so 
        # needed by SWIG - could use a symlink here (but that seems quite hard
        # to do for SCons novice)
        lib_path = os.path.join('build', 'lib%s' % name)
        swig_path = os.path.join('build', '_%s' % name)

        #append the user's additional compile flags
        #assume debugcflags and releasecflags are defined
        localenv.Append(LIBS=['MausCpp'])
        localenv.Append(LIBPATH = "%s/src/common_cpp" % MAUS_ROOT_DIR)
        if type(dependencies) != type([]):
            print 'Warning - dependencies should be a list - found '+\
                  str(dependencies)+' in project '+project
            dependencies = []
        for dep in dependencies:
            lib_name = dep.split('/')[-1]
            localenv.Append(LIBS=lib_name)

        #specify the build directory
        localenv.VariantDir(variant_dir=builddir, src_dir='.', duplicate=0)
        localenv.Append(CPPPATH='.')
        full_build_dir = os.path.join(MAUS_ROOT_DIR, builddir)

        srclst = [builddir + '/' + x for x in glob.glob('*.cc')]
        srclst += [builddir + '/' + x for x in glob.glob('*.i')]
        swig_lib = localenv.SharedLibrary(swig_path, source=srclst)
        normal_lib = localenv.SharedLibrary(lib_path, source=srclst)
        for dep in dependencies:
            localenv.Depends(dep, swig_lib)
            localenv.Depends(dep, normal_lib)

        if (os.uname()[0] == 'Darwin'):
            lib_so = self.env.Dylib2SO(swig_path)
            Depends(lib_so, pgm) #pylint: disable=E0602
            self.env.Install(full_build_dir, lib_so)

        tests = glob.glob('test_*.py')
        print 'Installing', project
        self.env.Install(full_build_dir, "build/%s.py" % name)
        self.env.Install(full_build_dir, swig_lib)
        self.env.Install(full_build_dir, normal_lib)
        self.env.Install(full_build_dir, tests)
        return True

    #sets up the build for a given project, using the C/Python API
    def build_project_capi(self, localenv, project, dependencies=None): #pylint: disable=R0914, C0301
        """
        Add a particular subproject to the build list

        Buildit is called by SConscript files to request that we build a module.
        @param localenv the local environment set up in the sconscript file
        @param project the name of the project (string)
        @param dependencies list of projects on which this project depends
        @return True on success
        """
        if dependencies == None:
            dependencies = [] # can cause evil if we use [] as default value

        # Determine the name of the project
        name = project.split('/')[-1]
        
        # Find the sources
        ccpath = [x for x in glob.glob('*.cc')]

        #append the user's additional compile flags
        #assume debugcflags and releasecflags are defined
        localenv.Append(LIBS=['MausCpp'])
        localenv.Append(LIBPATH = "%s/src/common_cpp" % MAUS_ROOT_DIR)
        if type(dependencies) != type([]):
            print 'Warning - dependencies should be a list - found '+\
                  str(dependencies)+' in project '+project
            dependencies = []
        for dep in dependencies:
            lib_name = dep.split('/')[-1]
            localenv.Append(LIBS=lib_name)

        #specify the build directory
        builddir = 'build'
        localenv.VariantDir(variant_dir=builddir, src_dir='.', duplicate=0)
        localenv.Append(CPPPATH='.')
        full_build_dir = os.path.join(MAUS_ROOT_DIR, builddir)

        # Build the library, and set the dependencies
        lib1_path = os.path.join(builddir, 'lib%s' % name)
        lib2_path = os.path.join(builddir, '_%s.so' % name)
        normal_lib1 = localenv.SharedLibrary(lib1_path, source=ccpath)
        normal_lib2 = localenv.SharedLibrary(lib2_path, source=ccpath)
        for dep in dependencies:
            localenv.Depends(dep, normal_lib1)
            localenv.Depends(dep, normal_lib2)

        # Build an extra copy with the .dylib extension for linking on OS X
        if (os.uname()[0] == 'Darwin'):
            lib_so = self.env.Dylib2SO(lib1_path)
            Depends(lib_so, pgm) #pylint: disable=E0602
            self.env.Install(full_build_dir, lib_so)

        # Ensure the build directory exists
        if not os.path.exists(builddir):
            os.makedirs(builddir)

        # Locate the test file
        tests = glob.glob('test_*.py')

        # Install the files in the build directory, under MAUS_ROOT_DIR.
        print 'Installing', project
        self.env.Install(full_build_dir, normal_lib1)
        self.env.Install(full_build_dir, normal_lib2)
        self.env.Install(full_build_dir, tests)
        return True

    def register_modules(self):
        """
        Find the modules that need building and register them (call sconscripts)
        """
        directories = []
        types = ["input", "map", "reduce", "output"]
        for my_type in types:
            directories += glob.glob(MAUS_ROOT_DIR+"/src/"+my_type+"/*")

        cpp_libs, py_libs = [], []

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
                py_libs.append(parts[2])

            for my_type in types:
                # map -> MapCpp, input -> InputCpp, etc
                if parts[2].find(my_type.capitalize()+'Cpp') == 0:
                    print 'Found C++ module: %s' % parts[2]
                    if os.path.exists(fail_path(directory)):
                        os.remove(fail_path(directory))
                    self.subproject(directory, self.env, self.conf)
                    if build_okay(directory):
                        cpp_libs.append(parts[2])
                    else:
                        print "  Build failed", directory
        return cpp_libs, py_libs


def sconscript_path(project):
    """
    return the sconscript path to use
    """
    return project + '/sconscript'

def cleanup_extras():
    """
    SCons only removes files it detects in the build/dependency tree. So some
    files can get built and then forgotten. Here we force clean up of all files
    from the build.
    """
    print("In cleaning mode")
    for root, dirs, files in \
                          os.walk('%s/build' % os.environ['MAUS_ROOT_DIR']):
        for basename in files:
            filename = os.path.join(root, basename)
            print 'Removing:', filename
            if os.path.isfile(filename):
                os.remove(filename)
        for basename in dirs:
            dirname = os.path.join(root, basename)
            if os.path.isdir(dirname):
                shutil.rmtree(dirname) 

def build_maus_lib(filename, import_module_list):
    """
    Build MAUS.py - single point of entry importer for all of the MAUS libraries
    """
    file_to_import = open(filename, 'w')

    file_to_import.write("import ROOT\n")
    file_to_import.write("ROOT.PyConfig.IgnoreCommandLineOptions = True\n")
    file_to_import.write("\n")

    file_to_import.write("try:\n")
    file_to_import.write("     from Go import Go\n")
    file_to_import.write("except ImportError:\n")
    file_to_import.write("     print 'failed to import Go'\n")
    file_to_import.write("\n")

    for import_module in import_module_list:
        import_string = """
try:
    from %%MODULE%% import %%MODULE%%
except ImportError:
    from _%%MODULE%% import %%MODULE%%
"""
        import_string = import_string.replace("%%MODULE%%", import_module) 
        file_to_import.write(import_string)
    file_to_import.close()

def fail_path(directory):
    """
    Return the name of the temporary file that flags a failed subproject build
    """
    _fail_path = os.path.split(directory)[-1]
    _fail_path = os.path.join('$MAUS_ROOT_DIR', 'tmp',
                                                     _fail_path+'_failed_build')
    _fail_path = os.path.expandvars(_fail_path)
    return _fail_path

def build_okay(directory):
    """
    Return true if the project at <directory> built okay. Else return false.

    I believe there is no way to return a value from a sconscript file.
    So to indicate a failed build we make a temporary file called
    <project>_failed_build in sconscript and then look for it here.
    """
    return not os.path.exists(fail_path(directory))

