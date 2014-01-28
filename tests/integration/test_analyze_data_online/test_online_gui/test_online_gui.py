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

THIS = "${MAUS_ROOT_DIR}/tests/integration/test_analyze_data_online/"+\
       "test_online_gui/test_online_gui.py"
GUI = "${MAUS_ROOT_DIR}/bin/online/online_gui/online_gui.py"

class OnlineGUIClient():
    def __init__(self, collection_name, port):
        ROOT.gROOT.SetBatch(True)
        print collection_name, port
        rds = RootDocumentStore(1., 0.01)
        rds.connect({"host":"localhost", "port":port})
        rds.create_collection(collection_name, 10)
        run = 9122
        spill = 0
        time.sleep(2)
        while run % 100 != 0:
            run += 1
            spill = 1
            canvas_list = []
            now = ROOT.MAUS.DateTime()
            for i in range(3):
                cname = collection_name+"_"+str(run)+"_"+str(i)
                canvas = ROOT.TCanvas(cname, cname)
                hist = ROOT.TH1D("test hist", "test hist;x;y", 100, -1., 1.)
                canvas_list.append((canvas, hist))
            while spill % 100 != 0:
                image_data = ROOT.MAUS.ImageData()
                image = ROOT.MAUS.Image()
                image.SetRunNumber(run)
                image.SetSpillNumber(spill)
                image.SetInputTime(now)
                now.SetDateTime(datetime.datetime.now().isoformat())
                image.SetOutputTime(now)
                for canvas, hist in canvas_list:
                    canvas_wrap = ROOT.MAUS.CanvasWrapper()
                    canvas_wrap.SetDescription("Description of "+collection_name)
                    canvas.cd()
                    hist.FillRandom("gaus", 100)
                    hist.Draw()
                    canvas_wrap.SetCanvas(canvas)
                    image.CanvasWrappersPushBack(canvas_wrap)
                image_data.SetImage(image)
                tree = ROOT.TTree("Image", "TTree")
                tree.Branch("data", image_data, image_data.GetSizeOf(), 1)
                tree.Fill()
                try:
                    rds.put(collection_name, spill, tree)
                    print "+"+str(port)
                except (DocumentStoreException, SocketError):
                    print "-"+str(port)
                spill += 1
                time.sleep(1)

    def __del__(self):
        pass

class OnlineGUITest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        rdb = RootDocumentDB(cls.db_input_ports+cls.db_output_ports, 0.01)
        for port in cls.db_input_ports:
            exe = os.path.expandvars(THIS)
            subprocess.Popen(["python", exe, "outputter", str(port)])

    def test_online_gui(self):
        exe = os.path.expandvars(GUI)
        for port in self.db_output_ports:
            subprocess.Popen(["python", exe,
                              "--online_gui_host", "localhost",
                              "--online_gui_port", str(port),
            ])
        raw_input()

    db_input_ports = [9091, 9092, 9093]
    db_output_ports = [9101, 9102, 9103]

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "outputter":
        port = int(sys.argv[2])
        OnlineGUIClient("test_"+str(port), port)
    else:
        unittest.main()

