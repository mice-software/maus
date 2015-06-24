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
import json
import sys
import os

from docstore.DocumentStore import DocumentStoreException
from framework.utilities import DataflowUtilities
from framework.utilities import DocumentStoreUtilities
from framework.workers import WorkerBirthFailedException
from framework.workers import WorkerDeathFailedException

import maus_cpp.run_action_manager

class MergeOutputExecutor: # pylint: disable=R0903, R0902
    """
    @class MergeOutputExecutor
    Execute merge-output MAUS dataflows reading spills from a document
    store.

    This is the algorithm used to merge spills and pass the results to
    an output stream: 
    @verbatim
    run_number = None
    end_of_run = None
    is_birthed = FALSE
    last_time = 01/01/1970
    WHILE TRUE
      READ spills added since last time from document store
      IF spill IS "end_of_run" 
        end_of_run = spill
      IF spill_run_number != run_number
        IF is_birthed
          IF end_of_run == None
              end_of_run = 
                  {"daq_event_type":"end_of_run",  "run_number":run_number}
          Send end_of_run to merger
          DEATH merger and outputter
        BIRTH merger and outputter
        run_number = spill_run_number
        end_of_run = None
        is_birthed = TRUE
      MERGE and OUTPUT spill
    Send END_OF_RUN block to merger
    DEATH merger and outputter
    @endverbatim
    This assumes that all spills for run N-1 have time stamps less
    than that of spills for run N.

    is_birthed is used to ensure that there is no BIRTH-DEATH-BIRTH
    redundancy on receipt of the first spill. 

    If a document store is not given to the constructor then this
    class expects a document store class to be specified in the JSON
    configuration e.g.  
    @verbatim
    doc_store_class = "MongoDBDocumentStore.MongoDBDocumentStore"
    @endverbatim
    The document store class itself may have additional configuration
    requirements.  

    If a collection name is given to the constructor then this is
    used. Otherwise there must be a doc_collection_name key in the
    configuration.
    """

    def __init__(self, merger, outputer, config_doc, doc_store = None, collection_name = None): # pylint: disable=R0913,C0301
        """
        Save references to arguments and parse the JSON configuration
        document, then connect to the document store. 
        @param self Object reference.
        @param merger Merger task.
        @param outputer Output task.
        @param config_doc JSON configuration document.
        @param doc_store Document store.
        @param collection_name Collection name in document store.
        @throws ValueError if collection_name is None and there is no
        "doc_collection_name" entry in config_doc.
        @throws KeyError If any required configuration parameters are
        missing.
        @throws ValueError If any configuration values are invalid.
        @throws TypeError If any dynamic class does not implement
        a required interface.
        @throws DocumentStoreException if there is a problem
        connecting to the document store.
        """
        self.last_time = datetime(1970, 01, 01)
        self.job_footer = None
        self.merger = merger
        self.outputer = outputer
        self.config_doc = config_doc
        self.config = json.loads(self.config_doc)
        # Current run number (from spills).
        self.run_number = None
        # Last end_of_run spill received.
        self.end_of_run_spill = None
        # Counts of spills processed.
        self.spill_process_count = 0
        self.write_headers = self.config["header_and_footer_mode"] == "append"
        # Connect to doc store.
        if (doc_store == None):
            self.doc_store = DocumentStoreUtilities.setup_doc_store(\
                self.config)
        else:
            self.doc_store = doc_store
        # Get collection name.
        if (collection_name == None):
            if (not self.config.has_key("doc_collection_name") or
               (self.config["doc_collection_name"] in [None, ""])):
                raise ValueError("collection is not specified")
            else:
                self.collection = self.config["doc_collection_name"]
        else:
            self.collection = collection_name

    def start_of_job(self, job_header):
        """
        Birth the outputer and save job header
        """
        # Register CTRL-C handler.
        # signal.signal(signal.SIGINT, self.handle_ctrl_c) 
        birth = self.outputer.birth(self.config_doc)
        if not(birth == True or birth == None): # new-style birth() returns None
            raise WorkerBirthFailedException(self.outputer.__class__)
        if self.write_headers:
            self.outputer.save(json.dumps(job_header))

 
    def start_of_run(self):
        """
        Prepare for a new run

        Calls maus_cpp.globals.start_of_run(), births the merger, writes the
        start_of_run and handles some internal bureaucracy

        @param self Object reference.
        @throws WorkerBirthFailedException if birth returns False.
        @throws Exception if there is a problem when birth is called.
        """
        self.spill_process_count = 0
        self.end_of_run_spill = None
        print "---------- START RUN %d ----------" % self.run_number
        print("BIRTH merger %s" % self.merger.__class__)
        birth = self.merger.birth(self.config_doc)
        if not(birth == True or birth == None): # new-style birth() returns None
            raise WorkerBirthFailedException(self.merger.__class__)
        run_header = maus_cpp.run_action_manager.start_of_run(self.run_number)
        if self.write_headers:
            print "SAVE RUN HEADER", run_header
            self.outputer.save(run_header)

    def end_of_run(self):
        """
        End a run
        
        Sends an end_of_run spill through the merger and outputer then deaths
        the merger. The end_of_run spill is the last one that was encountered
        before a change in run was detected. If there was no such end_of_run
        then a dummy is created.

        @param self Object reference.
        @throws WorkerDeathFailedException if death returns False.
        """
        if (self.end_of_run_spill == None):
            print "  Missing an end_of_run spill..."
            print "  ...creating one to flush the mergers!"
            self.end_of_run_spill = {"daq_event_type":"end_of_run",
                                     "maus_event_type":"Spill",
                                     "run_number":self.run_number,
                                     "spill_number":-1}
        end_of_run_spill_str = json.dumps(self.end_of_run_spill)
        end_of_run_spill_str = self.merger.process(end_of_run_spill_str)
        self.outputer.save(end_of_run_spill_str)
        death = self.merger.death()
        if not(death == True or death == None): # new-style death() returns None
            raise WorkerDeathFailedException(self.merger.__class__)
        run_footer = maus_cpp.run_action_manager.end_of_run(self.run_number)
        if self.write_headers:
            print "SAVE RUN FOOTER", run_footer
            self.outputer.save(run_footer)
        print "---------- END RUN %d ----------" % self.run_number

    def end_of_job(self, job_footer):
        """
        Output footer and then death the outputer
        """
        if self.write_headers:
            self.outputer.save(json.dumps(job_footer))
        death = self.outputer.death()
        if not(death == True or death == None): # new-style death() returns None
            raise WorkerDeathFailedException(self.outputer.__class__)

    def process_event(self, spill):
        """
        Process the spill
        """
        spill_doc = json.loads(spill)
        if not "maus_event_type" in spill_doc.keys():
            print json.dumps(spill_doc, indent=2)
            raise KeyError("Event has no maus_event_type")
        if spill_doc["maus_event_type"] != "Spill":
            outputter_ret = self.outputer.save(str(spill))
            if not (outputter_ret == None or outputter_ret == True):
                raise RuntimeError("Failed to execute Output")
        else:
            # Check for change in run.
            spill_run_number = DataflowUtilities.get_run_number(spill_doc)
            if (DataflowUtilities.is_end_of_run(spill_doc)):
                self.end_of_run_spill = spill_doc
            if (spill_run_number != self.run_number):
                if (self.run_number != None):
                    # Death workers only if birthed.
                    self.end_of_run()
                self.run_number = spill_run_number
                self.start_of_run()
            # Handle current spill.
            merged_spill = self.merger.process(spill)
            outputter_ret = self.outputer.save(str(merged_spill))
            if not (outputter_ret == None or outputter_ret == True):
                raise RuntimeError("Failed to execute Output")
            self.spill_process_count += 1
            print "Processed %d DAQ events from run %s" % \
                  (self.spill_process_count, self.run_number)

    def execute(self, job_header, job_footer, will_run_until_ctrl_c=True):
        """
        Execute the dataflow. Retrieve spills from document store and
        submit to merge and output workers. A date query is run on
        the document store so that only the spills added since the
        previous query are processed on each iteration.
        @param self Object reference.
        @param job_header JobHeader maus_event to be written at start_of_job
        @param job_footer JobFooter maus_event to be written at end_of_job
        @throws RabbitMQException if RabbitMQ cannot be contacted.
        @throws DocumentStoreException if there is a problem
        using the document store.
        @throws WorkerBirthFailedException if birth returns False.
        @throws WorkerDeathFailedException if death returns False.
        @throws Exception if there is a problem when merging or
        outputting a spill, birthing or deathing the merger or
        outputer or merging an end_of_run spill.
        """
        # Darkness and sinister evil 1 - the loop structure and exception
        # handling in this code is quite elaborate. Requirements:
        # * If we receive a sigint we should exit after clearing mongo buffer
        # * If mongo access throws an error we should ignore it
        # * See #1190

        # Darkness and sinister evil 2 - somewhere in the combination of ROOT,
        # SWIG, Python; I get a segmentation fault from OutputCppRoot fillevent
        # (*_outfile) << fillEvent; command depending
        # on which function I call outputer.save(...) from when outputer is
        # OutputCppRoot. I think I'm going to be sick (some scoping/garbage
        # collection issue?)
        #
        # Requirement: all outputer.save calls must be made from this execute
        # function to work around this  - Rogers
        try:
            self.job_footer = job_footer
            self.start_of_job(job_header)
            run_again = True # always run once
            the_time = datetime.now()
            while run_again:
                # enforce one second delay on MongoDB lookups (otherwise the
                # lookups are too frequent and MongoDB chews up processor - we
                # pull down a local queue anyway)
                if (datetime.now()-the_time).seconds > 0:
                    the_time = datetime.now()
                    run_again = will_run_until_ctrl_c and \
                                self._execute_inner_loop()
        except KeyboardInterrupt:
            print "Received SIGINT and dying"
            sys.exit(0)
        except Exception:
            sys.excepthook(*sys.exc_info())
            raise
        finally:
            # Finish the final run, if it was started; then finish the job
            if (self.run_number != None):
                print "No more data and set to stop - ending run"
                self.end_of_run()
            print "Ending job"
            self.end_of_job(self.job_footer)

    def _execute_inner_loop(self):
        """
        Get documents off the doc store and process them.

        @returns False if a keyboard interrupt is received, indicating that the
                 iteration should not wait for new data
        """
        # More darkness - MongoDB is very unstable/picky. This has been tuned
        # to work with MongoDB even for larger datasets
        sys.stdout.flush()
        sys.stderr.flush()
        keyboard_interrupt = False
        try:
            docs = self.doc_store.get_since(self.collection,
                                            self.last_time)
            # Iterate using while, not for, since docs is an
            # iterator which streams data from database and we
            # want to detect database errors.
            while True:
                try:
                    doc = self.docs_next(docs)
                    doc_id = doc["_id"]
                    doc_time = doc["date"]
                    spill = doc["doc"]
                    print "<"+str(datetime.now().time())+"> Read event "+\
                          str(doc_id)+" reconstructed at "+str(doc_time)+\
                          " compared to last_time "+str(self.last_time)+\
                          " with "+str(len(doc))+" more docs)"
                    if (doc_time >= self.last_time):
                        self.last_time = doc_time
                    try:
                        self.process_event(spill)
                    except TypeError:
                        print spill
                        raise
                    sys.stdout.flush()
                    sys.stderr.flush()
                except StopIteration:
                    # No more data so exit inner loop.
                    raise
                except KeyboardInterrupt:
                    # If there is a keyboard interrupt we should clear
                    # the buffer before quitting
                    print "Received SIGINT in", os.getpid(), \
                          "- processing open spills before quitting"
                    keyboard_interrupt = True
        except StopIteration:
            # No more data so exit inner loop. If we have received a
            # SIGINT anywhere, end the loop; otherwise wait for data
            if keyboard_interrupt:
                print "Finished processing spills and exiting on SIGINT"
                return False
        except KeyboardInterrupt:
            # If there is a keyboard interrupt we should clear
            # the buffer before quitting
            print "Received SIGINT in", os.getpid(), \
                  "- processing open spills before quitting"
            return False
        except DocumentStoreException:
            # Some instability in MongoDB - ignore and carry on
            sys.excepthook(*sys.exc_info())
            print "Caught document store error - ignore and carry on"
        return not keyboard_interrupt

    @staticmethod
    def get_dataflow_description():
        """
        Get dataflow description.
        @return description.
        """
        description = "Run MAUS merge-output in multi-processing mode. This\n"  
        description += "requires MongoDB to be installed. See\n" 
        description += "the wiki links on how to do this at\n"
        description += \
            "http://micewww.pp.rl.ac.uk/projects/maus/wiki/MAUSDevs\n"
        return description

    @staticmethod
    def docs_next(_docs):
        """
        Try to access document from the iterator - if it possible we
        pop it from front; else try to next() it; else give up

        @param _docs list or generator that yields a set of documents on the
               docstore
        @param max_number_of_retries Integer number of times to retry accessing
               document before giving up
        @param retry_time time to wait between retries 
        @throws DocumentStoreException if there was a problem accessing the
                docstore
        @throws StopIteration if the docstore was empty
        """
        # Note retry counter 
        while True:
            try:
                try:
                    return _docs.pop(0)
                except AttributeError:
                    return _docs.next()
                except IndexError:
                    raise StopIteration("Ran out of events")
            except StopIteration:
                raise
            except Exception:
                raise DocumentStoreException("Failed to access document store")

