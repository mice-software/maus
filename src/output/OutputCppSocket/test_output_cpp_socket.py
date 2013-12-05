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
Tests for OutputCppSocket
"""
import unittest
import signal
import sys
import subprocess
import time
import os

import ROOT
import xboa.Common as Common

# hints: nmap for doing a port scan - nmap
# hints: lsof -i :<PORT> for finding pid which is listening on port <PORT>

def get_port_procs(port):
    try:
        lsof = subprocess.check_output(['lsof', '-i', ':'+str(port)])
    except subprocess.CalledProcessError:
        return None
    status = lsof.split('\n')[1]
    pid_string = status.split()[1]
    return int(pid_string)


class OutputCppSocketTest(unittest.TestCase):
    @classmethod
    def server(cls):
        while get_port_procs(cls.port) != None:
            cls.port += 1
        print "Server starting on port", cls.port
        try:
            my_server = ROOT.TServerSocket(cls.port, False)
            print "Server accepting"
            sys.stdout.flush()
            my_socket = my_server.Accept()
            print "Server accepted socket"
            sys.stdout.flush()
            message = ROOT.TMessage(ROOT.TMessage.kMESS_OBJECT)
            my_object = None
            while my_object == None:
                time.sleep(0.1)
                my_socket.Recv(message)
                try:
                    my_object = message.ReadObject(message.GetClass())
                    my_object.Draw()
                    print "Server received message", message
                except ReferenceError:
                    print "Server ReferenceError"
            raw_input()
        except KeyboardInterrupt:
            print "Server killed"
            sys.stdout.flush()
        finally:
            sys.excepthook(*sys.exc_info())

    @classmethod
    def setUpClass(cls):
        while get_port_procs(cls.port) != None:
            cls.port += 1
        print "Client using port", cls.port
        cls.proc = subprocess.Popen([
              "python",
              "src/OutputCppSocket/test_output_cpp_socket.py",
              "receiver"
        ], stdin=subprocess.PIPE)

    @classmethod
    def tearDownClass(cls):
        i = 0
        print "Killing server subprocess"
        cls.proc.send_signal(signal.SIGINT)
        print "Killing process for port", cls.port,\
              "pid", get_port_procs(cls.port)
        try:
            os.kill(get_port_procs(cls.port), signal.SIGKILL)
        except Exception:
            print "Failed to kill process"

    def setUp(self):
        pass

    def test_socket(self):
        print "Client setting up socket"
        valid = False
        while not valid:
            my_socket = ROOT.TSocket("localhost", self.port)
            valid = my_socket.IsValid()
            time.sleep(0.1)            
        print "Client socket accepted"
        hpx = ROOT.TH1F("hpx","px distribution", 100, -4, 4)
        hpx.FillRandom("gaus", 100000);
        message = ROOT.TMessage(ROOT.TMessage.kMESS_OBJECT)
        message.WriteObject(hpx)

        print "Client sending message"
        my_socket.Send(message)
        my_socket.Close()
        print "Client waits 10 seconds..."
        time.sleep(10)
        
    port = 9090

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "receiver":
        OutputCppSocketTest.server()
    else:
        unittest.main()

