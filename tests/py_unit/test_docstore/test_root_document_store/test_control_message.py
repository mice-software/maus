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
Tests for control message
"""

import json
import unittest

from docstore.root_document_store import ControlMessage

class ControlMessageTest(unittest.TestCase): # pylint: disable=R0904
    """
    Tests for the ControlMessage
    """
    def setUp(self): # pylint: disable=C0103
        """
        Setup the ControlMessage
        """
        self.json_doc = {"class_name":"class", "function":"fnc", 
                    "args":[1, 2, 3],
                    "keywd_args":{"1":1, "2":2}, "return_value":True, 
                    "errors":{"error":"test"}, "acknowledge":True, "id":1}

    def test_init(self):
        """
        Test ControlMessage initialisation
        """
        ctrl = ControlMessage()
        self.assertEqual(ctrl.class_name, "")
        self.assertEqual(ctrl.function, "")
        self.assertEqual(ctrl.args, [])
        self.assertEqual(ctrl.keywd_args, {})
        self.assertEqual(ctrl.return_value, None)
        self.assertEqual(ctrl.errors, {})
        self.assertEqual(ctrl.acknowledge, None)
        self.assertEqual(ctrl.data, [])

        ctrl = ControlMessage("class", "fnc", [1, 2, 3], {"1":1, "2":2},
                              ["a", "b"])
        self.assertEqual(ctrl.class_name, "class")
        self.assertEqual(ctrl.function, "fnc")
        self.assertEqual(ctrl.args, [1, 2, 3])
        self.assertEqual(ctrl.keywd_args, {"1":1, "2":2})
        self.assertEqual(ctrl.return_value, None)
        self.assertEqual(ctrl.errors, {})
        self.assertEqual(ctrl.acknowledge, None)
        self.assertEqual(ctrl.data, ["a", "b"])

    def test_json_repr(self):
        """
        Test ControlMessage json format (new_from_json_repr and json_repr
        """
        ctrl = ControlMessage.new_from_json_repr(self.json_doc, ["a", "b"])
        json_test = ctrl.json_repr()
        err = json.dumps(json_test, indent=2)+"\n\n"+json.dumps(self.json_doc, indent=2)
        self.assertEqual(json_test, self.json_doc, msg=err)

    def test_new_from_root(self):
        """
        Check that we can create a ROOT TMessage from ControlMessage

        Due to the usual obscure, undocumented behaviour of ROOT, I can't parse
        a ROOT TMessage back into a ControlMessage as the TMessage is in "Write
        mode" and there doesnt seem to be any way to convert to "Read mode"
        safely without sending the message. 
        """
        ctrl_ref = ControlMessage.new_from_json_repr(self.json_doc, [])
        ctrl_ref.root_repr()


if __name__ == "__main__":
    unittest.main()

