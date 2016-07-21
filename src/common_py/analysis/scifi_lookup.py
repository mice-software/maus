#!/usr/bin/env python

"""  Check tracker efficiency """

import sys
import os
import ROOT
import libMausCpp #pylint: disable = W0611

class SciFiLookup():
    """ Class to link SciFi recon data to the MC truth """
    def __init__(self):
        """ Initialise the lookup (just declare a few members) """
        self.hits_map = {}
        self.noise_map = {}

    def clear_maps(self):
        """ Set the member variables to empty """
        self.hits_map = {}
        self.noise_map = {}

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

    def get_noise(self, digit):
        """ Return the scifi noise hits used to create the digit """
        digit_id = self.get_digit_id(digit)
        return self.noise_map[digit_id]

    def make_hits_map(self, mc_evt):
        """ Create a dictionary linking the channel id
            with the scifi hits in that channel """
        hits = mc_evt.GetSciFiHits()
        for hit in hits:
            channel_id = hit.GetChannelId().GetID()
            if not (channel_id in self.hits_map):
                self.hits_map[channel_id] = [hit]
            else:
                self.hits_map[channel_id].append(hit)

    def make_noise_hits_map(self, mc_evt):
        """ Create a dictionary linking the channel id
            with the scifi noise hits in that channel """
        hits = mc_evt.GetSciFiNoiseHits()
        for hit in hits:
            channel_id = hit.GetChannelId().GetID()
            if not (channel_id in self.noise_map):
                self.noise_map[channel_id] = [hit]
            else:
                self.noise_map[channel_id].append(hit)
