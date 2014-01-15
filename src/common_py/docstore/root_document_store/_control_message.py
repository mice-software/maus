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
control_message contains a small class that encapsulates messaging of control
data between sockets
"""
import json
import ROOT

class ControlMessage:
    def __init__(self, message_dict):
        self.message = message_dict

    @classmethod
    def new_from_root_message(cls, root_message):
        if root_message.Class() != ROOT.TMessage().Class() and \
           root_message.GetClass() != ROOT.TObjString().Class():
            raise TypeError("Message could not be parsed to a ControlMessage")               
        root_object = root_message.ReadObject(root_message.GetClass())
        root_string = ROOT.TObjString(root_object)
        ascii_string = str(root_string.GetString())
        json_doc = json.loads(ascii_string)
        if json_doc["socket_control"]:
            return ControlMessage(json_doc)
        else:
            raise TypeError("Message could not be parsed to a ControlMessage")               

    @classmethod
    def new_open_message(cls, port, timeout, retry_time):
        return ControlMessage({"socket_control":True,
                               "open":{"port":port, "timeout":timeout,
                                       "retry_time":retry_time}})

    @classmethod
    def new_close_message(cls, port, force):
        return ControlMessage({"socket_control":True,
                               "close":{"port":port, "force":force}})

    @classmethod
    def new_text_message(cls, message, level):
        return ControlMessage({"socket_control":True,
                               "message":{"text":str(message),
                                          "verbose_level":level}})

    def json_repr(self):
        return self.message

    def root_repr(self):
        return ROOT.TObjString(json.dumps(self.message))

    def __str__(self):
        return json.dumps(self.message)

    def is_close_message(self):
        return "close" in self.message

    def is_open_message(self):
        return "open" in self.message

    def is_text_message(self):
        return "message" in self.message
       


