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

import os
import json
import socket

from docstore.DocumentStore import DocumentStoreException
from framework.utilities import CeleryUtilities
from framework.utilities import DataflowUtilities
from framework.utilities import DocumentStoreUtilities
from framework.utilities import RabbitMQException
from framework.workers import WorkerUtilities
from framework.workers import WorkerBirthFailedException
from framework.workers import WorkerDeathFailedException
from mauscelery.tasks import execute_transform

class InputTransformExecutor: # pylint: disable=R0903, R0902
    """
    @class InputTransformExecutor
    Execute input-transform MAUS dataflows using a Celery
    distributed task queue and nodes and a document store to
    cache spills after being output from transformers. 

    This is the algorithm used to transform spills from an input
    stream: 
    @verbatim
    CLEAR document store
    run_number = None
    WHILE an input spill is available
      GET next spill
      IF spill does not have a run number
        # Assume pure MC
        spill_run_number = 0
      IF (spill_run_number != run_number)
        # We've changed run.
        IF spill is NOT a start_of_run spill
          WARN user of missing start_of_run spill
        WAIT for current Celery tasks to complete
          WRITE result spills to document store
        run_number = spill_run_number
        CONFIGURE Celery by DEATHing current transforms 
            and BIRTHing new transforms
      TRANSFORM spill using Celery
      WRITE result spill to document store
     DEATH Celery worker transforms
    @endverbatim
    If there is no initial start_of_run spill (or no spill_num in the
    spills) in the input stream then spill_run_number will be 0,
    run_number will be None and a Celery configuration will be done
    before the first spill needs to be transformed.  

    Spills are inserted into the document store in the order of their
    return from Celery workers. This may not be in synch with the order in
    which they were originally read from the input stream. 

    If a document store is not given to the constructor then this
    class expects a document store class to be specified in the JSON
    configuration e.g.  
    @verbatim
    doc_store_class = "MongoDBDocumentStore.MongoDBDocumentStore"
    @endverbatim
    The document store class itself may have additional configuration
    requirements.  

    If the configuration has a doc_collection_name key and this
    is not None, "" or "auto" then this collection is used, otherwise
    a name is auto-generated of form HOSTNAME-PROCESSID.
    """
    def __init__(self, inputer, transformer, config_doc, doc_store = None): # pylint: disable=R0913,C0301
        """
        Save references to arguments and parse the JSON configuration
        document, then connect to the document store. 
        @param self Object reference.
        @param inputer Input task.
        @param transformer Transformer task.
        @param config_doc JSON configuration document.
        @param doc_store Document store.
        @throws KeyError If any required configuration parameters are
        missing.
        @throws ValueError If any configuration values are invalid.
        @throws TypeError If any dynamic class does not implement
        a required interface.
        @throws DocumentStoreException if there is a problem
        connecting to the document store.
        """
        self.inputer = inputer
        self.transformer = transformer
        self.config_doc = config_doc
        self.config = json.loads(self.config_doc)
        # Unique ID for execution.
        self.config_id = "%s-%s" % (socket.gethostname(), os.getpid())
        # Current run number (from spills).
        self.run_number = "first"
        # Counts of spills input, processed, failed.
        self.spill_input_count = 0
        self.spill_process_count = 0
        self.spill_fail_count = 0
        # Active Celery jobs - Celery AsyncResult objects.
        self.celery_tasks = []
        # Connect to doc store.
        if (doc_store == None):
            self.doc_store = DocumentStoreUtilities.setup_doc_store( \
                self.config)
        else:
            self.doc_store = doc_store
        # Get collection name.        
        if (not self.config.has_key("doc_collection_name") or
           (self.config["doc_collection_name"] in     
           [None, "", "auto"])):
            self.collection = self.config_id
        else:
            self.collection = self.config["doc_collection_name"]

    def poll_celery_tasks(self):
        """
        Wait for tasks currently being executed by Celery nodes to 
        complete.
        @param self Object reference.
        @throws RabbitMQException if RabbitMQ cannot be contacted.
        @throws DocumentStoreException if there is a problem
        using the document store.
        """
        # Check each submitted Celery task.
        num_tasks = len(self.celery_tasks)
        current = 0
        while (current < num_tasks):
            result = self.celery_tasks[current]
            try:
                # Catch any RabbitMQ errors when querying status.
                is_successful = result.successful()
                is_failed = result.failed()
                result_result = result.result
                result_traceback = result.traceback
            except socket.error as exc:
                raise RabbitMQException(exc)
            if is_successful:
                self.celery_tasks.pop(current)
                num_tasks -= 1
                spill = result_result
                self.spill_process_count += 1
                print " Celery task %s SUCCESS " % (result.task_id)
                print "   SAVING to collection %s (with ID %s)" \
                    % (self.collection, self.spill_process_count)
                try:
                    self.doc_store.put(self.collection,
                        str(self.spill_process_count), spill)
                except Exception as exc:
                    raise DocumentStoreException(exc)
                self.print_counts()
            elif is_failed:
                self.celery_tasks.pop(current)
                self.spill_fail_count += 1
                num_tasks -= 1
                print " Celery task %s FAILED : %s : %s" \
                    % (result.task_id, result_result, result_traceback)
                self.print_counts()
            else:
                current += 1

    def submit_spill_to_celery(self, spill):
        """
        Submit a spill to Celery for transforming.
        @param self Object reference.
        @param spill string representation of json document
        @throws RabbitMQException if RabbitMQ cannot be contacted.
        """
        try:
            result = \
                execute_transform.delay(spill, self.config_id) # pylint:disable=E1101, C0301
        except socket.error as exc:
            raise RabbitMQException(exc)
        print "Task ID: %s" % result.task_id
        # Save asynchronous result object for checking task status. 
        self.celery_tasks.append(result)

    def start_of_run(self, run_number):
        """
        Prepare for a new run by waiting for current Celery
        tasks to complete, updating the local run number then
        reconfiguring the Celery nodes.
        @param self Object reference.
        @param run_number New run number.
        """
        print "New run detected...waiting for current processing to complete"
        # Wait for current tasks, from previous run, to complete.
        # This also ensures their timestamps < those of next run.
        while (len(self.celery_tasks) != 0):
            self.poll_celery_tasks()
        self.run_number = run_number
        # Configure Celery nodes.
        print "---------- RUN %d ----------" % self.run_number
        print "Configuring Celery nodes and birthing transforms..."
        transform = WorkerUtilities.get_worker_names(self.transformer)
        CeleryUtilities.birth_celery(transform,
            self.config_doc, self.config_id)
        print "Celery nodes configured!"

    def end_of_run(self, run_number): # pylint: disable=W0613, R0201
        """
        Kill the old run
        @param self Object reference.
        @param run_number Old run number.
        """
        CeleryUtilities.death_celery()

    def print_counts(self):
        """
        Print spill counts to date.
        @param self Object reference.
        """
        print "Spills input: %d Processed: %d Failed %d" % \
            (self.spill_input_count, self.spill_process_count,
             self.spill_fail_count)

    def execute(self, _job_header, _job_footer): # pylint: disable = R0914, R0912, R0915, C0301
        """
        Set up MAUS input tasks and, on receipt of spills, submit
        to transform tasks accessed via a distributed task queue. 
        @param self Object reference.
        @throws WorkerBirthFailedException if inputer.birth returns False.
        @throws WorkerDeathFailedException if inputer death returns False.
        @throws RabbitMQException if RabbitMQ cannot be contacted.
        @throws NoCeleryNodeException if no Celery nodes.
        @throws CeleryNodeException if Celery nodes fail to
        configure. 
        @throws DocumentStoreException if there is a problem
        using the document store.
        """
        # Purge the document store.
        print("Purging document store")
        DocumentStoreUtilities.create_doc_store_collection(self.doc_store,
            self.collection)
        # Do an initial check for active Celery nodes.
        print("Checking for active Celery nodes...")
        num_nodes = CeleryUtilities.ping_celery_nodes()
        print("Number of active Celery nodes: %d" % num_nodes)

        # reset input
        print("INPUT: Birth")
        if (not self.inputer.birth(self.config_doc)):
            raise WorkerBirthFailedException(self.inputer.__class__)
        emitter = self.inputer.emitter()
        map_buffer = DataflowUtilities.buffer_input(emitter, 1)

        # reset counters
        self.spill_input_count = 0
        self.spill_process_count = 0
        self.spill_fail_count = 0
        self.celery_tasks = []

        # process events
        while (len(map_buffer) != 0) or (len(self.celery_tasks) != 0):
            for spill in map_buffer:
                self.process_event(spill)
            map_buffer = DataflowUtilities.buffer_input(emitter, 1)
            if (len(map_buffer) != 0):
                print("%d spills left in buffer" % (len(map_buffer)))
            # Go through current transform tasks and see if any's completed. 
            self.poll_celery_tasks()
        print "--------------------"
        # Invoke death 
        print "Requesting Celery nodes death transforms..."
        self.end_of_run(self.run_number)
        print "Celery node transforms deathed!"
        print("TRANSFORM: transform tasks completed")
        print("INPUT: Death")
        if (not self.inputer.death()):
            raise WorkerDeathFailedException(self.inputer.__class__)
        print "--------------------"

    def process_event(self, event):
        """
        Process a single event
        
        Process a single event - if it is a Spill, check for run_number change
        and call EndOfEvent/StartOfEvent if run_number has changed.
        """
        event_json = json.loads(event)
        if DataflowUtilities.get_event_type(event_json) == "Spill":
            current_run_number = DataflowUtilities.get_run_number(event_json)
            if current_run_number != self.run_number:
                self.spill_input_count += 1
                if self.run_number != "first":
                    self.end_of_run(self.run_number)
                self.start_of_run(current_run_number)
                self.run_number = current_run_number
            self.submit_spill_to_celery(event)

    @staticmethod
    def get_dataflow_description():
        """
        Get dataflow description.
        @return description.
        """
        description = "Run MAUS in multi-processing mode. This\n"  
        description += "requires Celery and MongoDB to be installed. See\n" 
        description += "the wiki links on how to do this at\n"
        description += \
            "http://micewww.pp.rl.ac.uk/projects/maus/wiki/MAUSDevs\n"
        return description
