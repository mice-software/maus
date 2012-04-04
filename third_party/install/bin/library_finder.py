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
Routine to find libraries and put a soft link in a local file somewhere.

Does some filtering based on libary architecture.
"""

import os
import sys
import subprocess
import platform

def architecture_filter_function(full_filename, sys_arch):
    """
    Return true if the library has the same architecture as sys_arch
    """
    lib_arch = subprocess.check_output(["objdump", "-f", full_filename])
    lib_arch = lib_arch.split('\n')[1] 
    if sys_arch[0].find("32bit") > -1 and lib_arch.find("elf32") > -1:
        return True
    elif sys_arch[0].find("64bit") > -1 and lib_arch.find("elf64") > -1:
        return True
    return False


class library_linker:
    """
    Few collected routines to search the file system and link to libraries if
    they are found.
    """

    def __init__(self):
        self.__architecture = platform.architecture()
        if self.__architecture[1] != "ELF":
            raise SystemError("Only ELF file systems supported here")

    def file_searcher(self, start_directory, filename, \
                                             filter_function=lambda x, y: True):
        """
        Recursively search from start_directory to find a file with filename.

        @params start_directory directory to start searching in
        @params filename filename to look for
        @params filter_function if filter_function returns false when acted on
                the full path, ignore this function
        """
        for _dirpath, _dirnames, _filenames in os.walk(start_directory):
            if filename in _filenames:
                full_name = os.path.join(_dirpath, filename)
                if filter_function(full_name, self.__architecture):
                    return full_name
        return None

    def link_lib(self, lib_target_dir, lib_name, architecture=None):
        """
        Find a library and put a link to it in lib_target_dir

        Seems to be the fashion to make a /usr/lib/lib_64_bit or so. Because the
        number of possible permutations is large (/usr/lib,
        /usr/lib/x86_64-linux-gnu, /usr/lib/local/x86_64-linux-gnu, ...) and
        every linux puts them in a random place, this algorithm will search the
        specified directory (e.g. /usr/) and collect libraries in a single place
        (e.g. $MAUS_THIRD_PARTY/third_party/install/lib) by symbolic linking.
        Overwrites any existing symbolic link with the same name. Throws an
        exception if there is any other object with the same name in the target
        position.

        Another way would be to do `uname -a` to get the build system (in my
        case x86_64) and see if I could figure it out from there. 

        @param lib_target target location for the library
        @param lib_name name of the library to be found
        @param architecture architecture of the library (string like 32bit,
               64bit). If not specified, use system architecture.
        """
        lib_full_name = "lib"+lib_name+".so"
        lib_location = self.file_searcher
                          ("/usr/", lib_full_name, architecture_filter_function)
        if lib_location == None:
            raise RuntimeError("Failed to find dependency "+lib_full_name)
        link_target = os.path.join(lib_target_dir, lib_full_name)
        if os.path.islink(link_target):
            os.unlink(link_target)
        os.symlink(lib_location, link_target)

    def find_libs(self, args):
        """
        Look for the libraries specified in targets

        Puts a soft link in
            $MAUS_THIRD_PARTY/third_party/install/lib
        """
        third_party = os.environ["MAUS_THIRD_PARTY"]
        lib_dir = os.path.join(third_party, "third_party", "install", "lib")
        target_libs = args
        for lib in target_libs:
            self.link_lib(lib_dir, lib)

if __name__ == "__main__":
        my_lib_linker = library_linker()
        my_lib_linker.find_libs(sys.argv[1:])


