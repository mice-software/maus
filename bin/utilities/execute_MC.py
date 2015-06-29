#!/usr/bin/env python

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

#pylint: disable = C0103
"""
Appease pylint but does nothing
"""

DESCRIPTION = """
This is the script used for offline submission to batch farm.

execute_against_data is the script that is submitted to the batch farm for
running batch jobs once the data is pulled to the offline data store.
execute_against_data runs a simulation monte carlo job using simulate_mice.py 
and a reconstruction job using analyze_data_offline.py. The only input is a
run number which is found dynamically based on the file name.

The user needs to source env.sh in the usual way before running.

Creates a tarball called ######_offline.tar where ##### is the run number right
aligned and padded by 0s.

Return codes are:
    0 - Everything ran okay.
    1 - There was a transient error. Try again later. Transient errors are
        errors like the configuration database could not be contacted, or there
        was no suitable run geometry uploaded yet.
    2 - there was some internal problem with the reconstruction. It needs to be
        checked by the software expert.
    3 - there was some problem with this script. It needs to be checked by the
        software expert.
"""

# dynamically set __doc__ string so I can access it for argparse
#pylint: disable = W0622, C0301
__doc__ = DESCRIPTION+"""

Three classes are defined
  - RunManager: handles overall run execution;
  - FileManager: handles logging and output tarball;
  - RunSettings: handles run setup #pylint: disable = W0622
"""

# TODO (rogers): pylint: disable = W0511
# - tests
# - docstrings + pylint

import argparse
import tarfile
import sys
import os
import subprocess
import shutil
import time
from time import sleep
import cdb

def arg_parser():
    """
    Parse command line arguments.

    Use -h switch at the command line for information on command line args used.
    """
    parser = argparse.ArgumentParser(description=DESCRIPTION, \
                           formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--input-file', dest='input_file', \
                        help='Read interface file with this name', \
                        required=True)
    parser.add_argument('--run-number', dest='run_number', \
                        help='Output index number', \
                        required=True)
    parser.add_argument('--test', dest='test_mode', \
                        help='Run the batch job using test cdb output',
                        action='store_true', default=False)
    parser.add_argument('--no-test', dest='test_mode', \
                        help="Don't run the batch job using test cdb output",\
                        action='store_false', default=False)
    parser.add_argument('--mcserialnumber', dest='mc_iteration', type=int, \
                        help='MC Serial number for configuration DB',\
                        default=0)
    parser.add_argument('--geometry-id', dest='geoid', \
                        help='The simulation geometry ID number') # ,\
                        # required=True)
    return parser

class DownloadError(Exception):
    """
    DownloadError indicates a failure to download some necessary data for the
    run - these are considered transient errors (i.e. the calibration wasn't
    uploaded, the database wasn't available, etc). Try again later.
    """
    def __init__(self, error_message):
        """Initialise the exception with some error message"""
        super(DownloadError, self).__init__(error_message)
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
        super(MausError, self).__init__(error_message)
        self.error_message = error_message

    def __str__(self):
        """Return a string containing the the error message"""
        return repr(self.error_message)

class RunManager:
    """
    Run manager manages the overall run - calls reconstruction and monte carlo

    Main function is run()
    """
    def __init__(self, args_in_):
        """
        Setup log files and run setup

        @param args_in_ arg_parser object (called from arg_parser function)
        """
        self.logs = FileManager()
        self.logs.open_log('download.log', 'sim.log', 'batch.log')
        self.run_setup = RunSettings(args_in_)
        self.logs.tar_file_name = self.run_setup.tar_file_name

    def run(self):
        """
        Does the main execution loop against a run file

        - Checks that the run number can be executed
        - Performs any setup on the working directory
        - downloads geometry files from cdb
        - executes the simulation code
        """
        if not self.check_valid():
            print 'Error - run not valid'
            return 1
        self.setup()
        self.download_cards()
        self.download_geometry()
        self.execute_simulation()


    def check_valid(self): # pylint: disable = R0201
        """
        Checks that the run number can be executed

        Not implemented yet, but some day would like to check that the run has
        valid calibrations, cabling and geometry

        @returns True if run is valid
        """
        print 'Checking run validity'
        
        if os.path.exists(self.run_setup.g4bl_interface):
            return True
        else:
            return False

    def setup(self):
        """
        Set up the current working directory

        Cleans the current working directory; Makes a directory for downloads;
        extracts the raw data tarball 
        """
        print 'Setup files'
        print '   ', self.run_setup.download_target
        print '   ', self.run_setup.input_file_name
        print '   ', os.getcwd()
        self.cleanup()
        os.mkdir(self.run_setup.download_target)
        

    def cleanup(self):
        """
        Cleans up the current working directory

        Removes the download target directory, and removes all files belonging
        to this run (denoted by <run_number>*) unless they are a tar ball
        """
        print 'Cleaning working directory'
        download_dir = self.run_setup.download_target
        if os.path.isdir(download_dir):
            shutil.rmtree(download_dir)
        if os.path.exists(self.run_setup.input_file_name):
            os.remove(self.run_setup.input_file_name)

    
    def download_cards(self):
        """
        Downloads the datacards from the configuration database
        
        If running in test mode, uses legacy Stage4 geometry instead.

        If default mc_iteration number given (0) then CDB will not be accessed.

        @raises DownloadError on failure
        """
        print 'Getting cards'
        bi_number = self.run_setup.mc_iteration
        if bi_number > 0:
            for i in range(5):
                try:
                    print "    Contacting CDB"
                    if self.run_setup.test_mode:
                        mcs_service = cdb.MCSerialNumber(
                            "http://preprodcdb.mice.rl.ac.uk")
                    else:
                        mcs_service = cdb.MCSerialNumber()
                    print "    Found, accessing cards"
                    mc_cards = mcs_service.get_datacards(bi_number)['data']
                    if mc_cards == 'null':
                        raise DownloadError(
                            "No MC cards for batch iteration number "+str(bi_number))
                    mc_out = open(self.run_setup.sim_cards, 'w')
                    mc_out.write(mc_cards)
                    return
                except cdb.CdbTemporaryError:
                    print "CDB lookup failed on attempt", i+1
                    time.sleep(1)
                except cdb.CdbPermanentError:
                    raise DownloadError("Failed to download cards - CDB not found")
            raise DownloadError("Failed to download cards after 5 attempts")
        else:
            # write an empty file
            mc_cards = ''
            mc_out = open(self.run_setup.sim_cards, 'w')
            mc_out.write(mc_cards)

    
    def download_geometry(self):
        """
        Downloads the geometry from the configuration database
        
        Calls the download_geometry.py utility script to download geometries by
        run number... But the table does not link step IV geometries to MC data
        cards yet. 

        If running in test mode, uses legacy Stage4 geometry instead.

        @raises DownloadError on failure
        """
        print 'Getting geometry'
        download = [os.path.join(self.run_setup.maus_root_dir, 'bin', \
                                 'utilities', 'download_geometry.py')]
        # check that there is a selection for the geometry in the datacards
        download += self.run_setup.get_download_parameters()
        proc = subprocess.Popen(download, stdout=self.logs.download_log, \
                                                       stderr=subprocess.STDOUT)
        proc.wait()
        if self.run_setup.test_mode:
            test_path_in = os.path.join(self.run_setup.maus_root_dir, 'src',
                    'legacy', 'FILES', 'Models', 'Configurations', 'Test.dat')
            test_path_out = os.path.join(self.run_setup.download_target, \
                                                       'ParentGeometryFile.dat')
            shutil.copy(test_path_in, test_path_out)
        if proc.returncode != 0:
            raise DownloadError("Failed to download geometry successfully")


    def execute_simulation(self):
        """
        Executes the Monte Carlo simulation
        
        Executes the simulation; puts the output file into the tar queue
        """
        print 'Running simulation'
        simulation = [os.path.join(self.run_setup.maus_root_dir, 'bin', 
                                                            'simulate_beam.py')]
        simulation += self.run_setup.get_simulation_parameters()
        print simulation
        proc = subprocess.Popen(simulation, stdout=self.logs.sim_log, \
                                                       stderr=self.logs.sim_log)
        # stderr=subprocess.STDOUT)
        proc.wait()
        if proc.returncode != 0:
            raise MausError("MAUS simulation returned "+str(proc.returncode))
        self.logs.tar_queue.append(self.run_setup.mc_file_name)
        print self.logs.tar_queue
        # print self.logs.tar_queue
        
        # assuming all of the useful bits of the reconstruction are in
        # simulate_mice we can stop here.

    def __del__(self):
        """
        If not in test mode, calls cleanup to clean current working directory
        and closes log files.
        """
        if not self.run_setup == None:
            self.cleanup()
            self.logs.close_log()
            # if not self.run_setup.test_mode:


class RunSettings: #pylint: disable = R0902
    """
    RunSettings holds settings for the run - run numbers, file names, etc

    Can write this stuff out as a list of command line parameters for each of
    the main execution blocks
    """

    def __init__(self, args_in):
        """
        Initialise the run parameters

        @param argv list of command line arguments (strings)

        Run number is taken from the name of the tarball passed as an argument.
        All other file names etc are then built off that
        """
        self.input_file_name = args_in.input_file
        self.test_mode = args_in.test_mode    
        self.mc_iteration = args_in.mc_iteration
        self.run_number = args_in.run_number
        self.run_number_as_string = str(self.run_number).rjust(5, '0')
        self.tar_file_name = self.run_number_as_string+"_offline.tar"
        self.g4bl_interface = \
                   self.get_file_name_from_run_number(self.input_file_name,\
                                                      self.run_number)

        
        self.mc_file_name = self.run_number_as_string+"_sim.root"
        
        print self.g4bl_interface
        
        if os.path.exists(self.g4bl_interface):
            print "Download of interface file successful"
            
        self.maus_root_dir = os.environ["MAUS_ROOT_DIR"]
        self.download_target = '%s/downloads' % os.getcwd()
        self.sim_cards = 'sim.cards'
        self.geometry_id = args_in.geoid

    def get_file_name_from_run_number(self, file_index, run_number):
        # pylint: disable = R0201
        """
        Get the file identity from the file index input to the script

        @param file_index run_number Input file index, run number

        Assumes that the file index contains a list of file names and
        that the run_number corresponds to a line number in the file.

        @returns a file name as a string
        """
        
        index = os.path.basename(file_index)
        index = os.path.join(os.getcwd(), index)
        if os.path.exists(index):
            os.remove(index)

        n_max_tries = 5
        retries = 0
        
        args = ['wget', file_index]
        while (retries < n_max_tries):
            proc = subprocess.Popen(args, stdout=subprocess.PIPE, \
                                    stderr=subprocess.PIPE)
            (stdout, stderr) = proc.communicate()
            print stderr
            if not os.path.exists(index):
                retries += 1
                sleep(0.5)
                continue
            break
        # if retries == n_max_tries: return NULL
        # parse the local file name from the address
        
        list = open(index)
        i = 0
        target_entry = ''
        for entry in list:
            if int(run_number) == i:
                # need to remove the trailing carriage return
                target_entry = entry[:-1]
                break
            else:
                i += 1
        # default is a grid based copy
        args = ['lcg-cp', '--checksum']
        # file names on the grid are arbitrary so make
        # it something logical for the local copy
        file_name = "jsondoc_"+str(run_number)+".txt"
        if target_entry.find("http") >= 0:
            # use a wget algorithm instead
            args = ['wget']
            # use the file name from the list for the
            # local copy
            file_name = os.path.basename(target_entry)
            # build the download command and split it
            # for the Popen command
            args.append(target_entry)
            file_name = os.path.join(os.getcwd(), file_name)
            print args
        else:
            args.append(target_entry)
            file_name = os.path.join(os.getcwd(), file_name)
            args.append('file:/'+file_name)
        
        if os.path.exists(file_name):
            os.remove(file_name)
        retries = 0
        while (retries < n_max_tries):
            proc = subprocess.Popen(args, stdout=subprocess.PIPE, \
                                    stderr=subprocess.PIPE)
            (stdout, stderr) = proc.communicate()
            print stdout, stderr
            if os.path.exists(file_name):
                print "Download successful"
            else:
                sleep(0.5)
                retries += 1
                continue
            break
        # if retries == n_max_tries: return NULL
        # os.remove(index)
        return file_name
            
    def get_simulation_parameters(self):
        """
        Get the parameters for the simulation executable

        Sets output filename, geometry filename, verbose_level

        @return list of command line arguments for simulation
        """
        return [
            '-simulation_geometry_filename', \
                   os.path.join(self.download_target, 'ParentGeometryFile.dat'),
            '-input_json_file_name', self.g4bl_interface,
            '-output_root_file_name', self.mc_file_name,
            '-verbose_level', '0',
            '-will_do_stack_trace', 'False',
            '-configuration_file', 'sim.cards',
        ]

    ## This will need to be updated to reflect the source of the MC
    ## data cards.
    
    def get_download_parameters(self):
        """
        Get the parameters for the reconstruction exe

        Sets the run number, download directory, verbose level

        @return list of command line arguments for download
        """
        params = []
        # The geometry id should be included in the data cards
        if 'geometry_download_id' in open('sim.cards').read() or \
               'geometry_download_by' in open('sim.cards').read():
            params =  [
                '-geometry_download_directory', str(self.download_target),
                '-verbose_level', '0',
                '-configuration_file','sim.cards'
                ]
        # otherwise, use the default download parameters.
        #                 '-cdb_download_url', 'http://preprodcdb.mice.rl.ac.uk/cdb/',
        else:
            params = [
                '-geometry_download_id', str(self.geometry_id),
                '-geometry_download_directory', str(self.download_target),
                '-verbose_level', '0',
                ]
        return params

class FileManager: # pylint: disable = R0902
    """
    File manager handles log files and the tar archive

    File manager has two components; logs of this batch script and each of the
    downloaded components and a queue (actually a list) of items that should be
    added to the tarball before exiting.
    """

    def __init__(self):
        """
        Initialises to None
        """
        self._is_open = False
        self.tar_ball = None
        self.download_log = None
        self.batch_log = None
        self.sim_log = None
        self.rec_log = None
        self.tar_file_name = None
        self.tar_queue = []

    def is_open(self):
        """
        @return true if files are open; else false
        """
        return self._is_open

    def open_log(self, download_name, sim_name, batch_name):
        """
        Open the log files

        Open the log files if they are not open; add them to the queue to be
        tarred on exit. Redirect stdout and stderr to log files.
        """
        if self._is_open:
            raise IOError('Logs are already open')
        self.download_log = open(download_name, 'w')
        self.batch_log = open(batch_name, 'w')
        sys.stderr = self.batch_log
        sys.stdout = self.batch_log
        self.sim_log = open(sim_name, 'w')
        # self.rec_log = open(rec_name, 'w')
        self.tar_queue += [download_name, sim_name, batch_name]
        self._is_open = True

    def close_log(self):
        """
        Close the logs; add items in the tar_queue to the tarball

        If the logs aren't open, does nothing
        """
        if not self._is_open:
            return
        self.batch_log.close()
        self.sim_log.close()
        self.download_log.close()
        if self.tar_file_name != None:
            if os.path.isfile(self.tar_file_name):
                os.remove(self.tar_file_name)
            tar_file = tarfile.open(self.tar_file_name, 'w:gz')
            for item in self.tar_queue:
                if os.path.isfile(self.tar_file_name):         
                    tar_file.add(item)
            tar_file.close()
            #for item in self.tar_queue:
            #    if os.path.isfile(item):
            #        os.remove(item)
            #        pass # cleanup does something weird...
        self._is_open = False
        
    def __del__(self):
        """
        Close the logs
        """
        self.close_log()

def main(argv):
    """
    Calls run manager to run the execution

    return values are:
        0 - everything ran okay
        1 - transient error, try again later
        2 - there was some internal problem with the reconstruction - needs to be
            checked by software expert
        3 - there was some problem with this script - needs to be checked by
            checked by software expert
    """
    my_return_value = 3
    my_run = None
    args = arg_parser()
    args_in_ = args.parse_args(argv) # call the arg_parser before logging
                                     # starts so we get -h output okay
    try:
        my_run = RunManager(args_in_)
        my_return_value = my_run.run()
    # download errors are considered transient - i.e. try again later
    except DownloadError:
        print "Fail Download"
        my_return_value = 1
    # some failure in the reconstruction algorithms - needs investigation
    except MausError:
        print "Fail Maus"
        my_return_value = 2
        sys.excepthook(*sys.exc_info())
    # some other exception - probably a failure in this script - needs
    # investigation
    except:
        #print "Fail Something Else"
        my_return_value = 3
        sys.excepthook(*sys.exc_info())
    finally:
        del my_run

    return my_return_value
        

if __name__ == "__main__":
    RETURN_VALUE = main(sys.argv[1:])
    sys.exit(RETURN_VALUE)

