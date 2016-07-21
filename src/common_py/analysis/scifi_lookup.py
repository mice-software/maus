#!/usr/bin/env python

"""  Check tracker efficiency """

import sys
import os
import ROOT
import libMausCpp #pylint: disable = W0611

class SciFiLookup():
    """ Class to link SciFi recon data to the MC truth """
    __init__(self):
        """ Initialise the lookup (just declare a few members) """
        hits_map = {}
        noise_map = {}

    clear_maps(self):
        """ Set the member variables to empty """
        hits_map = {}
        noise_map = {}

    get_digit_id(self, digit):
        """ Calculate and eturn the id number of the digit """
        digit_id = dig.get_channel() + (1000 * dig.get_plane()) \
          + (10000 * dig.get_station()) \
          + (100000 * dig.get_tracker()) \
          + (1000000 * dig.get_event())
        return digit_id

    get_hits(self, digit):
        """ Return the scifi hits used to create the digit """
        digit_id = self.get_digit_id(digit)
        return self.hits_map[digit_id]

    get_noise(self, digit):
        """ Return the scifi noise hits used to create the digit """
        digit_id = self.get_digit_id(digit)
        return self.noise_map[digit_id]

    make_hits_map(self, mc_evt):
        """ Create a dictionary linking the channel id
            with the scifi hits in that channel """
        hits = mc_evt.GetSciFiHits()
        for hit in hits:
            channel_id = hit.GetChannelId()->GetID()
            if not (channel_id in hits_map):
                self.hits_map[channel_id] = [hit]
            else:
                self.hits_map[channel_id].append(hit)

    make_noise_hits_map(self, mc_evt):
        """ Create a dictionary linking the channel id
            with the scifi noise hits in that channel """
        hits = mc_evt.GetSciFiNoiseHits()
        for hit in hits:
            channel_id = hit.GetChannelId()->GetID()
            if not (channel_id in hits_map):
                self.noise_map[channel_id] = [hit]
            else:
                self.noise_map[channel_id].append(hit)
