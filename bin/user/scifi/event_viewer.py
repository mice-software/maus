#!/usr/bin/env python

""" Using PyROOT to visualise scifi events offline """

import sys
import ROOT
import libMausCpp #pylint: disable = W0611
import MausReducedData

# pylint: disable = C0103
# pylint: disable = E1101
# pylint: disable = R0902
# pylint: disable = R0903
# pylint: disable = R0912
# pylint: disable = R0913
# pylint: disable = R0914
# pylint: disable = R0915
# pylint: disable = W0612

class info_box():
    """ Class which wraps a ROOT TPaveText to display info for one tracker """

    def __init__(self):
        """ Constructor, set up the Canvas and TPaveTexts """
        self.can = ROOT.TCanvas("c_info", "Info Box", 1500, 000, 300, 650)
        self.can.cd()
        self.line1 = ROOT.TLine(0.585, 0.0, 0.585, 1.0)
        self.line2 = ROOT.TLine(0.0, 0.915, 1.0, 0.915)
        self.line1.SetLineWidth(3)
        self.line1.Draw()
        self.line2.SetLineWidth(3)
        self.line2.Draw()
        self.p_t1 = ROOT.TPaveText(.68, .0, 0.75, 1.0)
        self.p_t1.SetTextAlign(22)
        self.p_t1.SetFillColor(0)
        self.p_t1.SetBorderSize(0)
        self.p_t2 = ROOT.TPaveText(.88, .0, 0.95, 1.0)
        self.p_t2.SetTextAlign(22)
        self.p_t2.SetFillColor(0)
        self.p_t2.SetBorderSize(0)
        self.p_label = ROOT.TPaveText(.0, .0, 0.55, 1.0)
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
        self.tot_spoints = [0, 0]
        self.tot_stracks = [0, 0]
        self.tot_htracks = [0, 0]

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
    c_sp_gxyz = ROOT.TCanvas("sp_gxyz", "Spacepoint Global x-y", \
                             0, 0, 1400, 350)
    c_sp_gxyz.Divide(3)
    ib1 = info_box()

    start_num = 1
    start_num = raw_input('Enter the first spill number to start from: ')

    # Loop over spills
    for i in range(int(start_num)-1, tree.GetEntries()):
        tree.GetEntry(i)
        spill = data.GetSpill()
        # t1_spill = Tracker()
        # t2_spill = Tracker()

        # Print some basic information about the spill
        print "Found spill number", spill.GetSpillNumber(),
        print "in run number", spill.GetRunNumber(),
        # physics_events -> to particle data. Everything else is DAQ.
        print "DAQ event type", spill.GetDaqEventType()
        if spill.GetDaqEventType() == "physics_event":

            # Loop over all the particle events in a spill
            for i in range(spill.GetReconEvents().size()):
                t1 = MausReducedData.Tracker() # TkUS event data
                t2 = MausReducedData.Tracker() # TkDS event data
                tof0 = MausReducedData.TOF()   # TOF0 event data
                tof1 = MausReducedData.TOF()   # TOF1 event data
                tof2 = MausReducedData.TOF()   # TOF2 event data

                tk_evt = spill.GetReconEvents()[i].GetSciFiEvent()
                t1.accumulate_data(tk_evt, 0)
                t2.accumulate_data(tk_evt, 1)

                tof_evt = spill.GetReconEvents()[i].GetTOFEvent()
                tof0.accumulate_data(tof_evt, 0)
                tof1.accumulate_data(tof_evt, 1)
                tof2.accumulate_data(tof_evt, 2)

                # Update info box
                ib1.update(spill.GetSpillNumber(), t1, t2)

                # Make plots - local coordinates
                sp_graphs = draw_spoints(c_sp_xyz, t1.spoints_x, t1.spoints_y,
                        t1.spoints_z, t2.spoints_x, t2.spoints_y, t2.spoints_z)
                draw_htracks(t1, t2, c_sp_xyz)
                draw_stracks(t1, t2, c_sp_xyz)
                c_sp_xyz.Update()
                mg = draw_sz(t1.seeds_z, t1.seeds_s, t2.seeds_z, t2.seeds_s, \
                              t1.helix_sz_fits, t2.helix_sz_fits, c_trk_sz)
                c_trk_sz.Update()

                # Make plots - global coordinates
                tk_data = [t1.spoints_global_x, t1.spoints_global_y,
                           t1.spoints_global_z, t2.spoints_global_x,
                           t2.spoints_global_y, t2.spoints_global_z]
                tof_data = [tof0.spoints_global_x, tof0.spoints_global_y,
                            tof0.spoints_global_z, tof1.spoints_global_x,
                            tof1.spoints_global_y, tof1.spoints_global_z,
                            tof2.spoints_global_x, tof2.spoints_global_y,
                            tof2.spoints_global_z]
                all_data = tk_data
                all_data.extend(tof_data)

                sp_global_graphs = draw_global_spoints(c_sp_gxyz, all_data)
                c_sp_gxyz.Update()

                # Pause between events
                raw_input("Press any key to move to the next event...")

                mg[0].Clear()
                mg[1].Clear()
                del mg
                del t1
                del t2
                del tof0
                del tof1
                del tof2
        del spill

    raw_input("Press any key to finish...")
    print "Closing root file"
    root_file.Close()

def draw_spoints(can, t1_spoints_x, t1_spoints_y, t1_spoints_z,
                 t2_spoints_x, t2_spoints_y, t2_spoints_z):
    """ Draw the spacepoints in real space (x-y, x-z, y-z) """
    graphs = []

    can.cd(1)
    if (len(t1_spoints_x) > 0) and (len(t1_spoints_y) > 0):
        t1_sp_xy_graph = ROOT.TGraph(len(t1_spoints_x), t1_spoints_x, \
                                     t1_spoints_y)
        t1_sp_xy_graph.SetTitle("Spacepoint x-y plot for tracker 1")
        t1_sp_xy_graph.GetXaxis().SetTitle("x(mm)")
        t1_sp_xy_graph.GetYaxis().SetTitle("y(mm)")
        t1_sp_xy_graph.SetMarkerStyle(20)
        t1_sp_xy_graph.SetMarkerColor(ROOT.kBlack)
        t1_sp_xy_graph.Draw("AP")
        graphs.append(t1_sp_xy_graph)

    can.cd(2)
    if (len(t1_spoints_x) > 0) and (len(t1_spoints_z) > 0):
        t1_sp_xz_graph = ROOT.TGraph(len(t1_spoints_z), t1_spoints_z, \
                                     t1_spoints_x)
        t1_sp_xz_graph.SetTitle("Spacepoint z-x plot for tracker 1")
        t1_sp_xz_graph.GetXaxis().SetTitle("z(mm)")
        t1_sp_xz_graph.GetYaxis().SetTitle("x(mm)")
        t1_sp_xz_graph.SetMarkerStyle(20)
        t1_sp_xz_graph.SetMarkerColor(ROOT.kBlack)
        t1_sp_xz_graph.Draw("AP")
        graphs.append(t1_sp_xz_graph)

    can.cd(3)
    if (len(t1_spoints_y) > 0) and (len(t1_spoints_z) > 0):
        t1_sp_yz_graph = ROOT.TGraph(len(t1_spoints_z), t1_spoints_z, \
                                     t1_spoints_y)
        t1_sp_yz_graph.SetTitle("Spacepoint z-y plot for tracker 1")
        t1_sp_yz_graph.GetXaxis().SetTitle("z(mm)")
        t1_sp_yz_graph.GetYaxis().SetTitle("y(mm)")
        t1_sp_yz_graph.SetMarkerStyle(20)
        t1_sp_yz_graph.SetMarkerColor(ROOT.kBlack)
        t1_sp_yz_graph.Draw("AP")
        graphs.append(t1_sp_yz_graph)

    can.cd(4)
    if (len(t2_spoints_x) > 0) and (len(t2_spoints_y) > 0):
        t2_sp_xy_graph = ROOT.TGraph(len(t2_spoints_x), t2_spoints_x, \
                                     t2_spoints_y)
        t2_sp_xy_graph.SetTitle("Spacepoint x-y plot for tracker 2")
        t2_sp_xy_graph.GetXaxis().SetTitle("x(mm)")
        t2_sp_xy_graph.GetYaxis().SetTitle("y(mm)")
        t2_sp_xy_graph.SetMarkerStyle(20)
        t2_sp_xy_graph.SetMarkerColor(ROOT.kBlack)
        t2_sp_xy_graph.Draw("AP")
        graphs.append(t2_sp_xy_graph)

    can.cd(5)
    if (len(t2_spoints_x) > 0) and (len(t2_spoints_z) > 0):
        t2_sp_xz_graph = ROOT.TGraph(len(t2_spoints_z), t2_spoints_z, \
                                     t2_spoints_x)
        t2_sp_xz_graph.SetTitle("Spacepoint z-x plot for tracker 2")
        t2_sp_xz_graph.GetXaxis().SetTitle("z(mm)")
        t2_sp_xz_graph.GetYaxis().SetTitle("x(mm)")
        t2_sp_xz_graph.SetMarkerStyle(20)
        t2_sp_xz_graph.SetMarkerColor(ROOT.kBlack)
        t2_sp_xz_graph.Draw("AP")
        graphs.append(t2_sp_xz_graph)

    can.cd(6)
    if (len(t2_spoints_y) > 0) and (len(t2_spoints_z) > 0):
        t2_sp_yz_graph = ROOT.TGraph(len(t2_spoints_z), t2_spoints_z, \
                                    t2_spoints_y)
        t2_sp_yz_graph.SetTitle("Spacepoint z-y plot for tracker 2")
        t2_sp_yz_graph.GetXaxis().SetTitle("z(mm)")
        t2_sp_yz_graph.GetYaxis().SetTitle("y(mm)")
        t2_sp_yz_graph.SetMarkerStyle(20)
        t2_sp_yz_graph.SetMarkerColor(ROOT.kBlack)
        t2_sp_yz_graph.Draw("AP")
        graphs.append(t2_sp_yz_graph)

    return graphs


def draw_global_spoints(can, all_data):
    """ Draw spacepoints in real space """

    tkus_spoints_x = all_data[0]
    tkus_spoints_y = all_data[1]
    tkus_spoints_z = all_data[2]
    tkds_spoints_x = all_data[3]
    tkds_spoints_y = all_data[4]
    tkds_spoints_z = all_data[5]

    tof0_spoints_x = all_data[6]
    tof0_spoints_y = all_data[7]
    tof0_spoints_z = all_data[8]
    tof1_spoints_x = all_data[9]
    tof1_spoints_y = all_data[10]
    tof1_spoints_z = all_data[11]
    tof2_spoints_x = all_data[12]
    tof2_spoints_y = all_data[13]
    tof2_spoints_z = all_data[14]

    print 'tof0 spacepoints: ' + str(len(tof0_spoints_z))
    print 'tof1 spacepoints: ' + str(len(tof1_spoints_z))
    print 'tkus spacepoints: ' + str(len(tkus_spoints_z))
    print 'tkds spacepoints: ' + str(len(tkds_spoints_z))
    print 'tof2 spacepoints: ' + str(len(tof2_spoints_z))

    graphs1 = []
    can.cd(1)
    if (len(tkus_spoints_x) > 0) and (len(tkus_spoints_y) > 0):
        tkus_sp_xy_graph = ROOT.TGraph(len(tkus_spoints_x), \
                                      tkus_spoints_x, tkus_spoints_y)
        tkus_sp_xy_graph.SetMarkerStyle(20)
        tkus_sp_xy_graph.SetMarkerColor(ROOT.kBlue)
        graphs1.append(tkus_sp_xy_graph)

    if (len(tkds_spoints_x) > 0) and (len(tkds_spoints_y) > 0):
        tkds_sp_xy_graph = ROOT.TGraph(len(tkds_spoints_x), \
                                      tkds_spoints_x, tkds_spoints_y)
        tkds_sp_xy_graph.SetMarkerColor(ROOT.kCyan)
        tkds_sp_xy_graph.SetMarkerStyle(20)
        graphs1.append(tkds_sp_xy_graph)

    if (len(tof0_spoints_x) > 0) and (len(tof0_spoints_y) > 0):
        tof0_sp_xy_graph = ROOT.TGraph(len(tof0_spoints_x), \
                                      tof0_spoints_x, tof0_spoints_y)
        tof0_sp_xy_graph.SetMarkerColor(ROOT.kMagenta)
        tof0_sp_xy_graph.SetMarkerStyle(20)
        graphs1.append(tof0_sp_xy_graph)

    if (len(tof1_spoints_x) > 0) and (len(tof1_spoints_y) > 0):
        tof1_sp_xy_graph = ROOT.TGraph(len(tof1_spoints_x), \
                                      tof1_spoints_x, tof1_spoints_y)
        tof1_sp_xy_graph.SetMarkerColor(ROOT.kRed)
        tof1_sp_xy_graph.SetMarkerStyle(20)
        graphs1.append(tof1_sp_xy_graph)

    if (len(tof2_spoints_x) > 0) and (len(tof2_spoints_y) > 0):
        tof2_sp_xy_graph = ROOT.TGraph(len(tof2_spoints_x), \
                                      tof2_spoints_x, tof2_spoints_y)
        tof2_sp_xy_graph.SetMarkerColor(ROOT.kOrange)
        tof2_sp_xy_graph.SetMarkerStyle(20)
        graphs1.append(tof2_sp_xy_graph)

    for i in range(len(graphs1)):
        if i == 0:
            graphs1[i].SetTitle("Spacepoint x-y")
            graphs1[i].GetXaxis().SetTitle("x(mm)")
            graphs1[i].GetYaxis().SetTitle("y(mm)")
            graphs1[i].Draw("AP")
            graphs1[i].GetXaxis().SetLimits(-160, 160)
            graphs1[i].GetYaxis().SetRangeUser(-160, 160)
            graphs1[i].Draw("AP")
        else:
            graphs1[i].Draw("P")

    graphs2 = []
    can.cd(2)
    if (len(tkus_spoints_x) > 0) and (len(tkus_spoints_z) > 0):
        tkus_sp_xz_graph = ROOT.TGraph(len(tkus_spoints_z), \
                                      tkus_spoints_z, tkus_spoints_x)
        tkus_sp_xz_graph.SetTitle("Spacepoint z-x")
        tkus_sp_xz_graph.GetXaxis().SetTitle("z(mm)")
        tkus_sp_xz_graph.GetYaxis().SetTitle("x(mm)")
        tkus_sp_xz_graph.SetMarkerStyle(20)
        tkus_sp_xz_graph.SetMarkerColor(ROOT.kBlue)
        graphs2.append(tkus_sp_xz_graph)

    if (len(tkds_spoints_x) > 0) and (len(tkds_spoints_z) > 0):
        tkds_sp_xz_graph = ROOT.TGraph(len(tkds_spoints_z), \
                                      tkds_spoints_z, tkds_spoints_x)
        tkds_sp_xz_graph.SetMarkerColor(ROOT.kCyan)
        tkds_sp_xz_graph.SetMarkerStyle(20)
        graphs2.append(tkds_sp_xz_graph)

    if (len(tof0_spoints_x) > 0) and (len(tof0_spoints_z) > 0):
        tof0_sp_xz_graph = ROOT.TGraph(len(tof0_spoints_z), \
                                      tof0_spoints_z, tof0_spoints_x)
        tof0_sp_xz_graph.SetMarkerColor(ROOT.kMagenta)
        tof0_sp_xz_graph.SetMarkerStyle(20)
        graphs2.append(tof0_sp_xz_graph)

    if (len(tof1_spoints_x) > 0) and (len(tof1_spoints_z) > 0):
        tof1_sp_xz_graph = ROOT.TGraph(len(tof1_spoints_z), \
                                      tof1_spoints_z, tof1_spoints_x)
        tof1_sp_xz_graph.SetMarkerColor(ROOT.kRed)
        tof1_sp_xz_graph.SetMarkerStyle(20)
        graphs2.append(tof1_sp_xz_graph)

    if (len(tof2_spoints_x) > 0) and (len(tof2_spoints_z) > 0):
        tof2_sp_xz_graph = ROOT.TGraph(len(tof2_spoints_z), \
                                      tof2_spoints_z, tof2_spoints_x)
        tof2_sp_xz_graph.SetMarkerColor(ROOT.kOrange)
        tof2_sp_xz_graph.SetMarkerStyle(20)
        graphs2.append(tof2_sp_xz_graph)

    for i in range(len(graphs2)):
        if i == 0:
            graphs2[i].SetTitle("Spacepoint x-y")
            graphs2[i].GetXaxis().SetTitle("x(mm)")
            graphs2[i].GetYaxis().SetTitle("y(mm)")
            graphs2[i].Draw("AP")
            graphs2[i].GetXaxis().SetLimits(2000, 22000)
            graphs2[i].GetYaxis().SetRangeUser(-160, 160)
            graphs2[i].Draw("AP")
        else:
            graphs2[i].Draw("P")

    graphs3 = []
    can.cd(3)
    if (len(tkus_spoints_y) > 0) and (len(tkus_spoints_z) > 0):
        tkus_sp_yz_graph = ROOT.TGraph(len(tkus_spoints_z), \
                                      tkus_spoints_z, tkus_spoints_y)
        tkus_sp_yz_graph.SetMarkerStyle(20)
        tkus_sp_yz_graph.SetMarkerColor(ROOT.kBlue)
        graphs3.append(tkus_sp_yz_graph)

    if (len(tkds_spoints_y) > 0) and (len(tkds_spoints_z) > 0):
        tkds_sp_yz_graph = ROOT.TGraph(len(tkds_spoints_z), \
                                      tkds_spoints_z, tkds_spoints_y)
        tkds_sp_yz_graph.SetMarkerColor(ROOT.kCyan)
        tkds_sp_yz_graph.SetMarkerStyle(20)
        graphs3.append(tkds_sp_yz_graph)

    if (len(tof0_spoints_y) > 0) and (len(tof0_spoints_z) > 0):
        tof0_sp_yz_graph = ROOT.TGraph(len(tof0_spoints_z), \
                                      tof0_spoints_z, tof0_spoints_y)
        tof0_sp_yz_graph.SetMarkerColor(ROOT.kMagenta)
        tof0_sp_yz_graph.SetMarkerStyle(20)
        graphs3.append(tof0_sp_yz_graph)

    if (len(tof1_spoints_y) > 0) and (len(tof1_spoints_z) > 0):
        tof1_sp_yz_graph = ROOT.TGraph(len(tof1_spoints_z), \
                                      tof1_spoints_z, tof1_spoints_y)
        tof1_sp_yz_graph.SetMarkerColor(ROOT.kRed)
        tof1_sp_yz_graph.SetMarkerStyle(20)
        graphs3.append(tof1_sp_yz_graph)

    if (len(tof2_spoints_y) > 0) and (len(tof2_spoints_z) > 0):
        tof2_sp_yz_graph = ROOT.TGraph(len(tof2_spoints_z), \
                                      tof2_spoints_z, tof2_spoints_y)
        tof2_sp_yz_graph.SetMarkerColor(ROOT.kOrange)
        tof2_sp_yz_graph.SetMarkerStyle(20)
        graphs3.append(tof2_sp_yz_graph)

    for i in range(len(graphs3)):
        if i == 0:
            graphs3[i].SetTitle("Spacepoint x-y")
            graphs3[i].GetXaxis().SetTitle("x(mm)")
            graphs3[i].GetYaxis().SetTitle("y(mm)")
            graphs3[i].Draw("AP")
            graphs3[i].GetXaxis().SetLimits(2000, 22000)
            graphs3[i].GetYaxis().SetRangeUser(-160, 160)
            graphs3[i].Draw("AP")
        else:
            graphs3[i].Draw("P")

    can.Update()

    return graphs1 + graphs2 + graphs3

def draw_htracks(t1, t2, can):
    """ Draw the helix fits over the spacepoints """
    for circ in t1.helix_xy_fits:
        can.cd(1)
        circ.Draw("same")
        can.Update()
    for func in t1.helix_xz_fits:
        can.cd(2)
        func.Draw("same")
        can.Update()
    for func in t1.helix_yz_fits:
        can.cd(3)
        func.Draw("same")
        can.Update()
    for circ in t2.helix_xy_fits:
        can.cd(4)
        circ.Draw("same")
        can.Update()
    for func in t2.helix_xz_fits:
        can.cd(5)
        func.Draw("same")
        can.Update()
    for func in t2.helix_yz_fits:
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

def draw_stracks(t1, t2, can):
    """ Draw the straight fits over the spacepoints """
    for line in t1.straight_xz_fits:
        can.cd(2)
        line.SetLineColor(ROOT.kRed)
        line.Draw("same")
        can.Update()
    for line in t1.straight_yz_fits:
        can.cd(3)
        line.SetLineColor(ROOT.kRed)
        line.Draw("same")
        can.Update()
    for line in t2.straight_xz_fits:
        can.cd(5)
        line.SetLineColor(ROOT.kRed)
        line.Draw("same")
        can.Update()
    for line in t2.straight_yz_fits:
        can.cd(6)
        line.SetLineColor(ROOT.kRed)
        line.Draw("same")
        can.Update()

if __name__ == "__main__":
    main(sys.argv[1])

