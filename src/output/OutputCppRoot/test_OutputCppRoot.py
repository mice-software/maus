#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

# pylint: disable=C0103

"""
Test for OutputCppRoot
"""

import os
import unittest
import json
import OutputCppRoot

class TestOutputCppRoot(unittest.TestCase): # pylint: disable=R0904
    """
    Test we can write out ROOT tree.

    Check we can birth and death correctly; check we can write simple ROOT trees
    especially in context of - how are errors handled - 
    """

    def setUp(self): # pylint: disable=C0103, C0202
        """
        Define cards and initialise Output
        """
        self.output = OutputCppRoot.OutputCppRoot()
        outfile = os.path.join \
                 (os.environ["MAUS_ROOT_DIR"], "tmp", "test_outputCppRoot.root")
        self.cards = json.dumps({"output_root_file_name":outfile})
        self.output.birth(self.cards)
        self.test_data = {
            "scalars":{},
            "emr_spill_data":{},
            "daq_data":{},
            "spill_number":1,
            "recon_events":[],
            "mc_events":[]
        }

    def test_birth_death(self):
        """
        Check that we can birth and death properly
        """
        an_output = OutputCppRoot.OutputCppRoot()
        self.assertFalse(an_output.birth('{}'))
        self.assertFalse(an_output.birth('{"output_root_file_name":""}'))
        self.assertTrue(an_output.birth(self.cards))
        self.assertTrue(an_output.death())

    def test_save_normal_event(self):
        """
        Try saving a few standard events
        """
        self.assertTrue(self.output.save(
            json.dumps(self.test_data)
        ))
        self.assertTrue(self.output.save(
            json.dumps(self.test_data)
        ))
        self.assertTrue(self.output.save(
            json.dumps(self.test_data)
        ))

    def test_save_bad_event(self):
        """
        Check that if passed a bad event, code fails gracefully
        """
        self.assertFalse(self.output.save(
                      json.dumps({"no_branch":{}})
        ))
        self.assertFalse(self.output.save(''))
        self.output.death()

if __name__ == "__main__":
    unittest.main()


