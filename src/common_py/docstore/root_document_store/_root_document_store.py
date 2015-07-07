#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

"""
Client side RootDocumentStore
"""

import inspect
import time
import datetime
import Queue
import dateutil.parser

from docstore.DocumentStore import DocumentStoreException
from docstore.DocumentStore import DocumentStore
from docstore.root_document_store._socket_manager import SocketManager
from docstore.root_document_store._socket_error import SocketError
from docstore.root_document_store._control_message import ControlMessage

class RootDocumentStore(DocumentStore):
    """
    The RootDocumentStore provides client-side interface to a networkable
    document database capable of storing both Json and ROOT documents.

    Communication with the database goes like
    * Client makes a function call to the database (interface via 
      ControlMessage). This should include any data required (e.g. for a put
      call)
    * Server acts on the functon call and returns an acknowledgement message,
      together with any return data. If no acknowledgement is received within a
      timeout RootDocumentStore will raise an exception.
    The messaging protocol (see elsewhere) explicitly separates the function
    return value from any returning data.

    The database has a maximum size parameter which controls the maximum number
    of elements that can be stored, and a poll_time which controls the frequency
    with which the database will be polled.

    RootDocumentDB has the concept of different collections (analogous to
    tables) but we only have one database per instantiation of RootDocumentDB.

    The real code is defined in RootDocumentDB; here we only parse arguments 
    into messages, send them over the socket and hand response back to the user
    (handled by _parse_function and _return function respectively).
    """

    def __init__(self, timeout, poll_time, n_socket_retries=3,
                 n_docstore_retries=3):
        """
        Initialise the document store
        @param timeout: maximum time to attempt a given communication with the
               RootDocDB before raising an exception
        @param poll_time: check for messages with this periodicity
        @returns a RootDocumentStore
        """
        self.port = None
        self.timeout = timeout
        self.poll_time = poll_time
        self._socket_manager = SocketManager([], self.timeout, poll_time,
                                             n_socket_retries)
        self.n_retries = n_docstore_retries
        self.retry_time = 1

    def connect(self, parameters = None):
        """
        Connect to a RootDocumentDB
        @param parameters should be a dict with keys "host" pointing to a 
               hostname and "port" giving the port on the host which should be
               connected
        @return None
        """
        if parameters == None:
            raise DocumentStoreException("connect needs parameters")
        host = parameters["host"]
        port = parameters["port"]
        self.port = self._socket_manager.connect(host,
                                                 port,
                                                 self.timeout,
                                                 self.poll_time)
        self._socket_manager.start_processing()

    def create_collection(self, collection, maximum_size = 1000):
        """
        Create a new collection of documents
        @param collection string that names the collection
        @param maximum_size (integer) maximum number of documents in the db. If 
               maximum_size <= 0, store any number of documents. Earliest
               documents will be removed on attempt to put more documents than
               maximum_size.
        """
        msg = self._parse_function((collection, maximum_size), {})
        return msg.return_value

    def has_collection(self, collection):
        """
        Return true if collection is a collection in the DB, else false
        @param collection string that names the collection
        """
        msg = self._parse_function((collection,), {})
        return msg.return_value

    def get_ids(self, collection):
        """
        Return a list of document ids that can be found in the collection
        @param collection string that names the collection
        """
        msg = self._parse_function((collection,), {})
        return msg.return_value

    def count(self, collection):
        """
        Return the number of documents in the collection
        @param collection string that names the collection
        """
        msg = self._parse_function((collection,), {})
        return msg.return_value

    def put(self, collection, docid, document):
        """
        Put a new document in the collection
        @param collection string that names the collection
        @param docid unique id for the document; this can be any of the 
               following types; string, integer, bool, float; but all ids in the
               DB must be of the same type (and this is not checked)
        @param document either a ROOT.TTree() or a json document (in python
               format, i.e. either a dict or list)
        Overwrites existing documents if docid is duplicated; removes oldest
        document if maximum_size would otherwise be exceeded.
        """
        msg = self._parse_function((collection, docid), {}, [document])
        return msg.return_value

    def get(self, collection, docid):
        """
        Get a document from the collection, leaving it in the collection.
        @param collection string that names the collection
        @param docid unique id for the document
        @returns document either a ROOT.TTree() or a json document (in python
                 format)
        """
        msg = self._parse_function((collection, docid), {})
        if len(msg.data) == 0:
            return None
        elif len(msg.data) == 1:
            return msg.data[0]
        else:
            raise DocumentStoreException(
                                      "Message data should only be of length 1")

    def get_since(self, collection, earliest = None):
        """
        Get a list of documents that were uploaded after earliest. Nb timestamps
        are timestamps on the server side that a document was uploaded. We have
        no handling for mismatched times or different timezones, caller has to 
        ensure that the handled timestamp is appropriate for the system clock on
        the server.
        @param collection string that names the collection
        @param earliest time after which documents should have been uploaded. If
               earliest is None, returns all documents in the collection.
               Earliest can be either a datetime.datetime object or an 
               isoformatted string (YYYY-MM-DDTHH:MM:SS:MMMM)
        @returns list of dicts, like {'_id':id, 'date':date, 'doc':doc} where
                date is a datetime object
        """
        if earliest == None:
            earliest = datetime.datetime.min
        if type(earliest) == type(datetime.datetime.min):
            earliest = earliest.isoformat()
        msg = self._parse_function((collection, earliest), {})
        data = []
        for i in range(len(msg.data)/2):
            data.append(msg.data[2*i])
            data[-1]["doc"] = msg.data[2*i+1]
            date = dateutil.parser.parse(data[-1]["date"], yearfirst=True)
            data[-1]["date"] = date
        return data

    def delete_document(self, collection, docid):
        """
        Remove a document from the docstore
        @param collection string that names the collection
        @param docid unique id for the document
        """
        msg = self._parse_function((collection, docid), {})
        return msg.return_value

    def collection_names(self):
        """
        Return a list of the string names of all collections in the docstore
        """
        msg = self._parse_function((), {})
        return msg.return_value

    def delete_collection(self, collection_name):
        """
        Remove a collection from the docstore
        """
        msg = self._parse_function((collection_name,), {})
        return msg.return_value

    def disconnect(self):
        """
        Disconnect from the docstore
        """
        self._socket_manager.close_all(self.timeout)

    def _parse_function(self, args, keywd_args, data = None):
        """
        Parse a function call into a ControlMessage and send over the wire
        @param args iterable containing function arguments
        @param keywd_args dict containing keyword argumenst
        @param data list containing any documents that need to be pushed across.
               If None, will push an empty list.
        @return wait for the call to be acknowledged and return the
              acknowledgement message.
        @raises DocumentStoreException if the function failed to return.
        @raises TypeError if the data cannot be parsed to an appropriate format.

        Function name of the calling function must match a corresponding
        function in the RootDocumentDatabase. This is discovered dynamically at
        runtime by looking at the call stack.
        """
        if self.port == None:
            raise DocumentStoreException("DocumentStore has not been connected")
        if data == None:
            data = []
        message = ControlMessage("RootDocumentDB", inspect.stack()[1][3],
                                 args, keywd_args, data)
        # we retry both at socket level (see socket_manager) and at docstore
        # level (below); attempting to increase robustness, but in the presence
        # of lots of network traffic this can increase the amount of congestion
        for i in range(self.n_retries):
            try:
                self._socket_manager.send_message(self.port, message)
                return self._return_function(inspect.stack()[1][3],
                                             message.uuid)
            except DocumentStoreException:
                if i+1 == self.n_retries:
                    raise
            except SocketError as err:
                if i+1 == self.n_retries:
                    raise DocumentStoreException(*err.args)
            time.sleep(self.retry_time)
        # should never reach this line of code
        raise DocumentStoreException("Internal error while contact docstore")

    def _return_function(self, function_name, message_id):
        """
        Wait for a message to be acknowledged and return the acknowledgement 
        message.
        @param function_name name of the function that is awaiting
               acknowledgement
        @param message_id identifier of the message that made the function call.
        @returns the acknowledgement ControlMessage
        @raises DocumentStoreException if the function failed to return

        The acknowledgement message must have the same id, to ensure that the 
        return value corresponds to the function call. Server side this is 
        handled by returning the calling message with return value and return 
        data appended
        """
        start = time.time()
        while time.time() - start < self.timeout:
            # message_queue is Fifo, so get; do_something; put; should just
            # loop over the queue waiting for our acknowledgement
            try: 
                (port, ack) = \
                            self._socket_manager.recv_message_queue.get_nowait()
                if ack.acknowledge and message_id == ack.uuid:
                    if ack.errors == {}:
                        return ack
                    else:
                        error_string = ""
                        for key, value in ack.errors.iteritems():
                            error_string += str(key)+": "+str(value)+"\n"
                        raise DocumentStoreException(
                            "RootDocumentDB failed with errors:\n"+error_string)
                else:
                    self._socket_manager.recv_message_queue.put_nowait((port,
                                                                        ack))
            except Queue.Empty:
                pass #it's okay, just keep waiting
            time.sleep(self.poll_time)
        raise DocumentStoreException("Call to "+function_name+\
                                     " failed to return")

def _initialise_from_c(timeout, poll_time):
    """
    Wrapper for __init__ to help with C initialisation
    """
    return RootDocumentStore(timeout, poll_time)

