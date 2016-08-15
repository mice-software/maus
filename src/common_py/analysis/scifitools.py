#!/usr/bin/env python

"""  Check tracker efficiency """

import sys
import os
import ROOT
import libMausCpp #pylint: disable = W0611

def find_mc_track(hits, id_frequency_cut=0.5):
    """ Look to see if a single mc track id occurs in these hits
        more than 50% (by default) of the time, and if so return
        that track """

    # Loop over all spoints, then clusters, then digits, then scifi hits
    mc_track_counter = {} # Dict mapping track id to frequency it occurs
    for hit in hits:
        mc_track_id = hit.GetTrackId()
        if mc_track_id in mc_track_counter:
            mc_track_counter[mc_track_id] += 1
        else:
            mc_track_counter[mc_track_id] = 1

    # Does any one mc track id appear more than a factor of
    # (id_frequency_cut)of the time?
    most_frequent_id = 0
    highest_counter = 0
    for mc_track_id, counter in mc_track_counter.iteritems():
        if counter > highest_counter:
            most_frequent_id = mc_track_id
            highest_counter = counter

    # If such a track id was found, set the mc track
    mc_track = None
    if  mc_track_counter[most_frequent_id] > \
      (len(all_hits) * id_frequency_cut):
        for track in mc_evt.GetTracks():
            if most_frequent_id == track.GetTrackId():
                mc_track = track
                break

    return mc_track

def find_mc_hits(lkup, spoints, plane=-1, station=-1, tracker=-1):
    """ Find the mc hits used create the spoints,
        optional arguments to cut on tracker, station, plane """
    all_hits = [] # All the mc hits reconstructed in this trcker in this evt
    for sp in spoints:
        if (tracker != -1) and (sp.get_tracker() != tracker):
            continue
        if (station != -1) and (sp.get_station() != station):
            continue
        for clus in sp.get_channels_pointers():
            if (plane != -1) and (clus.get_plane() != plane):
                continue
            for dig in clus.get_digits_pointers():
                hits = lkup.get_hits(dig)
                all_hits += hits
    return all_hits

def find_mc_momentum(lkup, spoints, mc_track_id, trker_num):
    """ Find the mc truth momentum of the track that made the
        spacepoints at the tracker reference surface """
    hits = find_mc_hits(lkup, spoints, trker_num, 5, 0)
    num_matched_hits = 0
    px = 0
    py = 0
    pz = 0
    for hit in hits:
        if hit.GetTrackId() == mc_track_id:
            num_matched_hits += 1
            px += hit.GetMomentum().x()
            py += hit.GetMomentum().y()
            pz += hit.GetMomentum().z()
    if num_matched_hits != 0:
      px = px / num_matched_hits
      py = py / num_matched_hits
      pz = pz / num_matched_hits
    return px, py, pz

class SciFiLookup:
    """ Class to link SciFi recon data to the MC truth """
    def __init__(self, mc_evt):
        """ Initialise the lookup (just declare a few members) """
        self.hits_map = self.make_hits_map(mc_evt)

    def clear_maps(self):
        """ Set the member variables to empty """
        self.hits_map = {}

    @staticmethod
    def get_digit_id(digit):
        """ Calculate and eturn the id number of the digit """
        digit_id = digit.get_channel() + (1000 * digit.get_plane()) \
          + (10000 * digit.get_station()) \
          + (100000 * digit.get_tracker()) \
          + (1000000 * digit.get_event())
        return digit_id

    def get_hits(self, digit):
        """ Return the scifi hits used to create the digit """
        digit_id = self.get_digit_id(digit)
        return self.hits_map[digit_id]

    def make_hits_map(self, mc_evt):
        """ Create a dictionary linking the channel id
            with the scifi hits in that channel """
        self.hits_map = {}
        hits = mc_evt.GetSciFiHits()
        for hit in hits:
            channel_id = hit.GetChannelId().GetID()
            if not (channel_id in self.hits_map):
                self.hits_map[channel_id] = [hit]
            else:
                self.hits_map[channel_id].append(hit)
