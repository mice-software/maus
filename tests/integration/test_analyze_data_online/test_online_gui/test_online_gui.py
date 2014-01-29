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
import datetime
import unittest
import signal
import sys
import subprocess
import time
import os

import ROOT
import libMausCpp
import xboa.Common as Common

from docstore.DocumentStore import DocumentStoreException
from docstore.root_document_store import RootDocumentDB
from docstore.root_document_store import RootDocumentStore
from docstore.root_document_store import SocketError

ONLINE_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/bin/online/online_gui/")
sys.path.append(ONLINE_DIR)

from online_gui import CanvasRewrapped

THIS = "${MAUS_ROOT_DIR}/tests/integration/test_analyze_data_online/"+\
       "test_online_gui/test_online_gui.py"
GUI = "${MAUS_ROOT_DIR}/bin/online/online_gui/online_gui.py"

def generate_image_data(collection_name):
    """
    Generate some image data
    """
    ROOT.gROOT.SetBatch(True)
    run = 9122
    spill = 3
    while run % 100 != 0:
        run += 1
        spill = 1
        canvas_list = []
        now = ROOT.MAUS.DateTime()
        for i in range(3):
            cname = collection_name+" "+str(i)
            canvas = ROOT.TCanvas(cname, cname)
            hist = ROOT.TH1D("test hist", cname+";x;y", 100, -10., 10.)
            canvas_list.append((canvas, hist))
        while spill % 100 != 0:
            image_data = ROOT.MAUS.ImageData()
            image = ROOT.MAUS.Image()
            image.SetRunNumber(run)
            image.SetSpillNumber(spill)
            image.SetInputTime(now)
            now.SetDateTime(datetime.datetime.now().isoformat())
            image.SetOutputTime(now)
            for i, (canvas, hist) in enumerate(canvas_list):
                canvas_wrap = ROOT.MAUS.CanvasWrapper()
                canvas_wrap.SetDescription("Description of "+collection_name)
                canvas.cd()
                hist.FillRandom("gaus", 100)
                hist.Draw()
                latex = ROOT.TText()
                latex.DrawTextNDC(0.1, 0.01, "Run: "+str(image.GetRunNumber())+\
                                             " Spill: "+\
                                             str(image.GetSpillNumber()))
                canvas.Update()
                canvas_wrap.SetCanvas(canvas)
                image.CanvasWrappersPushBack(canvas_wrap)
            image_data.SetImage(image)
            yield image_data
            spill += 1

class OnlineGUIClient():
    def __init__(self, collection_name, port):
        ROOT.gROOT.SetBatch(True)
        print collection_name, port
        rds = RootDocumentStore(1., 0.01)
        rds.connect({"host":"localhost", "port":port})
        while True:
            try:
                rds.create_collection(collection_name, 10)
                break
            except DocumentStoreException:
                print "Failed to create collection on port", port
                time.sleep(1)
        time.sleep(2)
        for i, image_data in enumerate(generate_image_data(collection_name)):
            tree = ROOT.TTree("Image", "TTree")
            tree.Branch("data", image_data, image_data.GetSizeOf(), 1)
            tree.Fill()
            try:
                rds.put(collection_name, i, tree)
            except (DocumentStoreException, SocketError):
                print "REDUCER Failed on port "+str(port)
            time.sleep(1)

    def __del__(self):
        pass

class CanvasRewrappedTest(unittest.TestCase):
    """Test CanvasRewrapped"""

    def test_canvas_rewrapped(self):
        """Test CanvasRewrapped"""
        def_canvas = CanvasRewrapped()
        self.assertEqual(def_canvas.canvas_wrapper, None)
        def_canvas_copy = def_canvas.deepcopy()
        self.assertEqual(def_canvas_copy.canvas_wrapper, None)
        image_data = generate_image_data("collection").next()
        image = image_data.GetImage()
        wrap_list_new = CanvasRewrapped.new_list_from_image("collection", image)
        self.assertEqual(len(wrap_list_new), image.GetCanvasWrappers().size())
        wrap_list_copy = [wrap.deepcopy() for wrap in wrap_list_new]
        for wrap_list in [wrap_list_new, wrap_list_copy]:
            for i, item in enumerate(wrap_list):
                self.assertEqual(item.run_number, image.GetRunNumber())
                self.assertEqual(item.spill_number, image.GetSpillNumber())
                self.assertEqual(item.input_time,
                                 image.GetInputTime().GetDateTime())
                self.assertEqual(item.output_time,
                                 image.GetOutputTime().GetDateTime())
                ref_wrapper = image.GetCanvasWrappers()[i]
                self.assertEqual(item.canvas_wrapper.GetCanvas().GetTitle(),
                                 ref_wrapper.GetCanvas().GetTitle())

class OnlineGUITest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        rdb = RootDocumentDB(cls.db_input_ports+cls.db_output_ports, 0.01)
        for port in cls.db_input_ports:
            exe = os.path.expandvars(THIS)
            proc = subprocess.Popen(["python", exe, "outputter", str(port)],
                                    stdin=subprocess.PIPE)
            cls.subprocs.append(proc)

    @classmethod
    def tearDownClass(cls):
        for proc in cls.subprocs:
            proc.send_signal(signal.SIGKILL)

    def _test_clients(self):
        time.sleep(5)

    def test_online_gui(self):
        exe = os.path.expandvars(GUI)
        for port in self.db_output_ports[0:1]:
            proc = subprocess.Popen([
                "python", exe,
                "--online_gui_host", "localhost",
                "--online_gui_port", str(port),
            ])
            self.subprocs.append(proc)
        print "Press <cr> to finish"
        raw_input()

    db_input_ports = [9091, 9092, 9093]
    db_output_ports = [9101, 9102, 9103]
    subprocs = []

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "outputter":
        port = int(sys.argv[2])
        OnlineGUIClient("test_"+str(port), port)
    else:
        unittest.main()

