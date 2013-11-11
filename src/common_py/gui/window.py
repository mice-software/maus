#!/usr/bin/env python

import time
import json
import os
import sys
import math
import ROOT

KNORMAL = ROOT.TGLayoutHints.kLHintsNormal
KEXPANDX = ROOT.TGLayoutHints.kLHintsExpandX
KCENTERX = ROOT.TGLayoutHints.kLHintsCenterX

def layout(option):
    close = 1
    normal = 5
    if option == "normal":
        return ROOT.TGLayoutHints(KNORMAL, 5, 5, 5, 5)
    if option == "close": # close_packed
        return ROOT.TGLayoutHints(KNORMAL, 1, 1, 1, 1)
    if option == "close_v": # close packed vertically
        return ROOT.TGLayoutHints(KNORMAL, 5, 5, 1, 1)
    if option == "close_h": # close_packed horizontally
        return ROOT.TGLayoutHints(KNORMAL, 5, 5, 5, 5)

class Label:
    def __init__(self, parent, name, label_length, alignment, _layout):
        self.name = name
        self.frame = ROOT.TGLabel(parent, "a"*label_length)
        self.frame.SetTextJustify(alignment)
        parent.AddFrame(self.frame, layout(_layout))

    def update(self):
        self.frame.SetText(self.name)

    @staticmethod
    def new_from_dict(my_dict, parent):
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
        layout = "normal"
        if "layout" in my_dict:
            layout = my_dict["layout"]
        my_dict["label"] = Label(parent, name, label_length, alignment, layout)
        my_dict["frame"] = my_dict["label"].frame
        return my_dict

class NamedTextEntry:
    def __init__(self, parent, name, default_text, entry_length=10, label_length=5, tooltip=""):
        self.frame = ROOT.TGHorizontalFrame(parent)
        self.name = name
        self.label = ROOT.TGLabel(self.frame, "a"*label_length)
        self.label.SetTextJustify(ROOT.TGLabel.kTextLeft)
        self.text_entry = ROOT.TGTextEntry(self.frame, "a"*entry_length, 0)
        self.frame.AddFrame(self.label, layout("close_v"))
        self.frame.AddFrame(self.text_entry, layout("close_v"))
        self.text_entry.SetText(default_text)

    def update(self):
        self.label.SetText(self.name)

    @staticmethod
    def new_from_dict(my_dict, parent):
        if my_dict["type"] != "named_text_entry":
            raise ValueError("Not a text box")
        name = my_dict["name"]
        default = my_dict["default_text"]
        tool_tip = my_dict["tool_tip"]
        entry_length = 10
        if "entry_length" in my_dict:
            entry_length = my_dict["entry_length"]
        label_length = 5
        if "label_length" in my_dict:
            label_length = my_dict["label_length"]
        my_dict["text_entry"] = NamedTextEntry(parent, name, default, entry_length, label_length, tooltip=tool_tip)
        my_dict["frame"] = my_dict["text_entry"].frame
        return my_dict

class Window():
    def __init__(self, p, main, data_file):
        self.data = open(data_file).read()
        self.data = json.loads(self.data)
        self.main_frame = None
        self.text_entry_list = []
        self.socket_list = []
        try:
            self._setup_frame(p, main)
        except Exception:
            raise
        return None

    def get_frame(self, frame_name, frame_type, frame_list=None):
        frame_dict = self.get_frame_dict(frame_name, frame_type, frame_list)
        return frame_dict["frame"]

    def get_frame_dict(self, frame_name, frame_type, frame_list=None):  
        if frame_list == None:
            frame_list = self.data["children"]
        for item in frame_list:
            if "name" in item and "type" in item:
                if item["name"] == frame_name and item["type"] == frame_type:
                    return item
            # if we have not found the frame dict here, then recurse into 
            # children
            if "children" in item:
                frame = self.get_frame_dict(frame_name,
                                            frame_type,
                                            item["children"])
                if frame != None:
                    return frame

    def set_action(self, button_name, action):
        self.set_button_action(button_name, action)

    def set_button_action(self, button_name, action):
        button = self.get_frame(button_name, "button")
        self.socket_list.append(ROOT.TPyDispatcher(action))
        button.Connect('Clicked()', 'TPyDispatcher', self.socket_list[-1],
                       'Dispatch()')

    def _setup_frame(self, parent, main):
        if self.data["type"] == "transient_frame":
            self.main_frame = ROOT.TGTransientFrame(parent, main)
        elif self.data["type"] == "main_frame":
            self.main_frame = ROOT.TGMainFrame(parent)
        else:
            raise ValueError("Failed to recognise frame type "+str(self.data["type"]))
        self._expand_frames(self.data["children"], self.main_frame)
        self.data["frame"] = self.main_frame
        self.main_frame.SetWindowName(self.data["name"])
        self.main_frame.MapSubwindows()
        self.main_frame.Resize(self.main_frame.GetDefaultSize())
        self.main_frame.MapWindow()
        self._label_update()

    def _label_update(self):
        for item in self.text_entry_list:
            item.update()

    def _expand_frames(self, frames, parent):
        for item in frames:
            if item["type"] == "horizontal_frame":
                item["frame"] = ROOT.TGHorizontalFrame(parent)
            elif item["type"] == "vertical_frame":
                item["frame"] = ROOT.TGVerticalFrame(parent)
            elif item["type"] == "named_text_entry":
                item = NamedTextEntry.new_from_dict(item, parent)
                self.text_entry_list.append(item["text_entry"])
            elif item["type"] == "canvas":
                item["frame"] = ROOT.TRootEmbeddedCanvas('canvas',
                                          parent, item["width"], item["height"])
                item["frame"].Draw()
            elif item["type"] == "label":
                item = Label.new_from_dict(item, parent)
                self.text_entry_list.append(item["label"])
            elif item["type"] == "button":
                name = item["name"]
                item["frame"] = ROOT.TGTextButton(parent, name, 50)
            elif item["type"] == "button":
                name = item["name"]
                item["frame"] = ROOT.TGTextButton(parent, name, 50)
            elif item["type"] == "text_entry":
                entry_length = 10
                if "entry_length" in item:
                    entry_length = item["entry_length"]
                item["frame"] = ROOT.TGTextEntry(parent, "a"*entry_length, 0)
            else:
                raise ValueError("Did not recognise item type "+item["type"])
            layout_option = "normal"
            if "layout" in item.keys():
                layout_option = "layout"
            parent.AddFrame(item["frame"], layout(layout_option))
            if item["type"] == "text_entry":
                item["frame"].SetText(item["default_text"])
            if "children" in item:
                self._expand_frames(item["children"], item["frame"])

    def __del__(self):
        self.main_frame.Cleanup()

