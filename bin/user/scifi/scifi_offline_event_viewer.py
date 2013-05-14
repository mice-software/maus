#!/usr/bin/env python

""" Using PyROOT to visualise scifi events offline """

import sys
import array
import math
import ROOT 
import libMausCpp #pylint: disable = W0611

# pylint: disable = E1101
# pylint: disable = R0902
# pylint: disable = R0903
# pylint: disable = R0914
# pylint: disable = R0915
# pylint: disable = C0103
# pylint: disable = R0912
# pylint: disable = R0913
# pylint: disable = W0612

class Tracker:
    """ Class holding data for one tracker.
        Uses arrays to make compatible with ROOT plotting.
        May accumulate data over many events. """
    trker_num = 0
    num_events = 0
    num_digits = 0
    num_clusters = 0
    num_spoints = 0
    num_stracks = 0
    num_htracks = 0
    spoints_x = array.array('d', [0])
    spoints_y = array.array('d', [0])
    spoints_z = array.array('d', [0])
    seeds_z = []
    seeds_phi = []
    seeds_s = []
    seeds_circles = []
    seeds_xz = []
    seeds_yz = []
    sz_fits = []

    def __init__(self):
        """ Constructor, set everything to empty or zero """
        self.num_events = 0
        self.num_digits = 0
        self.num_clusters = 0
        self.num_spoints = 0
        self.num_stracks = 0
        self.num_htracks = 0
        self.spoints_x = array.array('d', [0])
        self.spoints_y = array.array('d', [0])
        self.spoints_z = array.array('d', [0])
        self.seeds_x = []
        self.seeds_y = []
        self.seeds_z = []
        self.seeds_phi = []
        self.seeds_s = []
        self.seeds_circles = []
        self.seeds_xz = []
        self.seeds_yz = []
        self.sz_fits = []

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
            if sp.get_tracker() == trker_num:
                if self.num_spoints == 0:
                    self.spoints_x[0] = x
                    self.spoints_y[0] = y
                    self.spoints_z[0] = z
                    self.num_spoints = self.num_spoints + 1
                else:
                    self.spoints_x.append(x)
                    self.spoints_y.append(y)
                    self.spoints_z.append(z)
                    self.num_spoints = self.num_spoints + 1

        # Loop over straight tracks
        for trk in evt.straightprtracks():
            if trk.get_tracker() == trker_num:
                self.num_stracks = self.num_stracks + 1

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
                print 'x\ty\tz\ttime\t\tphi\tpx_mc\tpy_mc\tpt_mc\tpz_mc'
                # Pull out the circle fit data
                x0 = trk.get_circle_x0()
                y0 = trk.get_circle_y0()
                dsdz = trk.get_dsdz()
                rad = trk.get_R()
                self.seeds_circles.append(make_circle(x0, y0, rad))
                # Pull out the turning angles of each seed spacepoint
                phi_per_trk = array.array('d', [0])
                s_per_trk = array.array('d', [0])
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
                    clus = trk.get_spacepoints()[i].get_channels()[0]
                    mom = clus.get_true_momentum()
                    t = trk.get_spacepoints()[i].get_time()
                    print '%.2f' % pos.x() + '\t',
                    print '%.2f' % pos.y() + '\t',
                    print '%.2f' % pos.z() + '\t',
                    print '%.2f' % t + '\t',
                    print '%.2f' % phi + '\t',
                    print '%.2f' % mom.x() + '\t',
                    print '%.2f' % mom.y() + '\t',
                    pt_mc = math.sqrt(mom.x()*mom.x()+mom.y()*mom.y())
                    print '%.2f' % pt_mc + '\t',
                    print '%.2f' % mom.z()
                self.seeds_phi.append(phi_per_trk)
                self.seeds_s.append(s_per_trk)

                # Pull out the coords of each seed spacepoint
                x_per_trk = array.array('d', [0])
                y_per_trk = array.array('d', [0])
                z_per_trk = array.array('d', [0])
                num_seeds = 0
                for seed in trk.get_spacepoints():
                    if num_seeds == 0:
                        x_per_trk[0] = seed.get_position().x()
                        y_per_trk[0] = seed.get_position().y()
                        z_per_trk[0] = seed.get_position().z()
                        num_seeds = num_seeds + 1
                    else:
                        x_per_trk.append(seed.get_position().x())
                        y_per_trk.append(seed.get_position().y())
                        z_per_trk.append(seed.get_position().z())
                        num_seeds = num_seeds + 1
                self.seeds_x.append(x_per_trk)
                self.seeds_y.append(y_per_trk)
                self.seeds_z.append(z_per_trk)

                if trker_num == 0:
                    dsdz = - trk.get_dsdz()
                elif trker_num == 1:
                    dsdz = trk.get_dsdz()
                # As the line_sz was drawn in ds - dz space (origin set by
                # the 1st spoint) dsdz is the same but the axis intercept
                # needs to be altered to plot in s - z space correctly.
                sz_c = trk.get_line_sz_c() + self.seeds_s[-1][0] \
                       - (dsdz * self.seeds_z[-1][0])
                # sz_c = trk.get_line_sz_c()
                self.seeds_xz.append(make_xz(x0, rad, dsdz, sz_c, 0, 1200))
                self.seeds_yz.append(make_yz(y0, rad, dsdz, sz_c, 0, 1200))
                self.sz_fits.append(make_line(dsdz, sz_c, 0, 1200))
                self.num_htracks = self.num_htracks + 1

    def clear(self):
        """ Clear all accumulated data """
        self.num_events = 0
        self.num_digits = 0
        self.num_clusters = 0
        self.num_spoints = 0
        self.num_stracks = 0
        self.num_htracks = 0
        self.spoints_x = array.array('d', [0])
        self.spoints_y = array.array('d', [0])
        self.spoints_z = array.array('d', [0])
        self.seeds_z = []
        self.seeds_phi = []
        self.seeds_s = []
        self.seeds_circles = []
        self.seeds_xz = []
        self.seeds_yz = []
        self.sz_fits = []

class info_box():
    """ Class which wraps a ROOT TPaveText to display info for one tracker """

    tot_spoints = [0, 0]    # Num of spacepoints in the current run so far
    tot_stracks = [0, 0]     # Num of straight tracks in the current run so far
    tot_htracks = [0, 0]     # Num of helical tracks in the current run so far
    p_label = ROOT.TPaveText(.0, .0, 0.55, 1.0)
    p_t1 = ROOT.TPaveText(.68, .0, 0.75, 1.0)
    p_t2 = ROOT.TPaveText(.88, .0, 0.95, 1.0)
    can = ROOT.TCanvas("c_info", "Info Box", 1500, 000, 300, 650)
    line1 = ROOT.TLine(0.585, 0.0, 0.585, 1.0)
    line2 = ROOT.TLine(0.0, 0.915, 1.0, 0.915)

    def __init__(self):
        """ Constructor, set up the Canvas and TPaveTexts """
        self.can.cd()
        self.line1.SetLineWidth(3)
        self.line1.Draw()
        self.line2.SetLineWidth(3)
        self.line2.Draw()
        self.p_t1.SetTextAlign(22)
        self.p_t1.SetFillColor(0)
        self.p_t1.SetBorderSize(0)
        self.p_t2.SetTextAlign(22)
        self.p_t2.SetFillColor(0)
        self.p_t2.SetBorderSize(0)
        self.p_label.SetTextAlign(12)
        self.p_label.SetFillColor(0)
        self.p_label.SetBorderSize(0)
        self.p_label.Clear()
        self.p_label.AddText("Tracker")
        self.p_label.AddText("Spill num")
        self.p_label.AddText("Events")
        self.p_label.AddText("Digits")
        self.p_label.AddText("Clusters")
        self.p_label.AddText("Spacepoints")
        self.p_label.AddText("Str Tracks")
        self.p_label.AddText("Helical Tracks")
        self.p_label.AddText("Total Spoints")
        self.p_label.AddText("Total Str Tracks")
        self.p_label.AddText("Total Helical Tracks")
        self.p_label.Draw()

    def update(self, spill_num, t1, t2):
        """ Update the info box data """
        self.p_t1.Clear()
        self.p_t2.Clear()

        self.p_t1.AddText("1")
        self.p_t1.AddText( str(spill_num) )
        self.p_t1.AddText( str(t1.num_events) )
        self.p_t1.AddText( str(t1.num_digits) )
        self.p_t1.AddText( str(t1.num_clusters) )
        self.p_t1.AddText( str(t1.num_spoints) )
        self.p_t1.AddText( str(t1.num_stracks) )
        self.p_t1.AddText( str(t1.num_htracks) )
        self.tot_spoints[0] = self.tot_spoints[0] + t1.num_spoints
        self.tot_stracks[0] = self.tot_stracks[0] + t1.num_stracks
        self.tot_htracks[0] = self.tot_htracks[0] + t1.num_htracks
        self.p_t1.AddText( str(self.tot_spoints[0]) )
        self.p_t1.AddText( str(self.tot_stracks[0]) )
        self.p_t1.AddText( str(self.tot_htracks[0]) )

        self.p_t2.AddText("2")
        self.p_t2.AddText( str(spill_num) )
        self.p_t2.AddText( str(t2.num_events) )
        self.p_t2.AddText( str(t2.num_digits) )
        self.p_t2.AddText( str(t2.num_clusters) )
        self.p_t2.AddText( str(t2.num_spoints) )
        self.p_t2.AddText( str(t2.num_stracks) )
        self.p_t2.AddText( str(t2.num_htracks) )
        self.tot_spoints[1] = self.tot_spoints[1] + t2.num_spoints
        self.tot_stracks[1] = self.tot_stracks[1] + t2.num_stracks
        self.tot_htracks[1] = self.tot_htracks[1] + t2.num_htracks
        self.p_t2.AddText( str(self.tot_spoints[1]) )
        self.p_t2.AddText( str(self.tot_stracks[1]) )
        self.p_t2.AddText( str(self.tot_htracks[1]) )

        self.can.cd()
        self.p_t1.Draw()
        self.p_t2.Draw()
        self.line1.Draw()
        self.line2.Draw()
        self.can.Update()

def main(file_name):
    """ Main function which performs analysis & produces the plots """

    # Load the ROOT file
    print "Loading ROOT file", file_name
    root_file = ROOT.TFile(file_name, "READ") # pylint: disable = E1101

    # and set up the data tree to be filled by ROOT IO
    print "Setting up data tree"
    data = ROOT.MAUS.Data() # pylint: disable = E1101
    tree = root_file.Get("Spill")
    tree.SetBranchAddress("data", data)

    # Set up the ROOT canvases
    c_sp_xyz = ROOT.TCanvas("sp_xyz", "Spacepoint x-y", 0, 0, 700, 500)
    c_sp_xyz.Divide(3, 2)
    c_trk_sz = ROOT.TCanvas("trk_sz", "Track s-z", 770, 0, 600, 500)
    c_trk_sz.Divide(1, 2)
    ib1 = info_box()

    start_num = 1
    start_num = raw_input('Enter the first spill number to start from: ')

    # Loop over spills
    for i in range(int(start_num)-1, tree.GetEntries()):
        tree.GetEntry(i)
        spill = data.GetSpill()
        t1 = Tracker()
        t2 = Tracker()

        # Print some basic information about the spill
        print "Found spill number", spill.GetSpillNumber(),
        print "in run number", spill.GetRunNumber(),
        # physics_events -> to particle data. Everything else is DAQ.
        print "DAQ event type", spill.GetDaqEventType()
        if spill.GetDaqEventType() == "physics_event":

            # Loop over all the particle events in a spill
            for i in range(spill.GetReconEvents().size()):
                evt = spill.GetReconEvents()[i].GetSciFiEvent()
                t1.accumulate_data(evt, 0)
                t2.accumulate_data(evt, 1)

            # Update info box
            ib1.update(spill.GetSpillNumber(), t1, t2)

            # Make plots
            sp_graphs = draw_spoints(c_sp_xyz, t1.spoints_x, t1.spoints_y,
                    t1.spoints_z, t2.spoints_x, t2.spoints_y, t2.spoints_z)
            draw_htracks(t1, t2, c_sp_xyz)
            c_sp_xyz.Update()
            mg = draw_sz(t1.seeds_z, t1.seeds_s, t2.seeds_z, t2.seeds_s, \
                         t1.sz_fits, t2.sz_fits, c_trk_sz)
            c_trk_sz.Update()
            # if ( t1.num_htracks < 1):
              # raw_input("Found one!")
            raw_input("Press any key to move to the next spill...")
            print 'Helical tracks found: ' + \
                  str(t1.num_htracks + t2.num_htracks)
            mg[0].Clear()
            mg[1].Clear()
            del mg
        del t1
        del t2
        del spill


    print "Closing root file"
    root_file.Close()

def make_line(m, c, xmin, xmax):
    """ Create a straight line using the ROOT TF1 class """
    line = ROOT.TF1("line", "[0]*x+[1]", xmin, xmax)
    line.SetParameter(0, m)
    line.SetParameter(1, c)
    line.SetLineColor(ROOT.kBlue)
    return line

def make_xz(circle_x0, rad, dsdz, sz_c, zmin, zmax):
    """ Make a function for the x-z projection of the helix """
    # The x in the cos term is actually representing z (the indep variable)
    func = ROOT.TF1("xz_func", "[0]+([1]*cos((1/[1])*([2]*x+[3])))", zmin, zmax)
    func.SetParameter(0, circle_x0)
    func.SetParameter(1, rad)
    func.SetParameter(2, dsdz)
    func.SetParameter(3, sz_c)
    func.SetLineColor(ROOT.kBlue)
    return func

def make_yz(circle_y0, rad, dsdz, sz_c, zmin, zmax):
    """ Make a function for the y-z projection of the helix """
    # The x in the cos term is actually representing z (the indep variable)
    func = ROOT.TF1("yz_func", "[0]+([1]*sin((1/[1])*([2]*x+[3])))", zmin, zmax)
    func.SetParameter(0, circle_y0)
    func.SetParameter(1, rad)
    func.SetParameter(2, dsdz)
    func.SetParameter(3, sz_c)
    func.SetLineColor(ROOT.kBlue)
    return func

def make_circle(x0, y0, rad):
    """ Make a circle from its centre coords & radius using the TArc class"""

    arc = ROOT.TArc(x0, y0, rad)
    arc.SetFillStyle(0)   # 0 - Transparent
    arc.SetLineColor(ROOT.kBlue)
    return arc

def draw_spoints(can, t1_spoints_x, t1_spoints_y, t1_spoints_z,
                 t2_spoints_x, t2_spoints_y, t2_spoints_z):
    """ Draw the spacepoints in real space (x-y, x-z, y-z) """

    can.cd(1)
    t1_sp_xy_graph = ROOT.TGraph(len(t1_spoints_x), t1_spoints_x, t1_spoints_y)
    t1_sp_xy_graph.SetTitle("Spacepoint x-y plot for tracker 1")
    t1_sp_xy_graph.GetXaxis().SetTitle("x(mm)")
    t1_sp_xy_graph.GetYaxis().SetTitle("y(mm)")
    t1_sp_xy_graph.SetMarkerStyle(20)
    t1_sp_xy_graph.SetMarkerColor(ROOT.kBlack)
    t1_sp_xy_graph.Draw("AP")

    can.cd(2)
    t1_sp_xz_graph = ROOT.TGraph(len(t1_spoints_z), t1_spoints_z, t1_spoints_x)
    t1_sp_xz_graph.SetTitle("Spacepoint z-x plot for tracker 1")
    t1_sp_xz_graph.GetXaxis().SetTitle("z(mm)")
    t1_sp_xz_graph.GetYaxis().SetTitle("x(mm)")
    t1_sp_xz_graph.SetMarkerStyle(20)
    t1_sp_xz_graph.SetMarkerColor(ROOT.kBlack)
    t1_sp_xz_graph.Draw("AP")

    can.cd(3)
    t1_sp_yz_graph = ROOT.TGraph(len(t1_spoints_z), t1_spoints_z, t1_spoints_y)
    t1_sp_yz_graph.SetTitle("Spacepoint z-y plot for tracker 1")
    t1_sp_yz_graph.GetXaxis().SetTitle("z(mm)")
    t1_sp_yz_graph.GetYaxis().SetTitle("y(mm)")
    t1_sp_yz_graph.SetMarkerStyle(20)
    t1_sp_yz_graph.SetMarkerColor(ROOT.kBlack)
    t1_sp_yz_graph.Draw("AP")

    can.cd(4)
    t2_sp_xy_graph = ROOT.TGraph(len(t2_spoints_x), t2_spoints_x, t2_spoints_y)
    t2_sp_xy_graph.SetTitle("Spacepoint x-y plot for tracker 2")
    t2_sp_xy_graph.GetXaxis().SetTitle("x(mm)")
    t2_sp_xy_graph.GetYaxis().SetTitle("y(mm)")
    t2_sp_xy_graph.SetMarkerStyle(20)
    t2_sp_xy_graph.SetMarkerColor(ROOT.kBlack)
    t2_sp_xy_graph.Draw("AP")

    can.cd(5)
    t2_sp_xz_graph = ROOT.TGraph(len(t2_spoints_z), t2_spoints_z, t2_spoints_x)
    t2_sp_xz_graph.SetTitle("Spacepoint z-x plot for tracker 2")
    t2_sp_xz_graph.GetXaxis().SetTitle("z(mm)")
    t2_sp_xz_graph.GetYaxis().SetTitle("x(mm)")
    t2_sp_xz_graph.SetMarkerStyle(20)
    t2_sp_xz_graph.SetMarkerColor(ROOT.kBlack)
    t2_sp_xz_graph.Draw("AP")

    can.cd(6)
    t2_sp_yz_graph = ROOT.TGraph(len(t2_spoints_z), t2_spoints_z, t2_spoints_y)
    t2_sp_yz_graph.SetTitle("Spacepoint z-y plot for tracker 2")
    t2_sp_yz_graph.GetXaxis().SetTitle("z(mm)")
    t2_sp_yz_graph.GetYaxis().SetTitle("y(mm)")
    t2_sp_yz_graph.SetMarkerStyle(20)
    t2_sp_yz_graph.SetMarkerColor(ROOT.kBlack)
    t2_sp_yz_graph.Draw("AP")

    return [t1_sp_xy_graph, t1_sp_xz_graph, t1_sp_yz_graph,
            t2_sp_xz_graph, t2_sp_yz_graph,t2_sp_xy_graph]

def draw_htracks(t1, t2, can):
    """ Draw the helix fits over the spacepoints """
    for circ in t1.seeds_circles:
        can.cd(1)
        circ.Draw("same")
        can.Update()
    for func in t1.seeds_xz:
        can.cd(2)
        func.Draw("same")
        can.Update()
    for func in t1.seeds_yz:
        can.cd(3)
        func.Draw("same")
        can.Update()
    for circ in t2.seeds_circles:
        can.cd(4)
        circ.Draw("same")
        can.Update()
    for func in t2.seeds_xz:
        can.cd(5)
        func.Draw("same")
        can.Update()
    for func in t2.seeds_yz:
        can.cd(6)
        func.Draw("same")
        can.Update()

def draw_sz(t1_z, t1_s, t2_z, t2_s, t1_fits, t2_fits, can):
    """ Draw s - z for each track """
    can.cd(1)
    mg1 = ROOT.TMultiGraph()
    if ( len(t1_z) > 0 ):
        for i, z_per_track in enumerate(t1_z):
            t1_sz_graph = ROOT.TGraph(len(z_per_track), z_per_track, t1_s[i])
            t1_sz_graph.SetMarkerStyle(20)
            t1_sz_graph.SetMarkerColor(ROOT.kBlack)
            mg1.Add(t1_sz_graph)
        mg1.Draw("ap")
        mg1.SetTitle("Seed s-z plot for tracker 1")
        mg1.GetXaxis().SetTitle("z(mm)")
        mg1.GetYaxis().SetTitle("s(mm)")
        for line in t1_fits:
            line.Draw("same")

    can.cd(2)
    mg2 = ROOT.TMultiGraph()
    if ( len(t2_z) > 0 ):
        for i, z_per_track in enumerate(t2_z):
            t2_sz_graph = ROOT.TGraph(len(z_per_track), z_per_track, t2_s[i])
            t2_sz_graph.SetMarkerStyle(20)
            t2_sz_graph.SetMarkerColor(ROOT.kBlack)
            mg2.Add(t2_sz_graph)
        mg2.Draw("ap")
        mg2.SetTitle("Seed s-z plot for tracker 2")
        mg2.GetXaxis().SetTitle("z(mm)")
        mg2.GetYaxis().SetTitle("s(mm)")
        for line in t2_fits:
            line.Draw("same")

    can.Update()
    return [mg1, mg2]

if __name__ == "__main__":
    main(sys.argv[1])

