"""
Go controls the running of MAUS dataflows.
"""

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

import os
import json
import sys
import datetime
import subprocess 

import maus_cpp.globals

# MAUS
from Configuration import Configuration # pylint: disable=W0403, C0301
from framework.input_transform import InputTransformExecutor # pylint: disable=W0403, C0301
from framework.merge_output import MergeOutputExecutor # pylint: disable=W0403, C0301
from framework.multi_process import MultiProcessExecutor # pylint: disable=W0403, C0301
from framework.single_thread import PipelineSingleThreadDataflowExecutor # pylint: disable=W0403, C0301

class Go: # pylint: disable=R0921, R0903
    """
    @class Go

    Go drives the execution of a MAUS dataflow. The types of
    components in a MAUS dataflow are: 

    - input: where the data comes from,
    - transformer: transform a spill to a new state (stateless),
    - reduce: process spill based on previous spills (has state),
    - output: where the data goes.

    There are different execution models e.g.

    - pipeline_single_thread - execute the complete dataflow for each
      spill in a single-threaded pipeline.
    - multi_process - submit spills to remove worker nodes for
      transformer application then merge and output the transformed
      spills.
    """

    def __init__(self, inputer, transformer, merger, outputer, config_file=None, command_line_args = True): # pylint: disable=R0913,C0301
        """
        Initialise the configuration dictionary and delegate execution
        to the relevant dataflow executor specified in the
        configuration. 
        
        @param self Object reference.
        @param inputer Input task.
        @param transformer Transformer task.
        @param merger Merger task.
        @param outputer Output task.
        @param config_file Configuration file.
        @param command_line_args If set to True, use command line
        arguments to handle configuration and throw a SystemExit
        exception if invalid arguments are passed. (Note some third
        party components, e.g. nosetests, have their own command
        line arguments that are incompatible with MAUS's).
        @throws Exception if MAUS_ROOT_DIR is not set or there are
        any other execution problems.
        """
        # Check MAUS_ROOT_DIR.
        self.json_config_doc = json.dumps({})
        Go.check_maus_root_dir()

        # Load the MAUS JSON configuration overriding it with the
        # contents of the given configuration file and command
        # line arguments.
        configuration  = Configuration()
        self.json_config_doc = configuration.getConfigJSON(
            config_file, command_line_args)

        # Parse the configuration JSON.
        json_config_dictionary = json.loads(self.json_config_doc)
        # How should we 'drive' the components?
        type_of_dataflow = json_config_dictionary['type_of_dataflow']
        # Grab version
        version = json_config_dictionary["maus_version"]

        # Print MAUS run info... userful for when the user wants to
        # know what they ran after the fact.  The PID can be used if
        # you want to kill MAUS at the command line: kill -s 9
        # PID_NUMBER.
        print("Welcome to MAUS:")
        print(("\tProcess ID (PID): %d" % os.getpid()))
        print(("\tProgram Arguments: %s" % str(sys.argv)))
        print ("\tVersion: %s" % version)
        print ("\tUnpacker: %s" % os.getenv("MAUS_UNPACKER_VERSION"))
        if json_config_dictionary["verbose_level"] == 0:
            print "Configuration: ", \
                json.dumps(json_config_dictionary, indent=2)
        print "Initialising Globals"
        # Initialise field maps, geant4, etc
        if not maus_cpp.globals.has_instance():
            maus_cpp.globals.birth(json.dumps(json_config_dictionary))        
        try:
            # Set up the dataflow executor.
            if type_of_dataflow == 'pipeline_single_thread':
                executor = PipelineSingleThreadDataflowExecutor(
                   inputer, transformer, merger, outputer, self.json_config_doc)
            elif type_of_dataflow == 'multi_process':
                executor = MultiProcessExecutor(
                   inputer, transformer, merger, outputer, self.json_config_doc)
            elif type_of_dataflow == 'multi_process_input_transform':
                executor = InputTransformExecutor( \
                    inputer, transformer, self.json_config_doc)
            elif type_of_dataflow == 'multi_process_merge_output':
                executor = MergeOutputExecutor( \
                    merger, outputer, self.json_config_doc)
            elif type_of_dataflow == 'many_local_threads':
                raise NotImplementedError()
            else:
                raise LookupError("bad type_of_dataflow: %s" % type_of_dataflow)

            # Execute the dataflow.
            print("Initiating Execution")
            executor.execute(self.get_job_header(json_config_dictionary),
                             self.get_job_footer())
            # Finish with job_footer
            #executor.finish(self.get_job_footer())
        except StopIteration:
            pass
        except:
            raise
        finally:
            print "Clearing Globals"
            maus_cpp.globals.death()
        print("DONE")

    @staticmethod
    def check_maus_root_dir():
        """
        Check that the MAUS_ROOT_DIR environment variable has been set
        and is equal to the current directory. 
 
        @throws Exception if MAUS_ROOT_DIR is not set.
        """
        # Determine what the 'env.sh' has set the user's environment
        # to. Exit if not set.
        #  otherwise with an exception.
        maus_root_dir = os.environ.get('MAUS_ROOT_DIR')
        if maus_root_dir == "":
            raise Exception("MAUS_ROOT_DIR environmental variable not set")

        # Warn the user that they could be using the wrong version of
        # MAUS. os.getcwd() is the current directory from which the
        # script is being executed. This warns the user to many common
        # errors. 
        if maus_root_dir not in os.getcwd():
            print("MAUS_ROOT_DIR = %s" % (maus_root_dir))
            print("Current working directory = %s\n" % (os.getcwd()))
    
    @staticmethod
    def get_possible_dataflows():
        """
        Enumerate list of possible types of dataflow.

        @return description strings indexed by dataflow type.
        """
        possible_types_of_dataflow = {}
        possible_types_of_dataflow['pipeline_single_thread'] = \
            PipelineSingleThreadDataflowExecutor.get_dataflow_description()
        possible_types_of_dataflow['multi_process'] = \
            MultiProcessExecutor.get_dataflow_description() 
        possible_types_of_dataflow['multi_process_input_transform'] = \
            InputTransformExecutor.get_dataflow_description()
        possible_types_of_dataflow['multi_process_merge_output'] = \
            MergeOutputExecutor.get_dataflow_description() 
        return possible_types_of_dataflow

    @staticmethod
    def get_job_header(json_datacards):
        """
        Generate the JobHeader object and send it to the output stream
        """
        start_of_job = {"date_time":datetime.datetime.utcnow().isoformat(' ')}
        bzr_dir = os.path.expandvars('$MAUS_ROOT_DIR/.bzr/branch/')
        bzr_configuration = 'bzr configuration not found'
        bzr_revision = 'bzr revision not found'
        bzr_status = 'bzr status not found'
        bzr_source = True
        if not os.path.isdir(bzr_dir):
            bzr_source = False 
        if bzr_source:
            try:
                bzr_conf_file = open(os.path.join(bzr_dir, 'branch.conf'))
                bzr_configuration = bzr_conf_file.read()
            except (OSError, IOError):
                pass
            try:
                bzr_rev_file = open(os.path.join(bzr_dir, 'last-revision'))
                bzr_revision = bzr_rev_file.read()
            except (OSError, IOError):
                pass
            try:
                mrd = os.environ["MAUS_ROOT_DIR"]
                bzr_status = subprocess.check_output(['bzr', 'status', mrd],
                                                 stderr=subprocess.STDOUT)
            except (OSError, IOError, subprocess.CalledProcessError):
                pass
        maus_version = json_datacards["maus_version"]
        return {
            "start_of_job":start_of_job,
            "bzr_configuration":bzr_configuration,
            "bzr_revision":bzr_revision,
            "bzr_status":bzr_status,
            "maus_version":maus_version,
            "json_configuration":json.dumps(json_datacards),
            "maus_event_type":"JobHeader"
        }

    @staticmethod
    def get_job_footer():
        """
        Generate the JobFooter object and send it to the output stream

        Really to make this useful, it needs to be available to users in the
        Globals (src/common_cpp/Utils/Globals.hh) object so that it can be
        edited during running.
        """
        end_of_job = {"date_time":datetime.datetime.utcnow().isoformat(' ')}
        return {
            "end_of_job":end_of_job,
            "maus_event_type":"JobFooter"
        }


