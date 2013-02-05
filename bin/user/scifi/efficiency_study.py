#!/usr/bin/env python

"""
Example to load a ROOT file with SciFi data and do some analysis
"""

import sys
from math import fabs

# basic PyROOT definitions
import ROOT 

# definitions of MAUS data structure for PyROOT
import libMausCpp #pylint: disable = W0611

# pylint: disable = E1101
# pylint: disable = R0914
# pylint: disable = R0915
# pylint: disable = C0103

delta_x = 2
delta_y = 2

def main(file_name):
    """ Main function which performs the analysis """

    # Load the ROOT file
    print "Loading ROOT file", file_name
    root_file = ROOT.TFile(file_name, "READ") # pylint: disable = E1101

    # Set up the data tree to be read by ROOT IO
    print "Setting up data tree"
    data = ROOT.MAUS.Data() # pylint: disable = E1101
    tree = root_file.Get("Spill")
    tree.SetBranchAddress("data", data)

    # Set up an output ascii file
    f1 = open('efficiency_study_output.dat', 'w')
    header = 'spill\tevent\tmctrack\tpid\ttracker\tmatched\tmissed\twrong\n'
    f1.write( header )

    # Loop over spills
    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        spill = data.GetSpill()
        print_spill_info(spill)
        if not check_spill(spill):
            continue

        # Loop over MC events
        for evt_num in range(spill.GetMCEvents().size()):
            mc_evt = spill.GetMCEvents()[evt_num]
            print ' Looking at MC event ' + str(evt_num)
            # mc_trks = mc_evt.GetTracks() # Make dict from track_id to trk
            sf_hits = mc_evt.GetSciFiHits()
            hits_by_track = sort_hits_by_track(sf_hits)

            sf_evt = spill.GetReconEvents()[evt_num].GetSciFiEvent()
            if not check_tracks(sf_evt):
                print ' Bad recon tracks, skipping'
                continue

            sorted_rec_trks = sort_rec_tracks(sf_evt.helicalprtracks())

            # Loop over MC track ids (a substitute for MC tracks)
            for trk_num, hits_in_trk in hits_by_track.iteritems():
                if not trk_num == 1:
                    break

                # Make a nested dictionary of tracker and station num
                # to track point (1 per station)
                sorted_tp = make_mc_track_pts(mc_evt)

                # Loop over trackers
                for trker_num in range(2):
                    print "  Track number: " + str(trk_num),
                    print " Tracker number: " + str(trker_num) +", containing ",
                    print str(len(sorted_rec_trks[trker_num])) + " rec tracks"

                    # Loop over recon tracks (one per tracker for now)
                    sp_matched = 0
                    for rec_trk in sorted_rec_trks[trker_num]:

                        # Should check this matches the MC track here somehow

                        seeds_in_track = rec_trk.get_spacepoints()
                        sorted_seeds = sort_spoints(seeds_in_track)

                        # Should check here that there is only 0 or 1
                        # seeds per tracker/station, otherwise break
                        print '  Found ' +str(len(sorted_tp[trker_num])),
                        print ' track points'
                        # Loop over track points
                        for t_num, tracker in sorted_tp.iteritems():
                            for s_num, tp in tracker.iteritems():
                                # Loop over spoints in same tracker and stat
                                seeds = sorted_seeds[t_num][s_num]
                                matched = False
                                for seed in seeds:
                                    matched = check_match(tp, seed)
                                    if matched:
                                        sp_matched = sp_matched + 1
                                        break

                        f1.write( str(spill.GetSpillNumber()) + '\t' )
                        f1.write( str(evt_num) + '\t' )
                        tid = hits_in_trk[0].GetTrackId()
                        f1.write( str(tid) + '\t' )

                        f1.write( str(trker_num) + '\t' )
                        f1.write( str(sp_matched) + '\t' )
                        f1.write( str(5 - sp_matched) + '\t' )
                        f1.write( str(len(seeds_in_track) - sp_matched) + '\n' )

    print "Closing files"
    root_file.Close()
    f1.close()

def check_match(tp, sp):
    """ Check whether a track point matches a spoint"""
    tp_pos = tp.get_position()
    sp_pos = sp.get_position()
    match = True

    if ( fabs(fabs(tp_pos.x()) - fabs(sp_pos.x())) > delta_x ):
        match = False
    if ( fabs(fabs(tp_pos.y()) - fabs(sp_pos.y())) > delta_y ):
        match = False

    s1 = "   dx is " + str( fabs( fabs(tp_pos.x()) - fabs(sp_pos.x())))
    s2 = "   dy is " + str( fabs( fabs(tp_pos.y()) - fabs(sp_pos.y())))
    print s1 + ", " + s2
    return match

def check_spill(spill):
    """ Check the spill object is good """
    if spill.GetDaqEventType() != "physics_event":
        print "Not a physics event"
        return False
    else:
        return True

def check_tracks(sf_evt):
    """ Check the sf evt contains tracks we can use for the analysis """
    # For now, enforce 1 and only 1 track per tracker
    ok = True
    if len(sf_evt.helicalprtracks()) != 2:
        print "Wrong number of helical tracks"
        ok = False
    else:
        trk1 = sf_evt.helicalprtracks()[0]
        trk2 = sf_evt.helicalprtracks()[1]
        if trk1.get_tracker() == trk2.get_tracker():
            print "Only have tracks in one tracker"
            ok = False
    return ok

def make_mc_track_pts(mc_evt):
    """ Make MC track points for the tracker stations using sf hits """
    # Sort hits by tracker then station number into two nested dicts
    sorted_hits = sort_hits(mc_evt.GetSciFiHits())

    # Make a dictionary to hold the track points by tracker
    tp_by_tracker = {}

    # Loop over trackers
    for t_num, tracker in sorted_hits.iteritems():
        tp_by_station = {}

        # Loop over stations
        for s_num, station in tracker.iteritems():
            print "  Forming track point in tracker" + str(t_num),
            print " station " + str(s_num) + " containing ",
            print str(len(station)) + " hits"
            if len(station) < 1:
                continue
            # Use the SpacePoint class for track points, as is near enough
            tp = ROOT.MAUS.SciFiSpacePoint()
            # Look for a hit corresponding to the primary track and
            # then make the track point from it 
            for hit in station:
                if hit.GetTrackId() == 1:
                    tp.set_position(hit.GetPosition())
                    tp.set_station(hit.GetChannelId().GetStationNumber())
                    tp.set_tracker(hit.GetChannelId().GetTrackerNumber())
                    tp_by_station[s_num] = tp
                    break

        tp_by_tracker[t_num] = tp_by_station
    return tp_by_tracker

def print_spill_info(spill):
    """ Print some basic information about the spill """
    print "Found spill number", spill.GetSpillNumber(),
    print "in run number", spill.GetRunNumber(),
    print "DAQ event type", spill.GetDaqEventType()

def sort_hits(sf_hits):
    """ Sort hits by tracker then station number into two nested dicts """
    # Sort hits by tracker into a dictionary
    hits_by_tracker = {0: [], 1:[]}
    for hit in sf_hits:
        hits_by_tracker[hit.GetChannelId().GetTrackerNumber()].append(hit)

    # Sort hits by tracker then by station into a dictionary
    sorted_hits = {}
    for num, tracker in hits_by_tracker.iteritems():
        hits_by_station = {1: [], 2: [], 3: [], 4: [], 5: []}
        for hit in tracker:
            hits_by_station[hit.GetChannelId().GetStationNumber()].append(hit)
        sorted_hits[num] = hits_by_station
    
    return sorted_hits

def sort_rec_tracks(trks):
    """ Sort tracks according to tracker """
    sorted_tracks = {0: [], 1: []}
    for trk in trks:
        sorted_tracks[trk.get_tracker()].append(trk)
    return sorted_tracks

def sort_spoints(spoints):
    """ Sort spoints by tracker then station number into two nested dicts """
    # Sort spoints by tracker into a dictionary
    spoints_by_tracker = {0: [], 1:[]}
    for sp in spoints:
        spoints_by_tracker[sp.get_tracker()].append(sp)

    # Sort spoints by tracker then by station into a dictionary
    sorted_spoints = {}
    for num, tracker in spoints_by_tracker.iteritems():
        spoints_by_station = {1: [], 2: [], 3: [], 4: [], 5: []}
        for sp in tracker:
            spoints_by_station[sp.get_station()].append(sp)
        sorted_spoints[num] = spoints_by_station

    return sorted_spoints

def sort_hits_by_track(sf_hits):
    """ Sort hits into a dictionary by MC track number """
    hits_by_trk = {}
    # print 'Sorting ' + str(sf_hits.size()) + ' sf hits'
    for i in range(sf_hits.size()):
        hit = sf_hits[i]
        if hit.GetTrackId() in hits_by_trk:
            hits_by_trk[hit.GetTrackId()].append(hit)
        else:
            hits_by_trk[hit.GetTrackId()] = [hit]

    for trk_num, trk in hits_by_trk.iteritems():
        print "  Track " + str(trk_num) + " has " + str(len(trk)) + " sf hits"
    return hits_by_trk

if __name__ == "__main__":
    main(sys.argv[1])

