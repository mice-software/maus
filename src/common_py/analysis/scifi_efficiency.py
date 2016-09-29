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

class EfficiencyData():
    """ Class to store pattern recognition efficiency data """
    def __init__(self):
        """ Initialise member variables """
        self.n_total_events = 0
        self.n_passed_tof_timing_events = 0 # Events which pass the TOF cut
        self.n_passed_tof_spoint_events = 0 # Events with 1 sp in TOF1 & TOF2
        self.n_10spoint_events = 0 # Events with 5 spoints in both TkUS & TkDS
        self.n_5spoint_tkus_events = 0 # Events with 5 spoints in TkUS
        self.n_5spoint_tkds_events = 0 # Events with 5 spoints in TkDS
        self.n_passed_tkus_events = 0 # Events which pass all the set cuts
        self.n_passed_tkds_events = 0 # Events which pass all the set cuts

        self.n_10spoint_tracks = 0
        self.n_5spoint_tkus_tracks = 0
        self.n_5spoint_tkds_tracks = 0
        self.n_3to5spoint_tkus_tracks = 0
        self.n_3to5spoint_tkds_tracks = 0

        # Efficiency numbers
        self.eff_tkus_5pt = 0.0
        self.eff_tkus_5pt_err = 0.0
        self.eff_tkus_3_5pt = 0.0
        self.eff_tkus_3_5pt_err = 0.0
        self.eff_tkds_5pt = 0.0
        self.eff_tkds_5pt_err = 0.0
        self.eff_tkds_3_5pt = 0.0
        self.eff_tkds_3_5pt_err = 0.0

    def __iadd__(self, other):
        """ Add another instance's data to that here,
            just the totals, not the final efficiency data"""
        self.n_total_events += other.n_total_events 
        self.n_passed_tof_timing_events += other.n_passed_tof_timing_events
        self.n_passed_tof_spoint_events += other.n_passed_tof_spoint_events
        self.n_10spoint_events += other.n_10spoint_events
        self.n_5spoint_tkus_events += other.n_5spoint_tkus_events
        self.n_5spoint_tkds_events += other.n_5spoint_tkds_events
        self.n_passed_tkus_events += other.n_passed_tkus_events
        self.n_passed_tkds_events += other.n_passed_tkds_events

        self.n_10spoint_tracks += other.n_10spoint_tracks
        self.n_5spoint_tkus_tracks += other.n_5spoint_tkus_tracks
        self.n_5spoint_tkds_tracks += other.n_5spoint_tkds_tracks
        self.n_3to5spoint_tkus_tracks += other.n_3to5spoint_tkus_tracks
        self.n_3to5spoint_tkds_tracks += other.n_3to5spoint_tkds_tracks

    def calculate_efficiency(self):
        """ Calculate the final efficiency figures for a file """
        self.eff_tkus_5pt = 0.0
        self.eff_tkus_5pt_err = 0.0
        self.eff_tkus_3_5pt = 0.0
        self.eff_tkus_3_5pt_err = 0.0
        self.eff_tkds_5pt = 0.0
        self.eff_tkds_5pt_err = 0.0
        self.eff_tkds_3_5pt = 0.0
        self.eff_tkds_3_5pt_err = 0.0

        # Upstream tracker
        try:
            self.eff_tkus_5pt = float(self.fdata.n_5spoint_tkus_tracks) \
              /float(self.fdata.n_passed_tkus_events)
            self.eff_tkus_5pt_err = \
              (self.eff_tkus_5pt * (1-self.eff_tkus_5pt)) \
              /(float((self.fdata.n_passed_tkus_events)) ** (0.5))
        except (ZeroDivisionError, ValueError):
            self.eff_tkus_5pt = 0.0
            self.eff_tkus_5pt_err = 0.0
        try:
            self.eff_tkus_3_5pt = float(self.fdata.n_3to5spoint_tkus_tracks) \
              /float(self.fdata.n_passed_tkus_events)
            self.eff_tkus_3_5pt_err = \
              (self.eff_tkus_3_5pt * (1-self.eff_tkus_3_5pt)) \
                /(float((self.fdata.n_passed_tkus_events)) ** (0.5))
            self.eff_tkus_3_5pt_err = 0.001
        except (ZeroDivisionError, ValueError):
            self.eff_tkus_3_5pt = 0.0
            self.eff_tkus_3_5pt_err = 0.0

        # Downstream tracker
        try:
            self.eff_tkds_5pt = float(self.fdata.n_5spoint_tkds_tracks) \
              / float(self.fdata.n_passed_tkds_events)
            self.eff_tkds_5pt_err = \
              (self.eff_tkds_5pt * (1-self.eff_tkds_5pt)) \
              /(float((self.fdata.n_passed_tkds_events)) ** (0.5))
        except (ZeroDivisionError, ValueError):
            self.eff_tkds_5pt = 0.0
            self.eff_tkds_5pt_err = 0.0
        try:
            self.eff_tkds_3_5pt = float(self.fdata.n_3to5spoint_tkds_tracks) \
              /float(self.fdata.n_passed_tkds_events)
            self.eff_tkds_3_5pt_err = \
              (self.eff_tkds_3_5pt * (1-self.eff_tkds_3_5pt)) \
                /(float((self.fdata.n_passed_tkds_events)) ** (0.5))
            #self.eff_tkds_3_5pt_err = 0.001
        except (ZeroDivisionError, ValueError):
            self.eff_tkds_3_5pt = 0.0
            self.eff_tkds_3_5pt_err = 0.0

    def clear():
        """ Clear all data """
        self.n_total_events = 0
        self.n_passed_tof_timing_events = 0
        self.n_passed_tof_spoint_events = 0
        self.n_10spoint_events = 0
        self.n_5spoint_tkus_events = 0
        self.n_5spoint_tkds_events = 0
        self.n_passed_tkus_events = 0
        self.n_passed_tkds_events = 0

        self.n_10spoint_tracks = 0
        self.n_5spoint_tkus_tracks = 0
        self.n_5spoint_tkds_tracks = 0
        self.n_3to5spoint_tkus_tracks = 0
        self.n_3to5spoint_tkds_tracks = 0

        self.eff_tkus_5pt = 0.0
        self.eff_tkus_5pt_err = 0.0
        self.eff_tkus_3_5pt = 0.0
        self.eff_tkus_3_5pt_err = 0.0
        self.eff_tkds_5pt = 0.0
        self.eff_tkds_5pt_err = 0.0
        self.eff_tkds_3_5pt = 0.0
        self.eff_tkds_3_5pt_err = 0.0

class PatternRecognitionEfficiency():
    """ Class to check Pattern Recognition efficiency with real data """

    def __init__(self):
        """ Initialise member variables """

        # Options for the user
        self.check_helical = True
        self.check_straight = True
        self.cut_on_tof = True
        self.cut_on_tof_time = True
        self.cut_on_tracker_10spnt = True
        self.cut_on_tracker_5spnt = True # Should always be true
        self.use_mc_truth = False
        self.tof_upper_cut = 50.0
        self.tof_lower_cut = 27.0
        self.fiducial_cut = 150.0 # Not currently used
        self.id_frequency_cut = 0.5

        # ROOT objects
        self.root_files = []
        # self.pt_hist = ROOT.TH1D("pt_hist", "pt", 100, -100.0, 100.0);
        # self.pz_hist = ROOT.TH1D("pz_hist", "pz", 100, -100.0, 100.0);

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
        self.bool_passed_tkus_event = False # Have all the choosen cuts passed?
        self.bool_passed_tkds_event = False # Have all the choosen cuts passed?

        # Per file data
        self.fdata = EfficiencyData()

        # Per job data
        self.jdata = EfficiencyData()

        # Other objects not reset by the clear() function
        self.n_print_calls = 0

    def run(self, files):
        """ Loop over input root file, send each file for processing, print
            summary information and produce any plots"""
        print '\nPattern Recognition Efficiency Calculator'
        print '*****************************************\n'

        print 'Parameters:'
        print 'Check helical\t' + str(self.check_helical)
        print 'Check straight\t' + str(self.check_straight)
        print 'Cut on TOF\t' + str(self.cut_on_tof)
        print 'Cut on TOF Time\t' + str(self.cut_on_tof_time)
        # print 'Cut on fiducial track\t' + str(self.cut_on_fiducial_track) 
        print 'Cut on trackers\t' + str(self.cut_on_tracker_10spnt) + '\n'

        self.root_files = self.load_data(files)
        if len(self.root_files) < 1:
            return False

        counter = 0
        for root_file_name in self.root_files:
            self.process_file(root_file_name)
            self.print_info(self.fdata, root_file_name)
            self.jdata += self.fdata # Accumulate the job data
            counter += 1
        self.jdata.calculate_efficiency()
        self.print_info(self.jdata, 'Final job results')
        return True

    def process_file(self, root_file_name):
        """ Process one root file of data"""
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
            self.process_spill(spill) # Analyse the spill

        # Perform final efficiency calculations
        self.fdata.calculate_efficiency()

        # Close the ROOT file
        root_file.Close()

    def process_spill(self, spill):
        """ Process one spill of data """
        #print "Spill number " + str(spill.GetSpillNumber())
        if spill.GetDaqEventType() != "physics_event":
            return False # remove event from consideration

        # Loop over recon events
        for i in range(spill.GetReconEvents().size()):
            self.fdata.n_total_events += 1

            # Pull out tof data and check if cuts pass
            tof_evt = spill.GetReconEvents()[i].GetTOFEvent()
            tof_good = self.check_tof(tof_evt)
            if not tof_good:
                continue # remove event from consideration

            # Pull out tracker data and examine
            tk_evt = spill.GetReconEvents()[i].GetSciFiEvent()

            # Look at tracker spacepoint data if we expect tracks in either
            # tracker
            self.check_tracker_spacepoints(tk_evt.spacepoints())

            # If we expect good tracks in neither tracker, skip this event
            if ( not self.bool_passed_tkus_event ) and \
              ( not self.bool_passed_tkds_event ) :
                continue # remove event from consideration

            # Now switch from calculating expected tracks to what
            # was actually reconstructed

            # Extract the desired pat rec tracks (straight, helical or both)
            prtracks = self.extract_prtracks(tk_evt)

            # Analyse the pat rec tracks and see what we actually found
            self.analyse_prtracks(prtracks)

        return True

    #def analyse_mc_truth_momentum(self, mc_evt, tk_spoints, trker_num):
        #""" Find the mc scifi hits that formed the spacepoints, find the
            #parent primary mc track id, extract the mc truth momentum from
            #scifi hits corresponding to that track """

        ## Check the mc event is ok
        #if mc_evt is None:
          #return ()

        ## Create a lookup to find the mc scifi hits that formed the spacepoints
        #lookup = tools.SciFiLookup(mc_evt) # Lookup to link rec to MC
        #if len(lookup.hits_map) < 0:
          #return ()

        ## Try to find the mc primary track
        #hits = tools.find_mc_hits(lookup, tk_spoints, trker_num)
        #parent_track_id = tools.find_mc_track(hits, self.id_frequency_cut)
        #if parent_track_id != 0:
            ## Pull out the mc momentum and fill the histogramms
            #mom = tools.find_mc_momentum_sfhits(lookup, tk_spoints, \
              #parent_track_id, trker_num)
            #return mom
        #return ()

    def analyse_prtracks(self, prtracks):
        """ Analyse the reconstructed tracks, see how many were found when
            they were expected """

        # Require 1 and only 1 track in a tracker
        n_tkus_tracks = 0
        n_tkds_tracks = 0
        for trk in prtracks:
            if trk.get_tracker() == 0:
                n_tkus_tracks += 1
            elif trk.get_tracker() == 1:
                n_tkds_tracks += 1
        if n_tkus_tracks == 1:
            self.bool_tkus_1track = True
        if n_tkds_tracks == 1:
            self.bool_tkds_1track = True

        # If there were 5 spacepoints in a tracker and 1 track only was
        # found in it, increment the 3 to 5 spoint track counter
        if self.bool_tkus_5spoint_event and self.bool_tkus_1track:
            self.fdata.n_3to5spoint_tkus_tracks += 1
        if self.bool_tkds_5spoint_event and self.bool_tkds_1track:
            self.fdata.n_3to5spoint_tkds_tracks += 1

        # Now check the tracks found had 5 spoints, 1 from each station
        self.bool_tkus_5spoint_track = False
        self.bool_tkds_5spoint_track = False
        for trk in prtracks:
            if trk.get_tracker() == 0:
                if (trk.get_spacepoints_pointers().size() == 5) and self.bool_tkus_1track:
                    self.fdata.n_5spoint_tkus_tracks += 1
                    self.bool_tkus_5spoint_track = True
            elif trk.get_tracker() == 1:
                if (trk.get_spacepoints_pointers().size() == 5) and self.bool_tkds_1track:
                    self.fdata.n_5spoint_tkds_tracks += 1
                    self.bool_tkds_5spoint_track = True

        # Lastly see if we found only one 5 spoint track in BOTH trackers
        if (len(prtracks) == 2 and self.bool_tkus_5spoint_track and \
            self.bool_tkds_5spoint_track):
            self.fdata.n_10spoint_tracks += 1
        return True

    def check_tof(self, tof_evt):
        """ Analyse tof data. Return boolean indicating if tof cuts pass"""
        try:
            tof1 = tof_evt.GetTOFEventSpacePoint().GetTOF1SpacePointArray()
            tof2 = tof_evt.GetTOFEventSpacePoint().GetTOF2SpacePointArray()
        except ReferenceError:
            print "Bad TOF data"
            if self.cut_on_tof or self.cut_on_tof_time:
                return False
            # The data might be bad, but we aren't cutting on TOF so passes
            self.fdata.n_passed_tof_spoint_events += 1
            self.fdata.n_passed_tof_timing_events += 1
            return True

        # Require 1 and only 1 sp in both TOF1 and TOF2
        self.bool_2tof_spoint_event = True
        if ((tof1.size() != 1) or (tof2.size() != 1)):
            self.bool_2tof_spoint_event = False

        # Require timing coincidence between TOF1 and TOF2
        self.bool_2tof_timing_event = False
        if self.bool_2tof_spoint_event:
            for j in range(tof1.size()):
                if tof1.size() == 1 and tof2.size() == 1:
                    tof_time_1 = tof1[j].GetTime()
                    tof_time_2 = tof2[j].GetTime()
                    if ((tof_time_2 - tof_time_1) < self.tof_upper_cut) \
                      and ((tof_time_2 - tof_time_1) > self.tof_lower_cut):
                        self.bool_2tof_timing_event = True

        # Are the cuts choosen passed?
        tof_good = True
        if self.bool_2tof_spoint_event:
            self.fdata.n_passed_tof_spoint_events += 1
        elif self.cut_on_tof:
            tof_good = False
        else:
            self.fdata.n_passed_tof_spoint_events += 1

        if self.bool_2tof_timing_event:
            self.fdata.n_passed_tof_timing_events += 1
        elif self.cut_on_tof_time:
            tof_good = False
        else:
            self.fdata.n_passed_tof_timing_events += 1

        return tof_good

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
            self.fdata.n_10spoint_events += 1
        if self.bool_tkus_5spoint_event:
            self.fdata.n_5spoint_tkus_events += 1
        if self.bool_tkds_5spoint_event:
            self.fdata.n_5spoint_tkds_events += 1

        # Are the complete set of tracker spacepoint cuts passed?
        self.bool_passed_tkus_event = False
        self.bool_passed_tkds_event = False
        # Do we require 5 spoints in BOTH trackers?
        if self.cut_on_tracker_10spnt:
            if self.bool_10spoint_event:
                self.bool_passed_tkus_event = True
                self.fdata.n_passed_tkus_events += 1
                self.bool_passed_tkds_event = True
                self.fdata.n_passed_tkds_events += 1
            else:
                self.bool_passed_tkus_event = False
                self.bool_passed_tkds_event = False
        # If not, just see if have 5 spoints in each tracker individually
        elif self.cut_on_tracker_5spnt:
            if self.bool_tkus_5spoint_event:
                self.bool_passed_tkus_event = True
                self.fdata.n_passed_tkus_events += 1
            else:
                self.bool_passed_tkus_event = False
            if self.bool_tkds_5spoint_event:
                self.bool_passed_tkds_event = True
                self.fdata.n_passed_tkds_events += 1
            else:
                self.bool_passed_tkds_event = False
        # Not cutting on tracker spacepoints so everything passes
        else:
            self.bool_passed_tkus_event = True
            self.fdata.n_passed_tkus_events += 1
            self.bool_passed_tkds_event = True
            self.fdata.n_passed_tkds_events += 1

    def clear(self):
        """ Set the internal file data counters to zero & booleans to false """
        self.bool_2tof_timing_event = False
        self.bool_2tof_spoint_event = False
        self.bool_10spoint_event = False
        self.bool_tkus_5spoint_event = False
        self.bool_tkds_5spoint_event = False
        self.bool_tkus_1track = False
        self.bool_tkds_1track = False
        self.bool_tkus_5spoint_track = False
        self.bool_tkds_5spoint_track = False
        self.bool_passed_tkus_event = False
        self.bool_passed_tkds_event = False

        self.fdata.clear()

    def extract_prtracks(self, tk_evt):
        """ Pull out the pattern recognition tracks selected for analysis -
            either straight, helical or both """
        prtracks = []
        if (self.check_helical) and (not self.check_straight):
            prtracks = tools.vector_to_list(tk_evt.helicalprtracks())
            # print 'Looking at helical tracks'
        elif (not self.check_helical) and (self.check_straight):
            prtracks = tools.vector_to_list(tk_evt.straightprtracks())
            # print 'Looking at straight tracks'
        elif (self.check_helical) and (self.check_straight):
            htracks = tools.vector_to_list(tk_evt.helicalprtracks())
            stracks = tools.vector_to_list(tk_evt.straightprtracks())
            prtracks = htracks + stracks
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

    def print_info(self, edata, data_name):
        """ Print the results per file """
        if self.n_print_calls == 0:
            print '\nFile\t\tRecon_evt\tTOF\tTOF_SP\tTkUS_5pt\
              \tTkDS_5pt\tTk_10pt\tGood_TkUS\tGood_TkDS\tTkUS_5trk\
              \tTkUS_5trk_err\tTkUS_3-5trk\tTkUS_3-5trk_err\t',
            print 'TkDS_5trk\tTkDS_5trk_err\tTkDS_3-5trk\tTkDS_3-5trk_err'

        print os.path.basename(data_name) + '  ',
        print str(edata.n_total_events) + '\t',
        print str(edata.n_passed_tof_timing_events) + '\t' + \
          str(edata.n_passed_tof_spoint_events) + '\t' + \
          str(edata.n_5spoint_tkus_events) + '\t' + \
          str(edata.n_5spoint_tkds_events) + '\t' + \
          str(edata.n_10spoint_events) + '\t' + \
          str(edata.n_passed_tkus_events) + '\t' + \
          str(edata.n_passed_tkds_events) + '\t',

        f = '%.4f \t%.4f \t%.4f \t%.4f \t%.4f \t%.4f \t%.4f  \t%.4f'
        print f % (self.eff_tkus_5pt, self.eff_tkus_5pt_err, \
          self.eff_tkus_3_5pt, self.eff_tkus_3_5pt_err, \
          self.eff_tkds_5pt, self.eff_tkds_5pt_err, \
          self.eff_tkds_3_5pt, self.eff_tkds_3_5pt_err)

        self.n_print_calls += 1
