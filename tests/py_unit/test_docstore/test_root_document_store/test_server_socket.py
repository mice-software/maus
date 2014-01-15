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
Tests for online_gui
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

from docstore.root_document_store import ServerSocket
from docstore.root_document_store import ClientSocket
from docstore.root_document_store import ControlMessage

class OnlineGUITest(unittest.TestCase):
    def setUp(self):
        self.server_list = []

    def test_server_socket_connect(self):
        print "connect"
        server_socket = ServerSocket(9080, [], -1., 1.)
        client_socket = ServerSocket(9081, [], -1., 1.)
        client_socket.connect("localhost", 9080, 2., 1.)
        time.sleep(1)
        self.assertEqual(sorted(server_socket.socket_dict.keys()), [9080])
        self.assertEqual(sorted(client_socket.socket_dict.keys()), [9080])
        server_socket.close_all()
        client_socket.close_all()
        print "Closing"
        self.assertEqual(sorted(server_socket.socket_dict.keys()), [])
        self.assertEqual(sorted(client_socket.socket_dict.keys()), [])
        time.sleep(1)

    def test_server_socket_reconnect(self):
        print "reconnect"
        server_socket = ServerSocket(9082, [], -1., 1.)
        client_socket = ServerSocket(9083, [], -1., 1.)
        client_socket.connect("localhost", 9082, 2., 1.)
        time.sleep(1)
        self.assertEqual(sorted(server_socket.socket_dict.keys()), [9082])
        self.assertEqual(sorted(client_socket.socket_dict.keys()), [9082])
        server_socket.close_all()
        client_socket.close_all()
        for i in range(18):
            print "sleep", i
        time.sleep(1)
        new_server_socket = ServerSocket(9082, [], -1., 1.)
        new_client_socket = ServerSocket(9083, [], -1., 1.)
        new_client_socket.connect("localhost", 9082, 2., 1.)
        self.assertEqual(sorted(new_server_socket.socket_dict.keys()), [9082])
        self.assertEqual(sorted(new_client_socket.socket_dict.keys()), [9082])
        new_server_socket.close_all()
        new_client_socket.close_all()


    def _test_server_socket_append_connection(self):
        print "append"
        server_socket = ServerSocket(9080, range(9081, 9090), -1., 1.)
        client_socket = ServerSocket(9070, range(9071, 9080), -1., 1.)
        client_socket.connect("localhost", 9080, 2., 1.)
        time.sleep(1)
        server_socket.close_all()
        client_socket.close_all()


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "sender":
        OnlineGUIClient("test", int(sys.argv[2]))
    else:
        unittest.main()

