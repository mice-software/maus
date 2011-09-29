"""
@brief Check that we obey python portion of the style guide

This walks the directory structure of MAUS and checks for each file of ending
*.py,. It's possible to set up exceptions to the pylint test by 
"""

import unittest
import subprocess
import os
import time

class TestPythonStyle(unittest.TestCase): # pylint: disable=R0904
    """
    @brief Run pylint and return exit status
    """
    def run_pylint(self, file_name_in, fout):
        """
        @brief Run pylint and return exit status

        @params file_name_in name of file to be checked
        @params fout filehandle open for writing - stdout and stderr of pylint
                call will be written to this file

        @returns exit status of the pylint call
        """
        if file_name_in in TestPythonStyle.ignore_files:
            return 0
        my_args = ['pylint', file_name_in, '--rcfile='+self.pylintrc]
        errors = subprocess.call(args=my_args, stdout=fout, \
                                 stderr=subprocess.STDOUT)
        return errors

    def walk_directories(self, target_dir, fout):
        """
        @brief walk the directory structure from target_dir
        """
        error_files = []
        for root_dir, dummy, ls_files in os.walk(target_dir):
            ls_files = [f for f in ls_files if f[-3:] == '.py']
            for file_name in ls_files:
                #maus_dir is root_dir relative to maus
                maus_dir = root_dir[len(self.maus_root_dir)+1:]
                ignore = False
                for a_dir in self.exclude_dirs:
                    if a_dir in maus_dir.split('/'):
                        ignore = True
                if not ignore:
                    file_name = os.path.join(maus_dir, file_name)
                    errors = self.run_pylint(file_name, fout)
                    if errors > 0:
                        error_files.append(file_name)
        return error_files

    def test_python_style(self):
        """
        @brief walk up from $MAUS_ROOT_DIR and run pylint looking for errors

        Walks the tree and looks for errors. Measures number of errors by
        counting the number of lines in the pylint summary file. If this
        increases, throws an error.
        """
        current_n_python_errors = 982 # Rogers, 29Sept2011
        file_out = os.path.join(self.maus_root_dir, 'tmp', 'pylint.out')
        fout = open(file_out, 'w')
        error_files = []
        for target_dir in self.include_dirs:
            target_dir = os.path.join(self.maus_root_dir, target_dir)
            error_files += self.walk_directories(target_dir, fout)
        fout.close()
        time.sleep(1.0)
        fout = open(file_out)
        # just go by number of lines - bit of a hack but will do
        n_errors = len(fout.readlines())
        if len(error_files) > 0:
            print str(n_errors)+'/'+str(current_n_python_errors)+\
                  ' style errors in following files '+\
                  '(see tmp/pylint.out for details)\n'+str(error_files)

        if n_errors > current_n_python_errors:
            raise RuntimeError("Number of python style errors has increased"+\
                               " - Previously "+str(current_n_python_errors)+\
                               ". Now "+str(n_errors))
        fout.close()

    # folders in maus_root_dir to look at
    include_dirs = ['tests', 'src', 'bin', 'doc']
    exclude_dirs = ['build']
    maus_root_dir = os.environ['MAUS_ROOT_DIR']
    pylintrc = os.path.join(maus_root_dir, 'tests', 'style', 'pylintrc')

    ignore_files = [
      'src/common_py/SpillSchema.py', # data file
      'src/common_py/ConfigurationDefaults.py'] # data file


if __name__ == '__main__':
    unittest.main()



