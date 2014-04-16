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
Gui handler class 
"""

import sys
import ROOT

import gui.window
from gui.window import GuiError

#pylint: disable=E1101

class MessageWindow: #pylint: disable=R0903
    """
    Make a window that just holds a label with a given message
    """
    def __init__(self, message):
        """
        Initialise; parent will always be main_frame
        """
        _message = str(message)
        _json_data = {
          "type":"transient_frame",
          "name":"error",
          "children":[{
                  "type":"label",
                  "label_length":len(_message)+5,
                  "name":_message,
              }, {
                  "type":"button",
                  "name":"&Okay"
              }
          ]
        }
        self.window = gui.window.Window(ROOT.gClient.GetRoot(),
                                        ROOT.gClient.GetRoot(),
                                        json_data=_json_data)
        self.window.set_button_action("&Okay", self.okay_action)

    def okay_action(self):
        """
        Pressing Okay just closes the window
        """
        self.window.close_window()

ERROR_LEVEL = 1
SYS_EXCEPTHOOK = sys.excepthook

def set_error_level(error_level):
    """
    Set the error level that will be handled by the exception handler
      - error_level: string or corresponding integer error level that will cause
        the exception handler to respond. Options are "errors", "exceptions",
        "gui_exceptions", "nothing"

    Also calls set_excepthook()
    """
    global ERROR_LEVEL # pylint: disable=W0603
    my_error_levels = ["errors", "exceptions", "gui_exceptions", "nothing"]
    if error_level in my_error_levels:
        ERROR_LEVEL = my_error_levels.index(error_level)
    elif error_level in range(4):
        ERROR_LEVEL = error_level
    else:
        raise ValueError("Did not recognise error_level "+str(error_level))
    set_excepthook()

def set_excepthook():
    """
    Set the excepthook to gui_exception_handler excepthook
    """
    sys.excepthook = _excepthook

def reset_excepthook():
    """
    Reset the excepthook to sys.excepthook
    """
    sys.excepthook = SYS_EXCEPTHOOK

def _excepthook(_type, _value, _traceback):
    """
    GuiExceptionHandler excepthook
    """
    if ERROR_LEVEL == 0:
        MessageWindow(str(_value))
    elif ERROR_LEVEL == 1 and isinstance(_value, Exception):
        MessageWindow(str(_value))
    elif ERROR_LEVEL == 2 and isinstance(_value, GuiError):
        MessageWindow(str(_value))
    SYS_EXCEPTHOOK(_type, _value, _traceback)


