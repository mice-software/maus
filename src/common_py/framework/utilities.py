"""
MAUS framework utilities module.
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

from docstore.DocumentStore import DocumentStore

class DataflowUtilities: # pylint: disable=W0232
    """
    @class DataflowUtilities.
    Dataflow-related utility functions.
    """

    @staticmethod
    def buffer_input(input_emitter, number_of_inputs):
        """
        Buffer an input stream of strings by only reading up to the
        first N inputs into memory. Returns an array of inputs. 

        @param input_emitter Input stream of strings.
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
        @throws KeyError. If there is no doc_store_class in the JSON
        configuration.
        @throws ValueError. If the module name do not exist,
        or the class is not in the module.
        @throws TypeError If doc_store_class does not sub-class
        docstore.DocumentStore.DocumentStore.
        """
        # Get class and bust up into module path and class name.
        doc_store_class = json_config_dictionary["doc_store_class"]
        path = doc_store_class.split(".")
        doc_store_class = path.pop()
        import_path = ".".join(path)
        try:
            module_object = __import__(import_path)
            path.pop(0)
            # Dynamically import the module.
            for sub_module in path:
                module_object = getattr(module_object, sub_module) 
            # Get class object.
            class_object = getattr(module_object, doc_store_class)
            # Create instance of class object.
            doc_store = class_object()
        except:
            raise ValueError("Module %s does not exist or has a problem" \
                % doc_store_class)
        # Validate.
        if (not isinstance(doc_store, DocumentStore)):
            raise TypeError("Document store class %s does not implement %s" \
                % (doc_store_class, DocumentStore))
        # Connect to the document store.
        doc_store.connect(json_config_dictionary) 
        return doc_store

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
