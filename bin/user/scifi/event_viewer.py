#!/usr/bin/env python

""" Using PyROOT to visualise scifi events offline """

import sys
import array
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
    #-------------------------

    # Spacepoints and PatRec fits in tracker local coordinates display
    c_sp_xyz = ROOT.TCanvas("sp_xyz", "Spacepoint x-y", 0, 0, 700, 500)
    c_sp_xyz.Divide(3, 2)

    # The PatRec s-z fit
    # c_trk_sz = ROOT.TCanvas("trk_sz", "Track s-z", 770, 0, 600, 500)
    # c_trk_sz.Divide(1, 2)

     # Spacepoints and Trackpoints in global coordinates
    c_sp_gxyz = ROOT.TCanvas("sp_gxyz", "Space and Track Point Global x-y", \
                             0, 0, 1400, 350)
    c_sp_gxyz.Divide(3)

    # Trackpoint momentum
    c_tp_mom = ROOT.TCanvas("tp_mom", "Trackpoint Momentum", 0, 0, 700, 500)

    # The InfoBox
    ib1 = info_box()

    # Select what spill number to start displaying from
    start_num = 1
    # start_num = raw_input('Enter the first spill number to start from: ')

    # Loop over spills
    for i in range(int(start_num)-1, tree.GetEntries()):
        tree.GetEntry(i)
        spill = data.GetSpill()
        # t1_spill = Tracker()
        # t2_spill = Tracker()

        # Print some basic information about the spill
        print "Found spill number", spill.GetSpillNumber(),
        print "in run number", spill.GetRunNumber(),
        # physics_events -> to particle data, everything else is DAQ
        print "DAQ event type", spill.GetDaqEventType()
        if spill.GetDaqEventType() == "physics_event":

            # Loop over all the particle events in a spill
            for i in range(spill.GetReconEvents().size()):
                tkus = MausReducedData.Tracker() # TkUS event data
                tkds = MausReducedData.Tracker() # TkDS event data
                tof0 = MausReducedData.TOF()   # TOF0 event data
                tof1 = MausReducedData.TOF()   # TOF1 event data
                tof2 = MausReducedData.TOF()   # TOF2 event data

                tk_evt = spill.GetReconEvents()[i].GetSciFiEvent()
                tkus.accumulate_data(tk_evt, 0)
                tkds.accumulate_data(tk_evt, 1)

                tof_evt = spill.GetReconEvents()[i].GetTOFEvent()
                tof0.accumulate_data(tof_evt, 0)
                tof1.accumulate_data(tof_evt, 1)
                tof2.accumulate_data(tof_evt, 2)

                # Update info box
                ib1.update(spill.GetSpillNumber(), tkus, tkds)

                # Make plots - local coordinates
                sp_graphs = draw_spoints(c_sp_xyz, tkus.spoints_x,
                                         tkus.spoints_y, tkus.spoints_z,
                                         tkds.spoints_x, tkds.spoints_y,
                                         tkds.spoints_z)

                #  Seed data are contained in arrays in a list indexed by track
                # - we want all track data in one container,
                # so combine list entries into single arrays
                tkus_seeds_x = array.array('d')
                tkus_seeds_y = array.array('d')
                tkus_seeds_z = array.array('d')
                tkds_seeds_x = array.array('d')
                tkds_seeds_y = array.array('d')
                tkds_seeds_z = array.array('d')
                for i in range(len(tkus.seeds_x)):
                    tkus_seeds_x.extend(tkus.seeds_x[i])
                    tkus_seeds_y.extend(tkus.seeds_y[i])
                    tkus_seeds_z.extend(tkus.seeds_z[i])
                for i in range(len(tkds.seeds_x)):
                    tkds_seeds_x.extend(tkds.seeds_x[i])
                    tkds_seeds_y.extend(tkds.seeds_y[i])
                    tkds_seeds_z.extend(tkds.seeds_z[i])

                tkus_seeds_global_x = array.array('d')
                tkus_seeds_global_y = array.array('d')
                tkus_seeds_global_z = array.array('d')
                tkds_seeds_global_x = array.array('d')
                tkds_seeds_global_y = array.array('d')
                tkds_seeds_global_z = array.array('d')
                for i in range(len(tkus.seeds_global_x)):
                    tkus_seeds_global_x.extend(tkus.seeds_global_x[i])
                    tkus_seeds_global_y.extend(tkus.seeds_global_y[i])
                    tkus_seeds_global_z.extend(tkus.seeds_global_z[i])
                for i in range(len(tkds.seeds_global_x)):
                    tkds_seeds_global_x.extend(tkds.seeds_global_x[i])
                    tkds_seeds_global_y.extend(tkds.seeds_global_y[i])
                    tkds_seeds_global_z.extend(tkds.seeds_global_z[i])

                seed_graphs = draw_spoints(c_sp_xyz, tkus_seeds_x,
                                  tkus_seeds_y, tkus_seeds_z, tkds_seeds_x,
                                  tkds_seeds_y, tkds_seeds_z, overlay=True,
                                  marker_style=24, marker_color=ROOT.kRed)

                draw_htracks(tkus, tkds, c_sp_xyz)
                draw_stracks(tkus, tkds, c_sp_xyz)

                c_sp_xyz.Update()

                # mg = draw_sz(tkus.seeds_z, tkus.seeds_s, tkds.seeds_z, \
                #              tkds.seeds_s, tkus.helix_sz_fits, \
                #              tkds.helix_sz_fits, c_trk_sz)
                # c_trk_sz.Update()

                # Make plots - global coordinates
                tk_spoint_data = [tkus.spoints_global_x, tkus.spoints_global_y,
                                  tkus.spoints_global_z, tkds.spoints_global_x,
                                  tkds.spoints_global_y, tkds.spoints_global_z]
                tk_seed_data = [tkus_seeds_global_x, tkus_seeds_global_y,
                                tkus_seeds_global_z, tkds_seeds_global_x,
                                tkds_seeds_global_y, tkds_seeds_global_z]
                tk_tpoint_data = [tkus.tpoints_global_x, tkus.tpoints_global_y,
                                  tkus.tpoints_global_z, tkds.tpoints_global_x,
                                  tkds.tpoints_global_y, tkds.tpoints_global_z]
                tof_data = [tof0.spoints_global_x, tof0.spoints_global_y,
                            tof0.spoints_global_z, tof1.spoints_global_x,
                            tof1.spoints_global_y, tof1.spoints_global_z,
                            tof2.spoints_global_x, tof2.spoints_global_y,
                            tof2.spoints_global_z]
                all_data = tk_spoint_data
                all_data.extend(tk_seed_data)
                all_data.extend(tk_tpoint_data)
                all_data.extend(tof_data)

                sp_global_graphs = draw_global_spoints(c_sp_gxyz, all_data)
                c_sp_gxyz.Update()
                draw_stracks_global(tkus, tkds, c_sp_gxyz)
                c_sp_gxyz.Update()

                tkus_pos = [tkus.tpoints_global_x, tkus.tpoints_global_y,
                            tkus.tpoints_global_z]

                tkds_pos = [tkds.tpoints_global_x, tkds.tpoints_global_y,
                            tkds.tpoints_global_z]

                tkus_mom = [tkus.tpoints_global_px, tkus.tpoints_global_py,
                            tkus.tpoints_global_pz]

                tkds_mom = [tkds.tpoints_global_px, tkds.tpoints_global_py,
                            tkds.tpoints_global_pz]

                tp_mom_graphs = draw_momentum(c_tp_mom, tkus_pos, tkds_pos,
                                              tkus_mom, tkds_mom)

                tkus_txz = 0.0
                tkus_tyz = 0.0
                tkds_txz = 0.0
                tkds_tyz = 0.0
                if len(tkus.straight_xz_fits) > 0:
                    tkus_txz = tkus.straight_xz_fits[0].GetParameter(0)
                if len(tkus.straight_yz_fits) > 0:
                    tkus_tyz = tkus.straight_yz_fits[0].GetParameter(0)
                if len(tkds.straight_xz_fits) > 0:
                    tkds_txz = tkds.straight_xz_fits[0].GetParameter(0)
                if len(tkds.straight_yz_fits) > 0:
                    tkds_tyz = tkds.straight_yz_fits[0].GetParameter(0)

                print 'Straight TkUS Pat Rec tracks: ',
                print str(len(tkus.straight_xz_fits))
                print 'Straight TkDS Pat Rec tracks: ',
                print str(len(tkds.straight_xz_fits))
                print 'tof0 spacepoints: ' + str(len(tof0.spoints_global_z))
                print 'tof1 spacepoints: ' + str(len(tof1.spoints_global_z))
                print 'tkus spacepoints: ' + str(len(tkus.spoints_z)),
                print ', tkus seeds: ' + str(tkus.num_sseeds),
                print ', tkus_tx: ' + str(tkus_txz) + ', tkus_ty: ',
                print str(tkus_tyz)
                print 'tkds spacepoints: ' + str(len(tkds.spoints_z)),
                print ', tkds seeds: ' + str(tkds.num_sseeds),
                print ', tkds_tx: ' + str(tkds_txz),
                print ', tkds_ty: ' + str(tkds_tyz)
                print 'tof2 spacepoints: ' + str(len(tof2.spoints_global_z))

                print 'Momentum'
                if len(tkus.tpoints_global_px) > 0:
                    print 'us-px: ' + str(tkus.tpoints_global_px[0]),
                    print ', us-py: ' + str(tkus.tpoints_global_py[0]),
                    print ', us-pz: ' + str(tkus.tpoints_global_pz[0])
                if len(tkds.tpoints_global_px) > 0:
                    print 'ds-px: ' + str(tkds.tpoints_global_px[0]),
                    print ', ds-py: ' + str(tkds.tpoints_global_py[0]),
                    print ', ds-pz: ' + str(tkds.tpoints_global_pz[0])

                # Pause between events
                raw_input("Press any key to move to the next event...")

                # mg[0].Clear()
                # mg[1].Clear()
                # del mg
                del tkus
                del tkds
                del tof0
                del tof1
                del tof2
        del spill

    raw_input("Press any key to finish...")
    print "Closing root file"
    root_file.Close()

def draw_spoints(can, t1_spoints_x, t1_spoints_y, t1_spoints_z,
                 t2_spoints_x, t2_spoints_y, t2_spoints_z,
                 overlay=False, marker_style=20, marker_color=ROOT.kBlack):
    """ Draw the spacepoints in real space (x-y, x-z, y-z) """
    graphs = []

    can.cd(1)
    if (len(t1_spoints_x) > 0) and (len(t1_spoints_y) > 0):
        t1_sp_xy_graph = ROOT.TGraph(len(t1_spoints_x), t1_spoints_x, \
                                     t1_spoints_y)
        t1_sp_xy_graph.SetTitle("Spacepoint x-y plot for tracker 1")
        t1_sp_xy_graph.GetXaxis().SetTitle("x(mm)")
        t1_sp_xy_graph.GetYaxis().SetTitle("y(mm)")
        t1_sp_xy_graph.SetMarkerStyle(marker_style)
        t1_sp_xy_graph.SetMarkerColor(marker_color)
        if overlay:
            t1_sp_xy_graph.Draw("P")
        else:
            t1_sp_xy_graph.Draw("AP")
        graphs.append(t1_sp_xy_graph)

    can.cd(2)
    if (len(t1_spoints_x) > 0) and (len(t1_spoints_z) > 0):
        t1_sp_xz_graph = ROOT.TGraph(len(t1_spoints_z), t1_spoints_z, \
                                     t1_spoints_x)
        t1_sp_xz_graph.SetTitle("Spacepoint z-x plot for tracker 1")
        t1_sp_xz_graph.GetXaxis().SetTitle("z(mm)")
        t1_sp_xz_graph.GetYaxis().SetTitle("x(mm)")
        t1_sp_xz_graph.SetMarkerStyle(marker_style)
        t1_sp_xz_graph.SetMarkerColor(marker_color)
        if overlay:
            t1_sp_xz_graph.Draw("P")
        else:
            t1_sp_xz_graph.Draw("AP")
        graphs.append(t1_sp_xz_graph)

    can.cd(3)
    if (len(t1_spoints_y) > 0) and (len(t1_spoints_z) > 0):
        t1_sp_yz_graph = ROOT.TGraph(len(t1_spoints_z), t1_spoints_z, \
                                     t1_spoints_y)
        t1_sp_yz_graph.SetTitle("Spacepoint z-y plot for tracker 1")
        t1_sp_yz_graph.GetXaxis().SetTitle("z(mm)")
        t1_sp_yz_graph.GetYaxis().SetTitle("y(mm)")
        t1_sp_yz_graph.SetMarkerStyle(marker_style)
        t1_sp_yz_graph.SetMarkerColor(marker_color)
        if overlay:
            t1_sp_yz_graph.Draw("P")
        else:
            t1_sp_yz_graph.Draw("AP")
        graphs.append(t1_sp_yz_graph)

    can.cd(4)
    if (len(t2_spoints_x) > 0) and (len(t2_spoints_y) > 0):
        t2_sp_xy_graph = ROOT.TGraph(len(t2_spoints_x), t2_spoints_x, \
                                     t2_spoints_y)
        t2_sp_xy_graph.SetTitle("Spacepoint x-y plot for tracker 2")
        t2_sp_xy_graph.GetXaxis().SetTitle("x(mm)")
        t2_sp_xy_graph.GetYaxis().SetTitle("y(mm)")
        t2_sp_xy_graph.SetMarkerStyle(marker_style)
        t2_sp_xy_graph.SetMarkerColor(marker_color)
        if overlay:
            t2_sp_xy_graph.Draw("P")
        else:
            t2_sp_xy_graph.Draw("AP")
        graphs.append(t2_sp_xy_graph)

    can.cd(5)
    if (len(t2_spoints_x) > 0) and (len(t2_spoints_z) > 0):
        t2_sp_xz_graph = ROOT.TGraph(len(t2_spoints_z), t2_spoints_z, \
                                     t2_spoints_x)
        t2_sp_xz_graph.SetTitle("Spacepoint z-x plot for tracker 2")
        t2_sp_xz_graph.GetXaxis().SetTitle("z(mm)")
        t2_sp_xz_graph.GetYaxis().SetTitle("x(mm)")
        t2_sp_xz_graph.SetMarkerStyle(marker_style)
        t2_sp_xz_graph.SetMarkerColor(marker_color)
        if overlay:
            t2_sp_xz_graph.Draw("P")
        else:
            t2_sp_xz_graph.Draw("AP")
        graphs.append(t2_sp_xz_graph)

    can.cd(6)
    if (len(t2_spoints_y) > 0) and (len(t2_spoints_z) > 0):
        t2_sp_yz_graph = ROOT.TGraph(len(t2_spoints_z), t2_spoints_z, \
                                    t2_spoints_y)
        t2_sp_yz_graph.SetTitle("Spacepoint z-y plot for tracker 2")
        t2_sp_yz_graph.GetXaxis().SetTitle("z(mm)")
        t2_sp_yz_graph.GetYaxis().SetTitle("y(mm)")
        t2_sp_yz_graph.SetMarkerStyle(marker_style)
        t2_sp_yz_graph.SetMarkerColor(marker_color)
        if overlay:
            t2_sp_yz_graph.Draw("P")
        else:
            t2_sp_yz_graph.Draw("AP")
        graphs.append(t2_sp_yz_graph)

    return graphs

def draw_global_spoints(can, all_data):
    """ Draw spacepoints in real space """

    tk_spoint_marker = 4
    tk_seed_marker = 20
    tk_tpoint_marker = 5
    tof_spoint_marker = 22

    tk_spoint_color = ROOT.kBlue
    tk_seed_color = ROOT.kBlue
    tk_tpoint_color = ROOT.kRed
    tof_color = ROOT.kOrange
    # ROOT.kCyan

    tkus_spoints_x = all_data[0]
    tkus_spoints_y = all_data[1]
    tkus_spoints_z = all_data[2]
    tkds_spoints_x = all_data[3]
    tkds_spoints_y = all_data[4]
    tkds_spoints_z = all_data[5]

    tkus_seeds_x = all_data[6]
    tkus_seeds_y = all_data[7]
    tkus_seeds_z = all_data[8]
    tkds_seeds_x = all_data[9]
    tkds_seeds_y = all_data[10]
    tkds_seeds_z = all_data[11]

    tkus_tpoints_x = all_data[12]
    tkus_tpoints_y = all_data[13]
    tkus_tpoints_z = all_data[14]
    tkds_tpoints_x = all_data[15]
    tkds_tpoints_y = all_data[16]
    tkds_tpoints_z = all_data[17]

    tof0_spoints_x = all_data[18]
    tof0_spoints_y = all_data[19]
    tof0_spoints_z = all_data[20]

    tof1_spoints_x = all_data[21]
    tof1_spoints_y = all_data[22]
    tof1_spoints_z = all_data[23]

    tof2_spoints_x = all_data[24]
    tof2_spoints_y = all_data[25]
    tof2_spoints_z = all_data[26]

    graphs1 = []
    can.cd(1)
    # SciFi spacepoints
    if (len(tkus_spoints_x) > 0) and (len(tkus_spoints_y) > 0):
        tkus_sp_xy_graph = ROOT.TGraph(len(tkus_spoints_x), \
                                      tkus_spoints_x, tkus_spoints_y)
        tkus_sp_xy_graph.SetMarkerStyle(tk_spoint_marker)
        tkus_sp_xy_graph.SetMarkerColor(tk_spoint_color)
        graphs1.append(tkus_sp_xy_graph)

    if (len(tkds_spoints_x) > 0) and (len(tkds_spoints_y) > 0):
        tkds_sp_xy_graph = ROOT.TGraph(len(tkds_spoints_x), \
                                      tkds_spoints_x, tkds_spoints_y)
        tkds_sp_xy_graph.SetMarkerColor(tk_spoint_color)
        tkds_sp_xy_graph.SetMarkerStyle(tk_spoint_marker)
        graphs1.append(tkds_sp_xy_graph)

    # SciFi seed spacepoints
    if (len(tkus_seeds_x) > 0) and (len(tkus_seeds_y) > 0):
        tkus_sd_xy_graph = ROOT.TGraph(len(tkus_seeds_x), \
                                      tkus_seeds_x, tkus_seeds_y)
        tkus_sd_xy_graph.SetMarkerStyle(tk_seed_marker)
        tkus_sd_xy_graph.SetMarkerColor(tk_seed_color)
        graphs1.append(tkus_sd_xy_graph)

    if (len(tkds_seeds_x) > 0) and (len(tkds_seeds_y) > 0):
        tkds_sd_xy_graph = ROOT.TGraph(len(tkds_seeds_x), \
                                      tkds_seeds_x, tkds_seeds_y)
        tkds_sd_xy_graph.SetMarkerColor(tk_seed_color)
        tkds_sd_xy_graph.SetMarkerStyle(tk_seed_marker)
        graphs1.append(tkds_sd_xy_graph)

    # SciFi trackpoints
    if (len(tkus_tpoints_x) > 0) and (len(tkus_tpoints_y) > 0):
        tkus_tp_xy_graph = ROOT.TGraph(len(tkus_tpoints_x), \
                                      tkus_tpoints_x, tkus_tpoints_y)
        tkus_tp_xy_graph.SetMarkerStyle(tk_tpoint_marker)
        tkus_tp_xy_graph.SetMarkerColor(tk_tpoint_color)
        graphs1.append(tkus_tp_xy_graph)

    if (len(tkds_tpoints_x) > 0) and (len(tkds_tpoints_y) > 0):
        tkds_tp_xy_graph = ROOT.TGraph(len(tkds_tpoints_x), \
                                      tkds_tpoints_x, tkds_tpoints_y)
        tkds_tp_xy_graph.SetMarkerStyle(tk_tpoint_marker)
        tkds_tp_xy_graph.SetMarkerColor(tk_tpoint_color)
        graphs1.append(tkds_tp_xy_graph)

    # TOF spacepoints
    if (len(tof0_spoints_x) > 0) and (len(tof0_spoints_y) > 0):
        tof0_sp_xy_graph = ROOT.TGraph(len(tof0_spoints_x), \
                                      tof0_spoints_x, tof0_spoints_y)
        tof0_sp_xy_graph.SetMarkerColor(tof_color)
        tof0_sp_xy_graph.SetMarkerStyle(tof_spoint_marker)
        graphs1.append(tof0_sp_xy_graph)

    if (len(tof1_spoints_x) > 0) and (len(tof1_spoints_y) > 0):
        tof1_sp_xy_graph = ROOT.TGraph(len(tof1_spoints_x), \
                                      tof1_spoints_x, tof1_spoints_y)
        tof1_sp_xy_graph.SetMarkerColor(tof_color)
        tof1_sp_xy_graph.SetMarkerStyle(tof_spoint_marker)
        graphs1.append(tof1_sp_xy_graph)

    if (len(tof2_spoints_x) > 0) and (len(tof2_spoints_y) > 0):
        tof2_sp_xy_graph = ROOT.TGraph(len(tof2_spoints_x), \
                                      tof2_spoints_x, tof2_spoints_y)
        tof2_sp_xy_graph.SetMarkerColor(tof_color)
        tof2_sp_xy_graph.SetMarkerStyle(tof_spoint_marker)
        graphs1.append(tof2_sp_xy_graph)

    for i in range(len(graphs1)):
        if i == 0:
            graphs1[i].SetTitle("Space and Track Points x-y")
            graphs1[i].GetXaxis().SetTitle("x(mm)")
            graphs1[i].GetYaxis().SetTitle("y(mm)")
            graphs1[i].Draw("AP")
            graphs1[i].GetXaxis().SetLimits(-170, 170)
            graphs1[i].GetYaxis().SetRangeUser(-170, 170)
            graphs1[i].Draw("AP")
        else:
            graphs1[i].Draw("P")

    graphs2 = []
    can.cd(2)
    # SciFi spacepoints
    if (len(tkus_spoints_x) > 0) and (len(tkus_spoints_z) > 0):
        tkus_sp_xz_graph = ROOT.TGraph(len(tkus_spoints_z), \
                                      tkus_spoints_z, tkus_spoints_x)
        tkus_sp_xz_graph.SetMarkerStyle(tk_spoint_marker)
        tkus_sp_xz_graph.SetMarkerColor(tk_spoint_color)
        graphs2.append(tkus_sp_xz_graph)

    if (len(tkds_spoints_x) > 0) and (len(tkds_spoints_z) > 0):
        tkds_sp_xz_graph = ROOT.TGraph(len(tkds_spoints_z), \
                                      tkds_spoints_z, tkds_spoints_x)
        tkds_sp_xz_graph.SetMarkerColor(tk_spoint_color)
        tkds_sp_xz_graph.SetMarkerStyle(tk_spoint_marker)
        graphs2.append(tkds_sp_xz_graph)

    # SciFi seed spacepoints
    if (len(tkus_seeds_x) > 0) and (len(tkus_seeds_z) > 0):
        tkus_sd_xz_graph = ROOT.TGraph(len(tkus_seeds_z), \
                                      tkus_seeds_z, tkus_seeds_x)
        tkus_sd_xz_graph.SetMarkerStyle(tk_seed_marker)
        tkus_sd_xz_graph.SetMarkerColor(tk_seed_color)
        graphs2.append(tkus_sd_xz_graph)

    if (len(tkds_seeds_x) > 0) and (len(tkds_seeds_z) > 0):
        tkds_sd_xz_graph = ROOT.TGraph(len(tkds_seeds_z), \
                                      tkds_seeds_z, tkds_seeds_x)
        tkds_sd_xz_graph.SetMarkerColor(tk_seed_color)
        tkds_sd_xz_graph.SetMarkerStyle(tk_seed_marker)
        graphs2.append(tkds_sd_xz_graph)

    # SciFi trackpoints
    if (len(tkus_tpoints_x) > 0) and (len(tkus_tpoints_z) > 0):
        tkus_tp_xz_graph = ROOT.TGraph(len(tkus_tpoints_z), \
                                      tkus_tpoints_z, tkus_tpoints_x)
        tkus_tp_xz_graph.SetMarkerStyle(tk_tpoint_marker)
        tkus_tp_xz_graph.SetMarkerColor(tk_tpoint_color)
        graphs2.append(tkus_tp_xz_graph)

    if (len(tkds_tpoints_x) > 0) and (len(tkds_tpoints_z) > 0):
        tkds_tp_xz_graph = ROOT.TGraph(len(tkds_tpoints_z), \
                                      tkds_tpoints_z, tkds_tpoints_x)
        tkds_tp_xz_graph.SetMarkerStyle(tk_tpoint_marker)
        tkds_tp_xz_graph.SetMarkerColor(tk_tpoint_color)
        graphs2.append(tkds_tp_xz_graph)

    # TOF spacepoints
    if (len(tof0_spoints_x) > 0) and (len(tof0_spoints_z) > 0):
        tof0_sp_xz_graph = ROOT.TGraph(len(tof0_spoints_z), \
                                      tof0_spoints_z, tof0_spoints_x)
        tof0_sp_xz_graph.SetMarkerColor(tof_color)
        tof0_sp_xz_graph.SetMarkerStyle(tof_spoint_marker)
        graphs2.append(tof0_sp_xz_graph)

    if (len(tof1_spoints_x) > 0) and (len(tof1_spoints_z) > 0):
        tof1_sp_xz_graph = ROOT.TGraph(len(tof1_spoints_z), \
                                      tof1_spoints_z, tof1_spoints_x)
        tof1_sp_xz_graph.SetMarkerColor(tof_color)
        tof1_sp_xz_graph.SetMarkerStyle(tof_spoint_marker)
        graphs2.append(tof1_sp_xz_graph)

    if (len(tof2_spoints_x) > 0) and (len(tof2_spoints_z) > 0):
        tof2_sp_xz_graph = ROOT.TGraph(len(tof2_spoints_z), \
                                      tof2_spoints_z, tof2_spoints_x)
        tof2_sp_xz_graph.SetMarkerColor(tof_color)
        tof2_sp_xz_graph.SetMarkerStyle(tof_spoint_marker)
        graphs2.append(tof2_sp_xz_graph)

    for i in range(len(graphs2)):
        if i == 0:
            graphs2[i].SetTitle("Space and Track Points z-x")
            graphs2[i].GetXaxis().SetTitle("z(mm)")
            graphs2[i].GetYaxis().SetTitle("x(mm)")
            graphs2[i].Draw("AP")
            graphs2[i].GetXaxis().SetLimits(2000, 22000)
            graphs2[i].GetYaxis().SetRangeUser(-170, 170)
            graphs2[i].Draw("AP")
        else:
            graphs2[i].Draw("P")

    graphs3 = []
    can.cd(3)
    # SciFi spacepoints
    if (len(tkus_spoints_y) > 0) and (len(tkus_spoints_z) > 0):
        tkus_sp_yz_graph = ROOT.TGraph(len(tkus_spoints_z), \
                                      tkus_spoints_z, tkus_spoints_y)
        tkus_sp_yz_graph.SetMarkerStyle(tk_spoint_marker)
        tkus_sp_yz_graph.SetMarkerColor(tk_spoint_color)
        graphs3.append(tkus_sp_yz_graph)

    if (len(tkds_spoints_y) > 0) and (len(tkds_spoints_z) > 0):
        tkds_sp_yz_graph = ROOT.TGraph(len(tkds_spoints_z), \
                                      tkds_spoints_z, tkds_spoints_y)
        tkds_sp_yz_graph.SetMarkerColor(tk_spoint_color)
        tkds_sp_yz_graph.SetMarkerStyle(tk_spoint_marker)
        graphs3.append(tkds_sp_yz_graph)

    # SciFi seed spacepoints
    if (len(tkus_seeds_y) > 0) and (len(tkus_seeds_z) > 0):
        print 'Drawing seeds upstream in z-y'
        tkus_sd_yz_graph = ROOT.TGraph(len(tkus_seeds_z), \
                                      tkus_seeds_z, tkus_seeds_y)
        tkus_sd_yz_graph.SetMarkerStyle(tk_seed_marker)
        tkus_sd_yz_graph.SetMarkerColor(tk_seed_color)
        graphs3.append(tkus_sd_yz_graph)

    if (len(tkds_seeds_y) > 0) and (len(tkds_seeds_z) > 0):
        print 'Drawing seeds downstream in z-y'
        tkds_sd_yz_graph = ROOT.TGraph(len(tkds_seeds_z), \
                                      tkds_seeds_z, tkds_seeds_y)
        tkds_sd_yz_graph.SetMarkerColor(tk_seed_color)
        tkds_sd_yz_graph.SetMarkerStyle(tk_seed_marker)
        graphs3.append(tkds_sd_yz_graph)

     # SciFi trackpoints
    if (len(tkus_tpoints_y) > 0) and (len(tkus_tpoints_z) > 0):
        tkus_tp_yz_graph = ROOT.TGraph(len(tkus_tpoints_z), \
                                      tkus_tpoints_z, tkus_tpoints_y)
        tkus_tp_yz_graph.SetMarkerStyle(tk_tpoint_marker)
        tkus_tp_yz_graph.SetMarkerColor(tk_tpoint_color)
        graphs3.append(tkus_tp_yz_graph)

    if (len(tkds_tpoints_y) > 0) and (len(tkds_tpoints_z) > 0):
        tkds_tp_yz_graph = ROOT.TGraph(len(tkds_tpoints_z), \
                                      tkds_tpoints_z, tkds_tpoints_y)
        tkds_tp_yz_graph.SetMarkerStyle(tk_tpoint_marker)
        tkds_tp_yz_graph.SetMarkerColor(tk_tpoint_color)
        graphs3.append(tkds_tp_yz_graph)

    # TOF spacepoints
    if (len(tof0_spoints_y) > 0) and (len(tof0_spoints_z) > 0):
        tof0_sp_yz_graph = ROOT.TGraph(len(tof0_spoints_z), \
                                      tof0_spoints_z, tof0_spoints_y)
        tof0_sp_yz_graph.SetMarkerColor(tof_color)
        tof0_sp_yz_graph.SetMarkerStyle(tof_spoint_marker)
        graphs3.append(tof0_sp_yz_graph)

    if (len(tof1_spoints_y) > 0) and (len(tof1_spoints_z) > 0):
        tof1_sp_yz_graph = ROOT.TGraph(len(tof1_spoints_z), \
                                      tof1_spoints_z, tof1_spoints_y)
        tof1_sp_yz_graph.SetMarkerColor(tof_color)
        tof1_sp_yz_graph.SetMarkerStyle(tof_spoint_marker)
        graphs3.append(tof1_sp_yz_graph)

    if (len(tof2_spoints_y) > 0) and (len(tof2_spoints_z) > 0):
        tof2_sp_yz_graph = ROOT.TGraph(len(tof2_spoints_z), \
                                      tof2_spoints_z, tof2_spoints_y)
        tof2_sp_yz_graph.SetMarkerColor(tof_color)
        tof2_sp_yz_graph.SetMarkerStyle(tof_spoint_marker)
        graphs3.append(tof2_sp_yz_graph)

    for i in range(len(graphs3)):
        if i == 0:
            graphs3[i].SetTitle("Space and Track Points z-y")
            graphs3[i].GetXaxis().SetTitle("z(mm)")
            graphs3[i].GetYaxis().SetTitle("y(mm)")
            graphs3[i].Draw("AP")
            graphs3[i].GetXaxis().SetLimits(2000, 22000)
            graphs3[i].GetYaxis().SetRangeUser(-170, 170)
            graphs3[i].Draw("AP")
        else:
            graphs3[i].Draw("P")

    can.Update()

    return graphs1 + graphs2 + graphs3

def draw_momentum(can, tkus_pos, tkds_pos, tkus_mom, tkds_mom):
    """ Draw scifi trackpoint momentum """
    tkus_pos_x = tkus_pos[0]
    tkus_pos_y = tkus_pos[1]
    tkus_pos_z = tkus_pos[2]
    tkds_pos_x = tkds_pos[0]
    tkds_pos_y = tkds_pos[1]
    tkds_pos_z = tkds_pos[2]

    tkus_mom_x = tkus_mom[0]
    tkus_mom_y = tkus_mom[1]
    tkus_mom_z = tkus_mom[2]
    tkds_mom_x = tkds_mom[0]
    tkds_mom_y = tkds_mom[1]
    tkds_mom_z = tkds_mom[2]

    graphs = []

    can.cd()
    if (len(tkus_pos_z) > 0) and (len(tkus_mom_x) > 0):
        tkus_tp_pxz_graph = ROOT.TGraph(len(tkus_pos_z), tkus_pos_z, tkus_mom_x)
        tkus_tp_pxz_graph.SetTitle("Trackpoint px and py vs z")
        tkus_tp_pxz_graph.GetXaxis().SetTitle("z(mm)")
        tkus_tp_pxz_graph.GetYaxis().SetTitle("pt(MeV/C)")
        tkus_tp_pxz_graph.GetXaxis().SetLimits(13000, 21000)
        tkus_tp_pxz_graph.GetYaxis().SetRangeUser(-50, 50)
        tkus_tp_pxz_graph.SetMarkerStyle(20)
        tkus_tp_pxz_graph.SetMarkerColor(ROOT.kGreen)
        tkus_tp_pxz_graph.Draw("AP")
        graphs.append(tkus_tp_pxz_graph)
    if (len(tkds_pos_z) > 0) and (len(tkds_mom_x) > 0):
        tkds_tp_pxz_graph = ROOT.TGraph(len(tkds_pos_z), tkds_pos_z, tkds_mom_x)
        tkds_tp_pxz_graph.SetTitle("Trackpoint px vs z")
        tkds_tp_pxz_graph.GetXaxis().SetTitle("z(mm)")
        tkds_tp_pxz_graph.GetYaxis().SetTitle("px(MeV/C)")
        tkds_tp_pxz_graph.SetMarkerStyle(20)
        tkds_tp_pxz_graph.SetMarkerColor(ROOT.kGreen)
        tkds_tp_pxz_graph.Draw("P")
        graphs.append(tkds_tp_pxz_graph)
    if (len(tkus_pos_z) > 0) and (len(tkus_mom_y) > 0):
        tkus_tp_pyz_graph = ROOT.TGraph(len(tkus_pos_z), tkus_pos_z, tkus_mom_y)
        tkus_tp_pyz_graph.SetTitle("Trackpoint py vs z")
        tkus_tp_pyz_graph.GetXaxis().SetTitle("z(mm)")
        tkus_tp_pyz_graph.GetYaxis().SetTitle("py(MeV/C)")
        tkus_tp_pyz_graph.SetMarkerStyle(20)
        tkus_tp_pyz_graph.SetMarkerColor(ROOT.kMagenta)
        tkus_tp_pyz_graph.Draw("P")
        graphs.append(tkus_tp_pyz_graph)
    if (len(tkds_pos_z) > 0) and (len(tkds_mom_y) > 0):
        tkds_tp_pyz_graph = ROOT.TGraph(len(tkds_pos_z), tkds_pos_z, tkds_mom_y)
        tkds_tp_pyz_graph.SetTitle("Trackpoint py vs z")
        tkds_tp_pyz_graph.GetXaxis().SetTitle("z(mm)")
        tkds_tp_pyz_graph.GetYaxis().SetTitle("py(MeV/C)")
        tkds_tp_pyz_graph.SetMarkerStyle(20)
        tkds_tp_pyz_graph.SetMarkerColor(ROOT.kMagenta)
        tkds_tp_pyz_graph.Draw("P")
        graphs.append(tkds_tp_pyz_graph)

    can.Update()

    return graphs

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
    if ( len(t1_fits) > 0 ):
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
    if ( len(t2_fits) > 0 ):
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

def draw_stracks_global(t1, t2, can):
    """ Draw the straight fits over the spacepoints """
    print 'Drawing global pr straight track fits'
    print 'No. of x-z fits: ' + str(len(t1.straight_global_xz_fits))
    for line in t1.straight_global_xz_fits:
        print 'Param 0: ' + str(line.GetParameter(0))
        print 'Param 1: ' + str(line.GetParameter(1))
        can.cd(2)
        line.SetLineColor(ROOT.kBlue)
        line.Draw("same")
        can.Update()
    for line in t1.straight_global_yz_fits:
        can.cd(3)
        line.SetLineColor(ROOT.kBlue)
        line.Draw("same")
        can.Update()
    for line in t2.straight_global_xz_fits:
        can.cd(2)
        line.SetLineColor(ROOT.kBlue)
        line.Draw("same")
        can.Update()
    for line in t2.straight_global_yz_fits:
        can.cd(3)
        line.SetLineColor(ROOT.kBlue)
        line.Draw("same")
        can.Update()

if __name__ == "__main__":
    main(sys.argv[1])

