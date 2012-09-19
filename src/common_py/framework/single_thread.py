"""
Single-threaded dataflows module.
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

import json
import maus_cpp.run_action_manager

from framework.utilities import DataflowUtilities

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

    def execute(self, job_header):
        """
        Execute the dataflow - events are, in turn, read from the 
        input, passed through the transform, merge and output.

        @param self Object reference.
        """
        # Note in all the assert statements - new style (API-compliant) modules
        # should raise an exception on fail and return void. Old style modules
        # would return true/false on success/failure of birth and death.
        try:
            print("INPUT: Reading input")
            assert(self.inputer.birth(self.json_config_doc) == True or \
                   self.inputer.birth(self.json_config_doc) == None)
            emitter = self.inputer.emitter()
            # This helps us time how long the setup that sometimes happens
            # in the first event takes
            print("HINT: MAUS will process 1 event only at first...")
            map_buffer = DataflowUtilities.buffer_input(emitter, 1)

            # NEEDS TO BE UNCOMMENTED - but only when I figure out how to
            # implement in multithreaded mode also
            #print "START OF RUN: Calling start of run"
            #run_number=DataflowUtilities.get_run_number
            #                                        (json.loads(map_buffer[0]))
            #maus_cpp.run_action_manager.start_of_run(run_number)

            print("TRANSFORM: Setting up transformer")
            assert(self.transformer.birth(self.json_config_doc) == True or \
                   self.transformer.birth(self.json_config_doc) == None)

            print("MERGE: Setting up merger")
            assert(self.merger.birth(self.json_config_doc) == True or \
                   self.merger.birth(self.json_config_doc) == None)

            print("OUTPUT: Setting up outputer")
            assert(self.outputer.birth(self.json_config_doc) == True or \
                   self.outputer.birth(self.json_config_doc) == None)

            print("PIPELINE: Get event, TRANSFORM, MERGE, OUTPUT, repeat")

            print("Processing JobHeader...")
            self.outputer.save(json.dumps(job_header))
            
            i = 1
            while len(map_buffer) != 0:
                for event in map_buffer:
                    event = self.transformer.process(event)
                    event = self.merger.process(event)
                    self.outputer.save(event)

                i += len(map_buffer)
                map_buffer = DataflowUtilities.buffer_input(emitter, 1)

                # Not Python 3 compatible print() due to backward
                # compatability. 
                print "TRANSFORM/MERGE/OUTPUT: ",
                print "Processed %d events so far," % i,
                print "%d events in buffer." % (len(map_buffer))
        except:
            raise

        finally:
            print("INPUT: Shutting down inputer")
            assert(self.inputer.death() == True or \
                   self.inputer.death() == None)

            print("TRANSFORM: Shutting down transformer")
            assert(self.transformer.death() == True or \
                   self.transformer.death() == None)

            print("MERGE: Shutting down merger")
            assert(self.merger.death() == True or \
                   self.merger.death() == None)

            print("OUTPUT: Shutting down outputer")
            assert(self.outputer.death() == True or \
                   self.outputer.death() == None)
            maus_cpp.run_action_manager.end_of_run()

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
