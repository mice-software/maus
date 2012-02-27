"""
Multi-process dataflows module.
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

from celery.task.control import inspect
from celery.task.control import broadcast
from framework.utilities import DataflowUtilities
from mauscelery.tasks import execute_transform
import ErrorHandler

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
        self.run_number = None
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
        num_tasks = len(celery_tasks)
        current = 0
        while (current < num_tasks):
            result = celery_tasks[current]
            if result.successful():
                celery_tasks.pop(current)
                num_tasks -= 1
                print " Celery task %s SUCCESS " % result.task_id
                # Index results by spill_count so can present
                # results to merge-output in same order.
                spill = result.result
                self.spill_count += 1
                self.doc_store.put(str(self.spill_count), spill)
            elif result.failed():
                celery_tasks.pop(current)
                num_tasks -= 1
                print " Celery task %s FAILED : %s : %s" \
                    % (result.task_id, result.result, result.traceback)
            else:
                current += 1

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
        celery_tasks = []
        i = 0
        while (len(map_buffer) != 0) or (len(celery_tasks) != 0):
            for spill in map_buffer:
                print("INPUT: read next spill")
                # Check run number.
                spill_doc = json.loads(spill)
                spill_run_number = DataflowUtilities.get_run_number(spill_doc) 
                if (spill_run_number == None):
                    # There was no run_num in spill so add a 0 (pure MC run).
                    spill_run_number = 0
                    spill_doc["run_num"] = spill_run_number
                    spill = json.dumps(spill_doc)
                if (spill_run_number != self.run_number):
                    if (not DataflowUtilities.is_start_of_run(spill_doc)):
                        print "  Missing a start_of_run spill!"
                    self.start_new_run(celery_tasks, spill_run_number)
                    print("TRANSFORM: processing spills")
                result = \
                    execute_transform.delay(spill, self.client_config_id, i) # pylint:disable=E1101, C0301
                print "Task ID: %s" % result.task_id
                # Save asynchronous result object for checking task status. 
                celery_tasks.append(result)
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
        self.run_number = None

    def end_run(self):
        """
        End a run by sending an END_OF_RUN spill through the merger
        then death the merger and outputer.
        @param self Object reference.
        """
        print("CLOSING PIPELINE: Sending END_OF_RUN to merger")
        end_of_run_spill = json.dumps({"END_OF_RUN":"END_OF_RUN"})
        spill = self.merger.process(end_of_run_spill)
        self.outputer.save(spill)
        print("MERGE: Shutting down merger")
        assert(self.merger.death() == True)
        print("OUTPUT: Shutting down outputer")
        assert(self.outputer.death() == True)

    def start_new_run(self, run_number):
        """
        Prepare for a new run by updating the local run number then
        birthing the merger and outputer.
        @param self Object reference.
        @param run_number New run number.
        """
        self.run_number = run_number
        print "---------- RUN %d ----------" % self.run_number
        print("MERGE: Setting up merger")
        assert(self.merger.birth(self.json_config_doc) == True)
        print("OUTPUT: Setting up outputer")
        assert(self.outputer.birth(self.json_config_doc) == True)

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
                spill = doc["doc"]
                print "Retrieved document %s (dated %s)" % \
                    (doc_id, doc_time)
                if (doc_time > last_time):
                    last_time = doc_time
                # Check run number.
                spill_doc = json.loads(spill)
                spill_run_number = DataflowUtilities.get_run_number(spill_doc) 
                if (spill_run_number != self.run_number):
                    print "New run detected..."
                    print "Waiting for current processing to complete"
                    self.end_run()
                    self.start_new_run(spill_run_number)
                print "Executing Merge->Output for spill %s\n" % doc_id,
                try:
                    spill = self.merger.process(spill)
                    self.outputer.save(spill)
                except: # pylint:disable = W0702
                    ErrorHandler.HandleException({}, self)

        self.end_run()

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
