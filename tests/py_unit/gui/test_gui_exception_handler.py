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
Tests for the GUI exception handler
"""

import StringIO
import sys
import unittest

import gui.gui_exception_handler
from gui.gui_exception_handler import GuiError
from gui.gui_exception_handler import MessageWindow

class TestGuiError(unittest.TestCase): # pylint: disable=R0904
    """Test the GuiError"""

    def test_init(self):
        """Test the GuiError"""
        try:
            raise GuiError(1, 2, 3)
        except GuiError:
            exc_value = sys.exc_info()[1]
            self.assertEqual(exc_value.args, (1, 2, 3))
        try:
            raise GuiError(1, 2, 3)
        except Exception: # pylint: disable=W0703
            exc_value = sys.exc_info()[1]
            self.assertEqual(exc_value.args, (1, 2, 3))


class TestMessageWindow(unittest.TestCase): # pylint: disable=R0904
    """Test the MessageWindow"""
    def _test_window(self):
        """Test window"""
        window = MessageWindow("hello world")
        label = window.window.get_frame("hello world", "label")
        self.assertEqual(label.GetText(), "hello world")
        window.window.get_frame("&Okay", "button").Clicked()
        self.assertEqual(window.window.main_frame, None)

class TestGuiErrorHandler(unittest.TestCase): # pylint: disable=R0904
    """Test the gui_excpetion_handler"""
    def test_set_error_level(self):
        """Test gui_exception_handler.set_error level"""
        gui.gui_exception_handler.set_error_level("errors")
        self.assertEqual(gui.gui_exception_handler.ERROR_LEVEL, 0)
        gui.gui_exception_handler.set_error_level(1)
        self.assertEqual(gui.gui_exception_handler.ERROR_LEVEL, 1)
        try:
            gui.gui_exception_handler.set_error_level(5)
            self.assertFalse(True, msg="Should have raised exception")
        except ValueError:
            pass
        try:
            gui.gui_exception_handler.set_error_level("abc")
            self.assertFalse(True, msg="Should have raised exception")
        except ValueError:
            pass
        # pylint: disable=W0212
        self.assertEqual(sys.excepthook, gui.gui_exception_handler._excepthook)
        gui.gui_exception_handler.reset_excepthook()

    def test_excepthook_gui_exceptions(self):
        """Test gui_exception_handler handles exceptions okay"""
        temp_stderr = sys.stderr
        error_pairs = [
            (None, "Not an error"),
           (KeyboardInterrupt("Not an error"), Exception("my exception")),
           (Exception("my exception"), GuiError("my exception")),
           (GuiError("my exception"), None)
        ]
        for error_level in range(4):
            gui.gui_exception_handler.set_error_level(error_level)
            sys.stderr = StringIO.StringIO()
            if error_pairs[error_level][1] != None:
                try:
                    raise error_pairs[error_level][1]
                except: # pylint: disable=W0702
                    sys.excepthook(*sys.exc_info())
                self.assertEqual(sys.stderr.getvalue(), "")
            if error_pairs[error_level][0] != None:
                try:
                    raise error_pairs[error_level][0]
                except: # pylint: disable=W0702
                    sys.excepthook(*sys.exc_info())
                self.assertNotEqual(sys.stderr.getvalue(), "")
        # now check it gets reset
        sys.stderr = StringIO.StringIO()
        gui.gui_exception_handler.reset_excepthook()
        try:
            raise GuiError("my exception")
        except GuiError:
            sys.excepthook(*sys.exc_info())    
        self.assertNotEqual(sys.stderr.getvalue(), "")
        sys.stderr = temp_stderr

if __name__ == "__main__":
    unittest.main()


