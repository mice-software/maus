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

import ROOT
import xboa.Common as Common

from docstore.root_document_store import SocketManager
from docstore.root_document_store import SocketError
from docstore.root_document_store import ControlMessage

NEXT_PORT = 49050

class SocketManagerTest(unittest.TestCase):
    """
    Test the socket manager
    """
    def setUp(self):
        """
        Assign a port
        """
        global NEXT_PORT
        NEXT_PORT += 1
        self.port = NEXT_PORT

    def test_server_socket_connect(self):
        """
        Test SocketManager.connect
        """
        msg = [ControlMessage() for i in range(3)]
        ports = [self.port+i for i in range(1, 4)]
        server_socket = SocketManager(self.port, copy.deepcopy(ports), -1., 0.01)
        socks = [SocketManager(None, [], -1., 0.01) for i in range(3)]
        for i, sock_manager in enumerate(socks):
            sock_manager.connect("localhost", self.port, 10, 0.01)
            time.sleep(1)
        print msg
        print ports
        print socks
        for i, sock_manager in enumerate(socks):
            sock_manager.send_message(ports[i], msg[i])
        time.sleep(1)
        msg_out = []
        while True:
              try:
                  msg_out.append(server_socket.message_queue.get_nowait())
              except Queue.Empty:
                  break
        print msg
        print msg_out

    def _test_server_socket_close_connection(self):
        """
        Test SocketManager.close_connection and SocketManager.close_all
        """
        server_socket = SocketManager(49079, -1., 0.01)
        try: # not connected
            server_socket.close_connection(49079, False)
            self.assertTrue(False, msg="Should have thrown SocketError")
        except SocketError:
            pass
        try: # not connected
            server_socket.close_connection(49079, True)
            self.assertTrue(False, msg="Should have thrown SocketError")
        except SocketError:
            pass
        client_socket = SocketManager(None, -1., 0.01)
        client_socket.connect("localhost", 49079, 2., 0.01)
        time.sleep(1)
        port = client_socket.port_list()[-1]
        socket = client_socket.get_socket(port)
        client_socket.put_socket(socket)
        time.sleep(1)
        server_socket.close_connection(49079, True)
        time.sleep(1)
        self.assertEqual(len(server_socket.port_list()), 0)
        server_socket.close_all(True)
        time.sleep(1)
        client_socket.close_all(True)

    def _test_server_socket_send_message(self):
        """
        Test SocketManager.send_message
        """
        server_socket = SocketManager(self.port, -1., 0.01)
        client_socket_1 = SocketManager(None, -1., 0.01)
        client_socket_1.connect("localhost", self.port, 2., 0.01)
        client_socket_2 = SocketManager(None, -1., 0.01)
        client_socket_2.connect("localhost", self.port, 2., 0.01)
        print "LSOF ", self.port
        time.sleep(1)
        self.assertEqual(len(server_socket.port_list()), 1)
        self.assertEqual(len(client_socket_1.port_list()), 1)
        self.assertEqual(len(client_socket_2.port_list()), 1)
        server_port = server_socket.port_list()[0]
        client_port_1 = client_socket_1.port_list()[0]
        client_port_2 = client_socket_2.port_list()[0]
        server_ctrl_msg = ControlMessage()
        client_ctrl_msg_1 = ControlMessage()
        client_ctrl_msg_2 = ControlMessage()
        server_socket.send_message(server_port, server_ctrl_msg)
        client_socket_1.send_message(client_port_1, client_ctrl_msg)
        client_socket_2.send_message(client_port_2, client_ctrl_msg)
        time.sleep(1)
        server_msg_1 = server_socket.message_queue.get_nowait()[1]
        server_msg_2 = server_socket.message_queue.get_nowait()[1]
        ids = [server_msg_1.id, server_msg_2.id]
        self.assertTrue(client_ctrl_msg_1.id in ids)
        self.assertTrue(client_ctrl_msg_2.id in ids)
        try:
            client_msg = client_socket_1.message_queue.get_nowait()[1]
        except Queue.Empty:
            client_msg = client_socket_2.message_queue.get_nowait()[1]
        self.assertEqual(server_ctrl_msg.id, client_msg.id)
        server_socket.close_all(True)
        client_socket_1.close_all(True)
        client_socket_2.close_all(True)

    def _test_server_socket_send_bad_message(self):
        """
        Test SocketManager.send_message with bad data
        """
        server_socket = SocketManager(49084, -1., 0.01)
        client_socket = SocketManager(None, -1., 0.01)
        client_socket.connect("localhost", 49084, 2., 0.01)
        time.sleep(1)
        try: # wrong type
            client_socket.send_message(49084, "wrong type")
            self.assertTrue(False, msg="Should have thrown")
        except SocketError:
            pass
        try: # wrong socket
            client_socket.send_message(49085, ControlMessage())
            self.assertTrue(False, msg="Should have thrown")
        except SocketError:
            pass
        server_socket.close_all(True)
        client_socket.close_all(True)

    def _test_get_put_socket(self):
        """
        Test SocketManager.get_socket and SocketManager.put_socket
        """
        server_socket = SocketManager(self.port, -1., 0.01)
        client_socket = SocketManager(None, -1., 0.01)
        client_socket.connect("localhost", self.port, 2., 0.01)
        time.sleep(1)
        socket = server_socket.get_socket(self.port)
        self.assertEqual(socket.GetLocalPort(), self.port)
        self.assertEqual(server_socket.port_list(), [])
        server_socket.put_socket(socket)
        self.assertEqual(server_socket.port_list(), [self.port])
        server_socket.put_socket(socket)

if __name__ == "__main__":
    unittest.main()

