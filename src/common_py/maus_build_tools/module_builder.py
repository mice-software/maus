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

    def __init__(self, scons_environment):
        """
        Initialise the ModuleBuilder
        """
        self.env = scons_environment
        self.project_list = []
        self.dependency_dict = {}
        self.built_list = []
        self.local_environments = {}

    # sets up the sconscript file for a given sub-project
    def subproject(self, project): #pylint: disable=C0103, R0201
        """
        Build the Subproject with name project
        """
        SConscript(sconscript_path(project), exports=['project']) # pylint: disable=E0602, C0301


    #sets up the build for a given project
    def build_project(self, localenv, project, dependencies=None): #pylint: disable=R0914, C0301
        """
        Add a particular subproject to the build list

        Buildit is called by SConscript files to request that we build a module.
        @param localenv the local environment set up in the sconscript file
        @param project the name of the project (string)
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

    def register_modules(self):
        """
        Find the modules that need building and register them (call sconscripts)
        """
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

            for my_type in types:
                # map -> MapCpp, input -> InputCpp, etc
                if parts[2].find(my_type.capitalize()+'Cpp') == 0:
                    print 'Found C++ module: %s' % parts[2]
                    self.subproject(directory)
                    stuff_to_import.append(parts[2])
        return stuff_to_import


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

def build_maus_lib(filename, stuff_to_import):
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

    for single_stuff in stuff_to_import:
        file_to_import.write("try:\n")
        file_to_import.write("     from %s import %s\n" % \
                                                   (single_stuff, single_stuff))
        file_to_import.write("except ImportError:\n")
        file_to_import.write("     print 'failed to import %s'\n" % \
                                                                   single_stuff)
        file_to_import.write("\n")

    file_to_import.close()

