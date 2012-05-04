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
Online run script to get maus to run online 
  - set up celery nodes
  - set up web app
  - set up input_transform
  - set up merger_output
  - logging
"""

import sys
import os
import signal
import xboa.Common
import subprocess
import multiprocessing
import time

LOCKFILE = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp', '.maus_lockfile')
PROCESSES = []
POLL_TIME = 10
REDUCER_LIST = [
  'reconstruct_daq_scalars_reducer.py',
  'reconstruct_daq_tof_reducer.py',
  'reconstruct_daq_ckov_reducer.py',
  'reconstruct_daq_single_station_reducer.py',
]

def poll_processes(proc_list):
    """
    Poll processes in process list. Return True if processes are all running,
    false if any have finished. 
    
    If verbose is True, prints out a message with status of each pid.
    If verbose is False, prints out a '.' or 'x' for each pid   
    """
    all_ok = True
    for proc in proc_list:
        running = proc.poll() == None
        all_ok = all_ok and running
        if running:
            print '.',
        else:
            print '\nProcess', proc.pid, 'failed'
    print
    return all_ok

def celeryd_process(celeryd_log_file_name):
    """
    Open the celery demon process - sets up workers for MAUS to reconstruct on
    """
    print 'Starting celery with log file ', celeryd_log_file_name,
    log = open(celeryd_log_file_name, 'w')
    proc = subprocess.Popen(['celeryd', '-c8', '-lINFO', '--purge'], \
                                           stdout=log, stderr=subprocess.STDOUT)
    print 'with pid', proc.pid
    return proc

def maus_web_app_process(maus_web_log_file_name):
    """
    Open the maus web app process - dynamically generates web pages for MAUS
    output display
    """
    print 'Starting maus web app with log file ', maus_web_log_file_name,
    log = open(maus_web_log_file_name, 'w')
    maus_web = os.path.join(os.environ['MAUS_WEB_DIR'], 'src/mausweb/manage.py')
    proc = subprocess.Popen(
                       ['python', maus_web, 'runserver', 'localhost:9000'],
                       stdout=log, stderr=subprocess.STDOUT)
    print 'with pid', proc.pid
    return proc

def maus_input_transform_process(maus_input_log):
    """
    Open the input transform process - runs against data and performs
    reconstruction, leaving reconstructed data in a database somewhere.
    """
    print 'Starting reconstruction with log file ', maus_input_log,
    log = open(maus_input_log, 'w')
    maus_inp = os.path.join(os.environ['MAUS_ROOT_DIR'],
                                   'bin/online/analyze_data_online_input_transform.py')
    proc = subprocess.Popen(
                       ['python', maus_inp, '-mongodb_database_name=maus-new',
                        '-type_of_dataflow=multi_process_input_transform'],
                       stdout=log, stderr=subprocess.STDOUT)
    print 'with pid', proc.pid
    return proc
    
def maus_merge_output_process(maus_output_log, reducer_name):
    """
    Open the merge output process - runs against reconstructed data and collects
    into a bunch of histograms.
    """
    print 'Starting reconstruction with log file ', maus_output_log,
    log = open(maus_output_log, 'w')
    maus_red = os.path.join(os.environ['MAUS_ROOT_DIR'], 'bin/online',
                                                                   reducer_name)
    proc = subprocess.Popen(
                       ['python', maus_red, '-mongodb_database_name=maus-new',
                        '-type_of_dataflow=multi_process_merge_output'],
                       stdout=log, stderr=subprocess.STDOUT)
    print 'with pid', proc.pid
    return proc

def clear_lockfile():
    if os.path.exists(LOCKFILE):
        print """
        Found lockfile - either you are running in two different windows or the
        application was not shut down properly last time. Please look at
            """+LOCKFILE+"""
        for a list of process ids - you will need to kill them by hand by doing
            kill <process id>
        Sorry about that. Once you are done, delete the file
             """+LOCKFILE+"""
        and things might run okay.
        """ 
        os.abort()
        # unfortunately this block fails to kill a child process spawned by
        # maus-web-app... otherwise I could automate the cleanup
        print 'Lockfile', LOCKFILE, 'found - killing processes'
        fin = open(LOCKFILE)
        for line in fin.readlines():
            pid = int(line.rstrip('\n'))
            try:
                os.kill(pid, signal.SIGTERM)
            except OSError:
                pass # maybe the pid didn't exist
            print 'Killed', pid
        time.sleep(3)

def make_lockfile(PROCESSES):
    print 'Making lockfile '+LOCKFILE
    fout = open(LOCKFILE, 'w')
    print >>fout, os.getpid()
    for proc in PROCESSES:
       print >>fout, proc.pid
    fout.close()

def cleanup():
    print 'Exiting... killing all MAUS processes'
    for process in PROCESSES:
        if process.poll() == None:
            process.send_signal(signal.SIGKILL)
            print 'Killed process '+str(process.pid)
    if os.path.exists(LOCKFILE):
        os.remove(LOCKFILE)
        print 'Cleared lockfile'
    else:
        print 'Strange, I lost the lockfile...'


def main():
    try:
        clear_lockfile()
        log_dir = os.environ['MAUS_WEB_MEDIA_RAW']

        celery_log = os.path.join(log_dir, 'celeryd.log')
        maus_web_log = os.path.join(log_dir, 'maus-web.log')
        input_log = os.path.join(log_dir, 'maus-input-transform.log')

        PROCESSES.append(celeryd_process(celery_log))
        PROCESSES.append(maus_web_app_process(maus_web_log))
        PROCESSES.append(maus_input_transform_process(input_log))
        for reducer in REDUCER_LIST:
            reduce_log = os.path.join(log_dir, reducer[0:-2]+'.log')
            PROCESSES.append(maus_merge_output_process(reduce_log,
                                                       reducer))
            
        make_lockfile(PROCESSES)
        print 'ctrl-c to quit'
        while poll_processes(PROCESSES):
           time.sleep(POLL_TIME)
    except Exception:
        sys.excepthook(*sys.exc_info())
    finally:
        cleanup()

if __name__ == "__main__":
    main()
