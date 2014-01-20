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
Server-side ROOT document store
"""

import time
import Queue
import os
import threading
import json
import ROOT
import sys

from _control_message import ControlMessage
from _socket_manager import SocketManager

class DocumentQueue:
    def __init__(self, port):
        self.server_socket = SocketManager(port, -1., 0.1)
        my_thread = threading.Thread(target=self.poll_queue)
        my_thread.daemon = True
        my_thread.start()
        self.collections = {}
        
    def poll_queue(self):
        while True:
            try:
                (port, message_in) = self.server_socket.message_queue.get(False)
                if message_in.class_name == self.__class__.__name__:
                    try:
                        message_in.acknowledge = True
                        func_call = getattr(self, message_in.function)
                        message_out = func_call(message_in)
                    except Exception:
                        message_out = message_in
                        message_out.data_tree = ROOT.TTree()
                        exc_name = str(sys.exc_info()[0])
                        exc_msg = str(sys.exc_info()[1])
                        message_out.errors[exc_name] = exc_msg
                    finally:
                        self.server_socket.send_message(port, message_out)                
                else:
                    self.server_socket.message_queue.put(False, message)
            except Queue.Empty:
                pass
            time.sleep(0.1)

    def create_collection(self, message):
        if len(message.args) != 2:
            raise KeyError("Bad argument to create_collection")
        collection_name = message.args[0]
        if type(collection_name) != type("") and \
           type(collection_name) != type(u""):
            raise TypeError("collection name should be a string, got "+\
                            str(type(collection_name)))
        if collection_name in self.collections:
            raise KeyError(collection_name+" is already a collection")
        self.collections[collection_name] = []
        return message

    def has_collection(self, message):
        if len(message.args) != 1:
            raise KeyError("Bad argument to has_collection")
        collection_name = message.args[0]
        if type(collection_name) != type("") and \
           type(collection_name) != type(u""):
            raise TypeError("collection name should be a string, got "+\
                            str(type(collection_name)))
        message.return_value = True
        return message

    def get_ids(self, collection):
        raise NotImplementedError()

    def count(self, collection):
        raise NotImplementedError()

    def get(self, message):
        raise NotImplementedError()

    def put(self, message):
        if len(message.args) != 3:
            raise KeyError("Bad argument to put")
        collection_name = message.args[0]
        if type(collection_name) != type("") and \
           type(collection_name) != type(u""):
            raise TypeError("collection name should be a string, got "+\
                            str(type(collection_name)))
        #message.data_tree.

    def get_since(self, message):
        raise NotImplementedError()
        
    def delete_document(self, message):
        raise NotImplementedError()

class DocWrapper():
    def __init__(self, data, doc_id):
        self.data = None
        self.timestamp = time.time()
        self.doc_id = doc_id

