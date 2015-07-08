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
Test ROOT gui wrapper
"""

# pylint: disable=E1101, R0904, C0103

import sys
import StringIO
import unittest
import os
import json

import ROOT

import gui.window

@unittest.skipIf(ROOT.gROOT.IsBatch(), "Needs a GUI window")
class TestAncillaries(unittest.TestCase):
    """
    Test GUI helper classes
    """
    def setUp(self):
        self.frame = ROOT.TGMainFrame(ROOT.gClient.GetRoot())

    def tearDown(self):
        self.frame.CloseWindow()

    def test_layout(self):
        """test gui.window.layout(...)"""        
        options = ["normal", "close", "close_v", "close_h"]
        left = 5, 1, 5, 1
        right = left
        top = 5, 1, 1, 5
        bottom = top
        for i, opt in enumerate(options):
            hints = gui.window.layout(opt)
            self.assertEqual(hints.GetPadLeft(), left[i])
            self.assertEqual(hints.GetPadRight(), right[i])
            self.assertEqual(hints.GetPadTop(), top[i])
            self.assertEqual(hints.GetPadBottom(), bottom[i])

    def test_label(self):
        """test gui.window.Label"""
        test_dict = {"name":"a", "type":"label", "alignment":"left",
                     "label_length":5, "layout":"close"}
        for label in [
                gui.window.Label(self.frame, "a", 5, 1, "close"),
                gui.window.Label.new_from_dict(test_dict, self.frame)["label"]]:
            self.assertEqual(label.name, "a")
            self.assertEqual(label.frame.GetText(), "a"*5)
            self.assertEqual(label.frame.GetTextJustify(), 1)
            label.update()
            self.assertEqual(label.frame.GetText(), "a")
        test_dict_2 = {"name":"a", "type":"label"}
        gui.window.Label.new_from_dict(test_dict_2, self.frame)

    def test_named_text_entry(self):
        """test gui.window.NamedTextEntry"""
        test_dict = {"name":"c", "type":"named_text_entry", "default_text":"b",
                     "entry_length":9, "label_length":4, "tool_tip":"a"}
        for entry in [
                gui.window.NamedTextEntry(self.frame, "c", "b", 9, 4, "a"),
                gui.window.NamedTextEntry.new_from_dict(test_dict,
                                                    self.frame)["text_entry"]]:
            self.assertEqual(entry.name, "c")
            self.assertEqual(entry.label.GetText(), "a"*4)
            self.assertEqual(entry.text_entry.GetText(), "b")
            self.assertEqual(entry.text_entry.GetToolTip().GetText(), "a")
        test_dict_2 = {"name":"c", "type":"named_text_entry",
                       "default_text":"a"}
        gui.window.NamedTextEntry.new_from_dict(test_dict_2, self.frame)

@unittest.skipIf(ROOT.gROOT.IsBatch(), "Needs a GUI window")
class TestFunctionWrapper(unittest.TestCase):
    """Small test case for function_wrapper and FunctionWrapper class"""
    def _my_function(self, message, alt_message=None): # pylint: disable=R0201
        """Test function to wrap"""
        if alt_message == None:
            return message
        if message == "exception":
            raise RuntimeError(message)
        return alt_message

    def test_function_wrapper(self):
        """Test gui.window.function_wrapper"""
        func = gui.window.function_wrapper(self._my_function)
        self.assertEqual(func("hello"), "hello")
        self.assertEqual(func("hello", "world"), "world")
        self.assertEqual(func("hello", alt_message="world"), "world")
        temp_stderr = sys.stderr
        sys.stderr = StringIO.StringIO()
        func("exception", alt_message="world")
        self.assertNotEqual(sys.stderr.getvalue(), "")
        sys.stderr = temp_stderr

@unittest.skipIf(ROOT.gROOT.IsBatch(), "Needs a GUI window")
class TestWindow(unittest.TestCase):
    """
    Test GUI window class
    """

    def setUp(self):
        """setup test data"""
        self.test_dir = os.path.expandvars(
            "${MAUS_ROOT_DIR}/tests/py_unit/gui/")
        self._manipulators = {
            "test_manipulator_1":self._window_manipulator_1,
        }
        self.window_1 = gui.window.Window(ROOT.gClient.GetRoot(),
                                          ROOT.gClient.GetRoot(),
                                          self.test_dir+"/test_window_1.json")
        self.window_2 = gui.window.Window(self.window_1.main_frame,
                                          self.window_1.main_frame,
                                          self.test_dir+"/test_window_2.json",
                                          manipulator_dict=self._manipulators)
        self._clicked = 0
        self.json_data = open(self.test_dir+"/test_window_2.json").read()
        self.json_data = json.loads(self.json_data)

    def tearDown(self):
        """tear down"""
        pass

    def _window_manipulator_1(self, item): # pylint: disable=R0201
        """Test manipulator"""
        item["type"] = "label"
        item["name"] = "test_8"
        return item

    def _click(self):
        """Test button action"""
        self._clicked += 1


    def test_init(self):
        """gui.window.Window initialisation"""
        window_3 = gui.window.Window(ROOT.gClient.GetRoot(),
                                     ROOT.gClient.GetRoot(),
                                     json_data=self.json_data,
                                     manipulator_dict=self._manipulators)
        frame_names = ["test_"+str(i) for i in range(1, 9)]
        frame_types = {
            "test_1":"vertical_frame",
            "test_2":"horizontal_frame",
            "test_3":"label",
            "test_4":"named_text_entry",
            "test_5":"button",
            "test_6":"drop_down",
            "test_7":"check_button",
            "test_8":"label",
            "test_9":"text_entry",
            "test_10":"list_box",
        }
        for name in frame_names:
            self.window_2.get_frame(name, frame_types[name])
            window_3.get_frame(name, frame_types[name])
        label_text = self.window_2.get_frame("test_3", "label").GetText()
        self.assertEqual(label_text, "test_3") # check the labels updated
        named_dict = self.window_2.get_frame_dict("test_4", "named_text_entry")
        label_text = named_dict["text_entry"].label.GetText()
        self.assertEqual(label_text, "test_4") # check the labels updated
        button_frame = self.window_2.get_frame("test_5", "button")
        self.assertEqual(button_frame.GetToolTip().GetText(), "tool")
        text_entered = self.window_2.get_frame("test_9", "text_entry").GetText()
        self.assertEqual(text_entered, "text")
        selected = ROOT.TList()
        lbox = self.window_2.get_frame("test_10", "list_box")
        selected = [i for i in range(lbox.GetNumberOfEntries()) \
                                                        if lbox.GetSelection(i)]
        self.assertEqual(selected, [0, 2])

    def test_init_exceptions(self):
        """gui.window.Window initialisation - exceptions"""
        # pylint: disable=W0612
        try:
            window_3 = gui.window.Window(ROOT.gClient.GetRoot(),
                                         ROOT.gClient.GetRoot(),
                                         self.test_dir+"/test_window_3.json")
            self.assertTrue(False, msg="Should have thrown ValueError")
        except ValueError:
            pass
        try:
            fname = self.test_dir+"/test_window_2.json"
            window_3 = gui.window.Window(ROOT.gClient.GetRoot(),
                                         ROOT.gClient.GetRoot(),
                                         data_file=fname,
                                         json_data=self.json_data,
                                         manipulator_dict=self._manipulators)
            self.assertTrue(False, msg="Should have thrown ValueError")
        except ValueError:
            pass
        try:
            window_3 = gui.window.Window(ROOT.gClient.GetRoot(),
                                         ROOT.gClient.GetRoot())
            self.assertTrue(False, msg="Should have thrown ValueError")
        except ValueError:
            pass

    def test_close_window(self):
        """gui.window.Window close_window"""
        self.window_1.close_window()
        self.window_2.close_window()

    def test_get_frame_dict(self):
        """gui.window.Window get_frame_dict"""
        try:
            self.window_2.get_frame_dict("test_0", "vertical_frame")
            self.assertTrue(False, "Should have raised a ValueError")
        except ValueError:
            pass
        try:
            self.window_2.get_frame_dict("test_1", "horizontal_frame")
            self.assertTrue(False, "Should have raised a ValueError")
        except ValueError:
            pass
        my_dict = self.window_2.get_frame_dict("test_1", "vertical_frame")
        self.assertEqual(type(my_dict["frame"]), type(ROOT.TGVerticalFrame()))

    def test_set_action(self):
        """gui.window.Window set_action, set_button_action"""
        self.window_2.set_button_action("test_5", self._click)
        button = self.window_2.get_frame("test_5", "button")
        self.assertEqual(self._clicked, 0)
        button.Clicked()
        self.assertEqual(self._clicked, 1)
        self.window_2.set_button_action("test_5", self._click)
        button.Clicked()
        self.assertEqual(self._clicked, 3)
        self.window_2.set_action("test_5", "button", "Clicked()", self._click)
        button.Clicked()
        self.assertEqual(self._clicked, 6)
        # test named_text_entry action sets to the text_entry frame, not the
        # parent composite

    def test_get_set_text_entry(self):
        """gui.window.Window get_text_entry, set_text_entry"""
        self.assertEqual(self.window_2.get_text_entry("test_4", type("")),
                         "test")
        self.assertEqual(self.window_2.get_text_entry("test_9", type("")),
                         "text")
        self.window_2.set_text_entry("test_4", "bling_blong")
        self.assertEqual(self.window_2.get_text_entry("test_4", type("")),
                         "bling_blo")
        self.window_2.set_text_entry("test_9", "blang_blung")
        self.assertEqual(self.window_2.get_text_entry("test_9", type("")),
                         "blang_blu")

if __name__ == "__main__":
    unittest.main()



