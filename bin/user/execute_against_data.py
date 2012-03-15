#!/usr/bin/env python

DESCRIPTION = """
Script for offline submission to batch farm

execute_against_data is the script that is submitted to the batch farm for
running batch jobs once the data is pulled to the offline data store.
execute_against_data runs a simulation monte carlo job using simulate_mice.py 
and a reconstruction job using analyze_data_offline.py. The only input is a
run number which is found dynamically based on the file name.
"""

__doc__ = DESCRIPTION #pylint: disable = W0622

# TODO(rogers):
# - tests
# - docstrings + pylint

import argparse
import tarfile
import sys
import glob
import os
import subprocess
import shutil
import time

def arg_parser():
    """
    Parse command line arguments.

    Use -h switch at the command line for information on command line args used.
    """
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument('--input-file', dest='input_file', \
                        help='Read in json file with this name', required=True)
    parser.add_argument('--test', dest='test_mode', \
                        help='Run the batch job using a test geometry',
                        action='store_true', default=False)
    parser.add_argument('--no-test', dest='test_mode', \
                        help="Don't run the batch job using a test geometry",
                        action='store_false', default=False)
    return parser

class DownloadError(Exception):
    """
    DownloadError indicates a failure to download some necessary data for the
    run - these are considered transient errors (i.e. the calibration wasn't
    uploaded, the database wasn't available, etc). Try again later.
    """
    def __init__(self, error_message):
        """Initialise the exception with some error message"""
        self.error_message = error_message

    def __str__(self):
        """Return a string containing the the error message"""
        return repr(self.error_message)

class MausError(Exception):
    """
    MausError indicates the batch job failed due to some internal MAUS
    error. This would need human intervention to understand what happened. It
    may be the result of corrupt data, in which case we want to know about it.
    """
    def __init__(self, error_message):
        """Initialise the exception with some error message"""
        self.error_message = error_message

    def __str__(self):
        """Return a string containing the the error message"""
        return repr(self.error_message)

class run_manager:
    """
    Run manager manages the overall run - calls logging, setup, etc.
    """
    def __init__(self, argv):
        self.logs = file_manager()
        self.logs.open_log('download.log', 'sim.log', 'reco.log', 'batch.log')
        self.run_setup = run_settings(argv)
        self.logs.tar_file_name = self.run_setup.tar_file_name

    def run(self):
        if not self.check_valid():
            print 'Error - run not valid'
            return 1
        self.cleanup()
        self.setup()
        self.download_geometry()
        self.execute_simulation()
        self.execute_reconstruction()

    def check_valid(self):
        print 'Checking run validity'
        return True

    def setup(self):
        print 'Setup files'
        print '   ', self.run_setup.download_target
        print '   ', self.run_setup.input_file_name
        print '   ', os.getcwd()
        os.mkdir(self.run_setup.download_target)
        tar_in = tarfile.open(self.run_setup.input_file_name)
        tar_in.extractall()

    def cleanup(self):
        print 'Cleaning working directory'
        download_dir = self.run_setup.download_target
        if os.path.isdir(download_dir):
            shutil.rmtree(download_dir)
        clean_target = glob.glob(self.run_setup.run_number_as_string+'*')
        for item in clean_target:
            if item[-3:] != 'tar':
                os.remove(item)
      
    def download_geometry(self):
        """
        Download geometry
        """
        print 'Getting geometry'
        download = [os.path.join(self.run_setup.maus_root_dir, 'bin', 
                                            'utilities', 'download_geometry.py')]
        download += self.run_setup.get_download_parameters()
        proc = subprocess.Popen(download, stdout=self.logs.download_log, \
                                                       stderr=subprocess.STDOUT)
        proc.wait()
        if self.run_setup.test_mode:
            test_path_in = os.path.join(self.run_setup.maus_root_dir, 'src',
                    'legacy', 'FILES', 'Models', 'Configurations', 'Stage4.dat')
            test_path_out = os.path.join(self.run_setup.download_target, \
                                                       'ParentGeometryFile.dat')
            shutil.copy(test_path_in, test_path_out)
        elif proc.returncode != 0:
            raise DownloadError("Failed to download geometry successfully")


    def execute_simulation(self):
        """
        Execute the simulation
        """
        print 'Running simulation'
        simulation = [os.path.join(self.run_setup.maus_root_dir, 'bin', 
                                                            'simulate_mice.py')]
        simulation += self.run_setup.get_simulation_parameters()
        print simulation
        proc = subprocess.Popen(simulation, stdout=self.logs.sim_log, \
                                                       stderr=subprocess.STDOUT)
        proc.wait()
        self.logs.tar_queue.append(self.run_setup.mc_file_name)

    def execute_reconstruction(self):
        """
        Execute the reconstruction
        """
        print 'Running reconstruction'
        reconstruction = [os.path.join(self.run_setup.maus_root_dir, 'bin', 
                                                     'analyze_data_offline.py')]
        reconstruction += self.run_setup.get_reconstruction_parameters()
        print reconstruction
        proc = subprocess.Popen(reconstruction, stdout=self.logs.rec_log, \
                                                       stderr=subprocess.STDOUT)
        proc.wait()
        self.logs.tar_queue.append(self.run_setup.recon_file_name)

    def __del__(self):
        if not self.run_setup.test_mode:
            self.cleanup()
        self.logs.close_log()


class run_settings:
    def __init__(self, argv):
        """
        Initialise the run parameters
        """
        args = arg_parser()
        args_in = args.parse_args(argv)

        self.input_file_name = args_in.input_file
        self.test_mode = args_in.test_mode    

        self.run_number = self.get_run_number_from_file_name(self.input_file_name)
        self.run_number_as_string = str(self.run_number).rjust(5, '0')
        self.tar_file_name = self.run_number_as_string+"_offline.tar"
        self.recon_file_name = self.run_number_as_string+"_recon.json"
        self.mc_file_name = self.run_number_as_string+"_sim.json"
        self.maus_root_dir = os.environ["MAUS_ROOT_DIR"]
        self.download_target = 'downloads'

    def get_run_number_from_file_name(self, file_name):
        """
        Get the run number based on the file name

        Assumes a file of format 000####.* where #### is some run number.
        """
        file_name.lstrip('0')
        file_name = file_name.split('.')[0]
        run_number = int(file_name)
        return run_number

    def get_simulation_parameters(self):
        """
        Get the parameters for the simulation exe
        """
        return [
            '-simulation_geometry_filename', \
                   os.path.join(self.download_target, 'ParentGeometryFile.dat'),
            '-output_json_file_name', self.mc_file_name,
            '-verbose_level', '0',
        ]
    
    def get_reconstruction_parameters(self):
        """
        Get the parameters for the reconstruction exe
        """
        return [
            '-reconstruction_geometry_filename', os.path.join(self.download_target, 'ParentGeometryFile.dat'),
            '-output_json_file_name', str(self.recon_file_name),
            '-daq_data_file', str(self.run_number),
            '-daq_data_path', './',
            '-verbose_level', '0',
        ]

    def get_download_parameters(self):
        """
        Get the parameters for the reconstruction exe
        """
        return [
            '-geometry_download_run_number', str(self.run_number),
            '-geometry_download_directory', str(self.download_target),
            '-verbose_level', '0',
        ]

class file_manager:
    def __init__(self):
        """
        Open the log files; redirect stderr, stdout to batch.log
        """
        self.is_open = False
        self.tar_ball = None
        self.batch_log = None
        self.sim_log = None
        self.recon_log = None
        self.tar_file_name = None
        self.tar_queue = []

    def is_open(self):
        """
        return true if files are open; else false
        """
        return self.is_open

    def tar_add(self, file_names):
        """
        Add a file to the tar
        """
        for name in file_names:
            self.tar_ball.add(name)

    def open_log(self, download_name, sim_name, rec_name, batch_name):
        """
        Open the log files
        """
        if self.is_open:
            raise IOError('Logs are already open')
        self.download_log = open(download_name, 'w')
        self.batch_log = open(batch_name, 'w')
        self.sim_log = open(sim_name, 'w')
        self.rec_log = open(rec_name, 'w')
        self.tar_queue += [download_name, sim_name, rec_name, batch_name]
        self.is_open = True

    def close_log(self):
        """
        Close the logs; add logs to the tar_file
        """
        self.batch_log.close()
        self.sim_log.close()
        self.rec_log.close()
        self.download_log.close()
        if self.tar_file_name != None:
            if os.path.isfile(self.tar_file_name):
                os.remove(self.tar_file_name)
            self.tar_file = tarfile.open(self.tar_file_name, 'w:gz')
            for item in self.tar_queue:
                if os.path.isfile(self.tar_file_name):         
                    self.tar_file.add(item)
            self.tar_file.close()
            for item in self.tar_queue:
                if os.path.isfile(item):       
                    pass # cleanup does something weird...
        self.is_open = False
        
    def __del__(self):
        """
        Close the logs
        """
        self.close_log()

def main(argv):
    """
    Main function - determines arguments and calls relevant executable scripts
    """
    return_value = 3
    my_run = None
    try:
        my_run = run_manager(argv)
        return_value = my_run.run()
    # download errors are considered transient - i.e. try again later
    except DownloadError:
        return_value = 1
    # some failure in the reconstruction algorithms - needs investigation
    except MausError:
        return_value = 2
        sys.excepthook(*sys.exc_info())
    # some other exception - probably a failure in this script - needs
    # investigation
    except Exception:
        return_value = 3
        sys.excepthook(*sys.exc_info())
    finally:
        del my_run
        return return_value
        

if __name__ == "__main__":
    return_value = main(sys.argv[1:])
    sys.exit(return_value)

