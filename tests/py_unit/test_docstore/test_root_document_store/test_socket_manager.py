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
Tests for socket handler
"""
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

NEXT_SOCK = 49050

class SocketManagerTest(unittest.TestCase):

    def test_server_socket_connect(self):
        server_socket = SocketManager(49080, -1., 1.)
        client_socket = SocketManager(49081, -1., 1.)
        client_socket.connect("localhost", 49080, 2., 1.)
        time.sleep(1)
        self.assertEqual(sorted(server_socket.port_list), [49080])
        self.assertEqual(len(client_socket.port_list), 1)
        server_socket.close_all(5)
        client_socket.close_all(5)
        self.assertEqual(sorted(server_socket.port_list), [])
        self.assertEqual(sorted(client_socket.port_list), [])

    def test_server_socket_close_connection(self):
        server_socket = SocketManager(49079, -1., 1.)
        try:
            server_socket.close_connection(49079, False)
            self.assertTrue(False, msg="Should have thrown SocketError")
        except SocketError:
            pass
        try:
            server_socket.close_connection(49079, True)
            self.assertTrue(False, msg="Should have thrown SocketError")
        except SocketError:
            pass
        client_socket = SocketManager(49078, -1., 1.)
        client_socket.connect("localhost", 49079, 2., 1.)
        time.sleep(1)
        server_socket.close_connection(49079, True)
        time.sleep(1)
        self.assertEqual(len(server_socket.port_list), 0)
        server_socket.close_all(5)
        client_socket.close_all(5)

    def test_server_socket_send_message(self):
        server_socket = SocketManager(49086, -1., 1.)
        client_socket = SocketManager(49087, -1., 1.)
        client_socket.connect("localhost", 49086, 2., 1.)
        time.sleep(1)
        self.assertEqual(len(server_socket.port_list), 1)
        self.assertEqual(len(client_socket.port_list), 1)
        server_port = server_socket.port_list[0]
        client_port = client_socket.port_list[0]
        server_ctrl_msg = ControlMessage.new_text_message("server to client", 1)
        client_ctrl_msg = ControlMessage.new_text_message("client to server", 1)
        server_socket.send_message(server_port, server_ctrl_msg)
        client_socket.send_message(client_port, client_ctrl_msg)
        server_msg = server_socket.message_queue.get()[1]
        self.assertEqual(client_ctrl_msg, server_msg)
        client_msg = client_socket.message_queue.get()[1]
        self.assertEqual(server_ctrl_msg, client_msg)
        server_socket.close_all(5)
        client_socket.close_all(5)

    def test_server_socket_send_bad_message(self):
        server_socket = SocketManager(49084, -1., 1.)
        client_socket = SocketManager(49085, -1., 1.)
        client_socket.connect("localhost", 49084, 2., 1.)
        time.sleep(1)
        try:
            client_socket.send_message(49084, "wrong type")
            self.assertTrue(False, msg="Should have thrown")
        except SocketError:
            pass
        try:
            client_socket.send_message(49085, ControlMessage({}))
            self.assertTrue(False, msg="Should have thrown")
        except SocketError:
            pass
        server_socket.close_all(5)
        client_socket.close_all(5)

if __name__ == "__main__":
    unittest.main()

