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

import socket

from celery.task.control import inspect
from celery.task.control import broadcast
from docstore.DocumentStore import DocumentStore
from docstore.DocumentStore import DocumentStoreException

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
    def is_start_of_run(spill):
        """
        Return true if spill represents a start of a run i.e. it has
        "daq_event_type" with value "start_of_run".
        @param spill Spill document as a dictionary.
        @return True or False.
        """
        return (spill.has_key("daq_event_type") and
            spill["daq_event_type"] == "start_of_run")

    @staticmethod
    def get_run_number(spill):
        """
        Extract run number from spill. Assumes spill has a 
        "run_num" entry.
        @param spill Spill document as a dictionary.
        @return run number or None if none.
        """
        run_number = None
        if spill.has_key("run_num"):
            run_number = spill["run_num"]
        return run_number

class DocumentStoreUtilities: # pylint: disable=W0232
    """
    @class DocumentStoreUtilities.
    Document store-related utility functions.
    """

    @staticmethod
    def setup_doc_store(config):
        """
        Set up document store. The document store is configured via
        the following parameter in the JSON configuration:

        -doc_store_class - document store class name. This value
         is assumed to be of form "modulename.classname".

        It is assumed that the class can be loaded by execution of 
        statements analogous to "import classname from modulename".
        The "connect" method of the document store is then invoked to 
        initialise the connection.

        @param config JSON configuration.
        @return document store.
        @throws KeyError. If there is no doc_store_class in the JSON
        configuration.
        @throws ValueError. If the module name do not exist,
        or the class is not in the module.
        @throws TypeError If doc_store_class does not sub-class
        docstore.DocumentStore.DocumentStore.
        @throws DocumentStoreException if there is a problem connecting
        to the document store.
        """
        # Get class and bust up into module path and class name.
        doc_store_class = config["doc_store_class"]
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
        try:
            doc_store.connect(config) 
        except Exception as exc:
            raise DocumentStoreException(exc)
        return doc_store

    @staticmethod
    def create_doc_store_collection(doc_store, collection):
        """
        Create a collection in the document store. If it exists already
        then it is deleted.
        @param doc_store Document store.
        @param collection Collection name.
        @throws DocumentStoreException if there is a problem.
        """
        try:
            if (doc_store.has_collection(collection)):
                doc_store.delete_collection(collection)
            doc_store.create_collection(collection)
        except Exception as exc:
            raise DocumentStoreException(exc)

class CeleryUtilities: # pylint: disable=W0232
    """
    @class CeleryUtilities.
    Celery-related utility functions.
    """

    @staticmethod
    def ping_celery_nodes():
        """
        Check for active Celery nodes.
        @return number of active nodes.
        @throws RabbitMQException if RabbitMQ cannot be contacted.
        @throws NoCeleryWorkerException if no Celery workers.
        """
        inspection = inspect()
        try:
            active_nodes = inspection.active()
        except socket.error as exc:
            raise RabbitMQException(exc)
        if (active_nodes == None):
            raise NoCeleryWorkerException()
        return len(active_nodes)

    @staticmethod
    def birth_celery(transform, config, config_id, num_nodes, timeout = 1000):
        """
        Set new configuration and transforms in Celery nodes, and
        birth the transforms. Each node is given up to 1000s to reply
        but if they all reply then the method returns sooner.
        @param transform Either a single name can be given - representing
        a single transform - or a list of transforms - representing a
        MapPyGroup. Sub-lists are treated as nested MapPyGroups. If None
        then the current transform is deathed and rebirthed.  
        @param config Valid JSON configuration document
        @param config_id Configuration ID from client.
        @param num_nodes Expected number of Celery nodes.
        @param timeout Time to wait for replies.
        @return results from Celery.
        @throws RabbitMQException if RabbitMQ cannot be contacted.
        """
        try:
            return broadcast("birth", arguments={
                "transform": transform, 
                "configuration": config,
                "config_id": config_id}, 
                reply=True, timeout=timeout, limit=num_nodes)
        except socket.error as exc:
            raise RabbitMQException(exc)

    @staticmethod
    def birth_celery_nodes(transform, config, config_id):
        """
        Configure Celery nodes with the MAUS configuration and
        transform names. An initial Celery ping is done to identify
        the number of live nodes.
        @param transform Either a single name can be given - representing
        a single transform - or a list of transforms - representing a
        MapPyGroup. Sub-lists are treated as nested MapPyGroups. If None
        then the current transform is deathed and rebirthed.  
        @param config Valid JSON configuration document
        @param config_id Configuration ID from client.
        @throws RabbitMQException if RabbitMQ cannot be contacted.
        @throws CeleryWorkerException if one or more Celery 
        workers fails to configure or birth.
        """
        num_nodes = CeleryUtilities.ping_celery_nodes()
        results = CeleryUtilities.birth_celery(transform, config,
            config_id, num_nodes, 1000)
        # Validate that all nodes updated.
        failed_nodes = []
        for node in results:
            node_id = node.keys()[0]
            node_status = node[node_id]
            if node_status["status"] == "error":
                failed_nodes.append((node_id, node_status))
        if (len(failed_nodes) > 0):
            raise CeleryWorkerException(failed_nodes)

    @staticmethod
    def death_celery():
        """
        Call death on transforms in Celery nodes.
        @return results from Celery.
        @throws RabbitMQException if RabbitMQ cannot be contacted.
        """
        try:
            return broadcast("death", reply=True)
        except socket.error as exc:
            raise RabbitMQException(exc)
 
    @staticmethod
    def death_celery_nodes():
        """
        Call death on transforms in Celery nodes.
        @throws RabbitMQException if RabbitMQ cannot be contacted.
        @throws CeleryWorkerException if one or more Celery 
        workers fails to death.
        """
        results = CeleryUtilities.death_celery()        
        # Validate that all nodes updated.
        failed_nodes = []
        for node in results:
            node_id = node.keys()[0]
            node_status = node[node_id]
            if node_status["status"] == "error":
                failed_nodes.append((node_id, node_status))
        if (len(failed_nodes) > 0):
            raise CeleryWorkerException(failed_nodes)

class RabbitMQException(Exception):
    """ Exception raised if RabbitMQ cannot be contacted. """

    def __init__(self, exception):
        """
        Constructor. Overrides Exception.__init__.
        @param self Object reference.
        @param exception Wrapped exception
        """
        Exception.__init__(self)
        self.exception = exception

    def __str__(self, exception):
        """
        Return string representation. Overrides Exception.__str__.
        @param self Object reference.
        @return string.
        """
        return "RabbitMQ cannot be contacted. Problem is %s" \
            % self.exception

class NoCeleryWorkerException(Exception):
    """ Exception raised if no Celery workers are available. """

    def __str__(self):
        """
        Return string representation. Overrides Exception.__str__.
        @param self Object reference.
        @return string.
        """
        return "No Celery workers are available"

class CeleryWorkerException(Exception):
    """ 
    Exception raised if Celery workers fail to configure, birth
    or death.
    """

    def __init__(self, nodes = []):  # pylint:disable = W0102
        """
        Constructor. Overrides Exception.__init__.
        @param self Object reference.
        @param nodes List of tuples of form (node_id, node_status)
        with failure information.
        """
        Exception.__init__(self)
        self.nodes = nodes

    def __str__(self):
        """
        Return string representation. Overrides Exception.__str__.
        @param self Object reference.
        @return string.
        """
        node_ids = [node[0] for node in self.nodes]
        return "Celery worker(s) %s failed to configure" % node_ids
