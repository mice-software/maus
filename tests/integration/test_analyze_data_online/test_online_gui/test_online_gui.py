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

from docstore.DocumentStore import DocumentStoreException
from docstore.root_document_store import RootDocumentDB
from docstore.root_document_store import RootDocumentStore
from docstore.root_document_store import SocketError
import threading_utils

ONLINE_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/bin/online/online_gui/")
sys.path.append(ONLINE_DIR)

import online_gui
from online_gui import CanvasRewrapped
from online_gui import OnlineGui
import gui  

THIS = "${MAUS_ROOT_DIR}/tests/integration/test_analyze_data_online/"+\
       "test_online_gui/"
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
                text = ROOT.TText()
                text.DrawTextNDC(0.1, 0.01, "Run: "+str(image.GetRunNumber())+\
                                             " Spill: "+\
                                             str(image.GetSpillNumber()))
                canvas.Update()
                canvas_wrap.SetCanvas(canvas)
                image.CanvasWrappersPushBack(canvas_wrap)
            image_data.SetImage(image)
            yield image_data
            spill += 1

class OnlineGUIClient(object):
    """Mockup of a reducer"""

    def __init__(self, collection_name, port):
        """Set the reducer going"""
        ROOT.gROOT.SetBatch(True)
        rds = RootDocumentStore(1., 0.01)
        rds.connect({"host":"localhost", "port":port})
        while True:
            try:
                rds.create_collection(collection_name, 10)
                break
            except DocumentStoreException:
                print "Failed to create collection on port", port
                rds.disconnect()
                rds = RootDocumentStore(1., 0.01)
                rds.connect({"host":"localhost", "port":port})
                time.sleep(1)
        for i, image_data in enumerate(generate_image_data(collection_name)):
            tree = ROOT.TTree("Image", "TTree")
            tree.Branch("data", image_data, image_data.GetSizeOf(), 1)
            tree.Fill()
            try:
                rds.put(collection_name, i, tree)
            except (DocumentStoreException, SocketError):
                sys.excepthook(*sys.exc_info())
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
        for port in OnlineGUITest.db_input_ports:
            exe = os.path.expandvars(THIS+"test_online_gui.py")
            proc = subprocess.Popen(["python", exe, "outputter", str(port)],
                                    stdin=subprocess.PIPE)
            OnlineGUITest.subprocs.append(proc)
        cls.ol_gui = GUI[0]
        cls.dc = DC
        while cls.ol_gui._redraw_target == None:
            time.sleep(0.1)

    @classmethod
    def tearDownClass(cls):
        for proc in cls.subprocs:
            proc.send_signal(signal.SIGKILL)

    def setUp(self):
        pass

    def _get_canvas_list(self, queue):
        gen = threading_utils.generate_fifo_queue(queue)
        threading_utils.go_back_fifo_queue(queue)
        return sorted([wrap.sort_key() for wrap in gen])

    def _test_online_gui_pause(self):
        # wait until the gui draws first hist, then click pause
        pause_button = self.ol_gui.window.get_frame("&Pause", "button")
        pause_button.Clicked()
        redraw_id = id(self.ol_gui._redraw_target)
        spill_list = self._get_canvas_list(self.ol_gui._canvases_read)
        # check that we paused okay
        self.assertEqual(self.ol_gui._paused.get_value(), True)
        self.assertEqual(pause_button.GetText(), "Play")
        time.sleep(5)
        # check that no variables changed which should be fixed
        self.assertEqual(redraw_id, id(self.ol_gui._redraw_target))
        self.assertEqual(spill_list,
                         self._get_canvas_list(self.ol_gui._canvases_read))
        # unpause
        pause_button.Clicked()
        # check that we unpaused okay
        self.assertEqual(self.ol_gui._paused.get_value(), False)
        self.assertEqual(pause_button.GetText(), "Pause")
        time.sleep(5)
        # check that variables are now changing
        self.assertNotEqual(redraw_id, id(self.ol_gui._redraw_target))
        self.assertNotEqual(spill_list,
                         self._get_canvas_list(self.ol_gui._canvases_read))

    def _test_online_gui_reload(self):
        # pause
        pause_button = self.ol_gui.window.get_frame("&Pause", "button")
        pause_button.Clicked()
        print "PAUSED"
        redraw = self.ol_gui._redraw_target
        spill_list = self._get_canvas_list(self.ol_gui._canvases_read)
        time.sleep(5)
        # reload
        print "RELOAD"
        reload_button = self.ol_gui.window.get_frame("Re&load", "button")
        reload_button.Clicked()
        # check that we redraw the same canvas and we updated the spill list
        self.assertNotEqual(redraw.spill_number,
                            self.ol_gui._redraw_target.spill_number)
        self.assertEqual(redraw.canvas_title(),
                         self.ol_gui._redraw_target.canvas_title())
        self.assertNotEqual(spill_list,
                         self._get_canvas_list(self.ol_gui._canvases_read))
        # unpause 
        pause_button.Clicked()

    def _test_online_gui_reconnect(self):
        # disconnect, then force a reload (clear any messages)
        self.ol_gui._docstore.disconnect()
        reload_button = self.ol_gui.window.get_frame("Re&load", "button")
        reload_button.Clicked()
        spill_list = self._get_canvas_list(self.ol_gui._canvases_read)
        time.sleep(5)
        self.assertEqual(spill_list,
                         self._get_canvas_list(self.ol_gui._canvases_read))
        # reconnect, then force another reload (restore the histograms)
        recon_button = self.ol_gui.window.get_frame("Re&connect", "button")
        recon_button.Clicked()
        reload_button.Clicked()
        time.sleep(5)
        self.assertNotEqual(spill_list,
                         self._get_canvas_list(self.ol_gui._canvases_read))

    def _test_forward_backward_button(self):
        pause_button = self.ol_gui.window.get_frame("&Pause", "button")
        pause_button.Clicked()
        draw_list = self.dc["online_gui_default_canvases"]
        # cycle forwards, compare 
        redraw_name = self.ol_gui._redraw_target.canvas_title()
        index_start = draw_list.index(redraw_name)
        fwd_button = self.ol_gui.window.get_frame("&>", "button")
        for i in range(index_start, len(draw_list)*3):
            test_index = i%len(draw_list)
            test_collection = draw_list[test_index].split(' ')[0]
            if test_collection in self.ol_gui._collections.get_value():
                redraw_name = self.ol_gui._redraw_target.canvas_title()
                self.assertEqual(draw_list[test_index], redraw_name)
            fwd_button.Clicked()
        # cycle backwards
        redraw_name = self.ol_gui._redraw_target.canvas_title()
        index_start = i
        bkwd_button = self.ol_gui.window.get_frame("&<", "button")
        for i in range(index_start+1, 0, -1):
            test_index = i%len(draw_list)
            test_collection = draw_list[test_index].split(' ')[0]
            if test_collection in self.ol_gui._collections.get_value():
                redraw_name = self.ol_gui._redraw_target.canvas_title()
                self.assertEqual(draw_list[test_index], redraw_name)
            bkwd_button.Clicked()
        pause_button.Clicked()
        
    def _test_canvas_select(self):
        select = self.ol_gui.window.get_frame("canvas_select", "list_box")
        target = select.GetEntry(6).GetText()
        # simulate GUI click (to select)
        select.Select(6, True)
        select.SelectionChanged() # emit the signal (happens in GUI click)
        # check draw_titles were updated
        draw_titles = [x for x in threading_utils.generate_fifo_queue(self.ol_gui._draw_titles)]
        self.assertTrue(target in draw_titles)
        # simulate GUI click (to unselect)
        select.Select(6, False)
        select.SelectionChanged()
        # check draw_titles were updated
        draw_titles = [x for x in threading_utils.generate_fifo_queue(self.ol_gui._draw_titles)]
        self.assertFalse(target in draw_titles)

    def test_rotate_period_action(self):
        rotate_text = self.ol_gui.window.set_text_entry("Rotate period", 2.1)
        self.ol_gui.window._find_text_entry("Rotate period")[0].ReturnPressed()
        self.assertEqual(self.ol_gui._rotate_period.get_value(), 2.1)
        raw_input()

    def test_reload_period_action(self):
        reload_text = self.ol_gui.window.set_text_entry("Reload period", 2.1)
        self.ol_gui.window._find_text_entry("Reload period")[0].ReturnPressed()
        self.assertEqual(self.ol_gui._reload_period.get_value(), 2.1)

    db_input_ports = [9091, 9092, 9093, 9094, 9095, 9096, 9097, 9098, 9099]
    subprocs = []

def get_dc():
    if len(sys.argv) > 1:
        return None
    dc = online_gui.get_datacards()
    dc["online_gui_host"] = "localhost"
    dc["online_gui_default_canvases"] = \
                                   ["test_9091 0", "test_9092 1", "test_9093 2",
                                    "no_such_coll 0"]
    return dc

def get_gui():
    dc = DC
    gui_ports = [9101]
    if len(sys.argv) > 1:
        return None
    rdb = RootDocumentDB(OnlineGUITest.db_input_ports+gui_ports, 0.01)
    my_gui = []
    for port in gui_ports:
        dc["online_gui_port"] = port
        my_gui.append(OnlineGui(dc))
    #gui.gui_exception_handler.set_error_level("exceptions")
    return my_gui

DC = get_dc()
GUI = get_gui()

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "outputter":
        port = int(sys.argv[2])
        OnlineGUIClient("test_"+str(port), port)
    else:
        unittest.main() #submain(10)

