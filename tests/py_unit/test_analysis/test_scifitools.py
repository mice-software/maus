#!/usr/bin/env python

"""
Tests for workers module.
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

import libMausCpp
from ROOT import MAUS as maus
import analysis.scifitools as tools

# SciFiHit = maus.Hit(maus.SciFiChannelId)
SciFiHit = maus.SciFiHit

class SciFiToolsTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for the SciFiLookup python module (not the C++ module)
    """
    def setUp(self): # pylint: disable=C0103

        # Set up some scifihits
        hits = []
        chan_ids = []
        for i in range(7):
            hits.append(SciFiHit())
            chan_ids.append(maus.SciFiChannelId())
            hits[i].SetChannelId(chan_ids[i])
            hits[i].SetMass(i*10.0)


        # Make some recon objects
        self.spoints = []
        self.clusters = []
        self.digits = []
        for i in range(2):
          self.spoints.append(maus.SciFiSpacePoint())
          self.clusters.append(maus.SciFiCluster())
          self.digits.append(maus.SciFiDigit())
          self.digits[i].set_tracker(1)
          self.digits[i].set_station(2)
          self.digits[i].set_plane(0)
          self.digits[i].set_channel(2**i)
          self.clusters[i].add_digit(self.digits[i])
          self.spoints[i].add_channel(self.clusters[i])

        # Make an MC event with the hits
        self.mcevt = maus.MCEvent()
        for i in range(7):
            self.mcevt.AddSciFiHit(hits[i])

        # Make a lookup
        self.lookup = tools.SciFiLookup(self.mcevt)
        dig_id0 = self.lookup.get_digit_id(self.digits[0])
        dig_id1 = self.lookup.get_digit_id(self.digits[1])

        # Let hits 1 and 2 correspond to the digit, but not hit0
        self.mcevt.GetSciFiHits()[0].GetChannelId().SetID(1)
        self.mcevt.GetSciFiHits()[1].GetChannelId().SetID(dig_id0)
        self.mcevt.GetSciFiHits()[2].GetChannelId().SetID(dig_id0)
        self.mcevt.GetSciFiHits()[3].GetChannelId().SetID(dig_id1)
        self.mcevt.GetSciFiHits()[4].GetChannelId().SetID(dig_id1)
        self.mcevt.GetSciFiHits()[5].GetChannelId().SetID(dig_id1)

        # Update the lookup
        self.lookup.make_hits_map(self.mcevt)

    def test_lookup_get_hits(self):
        """
        Test the main functionality of the scifi lookup for hits
        """
        hits = self.lookup.get_hits(self.digits[0])
        dig_id = self.lookup.get_digit_id(self.digits[0])

        self.assertEqual(2, len(hits))
        self.assertEqual(dig_id, int(hits[0].GetChannelId().GetID()))
        self.assertEqual(10.0, hits[0].GetMass())
        self.assertEqual(dig_id, int(hits[1].GetChannelId().GetID()))
        self.assertEqual(20.0, hits[1].GetMass())

    def test_find_mc_hits(self):
        """ Test the find_mc_hits function """
        

if __name__ == '__main__':
    unittest.main()

