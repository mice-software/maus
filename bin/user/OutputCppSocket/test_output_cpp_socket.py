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
Tests for OutputCppSocket
"""
import unittest
import signal
import sys
import subprocess
import time
import os
import json
import ROOT
import xboa.Common as Common
from docstore.root_document_store import RootDocumentStore
from docstore.root_document_store import RootDocumentDB

import MAUS

def get_image_data():
    my_image = ROOT.MAUS.ImageData()

NEXT_PORT = 39900

class OutputCppSocketTest(unittest.TestCase):
    def setUp(self):
        """Setup port"""
        global NEXT_PORT
        self.port = NEXT_PORT
        NEXT_PORT += 10

        self.json_doc = {
            "output_cpp_socket_host":"localhost",
            "output_cpp_socket_port":self.port,
            "output_cpp_socket_collection":"ocs_coll",
            "root_document_store_timeout":0.1,
            "root_document_store_poll_time":0.01
        }

    def test_output_cpp_socket_birth(self):
        """Test OutputCppSocket::birth()"""
        rd_db = RootDocumentDB([self.port+2, self.port+1, self.port], 0.01)
        ocs_1 = MAUS.OutputCppSocket()
        ocs_1.birth(json.dumps(self.json_doc))
        rds = RootDocumentStore(10., 0.01)
        rds.connect({"host":"localhost", "port":39901})
        self.assertTrue(rds.has_collection("ocs_coll"))
        # collection already exists
        self.json_doc["output_cpp_socket_port"] = self.port+2
        ocs_2 = MAUS.OutputCppSocket()
        ocs_2.birth(json.dumps(self.json_doc))
        # cant find root document DB (incorrect port)
        self.json_doc["output_cpp_socket_port"] = self.port+3
        ocs_3 = MAUS.OutputCppSocket()
        ocs_3.birth(json.dumps(self.json_doc))
        # birth twice should raise exception
        ocs_1.birth(json.dumps(self.json_doc))
        # death then birth should be okay
        ocs_1.death()
        ocs_1.birth(json.dumps(self.json_doc))

    def test_output_cpp_socket_save(self):
        """Test OutputCppSocket::save()"""
        rd_db = RootDocumentDB([self.port], 0.01)
        ocs = MAUS.OutputCppSocket()
        ocs.birth(json.dumps(self.json_doc))
        ocs.save(get_image_data())
        ocs.save(get_image_data())
        ocs.save(get_image_data())
        # bad type, should throw (but what?)
        ocs.save(0)

    def test_output_cpp_socket_end_of_run(self):
        raise NotImplementedError("Not implemented")

if __name__ == "__main__":
    unittest.main()

