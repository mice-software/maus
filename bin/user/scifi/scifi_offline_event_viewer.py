#!/usr/bin/env python

"""
Using PyROOT to visualise scifi events offline
"""

import sys
import array

# basic PyROOT definitions
import ROOT 

# definitions of MAUS data structure for PyROOT
import libMausCpp #pylint: disable = W0611

# pylint: disable = E1101
# pylint: disable = R0902
# pylint: disable = R0914
# pylint: disable = R0915
# pylint: disable = C0103
# pylint: disable = R0912
# pylint: disable = R0913
# pylint: disable = W0612

class Tracker:
    """ Class holding data for one tracker.
        Uses arrays to make compatible with ROOT.
        May accumulate data over many events. """
    trker_num = 0
    num_spoints = 0
    spoints_x = array.array('d', [0])
    spoints_y = array.array('d', [0])
    spoints_z = array.array('d', [0])
    seeds_z = []
    seeds_phi = []
    seeds_s = []
    seeds_circles = []

    def __init__(self):
        """ Constructor, set everything to empty or zero """
        self.num_spoints = 0
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
        spoints = evt.spacepoints()

        # Loop over all spacepoints and pull out data to arrays
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
        self.num_spoints = 0
        self.num_htracks = 0
        self.spoints_x = array.array('d', [0])
        self.spoints_y = array.array('d', [0])
        self.spoints_z = array.array('d', [0])
        self.seeds_z = []
        self.seeds_phi = []
        self.seeds_s = []
        self.seeds_circle = []


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

            print "Making plots..."
            sp_graphs = draw_spoints(c_sp_xy, t1.spoints_x, t1.spoints_y,
                    t1.spoints_z, t2.spoints_x, t2.spoints_y, t2.spoints_z)
            draw_htracks(t1.seeds_circle, t2.seeds_circle, c_sp_xy)
            c_sp_xy.Update()
            draw_sz(t1.seeds_z, t1.seeds_s, t2.seeds_z, t2.seeds_s, c_trk_sz)
            raw_input("Press any key to move to the next spill...")

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
    for i, z_per_track in enumerate(t1_z):
        t1_sz_graph = ROOT.TGraph(len(z_per_track), z_per_track, t1_s[i])
        t1_sz_graph.SetTitle("Seed s-z plot for tracker 1")
        t1_sz_graph.GetXaxis().SetTitle("z(mm)")
        t1_sz_graph.GetYaxis().SetTitle("s(mm)")
        t1_sz_graph.SetMarkerStyle(20)
        t1_sz_graph.SetMarkerColor(ROOT.kBlack)
        t1_sz_graph.Draw("AP")

    can.cd(2)
    for i, z_per_track in enumerate(t2_z):
        t2_sz_graph = ROOT.TGraph(len(z_per_track), z_per_track, t2_s[i])
        t2_sz_graph.SetTitle("Seed s-z plot for tracker 2")
        t2_sz_graph.GetXaxis().SetTitle("z(mm)")
        t2_sz_graph.GetYaxis().SetTitle("s(mm)")
        t2_sz_graph.SetMarkerStyle(20)
        t2_sz_graph.SetMarkerColor(ROOT.kBlack)
        t2_sz_graph.Draw("AP")

    can.Update()
    return [t1_sz_graph, t2_sz_graph]

if __name__ == "__main__":
    main(sys.argv[1])

