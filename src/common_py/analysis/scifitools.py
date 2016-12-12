#!/usr/bin/env python

"""  Some tools for tracker MC data analysis """

import os
import libMausCpp #pylint: disable = W0611

def load_data(files):
    """ Load data from files. If a dir is given, search recursively """
    if type(files) is not list:
        files = [files]

    root_files = []
    for file_name in files:
        # Check if file_name is a ROOT file
        if os.path.isfile(file_name):
            #pylint: disable = W0612
            file_suffix, file_extension = os.path.splitext(file_name)
            if file_extension == '.root':
                root_files.append(file_name)
            else:
                print 'Bad file name, aborting'
                return root_files

        # If file_name is a directory, walk it and save any ROOT files found
        if os.path.isdir(file_name):
            root_files_dir_search(file_name, root_files)
        if len(root_files) < 1:
            print 'No data files found'
            return root_files

    print '\nFound ' + str(len(root_files)) + ' ROOT files:'
    for rfile in root_files:
        print rfile
    return root_files

def root_files_dir_search(top_dir, root_files):
    """ Appends any ROOT files found in the directory to root_files """
    #pylint: disable = W0612
    for dir_name, subdir_list, file_list in os.walk(top_dir):
        print('Searching directory: %s' % dir_name)
        for fname in file_list:
            file_suffix, file_extension = os.path.splitext(fname)
            if file_extension == '.root':
                print('\t%s' % fname)
                root_files.append(dir_name + '/' + fname)

def vector_to_list(vec):
    """ Convert a std vector to a python list """
    pylist = []
    for i in range(vec.size()):
        pylist.append(vec[i])
    return pylist

def find_mc_track(hits, n_hits_cut=5):
    """ Any MC track ids which produce (n_hits_cut) hits or more
        are accepted and returned """

    # Loop over all spoints, then clusters, then digits, then scifi hits
    track_counter = {} # Dict mapping track id pair to frequency occurs
    for hit in hits:
        track_id = int(hit.GetTrackId())
        if track_id in track_counter:
            track_counter[track_id] += 1
        else:
            track_counter[track_id] = 1

    accepted_tracks = []
    for tid, counter in track_counter.iteritems():
        if counter >= n_hits_cut:
            accepted_tracks.append(tid)
    return accepted_tracks

def find_mc_hits(lkup, spoints, plane=-1, station=-1, tracker=-1):
    """ Find the mc hits used create the spoints,
        optional arguments to cut on tracker, station, plane """
    all_hits = [] # All the mc hits reconstructed in this trcker in this evt
    for spoint in spoints:
        if (tracker != -1) and (spoint.get_tracker() != tracker):
            continue
        if (station != -1) and (spoint.get_station() != station):
            continue
        for clus in spoint.get_channels_pointers():
            if (plane != -1) and (clus.get_plane() != plane):
                continue
            for dig in clus.get_digits_pointers():
                hits = lkup.get_hits(dig)
                all_hits += hits
    return all_hits

def find_mc_momentum_sfhits(lkup, spoints, track_id, trker_num):
    """ Find the mc truth momentum of the track that made the
        spacepoints at the tracker reference surface """
    hits = find_mc_hits(lkup, spoints, 0, 1, trker_num)
    num_matched_hits = 0
    #pylint: disable = C0103
    px = 0
    py = 0
    pz = 0
    for hit in hits:
        if hit.GetTrackId() == track_id:
            num_matched_hits += 1
            px += hit.GetMomentum().x()
            py += hit.GetMomentum().y()
            pz += hit.GetMomentum().z()
    if num_matched_hits != 0:
        px = px / num_matched_hits
        py = py / num_matched_hits
        pz = pz / num_matched_hits
    return px, py, pz

def find_mc_tracks_from_spoints(lkup, spoints, nstations=5, n_hits_cut=5):
    """ Find how many MC tracks produced a set of spacepoints which could be
        reconstructed to pat rec tracks. 
        A spacepoint is identified with a track if (n_hits_cut) or more scifi
        hits produced it (mutliple tracks can be associated with one spoint).
        Then any track ids that are associated with spacepoints in (nstations)
        or more stations are returned as good MC tracks.
    """

    # Dict to hold what stations the track caused spoints in, & how many spoints
    track_ids = {}

    for spoint in spoints:
        station = spoint.get_station() # station number
        hits = find_mc_hits(lkup, [spoint]) #  The hits which formed the spoint
        found_tracks = find_mc_track(hits, n_hits_cut) # list of tuples

        for trk in found_tracks:
            if trk != -1:
                if not trk in track_ids:
                    track_ids[trk] = {station : 1}
                elif not station in track_ids[trk]:
                    track_ids[trk][station] = 1
                else:
                    track_ids[trk][station] = \
                      track_ids[trk][station] + 1

    good_tracks = []
    # if nstations == 5: print 'Found ' + str(len(track_ids)) + ' tracks, ',
    for tid, stations_hit in track_ids.iteritems():
        # if this track formed a spoint in at least
        # the number of stations specified
        if (len(stations_hit)) >= float(nstations):
            good_tracks.append(tid)
    return good_tracks

class SciFiLookup:
    """ Class to link SciFi recon data to the MC truth """
    def __init__(self, mc_evt):
        """ Initialise the lookup (just declare a few members) """
        self.hits_map = {}
        self.make_hits_map(mc_evt)
        #if not self.hits_map:
            # print "WARNING: SciFiLookup: Building hits map failed"
        #elif len(self.hits_map) < 1:
            #print "WARNING: SciFiLookup: Hits map has 0 size"

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
            channel_id = int(hit.GetChannelId().GetID())
            if not (channel_id in self.hits_map):
                # print 'Adding new id ' + str(channel_id)
                self.hits_map[channel_id] = [hit]
                # print self.hits_map[channel_id]
            else:
                # print 'Adding to id ' + str(channel_id)
                self.hits_map[channel_id].append(hit)
                # print self.hits_map[channel_id]
