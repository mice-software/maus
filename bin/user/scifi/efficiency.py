#!/usr/bin/env python

"""  Check tracker efficiency """

import sys
import os
import ROOT
import libMausCpp #pylint: disable = W0611

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
        self.check_helical = False
        self.check_straight = True
        self.cut_on_tof = True
        self.cut_on_tof_time = True
        self.cut_on_trackers = True
        self.cut_on_fiducial_track = True

        self.root_files = []

        self.num_total_events = 0
        self.num_12spoint_events = 0
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
        print 'Cut on fiducial track\t' + str(self.cut_on_fiducial_track) 
        print 'Cut on trackers\t' + str(self.cut_on_trackers) + '\n'

        self.root_files = self.load_data(files)
        if len(self.root_files) < 1:
            return False

        print '\nFile\t\t\t#_Recon_Events\tTkUS_5pt\tTkUS_5pt_Err\
          \tTkUS_3-5_pt\tTkUS_3-5_pt_Err\t',
        print 'TkDS_5pt\tTkDS_5_pt_Err\tTkDS_3-5pt\tTkDS_3-5_pt_Err'
        for root_file_name in self.root_files:
            self.calculate_efficiency(root_file_name)
            self.print_file_info(root_file_name)
        return True

    def load_data(self, files):
        """ Load data from files. If a dir is given, search recursively """
        root_files = []

        if type(files) is not list:
            files = [files]

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
                self.root_files_dir_search(file_name, root_files)
            if len(root_files) < 1:
                print 'No data files found'
                return root_files

        print '\nFound ' + str(len(root_files)) + ' ROOT files:'
        for f in root_files:
            print f
        return root_files

    @staticmethod
    def root_files_dir_search(top_dir, root_files):
        """ Appends any ROOT files found in the directory to root_files """
        for dir_name, subdir_list, file_list in os.walk(top_dir):
            print('Searching directory: %s' % dir_name)
            for fname in file_list:
                file_suffix, file_extension = os.path.splitext(fname)
                if file_extension == '.root':
                    print('\t%s' % fname)
                    root_files.append(dir_name + '/' + fname)

    def calculate_efficiency(self, root_file_name):
        """ Calculate the Pattern Recognition efficiency for the input file """

        self.clear_counters() # Start clean each time

        # Load the ROOT file
        # print "Loading ROOT file", root_file_name
        root_file = ROOT.TFile(root_file_name, "READ") # pylint: disable = E1101

        # Set up the data tree to be filled by ROOT IO
        # print "Setting up data tree"

        tree = root_file.Get("Spill")
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree.SetBranchAddress("data", data)

        # Loop over spills
        for i in range(tree.GetEntries()):
            tree.GetEntry(i)
            spill = data.GetSpill()
            # Print some basic information about the spill
            # print "Found spill number", spill.GetSpillNumber(),
            # print "in run number", spill.GetRunNumber()
            if spill.GetDaqEventType() != "physics_event":
                continue
            for i in range(spill.GetReconEvents().size()):
                self.num_total_events += 1

                # Pull out tof data
                #------------------
                tof_evt = spill.GetReconEvents()[i].GetTOFEvent()
                tof1 = tof_evt.GetTOFEventSpacePoint().GetTOF1SpacePointArray()
                tof2 = tof_evt.GetTOFEventSpacePoint().GetTOF2SpacePointArray()
                scifi_evt = spill.GetReconEvents()[i].GetSciFiEvent()

                # Require a sp in TOF1 and TOF2
                bool_2tof_spoint_event = True
                if ((len(tof1) != 1) or (len(tof2) != 1)):
                    bool_2tof_spoint_event = False
                    if self.cut_on_tof:
                        continue

                # Require timing coincidence between TOF1 and TOF2
                bool_2tof_timing_event = True
                for j in range(tof1.size()):
                    if tof1.size() == 1 and tof2.size() == 1:
                        tof_time_1 = tof1[j].GetTime()
                        tof_time_2 = tof2[j].GetTime()
                        if (tof_time_2 - tof_time_1 > 50) \
                          or (tof_time_2 - tof_time_1 < 27):
                            bool_2tof_timing_event = False
                            if self.cut_on_tof_time:
                                continue

                # Fiducial track cut from upstream tracker:
                track_ok = False
                for track in scifi_evt.straightprtracks():
                    if track.get_tracker() == 0:
                        # Project to downstream:
                        z_loc = -3800
                        x = track.get_x0() + z_loc*track.get_mx()
                        y = track.get_y0() + z_loc*track.get_my()
                        if ((x*x+y*y) ** 0.5) < 120:
                            track_ok = True
                if not track_ok and self.cut_on_fiducial_track:
                    continue # remove event from consideration



                # Pull out tracker data
                #----------------------

                # All spacepoint data
                tk_evt = spill.GetReconEvents()[i].GetSciFiEvent()
                tk_spoints = tk_evt.spacepoints()
                bool_10spoint_event = True
                bool_tkus_5spoint_event = True
                bool_tkds_5spoint_event = True
                for i in range(2):
                    tracker = [sp for sp in tk_spoints if sp.get_tracker() == i]
                    for j in range(1, 6):
                        station = \
                          [sp for sp in tracker if sp.get_station() == j]
                        if len(station) != 1:
                            bool_10spoint_event = False
                            if i == 0:
                                bool_tkus_5spoint_event = False
                            elif i == 1:
                                bool_tkds_5spoint_event = False

                if self.cut_on_trackers and (not bool_10spoint_event):
                    continue

                if bool_2tof_spoint_event and bool_2tof_timing_event \
                  and bool_10spoint_event:
                    self.num_12spoint_events += 1
                if bool_10spoint_event:
                    self.num_10spoint_events += 1
                if bool_tkus_5spoint_event:
                    self.num_5spoint_tkus_events += 1
                if bool_tkds_5spoint_event:
                    self.num_5spoint_tkds_events += 1

                # Pat Rec seed data
                if (self.check_helical) and (not self.check_straight):
                    prtracks = tk_evt.helicalprtracks()
                    # print 'Looking at helical tracks'
                elif (not self.check_helical) and (self.check_straight):
                    prtracks = tk_evt.straightprtracks()
                    # print 'Looking at straight tracks'
                elif (self.check_helical) and (self.check_straight):
                    prtracks = []
                    prtracks.append(tk_evt.helicalprtracks())
                    prtracks.append(tk_evt.straightprtracks())
                    # print 'Looking at helical and straight tracks'
                else:
                    print 'Both track options set, aborting'
                    return

                # Is there at least 1 track present in either tracker
                bool_tkus_1track = False
                bool_tkds_1track = False
                if len(prtracks) < 1:
                    continue

                num_tkus_tracks = 0
                num_tkds_tracks = 0
                # Require 1 and only 1 track in a tracker
                for trk in prtracks:
                    if trk.get_tracker() == 0:
                        num_tkus_tracks += 1
                    elif trk.get_tracker() == 1:
                        num_tkds_tracks += 1
                if num_tkus_tracks == 1:
                    bool_tkus_1track = True
                if num_tkds_tracks == 1:
                    bool_tkds_1track = True

                # If there were 5 spacepoints in a tracker and 1 track only was
                # found in it, increment the 3 to 5 spoint track counter
                if bool_tkus_5spoint_event and bool_tkus_1track:
                    self.num_3to5spoint_tkus_tracks += 1
                if bool_tkds_5spoint_event and bool_tkds_1track:
                    self.num_3to5spoint_tkds_tracks += 1

                # Now check the tracks found had 5 spoints, 1 from each station
                bool_tkus_5spoint_track = False
                bool_tkds_5spoint_track = False
                for trk in prtracks:
                    if trk.get_tracker() == 0:
                        if (len(trk.get_spacepoints()) == 5) \
                          and bool_tkus_1track:
                            self.num_5spoint_tkus_tracks += 1
                            bool_tkus_5spoint_track = True
                    elif trk.get_tracker() == 1:
                        if (len(trk.get_spacepoints()) == 5) \
                          and bool_tkds_1track:
                            self.num_5spoint_tkds_tracks += 1
                            bool_tkds_5spoint_track = True

                    # Lastly see if we found one 5 spoint track in BOTH trackers
                    if (len(prtracks) != 2):
                        continue
                    if (bool_tkus_5spoint_track and bool_tkds_5spoint_track):
                        self.num_10spoint_tracks += 1

        #print 'Total recon events: ' + str(num_total_events)
        #print '12 spacepoint events: ' + str(num_12spoint_events)
        #print '10 spacepoint events: ' + str(num_10spoint_events)
        #print 'TkUS 5 spoint events: ' + str(num_5spoint_tkus_events)
        #print 'TkDS 5 spoint events: ' + str(num_5spoint_tkds_events)
        #print '10 spoint PR tracks: ' + str(num_10spoint_tracks)
        #print 'TkUS 5 spoint PR tracks: ' + str(num_5spoint_tkus_tracks)
        #print 'TkDS 5 spoint PR tracks: ' + str(num_5spoint_tkds_tracks)
        #print 'TkUS 3to5 spoint PR tracks: ' + str(num_3to5spoint_tkus_tracks)
        #print 'TkDS 3to5 spoint PR tracks: ' + str(num_3to5spoint_tkds_tracks)

        # Close the ROOT file
        # print "Closing root file"
        root_file.Close()

    def clear_counters(self):
        """ Set the internal counters to zero """
        self.num_total_events = 0
        self.num_12spoint_events = 0
        self.num_10spoint_events = 0
        self.num_5spoint_tkus_events = 0
        self.num_5spoint_tkds_events = 0

        self.num_10spoint_tracks = 0
        self.num_5spoint_tkus_tracks = 0
        self.num_5spoint_tkds_tracks = 0
        self.num_3to5spoint_tkus_tracks = 0
        self.num_3to5spoint_tkds_tracks = 0

    def print_file_info(self, root_file_name):
        """ Calculate the efficiencies and print """
        try:
            us_5pt = float(self.num_5spoint_tkus_tracks) \
              / float(self.num_5spoint_tkus_events)
            us_5pt_Err = ((us_5pt * (1-us_5pt)) \
              /float(self.num_5spoint_tkus_events)) ** (0.5)
        except ZeroDivisionError:
            us_5pt = 0.0
        try:
            us_3to5pt = float(self.num_3to5spoint_tkus_tracks) \
              / float(self.num_5spoint_tkus_events)
            us_3to5pt_Err = ((us_3to5pt * (1-us_3to5pt)) \
              /float(self.num_5spoint_tkus_events)) ** (0.5)
            #us_3to5pt_Err = 0.001
        except ZeroDivisionError:
            us_3to5pt = 0.0
        try:
            ds_5pt = float(self.num_5spoint_tkds_tracks) \
              / float(self.num_5spoint_tkds_events)
            ds_5pt_Err = ((ds_5pt * (1-ds_5pt)) \
              /float(self.num_5spoint_tkds_events)) ** (0.5)
        except ZeroDivisionError:
            ds_5pt = 0.0
        try:
            ds_3to5pt = float(self.num_3to5spoint_tkds_tracks) \
              / float(self.num_5spoint_tkds_events)
            ds_3to5pt_Err = ((ds_3to5pt * (1-ds_3to5pt)) \
              /float(self.num_5spoint_tkds_events)) ** (0.5)
            #ds_3to5pt_Err = 0.001
        except ZeroDivisionError:
            ds_3to5pt = 0.0

        print os.path.basename(root_file_name) + '\t',
        print str(self.num_total_events) + '\t\t',

        print '%.5f \t%.5f \t%.5f \t%.5f \t%.5f \t%.5f \t%.5f \t%.5f' % \
          (us_5pt, us_5pt_Err, us_3to5pt, us_3to5pt_Err, \
            ds_5pt, ds_5pt_Err, ds_3to5pt, ds_3to5pt_Err)

if __name__ == "__main__":
    eff = PatRecEfficiency()
    args = sys.argv
    args.pop(0)
    eff.run(args)
