"""
@brief Check that we obey python portion of the style guide

This walks the directory structure of MAUS and checks for each file of ending
*.py,. It's possible to set up exceptions to the pylint test by 
"""

import unittest
import subprocess
import os
import glob
import json

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
        # print '  INFO: Analysing ' + file_name_in + ', ',
        my_args = ['pylint', file_name_in, '--rcfile='+self.pylintrc]
        errors = subprocess.call(args=my_args, stdout=fout, \
                                 stderr=subprocess.STDOUT)
        # print str(errors) + ' errors found'
        return errors

    def in_place_filter(self, file_name): # pylint: disable=R0201
        """
        @brief filter pylint output file

        @params file_name name of the pylint output file

        @returns tuple of 
          (number of lines in the unfiltered file,
           number of lines in the filtered file)

        Remove lines that don't match like *.py:[0-9]: *
        """
        fin = open(file_name)
        lines_in = fin.readlines()
        lines_out = []
        fin.close()
        for line in lines_in:
            try:
                # filter for *:#:* where # is an integer. Probably easier in
                # regexp, but regexp is gross...
                # we have string like line = *.py:<str_1>
                str_1 = line[line.index(':')+1:]
                # we have string like str_1 = <str_2>:*
                str_2 = str_1[0:str_1.index(':')]
                # check str_2 = int
                end_number = int(str_2) #pylint: disable=W0612
                lines_out.append(line)
            except ValueError:
                pass # line filtered - index() or int() raises ValueError

        # sort here so we can do diffs on the output...
        lines_out.sort()
        fout = open(file_name, 'w')
        err_dict_ = {}
        for line in lines_out:
            fout.write(line)
            file_name = line[0:line.index(':')]
            if file_name in err_dict_.keys():
                err_dict_[file_name] += 1
            else:
                err_dict_[file_name] = 1
        return (len(lines_in), len(lines_out), err_dict_)

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
                if file_name in self.exclude_files:
                    ignore = True
                for a_glob_target in self.exclude_dirs:
                    test_dirs = glob.glob(self.maus_root_dir+'/'+a_glob_target)
                    if root_dir in test_dirs:
                        ignore = True
                if not ignore:
                    for local_dir in self.local_dirs:
                        if maus_dir.find(local_dir) > -1:
                            os.chdir(local_dir)
                            maus_dir = maus_dir[len(local_dir)+1:]
                            break
                    file_name = os.path.join(maus_dir, file_name)
                    errors = self.run_pylint(file_name, fout)
                    if errors > 0:
                        error_files.append(file_name)
                    os.chdir(self.maus_root_dir)
        return error_files

    def run_force_files(self, fout):
        """
        Explicitly iterate over items in force_files list and run pylint
        """
        error_files = []
        for file_name in self.force_files:   
            file_name = os.path.join(os.environ['MAUS_ROOT_DIR'], file_name)
            errors = self.run_pylint(file_name, fout)
            if errors > 0:
                error_files.append(file_name)
        return error_files

    def run_all_pylints(self):
        """
        Run pylint against all specified files
        
        @returns the file name of the file used to store pylint output
        """
        file_out = os.path.join(self.maus_root_dir, 'tmp', 'pylint.out')
        fout = open(file_out, 'w')
        for target_dir in self.include_dirs:
            target_dir = os.path.join(self.maus_root_dir, target_dir)
            self.walk_directories(target_dir, fout)
        self.run_force_files(fout)
        fout.close()
        return file_out

    def postprocess_error_file(self, file_out):
        """
        Postprocess the pylint errors to extract errors per file

        Filter the pylint output to show only errors using in_place_filter. Then
        compare mapping of errors to files with reference mapping. List areas
        where the errors have changed. Fail if errors have increased in any file
        """
        (dummy, n_errors, err_dict) = self.in_place_filter(file_out)
        fout = open(os.path.join(self.maus_root_dir, 'tmp/pylint.json'), 'w')
        print >> fout, json.dumps(err_dict)
        fout.close()
        fin = open(os.path.join(self.maus_root_dir, 
                                            'tests/style/ref-pylint.json'))
        ref_dict = json.loads(fin.readline())
        passes = True
        err_keys = sorted(err_dict.keys())
        print str(n_errors)+' style errors in following files '+\
          '(see tmp/pylint.out for details)\n'+str(err_keys)
        for key in err_keys:
            if key not in ref_dict:
                print 'FAILS:\nExpected: 0 found:', err_dict[key], \
                                                             'errors', key
                passes = False

            elif err_dict[key] != ref_dict[key]:
                if err_dict[key] > ref_dict[key]:
                    passes = False
                    print 'FAILS:'
                print 'Expected', ref_dict[key], \
                                  ' and found', err_dict[key], 'errors in ', key
        if not passes:
            raise RuntimeError("Number of python style errors has increased")
        fout.close()

    def test_python_style(self): #pylint: disable=R0201
        """
        Run pylint looking for python style errors

        Walks the tree and looks for errors. Measures number of errors by
        counting the number of lines in the pylint summary file. If this
        increases, throws an error.
        """
        here = os.getcwd()
        os.chdir(self.maus_root_dir)
        file_name = self.run_all_pylints()
        # just go by number of lines - bit of a hack but will do
        self.postprocess_error_file(file_name)
        os.chdir(here)

    # folders in maus_root_dir to look at
    include_dirs = ['doc', 'tests', 'src', 'bin', 'doc']
    # exclude if dir path globs to one of the following 
    exclude_dirs = ['bin/user', 'bin/user/*', 'bin/user/*/*', \
      'bin/user/*/*/*', 'bin/user/*/*/*/*', \
      'bin/user/*/*/*/*/*', 'src/*/*/build']
    # if any of these are in the path, chdir to the directory first
    # don't have nested local_dirs
    local_dirs = ['tests/integration/test_simulation/test_physics_model_full']
    # exclude if filename includes one of the following
    exclude_files = [
        'test_cdb__init__.py', # makes pylint error
    ]
    # force include these  files (even if pylint doesn't find them)
    force_files = [
        'SConstruct',
    ]
    maus_root_dir = os.environ['MAUS_ROOT_DIR']
    pylintrc = os.path.join(maus_root_dir, 'tests', 'style', 'pylintrc')

    ignore_files = [
      'src/common_py/SpillSchema.py', # data file
      'src/common_py/ConfigurationDefaults.py', # data file
      'tests/cpp_unit/Reconstruction/Global/global_recon_config.py', # data file
      'tests/cpp_unit/Reconstruction/Global/single_muon_json.py', # data file
      'tests/integration/test_simulation/test_tracking_speed/configuration.py',
      'bin/examples/simulate_particles_in_grid/'+\
                      'simulate_particles_in_grid_configuration.py', # data file
      'tests/integration/test_simulation/test_simulate_fs2a_cooling/'+\
                      'simulate_fs2a_configuration.py', # data file
      'bin/SciFi/Conf_PR_Momentum_Corrections.py', # config file
    ]

if __name__ == '__main__':
    unittest.main()

