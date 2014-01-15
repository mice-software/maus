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

import time
import os
import sys
import ROOT

import gui.gui_exception_handler
from gui.window import Window

ENV_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/bin/online/online_gui/")
SHARE_DIR = (ENV_DIR+"share/")

class OnlineGui():
     def __init__(self):
        """Initialise the main window"""
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
        pass

    def help_action(self):
        """Handle a Help button press"""
        pass

    def exit_action(self):
        """Handle a Exit button press"""
        self.window.close_window()

def main():
    """
    The main function - build a OnlineGui and poll to see if it has been closed
    """
    try:
        online_gui = OnlineGui()
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
