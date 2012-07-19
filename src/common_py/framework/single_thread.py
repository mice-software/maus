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

        print "START OF RUN: Calling start of run"
        run_number = DataflowUtilities.get_run_number(json.loads(map_buffer[0]))
        maus_cpp.run_action_manager.start_of_run(run_number)

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

        print("TRANSFORM: Shutting down transformer")
        assert(self.transformer.death() == True)

        print("MERGE: Shutting down merger")
        assert(self.merger.death() == True)

        print("OUTPUT: Shutting down outputer")
        assert(self.outputer.death() == True)
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
