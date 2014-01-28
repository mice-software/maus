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

# pylint does not import ROOT
# pylint: disable=E1101

"""
Tests for socket manager
"""
import Queue
import copy
import unittest
import signal
import sys
import subprocess
import time
import os
import threading
import atexit
import json

import ROOT
import xboa.Common as Common

from docstore.root_document_store import SocketManager
from docstore.root_document_store import SocketError
from docstore.root_document_store import ControlMessage

NEXT_PORT = 49050

sys.setcheckinterval(10)

class SocketManagerTest(unittest.TestCase):
    """
    Test the socket manager
    """
    def setUp(self):
        """
        Assign a port
        """
        global NEXT_PORT
        NEXT_PORT += 10
        self.port = NEXT_PORT
        self.retry = 0.1

    def test_server_socket_connect(self):
        """
        Test SocketManager.connect
        """

        ports = [self.port+i for i in range(1, 10)]
        msg_in = [ControlMessage() for i in range(10)]

        server = SocketManager(copy.deepcopy(ports), -1., self.retry, 100)
        client = SocketManager([], -1., self.retry, 100)
        client.connect("localhost", ports[0], 10., self.retry)
        server.start_processing()
        client.start_processing()
        try:
            for i in range(5):
                server.send_message(ports[0], msg_in[0])
            # one of them should get through
            msg_out = client.recv_message_queue.get(True, 5.)[1]
        except Queue.Empty:
            server.check_error_queue()
        self.assertEqual(msg_in[0].id, msg_out.id)

        # check max port is okay
        server = SocketManager([65535], -1., self.retry, 100)
        client = SocketManager([], -1., self.retry, 100)
        client.connect("localhost", 65535, 10., self.retry)
        try:
            server = SocketManager([65536], -1., self.retry, 100)
            self.assertTrue(False, "Should have raised")
        except SocketError:
            pass
        try:
            client.connect("localhost", 65536, 10., self.retry)
            self.assertTrue(False, "Should have raised")
        except SocketError:
            pass


    def test_server_socket_close_connection(self):
        """
        Test SocketManager.close_connection and SocketManager.close_all
        """
        server_socket = SocketManager(range(self.port, self.port+10), -1.,
                                      self.retry)
        try: # not connected
            server_socket.close_connection(self.port, False)
            self.assertTrue(False, msg="Should have thrown SocketError")
        except SocketError:
            pass
        client_socket = SocketManager([], -1., self.retry)
        client_socket.connect("localhost", self.port, 2., self.retry)
        client_socket.connect("localhost", self.port+1, 2., self.retry)
        client_socket.connect("localhost", self.port+2, 2., self.retry)
        time.sleep(1)
        self.assertEqual(len(server_socket.port_list()), 3)
        self.assertEqual(len(client_socket.port_list()), 3)
        server_socket.close_connection(self.port, True)
        self.assertEqual(len(server_socket.port_list()), 2)
        self.assertEqual(len(client_socket.port_list()), 3)
        server_socket.close_all(True)
        client_socket.close_all(True)
        self.assertEqual(len(server_socket.port_list()), 0)
        self.assertEqual(len(client_socket.port_list()), 0)
        time.sleep(0.1)
        # close non-existent port
        # close already-closed port

    def test_server_socket_send_message(self):
        """
        Test SocketManager.send_message
        """
        # send good message, over correct port (of many)
        # send good message with good data
        server = SocketManager(range(self.port, self.port+10), -1., self.retry, 10000)
        client = SocketManager([], -1., self.retry, 10000)
        client.connect("localhost", self.port, 2., self.retry)
        client.connect("localhost", self.port+1, 2., self.retry)
        client.connect("localhost", self.port+2, 2., self.retry)
        client_2 = SocketManager([], -1., self.retry, 10000)
        client_2.connect("localhost", self.port+3, 2., self.retry)
        sock_data = client.socket_list()[1]
        sock_data_2 = client_2.socket_list()[0]

        server.start_processing()
        client.start_processing()
        client_2.start_processing()
        message_in = ControlMessage()
        message_in.data = [{"test":"data"}, {"more":["data"]}, {"a":"a"*10000}]
        time.sleep(1)
        for i in range(10):
            client.send_message(sock_data["local_port"], message_in)
            client_2.send_message(sock_data_2["local_port"], message_in)
            time.sleep(0.1)

        sock_count = {
          sock_data["remote_port"]:0,
          sock_data_2["remote_port"]:0,
          "data":0,
          "id":0
        }
        for i in range(101):
            try:
                message_out = server.recv_message_queue.get(True, 0.1)
                sock_count[message_out[0]] += 1
                if message_out[1].id == message_in.id:
                    sock_count["id"] += 1
                if message_out[1].data == message_in.data:
                    sock_count["data"] += 1
                if i % 100 == 0:
                    print json.dumps(sock_count, indent=2)
                if sock_count["data"] == 20:
                    print json.dumps(sock_count, indent=2)
                    break # all the sheep are in the field
            except Queue.Empty:
                pass
        for key, value in sock_count.iteritems():
            self.assertTrue(value > 0)
        self.assertEqual(sock_count["data"], sock_count["id"])

        server.close_all(True)
        client.close_all(True)

    def __check_raises_socket_error(self, sock_man, message, port):
        try:
            sock_man.send_message(port, message)
            time.sleep(0.1)
            sock_man.check_error_queue()
            self.assertTrue(False, msg="Should have thrown")
        except SocketError:
            #sys.excepthook(*sys.exc_info())
            pass

    def test_server_socket_send_bad_message(self):
        """
        Test SocketManager.send_message with bad data
        """
        server = SocketManager(range(self.port, self.port+10), -1., 0.01)
        client = SocketManager([], -1., 0.01)
        client.connect("localhost", self.port, -1., 0.01)
        client.connect("localhost", self.port+1, -1., 0.01)
        client.connect("localhost", self.port+2, -1., 0.01)
        time.sleep(1)
        server.close_connection(self.port+2, True)

        # send when start_processing not called
        self.__check_raises_socket_error(server, ControlMessage(), self.port)
        server.start_processing()
        client.start_processing()
        # check we can send (should be okay)
        server.send_message(self.port, ControlMessage())
        server.send_message(self.port+1, ControlMessage())
        # send on closed port
        self.__check_raises_socket_error(server, ControlMessage(), self.port+2)
        # send on non-existent port
        self.__check_raises_socket_error(server, ControlMessage(), self.port-1)
        # send wrong type
        self.__check_raises_socket_error(server, "wrong type", self.port)
        ctrl_1 = ControlMessage()
        ctrl_1.data = "Neither json nor ROOT"
        # send message with wrong type in data
        self.__check_raises_socket_error(server, ctrl_1, self.port)
        ctrl_2 = ControlMessage()
        ctrl_2.class_name = ctrl_1
        # send message with non-serialisable type in json bit
        self.__check_raises_socket_error(server, ctrl_2, self.port)
        server.close_all(True)
        client.close_all(True)

if __name__ == "__main__":
    unittest.main()

