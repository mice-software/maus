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
SocketManager wrapper wraps the ROOT TServerSocket
"""

import inspect
import sys
import time
import threading
import Queue
import json

import ROOT
from _socket_error import SocketError
from _control_message import ControlMessage

class SocketManager:
    """
    SocketManager scans for TSockets broadcasting over a range of ports and 
    opens communication with any discovered TSockets. SocketManager will poll 
    the TSockets with a user-defined frequency and append any messages to the 
    in-memory queue.
    """
    def __init__(self, listen_port, listen_port_list, timeout, retry_time):
        self.retry_time = retry_time
        self.broadcast_port_list = None #broadcast_port_list
        self.socket_queue = Queue.Queue() # Fifo queue makes iterating easy
        self.message_queue = Queue.Queue() # Fifo queue makes iterating easy
        self.send_message_queue = Queue.Queue()
        for listen_port in listen_port_list:
            self.accept_connection(listen_port, timeout, retry_time)

    def start_processing(self):
        self._start_listening_for_messages()
        self._start_message_sender()

    def close_all(self, force):
        for socket in self._loop_over_sockets_unsafe():
            if force:
                socket.Close("force")
            else:
                socket.Close("")

    def send_message(self, port, message):
        if type(message) == type(ControlMessage()):
            root_message = message.root_repr()
        else:
            raise SocketError("Message must be a ControlMessage - got "+\
                              str(type(message)))
        try:
            print "QUEUEING", message.id
            self.send_message_queue.put_nowait((port, root_message))
            #socket = self.get_socket(port)
            #socket.Send(root_message)
            #self.put_socket(socket)
        except SocketError:
            raise SocketError("SocketManager knows nothing about sockets on "+\
                              "port "+str(port))


    def connect(self, url, port, timeout, retry_time):
        start = time.time()
        print "Request socket", timeout
        self._request_socket(url, port, timeout, retry_time)
        timeout = timeout - (time.time()-start)
        print "Await response", timeout
        time.sleep(retry_time)
        #self.get_next_message(timeout, retry_time)

    def close_connection(self, port, force):
        socket = self.get_socket(port)
        if force:
            socket.Close("force")
        else:
            socket.Close("")

    def accept_connection(self, listen_port, timeout, retry_time):
        my_thread = threading.Thread(target=self._accept_socket,
                                     args=(listen_port, timeout, retry_time))
        my_thread.daemon = True
        my_thread.start()

    def port_list(self):
        return [socket.GetLocalPort() for socket in self._loop_over_sockets()]

    def get_socket(self, port):
        """
        Get a socket from the socket queue; caller now owns the socket and must
        put it back when finished with it

        - port: integer corresponding to the socket LocalPort

        Throws a KeyError if the socket is not in the queue. Note that if 
        another thread is using a socket, it may not be in the queue - even if
        it has been opened by the socket manager
        """
        for socket in self._loop_over_sockets():
            if socket.GetLocalPort() == port:
                return socket
        raise SocketError("Could not find socket in socket_queue")

    def put_socket(self, socket):
        if socket.IsValid() and socket not in [self._loop_over_sockets()]:
            self.socket_queue.put_nowait(socket)
        else:
            raise SocketError("Socket was not valid")

    def get_next_message(self, timeout, retry_time):
        start = time.time()
        while time.time() - start < timeout or timeout <= 0.:
            try:
                (port, message_in) = self.message_queue.get_nowait()
                if message_in.class_name == self.__class__.__name__ and \
                                                not message_in.acknowledge:
                    try: # call the appropriate function
                        message_in.acknowledge = True
                        print "RECEIVING", message_in
                        func_call = getattr(self, message_in.function)
                        args = tuple(message_in.args)
                        message_in.return_value = func_call(*args)
                        print func_call, message_in.return_value
                    except Exception: # handle the error
                        message_in.data = []
                        exc_name = str(sys.exc_info()[0])
                        exc_msg = str(sys.exc_info()[1])
                        message_in.errors[exc_name] = exc_msg
                    finally: # return to user
                        print "RETURNING", self.port_list(), message_in
                        self.send_message(port, message_in)
                        return
                else: # not for us, put it back on the queue
                    self.message_queue.put((port, message_in), False)
            except Queue.Empty: # no messages, keep polling
                pass
            time.sleep(retry_time)

    def socket_list(self):
        my_list = []
        for socket in self._loop_over_sockets():
            my_list.append({
                "local_port":socket.GetLocalPort(),
                "local_address":socket.GetLocalInetAddress().GetHostAddress(),
                "remote_port":socket.GetPort(),
                "local_address":socket.GetInetAddress().GetHostAddress(),
                "valid":socket.IsValid()
            })
        return my_list

    def _loop_over_sockets(self):
        """
        Loop over sockets in the socket queue; put them back in the queue after
        caller is done
        """
        for socket in self._loop_over_sockets_unsafe():
            yield socket
            self.socket_queue.put_nowait(socket)

    def _loop_over_sockets_unsafe(self):
        """
        Loop over sockets in the socket queue; dont put them back in the queue 
        after caller is done
        """
        sock_list = []
        while True:
            try:
                socket = self.socket_queue.get_nowait()
            except Queue.Empty: 
                raise StopIteration("No sockets in socket_queue")
            # finished - we have looped once and not found the socket
            if socket in sock_list:
                self.socket_queue.put_nowait(socket)
                raise StopIteration("Looped over the queue")
            yield socket
            sock_list.append(socket)

    def _start_listening_for_messages(self):
        """
        Start a new thread listening for messages on each alive port

        If there is already an existing thread, then don't start a new one
        """
        my_thread = threading.Thread(target=self._listen_for_messages,
                         args=(self.retry_time,))
        my_thread.daemon = True
        my_thread.start()

    def _start_message_sender(self):
        my_thread = threading.Thread(target=self._send_queued_messages,
                                     args=(self.retry_time,))
        my_thread.daemon = True
        my_thread.start()
        

    def _send_queued_messages(self, retry_time):
        while True:
            for socket in self._loop_over_sockets():
                try:
                    port, message = self.send_message_queue.get_nowait()
                    if socket.GetLocalPort() == port:
                        print "SENDING", "sock", id(self), port, message
                        socket.Send(message)
                    else:
                        print "NOT SENDING", "sock", id(self), port
                        self.send_message_queue.put_nowait((port, message))
                except Queue.Empty:
                    pass
                time.sleep(retry_time)

    def _listen_for_messages(self, retry_time):
        while True:
            for socket in self._loop_over_sockets():
                message = ROOT.TMessage()
                if socket.IsValid():
                    socket.Recv(message)
                    self._queue_received_message(socket.GetLocalPort(), message)
            time.sleep(retry_time)

    def _queue_received_message(self, port, message):
        if message == None or message.GetClass() == None:
            # waiting for message
            return
        elif message.GetClass() == ROOT.TObjArray().Class():
            print "RECEIVED", message
            # control message
            try:
                ctrl_message = ControlMessage.new_from_root_repr(message)
                self.message_queue.put((port, ctrl_message), False)
            except TypeError:
                msg = str(sys.exc_info()[1])
                raise SocketError("Malformed control message: "+msg)
        else:
            raise SocketError( # bad type
                      "Malformed message - should be ROOT.TObjArray type")
            

    def _request_socket(self, url, port, timeout, retry_time):
        print "REQUEST SOCKET", url, port
        start_time = time.time()
        valid = False
        while not valid and \
              (timeout < 0 or time.time() - start_time < timeout) and \
              port not in self.port_list():
            tmp_socket = ROOT.TSocket(url, port)
            tmp_socket.SetOption(ROOT.TSocket.kNoBlock, 1)
            valid = tmp_socket.IsValid()
            if not valid:
                time.sleep(retry_time)
        if valid:
            print "REQUEST SOCKET OK", url, port
            port = tmp_socket.GetLocalPort()
            self.put_socket(tmp_socket)
            return port
        else:
            raise SocketError("Failed to connect to "+str(url)+":"+str(port))

    def _accept_socket(self, port, timeout, retry_time):
        print "ACCEPT SOCKET", port
        start_time = time.time()
        server_socket = ROOT.TServerSocket(port, True)
        server_socket.SetOption(ROOT.TServerSocket.kNoBlock, 1)
        tcp_socket_index = server_socket.GetDescriptor()
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
        print "ACCEPT SOCKET", port, "OK"
        self.put_socket(socket)
        return

        new_port = self.broadcast_port_list.pop()    
        self.accept_connection(new_port, timeout, retry_time)
        self.put_socket(socket)
        req_args = (
            socket.GetLocalInetAddress().GetHostAddress(),
            new_port, -1., 0.01)
        ctrl = ControlMessage("SocketManager", "_request_socket", req_args)
        self.send_message(port, ctrl)
        while (timeout < 0 or time.time()-start_time < timeout):
            try:
                (port, message_in) = self.message_queue.get_nowait()
                if message_in.id == ctrl.id and message_in.acknowledge == True:
                    break
            except Queue.Empty:
                pass
        print "CLOSING LISTEN SOCKET"
        del server_socket
        try:
            self.get_socket(port).Close() # close the socket
        except SocketError:
            pass
        self.accept_connection(port, timeout-(time.time()-start_time),
                               retry_time)
        print "Done accepting"

