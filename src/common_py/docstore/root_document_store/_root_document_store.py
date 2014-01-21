#!/usr/bin/env python

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
Client-side access to the ROOT document store
"""
import inspect
import sys
import time
import Queue

import ROOT

from docstore.DocumentStore import DocumentStoreException
from docstore.DocumentStore import DocumentStore
from _socket_manager import SocketManager
from _control_message import ControlMessage

class RootDocumentStore(DocumentStore):

    def __init__(self, timeout, poll_time):
        self.port = None
        self.timeout = timeout
        self.poll_time = poll_time
        self._socket_manager = SocketManager(None, self.timeout, poll_time)

    def connect(self, parameters):
        host = parameters["host"]
        port = parameters["port"]
        self.port = self._socket_manager.connect(host, port, self.timeout, self.poll_time)

    def _return_function(self, port, function_name, message_id):
        start = time.time()
        while time.time() - start < self.timeout:
            # message_queue is Fifo, so get; do_something; put; should just
            # loop over the queue waiting for our acknowledgement
            try: 
                (port, ack) = self._socket_manager.message_queue.get_nowait()
                if ack.acknowledge and message_id == ack.id:
                    if ack.errors == {}:
                        return ack
                    else:
                        error_string = ""
                        for key, value in ack.errors.iteritems():
                            error_string += str(key)+": "+str(value)+"\n"
                        raise DocumentStoreException(
                            "RootDocumentQueue failed with errors:\n"+error_string)
                else:
                    self._socket_manager.message_queue.put_nowait((port, ack))
            except Queue.Empty:
                pass #it's okay, just keep waiting
            time.sleep(self.poll_time)
        raise DocumentStoreException("Call to "+function_name+" failed to return")

    def _parse_function(self, args, keywd_args, data = None):
        if self.port == None:
            raise DocumentStoreException("DocummentStore has not been connected")
        if data == None:
            data = ROOT.TTree()
        message = ControlMessage("DocumentQueue", inspect.stack()[1][3],
                                 args, keywd_args, data)
        self._socket_manager.send_message(self.port, message)
        return self._return_function(self.port, inspect.stack()[1][3],
                                     message.id)

    def create_collection(self, collection, maximum_size):
        msg = self._parse_function((collection, maximum_size), {})
        return msg.return_value

    def has_collection(self, collection):
        msg = self._parse_function((collection,), {})
        return msg.return_value

    def get_ids(self, collection):
        msg = self._parse_function((collection,), {})
        return msg.return_value

    def count(self, collection):
        msg = self._parse_function((collection,), {})
        return msg.return_value

    def put(self, collection, docid, root_data):
        msg = self._parse_function((collection, docid), {}, root_data)
        return msg.return_value

    def get(self, collection, docid):
        msg = self._parse_function((collection, docid), {})
        return msg.data_tree

    def get_since(self, collection, earliest):
        msg = self._parse_function((collection, earliest), {})
        return msg.data_tree

    def delete_document(self, collection, docid):
        msg = self._parse_function((collection, docid), {})
        return msg.return_value

    def disconnect(self):
        self._socket_manager.close_all(self.timeout)

