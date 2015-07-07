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
# pylint: disable=E1101, R0913, R0902

"""
control_message contains a small class that encapsulates messaging of control
data between sockets
"""
import json
import uuid
import ROOT

class ControlMessage:
    """
    ControlMessage provides an API for handling communication between MAUS 
    instances via ROOT TSockets (intended to use the SocketManager class). 
    ControlMessage explicitly handles both ROOT and Json data formats. 

    ControlMessage holds a set of data which is essentially a function call and
    set of return values:
    * class_name, function, args, keywd_args reference the class and function to
      be called and the relevant, primitive arguments.
    * return_value references any primitive return values.
    * data holds any non-primitive data, both in call and return. It is a list
      of objects which can be formatted either in Json or ROOT format.
    * errors is a dictionary of <exception type>:<exception message>.
    * acknowledge should be set to true if this is a returning function call
    * id identifies the message; typically an outgoing function call and its
      return value are expected to hold the same id, which lets the caller know
      that a message corresponds to the associated outgoing function call.

    ControlMessage can be represented in either json format (for convenience) or
    in ROOT format (for transmitting over the TSocket).

    The Json format is a dictionary like
        {
            "class_name":<class name>,
            "function":<function name>,
            "args":[<list of args>],
            "keywd_args":{<keyword>:<argument>},
            "return_value":<return value>,
            "errors":{<error type>:<error message>},
            "acknowledge":acknowledge,
            "id":id
        } 
    Note that json formatted objects cannot have a data object associated. Json
    cannot (by its nature) encode ROOT data types in general.

    The ROOT format is a TMessage holding a TObjArray like:
      TMessage([
        <String holding json representation>,
        <data item 0>,
        <data item 1>, 
        ...
      ])
    If any data items are in Json format, they are converted to TObjStrings.
    """
    def __init__(self,
                 class_name = "",
                 function = "",
                 args = (), 
                 keywd_args = None,
                 data = None):
        """
        Initialise the ControlMessage. id is set as a uuid (universally unique 
        ID) in integer format.
        """
        self.class_name = class_name
        self.function = function
        self.args = []
        for an_arg in args:
            self.args.append(an_arg)
        if keywd_args == None:
            self.keywd_args = {}
        else:
            self.keywd_args = keywd_args
        self.return_value = None
        self.errors = {}
        self.acknowledge = None
        if data == None:
            data = []
        self.data = data
        self.uuid = uuid.uuid4().int

    @classmethod
    def new_from_json_repr(cls, json_repr, data = None):
        """
        Create a new ControlMessage from json format
        @param json_repr the Json formatted object
        @param data list of data. If data == None, it is set to an empty array
        @return the ControlMessage
        """
        ctrl = ControlMessage()
        ctrl.class_name = json_repr["class_name"]
        ctrl.function = json_repr["function"]
        ctrl.args = json_repr["args"]
        ctrl.keywd_args = json_repr["keywd_args"]
        ctrl.return_value = json_repr["return_value"]
        ctrl.errors = json_repr["errors"]
        ctrl.acknowledge = json_repr["acknowledge"]
        ctrl.uuid = json_repr["uuid"]
        if data == None:
            data = []
        ctrl.data = data
        return ctrl

    @classmethod
    def new_from_root_repr(cls, root_message):
        """
        Create a ControlMessage from a ROOT TMessage in Read Mode
        @param root_message ControlMessage parsed into a root_message and then
               sent through a socket to convert it into Read Mode. Not sure what
               Read Mode means, it seems to be an undocumented "Feature" of 
               ROOT. Will attempt to convert TObjStrings to Json format, or die
               trying.
        @return The ControlMessage
        """
        if root_message == None:
            raise TypeError("Message was None")               
        if not (root_message.Class() == ROOT.TMessage().Class() and \
                root_message.GetClass() == ROOT.TObjArray().Class()):
            raise TypeError("Message could not be parsed to a ControlMessage")
        root_array = root_message.ReadObject(root_message.GetClass())
        if root_array.GetEntries() == 0:
            raise TypeError("Message was empty")               
        root_string = root_array[0]
        if root_string.Class() != ROOT.TObjString().Class():
            raise TypeError("First element of message must be a string")
        ascii_string = str(root_string)
        json_doc = json.loads(ascii_string)
        root_data = []
        for i in range(1, root_array.GetEntries()):
            if root_array[i].Class() == ROOT.TObjString().Class():
                json_data = json.loads(str(root_array[i]))
                root_data.append(json_data)
            else:
                root_data.append(root_array[i])
        return ControlMessage.new_from_json_repr(json_doc, root_data)

    def json_repr(self):
        """
        Return the ControlMessage in Json representation. Note that Json 
        repesentation does not support the data object.
        """
        json_repr = {
          "class_name":self.class_name,
          "function":self.function,
          "args":self.args,
          "keywd_args":self.keywd_args,
          "return_value":self.return_value,
          "errors":self.errors,
          "acknowledge":self.acknowledge,
          "uuid":self.uuid
        }
        return json_repr

    def root_repr(self):
        """
        Return the ControlMessage in ROOT representation

        Note that the returned TMessage is in Write mode, whatever that means.
        It needs to be converted to Read mode to be converted back to a
        ControlMessage.
        """
        root_array = ROOT.TObjArray()
        obj_string = ROOT.TObjString(json.dumps(self.json_repr()))
        root_array.Add(obj_string)
        for item in self.data:
            try:
                if type(item) == type([]) or type(item) == type({}):
                    json_str = json.dumps(item)
                    item = ROOT.TObjString(json_str)
                root_array.Add(item)
            except TypeError:
                raise TypeError("Failed to convert "+str(item)+\
                                " to a root-serialisable object")
        my_message = ROOT.TMessage(ROOT.TMessage.kMESS_OBJECT)
        my_message.WriteObject(root_array)
        return my_message

    def __str__(self):
        """
        Return a string representation of the ControlMessage
        """
        return json.dumps(self.json_repr(), indent=2)

