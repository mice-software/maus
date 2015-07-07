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
Server-side ROOT document store
"""

import time
import Queue
import threading
import sys
import bisect
import datetime
import dateutil.parser

from docstore.root_document_store._socket_manager import SocketManager

class RootDocumentDB:
    """
    The RootDocumentDB provides server-side interface to a networkable document
    database capable of storing both Json and ROOT documents.

    Here we
    * poll the message queue on the SocketManager
    * parse ControlMessage instances into function calls
    * make the actual function calls
    * put return data back onto the ControlMessage
    * pass the ControlMessage back to the socket.
    Note that the returning ControlMessage is a copy of the incoming message,
    with return data added. This ensures that the message id is the same 
    (required for client to know that returning data belongs to a given function
    call).

    The ControlMessage is expected to have RootDocumentDB as class name 
    parameter and the appropriate function name as a function name parameter. If
    a function call generates an exception, this is packed onto the 
    ControlMessage error dict and handed back to the socket.

    The actual database is handled naively in memory like:
        self.collections = {
            collection_name:{
                "time_sorted":<time sorted list of documents>
                "docid_sorted":<doc id sorted list of documents>
                "length":<maximum length of collection>
            }
        }
    We store two lists, one with documents that are sorted by time, the other
    with documents that are sorted by doc id. Both contain the same data, but in
    different order to optimise lookup times. Because everything is a smart
    pointer in python, it is okay in terms of memory footprint. 

    Doc ID is a user-defined identifier. Any primitive type is okay (string, 
    int, bool, float) but behaviour when a mixture of types are used is
    undefined.

    At the moment I don't have a separate collection class, but if things get 
    any more complicated this would really be necessary; for example, if I need
    to start cacheing on disk or whatever.
    """
    def __init__(self, port_list, poll_time):
        """
        Initialise the database.
        @param port RootDocumentDB port
        @param poll_time poll for new messages with this periodicity

        The database will stay open waiting for new connections until this class
        is deleted. Polling and message handling is done in a separate thread so
        that the DB can be run inline if required (if it is run in a separate
        daemin
        """
        self.poll_time = poll_time
        self.collections = {}
        self.server_socket = SocketManager(port_list, -1., self.poll_time, 10)
        self.server_socket.start_processing()
        my_thread = threading.Thread(target=self.poll_queue)
        my_thread.daemon = True
        my_thread.start()
        
    def poll_queue(self):
        """
        Poll the message queue on the socket and handle any incoming messages.
        Hand them to the appropriate function (defined by message.function) and
        handle return message or exceptions that are thrown.

        Send the return message.
        """
        while True:
            try:
                (port, message_in) = self.server_socket.recv_message_queue.get(
                                                                          False)
                if message_in.class_name == self.__class__.__name__:
                    try: # call the appropriate function
                        message_in.acknowledge = True
                        func_call = getattr(self, message_in.function)
                        message_out = func_call(message_in)
                    except Exception: # pylint: disable=W0703
                        message_out = message_in
                        message_out.data = []
                        exc_name = str(sys.exc_info()[0])
                        exc_msg = str(sys.exc_info()[1])
                        message_out.errors[exc_name] = exc_msg
                    finally: # return to user
                        self.server_socket.send_message(port, message_out)
                else: # not for us, put it back on the queue
                    self.server_socket.recv_message_queue.put(
                                                      (port, message_in), False)
            except Queue.Empty: # no messages, keep polling
                pass
            try:
                self.server_socket.check_error_queue()
            except Exception: # pylint: disable=W0703
                pass
            time.sleep(self.poll_time)

    def create_collection(self, message):
        """
        Create a new collection. 
        @param message with message.args = (
                <collection_name string>,
                <collection maximum length>)
        @return copy of message
        """
        if len(message.args) != 2:
            raise KeyError("Bad argument to create_collection")
        collection_name = message.args[0]
        length = message.args[1]
        if type(collection_name) != type("") and \
           type(collection_name) != type(u""):
            raise TypeError("collection name should be a string, got "+\
                            str(type(collection_name)))
        if collection_name in self.collections:
            raise KeyError(collection_name+" is already a collection")
        self.collections[collection_name] = {"time_sorted":[],
                                             "docid_sorted":[],
                                             "length":length}
        return message

    def has_collection(self, message):
        """
        Return true if collection exists. 
        @param message with message.args = (
                <collection_name string>)
        @return copy of message with message.return_value True if collection
                exists
        """
        if len(message.args) != 1:
            raise KeyError("Bad argument to has_collection")
        collection_name = message.args[0]
        message.return_value = collection_name in self.collections
        return message

    def get_ids(self, message):
        """
        Return ids of documents in the collection
        @param message with message.args = (
                <collection_name string>)
        @return message with message.return_value holding list of docids.
        """
        if len(message.args) != 1:
            raise KeyError("Bad argument to get_ids")
        collection_name = message.args[0]
        if collection_name not in self.collections:
            raise KeyError(str(collection_name)+" is not a known collection")
        docid_sorted = self.collections[collection_name]["docid_sorted"]
        message.return_value  = [doc.doc_id for doc in docid_sorted]
        return message

    def count(self, message):
        """
        Return number of documents in the collection
        @param message with message.args = (
                <collection_name string>)
        @return copy of message with message.return_value holding number of
              documents
        """
        if len(message.args) != 1:
            raise KeyError("Bad argument to count")
        collection_name = message.args[0]
        if collection_name not in self.collections:
            raise KeyError(str(collection_name)+" is not a known collection")
        message.return_value = \
                           len(self.collections[collection_name]["time_sorted"])
        return message

    def get(self, message):
        """
        Return a message from the collection. 
        @param message with message.args = (
                <collection_name string>,
                <doc id>)
        @return copy of message with message.data holding the returned document 
                or and empty array if no document was found
        This does not remove the document from the collection.
        """
        if len(message.args) != 2:
            raise KeyError("Bad argument to put")
        collection_name = message.args[0]
        doc_id = message.args[1]
        if collection_name not in self.collections:
            raise KeyError(str(collection_name)+" is not a known collection")
        my_doc = self._get(collection_name, doc_id)
        if my_doc == None:
            message.data = []
        else:
            message.data = [my_doc.data]
        message.return_value = None
        return message

    def put(self, message):
        """
        Put a message in the collection.
        @param message with message.args = (
                <collection_name string>,
                <docid>)
               and message.data = the document to load
        @return copy of message
        """
        if len(message.args) != 2:
            raise KeyError("Bad argument to put")
        if len(message.data) != 1:
            raise KeyError("Can only put one data item at a time, found "+\
                           str(len(message.data)))
        collection_name = message.args[0]
        doc_id = message.args[1]
        
        if message.args[0] in self.collections:
            docid_sorted = self.collections[collection_name]["docid_sorted"]
            time_sorted = self.collections[collection_name]["time_sorted"]
            length = self.collections[collection_name]["length"]
            doc = DocWrapper(message.data[0], doc_id)
            # overwrite
            self._remove(collection_name, doc_id)
            time_sorted.append(doc)
            if len(time_sorted) > length and length > 0:
                remove_doc = time_sorted[0]
                time_sorted.remove(remove_doc)
                docid_sorted.remove(remove_doc)
            DocWrapper.sort_by_id = True
            bisect.insort(docid_sorted, doc)
            message.return_value = None
            return message
        raise KeyError(str(collection_name)+" is not a known collection")

    def get_since(self, message):
        """
        Get a list of documents uploaded since a given time
        @param message with message.args = (
                <collection_name string>,
                <ISO string time stamp>)
        @return copy of message with message.data holding an array of documents.

        The client side interface requires a list of dicts to return like
        [{'_id':id, 'date':date, 'doc':doc}]. We handle that on server side by
        making a double length array like [
            {'_id':id1, 'date':date_for_id1},
            data_for_id1,
            {'_id':id2, 'date':date_for_id2},
            data_for_id2,
            ...
        ]
        This way we can stay blind to the data format and the metadata always
        serialises as a json object (much easier to handle than ROOT objects).

        This does not remove the documents from the collection.
        """
        if len(message.args) != 2:
            raise KeyError("Bad argument to put")
        collection_name = message.args[0]
        time_stamp = dateutil.parser.parse(message.args[1], yearfirst=True)
        if collection_name not in self.collections:
            raise KeyError(str(collection_name)+" is not a known collection")
        time_sorted = self.collections[collection_name]["time_sorted"]
        message.return_value = None
        message.data = []
        test_doc = DocWrapper(None, 0)
        test_doc.timestamp = time_stamp
        DocWrapper.sort_by_id = False
        index = bisect.bisect_left(time_sorted, test_doc)
        if index < len(time_sorted):
            my_doc_list = time_sorted[index:]
            for my_doc in my_doc_list:
                if my_doc.timestamp == time_stamp:
                    continue 
                message.data.append({"_id":my_doc.doc_id,
                                     "date":my_doc.timestamp.isoformat()})
                message.data.append(my_doc.data)
        return message

    def delete_document(self, message):
        """
        Remove a message from the collection.
        @param message with message.args = (
                <collection_name string>,
                <docid>)
        @return copy of message
        """
        if len(message.args) != 2:
            raise KeyError("Bad argument to put")
        collection_name = message.args[0]
        if collection_name not in self.collections:
            raise KeyError(str(collection_name)+" is not a known collection")
        self._remove(message.args[0], message.args[1])
        message.return_value = None
        return message

    def collection_names(self, message):
        """
        Return a list of names of collections.
        @param message any message - this is just providing a common interface
        @return copy of message
        """
        if len(message.args) != 0:
            raise KeyError("Bad argument to collection_name")
        message.return_value = self.collections.keys()
        return message

    def delete_collection(self, message):
        """
        Delete a collection.
        @param message with message.args = (
                <collection_name string>)
        @return copy of message
        """
        if len(message.args) != 1:
            raise KeyError("Bad argument to delete_collection")
        collection_name = message.args[0]
        if collection_name not in self.collections:
            raise KeyError(str(collection_name)+" is not a known collection")
        del self.collections[collection_name]
        message.return_value = None
        return message

    def _get(self, collection_name, doc_id):
        """
        Get a document with ID doc_id from the collection
        """
        docid_sorted = self.collections[collection_name]["docid_sorted"]
        DocWrapper.sort_by_id = True
        index = bisect.bisect_left(docid_sorted, DocWrapper(None, doc_id))
        if index != len(docid_sorted):
            my_doc = docid_sorted[index]
            if my_doc.doc_id == doc_id:
                return my_doc
        return None

    def _remove(self, collection_name, doc_id):
        """
        Remove a document with ID doc_id from the collection
        """
        my_doc = self._get(collection_name, doc_id)
        if my_doc == None:
            return
        time_sorted = self.collections[collection_name]["time_sorted"]
        time_sorted.remove(my_doc)
        docid_sorted = self.collections[collection_name]["docid_sorted"]
        docid_sorted.remove(my_doc)        

class DocWrapper(): # pylint: disable=R0903
    """
    Small wrapper class to associate a timestamp and document id with a
    document
    """
    def __init__(self, data, doc_id):
        """
        Initialise the wrapper, setting the timestamp
        """
        self.data = data
        self.timestamp = datetime.datetime.now()
        self.doc_id = doc_id
        DocWrapper.sort_by_id = True

    def __lt__(self, other):
        """
        Used for sorting. Return self.doc_id < other.doc_id if sort_by_id is
        True; else return self.timestamp < other.timestamp
        """
        if DocWrapper.sort_by_id:
            return self.doc_id < other.doc_id
        else:
            return self.timestamp < other.timestamp

    sort_by_id = True
