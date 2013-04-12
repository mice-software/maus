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
# pylint: disable = R0914
# pylint: disable = R0915
# pylint: disable = C0103
# pylint: disable = R0912
# pylint: disable = R0913
# pylint: disable = W0612

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

    # Loop over spills
    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        spill = data.GetSpill()
        # Print some basic information about the spill
        print "Found spill number", spill.GetSpillNumber(),
        print "in run number", spill.GetRunNumber(),
        # physics_events -> to particle data. Everything else is DAQ.
        print "DAQ event type", spill.GetDaqEventType()
        if spill.GetDaqEventType() == "physics_event":

            # Loop over all the particle events in a spill
            for i in range(spill.GetReconEvents().size()):
                # Some setup
                t1_sp_x_arr = array.array('d', [0])
                t1_sp_y_arr = array.array('d', [0])
                t1_sp_z_arr = array.array('d', [0])
                t1_sp_phi_arr = array.array('d', [0])
                t2_sp_x_arr = array.array('d', [0])
                t2_sp_y_arr = array.array('d', [0])
                t2_sp_z_arr = array.array('d', [0])
                t2_sp_phi_arr = array.array('d', [0])
                t1_circles_xy = []
                t2_circles_xy = []
                t1_num_sp = 0
                t2_num_sp = 0
                evt = spill.GetReconEvents()[i].GetSciFiEvent()
                spoints = evt.spacepoints()

                # Loop over spacepoints and pull out data to plot
                for j in range(spoints.size()):
                    if spoints[j].get_tracker() == 0:
                        x = spoints[j].get_position().x()
                        y = spoints[j].get_position().y()
                        z = spoints[j].get_position().z()
                        if t1_num_sp == 0:
                            t1_sp_x_arr[0] = x
                            t1_sp_y_arr[0] = y
                            t1_sp_z_arr[0] = z
                            t1_num_sp = t1_num_sp + 1
                        else:
                            t1_sp_x_arr.append(x)
                            t1_sp_y_arr.append(y)
                            t1_sp_z_arr.append(z)
                            t1_num_sp = t1_num_sp + 1
                    elif spoints[j].get_tracker() == 1:
                        x = spoints[j].get_position().x()
                        y = spoints[j].get_position().y()
                        z = spoints[j].get_position().z()
                        if t2_num_sp == 0:
                            t2_sp_x_arr[0] = x
                            t2_sp_y_arr[0] = y
                            t2_sp_z_arr[0] = z
                            t2_num_sp = t2_num_sp + 1
                        else:
                            t2_sp_x_arr.append(x)
                            t2_sp_y_arr.append(y)
                            t2_sp_z_arr.append(z)
                            t2_num_sp = t2_num_sp + 1

                # Loop over helical tracks and pull out data to plot
                trks = evt.helicalprtracks()
                print 'Found ' + str(trks.size()) + ' helical tracks'
                for j in range(trks.size()):
                    print 'Track ' + str(j) + ':'

                    # Extract circle fit data
                    if trks[j].get_tracker() == 0:
                        x0 = trks[j].get_circle_x0()
                        y0 = trks[j].get_circle_y0()
                        rad = trks[j].get_R()
                        t1_circles_xy.append(make_circle(x0, y0, rad))
                    elif trks[j].get_tracker() == 1:
                        x0 = trks[j].get_circle_x0()
                        y0 = trks[j].get_circle_y0()
                        rad = trks[j].get_R()
                        t2_circles_xy.append(make_circle(x0, y0, rad))

                    # Print turning angles to command line
                    phi = trks[j].get_phi()
                    print phi.size()
                    for k, phi_i in enumerate(phi):
                        print 'phi' + str(k+1) + ' is ' + str(phi_i)

                print "Making plots..."
                sp_graphs = draw_spoints(c_sp_xy, t1_sp_x_arr, t1_sp_y_arr,
                        t1_sp_z_arr, t2_sp_x_arr, t2_sp_y_arr, t2_sp_z_arr)
                draw_htracks(t1_circles_xy, t2_circles_xy, c_sp_xy)
                c_sp_xy.Update()
                raw_input("Press any key to move to the next event...")

    print "Closing root file"
    root_file.Close()

def make_circle(x0, y0, rad):
    """ Make a circle from its centre coords & radius using the TArc class"""

    arc = ROOT.TArc(x0, y0, rad)
    arc.SetFillStyle(0)   # 0 - Transparent
    arc.SetLineColor(ROOT.kBlue)
    return arc

def draw_spoints(can, t1_sp_x_arr, t1_sp_y_arr, t1_sp_z_arr,
                 t2_sp_x_arr, t2_sp_y_arr, t2_sp_z_arr):
    """ Draw the spacepoints in real space (x-y, x-z, y-z) """

    can.cd(1)
    t1_sp_xy_graph = ROOT.TGraph(len(t1_sp_x_arr), t1_sp_x_arr, t1_sp_y_arr)
    t1_sp_xy_graph.SetTitle("Spacepoint x-y plot for tracker 1")
    t1_sp_xy_graph.GetXaxis().SetTitle("x(mm)")
    t1_sp_xy_graph.GetYaxis().SetTitle("y(mm)")
    t1_sp_xy_graph.SetMarkerStyle(20)
    t1_sp_xy_graph.SetMarkerColor(ROOT.kBlack)
    t1_sp_xy_graph.Draw("AP")

    can.cd(2)
    t1_sp_xz_graph = ROOT.TGraph(len(t1_sp_z_arr), t1_sp_z_arr, t1_sp_x_arr)
    t1_sp_xz_graph.SetTitle("Spacepoint z-x plot for tracker 1")
    t1_sp_xz_graph.GetXaxis().SetTitle("z(mm)")
    t1_sp_xz_graph.GetYaxis().SetTitle("x(mm)")
    t1_sp_xz_graph.SetMarkerStyle(20)
    t1_sp_xz_graph.SetMarkerColor(ROOT.kBlack)
    t1_sp_xz_graph.Draw("AP")

    can.cd(3)
    t1_sp_yz_graph = ROOT.TGraph(len(t1_sp_z_arr), t1_sp_z_arr, t1_sp_y_arr)
    t1_sp_yz_graph.SetTitle("Spacepoint z-y plot for tracker 1")
    t1_sp_yz_graph.GetXaxis().SetTitle("z(mm)")
    t1_sp_yz_graph.GetYaxis().SetTitle("y(mm)")
    t1_sp_yz_graph.SetMarkerStyle(20)
    t1_sp_yz_graph.SetMarkerColor(ROOT.kBlack)
    t1_sp_yz_graph.Draw("AP")

    can.cd(4)
    t2_sp_xy_graph = ROOT.TGraph(len(t2_sp_x_arr), t2_sp_x_arr, t2_sp_y_arr)
    t2_sp_xy_graph.SetTitle("Spacepoint x-y plot for tracker 2")
    t2_sp_xy_graph.GetXaxis().SetTitle("x(mm)")
    t2_sp_xy_graph.GetYaxis().SetTitle("y(mm)")
    t2_sp_xy_graph.SetMarkerStyle(20)
    t2_sp_xy_graph.SetMarkerColor(ROOT.kBlack)
    t2_sp_xy_graph.Draw("AP")

    can.cd(5)
    t2_sp_xz_graph = ROOT.TGraph(len(t2_sp_z_arr), t2_sp_z_arr, t2_sp_x_arr)
    t2_sp_xz_graph.SetTitle("Spacepoint z-x plot for tracker 2")
    t2_sp_xz_graph.GetXaxis().SetTitle("z(mm)")
    t2_sp_xz_graph.GetYaxis().SetTitle("x(mm)")
    t2_sp_xz_graph.SetMarkerStyle(20)
    t2_sp_xz_graph.SetMarkerColor(ROOT.kBlack)
    t2_sp_xz_graph.Draw("AP")

    can.cd(6)
    t2_sp_yz_graph = ROOT.TGraph(len(t2_sp_z_arr), t2_sp_z_arr, t2_sp_y_arr)
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

if __name__ == "__main__":
    main(sys.argv[1])

