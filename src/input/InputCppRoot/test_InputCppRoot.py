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

import unittest
import json
import ROOT
import os

import InputCppRoot

class TestInputCppRoot(unittest.TestCase): # pylint: disable=R0904
    """
    Test we can write out ROOT tree.

    Check we can birth and death correctly; check we can write simple ROOT trees
    especially in context of - how are errors handled - 
    """
    @classmethod
    def setUpClass(self): # pylint: disable=C0103, C0202
        """
        Make a sample TFile
        """
        mrd = os.environ['MAUS_ROOT_DIR']
        self.fname = os.path.join(mrd, "tmp", "TestInputCppRoot.root")
        tfile = ROOT.TFile(self.fname, "RECREATE") # pylint: disable=E1101
        ttree = ROOT.TTree("Data", "treetitle") # pylint: disable=E1101
        mc_events = ROOT.MAUS.MCEventArray()
        for i in range(3):
            mc_events.push_back(ROOT.MAUS.MCEvent())
        self.spill = ROOT.MAUS.Spill()
        self.spill.SetScalars(ROOT.MAUS.Scalars())
        self.spill.SetEMRSpillData(ROOT.MAUS.EMRSpillData())
        self.spill.SetDAQData(ROOT.MAUS.DAQData())
        self.spill.SetMCEventArray(mc_events)
        self.spill.SetReconEventArray(ROOT.MAUS.ReconEventArray())
        ttree.Branch('spill', self.spill, 'spill')
        ttree.Fill()
        ttree.Fill()
        ttree.Fill()
        ttree.Write()
        tfile.Close()

    def test_birth_death(self):
        """
        Check that we can birth and death properly
        """
        inputter = InputCppRoot.InputCppRoot()
        inputter.birth(json.dumps({"root_input_filename":self.fname}))
        inputter.death()
        inputter.death()
        inputter.birth(json.dumps({"root_input_filename":self.fname}))
        inputter.death()

        inputter_2 = InputCppRoot.InputCppRoot(self.fname)
        inputter_2.death()

        inputter_3 = InputCppRoot.InputCppRoot()
        inputter_3.death()

    def test_read_normal_event(self):
        """
        Try saving a few standard events
        """
        print "init"
        inputter = InputCppRoot.InputCppRoot(self.fname)
        inputter.birth("{}")
        print "get next event"
        print inputter.getNextEvent()
        print "emitter"
        for item in inputter.emitter():
            print "emitted"
            print item
            print type(item)
        

    def test_read_bad_event(self):
        """
        Check that if passed a bad event, code fails gracefully
        """
        pass

if __name__ == "__main__":
    unittest.main()
