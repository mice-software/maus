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

from datetime import datetime
import os
import json
import socket
import sys

# MAUS
from Configuration import Configuration
import ErrorHandler
from docstore.DocumentStore import DocumentStore

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
        Go.check_maus_root_dir()

        # Load the MAUS JSON configuration overriding it with the
        # contents of the given configuration file and command
        # line arguments.
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
        elif type_of_dataflow == 'multi_process_input_transform':
            executor = MultiProcessInputTransformDataflowExecutor( \
                inputer, transformer, json_config_doc)
        elif type_of_dataflow == 'multi_process_merge_output':
            executor = MultiProcessMergeOutputDataflowExecutor( \
                merger, outputer, json_config_doc)
        elif type_of_dataflow == 'many_local_threads':
            raise NotImplementedError()
        else:
            raise LookupError("bad type_of_dataflow: %s" % type_of_dataflow)

        # Execute the dataflow.
        print("INITIATING EXECUTION")
        executor.execute()
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
            print("\nWARNING: YOU ARE RUNNING MAUS OUTSIDE ITS MAUS_ROOT_DIR")
            print("WARNING:\tMAUS_ROOT_DIR = %s" % (maus_root_dir))
            print("WARNING:\tCURRENT DIRECTORY = %s\n" % (os.getcwd()))
    
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
        possible_types_of_dataflow['multi_process_input_transform'] = \
            MultiProcessInputTransformDataflowExecutor.get_dataflow_description()  # pylint: disable=C0301
        possible_types_of_dataflow['multi_process_merge_output'] = \
            MultiProcessMergeOutputDataflowExecutor.get_dataflow_description() 
        return possible_types_of_dataflow

class DataflowUtilities: # pylint: disable=W0232
    """
    @class DataflowUtilities.
    Dataflow-related utility functions.
    """

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

    @staticmethod
    def setup_doc_store(json_config_dictionary):
        """
        Set up document store. The document store is configured via
        the following parameter in the JSON configuration:

        -doc_store_class - document store class name. This value
         is assumed to be of form "modulename.classname".

        It is assumed that the class can be loaded by execution of 
        statements analogous to "import classname from modulename".
        The "connect" method of the document store is then invoked to 
        initialise the connection.

        @param json_config_dictionary JSON configuration.
        @return document store.
        @throws ImportError. If the module name do not exist.
        @throws AttributeError. If the class is not in the module.
        @throws KeyError. If there is no doc_store_class in the JSON
        configuration.
        @throws TypeError If doc_store_class does not sub-class
        docstore.DocumentStore.DocumentStore.
        """
        # Get class and bust up into module path and class name.
        doc_store_class = json_config_dictionary["doc_store_class"]
        path = doc_store_class.split(".")
        doc_store_class = path.pop()
        import_path = ".".join(path)
        module_object = __import__(import_path)
        path.pop(0)
        # Dynamically import the module.
        for sub_module in path:
            module_object = getattr(module_object, sub_module) 
        # Get class object.
        class_object = getattr(module_object, doc_store_class)
        # Create instance of class object.
        doc_store = class_object()
        # Validate.
        if (not isinstance(doc_store, DocumentStore)):
            raise TypeError("Document store class %s does not implement %s" \
                % (doc_store_class, DocumentStore))
        # Connect to the document store.
        doc_store.connect(json_config_dictionary) 
        return doc_store

class PipelineSingleThreadDataflowExecutor:
    """
    @class PipelineSingleThreadDataflowExecutor
    Execute MAUS dataflows as a single-threaded pipeline.
    """

    def __init__(self, inputer, transformer, merger, outputer, json_config_doc): # pylint: disable=R0913,C0301
        """
        Save references to arguments and parse the JSON configuration
        document.
        
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
        map_buffer = DataflowUtilities.buffer_input(emitter, 1)

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
            map_buffer = DataflowUtilities.buffer_input(emitter, 1)

            # Not Python 3 compatible print() due to backward
            # compatability. 
            print "TRANSFORM/MERGE/OUTPUT: ",
            print "Processed %d spills so far," % i,
            print "%d spills in buffer." % (len(map_buffer))

        print("CLOSING PIPELINE: Sending END_OF_RUN to merger")

        end_of_run_spill = json.dumps({"END_OF_RUN":"END_OF_RUN"})
        spill = self.merger.process(end_of_run_spill)
        self.outputer.save(spill)

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

class MultiProcessDataflowExecutor:
    """
    @class MultiProcessDataflowExecutor
    Execute MAUS dataflows using a Celery distributed task queue and
    worker nodes and a document store to cache spills after being
    output from transformers, before they are consumed by mergers.

    This class expects a document store class to be specified in
    the JSON configuration e.g.
    @verbatim
    doc_store_class = "InMemoryDocumentStore.InMemoryDocumentStore"
    @endverbatim
    The class used may have additional configuration requirements.
    """

    def __init__(self, inputer, transformer, merger, outputer, json_config_doc): # pylint: disable=R0913,C0301
        """
        Save references to arguments and parse the JSON configuration
        document, then connect to the data store.
       
        @param self Object reference.
        @param inputer Input task.
        @param transformer Transformer task.
        @param merger Merger task.
        @param outputer Output task.
        @param json_config_doc JSON configuration document.
        """
        #  Parse the configuration JSON
        json_config_dictionary = json.loads(json_config_doc)
        # Create proxy for and connect to document store.
        self.doc_store = DataflowUtilities.setup_doc_store(
            json_config_dictionary)
        # Create objects to manage workflow execution.
        self.input_transform_executor = \
            MultiProcessInputTransformDataflowExecutor( \
                inputer, transformer, json_config_doc, self.doc_store)
        self.merge_output_executor = \
            MultiProcessMergeOutputDataflowExecutor( \
                merger, outputer, json_config_doc, self.doc_store)
          
    def execute(self):
        """
        Execute the dataflow - spills are, in turn, read from the 
        input, passed through the transform, saved into the spill 
        document store, then read from this, and passed through the 
        merge and output. 

        @param self Object reference.
        """
        self.input_transform_executor.execute()
        self.merge_output_executor.execute()

    @staticmethod
    def get_dataflow_description():
        """
        Get dataflow description.

        @return description.
        """
        description = "Run MICE how it is run in the control room. This\n"  
        description += "requires MongoDB and Celery to be installed. See\n"
        description += "the wiki links on how to do this at\n"
        description += \
            "http://micewww.pp.rl.ac.uk/projects/maus/wiki/MAUSCeleryConfiguration\n" # pylint:disable=C0301
        return description

from celery.task.control import inspect
from celery.task.control import broadcast
from mauscelery.tasks import execute_transform

class MultiProcessInputTransformDataflowExecutor: # pylint: disable=R0903, R0902
    """
    @class MultiProcessInputTransformDataflowExecutor
    Execute the input-transform part of MAUS dataflows using a Celery
    distributed task queue and worker nodes and a document store to
    cache spills after being output from transformers.

    If a document store is not given then a connection to one will be
    created. In this case, the class expects a document store class to
    be specified in the JSON configuration e.g.
    @verbatim
    doc_store_class = "MongoDBDocumentStore.MongoDBDocumentStore"
    @endverbatim
    The class used may have additional configuration requirements.

    Note that if using an in-memory data store then this MUST be
    provided as a constructor argument.    
    """
    def __init__(self, inputer, transformer, json_config_doc, doc_store = None): # pylint: disable=R0913,C0301
        """
        Constructor. Call super-class constructor then create
        a document data store.
        
        @param self Object reference.
        @param inputer Input task.
        @param transformer Transformer task.
        @param json_config_doc JSON configuration document.
        @param doc_store Document store.
        """
        self.inputer = inputer
        self.transformer = transformer
        self.json_config_doc = json_config_doc
        # Unique ID.
        self.client_config_id = "%s (%s)" \
            % (socket.gethostname(), os.getpid())
        self.spill_count = 0
        self.run_number = 0
        # Parse the configuration JSON
        self.json_config_dictionary = json.loads(self.json_config_doc)
        if (doc_store == None):
            # Create proxy for and connect to document store.
            self.doc_store = DataflowUtilities.setup_doc_store(
                self.json_config_dictionary)
        else:
            self.doc_store = doc_store

    @staticmethod
    def ping_celery_nodes():
        """
        Check for active Celery nodes.
        @return number of active nodes.
        @throws Exception if no active nodes.
        """
        print("Checking for active Celery nodes...")
        inspection = inspect()
        active_nodes = inspection.active()
        if (active_nodes == None):
            raise Exception("No active Celery nodes!")
        num_nodes = len(active_nodes)
        print("Number of active Celery nodes: %d" % num_nodes)
        return num_nodes

    def configure_celery_nodes(self):
        """
        Configure Celery nodes with the current MAUS configuration
        and transformer names via a Celery broadcast.
        @throws Exception if one or more nodes fail to reconfigure.
        """
        print "Configuring Celery nodes and birthing transforms..."
        if hasattr(self.transformer, "get_worker_names"):
            workers = self.transformer.get_worker_names()
        else:
            workers = self.transformer.__class__.__name__
        # First ping the workers to see if there's at least one.
        num_nodes = self.ping_celery_nodes()
        # Send a birth request. Give each node up to 1000s to reply,
        # but if they all reply before that just continue. This is
        # why we record the number of active nodes from the ping.
        print "Pushing new configuration..."
        results = []
        results = broadcast("birth", arguments={
            "transform": workers, 
            "configuration": self.json_config_doc,
            "config_id": self.client_config_id}, 
            reply=True, timeout=1000, limit=num_nodes)
        # Validate that all nodes updated.
        updated_ok = True
        for node in results:
            node_id = node.keys()[0]
            node_status = node[node_id]
            if node_status["status"] == "error":
                print "  Node error: %s : %s" % (node_id, node_status)
                updated_ok = False
            if node_status["status"] == "unchanged":
                print "  Node unchanged: %s" % node_id
#                updated_ok = False
            else:
                print "  Node configured: %s" % node_id
        if (not updated_ok):
            raise Exception("Celery nodes failed to configure!")
        print "Celery nodes configured!"

    @staticmethod
    def death_celery_nodes():
        """
        Call death on transforms in Celery nodes.
        """
        print "Requesting Celery nodes death transforms..."
        results = broadcast("death", reply=True)
        for node in results:
            node_id = node.keys()[0]
            node_status = node[node_id]
            if node_status.has_key("error"):
                print "  Node error: %s : %s" % (node_id, node_status)
            else:
                print "  Node transforms deathed: %s" % node_id
        print "Celery node transforms deathed!"

    def handle_celery_tasks(self, celery_tasks):
        """
        Wait for tasks currently being executed by Celery nodes to 
        complete.
        @param self Object reference.
        @param celery_tasks Celery AsyncResult used to access individual
        task status, indexed by a task ID.
        """
        # Check each submitted Celery task.
        for task_id in celery_tasks.keys():
            result = celery_tasks[task_id]
            if result.successful():
                del celery_tasks[task_id]
                print " Celery task %s SUCCESS " % task_id
                # Index results by spill_count so can present
                # results to merge-output in same order.
                spill = result.result
                self.spill_count += 1
                self.doc_store.put(str(self.spill_count), spill)
            elif result.failed():
                del celery_tasks[task_id]
                print " Celery task %s FAILED : %s : %s" \
                    % (task_id, result.result, result.traceback)

    @staticmethod
    def is_start_of_run(spill_doc):
        """
        Return true if spill represents a start of a run i.e. it has
        "daq_event_type" with value "start_of_run".
        @param spill_doc Spill as a JSON doc.
        @return True or False.
        """
        return (spill_doc.has_key("daq_event_type") and
            spill_doc["daq_event_type"] == "start_of_run")

    @staticmethod
    def get_run_number(spill_doc):
        """
        Extract run number from spill. Assumes spill has a 
        "run_num" entry.
        @param spill_doc Spill as a JSON doc.
        @return run number or -1 if none.
        """
        run_number = -1
        if spill_doc.has_key("run_num"):
            run_number = spill_doc["run_num"]
        return run_number

    def start_new_run(self, celery_tasks, run_number):
        """
        Prepare for a new run by waiting for current Celery
        tasks to complete, updating the local run number then
        reconfiguring the Celery nodes.
        @param self Object reference.
        @param celery_tasks Celery AsyncResult used to access individual
        task status, indexed by a task ID.
        @param run_number New run number.
        """
        print "New run detected...waiting for current processing to complete"
        # Wait for current tasks, from previous run, to complete.
        # This also ensures their timestamps < those of next run.
        while (len(celery_tasks) != 0):
            self.handle_celery_tasks(celery_tasks)
        self.run_number = run_number
        # Configure Celery nodes.
        print "---------- RUN %d ----------" % self.run_number
        self.configure_celery_nodes()

    def execute(self): # pylint: disable = R0914, R0912, R0915
        """
        Set up MAUS input tasks and, on receipt of spills, submit
        to transform tasks accessed via a distributed task queue. 

        @param self Object reference.
        @throws Exception if there are no active Celery nodes.
        """
        # Purge the document store.
        print("Purging data store")
        self.doc_store.clear()
        # Do an initial check for active Celery nodes.
        self.ping_celery_nodes()

        print("INPUT: Birth")
        assert(self.inputer.birth(self.json_config_doc) == True)
        emitter = self.inputer.emitter()
        map_buffer = DataflowUtilities.buffer_input(emitter, 1)

        self.spill_count = 0
        celery_tasks = {}
        i = 0
        while (len(map_buffer) != 0) or (len(celery_tasks) != 0):
            for spill in map_buffer:
                print("INPUT: read next spill")
                # Check run number.
                spill_doc = json.loads(spill)
                spill_run_number = self.get_run_number(spill_doc) 
                if (spill_run_number != self.run_number):
                    if (not self.is_start_of_run(spill_doc)):
                        print "  Missing a start_of_run spill!"
                    self.start_new_run(celery_tasks, spill_run_number)
                    print("TRANSFORM: processing spills")
                result = \
                    execute_transform.delay(spill, self.client_config_id, i) # pylint:disable=E1101, C0301
                print "Task ID: %s" % result.task_id
                # Save task ID for monitoring status.
                celery_tasks[result.task_id] = result
                i += 1
            map_buffer = DataflowUtilities.buffer_input(emitter, 1)
            if (len(map_buffer) != 0):
                print "Processed %d spills so far," % i,
                print(" %d spills left in buffer." % (len(map_buffer)))
            # Go through current transform tasks and see if any's completed. 
            self.handle_celery_tasks(celery_tasks)
        print "--------------------"
        # Invoke death 
        self.death_celery_nodes()
        print("TRANSFORM: transform tasks completed")
        print "--------------------"

    @staticmethod
    def get_dataflow_description():
        """
        Get dataflow description.

        @return description.
        """
        description = "Run MICE how it is run in the control room. This\n"  
        description += "requires MongoDB and Celery to be installed. See\n"
        description += "the wiki links on how to do this at\n"
        description += \
            "http://micewww.pp.rl.ac.uk/projects/maus/wiki/MAUSCeleryConfiguration\n" # pylint:disable=C0301
        description += "This runs input-transform dataflows only!"
        return description

class MultiProcessMergeOutputDataflowExecutor: # pylint: disable=R0903
    """
    @class MultiProcessMergeOutputDataflowExecutor
    Execute the merge-output part of MAUS dataflows reading spills
    from a document store where they have been previously cached. 

    If a document store is not given then a connection to one will be
    created. In this case, the class expects a document store class to
    be specified in the JSON configuration e.g.
    @verbatim
    doc_store_class = "MongoDBDocumentStore.MongoDBDocumentStore"
    @endverbatim
    The class used may have additional configuration requirements.

    Note that if using an in-memory data store then this MUST be
    provided as a constructor argument.
    """

    def __init__(self, merger, outputer, json_config_doc, doc_store = None): # pylint: disable=R0913,C0301
        """
        Constructor. Call super-class constructor then create
        a document data store.
        
        @param self Object reference.
        @param merger Merger task.
        @param outputer Output task.
        @param json_config_doc JSON configuration document.
        @param doc_store Document store.
        """
        self.merger = merger
        self.outputer = outputer
        self.json_config_doc = json_config_doc
        #  Parse the configuration JSON
        self.json_config_dictionary = json.loads(self.json_config_doc)
        if (doc_store == None):
            # Create proxy for and connect to document store.
            self.doc_store = DataflowUtilities.setup_doc_store(
                self.json_config_dictionary)
        else:
            self.doc_store = doc_store

    def execute(self):
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

        # This is a hack - it WILL be resolved in terms of
        # querying the database for remaining spills.

        last_time = datetime(1970, 01, 01)
        while True:
            recent_docs = self.doc_store.get_since(last_time)
            for doc in recent_docs:
                doc_id = doc["_id"]
                doc_time = doc["date"]
                if (doc_time > last_time):
                    last_time = doc_time
                spill = doc["doc"]
                print "Retrieved document %s (dated %s)" % \
                    (doc_id, doc_time)
                print "Executing Merge->Output for spill %s\n" % doc_id,
                try:
                    spill = self.merger.process(spill)
                    self.outputer.save(spill)
                except: # pylint:disable = W0702
                    ErrorHandler.HandleException({}, self)

        print("CLOSING PIPELINE: Sending END_OF_RUN to merger")

        end_of_run_spill = json.dumps({"END_OF_RUN":"END_OF_RUN"})
        spill = self.merger.process(end_of_run_spill)
        self.outputer.save(spill)
                
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
        description += "requires MongoDB to be installed. See\n"
        description += "the wiki links on how to do this at\n"
        description += \
            "http://micewww.pp.rl.ac.uk/projects/maus/wiki/MAUSCeleryConfiguration\n" # pylint:disable=C0301
        description += "This runs merge-output dataflows only!"
        return description
