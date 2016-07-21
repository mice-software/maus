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
import ROOT
import ROOT.MAUS as m
from analysis.scifi_lookup import SciFiLookup

class SciFiLookupTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for the SciFiLookup python module (not the C++ module)
    """
    def test_get_hits(self):
        """
        Test the main functionality of the lookup
        """

        # Set up some scifihits
        hit0 = m.SciFiHit()
        hit1 = m.SciFiHit()
        hit2 = m.SciFiHit()

        chan_id0 = m.SciFiChannelId()
        chan_id1 = m.SciFiChannelId()
        chan_id2 = m.SciFiChannelId()

        hit0.SetChannelId(chan_id0)
        hit1.SetChannelId(chan_id1)
        hit2.SetChannelId(chan_id2)

        # Set masses just so we can tell which hits are which later
        hit0.SetMass(0.0)
        hit1.SetMass(10.0)
        hit2.SetMass(20.0)

        # Set up a scifi digit with digit ID 132064.0
        dig1 = m.SciFiDigit()
        dig1.set_channel(64)
        dig1.set_plane(2)
        dig1.set_station(3)
        dig1.set_tracker(1)

        # Set up the lookup and use it calculate the digit id
        lk = SciFiLookup()
        dig_id = lk.get_digit_id(dig1)

        # Let hits 2 and 3 correspond to the digit, but not hit1
        hit0.GetChannelId().SetID(1)
        hit1.GetChannelId().SetID(dig_id)
        hit2.GetChannelId().SetID(dig_id)

        # Make an MC event with the hits
        mcevt = m.MCEvent()
        mcevt.AddSciFiHit(hit0)
        mcevt.AddSciFiHit(hit1)
        mcevt.AddSciFiHit(hit2)

        # See if the lookup correctly finds hits 2 and 3 but not 1
        lk.make_hits_map(mcevt)
        hits = lk.get_hits(dig1)

        self.assertEqual(2, len(hits))
        self.assertEqual(dig_id, int(hits[0].GetChannelId().GetID()))
        self.assertEqual(10.0, hits[0].GetMass())
        self.assertEqual(dig_id, int(hits[1].GetChannelId().GetID()))
        self.assertEqual(20.0, hits[1].GetMass())

if __name__ == '__main__':
    unittest.main()

