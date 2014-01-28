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

import json
import datetime
import time
import os
import sys
import ROOT
import Queue

import gui.gui_exception_handler
from gui.window import Window
from docstore.root_document_store import RootDocumentStore
from Configuration import Configuration

ENV_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/bin/online/online_gui/")
SHARE_DIR = (ENV_DIR+"share/")

def _canvas_sort_predicate(canvas):
    return (canvas["collection"], canvas["title"], canvas["input_time"])

class OnlineGui():
    def __init__(self, datacards):
        """Initialise the main window"""
        self.last = datetime.datetime.now()
        self.images = {}
        self.window = Window(ROOT.gClient.GetRoot(), # pylint: disable=E1101
                             ROOT.gClient.GetRoot(), # pylint: disable=E1101
                             SHARE_DIR+"online_gui.json")
#        self.window.set_button_action("&Pause", self.pause_action)
#        self.window.set_button_action("&Redraw", self.redraw_action)
#        self.window.set_button_action("&Reload", self.redraw_action)
#        self.window.set_button_action("&Reconnect", self.reconnect_action)
#        self.window.set_button_action("&Help", self.help_action)
        self.window.set_button_action("E&xit", self.exit_action)
        self.window.get_frame("canvas_options", "vertical_frame").Resize(200, 500)
        self.window.get_frame("canvas_select", "list_box").Resize(200, 200)
        self.docstore = None
        self._init_docstore(datacards)
        self._canvases_drawn = []
        self._canvases_read = Queue.Queue()
        self._rotate_period = 1.
        self._reload_period = 1.
        self._poll_docstore()

    def _init_docstore(self, datacards):
        self.docstore = RootDocumentStore(
                  datacards["root_document_store_timeout"],
                  datacards["root_document_store_poll_time"])
        self.docstore.connect({"host":datacards["online_gui_host"],
                               "port":datacards["online_gui_port"]})

    def pause_action(self):
        """Handle a Pause/Unpause button press"""
        pass

    def redraw_action(self):
        """Handle a Redraw button press"""
        pass

    def refresh_action(self):
        """Handle a Refresh button press"""
        pass

    def reconnect_action(self):
        """Handle a Reconnect button press"""
        self.docstore.disconnect()
        self.docstore.connect()

    def help_action(self):
        """Handle a Help button press"""
        pass

    def exit_action(self):
        """Handle a Exit button press"""
        self.window.close_window()

    def _update_canvases(self):
        pass


    def _do_redraw(self):
        self._loop_over_queue(self, queue)        

    def _poll_docstore(self):
        """Poll the doc store for documents"""
        while True:
            collections = self.docstore.collection_names()
            temp_canvases = []
            for collection_name in collections:
                temp_canvases += self._get_canvases(collection_name)
            # purge any duplicate canvases
            self._purge_canvases(temp_canvases)
            self.last = datetime.datetime.now()
            time.sleep(self._reload_period)

    def _get_canvases(self, collection_name):
        temp_canvases = []
        doc_list = self.docstore.get_since(collection_name, self.last)
        if len(doc_list) == 0:
            return []
        tree = doc_list[-1]["doc"]
        image_data = ROOT.MAUS.ImageData()
        tree.SetBranchAddress("data", image_data)
        if tree.GetEntries() == 0:
            return []
        tree.GetEntry()
        image = image_data.GetImage()
        canvas_wrappers = image.GetCanvasWrappers()
        for wrap in canvas_wrappers:
            # flatten off the data structure, purge ROOT evil
            temp_canvases.append({
                "collection":collection_name,
                "title":wrap.GetCanvas().GetTitle(),
                "canvas":wrap.GetCanvas(),
                "description":wrap.GetDescription(),
                "run_number":image.GetRunNumber(),
                "spill_number":image.GetSpillNumber(),
                "input_time":image.GetInputTime(),
                "output_time":image.GetOutputTime(),
            })
        return temp_canvases

    def _purge_canvases(self, new_canvases):
        """
        Enforce that we only have one copy with a given (collection, title)

        Note that we want to keep the item with the highest input_time
        """
        combined_list = []
        try:
            while True:
                combined_list.append(self._canvases_read.get_nowait())
        except Queue.Empty:
            pass
        combined_list += new_canvases
        combined_list.sort(key=_canvas_sort_predicate)
        temp_list = []
        for i, item_1 in enumerate(combined_list[1:]):
            item_2 = combined_list[i]
            if item_1["collection"] == item_2["collection"] and \
               item_1["title"] == item_2["title"]:
                pass
            else:
                temp_list.append(item_1)
        for item in temp_list:
            self._canvases_read.put_nowait(item)

    def _loop_over_queue(self, queue):
        """
        Loop over sockets in the socket queue; put them back in the queue after
        caller is done
        """
        for item in self._loop_over_queue_unsafe(queue):
            yield item
            queue.put_nowait(item)

    def _loop_over_queue_unsafe(self, queue):
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



def get_datacards():
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
