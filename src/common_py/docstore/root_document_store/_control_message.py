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
import uuid
import ROOT

class ControlMessage:
    def __init__(self, class_name = "", function = "", args = (),
                 keywd_args = {}, data_tree = None):
        self.class_name = class_name
        self.function = function
        self.args = []
        for an_arg in args:
            self.args.append(an_arg)
        self.keywd_args = keywd_args
        self.return_value = None
        self.errors = {}
        self.acknowledge = None
        if data_tree == None:
            data_tree = ROOT.TTree()
        self.data_tree = data_tree
        self.id = uuid.uuid4().int

    @classmethod
    def new_from_json_repr(cls, json_repr, data_tree = None):
        ctrl = ControlMessage()
        ctrl.class_name = json_repr["class_name"]
        ctrl.function = json_repr["function"]
        ctrl.args = json_repr["args"]
        ctrl.keywd_args = json_repr["keywd_args"]
        ctrl.return_value = json_repr["return_value"]
        ctrl.errors = json_repr["errors"]
        ctrl.acknowledge = json_repr["acknowledge"]
        ctrl.id = json_repr["id"]
        if data_tree == None:
            data_tree = ROOT.TTree()
        ctrl.data_tree = data_tree
        return ctrl

    @classmethod
    def new_from_root_repr(cls, root_message):
        if root_message == None:
            raise TypeError("Message was None")               
        if root_message.Class() != ROOT.TMessage().Class() and \
           root_message.GetClass() != ROOT.TTree().Class():
            raise TypeError("Message could not be parsed to a ControlMessage")               
        root_tree = root_message.ReadObject(root_message.GetClass())
        root_string = ROOT.TObjString()
        root_tree.SetBranchAddress("ControlMessage", root_string)
        if root_tree.GetEntries() > 0:
            root_tree.GetEntry(0)
        else:
            raise TypeError("Message was empty")               
        ascii_string = str(root_string.GetString())
        json_doc = json.loads(ascii_string)
        # now remove ControlMessage branch from the root tree
        branch = root_tree.GetBranch("ControlMessage")
        root_tree.GetListOfBranches().Remove(branch)
        return ControlMessage.new_from_json_repr(json_doc, root_tree)

    def __eq__(self, other):
        pass

    def json_repr(self):
        json_repr = {
          "class_name":self.class_name,
          "function":self.function,
          "args":self.args,
          "keywd_args":self.keywd_args,
          "return_value":self.return_value,
          "errors":self.errors,
          "acknowledge":self.acknowledge,
          "id":self.id
        }
        return json_repr

    def root_repr(self):
        print "ROOT_REPR"
        for an_object in self.data_tree.GetListOfBranches():
            print an_object
        if self.data_tree.FindBranch("ControlMessage"):
            branch = self.data_tree.GetBranch()
            self.data_tree.GetListOfBranches().Remove(branch)
        obj_string = ROOT.TObjString(json.dumps(self.json_repr()))
        self.data_tree.Branch("ControlMessage", obj_string.ClassName(),
                              obj_string)
        self.data_tree.Fill()
        for an_object in self.data_tree.GetListOfBranches():
            print an_object, an_object.GetName()
        return self.data_tree

    def __str__(self):
        return json.dumps(self.json_repr(), indent=2)

