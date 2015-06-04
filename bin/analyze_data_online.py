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

"""
Online run script to get maus to run online.

Contains subprocess commands to
  - set up celery nodes
  - set up web app
  - set up input_transform
  - set up merger_outputs
  - logging
Also some reasonably complicated process handling.

Each of the subprocesses is handled by the subprocess module. Cleanup of child
processes is done automatically on exit.

Only one instance of analyze_data_online is allowed to run at any time. This is
controlled by a lock file. Additionally the lock file contains a list of child
process ids. If this script fails to clean up at the end of the run, for example
if it is killed by an external signal, then next time it is run we can
kill all child processes.

Any command line arguments are passed to the MAUS input-transform and all MAUS
merge-output processes
"""

# would be great to have some tests for this

import sys
import os
import signal
import subprocess
import time
import string # pylint: disable=W0402
import pymongo

MONGODB = 'maus-new' # no '.' character
LOCKFILE = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp', '.maus_lockfile')
PROCESSES = []
# maximum % of total memory usage before a process is restarted
MAX_MEM_USAGE = 15.

# time between polls in seconds
POLL_TIME = 10

# list of reducers to be used in the online job
REDUCER_LIST = [
  'reconstruct_daq_scalars_reducer.py',
  'reconstruct_daq_tof_reducer.py',
  'reconstruct_daq_tofcalib_reducer.py',
  'reconstruct_daq_ckov_reducer.py',
  'reconstruct_daq_kl_reducer.py',
  'reconstruct_monitor_reducer.py',
  'reconstruct_daq_sci_fi_reducer.py',
]

class OnlineProcess:
    """
    Wrapper for a subprocess POpen object

    Wraps a subprocess with additional functionality to check memory usage and
    kill the subprocess and restart it in the case of a leak. See #1328.
    """

    def __init__(self, subprocess_arg_list, log_file):
        """
        Set up the log file and start the subprocess
        """
        self.arg_list = subprocess_arg_list
        self.log_name = log_file
        self.log = open(log_file, "w")
        self.subproc = None
        self._start_process()

    def poll(self):
        """
        Returns None if the process is running or the returncode if it finished

        Checks memory footprint for the subprocess and restarts the process if
        it exceeeds MAX_MEM_USAGE
        """
        proc = self.subproc
        poll_out = proc.poll()
        if poll_out == None:
            mem_usage = self.memory_usage()
            if mem_usage > MAX_MEM_USAGE:
                print mem_usage, "% of memory used for process", proc.pid, \
                      "which exceeds the maximum", MAX_MEM_USAGE, \
                      "% - restarting the process"
                cleanup([self])
                self._start_process()
            print str(proc.pid).rjust(6), str(mem_usage).ljust(6),
        else:
            print '\nProcess', proc.pid, 'failed'
        return poll_out

    def memory_usage(self):
        """
        Return the memory usage (%) of the associated subprocess
        """
        ps_out = subprocess.check_output(['ps', '-p', str(self.subproc.pid),
                                          'h', '-o%mem'])
        return float(ps_out.strip(' \n'))

    def _start_process(self):
        """
        Start the subprocess
        """
        self.subproc = subprocess.Popen(self.arg_list, \
                                      stdout=self.log, stderr=subprocess.STDOUT)
        print 'Started process with pid', self.subproc.pid, 'and log file', \
              self.log_name

def poll_processes(proc_list):
    """
    Poll processes in process list. Return True if processes are all running,
    false if any have finished.
    """
    all_ok = True
    for proc in proc_list:
        all_ok = all_ok and proc.poll() == None
    print
    return all_ok

def celeryd_process(celeryd_log_file_name):
    """
    Open the celery demon process - sets up workers for MAUS to reconstruct on
    """
    print 'Starting celery... ',
    proc = OnlineProcess(['celeryd', '-c8', '-lINFO', '--purge'],
                         celeryd_log_file_name)
    return proc

def maus_web_app_process(maus_web_log_file_name):
    """
    Open the maus web app process - dynamically generates web pages for MAUS
    output display
    """
    print 'Starting maus web app...',
    maus_web = os.path.join(os.environ['MAUS_WEB_DIR'], 'src/mausweb/manage.py')
    proc = OnlineProcess(['python', maus_web, 'runserver', 'localhost:9000'],
                         maus_web_log_file_name)
    return proc

def maus_input_transform_process(maus_input_log, _extra_args):
    """
    Open the input transform process - runs against data and performs
    reconstruction, leaving reconstructed data in a database somewhere.
    """
    print 'Starting input-transform...',
    maus_inp = \
             os.path.join(os.environ['MAUS_ROOT_DIR'],
                          'bin/online/analyze_data_online_input_transform.py')
    proc = OnlineProcess(['python', maus_inp,
                          '-mongodb_database_name='+MONGODB,
                          '-type_of_dataflow=multi_process_input_transform',
				                  '-verbose_level=0',
						              '-DAQ_hostname=miceraid5']+_extra_args,
                          maus_input_log)
    return proc
    
def maus_merge_output_process(maus_output_log, reducer_name, output_name,
                              _extra_args):
    """
    Open the merge output process - runs against reconstructed data and collects
    into a bunch of histograms.
    """
    print 'Starting reducer...',
    maus_red = os.path.join(os.environ['MAUS_ROOT_DIR'], 'bin/online',
                                                                   reducer_name)
    # strip trailing .py or whatever
    root_name = string.join(reducer_name.split('.')[0:-1], '.')
    if root_name == '':
        root_name = reducer_name
    root_name += '.root'
    proc = OnlineProcess(['python', maus_red,
                          '-mongodb_database_name='+MONGODB,
                          '-type_of_dataflow=multi_process_merge_output',
                          '-output_json_file_name='+output_name,
                          '-reduce_plot_refresh_rate=60',
                          '-output_root_file_name='+root_name,
                          '-output_root_file_mode=end_of_run_file_per_run']+\
                          _extra_args,
                          maus_output_log)
    return proc

def monitor_mongodb(url, database_name, file_handle):
    """
    Summarise the database.
    @param url URL.
    @param database_name Database name or "ALL" for all.
    """
    mongo = pymongo.Connection(url)
    database_names = mongo.database_names()
    if (database_name != "ALL"):
        if (database_name not in database_names):
            print >> file_handle, "Database %s not found" % database_name
            return
        else:
            database_names = [database_name]
    for database_name in database_names:
        print >> file_handle, "Database: %s" % database_name,
        mongodb = mongo[database_name]
        collection_names = mongodb.collection_names()
        if ("system.indexes" in collection_names):
            collection_names.remove("system.indexes")
        if (len(collection_names) == 0):
            print >> file_handle, "  No collections"
            continue
        for collection_name in collection_names:
            collection = mongodb[collection_name]
            validate = mongodb.validate_collection(collection_name)
            if "datasize" in validate.keys():
                space = validate["datasize"]
                space_kb = space / 1024
                space_mb = space_kb / 1024
                print >> file_handle, \
                    "  Collection: %s : %d documents (%d bytes %d Kb %d Mb)" \
                    % (collection_name, collection.count(), space, \
                    space_kb, space_mb)
    file_handle.flush()

def force_kill_celeryd():
    """
    celeryd likes to leave lurking subprocesses. This function searches the
    process table for celeryd child process and kills it.
    """
    ps_out =  subprocess.check_output(['ps', '-e', '-F'])
    pids = []
    for line in ps_out.split('\n')[1:]:
        if line.find('celeryd') > -1:
            words = line.split()
            pids.append(int(words[1]))
            print "Found lurking celeryd process", pids[-1]
    for a_pid in pids:
        os.kill(a_pid, signal.SIGKILL)
        print "Killed", a_pid

def force_kill_maus_web_app():
    """
    maus web app spawns a child process that is pretty hard to kill. This
    function searches the process table for mausweb child process and kills it.
    """
    hack_stdout = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp', 'grep.out')
    fout = open(hack_stdout, 'w')
    ps_proc = subprocess.Popen(['ps', '-e', '-F'], stdout=fout, \
                                                       stderr=subprocess.STDOUT)
    ps_proc.wait() # pylint: disable = E1101
    fout.close()
    fin = open(hack_stdout)
    pid = None
    for line in fin.readlines():
        if line.find('src/mausweb/manage.py') > -1:
            words = line.split()
            pid = int(words[1])
            print "Found lurking maus-web-app process"
    if pid != None:
        os.kill(pid, signal.SIGKILL)
        print "Killed", pid

def remove_lockfile():
    """
    Delete the lockfile
    """
    if os.path.exists(LOCKFILE):
        os.remove(LOCKFILE)
        print 'Cleared lockfile'
    else:
        print 'Strange, I lost the lockfile...'

def clear_lockfile():
    """
    Clear an existing lockfile

    If the script fails to exit gracefully, we leave a lock file and can leave
    associated child processes running. In this case, this function kills all
    child processes.
    """
    if os.path.exists(LOCKFILE):
        print """
Found lockfile - this may mean you have an existing session running elsewhere.
Kill existing session? (y/N)""" 
        sys.stdout.flush()
        user_input = raw_input()
        if len(user_input) == 0 or user_input[0].lower() != 'y':
            # note this doesnt go through cleanup function - just exits
            os.abort()
        print 'Lockfile', LOCKFILE, 'found - killing processes'
        fin = open(LOCKFILE)
        for line in fin.readlines():
            pid = int(line.rstrip('\n'))
            try:
                os.kill(pid, signal.SIGKILL)
            except OSError:
                pass # maybe the pid didn't exist
            print 'Killed', pid
        # maus web app spawns a child that needs special handling
        force_kill_maus_web_app()
        # celeryd must die
        force_kill_celeryd()
        time.sleep(3)

def make_lockfile(_procs):
    """
    Make a lock file listing pid of this process and all children
    """
    print 'Making lockfile '+LOCKFILE
    fout = open(LOCKFILE, 'w')
    print >> fout, os.getpid()
    for proc in _procs  :
        print >> fout, proc.subproc.pid
    fout.close()

def cleanup(_procs):
    """
    Kill any subprocesses in _procs list of OnlineProcesses
    """
    returncode = 0
    for online_process in _procs:
        process = online_process.subproc
        if process.poll() == None:
            print 'Attempting to kill process', str(process.pid)
            process.send_signal(signal.SIGINT)
    while len(_procs) > 0:
        _proc_alive = []
        for online_process in _procs:
            process = online_process.subproc
            print 'Polling process', process.pid,
            if process.poll() == None:
                print '... process did not die - it is still working '+\
                      '(check the log file)'
                _proc_alive.append(online_process)
            else:
                print '... process', str(process.pid), \
                      'is dead with return code', str(process.returncode)
                returncode = process.returncode
        sys.stdout.flush()
        _procs = _proc_alive
        time.sleep(10)
    return returncode

def main():
    """
    Make a lockfile; spawn child processes; and poll subprocesses until user
    hits ctrl-c

    If the subprocesses fail, have a go at setting up rabbitmcq and mongo

    Pass any command line arguments to all MAUS processes
    """
    extra_args = sys.argv[1:]
    returncode = 0
    try:
        force_kill_maus_web_app()
        force_kill_celeryd()
        clear_lockfile()
        log_dir = os.environ['MAUS_WEB_MEDIA_RAW']

        celery_log = os.path.join(log_dir, 'celeryd.log')
        maus_web_log = os.path.join(log_dir, 'maus-web.log')
        input_log = os.path.join(log_dir, 'maus-input-transform.log')
        debug_json = os.path.join(log_dir, 'reconstruct_monitor_reducer.json')

        PROCESSES.append(celeryd_process(celery_log))
        PROCESSES.append(maus_web_app_process(maus_web_log))
        PROCESSES.append(maus_input_transform_process(input_log, extra_args))
        for reducer in REDUCER_LIST:
            reduce_log = os.path.join(log_dir, reducer[0:-3]+'.log')
            PROCESSES.append(maus_merge_output_process(reduce_log,
                                              reducer, debug_json, extra_args))
        make_lockfile(PROCESSES)
        print '\nCTRL-C to quit\n'
        mongo_log = open(os.path.join(log_dir, 'mongodb.log'), 'w')
        while poll_processes(PROCESSES):
            monitor_mongodb("localhost:27017", MONGODB, mongo_log)
            sys.stdout.flush()
            sys.stderr.flush()
            time.sleep(POLL_TIME)
    except KeyboardInterrupt:
        print "Closing"
    except Exception:
        sys.excepthook(*sys.exc_info())
        returncode = 1
    finally:
        returncode = cleanup(PROCESSES)+returncode
        remove_lockfile()
        sys.exit(returncode)

if __name__ == "__main__":
    main()

