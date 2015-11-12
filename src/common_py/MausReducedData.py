#!/usr/bin/env python

""" MAUS Reduced Data objects for easy plotting with PyROOT """

import array
import ROOT

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
        if self.tof_num == 0:
            spoints = evt.GetTOFEventSpacePoint().GetTOF0SpacePointArray()
        elif self.tof_num == 1:
            spoints = evt.GetTOFEventSpacePoint().GetTOF1SpacePointArray()
        elif self.tof_num == 2:
            spoints = evt.GetTOFEventSpacePoint().GetTOF2SpacePointArray()
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

class Tracker:
    """ Class holding data for one tracker.
        Uses arrays to make compatible with ROOT plotting.
        May accumulate data over many events. """

    def __init__(self):
        """ Constructor, set everything to empty or zero """
        self.trker_num = 0
        self.num_events = 0
        self.num_digits = 0
        self.num_clusters = 0
        self.num_spoints = 0
        self.num_stracks = 0
        self.num_htracks = 0
        self.spoints_x = array.array('d')
        self.spoints_y = array.array('d')
        self.spoints_z = array.array('d')
        self.spoints_global_x = array.array('d')
        self.spoints_global_y = array.array('d')
        self.spoints_global_z = array.array('d')
        self.seeds_x = []
        self.seeds_y = []
        self.seeds_z = []
        self.seeds_global_x = []
        self.seeds_global_y = []
        self.seeds_global_z = []
        self.seeds_phi = []
        self.seeds_s = []
        self.straight_xz_fits = []
        self.straight_yz_fits = []
        self.helix_xy_fits = []
        self.helix_xz_fits = []
        self.helix_yz_fits = []
        self.helix_sz_fits = []

    def accumulate_data(self, evt, trker_num):
        """ Add data from a scifi event """
        self.trker_num = trker_num
        self.num_events = self.num_events + 1

        # Add on the number of digits and clusters in this tracker
        for dig in evt.digits():
            if dig.get_tracker() == trker_num:
                self.num_digits = self.num_digits + 1
        for clus in evt.clusters():
            if clus.get_tracker() == trker_num:
                self.num_clusters = self.num_clusters + 1

        # Loop over all spacepoints and pull out data to arrays
        spoints = evt.spacepoints()
        for sp in spoints:
            x = sp.get_position().x()
            y = sp.get_position().y()
            z = sp.get_position().z()
            gx = sp.get_global_position().x()
            gy = sp.get_global_position().y()
            gz = sp.get_global_position().z()
            if sp.get_tracker() == trker_num:
                self.spoints_x.append(x)
                self.spoints_y.append(y)
                self.spoints_z.append(z)
                self.spoints_global_x.append(gx)
                self.spoints_global_y.append(gy)
                self.spoints_global_z.append(gz)
                self.num_spoints = self.num_spoints + 1

        # Loop over straight tracks and pull out data
        for trk in evt.straightprtracks():
            if trk.get_tracker() == trker_num:
                self.num_stracks = self.num_stracks + 1
                x0 = trk.get_x0()
                mx = trk.get_mx()
                y0 = trk.get_y0()
                my = trk.get_my()
                if trker_num == 0:
                    self.straight_xz_fits.append(self.make_line(mx, x0, \
                                                                 0, 1200))
                    self.straight_yz_fits.append(self.make_line(my, y0, \
                                                                 0, 1200))
                else:
                    self.straight_xz_fits.append(self.make_line(mx, x0, \
                                                                 0, 1200))
                    self.straight_yz_fits.append(self.make_line(my, y0, \
                                                                 0, 1200))

        # Loop over helical tracks and pull out data
        for trk in evt.helicalprtracks():
            if trk.get_tracker() == trker_num:
                pt = trk.get_R()*1.2
                pz = pt / trk.get_dsdz()
                print '\nTracker ' + str(self.trker_num) + ',',
                print 'Track ' + str(self.num_htracks) + ',',
                print 'rad = ' + '%.2f' % trk.get_R() + 'mm,',
                print 'X0 = ' + '%.2f' % trk.get_circle_x0() + 'mm,',
                print 'Y0 = ' + '%.2f' % trk.get_circle_y0() + 'mm,',
                print 'dsdz = ' + '%.4f' % trk.get_dsdz() + ', ',
                print 'pt = ' + '%.2f' % pt + 'MeV/c' + ',',
                print 'pz = ' + '%.2f' % pz + 'MeV/c',
                print 'xy_chi2 = ' + '%.4f' % trk.get_circle_chisq(),
                print 'sz_c = ' + '%.4f' % trk.get_line_sz_c(),
                print 'sz_chi2 = ' + '%.4f' % trk.get_line_sz_chisq()
                print 'x\ty\tz\tphi\tgx\tgy\tgz'
                # Pull out the circle fit data
                x0 = trk.get_circle_x0()
                y0 = trk.get_circle_y0()
                dsdz = trk.get_dsdz()
                # if ( trker_num == 0 ):
                #    dsdz = -dsdz  # Needed due to the way we plot
                rad = trk.get_R()
                self.helix_xy_fits.append(self._make_circle(x0, y0, rad))
                # Pull out the turning angles of each seed spacepoint
                phi_per_trk = array.array('d')
                s_per_trk = array.array('d')
                num_phi = 0
                for i, phi in enumerate(trk.get_phi()):
                    if num_phi == 0:
                        phi_per_trk[0] = phi
                        s_per_trk[0] = phi * rad
                        num_phi = num_phi + 1
                    else:
                        phi_per_trk.append(phi)
                        s_per_trk.append(phi*rad)
                        num_phi = num_phi + 1
                    pos = trk.get_spacepoints()[i].get_position()
                    gpos = trk.get_spacepoints()[i].get_global_position()
                    print '%.2f' % pos.x() + '\t',
                    print '%.2f' % pos.y() + '\t',
                    print '%.2f' % pos.z() + '\t',
                    print '%.2f' % phi + '\t',
                    print '%.2f' % gpos.x() + '\t',
                    print '%.2f' % gpos.y() + '\t',
                    print '%.2f' % gpos.z() + '\t'
                self.seeds_phi.append(phi_per_trk)
                self.seeds_s.append(s_per_trk)

                # Pull out the coords of each seed spacepoint
                x_per_trk = array.array('d')
                y_per_trk = array.array('d')
                z_per_trk = array.array('d')
                gx_per_trk = array.array('d')
                gy_per_trk = array.array('d')
                gz_per_trk = array.array('d')
                num_seeds = 0
                for seed in trk.get_spacepoints():
                    x_per_trk.append(seed.get_position().x())
                    y_per_trk.append(seed.get_position().y())
                    z_per_trk.append(seed.get_position().z())
                    gx_per_trk.append(seed.get_global_position().x())
                    gy_per_trk.append(seed.get_global_position().y())
                    gz_per_trk.append(seed.get_global_position().z())
                    num_seeds = num_seeds + 1
                self.seeds_x.append(x_per_trk)
                self.seeds_y.append(y_per_trk)
                self.seeds_z.append(z_per_trk)
                self.seeds_global_x.append(gx_per_trk)
                self.seeds_global_y.append(gy_per_trk)
                self.seeds_global_z.append(gz_per_trk)

                sz_c = trk.get_line_sz_c()
                self.helix_xz_fits.append(self._make_helix_xz(x0, rad, dsdz, \
                                          sz_c, 0, 1200))
                self.helix_yz_fits.append(self._make_helix_yz(y0, rad, dsdz, \
                                          sz_c, 0, 1200))
                self.helix_sz_fits.append(self._make_line(dsdz, sz_c, 0, 1200))
                self.num_htracks = self.num_htracks + 1

    @staticmethod
    def make_circle(x0, y0, rad):
        """ Make a circle from its centre coords & radius using TArc class """
        arc = ROOT.TArc(x0, y0, rad)
        arc.SetFillStyle(0)   # 0 - Transparent
        arc.SetLineColor(ROOT.kBlue)
        return arc

    @staticmethod
    def make_helix_xz(circle_x0, rad, dsdz, sz_c, zmin, zmax):
        """ Make a function for the x-z projection of the helix """
        # The x in the cos term is actually representing z (the indep variable)
        func = ROOT.TF1("xz_func", "[0]-([1]*cos((1/[1])*([2]*x+[3])))", \
                        zmin, zmax)
        func.SetParameter(0, circle_x0)
        func.SetParameter(1, rad)
        func.SetParameter(2, dsdz)
        func.SetParameter(3, sz_c)
        func.SetLineColor(ROOT.kBlue)
        return func

    @staticmethod
    def make_helix_yz(circle_y0, rad, dsdz, sz_c, zmin, zmax):
        """ Make a function for the y-z projection of the helix """
        # The x in the cos term is actually representing z (the indep variable)
        func = ROOT.TF1("yz_func", "[0]+([1]*sin((1/[1])*([2]*x+[3])))", \
                        zmin, zmax)
        func.SetParameter(0, circle_y0)
        func.SetParameter(1, rad)
        func.SetParameter(2, dsdz)
        func.SetParameter(3, sz_c)
        func.SetLineColor(ROOT.kBlue)
        return func

    @staticmethod
    def make_line(m, c, xmin, xmax):
        """ Create a straight line using the ROOT TF1 class """
        line = ROOT.TF1("line", "[0]*x+[1]", xmin, xmax)
        line.SetParameter(0, m)
        line.SetParameter(1, c)
        line.SetLineColor(ROOT.kBlue)
        return line

    def clear(self):
        """ Clear all accumulated data """
        self.num_events = 0
        self.num_digits = 0
        self.num_clusters = 0
        self.num_spoints = 0
        self.num_stracks = 0
        self.num_htracks = 0
        self.spoints_x = array.array('d')
        self.spoints_y = array.array('d')
        self.spoints_z = array.array('d')
        self.spoints_global_x = array.array('d')
        self.spoints_global_y = array.array('d')
        self.spoints_global_z = array.array('d')
        self.seeds_x = []
        self.seeds_y = []
        self.seeds_z = []
        self.seeds_global_x = []
        self.seeds_global_y = []
        self.seeds_global_z = []
        self.seeds_phi = []
        self.seeds_s = []
        self.straight_xz_fits = []
        self.straight_yz_fits = []
        self.helix_xy_fits = []
        self.helix_xz_fits = []
        self.helix_yz_fits = []
        self.helix_sz_fits = []

class TrackerStation:
    """ Class to hold tracker station data """

    def __init__(self, tracker_num, station_num):
        self.tracker_num = tracker_num
        self.station_num = station_num
        self.num_events = 0
        self.num_spoints = 0
        self.num_straight_seeds = 0
        self.num_helical_seeds = 0
        self.spoints_local_x = array.array('d')
        self.spoints_local_y = array.array('d')
        self.spoints_local_z = array.array('d')
        self.spoints_global_x = array.array('d')
        self.spoints_global_y = array.array('d')
        self.spoints_global_z = array.array('d')
        self.straight_seeds_local_x = array.array('d')
        self.straight_seeds_local_y = array.array('d')
        self.straight_seeds_local_z = array.array('d')
        self.straight_seeds_global_x = array.array('d')
        self.straight_seeds_global_y = array.array('d')
        self.straight_seeds_global_z = array.array('d')
        self.helical_seeds_local_x = array.array('d')
        self.helical_seeds_local_y = array.array('d')
        self.helical_seeds_local_z = array.array('d')
        self.helical_seeds_global_x = array.array('d')
        self.helical_seeds_global_y = array.array('d')
        self.helical_seeds_global_z = array.array('d')

    def accumulate_data(self, evt):
        """ Add data from a scifi event """
        self.num_events = self.num_events + 1

        # Loop over all spacepoints and pull out data to arrays
        spoints = evt.spacepoints()
        for sp in spoints:
            if (sp.get_tracker() == self.tracker_num and \
                sp.get_station() == self.station_num):
                x = sp.get_position().x()
                y = sp.get_position().y()
                z = sp.get_position().z()
                gx = sp.get_global_position().x()
                gy = sp.get_global_position().y()
                gz = sp.get_global_position().z()
                self.spoints_local_x.append(x)
                self.spoints_local_y.append(y)
                self.spoints_local_z.append(z)
                self.spoints_global_x.append(gx)
                self.spoints_global_y.append(gy)
                self.spoints_global_z.append(gz)
                self.num_spoints = self.num_spoints + 1

        # Loop over straight tracks and pull out data
        for trk in evt.straightprtracks():
            if trk.get_tracker() == self.tracker_num:
                # Pull out the coords of each seed spacepoint
                for seed in trk.get_spacepoints():
                    if seed.get_station() == self.station_num:
                        x = seed.get_position().x()
                        y = seed.get_position().y()
                        z = seed.get_position().z()
                        gx = seed.get_global_position().x()
                        gy = seed.get_global_position().y()
                        gz = seed.get_global_position().z()
                        self.straight_seeds_local_x.append(x)
                        self.straight_seeds_local_y.append(y)
                        self.straight_seeds_local_z.append(z)
                        self.straight_seeds_global_x.append(gx)
                        self.straight_seeds_global_y.append(gy)
                        self.straight_seeds_global_z.append(gz)
                        self.num_straight_seeds += 1

        # Loop over helical tracks and pull out data
        for trk in evt.helicalprtracks():
            if trk.get_tracker() == self.tracker_num:
                # Pull out the coords of each seed spacepoint
                for seed in trk.get_spacepoints():
                    if seed.get_station == self.station_num:
                        x = seed.get_position().x()
                        y = seed.get_position().y()
                        z = seed.get_position().z()
                        gx = seed.get_global_position().x()
                        gy = seed.get_global_position().y()
                        gz = seed.get_global_position().z()
                        self.helical_seeds_local_x.append(x)
                        self.helical_seeds_local_y.append(y)
                        self.helical_seeds_local_z.append(z)
                        self.helical_seeds_global_x.append(gx)
                        self.helical_seeds_global_y.append(gy)
                        self.helical_seeds_global_z.append(gz)
                        self.num_helical_seeds += 1

    def clear_spacepoint_data(self):
        """ Clear the data associated with spacepoint data (including seeds) """
        self.num_straight_seeds = 0
        self.num_helical_seeds = 0
        self.spoints_local_x = array.array('d')
        self.spoints_local_y = array.array('d')
        self.spoints_local_z = array.array('d')
        self.spoints_global_x = array.array('d')
        self.spoints_global_y = array.array('d')
        self.spoints_global_z = array.array('d')
        self.straight_seeds_local_x = array.array('d')
        self.straight_seeds_local_y = array.array('d')
        self.straight_seeds_local_z = array.array('d')
        self.straight_seeds_global_x = array.array('d')
        self.straight_seeds_global_y = array.array('d')
        self.straight_seeds_global_z = array.array('d')
        self.helical_seeds_local_x = array.array('d')
        self.helical_seeds_local_y = array.array('d')
        self.helical_seeds_local_z = array.array('d')
        self.helical_seeds_global_x = array.array('d')
        self.helical_seeds_global_y = array.array('d')
        self.helical_seeds_global_z = array.array('d')
