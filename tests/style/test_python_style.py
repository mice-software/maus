"""
@brief Check that we obey python portion of the style guide

This walks the directory structure of MAUS and checks for each file of ending
*.py,. It's possible to set up exceptions to the pylint test by 
"""

import unittest
import subprocess
import os
import io
import time

class test_python_style(unittest.TestCase):
    def run_pylint(self, file_name_in, fh):
        my_args = ['pylint', file_name_in, '--rcfile='+pylintrc]
        errors = subprocess.call(args=my_args, stdout=fh, stderr=subprocess.STDOUT)
        return errors

    def test_python_style(self):
        """
        @brief walk up from maus_root_dir and run pylint

        @params maus_root_dir start walking from here
        @params exclude_dirs list of directory paths relative to maus_root_dir that should be excluded from the walk (hence recursive)
        @params exclude_files list of file paths relative to maus_root_dir that should be excluded from cpplint

        @returns number of errors found by pylint
        """
        file_out = os.path.join(maus_root_dir, 'tmp', 'pylint.out')
        fh = open(file_out, 'w')
        errors = 0
        for target_dir in include_dirs:
            target_dir = os.path.join(maus_root_dir, target_dir)
            for root_dir, ls_dirs, ls_files in os.walk(target_dir):
                ls_files = filter(lambda f: f[-3:] == '.py', ls_files)
                for file_name in ls_files:
                    maus_dir = root_dir[len(maus_root_dir)+1:] #root_dir relative to maus
                    file_name = os.path.join(maus_dir, file_name)
                    errors = self.run_pylint(file_name, fh)
                    if errors > 0:
                        print file_name,errors
                    if errors > 30:
                        raise('Too many style errors in file ',file_name)
        return 0

include_dirs = ['tests', 'src', 'bin', 'doc']

maus_root_dir = os.environ['MAUS_ROOT_DIR']
pylintrc = os.path.join(maus_root_dir, 'tests', 'style', 'pylintrc')

if __name__ == '__main__':
    unittest.main()



