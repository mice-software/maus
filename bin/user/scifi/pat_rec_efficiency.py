#!/usr/bin/env python

"""  Check tracker efficiency """

import sys
import os
import math
import ROOT
import libMausCpp #pylint: disable = W0611
import analysis.scifitools as tools

#pylint: disable = R0902
#pylint: disable = R0912
#pylint: disable = R0914
#pylint: disable = R0915
#pylint: disable = C0103
#pylint: disable = W0612

class PatRecEfficiency():
    """ Class to check Pattern Recognition efficiency with real data """

    def __init__(self):
        """ Initialise member variables """

        # Options for the user
        self.check_helical = False
        self.check_straight = True
        self.cut_on_tof = True
        self.cut_on_tof_time = True
        self.cut_on_trackers = True
        self.use_mc_truth = False

        self.tof_upper_cut = 50.0
        self.tof_lower_cut = 27.0
        self.fiducial_cut = 150.0
        self.id_frequency_cut = 0.5

        # ROOT objects
        self.root_files = []
        self.pt_hist = ROOT.TH1D("pt_hist", "pt", 100, -100.0, 100.0);
        self.pz_hist = ROOT.TH1D("pz_hist", "pz", 100, -100.0, 100.0);

        # Results
        self.bool_2tof_timing_event = False # Tof timing ok
        self.bool_2tof_spoint_event = False # 1 spacepoint only in each tof
        self.bool_10spoint_event = False # Expect one 5pt track in each trcker
        self.bool_tkus_5spoint_event = False # Expect one 5pt track in TkUS
        self.bool_tkds_5spoint_event = False # Expect one 5pt track in TkDS
        self.bool_tkus_1track = False # Found one track in TkUS
        self.bool_tkds_1track = False # Found one track in TkDS
        self.bool_tkus_5spoint_track = False # Found one 5pt track in TkUS
        self.bool_tkds_5spoint_track = False # Found one 5pt track in TkDS

        self.num_total_events = 0
        self.num_tof_good_events = 0
        self.num_tof_spoint_good_events = 0
        self.num_10spoint_events = 0
        self.num_5spoint_tkus_events = 0
        self.num_5spoint_tkds_events = 0

        self.num_10spoint_tracks = 0
        self.num_5spoint_tkus_tracks = 0
        self.num_5spoint_tkds_tracks = 0
        self.num_3to5spoint_tkus_tracks = 0
        self.num_3to5spoint_tkds_tracks = 0

    def run(self, files):
        """ Calculate the Pattern Recognition efficiency for the given files """
        print '\nPattern Recognition Efficiency Calculator'
        print '*****************************************\n'

        print 'Parameters:'
        print 'Check helical\t' + str(self.check_helical)
        print 'Check straight\t' + str(self.check_straight)
        print 'Cut on TOF\t' + str(self.cut_on_tof)
        print 'Cut on TOF Time\t' + str(self.cut_on_tof_time)
        # print 'Cut on fiducial track\t' + str(self.cut_on_fiducial_track) 
        print 'Cut on trackers\t' + str(self.cut_on_trackers) + '\n'

        self.root_files = self.load_data(files)
        if len(self.root_files) < 1:
            return False

        print '\nFile\t\t\t#_Recon_Events\tTOF\tTOF_SP\tTkUS_5pt\tTkUS_5pt_Err\
          \tTkUS_3-5_pt\tTkUS_3-5_pt_Err\t',
        print 'TkDS_5pt\tTkDS_5_pt_Err\tTkDS_3-5pt\tTkDS_3-5_pt_Err'
        for root_file_name in self.root_files:
            self.calculate_efficiency(root_file_name)
            self.print_file_info(root_file_name)
            if self.use_mc_truth:
                self.make_plots(root_file_name)
        return True

    def calculate_efficiency(self, root_file_name):
        """ Calculate the Pattern Recognition efficiency for the input file """
        self.clear() # Start clean each time

        # Load the ROOT file
        root_file = ROOT.TFile(root_file_name, "READ") # pylint: disable = E1101
        tree = root_file.Get("Spill")
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree.SetBranchAddress("data", data)

        # Loop over spills
        for i in range(tree.GetEntries()):
            tree.GetEntry(i)
            spill = data.GetSpill()
            if spill.GetDaqEventType() != "physics_event":
                continue

            # Loop over recon events
            for i in range(spill.GetReconEvents().size()):
                self.num_total_events += 1

                # Pull out tof data and check if cuts pass
                tof_evt = spill.GetReconEvents()[i].GetTOFEvent()
                tof_good = self.check_tof(tof_evt)
                if not tof_good:
                    continue # remove event from consideration

                # Pull out tracker data and examine
                tk_evt = spill.GetReconEvents()[i].GetSciFiEvent()

                # Look at tracker spacepoint data to see if we expect a track/s
                tracker_spoints_good = \
                  self.check_tracker_spacepoints(tk_evt.spacepoints())
                if not tracker_spoints_good:
                    continue # remove event from consideration

                # Now switch from calculating expected tracks to what
                # was actually reconstructed

                # Extract the desired pat rec tracks (straight, helical or both)
                prtracks = self.extract_prtracks(tk_evt)

                # Is there at least 1 track present in either tracker
                if len(prtracks) < 1:
                    continue # remove event from consideration

                # Analyse the pat rec tracks and see what we actually found
                self.analyse_prtracks(prtracks)

        # Close the ROOT file
        root_file.Close()

    def analyse_mc_truth_momentum(self, mc_evt, tk_spoints, trker_num):
        """ Find the mc scifi hits that formed the spacepoints, find the
            parent primary mc track id, extract the mc truth momentum from
            scifi hits corresponding to that track """

        # Check the mc event is ok
        if mc_evt is None:
          return ()

        # Create a lookup to find the mc scifi hits that formed the spacepoints
        lookup = tools.SciFiLookup(mc_evt) # Lookup to link rec to MC
        if len(lookup.hits_map) < 0:
          return ()

        # Try to find the mc primary track
        hits = tools.find_mc_hits(lookup, tk_spoints, trker_num)
        parent_track_id = tools.find_mc_track(hits, self.id_frequency_cut)
        if parent_track_id != 0:
            # Pull out the mc momentum and fill the histogramms
            mom = tools.find_mc_momentum_sfhits(lookup, tk_spoints, \
              parent_track_id, trker_num)
            return mom
        return ()

    def analyse_prtracks(self, prtracks):
        """ Analyse the reconstructed tracks, see how many were found when
            they were expected """

        # Require 1 and only 1 track in a tracker
        num_tkus_tracks = 0
        num_tkds_tracks = 0
        for trk in prtracks:
            if trk.get_tracker() == 0:
                num_tkus_tracks += 1
            elif trk.get_tracker() == 1:
                num_tkds_tracks += 1
        if num_tkus_tracks == 1:
            self.bool_tkus_1track = True
        if num_tkds_tracks == 1:
            self.bool_tkds_1track = True

        # If there were 5 spacepoints in a tracker and 1 track only was
        # found in it, increment the 3 to 5 spoint track counter
        if self.bool_tkus_5spoint_event and self.bool_tkus_1track:
            self.num_3to5spoint_tkus_tracks += 1
        if self.bool_tkds_5spoint_event and self.bool_tkds_1track:
            self.num_3to5spoint_tkds_tracks += 1

        # Now check the tracks found had 5 spoints, 1 from each station
        self.bool_tkus_5spoint_track = False
        self.bool_tkds_5spoint_track = False
        for trk in prtracks:
            if trk.get_tracker() == 0:
                if (len(trk.get_spacepoints()) == 5) and self.bool_tkus_1track:
                    self.num_5spoint_tkus_tracks += 1
                    self.bool_tkus_5spoint_track = True
            elif trk.get_tracker() == 1:
                if (len(trk.get_spacepoints()) == 5) and self.bool_tkds_1track:
                    self.num_5spoint_tkds_tracks += 1
                    self.bool_tkds_5spoint_track = True

        # Lastly see if we found one 5 spoint track in BOTH trackers
        if (len(prtracks) == 2 and self.bool_tkus_5spoint_track and \
            self.bool_tkds_5spoint_track):
            self.num_10spoint_tracks += 1
        return True

    def check_tof(self, tof_evt):
        """ Analyse tof data. Return boolean indicating if tof cuts pass"""
        tof1 = tof_evt.GetTOFEventSpacePoint().GetTOF1SpacePointArray()
        tof2 = tof_evt.GetTOFEventSpacePoint().GetTOF2SpacePointArray()

        # Require 1 and only 1 sp in both TOF1 and TOF2
        self.bool_2tof_spoint_event = True
        if ((len(tof1) != 1) or (len(tof2) != 1)):
            self.bool_2tof_spoint_event = False

        # Require timing coincidence between TOF1 and TOF2
        self.bool_2tof_timing_event = True
        for j in range(tof1.size()):
            if tof1.size() == 1 and tof2.size() == 1:
                tof_time_1 = tof1[j].GetTime()
                tof_time_2 = tof2[j].GetTime()
                if (tof_time_2 - tof_time_1 > self.tof_upper_cut) \
                  or (tof_time_2 - tof_time_1 < self.tof_lower_cut):
                    self.bool_2tof_timing_event = False

        # Are the cuts choosen passed?
        tof_good = True
        if self.bool_2tof_spoint_event:
            self.num_tof_spoint_good_events += 1
        elif self.cut_on_tof:
            tof_good = False

        if self.bool_2tof_timing_event:
            self.num_tof_good_events += 1
        elif self.cut_on_tof_time:
            tof_good = False

        return tof_good

    def check_tracker_fiducial(self, straight_track):
        """ Project a straight track for TkUS to TkDS and make sure it falls
            in the fiducial volume """
        track_ok = False
        if track.get_tracker() == 0:
            # Project to downstream:
            z_loc = -3800
            x = track.get_x0() + z_loc*track.get_mx()
            y = track.get_y0() + z_loc*track.get_my()
            if ((x*x+y*y) ** 0.5) < self.fiducial_cut:
                track_ok = True
        return track_ok

    def check_tracker_spacepoints(self, spoints):
        """ Look for 5 spacepoint events in each tracker, increment the internal
            good event counters, and return if the tracker spacepoint data
            passes the selected cuts """
        self.bool_10spoint_event = True
        self.bool_tkus_5spoint_event = True
        self.bool_tkds_5spoint_event = True

        # Loop over trackers
        for i in range(2):
            tracker = [sp for sp in spoints if sp.get_tracker() == i]
            # Loop over spacepoints
            for j in range(1, 6):
                station = \
                  [sp for sp in tracker if sp.get_station() == j]
                # Does each station have one and only one sp
                if len(station) != 1:
                    self.bool_10spoint_event = False
                    if i == 0:
                        self.bool_tkus_5spoint_event = False
                    elif i == 1:
                        self.bool_tkds_5spoint_event = False

        # Update the internal expected tracks counters
        if self.bool_10spoint_event:
            self.num_10spoint_events += 1
        if self.bool_tkus_5spoint_event:
            self.num_5spoint_tkus_events += 1
        if self.bool_tkds_5spoint_event:
            self.num_5spoint_tkds_events += 1

        # Is the tracker spacepoint cut passed?
        tracker_spoints_good = True
        if self.cut_on_trackers and not self.bool_10spoint_event:
            tracker_spoints_good = True
        return tracker_spoints_good

    def clear(self):
        """ Set the internal counters to zero and booleans to false """
        self.bool_2tof_timing_event = False
        self.bool_2tof_spoint_event = False
        self.bool_10spoint_event = False
        self.bool_tkus_5spoint_event = False
        self.bool_tkds_5spoint_event = False
        self.bool_tkus_1track = False
        self.bool_tkds_1track = False
        self.bool_tkus_5spoint_track = False
        self.bool_tkds_5spoint_track = False

        self.num_total_events = 0
        self.num_10spoint_events = 0
        self.num_5spoint_tkus_events = 0
        self.num_5spoint_tkds_events = 0
        self.num_10spoint_tracks = 0
        self.num_5spoint_tkus_tracks = 0
        self.num_5spoint_tkds_tracks = 0
        self.num_3to5spoint_tkus_tracks = 0
        self.num_3to5spoint_tkds_tracks = 0

    def extract_prtracks(self, tk_evt):
        """ Pull out the pattern recognition tracks selected for analysis -
            either straight, helical or both """
        prtracks = []
        if (self.check_helical) and (not self.check_straight):
            prtracks = tk_evt.helicalprtracks()
            # print 'Looking at helical tracks'
        elif (not self.check_helical) and (self.check_straight):
            prtracks = tk_evt.straightprtracks()
            # print 'Looking at straight tracks'
        elif (self.check_helical) and (self.check_straight):
            prtracks.append(tk_evt.helicalprtracks())
            prtracks.append(tk_evt.straightprtracks())
        else:
            print 'Warning: Both track type options not set'
        return prtracks

    def load_data(self, files):
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

    def make_plots(self, file_name_prefix):
        """ Draw the histogramms and save """
        c1 = ROOT.TCanvas()
        c1.Divide(2)
        c1.cd(1)
        self.pt_hist.Draw()
        c1.cd(2)
        self.pz_hist.Draw()
        c1.SaveAs(file_name_prefix + "_momentum_failed_events.pdf")

    def print_file_info(self, root_file_name):
        """ Calculate the efficiencies and print """
        try:
            us_5pt = float(self.num_5spoint_tkus_tracks) \
              / float(self.num_5spoint_tkus_events)
            us_5pt_Err = ((us_5pt * (1-us_5pt)) \
              /float(self.num_5spoint_tkus_events)) ** (0.5)
        except (ZeroDivisionError, ValueError):
            us_5pt = 0.0
            us_5pt_Err = 0.0
        try:
            us_3to5pt = float(self.num_3to5spoint_tkus_tracks) \
              / float(self.num_5spoint_tkus_events)
            us_3to5pt_Err = ((us_3to5pt * (1-us_3to5pt)) \
              /float(self.num_5spoint_tkus_events)) ** (0.5)
            #us_3to5pt_Err = 0.001
        except (ZeroDivisionError, ValueError):
            us_3to5pt = 0.0
            us_3to5pt_Err = 0.0
        try:
            ds_5pt = float(self.num_5spoint_tkds_tracks) \
              / float(self.num_5spoint_tkds_events)
            ds_5pt_Err = ((ds_5pt * (1-ds_5pt)) \
              /float(self.num_5spoint_tkds_events)) ** (0.5)
        except (ZeroDivisionError, ValueError):
            ds_5pt = 0.0
            ds_5pt_Err = 0.0
        try:
            ds_3to5pt = float(self.num_3to5spoint_tkds_tracks) \
              / float(self.num_5spoint_tkds_events)
            ds_3to5pt_Err = ((ds_3to5pt * (1-ds_3to5pt)) \
              /float(self.num_5spoint_tkds_events)) ** (0.5)
            #ds_3to5pt_Err = 0.001
        except (ZeroDivisionError, ValueError):
            ds_3to5pt = 0.0
            ds_3to5pt_Err = 0.0
        print os.path.basename(root_file_name) + '\t',
        print str(self.num_total_events) + '\t\t',
        print str(self.num_tof_spoint_good_events) + '\t' + \
          str(self.num_tof_good_events) + '\t',

        f = '%.5f \t%.5f \t%.5f \t%.5f \t%.5f \t%.5f \t%.5f  \t%.5f'
        print f % (us_5pt, us_5pt_Err, us_3to5pt, \
          us_3to5pt_Err, ds_5pt, ds_5pt_Err, ds_3to5pt, ds_3to5pt_Err)

if __name__ == "__main__":
    eff = PatRecEfficiency()
    args = sys.argv
    args.pop(0)
    eff.run(args)
