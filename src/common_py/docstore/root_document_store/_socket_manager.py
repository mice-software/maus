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

#pylint: disable=E1101

"""
SocketManager wrapper wraps the ROOT TServerSocket
"""

import sys
import time
import threading
import Queue

import ROOT
from docstore.root_document_store._socket_error import SocketError
from docstore.root_document_store._control_message import ControlMessage

class SocketManager:
    """
    SocketManager handles socket set up and communications.
    """
    def __init__(self, listen_port_list, timeout, retry_time,
                 max_send_attempts=100):
        self.retry_time = retry_time
        self.socket_queue = Queue.Queue() # Fifo queue for easy iterating
        self.recv_message_queue = Queue.Queue() # Fifo queue for easy iterating
        self.send_message_queue = Queue.Queue()
        self.error_queue = Queue.Queue()
        self.max_send_attempts = max_send_attempts
        self.processing = False
        for listen_port in listen_port_list:
            if listen_port > 65535 or listen_port <= 0: # segv in ROOT
                raise SocketError("Port "+str(listen_port)+" out of range")
            self.accept_connection(listen_port, timeout, retry_time)

    def check_error_queue(self):
        """
        Check the error queue; raise an exception if there is a waiting error
        """
        try:
            raise self.error_queue.get_nowait()
        except Queue.Empty:
            pass

    def start_processing(self):
        """
        start the socket processing, looking for events to process
        """
        self._start_message_handler()
        self.processing = True

    def close_all(self, force):
        """
        close the socket
        - force: bool, if True then force the socket to close, otherwise wait
          for any existing messages to get picked up
        """
        for socket in self._loop_over_sockets_unsafe():
            if force:
                socket.Close("force")
            else:
                socket.Close("")

    def send_message(self, port, message):
        """
        Send a message through the socket
        - port: integer corresponding to the port over which the message should
                be sent. Raises SocketError if no socket is open on the target
                port.
        - message: object of type ControlMessage; raises SocketError if not
                a TMessage.
        """
        if not self.processing:
            raise SocketError("Can't post a message until start_processing()")
        if type(message) == type(ControlMessage()):
            try:
                root_message = message.root_repr()
            except TypeError as exc:
                raise SocketError(*exc.args)
        else:
            raise SocketError("Message must be a ControlMessage - got "+\
                              str(type(message)))
        try:
            self.send_message_queue.put_nowait((port, root_message, 0))
        except SocketError:
            raise SocketError("SocketManager knows nothing about sockets on "+\
                              "port "+str(port))


    def connect(self, url, port, timeout, retry_time):
        """
        Connect to a given port
        - url: url of the target machine
        - port: port on the target machine to connect to
        - timeout: give up after timeout seconds
        - retry_time: attempt to reconnect every retry_time seconds
        """
        return self._request_socket(url, port, timeout, retry_time)

    def close_connection(self, port, force):
        """
        close the a socket on a given port
        - port: the port on which the socket is open.
        - force: bool, if True then force the socket to close, otherwise wait
          for any existing messages to get picked up
        """
        socket = self._get_socket(port)
        if force:
            socket.Close("force")
        else:
            socket.Close("")

    def accept_connection(self, listen_port, timeout, retry_time):
        """
        Accept a connection on a given port
        - listen_port: the port on which to listen
        - timeout: give up after timeout seconds
        - retry_time: retry every retry_time seconds
        """
        my_thread = threading.Thread(target=self._accept_socket,
                                     args=(listen_port, timeout, retry_time))
        my_thread.daemon = True
        my_thread.start()

    def port_list(self):
        """
        Return a list of ports (integers) on which sockets are open.
        """
        return [socket.GetLocalPort() for socket in self._loop_over_sockets()]

    def socket_list(self):
        """
        Return a list of sockets metadata.

        @returns list of dictionaries, each containing following keys:
        - local_port: host port on which the socket is open
        - local_address: host address
        - remote_port: remote port on which the socket is open
        - remote_address: remote address
        - valid: true if the port is able to send and receive messages.
        """
        my_list = []
        for socket in self._loop_over_sockets():
            my_list.append({
                "local_port":socket.GetLocalPort(),
                "local_address":socket.GetLocalInetAddress().GetHostAddress(),
                "remote_port":socket.GetPort(),
                "remote_address":socket.GetInetAddress().GetHostAddress(),
                "valid":socket.IsValid()
            })
        return my_list


    def _get_socket(self, port):
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

    def _put_socket(self, socket):
        """
        Put a socket on the socket queue. Socket queue now owns the socket.
        - socket: the socket to put
        """
        if socket.IsValid() and socket not in [self._loop_over_sockets()]:
            self.socket_queue.put_nowait(socket)
        else:
            raise SocketError("Socket was not valid")

    def _loop_over_sockets(self):
        """
        Loop over sockets in the socket queue; put them back in the queue after
        caller is done
        """
        return self._loop_over_queue(self.socket_queue)

    def _loop_over_sockets_unsafe(self):
        """
        Loop over sockets in the socket queue; dont put them back in the queue 
        after caller is done
        """
        return self._loop_over_queue_unsafe(self.socket_queue)


    def _loop_over_queue(self, queue):
        """
        Loop over sockets in the socket queue; put them back in the queue after
        caller is done
        """
        for item in self._loop_over_queue_unsafe(queue):
            yield item
            queue.put_nowait(item)

    @classmethod
    def _loop_over_queue_unsafe(cls, queue):
        """
        Loop over sockets in the socket queue; dont put them back in the queue 
        after caller is done
        """
        item_list = []
        while True:
            try:
                item = queue.get_nowait()
            except Queue.Empty: 
                raise StopIteration("No items in queue")
            if item in item_list: # we have looped once
                queue.put_nowait(item)
                raise StopIteration("Looped over the queue")
            yield item
            item_list.append(item)
        
    def _start_message_handler(self):
        """
        Start the message handler
        """
        my_thread = threading.Thread(target=self._message_handler,
                                     args=(self.retry_time,))
        my_thread.daemon = True
        my_thread.start()

    def _message_handler(self, retry_time): # pylint: disable=R0912
        """
        Message handler; send any messages on the send queue; receive any
        incoming messages
        """
        max_sends = self.max_send_attempts
        while True:
            try:
                 # Try to send all messages
                for socket in self._loop_over_sockets():
                    sys.stdout.flush()
                    for item in \
                          self._loop_over_queue_unsafe(self.send_message_queue):
                        port, message, sends = item
                        if socket.GetLocalPort() == port:
                            socket.Send(message)
                        else:
                            self.send_message_queue.put(item)
                # Increment sent count on messages that did not send
                # Remove them if they have exceeded max_sends
                for item in \
                          self._loop_over_queue_unsafe(self.send_message_queue):
                    port, message, sends = item
                    if sends >= max_sends and max_sends > 0:
                        raise SocketError("Failed to send message after "+\
                                          str(sends)+" attempts")
                    else:
                        self.send_message_queue.put((port, message, sends+1))
                # Try to receive any queued messages
                for socket in self._loop_over_sockets():
                    message = ROOT.TMessage()
                    if socket.IsValid():
                        try_again = True
                        while try_again:
                            socket.Recv(message)
                            try_again = self._queue_received_message(
                                                        socket.GetLocalPort(),
                                                        message)
                # Drop any invalid sockets
                for socket in self._loop_over_sockets_unsafe():
                    if socket.IsValid():
                        self.socket_queue.put(socket)
                time.sleep(retry_time)
            except Exception as exc: # pylint: disable=W0703
                sys.excepthook(*sys.exc_info())
                self.error_queue.put_nowait(exc)
            sys.stdout.flush()
            time.sleep(retry_time)

    def _queue_received_message(self, port, message):
        """
        Receive a message and put it on the recv_message_queue for reading later
        """
        if message == None or message.GetClass() == None:
            # waiting for message
            return False
        elif message.GetClass() == ROOT.TObjArray().Class():
            # control message
            ctrl_message = ControlMessage.new_from_root_repr(message)
            self.recv_message_queue.put((port, ctrl_message), False)
            return True
        else:
            raise SocketError( # bad type
                      "Malformed message - should be ROOT.TObjArray type")

    def _request_socket(self, url, port, timeout, retry_time):
        """
        Request a socket on a remote machine
        """
        if port > 65535 or port <= 0: # segv in ROOT
            raise SocketError("Port "+str(port)+" out of range")
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
            port = tmp_socket.GetLocalPort()
            self._put_socket(tmp_socket)
            return port
        else:
            raise SocketError("Failed to connect to "+str(url)+":"+str(port))

    def _accept_socket(self, port, timeout, retry_time):
        """
        Accept a socket request
        """
        start_time = time.time()
        server_socket = ROOT.TServerSocket(port, True)
        server_socket.SetOption(ROOT.TServerSocket.kNoBlock, 1)
        tcp_socket_index = server_socket.GetDescriptor()
        accepted_socket_index = ROOT.gSystem.AcceptConnection(tcp_socket_index)
        while True:
            while accepted_socket_index < 0 and \
                  (timeout < 0 or time.time()-start_time < timeout):
                sys.stdout.flush()
                accepted_socket_index = \
                                 ROOT.gSystem.AcceptConnection(tcp_socket_index)
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
            self._put_socket(socket)
            sys.stdout.flush()
            accepted_socket_index = \
                                 ROOT.gSystem.AcceptConnection(tcp_socket_index)
            time.sleep(retry_time)
        return

