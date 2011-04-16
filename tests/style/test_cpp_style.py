import os
import sys
import codecs
import unittest

import cpplint

maus_root_dir = os.environ['MAUS_ROOT_DIR']

exclude_dirs = [
'third_party',
'doc',
'tmp',
'build',
'.sconf_temp',
'src', #remove
'bin', #remove
os.path.join('tests','unit'),
os.path.join('tests','hudson'),
'.bzr'
]

exclude_files = [
os.path.join('tests', 'style', 'test_python_style.py')
]

def walker(maus_root_dir, exclude_dirs, exclude_files):
    """
    @brief walk up from maus_root_dir and run cpplint

    @params maus_root_dir start walking from here
    @params exclude_dirs list of directory paths relative to maus_root_dir that should be excluded from the walk (hence recursive)
    @params exclude_files list of file paths relative to maus_root_dir that should be excluded from cpplint

    @returns number of errors found by cpplint
    """
    errors = 0
    for root_dir, ls_dirs, ls_files in os.walk(maus_root_dir):
        maus_dir = root_dir[len(maus_root_dir)+1:] #root_dir relative to maus
        path_exclude(exclude_dirs, maus_dir, ls_dirs)
        path_exclude(exclude_files, maus_dir, ls_files)
        run_cpplint(root_dir, ls_files)
    return errors

def path_exclude(exclude_paths, path_root, path_list):
    """
    @brief Remove paths from path_list that are found in exclude_paths

    @params exclude_paths list of paths to exclude (paths relative to MAUS_ROOT_DIR)
    @params path_root suffix to add to path_list using os.path.join
    @params path_list list of paths to check for exclusions and modify

    @returns reference to ls_dirs
    """
    i = 0
    while i < len(path_list): #loop over all elements in list that is changing length
        if os.path.join(path_root, path_list[i]) in exclude_paths:
            del(path_list[i])
        else:
            i += 1

cpplint_suffixes = ['.hh', '.h', '.cpp', '.cc']    
def run_cpplint(root_dir, file_list):
    """
    @brief Run cpplint style checking script against a list of files

    @params root_dir path at which files can be found
    @params file_list list of files to run against cpplint

    @returns number of errors found

    Note that this only runs against files with suffixes in the list:
    """
    for a_file in file_list:
        target = os.path.join(root_dir, a_file)
        for suffix in cpplint_suffixes:
            if target.endswith(suffix):
                cpplint.ProcessFile(target, 3)
    return cpplint._cpplint_state.error_count
run_cpplint.__doc__ += str(cpplint_suffixes)
print run_cpplint.__doc__

class test_cpp_style(unittest.TestCase):
    def test_cpp_style(self):
        """
        @brief Walk the directory structure and look for errors in cpplint
        """
        err_fh = open(os.path.join(maus_root_dir,'tmp','cpplint.out'), 'w')
        sys.stderr = err_fh
        self.assertTrue(walker(maus_root_dir, exclude_dirs, exclude_files) == 0)
        sys.stderr = sys.__stderr__
        err_fh.close()

if __name__=="__main__":
  unittest.main()


