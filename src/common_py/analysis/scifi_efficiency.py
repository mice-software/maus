#!/usr/bin/env python

"""  Check tracker efficiency """

import os
import math
import numpy as np
# import abc
import ROOT
import libMausCpp #pylint: disable = W0611
import analysis.scifitools as tools # pylint: disable = E0401

#pylint: disable = R0902
#pylint: disable = R0912
#pylint: disable = R0914
#pylint: disable = R0915
#pylint: disable = C0103
#pylint: disable = W0612
#pylint: disable = E1101

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

        return [self.eff_tkus_5pt, -1.0, self.eff_tkus_3_5pt, \
          self.eff_tkds_5pt, -1.0, self.eff_tkds_3_5pt]

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
            self.eff_tkus_tracks = float(self.n_rec_tku_tracks_5pt_good) \
              / float(self.n_mc_tku_tracks_5pt)
        except (ZeroDivisionError, ValueError):
            self.eff_tkus_tracks = 0.0
        # Downstream tracker
        try:
            self.eff_tkds_tracks = float(self.n_rec_tkd_tracks_5pt_good) \
              / float(self.n_mc_tkd_tracks_5pt)
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

class EfficiencyBase(object):
    """ Base class for other Pattern Recognition Efficiency classes """
    #__metaclass__ = abc.ABCMeta

    def __init__(self):
        """ Initialise member variables """
        self.root_files = []
        self.current_run_number = -1

        # Options for the user
        self.cut_on_tof = True
        self.cut_on_tof_time = True
        self.tof_upper_cut = 50.0
        self.tof_lower_cut = 27.0
        print self.cut_on_tof

        # Event Results
        self.bool_2tof_timing_event = False # Tof timing ok
        self.bool_2tof_spoint_event = False # 1 spacepoint only in each tof

        # Job level results
        self.data = {}
        self.data['run_numbers']  = np.array([])
        self.data['tkus_5spoint'] = np.array([])
        self.data['tkus_4to5spoint'] = np.array([])
        self.data['tkus_3to5spoint'] = np.array([])
        self.data['tkds_5spoint'] = np.array([])
        self.data['tkds_4to5spoint'] = np.array([])
        self.data['tkds_3to5spoint'] = np.array([])

    def accumulate_job_data(self, data):
        """ Accumulate job level data in arrays by run number. data should
            be a list containing 7 entries:
            run number, tkus_5spoint, tkus_4to5spoint, tkus_3to5spoint,
            tkds_5spoint, tkds_4to5spoint, tkds_3to5spoint
        """
        self.data['run_numbers'] = np.append(self.data['run_numbers'], data[0])
        self.data['tkus_5spoint'] = \
          np.append(self.data['tkus_5spoint'], data[1])
        self.data['tkus_4to5spoint'] = \
          np.append(self.data['tkus_4to5spoint'], data[2])
        self.data['tkus_3to5spoint'] = \
          np.append(self.data['tkus_3to5spoint'], data[3])
        self.data['tkds_5spoint'] = \
          np.append(self.data['tkds_5spoint'], data[4])
        self.data['tkds_4to5spoint'] = \
          np.append(self.data['tkds_4to5spoint'], data[5])
        self.data['tkds_3to5spoint'] = \
          np.append(self.data['tkds_3to5spoint'], data[6])

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
            return True

        # Require 1 and only 1 sp in both TOF1 and TOF2
        self.bool_2tof_spoint_event = True
        if ((tof1.size() != 1) or (tof2.size() != 1)):
            self.bool_2tof_spoint_event = False

        # Require timing coincidence between TOF1 and TOF2
        self.bool_2tof_timing_event = False
        if self.bool_2tof_spoint_event:
            for j in range(tof1.size()):
                if (tof1.size() != 1) or (tof2.size() != 1):
                    continue
                dt = tof2[j].GetTime() - tof1[j].GetTime()

                if (dt < self.tof_upper_cut) and (dt > self.tof_lower_cut):
                    self.bool_2tof_timing_event = True

        # Are the cuts choosen passed?
        tof_good = True
        if (not self.bool_2tof_spoint_event) and self.cut_on_tof:
            tof_good = False

        if (not self.bool_2tof_timing_event) and self.cut_on_tof_time:
            tof_good = False

        return tof_good

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
            counter += 1
        self.calculate_average_efficiency()
        self.print_info('Final job results', 'average')
        return True

    def process_file(self, root_file_name):
        """ Process one root file of data"""
        self.clear() # Start clean each time
        self.current_run_number = tools.extract_run_number(root_file_name)

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
        self.accumulate_average_data()
        results = [self.current_run_number] + self.calculate_file_efficiency()
        self.accumulate_job_data(results)

        # Close the ROOT file
        root_file.Close()

    #@abc.abstractmethod
    def accumulate_average_data(self):
        """ Accumulate data in job averages """
        pass

    #@abc.abstractmethod
    def calculate_file_efficiency(self):
        """ Calculate the file efficiency """
        pass

    #@abc.abstractmethod
    def calculate_average_efficiency(self):
        """ Calculate the job efficiency """
        pass

    #@abc.abstractmethod
    def clear(self):
        """ Set the internal data counters to zero & booleans to false """
        pass

    #@abc.abstractmethod
    def print_info(self, data_name, info_type='file'):
        """ Print the results """
        pass

    #@abc.abstractmethod
    def print_welcome(self):
        """ Message to be printed at programme start """
        pass

    #@abc.abstractmethod
    def process_spill(self, spill):
        """ Process one spill of data """
        pass

class PatternRecognitionEfficiencyReal(EfficiencyBase):
    """ Class to check Pattern Recognition efficiency with real data """

    def __init__(self):
        """ Initialise member variables """
        EfficiencyBase.__init__(self)

        # Options for the user
        self.cut_on_tof = True
        self.cut_on_tof_time = True
        self.tof_upper_cut = 50.0
        self.tof_lower_cut = 27.0
        self.check_helical = True
        self.check_straight = True
        self.cut_on_both_tracker_num_spnts = True
        self.cut_on_tracker_5spnt = True # Should always be true

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
        self.adata = EfficiencyDataReal() # Per job data

        # Other objects not reset by the clear() function
        self.n_print_calls = 0

    def accumulate_average_data(self):
        """ Accumulate the job data """
        self.adata += self.fdata

    def calculate_file_efficiency(self):
        """ Calculate the file efficiency """
        return self.fdata.calculate_efficiency()

    def calculate_average_efficiency(self):
        """ Calculate the job efficiency """
        return self.adata.calculate_efficiency()

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
                if (tof1.size() != 1) or (tof2.size() != 1):
                    continue
                dt = tof2[j].GetTime() - tof1[j].GetTime()

                if (dt < self.tof_upper_cut) and \
                  (dt > self.tof_lower_cut):
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
        self.bool_tkus_1track = False
        self.bool_tkds_1track = False
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
                  and self.bool_tkus_5spoint_event and self.bool_tkus_1track:
                    self.fdata.n_5spoint_tkus_tracks += 1
                    self.bool_tkus_5spoint_track = True
            elif trk.get_tracker() == 1:
                if (trk.get_spacepoints_pointers().size() == 5) \
                  and self.bool_tkds_5spoint_event and self.bool_tkds_1track:
                    self.fdata.n_5spoint_tkds_tracks += 1
                    self.bool_tkds_5spoint_track = True

        # Lastly see if we found only one 5 spoint track in BOTH trackers
        if (len(prtracks) == 2 and self.bool_tkus_5spoint_track and \
            self.bool_tkds_5spoint_track):
            self.fdata.n_10spoint_tracks += 1
        return True


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
        elif info_type == 'average':
            edata = self.adata

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

class PatternRecognitionEfficiencyMC(EfficiencyBase):
    """ Class to check Pattern Recognition efficiency with MC data """

    def __init__(self):
        """ Initialise member variables """
        EfficiencyBase.__init__(self)

        # Parameters
        self.n_hits_cut = 5

        # Internal counters

        # Data storage
        self.fdata = EfficiencyDataMC() # Per file data
        self.adata = EfficiencyDataMC() # Per job data

        # Other objects not reset by the clear() function
        self.n_print_calls = 0

        self.hpt_m_tku = \
          ROOT.TH1F('hpt_m_tku', 'pt of missed tracks TKU', 100, 0.0, 100.0)
        self.hpt_m_tkd = \
          ROOT.TH1F('hpt_m_tkd', 'pt of missed tracks TKD', 100, 0.0, 100.0)
        self.hpz_m_tku = \
          ROOT.TH1F('hpz_m_tku', 'pz of missed tracks TKU', 100, 0.0, 260.0)
        self.hpz_m_tkd = \
          ROOT.TH1F('hpz_m_tkd', 'pz of missed tracks TKD', 100, 0.0, 260.0)

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

        # Calculate the expected number of tracks by calculating how many
        # MC tracks produced spacepoints in every station
        # ***************************************************************

        # Pull out tof data and check if cuts pass
        tof_evt = revent.GetTOFEvent()
        tof_good = self.check_tof(tof_evt)
        if not tof_good:
            return False # remove event from consideration

        # Now the tracker data
        bool_tkus_5spoint_event = False
        bool_tkds_5spoint_event = False

        sfevent = revent.GetSciFiEvent()
        spoints_tku = \
          [sp for sp in sfevent.spacepoints() if sp.get_tracker() == 0]
        spoints_tkd = \
          [sp for sp in sfevent.spacepoints() if sp.get_tracker() == 1]
        #print '    Found ' +str(len(spoints_tku)) + ' spoints in TKU'
        #print '    Found ' +str(len(spoints_tkd)) + ' spoints in TKD'
        lkup = tools.SciFiLookup(mcevent)

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

        # Only use the event for each detector if we have one 5pt track only
        if len(tracks_tku_5pt) == 1 and len(tracks_tku_4to5pt) == 1 and \
            len(tracks_tku_3to5pt) == 1:
            bool_tkus_5spoint_event = True
        else:
            bool_tkus_5spoint_event = False

        if len(tracks_tkd_5pt) == 1 and len(tracks_tkd_4to5pt) == 1 and \
            len(tracks_tkd_3to5pt) == 1:
            bool_tkds_5spoint_event = True
        else:
            bool_tkds_5spoint_event = False

        # If we do not have good events in either tracker, save time and stop
        if (not bool_tkus_5spoint_event) and (not bool_tkds_5spoint_event):
            return False

        #print 'Found ' + str(len(tracks_tku_5pt)) + ' 5pt MC tracks in TKU'
        #print 'Found ' + str(len(tracks_tkd_5pt)) + ' 5pt MC tracks in TKD'
        #print 'Found ' + str(len(tracks_tku_4to5pt)) + ' 4pt+ MC tracks in TKU'
        #print 'Found ' + str(len(tracks_tkd_4to5pt)) + ' 4pt+ MC tracks in TKD'
        #print 'Found ' + str(len(tracks_tku_3to5pt)) + ' 3pt+ MC tracks in TKU'
        #print 'Found ' + str(len(tracks_tkd_3to5pt)) + ' 3pt+ MC tracks in TKD'

        # Update internal counters
        if bool_tkus_5spoint_event:
            self.fdata.n_mc_tku_tracks_5pt += len(tracks_tku_5pt)
            self.fdata.n_mc_tku_tracks_4to5pt += len(tracks_tku_4to5pt)
            self.fdata.n_mc_tku_tracks_3to5pt += len(tracks_tku_3to5pt)
        if bool_tkds_5spoint_event:
            self.fdata.n_mc_tkd_tracks_5pt += len(tracks_tkd_5pt)
            self.fdata.n_mc_tkd_tracks_4to5pt += len(tracks_tkd_4to5pt)
            self.fdata.n_mc_tkd_tracks_3to5pt += len(tracks_tkd_3to5pt)

        # Find the number of tracks actually reconstructed by pat rec
        # ***********************************************************

        stracks_tku = [trk for trk in sfevent.straightprtracks() \
          if trk.get_tracker() == 0]
        stracks_tkd = [trk for trk in sfevent.straightprtracks() \
          if trk.get_tracker() == 1]
        htracks_tku = [trk for trk in sfevent.helicalprtracks() \
          if trk.get_tracker() == 0]
        htracks_tkd = [trk for trk in sfevent.helicalprtracks() \
          if trk.get_tracker() == 1]

        # Do these tracks come from 1 MC track each? Are there mixed tracks?
        tracks_tku = htracks_tku + stracks_tku
        tracks_tkd = htracks_tkd + stracks_tkd

        results_tku_5pt = self.check_tracks(lkup, 5, tracks_tku)
        results_tkd_5pt = self.check_tracks(lkup, 5, tracks_tkd)
        results_tku_4pt = self.check_tracks(lkup, 4, tracks_tku)
        results_tkd_4pt = self.check_tracks(lkup, 4, tracks_tkd)
        results_tku_3pt = self.check_tracks(lkup, 3, tracks_tku)
        results_tkd_3pt = self.check_tracks(lkup, 3, tracks_tkd)

        # Update internal counters
        if bool_tkus_5spoint_event:
            self.fdata.n_rec_tku_tracks_5pt_good += results_tku_5pt[0]
            self.fdata.n_rec_tku_tracks_4to5pt_good += results_tku_4pt[0]
            self.fdata.n_rec_tku_tracks_3to5pt_good += results_tku_3pt[0]
        if bool_tkds_5spoint_event:
            self.fdata.n_rec_tkd_tracks_5pt_good += results_tkd_5pt[0]
            self.fdata.n_rec_tkd_tracks_4to5pt_good += results_tkd_4pt[0]
            self.fdata.n_rec_tkd_tracks_3to5pt_good += results_tkd_3pt[0]

        # Update histos, require that only 1 MC track is present
        if bool_tkus_5spoint_event:
            # Need the 2nd index to get track id rather than particle event id
            track_id = tracks_tku_5pt[0]
            mom = \
              tools.find_mc_momentum_sfhits(lkup, spoints_tku, track_id, 0)
            if (results_tku_3pt[0] < 1): # Fill if no rec track found
                self.hpt_m_tku.Fill(math.sqrt(mom[0]**2 + mom[1]**2))
                self.hpz_m_tku.Fill(mom[2])

        if bool_tkds_5spoint_event:
            track_id = tracks_tkd_5pt[0]
            mom = \
              tools.find_mc_momentum_sfhits(lkup, spoints_tkd, track_id, 1)
            if (results_tkd_3pt[0] < 1): # Fill if no rec track found
                self.hpt_m_tkd.Fill(math.sqrt(mom[0]**2 + mom[1]**2))
                self.hpz_m_tkd.Fill(mom[2])

        return True

    def accumulate_average_data(self):
        """ Accumulate data for averging over job """
        self.adata += self.fdata

    def calculate_file_efficiency(self):
        """ Calculate the file efficiency """
        self.fdata.calculate_efficiency()
        return [0.0, 0.0, 0.0, 0.0, 0.0, 0.0] # NOTE Placeholder

    def calculate_average_efficiency(self):
        """ Calculate the average job efficiency """
        self.adata.calculate_efficiency()

    def check_tracks(self, lkup, nstations, recon_tracks):
        """ Are the recon tracks found good tracks wrt the MC data?
            nstations is the number of seed spacepoints originating from
            the same MC track required for a recon track to be classed as
            good. Hence if nstations = the actual number of tracker stations,
            only perfect recon tracks will count as good """
        n_good_tracks = 0 # tracks which can be associated with an MC track
        # tracks generated from spoints from different MC tracks
        n_mixed_tracks = 0
        n_bad_tracks = 0 # tracks with mixed spoints or < expected spoints
        for trk in recon_tracks:
            spoints = trk.get_spacepoints_pointers()
            mc_tids = tools.find_mc_tracks_from_spoints(lkup, spoints, \
              nstations, self.n_hits_cut)
            if len(mc_tids) == 1:
                n_good_tracks = n_good_tracks + 1
            elif len(mc_tids) > 1:
                n_mixed_tracks = n_mixed_tracks + 1
            elif len(mc_tids) < 1:
                n_bad_tracks = n_bad_tracks + 1
        return n_good_tracks, n_mixed_tracks, n_bad_tracks

    def clear(self):
        """ Set the internal file data counters to zero & booleans to false """
        self.fdata.clear()

    def print_info(self, data_name, info_type='file'):
        """ Print the results """
        if info_type == 'file':
            edata = self.fdata
        elif info_type == 'average':
            edata = self.adata

        if self.n_print_calls == 0:
            # print '\nRecon_evt\tTUMC5\tTURG5\tTUMC45\tTURG45\tTUMC35\tTURG35',
            # print '\tTDMC5\tTDRG5\tTDMC45\tTDRG45\tTDMC35\tTDRG35\tFile'
            print '\nRecon_evt\tTUMC5\tTURG5\tTURG45\tTURG35',
            print '\tTDMC5\tTDRG5\tTDRG45\tTDRG35\tFile'

        print str(edata.n_events_total) + '\t\t',
        print str(edata.n_mc_tku_tracks_5pt) + '\t' + \
          str(edata.n_rec_tku_tracks_5pt_good) + '\t' + \
          str(edata.n_rec_tku_tracks_4to5pt_good) + '\t' + \
          str(edata.n_rec_tku_tracks_3to5pt_good) + '\t' + \
          str(edata.n_mc_tkd_tracks_5pt) + '\t' + \
          str(edata.n_rec_tkd_tracks_5pt_good) + '\t' + \
          str(edata.n_rec_tkd_tracks_4to5pt_good) + '\t' + \
          str(edata.n_rec_tkd_tracks_3to5pt_good) + '\t' + \
          os.path.basename(data_name)
        self.n_print_calls += 1

    def print_welcome(self):
        """ Message to be printed at programme start """
        print '\nPattern Recognition Efficiency Calculator (MC)'
        print '************************************************\n'

        # print 'Parameters:'
