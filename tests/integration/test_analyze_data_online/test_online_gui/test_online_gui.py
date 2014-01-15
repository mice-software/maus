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


class OnlineGUIClient():
    def __init__(self, client_name, port):
        print "Client starting on port", port
        try:
            valid = False
            while not valid:
                my_socket = ROOT.TSocket("localhost", port)
                valid = my_socket.IsValid()
                time.sleep(0.1)
            print "Client socket accepted"
            image_data = ROOT.MAUS.ImageData() # pylint: disable = E1101
            image = ROOT.MAUS.Image() # pylint: disable = E1101
            image.SetRunNumber(1)
            image.SetSpillNumber(2)
            dt = ROOT.MAUS.DateTime()
            dt.SetDateTime("2013-01-08T01:02:03.456789")
            image.SetInputTime(dt)
            dt.SetDateTime("2014-01-08T01:02:03.456789")
            image.SetOutputTime(dt)
            canvas_wrappers = image.GetCanvasWrappers()
            wrap = ROOT.MAUS.CanvasWrapper()
            cpx = ROOT.TCanvas("px canvas", "px canvas")
            hpx = ROOT.TH1F("hpx","px distribution", 100, -4, 4)
            hpx.FillRandom("gaus", 100000);
            hpx.Draw()
            wrap.SetCanvas(cpx)
            wrap.SetDescription("canvas description")
            canvas_wrappers.push_back(wrap)
            image.SetCanvasWrappers(canvas_wrappers)
            image_data.SetImage(image)
            tree = ROOT.TTree("Image", "TTree") # pylint: disable = E1101
            tree.Branch("image_data", image_data, image_data.GetSizeOf(), 1)

            message = ROOT.TMessage(ROOT.TMessage.kMESS_OBJECT)
            message.WriteObject(tree)

            print "Client sending message"
            my_socket.Send(message)
            my_socket.Close()
            print "Client waits 10 seconds..."
            time.sleep(10)
        except KeyboardInterrupt:
            print "Server killed"
            sys.stdout.flush()
        finally:
            sys.excepthook(*sys.exc_info())

    def __del__(self):
        pass

class OnlineGUITest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        this = os.path.expandvars(
            "${MAUS_ROOT_DIR}/tests/integration/test_analyze_data_online/"+\
            "test_online_gui.py",
        )
        for port in cls.ports:
            cls.clients[port] = subprocess.Popen([
                  "python", this_exe, "sender", str(port)
            ], stdin=subprocess.PIPE)

    @classmethod
    def tearDownClass(cls):
        i = 0
        print "Killing server subprocess"
        for port, process in cls.clients:
            process.send_signal(signal.SIGINT)
            print "Killing process for port", cls.port,\
                  "pid", get_port_procs(cls.port)
        try:
            os.kill(get_port_procs(cls.port), signal.SIGKILL)
        except Exception:
            print "Failed to kill process"

    def setUp(self):
        pass

    def test_online_gui(self):
        pass
        
    ports = [9091, 9092, 9093]
    clients = {}

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "sender":
        OnlineGUIClient("test", int(sys.argv[2]))
    else:
        unittest.main()

