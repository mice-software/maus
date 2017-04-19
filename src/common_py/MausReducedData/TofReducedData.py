#!/usr/bin/env python

""" TOF Reduced Data objects for easy plotting with PyROOT """

import array

# pylint: disable = C0103
# pylint: disable = E1101
# pylint: disable = R0902
# pylint: disable = R0903
# pylint: disable = R0912
# pylint: disable = R0913
# pylint: disable = R0914
# pylint: disable = R0915

class TOF:
    """ Class holding reduced data for one TOF detector
        Uses arrays to make compatible with ROOT plotting.
        May accumulate data over many events. """

    def __init__(self):
        """ Constructor, set everything to empty or zero """
        self.tof_num = 0
        self.num_spoints = 0
        self.spoints_global_x = array.array('d')
        self.spoints_global_y = array.array('d')
        self.spoints_global_z = array.array('d')

    def accumulate_data(self, evt, tof_num):
        """ Add data from an event """
        self.tof_num = tof_num
        try:
            if self.tof_num == 0:
                spoints = \
                  evt.GetTOFEventSpacePoint().GetTOF0SpacePointArray()
            elif self.tof_num == 1:
                spoints = \
                  evt.GetTOFEventSpacePoint().GetTOF1SpacePointArray()
            elif self.tof_num == 2:
                spoints = \
                  evt.GetTOFEventSpacePoint().GetTOF2SpacePointArray()
        except ReferenceError:
            spoints = []
        for sp in spoints:
            x = sp.GetGlobalPosX()
            y = sp.GetGlobalPosY()
            z = sp.GetGlobalPosZ()
            self.spoints_global_x.append(x)
            self.spoints_global_y.append(y)
            self.spoints_global_z.append(z)
            self.num_spoints += 1

    def clear(self):
        """ Clear all accumulated data """
        self.tof_num = 0
        self.num_spoints = 0
        self.spoints_global_x = array.array('d')
        self.spoints_global_y = array.array('d')
        self.spoints_global_z = array.array('d')
