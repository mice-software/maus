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
import maus_cpp.converter
import ROOT

from framework.utilities import DataflowUtilities

class PipelineSingleThreadDataflowExecutor: # pylint: disable=R0902
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
        head_mode = json.loads(self.json_config_doc)["header_and_footer_mode"]
        self.write_headers = head_mode == "append"
        self.run_number = "first" # used to register first run
        self.end_of_run_spill = None

    def execute(self, job_header, job_footer):
        """
        Execute the dataflow

        Birth outputter, write job header, birth merger, transformer, inputter.
        Read events from the input, pass through the transform, merge and
        output. Death inputter, transformer, merger; write job footer; death
        outputter.

        Birth order is chosen because I want to write JobHeader as early as
        possible and JobFooter as late as possible.

        @param job_header JobHeader in python (i.e. dicts etc) format.
        @param job_footer JobFooter in python (i.e. dicts etc) format.
        """
        # Note in all the assert statements - new style (API-compliant) modules
        # should raise an exception on fail and return void. Old style modules
        # would return true/false on success/failure of birth and death.
        try:
            print("OUTPUT: Setting up outputer")
            birth = self.outputer.birth(self.json_config_doc)
            assert(birth == True or birth == None)

            print("Writing JobHeader...")
            if self.write_headers:
                self.outputer.save(json.dumps(job_header))

            print("INPUT: Setting up input")
            birth = self.inputer.birth(self.json_config_doc)
            assert(birth == True or birth == None)

            print("PIPELINE: Get event, TRANSFORM, MERGE, OUTPUT, repeat")

            emitter = self.inputer.emitter()
            # This helps us time how long the setup that sometimes happens
            # in the first event takes
            print("HINT: MAUS will process 1 event only at first...")
            map_buffer = DataflowUtilities.buffer_input(emitter, 1)
            
            i = 0
            while len(map_buffer) != 0:
                for event in map_buffer:
                    self.process_event(event)
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
            if self.run_number == "first":
                self.run_number = 0
            self.end_of_run(self.run_number)

            print("INPUT: Shutting down inputer")
            death = self.inputer.death()
            assert(death == True or death == None)
            if self.write_headers:
                self.outputer.save(json.dumps(job_footer))

            death = self.outputer.death()
            print("OUTPUT: Shutting down outputer")
            assert(death == True or death == None)

    def process_event(self, event):
        """
        Process a single event
        
        Process a single event - if it is a Spill, check for run_number change
        and call EndOfEvent/StartOfEvent if run_number has changed.
        """
        if event == "":
            raise StopIteration("End of event")
        event_json = None
        try:
            evtype = event.GetEventType()
        except AttributeError:
            try:
                event_json = maus_cpp.converter.json_repr(event)
                evtype = DataflowUtilities.get_event_type(event_json)
            except ValueError:
                print 'could not determine type'
                return
        except:
            raise

        if evtype == "Spill":
            if event_json is not None:
                current_run_number = DataflowUtilities.get_run_number(event_json) # pylint: disable=C0301
                if (DataflowUtilities.is_end_of_run(event_json)):
                    self.end_of_run_spill = event_json
            else:
                current_run_number = event.GetSpill().GetRunNumber()
                if (event.GetEventType() == "end_of_run"):
                    self.end_of_run_spill = event
            if current_run_number != self.run_number:
                if self.run_number != "first":
                    self.end_of_run(self.run_number)
                self.start_of_run(current_run_number)
                self.run_number = current_run_number
            event = self.transformer.process(event)
            event = self.merger.process(event)
        self.outputer.save(event)
        try:
            maus_cpp.converter.del_data_repr(event)
        except: # pylint: disable = W0702
            pass

    def start_of_run(self, new_run_number):
        """
        At the start_of_run, we birth the merger and transformer, then
        call start_of_run on the run_action_manager

        @param new_run_number run number of the run that is starting
        """
        run_header = maus_cpp.run_action_manager.start_of_run(new_run_number)

        print("MERGE: Setting up merger")
        birth = self.merger.birth(self.json_config_doc)
        assert(birth == True or birth == None)

        print("TRANSFORM: Setting up transformer")
        birth = self.transformer.birth(self.json_config_doc)
        assert(birth == True or birth == None)
        if self.write_headers:
            self.outputer.save(run_header)

    def end_of_run(self, old_run_number):
        """
        At the end_of_run, we death the transformer and merger, then call
        end_of_run on the run_action_manager (note reverse ordering, not that it
        should matter)

        @param old_run_number run number of the run that is ending
        """
        if (self.end_of_run_spill == None):
            print "  Missing an end_of_run spill..."
            print "  ...creating one to flush the mergers!"
            self.end_of_run_spill = {"daq_event_type":"end_of_run",
                                     "maus_event_type":"Spill",
                                     "run_number":self.run_number,
                                     "spill_number":-1}
            #end_of_run_spill_str = json.dumps(self.end_of_run_spill)
            #end_of_run_spill_str = self.merger.process(end_of_run_spill_str)

            data = ROOT.MAUS.Data() # pylint: disable=E1101
            spill = ROOT.MAUS.Spill() # pylint: disable=E1101
            spill.SetRunNumber(self.run_number)
            spill.SetDaqEventType("end_of_run")
            spill.SetSpillNumber(-1)
            data.SetSpill(spill)

            end_of_run_spill_str = self.merger.process(data)
            if self.write_headers: # write to disk only if write_headers is set
                self.outputer.save(end_of_run_spill_str)
            try:
                maus_cpp.converter.del_data_repr(data)
            except TypeError:
                pass
        try:
            maus_cpp.converter.del_data_repr(self.end_of_run_spill)
        except TypeError:
            pass
        self.end_of_run_spill = None

        print("TRANSFORM: Shutting down transformer")
        death = self.transformer.death()
        assert(death == True or death == None)

        print("MERGE: Shutting down merger")
        death = self.merger.death()
        assert(death == True or death == None)

        run_footer = maus_cpp.run_action_manager.end_of_run(old_run_number)
        if self.write_headers:
            self.outputer.save(run_footer)

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
