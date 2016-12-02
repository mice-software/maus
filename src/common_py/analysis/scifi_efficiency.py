#!/usr/bin/env python

"""  Check tracker efficiency """

import os
import math
import abc
import ROOT
import libMausCpp #pylint: disable = W0611
import analysis.scifitools as tools

#pylint: disable = R0902
#pylint: disable = R0912
#pylint: disable = R0914
#pylint: disable = R0915
#pylint: disable = C0103
#pylint: disable = W0612

class EfficiencyDataReal():
    """ Class to store pattern recognition efficiency data from real data """
    def __init__(self):
        """ Initialise member variables """
        self.n_events_total = 0
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
        self.n_events_total += other.n_events_total
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
        return self

    def calculate_efficiency(self):
        """ Calculate the final efficiency figures """
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
            self.eff_tkus_5pt = float(self.n_5spoint_tkus_tracks) \
              /float(self.n_passed_tkus_events)
            self.eff_tkus_5pt_err = \
              (self.eff_tkus_5pt * (1-self.eff_tkus_5pt)) \
              /(float((self.n_passed_tkus_events)) ** (0.5))
        except (ZeroDivisionError, ValueError):
            self.eff_tkus_5pt = 0.0
            self.eff_tkus_5pt_err = 0.0
        try:
            self.eff_tkus_3_5pt = float(self.n_3to5spoint_tkus_tracks) \
              /float(self.n_passed_tkus_events)
            self.eff_tkus_3_5pt_err = \
              (self.eff_tkus_3_5pt * (1-self.eff_tkus_3_5pt)) \
                /(float((self.n_passed_tkus_events)) ** (0.5))
            self.eff_tkus_3_5pt_err = 0.001
        except (ZeroDivisionError, ValueError):
            self.eff_tkus_3_5pt = 0.0
            self.eff_tkus_3_5pt_err = 0.0

        # Downstream tracker
        try:
            self.eff_tkds_5pt = float(self.n_5spoint_tkds_tracks) \
              / float(self.n_passed_tkds_events)
            self.eff_tkds_5pt_err = \
              (self.eff_tkds_5pt * (1-self.eff_tkds_5pt)) \
              /(float((self.n_passed_tkds_events)) ** (0.5))
        except (ZeroDivisionError, ValueError):
            self.eff_tkds_5pt = 0.0
            self.eff_tkds_5pt_err = 0.0
        try:
            self.eff_tkds_3_5pt = float(self.n_3to5spoint_tkds_tracks) \
              /float(self.n_passed_tkds_events)
            self.eff_tkds_3_5pt_err = \
              (self.eff_tkds_3_5pt * (1-self.eff_tkds_3_5pt)) \
                /(float((self.n_passed_tkds_events)) ** (0.5))
            #self.eff_tkds_3_5pt_err = 0.001
        except (ZeroDivisionError, ValueError):
            self.eff_tkds_3_5pt = 0.0
            self.eff_tkds_3_5pt_err = 0.0

    def clear(self):
        """ Clear all data """
        self.n_events_total = 0
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

class EfficiencyDataMC():
    """ Class to store pattern recognition efficiency data from MC data """
    def __init__(self):
        """ Initialise member variables """
        self.n_events_total = 0 # Total number of events
        self.n_mc_tku_tracks_total = 0 # Total number of tkus tracks
        self.n_mc_tkd_tracks_total = 0 # Total number of tkus tracks

        # MC tracks which created a viable # of spoints
        self.n_mc_tku_tracks_5pt = 0
        self.n_mc_tku_tracks_4to5pt = 0
        self.n_mc_tku_tracks_3to5pt = 0
        self.n_mc_tkd_tracks_5pt = 0
        self.n_mc_tkd_tracks_4to5pt = 0
        self.n_mc_tkd_tracks_3to5pt = 0

        # Number of good recon tracks found
        self.n_rec_tku_tracks_5pt_good = 0
        self.n_rec_tkd_tracks_5pt_good = 0
        self.n_rec_tku_tracks_4to5pt_good = 0
        self.n_rec_tkd_tracks_4to5pt_good = 0
        self.n_rec_tku_tracks_3to5pt_good = 0
        self.n_rec_tkd_tracks_3to5pt_good = 0

        self.eff_tkus_tracks = 0.0 # Efficiency of finding good tracks in tkus
        self.eff_tkds_tracks = 0.0 # Efficiency of finding good tracks in tkds

    def __iadd__(self, other):
        """ Add another instance's data to that here,
            just the totals, not the final efficiency data"""
        self.n_events_total += other.n_events_total
        self.n_mc_tku_tracks_total += other.n_mc_tku_tracks_total
        self.n_mc_tkd_tracks_total += other.n_mc_tkd_tracks_total

        self.n_mc_tku_tracks_5pt += other.n_mc_tku_tracks_5pt
        self.n_mc_tku_tracks_4to5pt += other.n_mc_tku_tracks_4to5pt
        self.n_mc_tku_tracks_3to5pt += other.n_mc_tku_tracks_3to5pt

        self.n_mc_tkd_tracks_5pt += other.n_mc_tkd_tracks_5pt
        self.n_mc_tkd_tracks_4to5pt += other.n_mc_tkd_tracks_4to5pt
        self.n_mc_tkd_tracks_3to5pt += other.n_mc_tkd_tracks_3to5pt

        self.n_rec_tku_tracks_5pt_good += other.n_rec_tku_tracks_5pt_good
        self.n_rec_tkd_tracks_5pt_good += other.n_rec_tkd_tracks_5pt_good
        self.n_rec_tku_tracks_4to5pt_good += other.n_rec_tku_tracks_4to5pt_good
        self.n_rec_tkd_tracks_4to5pt_good += other.n_rec_tkd_tracks_4to5pt_good
        self.n_rec_tku_tracks_3to5pt_good += other.n_rec_tku_tracks_3to5pt_good
        self.n_rec_tkd_tracks_3to5pt_good += other.n_rec_tkd_tracks_3to5pt_good

        return self

    def calculate_efficiency(self):
        """ Calculate the final efficiency figures """
        self.eff_tkus_tracks = 0.0
        self.eff_tkds_tracks = 0.0

        # Upstream tracker
        try:
            self.eff_tkus_tracks = float(self.n_rec_tku_tracks_user_good) \
              / float(self.n_mc_tku_tracks_user)
        except (ZeroDivisionError, ValueError):
            self.eff_tkus_tracks = 0.0
        # Downstream tracker
        try:
            self.eff_tkds_tracks = float(self.n_rec_tkd_tracks_user_good) \
              / float(self.n_mc_tkd_tracks_user)
        except (ZeroDivisionError, ValueError):
            self.eff_tkds_tracks = 0.0

    def clear(self):
        """ Clear all data """
        self.n_events_total = 0
        self.n_mc_tku_tracks_total = 0
        self.n_mc_tkd_tracks_total = 0

        self.n_mc_tku_tracks_5pt = 0
        self.n_mc_tku_tracks_4to5pt = 0
        self.n_mc_tku_tracks_3to5pt = 0
        self.n_mc_tkd_tracks_5pt = 0
        self.n_mc_tkd_tracks_4to5pt = 0
        self.n_mc_tkd_tracks_3to5pt = 0

        self.n_rec_tku_tracks_5pt_good = 0
        self.n_rec_tkd_tracks_5pt_good = 0
        self.n_rec_tku_tracks_4to5pt_good = 0
        self.n_rec_tkd_tracks_4to5pt_good = 0
        self.n_rec_tku_tracks_3to5pt_good = 0
        self.n_rec_tkd_tracks_3to5pt_good = 0

        self.eff_tkus_tracks = 0.0
        self.eff_tkds_tracks = 0.0

class PatternRecognitionEfficiencyBase():
    """ Base class for other Pattern Recognition Efficiency classes """
    __metaclass__ = abc.ABCMeta

    def __init__(self):
        """ Initialise member variables """
        self.root_files = []

    def run(self, files):
        """ Loop over input root file, send each file for processing, print
            summary information and produce any plots"""
        self.print_welcome()

        self.root_files = tools.load_data(files)
        if len(self.root_files) < 1:
            return False

        counter = 0
        for root_file_name in self.root_files:
            self.process_file(root_file_name)
            self.print_info(root_file_name, 'file')
            self.accumulate_job_data()
            counter += 1
        self.calculate_job_efficiency()
        self.print_info('Final job results', 'job')
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
        self.calculate_file_efficiency()

        # Close the ROOT file
        root_file.Close()

    @abc.abstractmethod
    def accumulate_job_data(self):
        """ Accumulate the job data """
        pass

    @abc.abstractmethod
    def calculate_file_efficiency(self):
        """ Calculate the file efficiency """
        pass

    @abc.abstractmethod
    def calculate_job_efficiency(self):
        """ Calculate the job efficiency """
        pass

    @abc.abstractmethod
    def clear(self):
        """ Set the internal data counters to zero & booleans to false """
        pass

    @abc.abstractmethod
    def print_info(self, data_name, info_type='file'):
        """ Print the results """
        pass

    @abc.abstractmethod
    def print_welcome(self):
      """ Message to be printed at programme start """
      pass

    @abc.abstractmethod
    def process_spill(self, spill):
      """ Process one spill of data """
      pass

class PatternRecognitionEfficiencyReal(PatternRecognitionEfficiencyBase):
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

        self.fdata = EfficiencyDataReal() # Per file data
        self.jdata = EfficiencyDataReal() # Per job data

        # Other objects not reset by the clear() function
        self.n_print_calls = 0

    def accumulate_job_data(self):
        """ Accumulate the job data """
        self.jdata += self.fdata

    def calculate_file_efficiency(self):
        """ Calculate the file efficiency """
        self.fdata.calculate_efficiency()

    def calculate_job_efficiency(self):
        """ Calculate the job efficiency """
        self.jdata.calculate_efficiency()

    def print_welcome(self):
        """ Message to be printed at programme start """
        print '\nPattern Recognition Efficiency Calculator (Real Data)'
        print '*******************************************************\n'

        print 'Parameters:'
        print 'Check helical\t' + str(self.check_helical)
        print 'Check straight\t' + str(self.check_straight)
        print 'Cut on TOF\t' + str(self.cut_on_tof)
        print 'Cut on TOF Time\t' + str(self.cut_on_tof_time)
        print 'Cut on trackers\t' + str(self.cut_on_tracker_10spnt) + '\n'

    def process_spill(self, spill):
        """ Process one spill of data """
        #print "Spill number " + str(spill.GetSpillNumber())
        if spill.GetDaqEventType() != "physics_event":
            return False # remove event from consideration

        # Loop over recon events
        for i in range(spill.GetReconEvents().size()):
            self.fdata.n_events_total += 1

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
                if (trk.get_spacepoints_pointers().size() == 5) \
                  and self.bool_tkus_1track:
                    self.fdata.n_5spoint_tkus_tracks += 1
                    self.bool_tkus_5spoint_track = True
            elif trk.get_tracker() == 1:
                if (trk.get_spacepoints_pointers().size() == 5) \
                  and self.bool_tkds_1track:
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
        """ Look for 5 spacepoint events in each tracker, increment the
            internal good event counters, and return if the tracker spacepoint
            data passes the selected cuts """
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

    #def make_plots(self, file_name_prefix):
        #""" Draw the histogramms and save """
        #c1 = ROOT.TCanvas()
        #c1.Divide(2)
        #c1.cd(1)
        #self.pt_hist.Draw()
        #c1.cd(2)
        #self.pz_hist.Draw()
        #c1.SaveAs(file_name_prefix + "_momentum_failed_events.pdf")

    def print_info(self, data_name, info_type='file'):
        """ Print the results """
        if info_type == 'file':
            edata = self.fdata
        elif info_type == 'job':
            edata = self.jdata

        if self.n_print_calls == 0:
            print '\nFile\t\t\tRecon_evt\tTOF\tTOF_SP\tTU_5pt' + \
              '\tTD_5pt\tT_10pt\tGood_TU\tGood_TD\tTU_5trk' + \
              '\tTU_5trk_err\tTU_3-5trk\tTU_3-5trk_err\t',
            print 'TD_5trk\tTD_5trk_err\tTD_3-5trk\tTD_3-5trk_err'

        print os.path.basename(data_name) + '\t',
        print str(edata.n_events_total) + '\t\t',
        print str(edata.n_passed_tof_timing_events) + '\t' + \
          str(edata.n_passed_tof_spoint_events) + '\t' + \
          str(edata.n_5spoint_tkus_events) + '\t' + \
          str(edata.n_5spoint_tkds_events) + '\t' + \
          str(edata.n_10spoint_events) + '\t' + \
          str(edata.n_passed_tkus_events) + '\t' + \
          str(edata.n_passed_tkds_events) + '\t',

        f = '%.4f \t%.4f \t%.4f \t%.4f \t%.4f \t%.4f \t%.4f  \t%.4f'
        print f % (edata.eff_tkus_5pt, edata.eff_tkus_5pt_err, \
          edata.eff_tkus_3_5pt, edata.eff_tkus_3_5pt_err, \
          edata.eff_tkds_5pt, edata.eff_tkds_5pt_err, \
          edata.eff_tkds_3_5pt, edata.eff_tkds_3_5pt_err)

        self.n_print_calls += 1

class PatternRecognitionEfficiencyMC(PatternRecognitionEfficiencyBase):
    """ Class to check Pattern Recognition efficiency with MC data """

    def __init__(self, nstations_mc_tku=5, nstations_mc_tkd=5, \
      nstations_rec_tku=5, nstations_rec_tkd=5):
        """ Initialise member variables """
        self.n_hits_cut = 5
        self.nstations_rec_tku = nstations_rec_tku
        self.nstations_rec_tkd = nstations_rec_tkd
        self.nstations_mc_tku = nstations_mc_tku
        self.nstations_mc_tkd = nstations_mc_tkd
        self.fdata = EfficiencyDataMC() # Per file data
        self.jdata = EfficiencyDataMC() # Per job data

        # Other objects not reset by the clear() function
        self.n_print_calls = 0

    def process_spill(self, spill):
        """ Process one spill of data """
        if spill.GetDaqEventType() != "physics_event":
            return False # remove event from consideration

        # print 'Processing spill ' + str(spill.GetSpillNumber())

        # Loop over events
        for i in range(spill.GetReconEvents().size()):
             # print '  Processing event ' + str(i)
             revent = spill.GetReconEvents()[i]
             mcevent = spill.GetMCEvents()[i]
             self.process_event(revent, mcevent)

    def process_event(self, revent, mcevent):
        """ Process the data for one event """
        sfevent = revent.GetSciFiEvent()

        # Calculate the expected number of tracks by calculating how many
        # MC tracks produced spacepoints in every station
        # ***************************************************************

        #  Separate the spacepoints by tracker
        spoints_tku = \
          [sp for sp in sfevent.spacepoints() if sp.get_tracker() == 0]
        spoints_tkd = \
          [sp for sp in sfevent.spacepoints() if sp.get_tracker() == 1]

        # Create the MC lookup
        lkup = tools.SciFiLookup(mcevent)

        # Find the number of expected 5, 4, and 3 point tracks from the MC
        tracks_tku_5pt = tools.find_mc_tracks_from_spoints(lkup, \
          spoints_tku, 5, self.n_hits_cut)
        tracks_tkd_5pt = tools.find_mc_tracks_from_spoints(lkup, \
          spoints_tkd, 5, self.n_hits_cut)
        tracks_tku_4to5pt = tools.find_mc_tracks_from_spoints(lkup, \
          spoints_tku, 4, self.n_hits_cut)
        tracks_tkd_4to5pt = tools.find_mc_tracks_from_spoints(lkup, \
          spoints_tkd, 4, self.n_hits_cut)
        tracks_tku_3to5pt = tools.find_mc_tracks_from_spoints(lkup, \
          spoints_tku, 3, self.n_hits_cut)
        tracks_tkd_3to5pt = tools.find_mc_tracks_from_spoints(lkup, \
          spoints_tkd, 3, self.n_hits_cut)

        # Update internal counters
        self.fdata.n_mc_tku_tracks_5pt += len(tracks_tku_5pt)
        self.fdata.n_mc_tkd_tracks_5pt += len(tracks_tkd_5pt)
        self.fdata.n_mc_tku_tracks_4to5pt += len(tracks_tku_4to5pt)
        self.fdata.n_mc_tkd_tracks_4to5pt += len(tracks_tkd_4to5pt)
        self.fdata.n_mc_tku_tracks_3to5pt += len(tracks_tku_3to5pt)
        self.fdata.n_mc_tkd_tracks_3to5pt += len(tracks_tkd_3to5pt)

        # Find the number of tracks actually reconstructed by pat rec
        # ***********************************************************

        # Sort tracks by type and tracker
        stracks_tku = [trk for trk in sfevent.straightprtracks() \
          if trk.get_tracker() == 0]
        stracks_tkd = [trk for trk in sfevent.straightprtracks() \
          if trk.get_tracker() == 1]
        htracks_tku = [trk for trk in sfevent.helicalprtracks() \
          if trk.get_tracker() == 0]
        htracks_tkd = [trk for trk in sfevent.helicalprtracks() \
          if trk.get_tracker() == 1]

        # How many 5, 4-5, 3-5 point tracks were reconstructed
        tracks_tku = htracks_tku + stracks_tku
        tracks_tkd = htracks_tkd + stracks_tkd
        results_tku_5pt = self.check_tracks(lkup, 5, tracks_tku)
        results_tkd_5pt = self.check_tracks(lkup, 5, tracks_tkd)
        results_tku_4pt = self.check_tracks(lkup, 4, tracks_tku)
        results_tkd_4pt = self.check_tracks(lkup, 4, tracks_tkd)
        results_tku_3pt = self.check_tracks(lkup, 3, tracks_tku)
        results_tkd_3pt = self.check_tracks(lkup, 3, tracks_tkd)

        # Update internal counters
        self.fdata.n_rec_tku_tracks_5pt_good += results_tku_5pt[0]
        self.fdata.n_rec_tkd_tracks_5pt_good += results_tkd_5pt[0]
        self.fdata.n_rec_tku_tracks_4to5pt_good += results_tku_4pt[0]
        self.fdata.n_rec_tkd_tracks_4to5pt_good += results_tkd_4pt[0]
        self.fdata.n_rec_tku_tracks_3to5pt_good += results_tku_3pt[0]
        self.fdata.n_rec_tkd_tracks_3to5pt_good += results_tkd_3pt[0]

    def accumulate_job_data(self):
        """ Accumulate the job data """
        self.jdata += self.fdata

    def calculate_file_efficiency(self):
        """ Calculate the file efficiency """
        self.fdata.calculate_efficiency()

    def calculate_job_efficiency(self):
        """ Calculate the job efficiency """
        self.jdata.calculate_efficiency()

    def check_tracks(self, lkup, nstations, recon_tracks, purity_cut=0.99):
        """ Are the recon tracks found good tracks wrt the MC data?
            nstations is the number of tracker stations, it can be used
            to control the minimum number of stations the recon track
            has correct spacepoints for, to count as a good track """

        # good = tracks with a correctly found fraction of spoints > purity_cut
        # mixed = tracks with a correctly found fraction of spoints > 0.5
        # bad = any other tracks (badly messed up ones)
        n_good_tracks = 0
        n_mixed_tracks = 0
        n_bad_tracks = 0
        for trk in recon_tracks:
            purity = \
              tools.calculate_purity(lkup, trk, nstations, self.n_hits_cut)
            if purity >= purity_cut:
                n_good_tracks += 1
            elif purity > 0.5:
                n_mixed_tracks += 1
            else:
                n_bad_tracks += 1
        return n_good_tracks, n_mixed_tracks, n_bad_tracks

    def clear(self):
        """ Set the internal file data counters to zero & booleans to false """
        self.fdata.clear()

    def print_info(self, data_name, info_type='file'):
        """ Print the results """
        if info_type == 'file':
            edata = self.fdata
        elif info_type == 'job':
            edata = self.jdata

        if self.n_print_calls == 0:
            print '\nFile\t\t\tRecon_evt\tTUMC5\tTDMC5\tTUMC45\tTDMC45\t' + \
              'TUMC35\tTDMC35\tTURG5\tTDRG5\tTURG45\tTDRG45\tTURG35\tTDRG35\t'

        print os.path.basename(data_name) + '\t',
        print str(edata.n_events_total) + '\t\t',
        print str(edata.n_mc_tku_tracks_5pt) + '\t' + \
          str(edata.n_mc_tkd_tracks_5pt) + '\t' + \
          str(edata.n_mc_tku_tracks_4to5pt) + '\t' + \
          str(edata.n_mc_tkd_tracks_4to5pt) + '\t' + \
          str(edata.n_mc_tku_tracks_3to5pt) + '\t' + \
          str(edata.n_mc_tkd_tracks_3to5pt) + '\t' + \
          str(edata.n_rec_tku_tracks_5pt_good) + '\t' + \
          str(edata.n_rec_tkd_tracks_5pt_good) + '\t' + \
          str(edata.n_rec_tku_tracks_4to5pt_good) + '\t' + \
          str(edata.n_rec_tkd_tracks_4to5pt_good) + '\t' + \
          str(edata.n_rec_tku_tracks_3to5pt_good) + '\t' + \
          str(edata.n_rec_tkd_tracks_3to5pt_good)
        self.n_print_calls += 1

    def print_welcome(self):
        """ Message to be printed at programme start """
        print '\nPattern Recognition Efficiency Calculator (MC)'
        print '************************************************\n'
