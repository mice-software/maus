#!/usr/bin/env python

"""
Tests for the SciFiLookup python module (not the C++ module)
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

#pylint: disable = W0611, E0611, R0915, C0103
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

        # Make an MC event
        self.mcevt = maus.MCEvent()

        # Set up some mc tracks and add to the mc event
        mctracks = []
        for i in range(2):
            mctracks.append(maus.Track())
            mctracks[i].SetTrackId(i)
            self.mcevt.AddTrack(mctracks[i])

        # Set up some scifihits and add to the mc event
        hits = []
        mom0 = maus.ThreeVector(0.0, 0.0, 0.0)
        mom1 = maus.ThreeVector(1.0, 2.0, 3.0)
        mom2 = maus.ThreeVector(4.0, 5.0, 6.0)
        chan_ids = []
        for i in range(7):
            hits.append(SciFiHit())
            chan_ids.append(maus.SciFiChannelId())
            hits[i].SetChannelId(chan_ids[i])
            hits[i].SetMass(i*10.0)
            # Associate hits 1 - 3 with track 0, hits 4 - 6 with track 1,
            # and hit0 with no existing track
            if i > 0 and i < 4:
                hits[i].SetTrackId(0)
                hits[i].SetMomentum(mom1)
            elif i > 3:
                hits[i].SetTrackId(1)
                hits[i].SetMomentum(mom2)
            else:
                hits[i].SetTrackId(-1)
                hits[i].SetMomentum(mom0)
            self.mcevt.AddSciFiHit(hits[i])

        # Make some recon objects. Set station=1 plane=0 i.e. ref surface
        self.spoints = []
        self.clusters = []
        self.digits = []
        for i in range(2):
            self.spoints.append(maus.SciFiSpacePoint())
            self.clusters.append(maus.SciFiCluster())
            self.digits.append(maus.SciFiDigit())
            self.digits[i].set_tracker(i)
            self.digits[i].set_station(5)
            self.digits[i].set_plane(0)
            self.digits[i].set_channel(2**i)
            self.clusters[i].add_digit(self.digits[i])
            self.clusters[i].set_tracker(i)
            self.clusters[i].set_station(1)
            self.clusters[i].set_plane(0)
            self.spoints[i].add_channel(self.clusters[i])
            self.spoints[i].set_tracker(i)
            self.spoints[i].set_station(1)

        # Set the digit ids
        dig_id0 = tools.SciFiLookup.get_digit_id(self.digits[0])
        dig_id1 = tools.SciFiLookup.get_digit_id(self.digits[1])

        # Let hits 1 to 3 correspond to digit 0, hits 4 - 6 to digit 1
        # and hit 0 to no digit at all
        self.mcevt.GetSciFiHits()[0].GetChannelId().SetID(0)
        self.mcevt.GetSciFiHits()[1].GetChannelId().SetID(dig_id0)
        self.mcevt.GetSciFiHits()[2].GetChannelId().SetID(dig_id0)
        self.mcevt.GetSciFiHits()[3].GetChannelId().SetID(dig_id0)
        self.mcevt.GetSciFiHits()[4].GetChannelId().SetID(dig_id1)
        self.mcevt.GetSciFiHits()[5].GetChannelId().SetID(dig_id1)
        self.mcevt.GetSciFiHits()[6].GetChannelId().SetID(dig_id1)

        # Use the lookup
        self.lookup = tools.SciFiLookup(self.mcevt)
        # self.lookup.make_hits_map(self.mcevt)

    def test_lookup_get_hits(self):
        """
        Test the main functionality of the scifi lookup for hits
        """
        hits = self.lookup.get_hits(self.digits[0])
        dig_id = self.lookup.get_digit_id(self.digits[0])

        self.assertEqual(3, len(hits))
        self.assertEqual(dig_id, int(hits[0].GetChannelId().GetID()))
        self.assertEqual(10.0, hits[0].GetMass())
        self.assertEqual(dig_id, int(hits[1].GetChannelId().GetID()))
        self.assertEqual(20.0, hits[1].GetMass())
        self.assertEqual(dig_id, int(hits[2].GetChannelId().GetID()))
        self.assertEqual(30.0, hits[2].GetMass())

    def test_find_mc_hits(self):
        """ Test the find_mc_hits function """
        # Check the first spacepoint (which is in tracker 0)
        hits = tools.find_mc_hits(self.lookup, self.spoints, \
          plane=-1, station=-1, tracker=0)
        self.assertEqual(3, len(hits))
        self.assertEqual(10.0, hits[0].GetMass())
        self.assertEqual(20.0, hits[1].GetMass())
        self.assertEqual(30.0, hits[2].GetMass())
        # Check the other spacepoint (which is in tracker 1)
        hits = tools.find_mc_hits(self.lookup, self.spoints, \
          plane=-1, station=-1, tracker=1)
        self.assertEqual(3, len(hits))
        self.assertEqual(40.0, hits[0].GetMass())
        self.assertEqual(50.0, hits[1].GetMass())
        self.assertEqual(60.0, hits[2].GetMass())

    def test_find_mc_track(self):
        """ Test the find_mc_track function """
        # Pull out the mc tracks and hits
        all_hits = tools.vector_to_list(self.mcevt.GetSciFiHits())

        # Select first 4 hits, 3 of which match track id 0
        hits = all_hits[:4]
        track_id = tools.find_mc_track(hits)
        self.assertEqual(0, track_id)

        # Select the last 3 hits, all of which should match track id 1
        hits = all_hits[-3:]
        track_id = tools.find_mc_track(hits)
        self.assertEqual(1, track_id)

        # Now choose 1 hit from each track - no track should then be selected
        hits = []
        hits.append(all_hits[1])
        hits.append(all_hits[4])
        track = tools.find_mc_track(hits)
        self.assertFalse(track)

    def test_find_mc_momentum_sfhits(self):
        """ Test finding the mc momentum of some sf hits via the lookup """
        mom = \
          tools.find_mc_momentum_sfhits(self.lookup, [self.spoints[0]], 0, 0)
        self.assertEqual(1.0, mom[0])
        self.assertEqual(2.0, mom[1])
        self.assertEqual(3.0, mom[2])
        mom = \
          tools.find_mc_momentum_sfhits(self.lookup, [self.spoints[1]], 1, 1)
        self.assertEqual(4.0, mom[0])
        self.assertEqual(5.0, mom[1])
        self.assertEqual(6.0, mom[2])

    def load_data(files):
        """ Load data from files. If a dir is given, search recursively """
        if type(files) is not list:
            files = [files]

        root_files = []
        for file_name in files:
            # Check if file_name is a ROOT file
            if os.path.isfile(file_name):
                file_suffix, file_extension = os.path.splitext(file_name)
                if file_extension == '.root':
                    root_files.append(file_name)
                else:
                    print 'Bad file name, aborting'
                    return root_files

            # If file_name is a directory, walk it and save any ROOT files found
            if os.path.isdir(file_name):
                tools.root_files_dir_search(file_name, root_files)
            if len(root_files) < 1:
                print 'No data files found'
                return root_files

        print '\nFound ' + str(len(root_files)) + ' ROOT files:'
        for f in root_files:
            print f
        return root_files

if __name__ == '__main__':
    unittest.main()

