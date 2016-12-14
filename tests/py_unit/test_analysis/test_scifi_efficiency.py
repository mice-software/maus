#!/usr/bin/env python

"""
Tests for the scifi_efficiency python module
"""

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

import unittest

#pylint: disable = W0611, E0611, C0103
import libMausCpp
from ROOT import MAUS as maus
import analysis.scifi_efficiency as scifi_efficiency

class SciFiEfficiencyTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for the scifi_efficiency python module
    """
    def test_process_spill(self):
        """ Test calculating the efficiency for one spill """
        spill = maus.Spill()
        spill.SetSpillNumber(1)
        spill.SetDaqEventType('physics_event')
        revts = maus.ReconEventPArray()
        revts.push_back(maus.ReconEvent())
        sfevt = maus.SciFiEvent()

        # Set up tracker spacepoints
        spoints = maus.SciFiSpacePointPArray()
        spoints_us = maus.SciFiSpacePointPArray()
        spoints_ds = maus.SciFiSpacePointPArray()
        for i in range(5):
            sp = maus.SciFiSpacePoint()
            sp.set_tracker(0)
            sp.set_station(i+1)
            spoints.push_back(sp)
            spoints_us.push_back(sp)
        for i in range(5):
            sp = maus.SciFiSpacePoint()
            sp.set_tracker(1)
            sp.set_station(i+1)
            spoints.push_back(sp)
            spoints_ds.push_back(sp)
        sfevt.set_spacepoints(spoints)

        # Set up tracks
        for i in range(2):
            trk = maus.SciFiStraightPRTrack()
            trk.set_tracker(i)
            if i == 0:
                trk.set_spacepoints_pointers(spoints_us)
            else:
                trk.set_spacepoints_pointers(spoints_ds)
            sfevt.add_straightprtrack(trk)

        # Add the tracks and spacepoints to the spill
        revts.at(0).SetSciFiEvent(sfevt)
        spill.SetReconEvents(revts)

        # Set up and run the pat rec efficiency class
        eff = scifi_efficiency.PatternRecognitionEfficiencyReal()
        eff.check_helical = False
        eff.check_straight = True
        eff.cut_on_tof = False
        eff.cut_on_tof_time = False
        eff.cut_on_tracker_10spnt = True
        eff.process_spill(spill)
        eff.fdata.calculate_efficiency()

        # Check the results
        self.assertEqual(1.0, eff.fdata.eff_tkus_5pt)
        self.assertEqual(1.0, eff.fdata.eff_tkus_3_5pt)
        self.assertEqual(1.0, eff.fdata.eff_tkds_5pt)
        self.assertEqual(1.0, eff.fdata.eff_tkds_3_5pt)

if __name__ == '__main__':
    unittest.main()