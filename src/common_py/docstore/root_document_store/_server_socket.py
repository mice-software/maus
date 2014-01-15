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
server_socket wraps the ROOT TServerSocket

ServerSocket scans for TSockets broadcasting over a range of ports and opens
communication with any discovered TSockets. ServerSocket will poll the TSockets
with a user-defined frequency and append any messages to the in-memory queue.
"""

import sys
import time
import threading
import Queue

import ROOT
from _control_message import ControlMessage

class SocketError(Exception):
    """
    SocketError is thrown when there is a problem connecting with a socket
    """
    def __init__(self, *args, **kwargs):
        """Initialise Exception base class"""
        Exception.__init__(self, *args, **kwargs)

class ServerSocket:
    def __init__(self, listen_port, port_list, timeout, retry_time):
        self.lock = threading.Lock()
        self.timeout = timeout
        self.retry_time = retry_time
        self.socket_dict = {}
        self.listen_dict = {}
        self.data_queue = Queue.Queue()
        self.command_queue = Queue.Queue()
        self.port_list = port_list
        self.accept_connection(listen_port, timeout, retry_time)
        self._listen_for_messages()

    def close_all(self):
        port_list = self.socket_dict.keys()
        for port in port_list:
            self.close_connection(port, False)

    def send_message(self, port, message):
        if type(message) == type(ControlMessage({})):
            message = message.root_repr()
        elif type(message) != type(TTree()):
            raise TypeError("Message must be a TTree or a ControlMessage - "+\
                            "got "+str(type(message)))
        my_message = ROOT.TMessage(ROOT.TMessage.kMESS_OBJECT)
        my_message.WriteObject(message)
        self.socket_dict[port].Send(my_message)

    def connect(self, url, port, timeout, retry_time):
        self._request_socket(url, port, timeout, retry_time)

    def append_connection(self, url, port, timeout, retry_time):
        self.send_message(port, ControlMessage.new_open_message(port,
                                                                timeout,
                                                                retry_time))
        self._request_socket(url, port, timeout, retry_time)

    def close_connection(self, port, force):
        self.send_message(port, ControlMessage.new_close_message(port, force))
        self._close_connection(port, force)

    def accept_connection(self, listen_port, timeout, retry_time):
        my_thread = threading.Thread(target=self._accept_socket,
                                     args=(listen_port, timeout, retry_time))
        my_thread.daemon = True
        my_thread.start()

    def _listen_for_messages(self):
        port_list = self.socket_dict.keys()
        for port in port_list:
            if port in self.listen_dict and self.listen_dict[port].is_alive():
                continue
            self.listen_dict[port] = threading.Thread(
                                          target=self._listen_for_messages_port,
                                          args=(self.socket_dict[port],
                                                self.retry_time))
            self.listen_dict[port].daemon = True
            self.listen_dict[port].start()

    def _listen_for_messages_port(self, socket, retry_time):
        objstring_class = ROOT.TObjString().Class()
        message = ROOT.TMessage()
        while socket.IsValid():
            time.sleep(retry_time)
            socket.Recv(message)
            if message == None:
                # waiting for message
                continue
            elif message.GetClass() == objstring_class:
                # control message
                try:
                    ctrl_message = \
                               ControlMessage.new_from_root_message(message)
                    self.command_queue.put(ctrl_message, block=True, timeout=retry_time)
                except TypeError:
                    raise SocketError("Malformed control message")
            elif message.GetClass() == ROOT.TTree().Class():
                # data message
                my_object = message.ReadObject(message.GetClass())
                my_tree = ROOT.TTree(my_object)
                self.data_queue.put(my_tree, block=True, timeout=retry_time)
            elif message.What() != ROOT.EMessageTypes.kMESS_OBJECT:
                raise SocketError( # bad type
                          "Malformed message - should be kMESS_OBJECT type")

    def _parse_control_message(self, control_message):
        msg = control_message.message
        if control_message.is_open_message():
            self._accept_socket(msg["open"]["port"],
                                msg["open"]["timeout"],
                                msg["open"]["retry_time"])
        if control_message.is_close_message():
            self._close_connection(msg["close"]["port"],
                                   msg["close"]["force"])
        if control_message.is_text_message():
            print "LOG-"+str(msg["message"]["verbose_level"])+":", \
                  msg["message"]["text"]

    def _close_connection(self, port, force):
        if port not in self.socket_dict:
            raise SocketError(
                  "ServerSocket knows nothing about sockets on port"+str(port))
        socket = self.socket_dict[port]
        if force:
            socket.Close("force")
        else:
            socket.Close("")
        del self.socket_dict[port]

    def _request_socket(self, url, port, timeout, retry_time):
        start_time = time.time()
        valid = False
        while not valid and (timeout < 0 or time.time() - start_time < timeout):
            tmp_socket = ROOT.TSocket(url, port)
            tmp_socket.SetOption(ROOT.TSocket.kNoBlock, 1)
            valid = tmp_socket.IsValid()
            if not valid:
                time.sleep(retry_time)
        if valid:
            self.socket_dict[port] = tmp_socket
            self._listen_for_messages()
        else:
            raise SocketError("Failed to connect to "+str(url)+":"+str(port))

    def _accept_socket(self, port, timeout, retry_time):
        server_socket = ROOT.TServerSocket(port)
        server_socket.SetOption(ROOT.TServerSocket.kNoBlock, 1)
        tcp_socket_index = server_socket.GetDescriptor()
            
        start_time = time.time()
        accepted_socket_index = ROOT.gSystem.AcceptConnection(tcp_socket_index)
        while accepted_socket_index < 0 and \
              (timeout < 0 or time.time()-start_time < timeout):
                sys.stdout.flush()
                accepted_socket_index = ROOT.gSystem.AcceptConnection(tcp_socket_index)
                sys.stdout.flush()
                if accepted_socket_index < 0:
                    time.sleep(retry_time)
        sys.stdout.flush()
        if accepted_socket_index < 0:
            raise SocketError("Failed to accept connection on port "+\
                                  str(port))
        ROOT.gSystem.SetSockOpt(accepted_socket_index, ROOT.kReuseAddr, 1)
        socket = ROOT.TSocket(accepted_socket_index)
        socket.fAddress = ROOT.gSystem.GetPeerName(accepted_socket_index)
        socket.fSecContext = 0
        if socket.GetDescriptor() >= 0:
          ROOT.gROOT.GetListOfSockets().Add(socket)
        self.socket_dict[port] = socket
        self._listen_for_messages()

