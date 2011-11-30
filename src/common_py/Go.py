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

# MAUS
from Configuration import Configuration

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
            print("\nWARNING: YOU ARE RUNNING MAUS OUTSIDE ITS MAUS_ROOT_DIR")
            print("WARNING:\tMAUS_ROOT_DIR = %s" % (maus_root_dir))
            print("WARNING:\tCURRENT DIRECTORY = %s\n" % (os.getcwd()))

        # Get a copy of the configuration JSON document and keep it
        # local. 
        configuration  = Configuration()
        json_config_doc = configuration.getConfigJSON(
            config_file, command_line_args)
        # Parse the configuration JSON.
        json_config_dictionary = json.loads(json_config_doc)

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
        if json_config_dictionary["verbose_level"] == 0:
            print "Configuration: ", \
                json.dumps(json_config_dictionary, indent=2)

        # Set up the dataflow executor.
        if type_of_dataflow == 'pipeline_single_thread':
            executor = PipelineSingleThreadDataflowExecutor(
                inputer, transformer, merger, outputer, json_config_doc)
        elif type_of_dataflow == 'multi_process':
            executor = MultiProcessDataflowExecutor(
                inputer, transformer, merger, outputer, json_config_doc)
        elif type_of_dataflow == 'many_local_threads':
            raise NotImplementedError()
        else:
            raise LookupError("bad type_of_dataflow: %s" % type_of_dataflow)

        # Execute the dataflow.
        print("INITIATING EXECUTION")
        executor.execute()
        print("DONE")

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
            MultiProcessDataflowExecutor.get_dataflow_description() 
        return possible_types_of_dataflow

class DataflowExecutor:
    """
    @class DataflowExecutor
    Super-class for classes that implement MAUS task execution dataflows.
    """

    def __init__(self, inputer, transformer, merger, outputer, json_config_doc): # pylint: disable=R0913,C0301
        """
        Constructor. Save references to given arguments then
        parse the configuration document.
        
        @param self Object reference.
        @param inputer Input task.
        @param transformer Transformer task.
        @param merger Merger task.
        @param outputer Output task.
        @param json_config_doc JSON configuration document.
        """
        self.inputer = inputer
        self.transformer = transformer
        self.merger = merger
        self.outputer = outputer
        self.json_config_doc = json_config_doc
        #  Parse the configuration JSON
        self.json_config_dictionary = json.loads(self.json_config_doc)

    def setup_doc_store_proxy(self):
        """
        Set up document store. The document store is configured via
        the following parameter in the JSON configuration:

        -doc_store_class - document store class name. This value
         is assumed to be of form "modulename.classname".

        It is assumed that the class can be loaded by execution of 
        statements analogous to "import classname from modulename".
        The "connect" method of the document store is then invoked to 
        initialise the connection.

        @param self Object reference.
        @return data store.
        @throws ImportError. If the module name do not exist.
        @throws AttributeError. If the class is not in the module.
        @throws KeyError. If there is no doc_store_class in the JSON
        configuration.
        """
        # Get class and bust up into module path and class name.
        doc_store_class = self.json_config_dictionary["doc_store_class"]
        path = doc_store_class.split(".")
        doc_store_class = path.pop()
        doc_store_module = ".".join(path)

        # Dynamically import the module.
        module_object = __import__(doc_store_module)
        # Get class object.
        class_object = getattr(module_object, doc_store_class)
        # Create instance of class object.
        doc_store = class_object()
        # Connect to the data store.
        doc_store.connect(self.json_config_dictionary)

        return doc_store

    @staticmethod
    def buffer_input(input_emitter, number_of_inputs):
        """
        Buffer the input stream by only reading up to the first N
        inputs into memory.  Returns an array of inputs. 

        @param input_emitter Input stream.
        @param number_of_inputs Number of inputs to read.
        @return array of number_of_inputs inputs.
        """
        my_buffer = []

        for i in range(number_of_inputs):  # pylint: disable=W0612
            try:
                value = next(input_emitter)
                my_buffer.append(value.encode('ascii'))
            except StopIteration:
                return my_buffer
        return my_buffer

class PipelineSingleThreadDataflowExecutor(DataflowExecutor):
    """
    @class PipelineSingleThreadDataflowExecutor
    Execute MAUS dataflows as a single-threaded pipeline.
    """

    def __init__(self, inputer, transformer, merger, outputer, json_config_doc): # pylint: disable=R0913,C0301
        """
        Constructor. Call super-class constructor.
        
        @param self Object reference.
        @param inputer Input task.
        @param transformer Transformer task.
        @param merger Merger task.
        @param outputer Output task.
        @param json_config_doc JSON configuration document.
        """
        DataflowExecutor.__init__(self, inputer, transformer, merger,
                                  outputer,  json_config_doc)

    def execute(self):
        """
        Execute the dataflow - spills are, in turn, read from the 
        input, passed through the transform, merge and output.

        @param self Object reference.
        """
        print("INPUT: Reading input")
        assert(self.inputer.birth(self.json_config_doc) == True)
        emitter = self.inputer.emitter()
        # This helps us time how long the setup that sometimes happens
        # in the first spill takes
        print("HINT: MAUS will process 1 spill only at first...")
        map_buffer = DataflowExecutor.buffer_input(emitter, 1)

        print("TRANSFORM: Setting up transformer (this can take a while...)")
        assert(self.transformer.birth(self.json_config_doc) == True)

        print("MERGE: Setting up merger")
        assert(self.merger.birth(self.json_config_doc) == True)

        print("OUTPUT: Setting up outputer")
        assert(self.outputer.birth(self.json_config_doc) == True)

        print("PIPELINE: Get spill, TRANSFORM, MERGE, OUTPUT, repeat")

        i = 0
        while len(map_buffer) != 0:
            for spill in map_buffer:
                spill = self.transformer.process(spill)
                spill = self.merger.process(spill)
                self.outputer.save(spill)

            i += len(map_buffer)
            map_buffer = DataflowExecutor.buffer_input(emitter, 128)

            # Not Python 3 compatible print() due to backward
            # compatability. 
            print "TRANSFORM/MERGE/OUTPUT: ",
            print "Processed %d spills so far," % i,
            print "%d spills in buffer." % (len(map_buffer))

        print("TRANSFORM: Shutting down transformer")
        assert(self.transformer.death() == True)

        print("MERGE: Shutting down merger")
        assert(self.merger.death() == True)

        print("OUTPUT: Shutting down outputer")
        assert(self.outputer.death() == True)

    @staticmethod
    def get_dataflow_description():
        """
        Get dataflow description.

        @return description.
        """
        description = "Run in a pipeline programming fashion with only a\n"
        description += "single thread. See Wikipedia on 'pipeline\n"
        description += "programming' for more information."
        return description

class MultiProcessDataflowExecutor(DataflowExecutor):
    """
    @class MultiProcessDataflowExecutor
    Execute MAUS dataflows using a Celery distributed task queue and
    worker nodes and a data store to cache spills after being
    output from transformers, before they are consumed by mergers.

    This class expects a data store class to be specified in
    the JSON configuration e.g.
    @verbatim
    doc_store_class = "InMemoryDocumentStore.InMemoryDocumentStore"
    @endverbatim
    The class used may have additional configuration requirements.
    """

    def __init__(self, inputer, transformer, merger, outputer, json_config_doc): # pylint: disable=R0913,C0301
        """
        Constructor. Call super-class constructor then create
        a document data store.
        
        @param self Object reference.
        @param inputer Input task.
        @param transformer Transformer task.
        @param merger Merger task.
        @param outputer Output task.
        @param json_config_doc JSON configuration document.
        """
        DataflowExecutor.__init__(self, inputer, transformer, merger,
                                  outputer,  json_config_doc)
        # Create and connect to data store proxy.
        self.doc_store = DataflowExecutor.setup_doc_store_proxy(self)

    def execute(self):
        """
        Execute the dataflow - spills are, in turn, read from the 
        input, passed through the transform, saved into the spill 
        data store, then read from this, and passed through the 
        merge and output. 

        @param self Object reference.
        """
        # Input and transform spills.
        self.transform_input()
        # Merge and output spills.
        self.merge_output()

    def transform_input(self):
        """
        Set up MAUS input tasks and, on receipt of spills, submit
        to transform tasks accessed via a distributed task queue. 

        @param self Object reference.
        @throws Exception if there are no active Celery workers.
        """
        # Check for active Celery workers.
        from celery.task.control import inspect
        print("Checking for active workers")
        inspection = inspect()
        active_workers = inspection.active()
        if (active_workers == None):
            raise Exception("No active Celery workers!")

        print("INPUT: Reading input")
        assert(self.inputer.birth(self.json_config_doc) == True)
        emitter = self.inputer.emitter()
        map_buffer = DataflowExecutor.buffer_input(emitter, 1)

        print("TRANSFORM: spawning transform jobs for each spill")
        i = 0
        transform_results = {}
        while len(map_buffer) != 0:
            for spill in map_buffer:
#                from maustasks import MausSimulationTask
                from maustasks import MausDoNothingTask
                result = \
                    MausDoNothingTask.delay(spill) # pylint:disable=E1101
                # Index results by spill_id so can present
                # results to merge-output in same order.
                transform_results[i] = result
                i += 1
            map_buffer = DataflowExecutor.buffer_input(emitter, 128)
            print " Processed %d spills so far," % i,
            print(" %d spills left in buffer." % (len(map_buffer)))

        print("TRANSFORM: waiting for transform jobs to complete")
        # Wait for workers to finish - just keep looping until each
        # either succeeds or fails.
        spills = {}
        while len(transform_results) != 0:
            for spill_id in transform_results.keys():
                result = transform_results[spill_id]
                if result.successful():
                    del transform_results[spill_id]
                    print " Spill %d task %s OK " % (spill_id, result.task_id)
                    # Index results by spill_id so can present
                    # results to merge-output in same order.
                    spill = result.result
                    spills[spill_id] = spill
                    self.doc_store.put(str(spill_id), spill)
                elif result.failed():
                    del transform_results[spill_id]
                    print " Spill %d task %s FAILED " \
                        % (spill_id, result.task_id)
                    print "Exception in worker: %s " % result.result
                    print "Traceback in worker: %s " % result.traceback
                else:
                    continue
        print("TRANSFORM: transform jobs completed")

    def merge_output(self):
        """
        Pull transformed spills from a data store and submit
        these to merger then output tasks.

        @param self Object reference.
        """
        print("MERGE: Setting up merger")
        assert(self.merger.birth(self.json_config_doc) == True)

        print("OUTPUT: Setting up outputer")
        assert(self.outputer.birth(self.json_config_doc) == True)

        print("MULTI-PROCESS: Get spill, MERGE, OUTPUT, repeat")

        for spill_id in self.doc_store.ids():
            spill = self.doc_store.get(spill_id)
            print "  Executing Merge->Output for spill %s\n" % spill_id,
            spill = self.merger.process(spill)
            self.outputer.save(spill)
            self.doc_store.delete(spill_id)
            print("  %d spills left in data store." % (len(self.doc_store)))

        print("MERGE: Shutting down merger")
        assert(self.merger.death() == True)

        print("OUTPUT: Shutting down outputer")
        assert(self.outputer.death() == True)

        print("%d spills left in data store." % (len(self.doc_store)))

    @staticmethod
    def get_dataflow_description():
        """
        Get dataflow description.

        @return description.
        """
        description = "Run MICE how it is run in the control room. This\n"  
        description += "requires CouchDB and celery to be installed. See\n"
        description += "the wiki links on how to do this at\n"
        description += \
            "http://micewww.pp.rl.ac.uk/projects/maus/wiki/MAUSCeleryConfiguration\n" # pylint:disable=C0301
        return description
