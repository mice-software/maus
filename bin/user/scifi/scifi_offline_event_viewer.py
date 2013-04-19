#!/usr/bin/env python

""" Using PyROOT to visualise scifi events offline """

import sys
import array
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
        self.seeds_z = []
        self.seeds_phi = []
        self.seeds_s = []
        self.seeds_circle = []

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
                print 'Tracker ' + str(self.trker_num),
                print ', Track ' + str(self.num_htracks) + ':'
                # Pull out the circle fit data
                x0 = trk.get_circle_x0()
                y0 = trk.get_circle_y0()
                rad = trk.get_R()
                self.seeds_circle.append(make_circle(x0, y0, rad))
                self.num_htracks = self.num_htracks + 1

                # Pull out the turning angles of each seed spacepoint
                phi_per_trk = array.array('d', [0])
                s_per_trk = array.array('d', [0])
                num_phi = 0
                for phi in trk.get_phi():
                    if num_phi == 0:
                        phi_per_trk[0] = phi
                        s_per_trk[0] = phi * rad
                        num_phi = num_phi + 1
                        print 'phi' + str(num_phi+1) + ' is ' + str(phi)
                    else:
                        phi_per_trk.append(phi)
                        s_per_trk.append(phi*rad)
                        num_phi = num_phi + 1
                        print 'phi' + str(num_phi+1) + ' is ' + str(phi)
                    self.seeds_phi.append(phi_per_trk)
                    self.seeds_s.append(s_per_trk)

                # Pull out the z of each seed spacepoint
                z_per_trk = array.array('d', [0])
                num_seeds = 0
                for seed in trk.get_spacepoints():
                    if num_seeds == 0:
                        z_per_trk[0] = seed.get_position().z()
                        num_seeds = num_seeds + 1
                    else:
                        z_per_trk.append(seed.get_position().z())
                        num_seeds = num_seeds + 1
                    self.seeds_z.append(z_per_trk)

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
        self.seeds_circle = []

class info_box():
    """ Class which wraps a ROOT TPaveText to display info for one tracker """

    tot_spoints = [0, 0]    # Num of spacepoints in the current run so far
    tot_stracks = [0, 0]     # Num of straight tracks in the current run so far
    tot_htracks = [0, 0]     # Num of helical tracks in the current run so far
    p_label = ROOT.TPaveText(.0, .0, 0.55, 1.0)
    p_t1 = ROOT.TPaveText(.68, .0, 0.75, 1.0)
    p_t2 = ROOT.TPaveText(.88, .0, 0.95, 1.0)
    can = ROOT.TCanvas("c_info", "Info Box", 350, 50, 380, 800)
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
    c_sp_xy = ROOT.TCanvas("sp_xy", "Spacepoint x-y", 200, 10, 700, 500)
    c_sp_xy.Divide(3, 2)
    c_trk_sz = ROOT.TCanvas("trk_sz", "Track s-z", 350, 10, 700, 500)
    c_trk_sz.Divide(1, 2)
    ib1 = info_box()

    # Loop over spills
    for i in range(tree.GetEntries()):
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
            sp_graphs = draw_spoints(c_sp_xy, t1.spoints_x, t1.spoints_y,
                    t1.spoints_z, t2.spoints_x, t2.spoints_y, t2.spoints_z)
            draw_htracks(t1.seeds_circle, t2.seeds_circle, c_sp_xy)
            c_sp_xy.Update()
            mg = draw_sz(t1.seeds_z, t1.seeds_s,
                         t2.seeds_z, t2.seeds_s, c_trk_sz)
            c_trk_sz.Update()
            raw_input("Press any key to move to the next spill...")
            mg[0].Clear()
            mg[1].Clear()
            del mg


    print "Closing root file"
    root_file.Close()

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

def draw_htracks(t1_circles_xy, t2_circles_xy, can):
    """ Draw the circle fits over the spacepoints """

    for circ in t1_circles_xy:
        can.cd(1)
        circ.Draw("same")
        can.Update()
    for circ in t2_circles_xy:
        can.cd(4)
        circ.Draw("same")
        can.Update()

def draw_sz(t1_z, t1_s, t2_z, t2_s, can):
    """ Draw s - z for each track """
    can.cd(1)
    mg1 = ROOT.TMultiGraph()
    for i, z_per_track in enumerate(t1_z):
        t1_sz_graph = ROOT.TGraph(len(z_per_track), z_per_track, t1_s[i])
        t1_sz_graph.SetMarkerStyle(20)
        t1_sz_graph.SetMarkerColor(ROOT.kBlack)
        mg1.Add(t1_sz_graph)
    mg1.Draw("ap")
    mg1.SetTitle("Seed s-z plot for tracker 1")
    mg1.GetXaxis().SetTitle("z(mm)")
    mg1.GetYaxis().SetTitle("s(mm)")

    can.cd(2)
    mg2 = ROOT.TMultiGraph()
    for i, z_per_track in enumerate(t2_z):
        t2_sz_graph = ROOT.TGraph(len(z_per_track), z_per_track, t2_s[i])
        t2_sz_graph.SetMarkerStyle(20)
        t2_sz_graph.SetMarkerColor(ROOT.kBlack)
        mg2.Add(t2_sz_graph)
    mg2.Draw("ap")
    mg2.SetTitle("Seed s-z plot for tracker 2")
    mg2.GetXaxis().SetTitle("z(mm)")
    mg2.GetYaxis().SetTitle("s(mm)")

    can.Update()
    return [mg1, mg2]

if __name__ == "__main__":
    main(sys.argv[1])

