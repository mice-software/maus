#!/usr/bin/env python

"""
Example to load a ROOT file with SciFi data and do some analysis
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

def main(file_name):
    """
    The main function which performs the analysis and produces the plots
    """

    # Load the ROOT file
    print "Loading ROOT file", file_name
    root_file = ROOT.TFile(file_name, "READ") # pylint: disable = E1101

    # and set up the data tree to be filled by ROOT IO
    print "Setting up data tree"
    data = ROOT.MAUS.Data() # pylint: disable = E1101
    tree = root_file.Get("Spill")
    tree.SetBranchAddress("data", data)

    # We will try to load the data now into a analysable format and use it to
    # make a profile of the beam by plotting the number of digits (PMT pulses)
    # in the Tracker
    print "Getting some data"
    t1_sp_x_hist = ROOT.TH1D("t1_sp", # pylint: disable = E1101
                             "t1 spoints x coord",
                             7, -0.5, 6.5)
    t2_sp_x_hist = ROOT.TH1D("t2_sp", # pylint: disable = E1101
                             "t2 spoints x coord",
                             7, -0.5, 6.5)
    t1_sp_x_arr = array.array('d', [0])
    t1_sp_y_arr = array.array('d', [0])
    t2_sp_x_arr = array.array('d', [0])
    t2_sp_y_arr = array.array('d', [0])

    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        spill = data.GetSpill()
        # Print some basic information about the spill
        print "Found spill number", spill.GetSpillNumber(),
        print "in run number", spill.GetRunNumber(),
        # physics_events correspond to particle data. Everything else is DAQ
        # bureaucracy
        print "DAQ event type", spill.GetDaqEventType()
        if spill.GetDaqEventType() == "physics_event":
            # note PyROOT gives a segmentation fault if we try to call the STL
            # vector directly
            for i in range(spill.GetReconEvents().size()):
                evt = spill.GetReconEvents()[i].GetSciFiEvent()
                spoints = evt.spacepoints()
                for j in range(spoints.size()):
                    if spoints[j].get_tracker() == 0:
                        x = spoints[j].get_position().x()
                        y = spoints[j].get_position().y()
                        t1_sp_x_hist.Fill(x)
                        if t1_sp_x_arr == 0:
                            t1_sp_x_arr[0] = x
                            t1_sp_y_arr[0] = y
                        else:
                            t1_sp_x_arr.append(x)
                            t1_sp_y_arr.append(y)
                    elif spoints[j].get_tracker() == 1:
                        x = spoints[j].get_position().x()
                        y = spoints[j].get_position().y()
                        t2_sp_x_hist.Fill(x)
                        if t2_sp_x_arr == 0:
                            t2_sp_x_arr[0] = x
                            t2_sp_y_arr[0] = y
                        else:
                            t2_sp_x_arr.append(x)
                            t2_sp_y_arr.append(y)

    # draw the histograms and graphs and write to disk
    print "Writing histogram files"
    canvas_0 = ROOT.TCanvas("c_t1_sp",
                            "c_t1_sp")
    t1_sp_x_hist.Draw()
    canvas_0.Draw()
    canvas_0.Print('t1_sp_x_analyse_root_file.png')

    canvas_1 = ROOT.TCanvas("c_t2_sp",
                            "c_t2_sp")
    t2_sp_x_hist.Draw()
    canvas_1.Draw()
    canvas_1.Print('t2_sp_x_analyse_root_file.png')

    print "Writing graph files"
    t1_sp_xy_graph = ROOT.TGraph(len(t1_sp_x_arr), t1_sp_x_arr, t1_sp_y_arr)
    canvas_2 = ROOT.TCanvas("c_t1_sp_gr",
                            "c_t1_sp_gr")
    t1_sp_xy_graph.SetTitle("Spacepoint x-y plot for tracker 1")
    t1_sp_xy_graph.GetXaxis().SetTitle("x(mm)")
    t1_sp_xy_graph.GetYaxis().SetTitle("y(mm)")
    t1_sp_xy_graph.SetMarkerStyle(1)
    t1_sp_xy_graph.SetMarkerColor(ROOT.kBlue)
    t1_sp_xy_graph.Draw("AP")
    canvas_2.Print('t1_sp_xy_analyse_root_file.png')

    t2_sp_xy_graph = ROOT.TGraph(len(t2_sp_x_arr), t2_sp_x_arr, t2_sp_y_arr)
    canvas_3 = ROOT.TCanvas("c_t2_sp_gr",
                            "c_t2_sp_gr")
    t2_sp_xy_graph.SetTitle("Spacepoint x-y plot for tracker 2")
    t2_sp_xy_graph.GetXaxis().SetTitle("x(mm)")
    t2_sp_xy_graph.GetYaxis().SetTitle("y(mm)")
    t2_sp_xy_graph.SetMarkerStyle(1)
    t2_sp_xy_graph.SetMarkerColor(ROOT.kBlue)
    t2_sp_xy_graph.Draw("AP")
    canvas_3.Print('t2_sp_xy_analyse_root_file.png')

    print "Closing root file"

    # A feature of ROOT is that closing the root file has weird effects like
    # deleting the histograms drawn above from memory - beware. Probably also
    # silently deallocates memory assigned to data. Probably does some other
    # sinister stuff.
    root_file.Close()

if __name__ == "__main__":
    main(sys.argv[1])

