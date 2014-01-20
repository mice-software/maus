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

SocketManager scans for TSockets broadcasting over a range of ports and opens
communication with any discovered TSockets. SocketManager will poll the TSockets
with a user-defined frequency and append any messages to the in-memory queue.
"""

import sys
import time
import threading
import Queue

import ROOT
from docstore.root_document_store import SocketError
from docstore.root_document_store import ControlMessage

class SocketManager:
    def __init__(self, listen_port, timeout, retry_time):
        self.retry_time = retry_time
        self.socket_queue = Queue.Queue() # Fifo queue makes iterating easy
        self.port_list = []
        self.message_queue = Queue.Queue() # Fifo queue makes iterating easy
        self._listen_dict = {}
        if listen_port != None:
            self.accept_connection(listen_port, timeout, retry_time)
        self._listen_for_messages()

    def close_all(self, timeout):
        start = time.time()
        while len(self.port_list) > 0 and \
              (timeout < 0. or time.time()-start < timeout):
            self.close_connection(self.port_list[0], False)
        if len(self.port_list) > 0:
            raise SocketError("Failed to close all sockets - left "+\
                              str(self.port_list))

    def send_message(self, port, message):
        if type(message) == type(ControlMessage()):
            root_message = message.root_repr()
        else:
            raise SocketError("Message must be a ControlMessage - got "+\
                              str(type(message)))
        my_message = ROOT.TMessage(ROOT.TMessage.kMESS_OBJECT)
        my_message.WriteObject(root_message)
        try:
            socket = self.get_socket(port)
            socket.Send(my_message)
            self.put_socket(socket)
        except:
            raise SocketError("SocketManager knows nothing about sockets on "+\
                              "port "+str(port))


    def connect(self, url, port, timeout, retry_time):
        return self._request_socket(url, port, timeout, retry_time)

    def close_connection(self, port, force):
        if port not in self.port_list:
            raise SocketError("No socket found on port "+str(port))
        self._close_connection(port, force)

    def accept_connection(self, listen_port, timeout, retry_time):
        my_thread = threading.Thread(target=self._accept_socket,
                                     args=(listen_port, timeout, retry_time))
        my_thread.daemon = True
        my_thread.start()

    def _listen_for_messages(self):
        """
        Start a new thread listening for messages on each alive port

        If there is already an existing thread, then don't start a new one
        """
        for port in self.port_list:
            if port in self._listen_dict and self._listen_dict[port].is_alive():
                continue
            self._listen_dict[port] = threading.Thread(
                                          target=self._listen_for_messages_port,
                                          args=(port,
                                                self.retry_time))
            self._listen_dict[port].daemon = True
            self._listen_dict[port].start()

    def _listen_for_messages_port(self, port, retry_time):
        while True:
            time.sleep(retry_time)
            message = ROOT.TMessage()
            try:
                socket = self.get_socket(port)
                if not socket.IsValid():
                    self.put_socket(socket)
                    break
                socket.Recv(message)
                self.put_socket(socket)
            except KeyError:
                pass # someone else was using the socket
            if message == None or message.GetClass() == None:
                # waiting for message
                continue
            elif message.GetClass() == ROOT.TTree().Class():
                # control message
                try:
                    ctrl_message = ControlMessage.new_from_root_repr(message)
                    self.message_queue.put((socket.GetLocalPort(), ctrl_message), False)
                except TypeError:
                    raise SocketError("Malformed control message")
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
        if port not in self.port_list:
            raise SocketError(
                  "SocketManager knows nothing about sockets on port"+str(port))
        socket = self.get_socket(port)
        self.port_list.remove(port)
        if force:
            socket.Close("force")
        else:
            socket.Close("")

    def _request_socket(self, url, port, timeout, retry_time):
        start_time = time.time()
        valid = False
        while not valid and \
              (timeout < 0 or time.time() - start_time < timeout) and \
              port not in self.port_list:
            tmp_socket = ROOT.TSocket(url, port)
            tmp_socket.SetOption(ROOT.TSocket.kNoBlock, 1)
            valid = tmp_socket.IsValid()
            if not valid:
                time.sleep(retry_time)
        if valid:
            print "Request socket - local", tmp_socket.GetLocalInetAddress().GetHostName(), tmp_socket.GetLocalInetAddress().GetPort()
            print "Request socket - remote", tmp_socket.GetInetAddress().GetHostName(), tmp_socket.GetInetAddress().GetPort()
            port = tmp_socket.GetLocalPort()
            self.put_socket(tmp_socket)
            print "Listening"
            self._listen_for_messages()
            return port
        else:
            raise SocketError("Failed to connect to "+str(url)+":"+str(port))

    def _accept_socket(self, port, timeout, retry_time):
        server_socket = ROOT.TServerSocket(port, True)
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
        socket.SetOption(ROOT.TSocket.kNoBlock, 1)
        socket.fAddress = ROOT.gSystem.GetPeerName(accepted_socket_index)
        socket.fSecContext = 0
        if socket.GetDescriptor() >= 0:
          ROOT.gROOT.GetListOfSockets().Add(socket)
        print "Accept socket - local", socket.GetLocalInetAddress().GetHostName(), socket.GetLocalInetAddress().GetPort()
        print "Accept socket - remote", socket.GetInetAddress().GetHostName(), socket.GetInetAddress().GetPort()

        self.put_socket(socket)
        print "Listen"
        self._listen_for_messages()
        print "Done"

    def get_socket(self, port):
        """
        Get a socket from the socket queue; caller now owns the socket and must
        put it back when finished with it

        - port: integer corresponding to the socket LocalPort

        Throws a KeyError if the socket is not in the queue. Note that if 
        another thread is using a socket, it may not be in the queue - even if
        it has been opened by the socket manager
        """
        # Never access items in sock_list - this breaks the threading
        sock_list = []
        while True:
            try: 
                socket = self.socket_queue.get_nowait()
            except Queue.Empty: 
                raise KeyError("No sockets in socket_queue")
            sock_list.append(socket)
            if socket.GetLocalPort() == port:
                return socket
            else:
                # finished - we have looped once and not found the socket
                finished = socket in sock_list
                self.socket_queue.put_nowait(socket)
                if finished:
                    raise KeyError("Could not find socket in socket queue")
                        
    def put_socket(self, socket):
        if socket.GetLocalPort() not in self.port_list:
            self.port_list.append(socket.GetLocalPort())
        self.socket_queue.put_nowait(socket)

