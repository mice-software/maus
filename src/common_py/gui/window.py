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
Module handles GUI elements for making python GUIs within MAUS. 

An interface is provided that enables layout to be defined using json
configuration files.
"""

import sys
import json
import ROOT

KNORMAL = ROOT.TGLayoutHints.kLHintsNormal
KEXPANDX = ROOT.TGLayoutHints.kLHintsExpandX
KCENTERX = ROOT.TGLayoutHints.kLHintsCenterX
DEFAULT_TEXT_LENGTH = 10
WINDOW_LIST = []

def layout(option):
    """
    Select a layout option for the element
      - option: string layout option - either normal (default), close (close 
                packed), close_v, close_h (close packed in only vertical or 
                horizontal direction)
    Returns an instance of ROOT.TGLayoutHints.

    Throws a ValueError if the option is not recognised.
    """
    close = 1
    normal = 5
    if option == "normal":
        return ROOT.TGLayoutHints(KNORMAL, normal, normal, normal, normal)
    if option == "close": # close_packed
        return ROOT.TGLayoutHints(KNORMAL, close, close, close, close)
    if option == "close_v": # close packed vertically
        return ROOT.TGLayoutHints(KNORMAL, normal, normal, close, close)
    if option == "close_h": # close_packed horizontally
        return ROOT.TGLayoutHints(KNORMAL, close, close, normal, normal)
    raise ValueError("Failed to recognise layout option "+str(option))

class GuiError(Exception):
    """
    GuiErrot can be thrown when user wants to catch and handle errors that
    are specifically related to gui errors (and e.g. let other errors pass
    through to caller)
    """
    def __init__(self, *args, **kwargs):
        """Initialise Exception base class"""
        Exception.__init__(self, *args, **kwargs)


class Label:
    """
    Wrapper for ROOT TGLabel

    I couldn't find an elegant way to build a ROOT TGLabel with a particular
    width; so I fill the label with default characters, and then update once
    the window is drawn with the actual text. Note that ROOT default font is not
    fixed width, so fill characters should have some width.
    """
    def __init__(self, parent, name, label_length, alignment, _layout):
        """
        Initialise the label

          - parent; parent ROOT frame
          - name; text that will fill the label
          - label_length; width of the label
          - alignment; integer index defining how the text is aligned within the
            label
        """
        self.name = name
        self.frame = ROOT.TGLabel(parent, "a"*label_length)
        self.frame.SetTextJustify(alignment)
        parent.AddFrame(self.frame, layout(_layout))

    def update(self):
        """Call to update the text once the window is built"""
        self.frame.SetText(self.name)

    @staticmethod
    def new_from_dict(my_dict, parent):
        """
        Build a Label based on a dictionary
            - my_dict; dictionary of string:value pairs defining the layout
                - type; should be "label"
                - name; text to fill the label
                - alignment; 'right', 'center' or 'left' to align the text.
                    Vertical alignment is always center
                - label_length; Integer defining the default width of the label. 
                - layout; see layout function...
              alignment, label_length, layout are optional
            - parent; reference to the parent frame
        Return value is the dictionary with additional entry "frame" appended
        containing a reference to the TGLabel object
        """
        if my_dict["type"] != "label":
            raise ValueError("Not a label")
        name = my_dict["name"]
        alignment = ROOT.TGLabel.kTextLeft
        if "alignment" in my_dict and my_dict["alignment"] == "right":
            alignment = ROOT.TGLabel.kTextRight
        elif "alignment" in my_dict and my_dict["alignment"] == "center":
            alignment = ROOT.TGLabel.kTextCenterX
        label_length = 5
        if "label_length" in my_dict:
            label_length = my_dict["label_length"]
        else:
            my_dict["label_length"] = label_length
        _layout = "normal"
        if "layout" in my_dict:
            _layout = my_dict["layout"]
        else:
            my_dict["layout"] = _layout
        my_dict["label"] = Label(parent, name, label_length, alignment, _layout)
        my_dict["frame"] = my_dict["label"].frame
        return my_dict

class NamedTextEntry: # pylint: disable=R0913
    """
    A named text entry is a label followed by a text entry box.

    These are used frequently enough that it is worthwhile to make a special
    entry for this.
    """
    def __init__(self, parent, name, default_text, entry_length=10, \
                 label_length=5, tooltip=""):
        """
        Initialise the text entry
          - parent; parent frame
          - name; string name that will appear in the label and is used to name
            the text entry
          - default_text; text that will be placed in the text entry by default
          - entry_length; width of the text entry field (number of characters)
          - label_length; width of the label (number of characters)
          - tooltip; text to place in a tool tip - if empty string, no tool tip
            will be made.
        """
        self.frame = ROOT.TGHorizontalFrame(parent)
        self.name = name
        self.label = ROOT.TGLabel(self.frame, "a"*label_length)
        self.label.SetTextJustify(ROOT.TGLabel.kTextLeft)
        self.text_entry = ROOT.TGTextEntry(self.frame, "a"*entry_length, 0)
        self.frame.AddFrame(self.label, layout("close_v"))
        self.frame.AddFrame(self.text_entry, layout("close_v"))
        self.text_entry.SetText(default_text)
        if tooltip != "":
            self.text_entry.SetToolTipText(tooltip)

    def update(self):
        """
        Update the label to contain actual text
        """
        self.label.SetText(self.name)

    @staticmethod
    def new_from_dict(my_dict, parent):
        """
        Initialise a text entry from a dictionary
          - my_dict; dictionary containing configuration options
              - type; should be string like 'named_text_entry'
              - name; text to put in the label
              - default_text; default text that fills the text entry box
              - tool_tip; make a tool tip next to the text entry
              - entry_length (optional); width of the text entry box (number of
                characters)
              - label_length (optional); width of the label (number of
                characters)
          - parent; parent frame
        Return value is the dictionary with "text_entry" field and "frame" field
        appended containing references to the NamedTextEntry and the actual
        TGTextEntry box
        """
        if my_dict["type"] != "named_text_entry":
            raise ValueError("Not a text box")
        name = my_dict["name"]
        default = my_dict["default_text"]
        entry_length = DEFAULT_TEXT_LENGTH
        tool_tip = ""
        if "tool_tip" in my_dict:
            tool_tip = my_dict["tool_tip"]
        if "entry_length" in my_dict:
            entry_length = my_dict["entry_length"]
        else:
            my_dict["entry_length"] = entry_length
        label_length = 5
        if "label_length" in my_dict:
            label_length = my_dict["label_length"]
        else:
            my_dict["label_length"] = label_length
        my_dict["text_entry"] = NamedTextEntry(parent, name, default,
                                   entry_length, label_length, tooltip=tool_tip)
        my_dict["frame"] = my_dict["text_entry"].frame
        return my_dict

def function_wrapper(function):
    """
    Lightweight wrapper to intervene before TPyDispatcher handles python errors
    """
    return FunctionWrapper(function).execute

class FunctionWrapper: #pylint: disable=R0903
    """
    Function wrapper to intervene before TPyDispatcher handles python errors
    """
    def __init__(self, function):
        """
        Lightweight class that adds some error handling
        """
        self.function = function

    def execute(self, *args, **keywdargs):
        """
        Handle the errors from python function calls as I want
        """
        try:
            return self.function(*args, **keywdargs)
        except: # pylint: disable=W0702, W0142
            sys.excepthook(*sys.exc_info())

class Window(): # pylint: disable=R0201
    """
    This is the main class in this module. Initialise a window based on a json
    configuration file and layout the elements.

    Layout options
      Json configuration should be a dictionary. The top level object requires
      following fields
        - type; either main_frame (builds a TGMainFrame) or transient_frame 
          (builds a TGTransientFrmae
        - name; window name
        - children; list of child frames, each of which is a dict

      Window initialisation iterates over the child dicts and parses each one
      into a child TGFrame.  All frames support a "layout" entry, which 
      determines the amount of padding that will be placed around the entry.
        - "layout"; set the amount of padding in the frame. Can be either
          "normal" to make standard padding (5 units in each direction), "close"
          padding (1 unit in each direction), "close_v" (1 unit vertically, 5
          horizontally) or "close_h" (1 unit horizontally, 5 vertically) 
      The "type" entry of the dictionary determines the type of TGFrame that 
      will be contructed. Options are:
      - "horizontal_frame"; makes a TGHorizontalFrame, with a list of children.
          - "children": list of child frames to place in the horizontal frame
            Each sub-frame will be placed horizontally next to the previous one.
      - "vertical_frame"; makes a TGVerticalFrame, with a list of children.
          - "children": list of child frames to place in the horizontal frame.
            Each sub-frame will be placed vertically below the previous one.
      - "label"; makes a text TGLabel
          - "name": name of the label
          - "label_length": width of space allocated to the label in characters.
             (Default 5)
      - "text_entry"; makes a TGTextEntry
          - "name": name of the label
          - "label_length": width of space allocated to the label in characters.
             (Default 5)
      - "named_text_entry": makes a NamedTextEntry, which is a collection of
         TGHorizontalFrame, TGLabel and TGTextEntry. See NamedTextEntry 
         documentation for more details.
      - "button": makes a TGTextButton i.e. standard click button
          - "name": text to be used on the button; put an ampersand in to set a
            hot key
          - "tool_tip": text to be used as a tool tip
      - "drop_down": make a TGComboBox i.e. a drop down box.
          - "entries": list of strings, each of which becomes an entry in the
            drop down box, indexing from 0
          - "selected": integer determining the entry that will be selected
            initialliy (Default 0)
      - "list_box": make a TGListBox i.e. a list multiselect box.
          - "entries": list of strings, each of which becomes an entry in the
            drop down box, indexing from 0
          - "selected": list of integers determining the entries that will be 
            selected initialliy (Default is an empty list)
      - "check_button": make a TGCheckButton i.e. a true/false tick box
          - "text": text to place next to the check button
          - "default_state": set to 1 to make the box ticked by default, 0 to
            make it unticked
      - "special": user-defined GUI elements can be added at RunTime by making a
        special.
          - "manipulator": name of the manipulator to use for this special item.
            manipulators are set at the initialisation stage by passing a
            dictionary to manipulator_dict. Dictionary should contain a mapping
            that maps string "manipulator" to the function that will be called
            at run time to set up the "special" GUI element. The "manipulator"
            function should take the GUI element dict as an argument and return
            it at the end. The returned GUI element dict will be used to 
            construct the GUI element as per normal.
    """

    def __init__(self, parent, main, data_file=None, # pylint: disable = R0913
                 json_data=None, manipulator_dict=None):
        """
        Initialise the window
          - parent; parent ROOT frame (e.g. TGMainFrame/TGTransientFrame)
          - main; top level TGMainFrame for the whole GUI
          - data_file; string containing the file name for the layout data
          - json_data; json object containing the layout data
          - manipulator_dict; if there are any frames of type "special", this
            dictionary maps to a function which performs the layout for that
            frame.
        Throws an exception if both, or neither, of data_file and json_data are
        defined - caller must define the window layout in one and only one place
        """
        if data_file != None and json_data != None:
            raise ValueError("Define only one of json_data and data_file")
        if data_file == None and json_data == None:
            raise ValueError("Must define json_data or data_file")
        if data_file != None:
            self.data = open(data_file).read()
            self.data = json.loads(self.data)
        else:
            self.data = json_data
        if manipulator_dict == None:
            manipulator_dict = {}
        self.main_frame = None
        self.update_list = []
        self.socket_list = []
        self.tg_text_entries = {}
        self.manipulators = manipulator_dict
        self._setup_frame(parent, main)
        return None

    def close_window(self):
        """
        Close the window
        """
        # root v5.34 - makes segv if I don't disable the text entry before
        # closing
        for tg_text_entry in self.tg_text_entries.values():
            tg_text_entry.SetState(0)
        self.main_frame.CloseWindow()
        self.main_frame = None

    def get_frame(self, frame_name, frame_type, frame_list=None):
        """
        Get the frame with given name and type from a frame list
          - frame_name; specify the "name" field in the frame's dictionary
          - frame_type; specify the "type" field in the frame's dictionary
          - frame_list; search within this list. If not defined, will use the
            top level frames list (self.data)
        Returns a ROOT TGFrame object. Raises ValueError if no frame was found
        """
        frame_dict = self.get_frame_dict(frame_name, frame_type, frame_list)
        return frame_dict["frame"]

    def get_frame_dict(self, frame_name, frame_type, frame_list=None):  
        """
        Get the frame dictionary with given name and type from a frame list
          - frame_name; specify the "name" field in the frame's dictionary
          - frame_type; specify the "type" field in the frame's dictionary
          - frame_list; search within this list. If not defined, will use the
            top level frames list (self.data)
        Returns a dictionary object containing the frame configuration. The ROOT
        TGFrame object is mapped to the "frame" key.
        """
        if frame_list == None:
            frame_list = self.data["children"]
        frame = self._get_frame_dict_recurse(frame_name, frame_type, frame_list)
        if frame == None:
            raise ValueError("Could not find frame of name "+str(frame_name)+\
                             " type "+str(frame_type))
        return frame

    def set_action(self, frame_name, frame_type, frame_socket, action):
        """
        Set an action that will occur on a given signal
          - frame_name; string containing the name of the frame that makes the
            signal; if frame_type is a named_text_entry, then the TGTextEntry
            will be used
          - frame_type; string containing the type of the frame that makes the
            signal
          - frame_socket; string containing the name of the ROOT function that
            makes the signal. If the ROOT function takes arguments, those must
            be specified too
          - action; function that will be called when the specified signal is
            dispatched
        e.g.  my_window.set_action("select_box", "drop_down", "Selected(Int_t)",
                                   select_action)
        will call select_action() when the frame with "type":"drop_down" and
        "name":"select_box" makes a signal Selected(Int_t).
        """
        frame = self.get_frame(frame_name, frame_type)
        # for named_text_entry, we want the text_entry, not the HorizontalFrame
        # that wraps it
        if frame_type == "named_text_entry":
            frame = self._find_text_entry(frame_name)[0]
        self.socket_list.append(ROOT.TPyDispatcher(function_wrapper(action)))
        frame.Connect(frame_socket, 'TPyDispatcher', self.socket_list[-1], 
                      'Dispatch()')

    def set_button_action(self, button_name, action):
        """
        Set an action that will occur when a button emits a  Clicked() signal
            - button_name; name of the button
            - action; function that will be called when the button is clicked
        """
        self.set_action(button_name, "button", "Clicked()", action)

    def get_text_entry(self, name, value_type):
        """
        Get the text stored in either a text_entry or a named_text_entry
            - name; name of the text entry
            - value_type; convert the value to the given type.
        Returns a value of type value_type.
        Raises a type error if the conversion fails (string conversion will 
        always succeed).
        """
        text_entry, text_length = self._find_text_entry(name)
        if text_length:
            pass
        try:
            value = value_type(text_entry.GetText())
            return value
        except ValueError:
            raise GuiError("Failed to parse text entry "+name+" as "+\
                           str(value_type))

    def set_text_entry(self, name, value):
        """
        Set the text stored in either a text_entry or a named_text_entry
            - name; name of the text entry
            - value; put the given value in the text_entry, converting to a
                     string using the value's __str__ method. The resultant 
                     string will be truncated to the width of the text entry.
        """
        text_entry, text_length = self._find_text_entry(name)
        my_value = str(value)
        if len(my_value) > text_length-1:
            my_value = my_value[0:text_length-1]
        text_entry.SetText(my_value)

    def _get_frame_dict_recurse(self, frame_name, frame_type, frame_list=None):
        """
        (Private) recursively search for a frame of a given name and type.
        Called by get_frame_dict.
        """
        for item in frame_list:
            if "name" in item and "type" in item:
                if item["name"] == frame_name and item["type"] == frame_type:
                    return item
            # if we have not found the frame dict here, then recurse into 
            # children
            if "children" in item:
                frame = self._get_frame_dict_recurse(frame_name,
                                                      frame_type,
                                                      item["children"])
                if frame != None:
                    return frame

    def _find_text_entry(self, name):
        """
        (Private) find a text entry. Search first for named_text_entry, if that
        fails search for text_entry. Return the TGTextEntry and the length of
        the text entry (number of characters).
        """
        try:
            value_dict = self.get_frame_dict(name, 'named_text_entry')
        except ValueError:
            value_dict = None
        entry_length = DEFAULT_TEXT_LENGTH
        if value_dict != None:
            if "entry_length" in value_dict:
                entry_length = value_dict["entry_length"]
            return (value_dict['text_entry'].text_entry,
                    entry_length)
        value_dict = self.get_frame_dict(name, 'text_entry')
        if "entry_length" in value_dict:
            entry_length = value_dict["entry_length"]
        if value_dict == None:
            raise ValueError("Could not find text entry named "+str(name))
        return value_dict["frame"], entry_length

    def _setup_frame(self, parent, main):
        """
        (Private) set up the frame. Parse the json file and convert to ROOT 
        TGFrames, lay out the windows and so forth.
        """
        if self.data["type"] == "transient_frame":
            self.main_frame = ROOT.TGTransientFrame(parent, main)
        elif self.data["type"] == "main_frame":
            self.main_frame = ROOT.TGMainFrame(parent)
        else:
            raise ValueError("Failed to recognise frame type "+\
                             str(self.data["type"]))
        self._expand_frames(self.data["children"], self.main_frame)
        self.data["frame"] = self.main_frame
        self.main_frame.SetWindowName(self.data["name"])
        self.main_frame.MapSubwindows()
        self.main_frame.Resize(self.main_frame.GetDefaultSize())
        self.main_frame.MapWindow()
        self._label_update()

    def _label_update(self):
        """
        (Private) once the labels are laid out, with width enforced by fake
        string data, update the labels with the real text. A bit hacky.
        """
        for item in self.update_list:
            item.update()

    def _expand_frames(self, frames, parent):
        """
        Parse the json object, adding frames to all json items
        """
        for frames_index, item in enumerate(frames):
            if item["type"] == "special":
                manipulator_name = item["manipulator"]
                if manipulator_name not in self.manipulators:
                    raise ValueError("Manipulator "+manipulator_name+\
                                     " has not been defined")
                item = self.manipulators[manipulator_name](item)
                frames[frames_index] = item
            if item["type"] in self.parse_item_dict.keys():
                parser = self.parse_item_dict[item["type"]]
                parser(self, parent, item)
            else:
                raise ValueError("Did not recognise item type "+item["type"])
            layout_option = "normal"
            if "layout" in item.keys():
                layout_option = item["layout"]
            parent.AddFrame(item["frame"], layout(layout_option))
            if "children" in item:
                self._expand_frames(item["children"], item["frame"])

    def _parse_horizontal_frame(self, parent, item):
        """parse a horizontal_frame into a TGHorizontalFrame"""
        item["frame"] = ROOT.TGHorizontalFrame(parent)

    def _parse_vertical_frame(self, parent, item):
        """parse a vertical_frame into a TGVerticalFrame"""
        item["frame"] = ROOT.TGVerticalFrame(parent)

    def _parse_named_text_entry(self, parent, item):
        """parse a name_text_entry into a NamedEntry"""
        item = NamedTextEntry.new_from_dict(item, parent)
        self.tg_text_entries[item["name"]] = item["text_entry"].text_entry
        self.update_list.append(item["text_entry"])

    def _parse_canvas(self, parent, item):
        """parse a canvas into a TRootEmbeddedCanvas"""
        item["frame"] = ROOT.TRootEmbeddedCanvas('canvas',
                                         parent, item["width"], item["height"])
        item["frame"].Draw()

    def _parse_label(self, parent, item):
        """parse a label into a TGLabel"""
        item = Label.new_from_dict(item, parent)
        self.update_list.append(item["label"])

    def _parse_button(self, parent, item):
        """parse a text_button into a TGTextButton"""
        name = item["name"]
        item["frame"] = ROOT.TGTextButton(parent, name, 50)
        if "tool_tip" in item:
            item["frame"].SetToolTipText(item["tool_tip"])

    def _parse_text_entry(self, parent, item):
        """parse a text_entry into a TGTextEntry"""
        entry_length = DEFAULT_TEXT_LENGTH
        if "entry_length" in item:
            entry_length = item["entry_length"]
        item["frame"] = ROOT.TGTextEntry(parent, "a"*entry_length, 0)
        # use default, then change to get width right?
        default_text = ""
        if "default_text" in item:
            default_text = item["default_text"]
        item["frame"].SetText(default_text)
        self.tg_text_entries[item["name"]] = item["frame"]

    def _parse_list_box(self, parent, item):
        """parse a list_box into a TGListBox"""
        item["frame"] = ROOT.TGListBox(parent)
        for i, entry in enumerate(item["entries"]):
            item["frame"].AddEntry(entry, i)
        item["frame"].SetMultipleSelections(True)
        item["frame"].Resize(150, 20)
        if "selected" in item:
            for selected in item["selected"]:
                item["frame"].Select(selected)

    def _parse_drop_down(self, parent, item):
        """parse a drop_down into a TGComboBox"""
        item["frame"] = ROOT.TGComboBox(parent)
        for i, entry in enumerate(item["entries"]):
            item["frame"].AddEntry(entry, i)
        if "selected" in item:
            item["frame"].Select(item["selected"])

    def _parse_check_button(self, parent, item):
        """parse a check_button into a TGCheckButton"""
        item["frame"] = ROOT.TGCheckButton(parent, item["text"])
        item["frame"].SetState(item["default_state"])

    parse_item_dict = {
         "horizontal_frame":_parse_horizontal_frame,
         "vertical_frame":_parse_vertical_frame,
         "named_text_entry":_parse_named_text_entry,
         "canvas":_parse_canvas,
         "label":_parse_label,
         "button":_parse_button,
         "text_entry":_parse_text_entry,
         "list_box":_parse_list_box,
         "drop_down":_parse_drop_down,
         "check_button":_parse_check_button,
    }

