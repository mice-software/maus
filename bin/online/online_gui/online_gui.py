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

# ROOT false positives
# pylint: disable=E1101

"""
Online gui for viewing reconstructed histograms
"""

import json
import datetime
import time
import os
import sys
import ROOT
import Queue
import threading
import copy

import gui.gui_exception_handler
from gui.gui_exception_handler import GuiError
from gui.window import Window
from docstore.root_document_store import RootDocumentStore
from docstore.root_document_store import SocketError
from docstore.DocumentStore import DocumentStoreException
from Configuration import Configuration
from threading_utils import ThreadedValue
from threading_utils import generate_fifo_queue
from threading_utils import go_back_fifo_queue

ENV_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/bin/online/online_gui/")
SHARE_DIR = (ENV_DIR+"share/")

class OnlineGui(): # pylint: disable = R0902
    """Main frame for the online gui window"""
    def __init__(self, datacards):
        """Initialise the main window"""
        self.last = datetime.datetime.now()
        self.window = Window(ROOT.gClient.GetRoot(),
                             ROOT.gClient.GetRoot(),
                             SHARE_DIR+"online_gui.json")
        self._set_actions()
        self._docstore = None
        self._datacards = datacards
        self._draw_titles = Queue.Queue()
        for title in datacards["online_gui_default_canvases"]:
            self._draw_titles.put(title)
        self._canvases_read = Queue.Queue()
        self._canvases_draw = ThreadedValue([])
        self._redraw_target = None
        self._paused = ThreadedValue(False)
        rotate_tmp = self.window.get_text_entry("Rotate period", type(1.))
        self._rotate_period = ThreadedValue(rotate_tmp)
        reload_tmp = self.window.get_text_entry("Reload period", type(1.))
        self._reload_period = ThreadedValue(reload_tmp)
        self._poll_period = ThreadedValue(0.1)
        self._collections = ThreadedValue([])
        self._init_docstore()
        self._start_polling()

    def _set_actions(self):
        """
        Set up actions (responses to signals)
        """
        self.window.set_button_action("&Pause", self.pause_action)
        self.window.set_button_action("&Redraw", self.redraw_action)
        self.window.set_button_action("Re&load", self.reload_action)
        self.window.set_button_action("Re&connect", self.reconnect_action)
        self.window.set_button_action("E&xit", self.exit_action)
        self.window.set_button_action("&<", self.back_action)
        self.window.set_button_action("&>", self.forward_action)
        self.window.get_frame("canvas_options", "vertical_frame").\
                                                                Resize(200, 500)
        self.window.get_frame("canvas_select", "list_box").Resize(200, 200)
        self.window.set_action("canvas_select",
                               "list_box",
                               "SelectionChanged()",
                               self.canvas_select_action)
        self.window.set_action("Rotate period", "named_text_entry",
                               "ReturnPressed()", self.rotate_period_action)
        self.window.set_action("Reload period", "named_text_entry",
                               "ReturnPressed()", self.reload_period_action)

    def _init_docstore(self):
        """
        Initialise the docstore
        """
        datacards = self._datacards
        self._docstore = RootDocumentStore(
                  datacards["root_document_store_timeout"],
                  datacards["root_document_store_poll_time"], 10, 10)
        self._docstore.connect({"host":datacards["online_gui_host"],
                               "port":datacards["online_gui_port"]})

    def _start_polling(self):
        """
        Start polling the database and rotating the canvases
        """
        my_thread = threading.Thread(target=self._poll_canvas_draw,
                                     args=())
        my_thread.daemon = True
        my_thread.start()
        my_thread = threading.Thread(target=self._poll_docstore,
                                     args=())
        my_thread.daemon = True
        my_thread.start()

    def pause_action(self):
        """Handle a Pause/Unpause button press"""
        pause_button = self.window.get_frame("&Pause", "button")
        if self._paused.get_value():
            self._paused.set_value(False)
            pause_button.SetText("&Pause")
        else:
            self._paused.set_value(True)
            pause_button.SetText("&Play")

    def redraw_action(self):
        """
        Handle a Redraw button press

        Redraws self._redraw_target
        """
        if self._redraw_target == None:
            raise GuiError("No canvas to draw")
        self._do_redraw(self._redraw_target)

    def reload_action(self):
        """Handle a Reload button press"""
        self._reload()
        if self._redraw_target != None:
            go_back_fifo_queue(self._draw_titles)
            self._draw_next()

    def reconnect_action(self):
        """Handle a Reconnect button press"""
        self._docstore.disconnect()
        self._init_docstore()

    def exit_action(self):
        """Handle a Exit button press"""
        self._paused.set_value(True) # tell polling to wait
        time.sleep(self._poll_period.get_value()) # thinkapow
        self.window.close_window()

    def reload_period_action(self):
        """Handle a change to the reload period"""
        tmp_reload_period = self.window.get_text_entry("Reload period",
                                                       type(1.))
        tmp_reload_period = float(tmp_reload_period)
        self._reload_period.set_value(tmp_reload_period)

    def rotate_period_action(self):
        """Handle a change to the rotate period"""
        tmp_rotate_period = self.window.get_text_entry("Rotate period",
                                                       type(1.))
        tmp_rotate_period = float(tmp_rotate_period)
        self._rotate_period.set_value(tmp_rotate_period)

    def forward_action(self):
        """Handle a click on the > forward button"""
        self._draw_next()

    def back_action(self):
        """Handle a click on the < back button"""
        go_back_fifo_queue(self._draw_titles)
        go_back_fifo_queue(self._draw_titles)
        self._draw_next()

    def canvas_select_action(self):
        """Handle an update to the selected canvases"""
        selected = self._get_canvas_selected()
        new_draw_titles = Queue.Queue()
        for item in selected:
            new_draw_titles.put(item)
        self._draw_titles = new_draw_titles

    def _poll_canvas_draw(self):
        """
        Iterate over the canvas_draw queue, updating the GUI as appropriate

        * Update the queue from the canvas_read queue
        * Refresh the combo box with items in the canvases_draw queue
        """
        # poll_number = 0
        while True:
            # poll_number += 1
            self._draw_next()
            self._sleepish(self._rotate_period)
            # handle pause
            while self._paused.get_value():
                time.sleep(self._poll_period.get_value())

    def _draw_next(self):
        """
        Draw the next canvas in the selected queue
        """
        canvases_draw = self._filter_canvases(self._canvases_draw.get_value())
        if len(canvases_draw) > 0:
            try:
                title = self._draw_titles.get_nowait()
                new_title = copy.deepcopy(title)
                self._draw_titles.put(title)
                for wrap in canvases_draw:
                    if wrap.canvas_title() == new_title:
                        self._do_redraw(wrap)
                        break
            except Queue.Empty:
                pass # no canvas selected
            except ValueError:
                pass
        self._canvases_draw.set_value(canvases_draw)


    def _sleepish(self, sleep_length):
        """
        Sleep - but keep polling for changes to the sleep_length
        * sleep_length - threaded value containing numeric sleep time
        """
        time_slept = 0.
        poll_period = self._poll_period.get_value()
        while sleep_length.get_value() - time_slept > poll_period:
            time_slept += self._poll_period.get_value()
            time.sleep(poll_period)

    def _update_canvases_draw(self):
        """update canvases"""
        draw = [wrap.deepcopy() \
                           for wrap in generate_fifo_queue(self._canvases_read)]
        draw.sort(key = lambda wrap: wrap.sort_key())
        return draw

    def _do_redraw(self, wrap):
        """
        Draw the canvas wrap in the main_canvas
        """
        # wrap_copy is for ROOT
        wrap_copy = wrap.deepcopy()
        embedded_canvas = self.window.get_frame("main_canvas", "canvas")
        # this eats the wrap_copy TCanvas
        wrap_copy.canvas_wrapper.EmbedCanvas(embedded_canvas)
        embedded_canvas.GetCanvas().Update()
        # this _redraw_target is for the main thread
        self._redraw_target = wrap.deepcopy()

    def _poll_docstore(self):
        """
        Poll the doc store for documents
        """
        while True:
            print "poll_docstore"
            self._reload()
            self._sleepish(self._reload_period)
            # handle pause
            while self._paused.get_value():
                time.sleep(self._poll_period.get_value())

    def _reload(self):
        """
        Reload canvases; if the list of canvases changed, update the multiselect
        """
        self._reload_canvases()
        # now update GUI elements (each call iterates over the whole
        # _canvases_draw queue)
        self._update_canvas_select()

    def _update_canvas_select(self):
        """
        Update the list of canvases in the canvas_select frame
        """
        all_canvas_titles = [wrap.canvas_wrapper.GetCanvas().GetTitle() \
                           for wrap in generate_fifo_queue(self._canvases_read)]

        select_box = self.window.get_frame("canvas_select", "list_box")
        select_box.RemoveAll()
        for i, title in enumerate(all_canvas_titles):
            select_box.AddEntry(title, i)
        for title in generate_fifo_queue(self._draw_titles):
            try:
                index = all_canvas_titles.index(title)
                select_box.Select(index)
            except ValueError: # item was no longer in the select - ignore it
                pass
        go_back_fifo_queue(self._draw_titles)
        # ROOT doesn't like redrawing the histogram titles properly - need to
        # force it
        self.window.get_frame("canvas_select", "list_box").Resize(200, 10)
        self.window.get_frame("canvas_select", "list_box").Resize(200, 200)

    def _get_canvas_selected(self):
        """
        Get the list of canvases selected in the canvas_select frame
        """
        select_box = self.window.get_frame("canvas_select", "list_box")
        selected_root = ROOT.TList()
        selected = []
        select_box.GetSelectedEntries(selected_root)
        while selected_root.Last() != None:
            selected.insert(0, copy.deepcopy(selected_root.Last().GetText()))
            selected_root.RemoveLast()
        return selected

    def _reload_canvases(self):
        """
        Reload canvases from the docstore
        """
        # get all of the collections (one for each reducer)
        try:
            collections = self._docstore.collection_names()
        except (DocumentStoreException, SocketError):
            print "Failed to get collection names from the docstore"
            return
        self._collections.set_value(collections)
        temp_canvases = []
        # update "last accessed" timestamp
        self.last = datetime.datetime.now()
        # get new canvases
        for collection_name in collections:
            temp_canvases += self._get_new_canvases(collection_name)
        # purge any duplicate canvases
        filtered_canvases = self._filter_canvases(temp_canvases)
        # build a new queue and reassign _canvases_read
        canvases_read = Queue.Queue()
        for canvas in filtered_canvases:
            canvases_read.put_nowait(canvas)
        self._canvases_read = canvases_read
        self._collections.set_value(collections)

    def _get_new_canvases(self, collection_name):
        """
        Get a list of new canvases from the collection
        """
        try:
            doc_list = self._docstore.get_since(collection_name, self.last)
        except DocumentStoreException:
            print "Failed to get documents from the docstore"
            return []
        if len(doc_list) == 0:
            return []
        tree = doc_list[-1]["doc"]
        image_data = ROOT.MAUS.ImageData()
        tree.SetBranchAddress("data", image_data)
        if tree.GetEntries() == 0:
            return []
        tree.GetEntry()
        return CanvasRewrapped.new_list_from_image(collection_name,
                                                   image_data.GetImage())

    def _filter_canvases(self, new_canvases):
        """
        Enforce that we only have one copy with a given (collection, title)

        Note that we want to keep the item with the highest input_time if there
        is a choice
        """
        combined_list = [wrap.deepcopy() \
                           for wrap in generate_fifo_queue(self._canvases_read)]
        combined_list += new_canvases
        combined_list.sort(key=lambda wrap: wrap.sort_key())

        if len(combined_list) == 0:
            return []
        filtered_list = []
        for i, item_2 in enumerate(combined_list[1:]):
            item_1 = combined_list[i]
            if item_1.collection == item_2.collection and \
               item_1.canvas_title() == item_2.canvas_title():
                pass
            else:
                filtered_list.append(item_1)
        filtered_list.append(combined_list[-1])
        return filtered_list

class CanvasRewrapped:
    """
    Small wrapper class for canvas; differs from the interface class because
    it flattens the data structure i.e. all of the time and run number
    information is stored at the same level as the canvas

    Store canvas_wrapper rather than canvas because I am not confident in
    TCanvas copying - only want that logic once
    """
    def __init__(self):
        """Initialise with dummy data"""
        self.collection = ""
        self.canvas_wrapper = None
        self.run_number = 0
        self.spill_number = 0
        self.input_time = datetime.datetime.now().isoformat()
        self.output_time = datetime.datetime.now().isoformat()

    @classmethod
    def new_from_c(cls, collection_name, image, c_canvas_wrapper):
        """
        Initialise from C++ data
        @param collection_name string containing the name of the collection
        @param image C++ ROOT.MAUS.Image object
        @param image C++ ROOT.MAUS.CanvasWrapper object
        @returns new CanvasRewrapped
        """
        wrap = CanvasRewrapped()
        wrap.collection = collection_name
        wrap.canvas_wrapper = ROOT.MAUS.CanvasWrapper(c_canvas_wrapper)
        wrap.run_number = image.GetRunNumber()
        wrap.spill_number = image.GetSpillNumber()
        wrap.input_time = image.GetInputTime().GetDateTime()
        wrap.output_time = image.GetOutputTime().GetDateTime()
        return wrap

    @classmethod
    def new_list_from_image(cls, collection_name, image):
        """
        Generate a new list of CanvasRewrappeds from an image
        @param image an instance of ROOT.MAUS.Image() - will make one
                CanvasRewrapped per CanvasWrapper on the image
        """
        wrap_list = []
        wrapper_vector = image.GetCanvasWrappers()
        for i in range(wrapper_vector.size()):
            wrap_list.append(CanvasRewrapped.new_from_c(collection_name,
                                                        image,
                                                        wrapper_vector[i]))
        return wrap_list

    def deepcopy(self):
        """
        Make a deepcopy of self
        """
        wrap = CanvasRewrapped()
        wrap.collection = self.collection
        if self.canvas_wrapper == None:
            wrap.canvas_wrapper = None
        else:
            wrap.canvas_wrapper = ROOT.MAUS.CanvasWrapper(self.canvas_wrapper)
        wrap.run_number = self.run_number
        wrap.spill_number = self.spill_number
        wrap.input_time = self.input_time
        wrap.output_time = self.output_time
        return wrap

    def sort_key(self):
        """Return the key for sorting"""
        return (self.collection,
                self.canvas_title(),
                self.input_time)

    def canvas_title(self):
        """Return the canvas title"""
        return self.canvas_wrapper.GetCanvas().GetTitle()

def get_datacards():
    """
    Return the datacards as a json document
    """
    config = Configuration().getConfigJSON(command_line_args = True)
    return json.loads(config)

def main():
    """
    The main function - build a OnlineGui and poll to see if it has been closed
    """
    try:
        cards = get_datacards()
        online_gui = OnlineGui(cards)
        gui.gui_exception_handler.set_error_level("exceptions")
        while online_gui.window.main_frame != None:
            time.sleep(1)
    except Exception:
        sys.excepthook(*sys.exc_info())
    except KeyboardInterrupt:
        print "Pressed Ctrl-C"
    finally:
        print "done"
        return 0 # pylint: disable=W0150

if __name__ == '__main__':
    main()
