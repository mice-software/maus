#!/usr/bin/env python

""" Create momentum plots of mc vs recon for SciFi Pattern Recognition """

import sys
import math
from array import array

# basic PyROOT definitions
import ROOT

# definitions of MAUS data structure for PyROOT
import libMausCpp #pylint: disable = W0611

# pylint: disable = E1101
# pylint: disable = R0914
# pylint: disable = R0915
# pylint: disable = C0103

def main(file_name):
    """ The main func which performs the analysis and produces the plots """

    # Load the ROOT file
    print "Loading ROOT file", file_name
    root_file = ROOT.TFile(file_name, "READ") # pylint: disable = E1101
    ROOT.gStyle.SetOptStat(1111111)

    # and set up the data tree to be filled by ROOT IO
    print "Setting up data tree"
    data = ROOT.MAUS.Data() # pylint: disable = E1101
    tree = root_file.Get("Spill")
    tree.SetBranchAddress("data", data)

    # Output tree
    out_tree = ROOT.TTree("tracker_data", "tracker_data")
    a_pt = array('d', [0])
    a_pz = array('d', [0])
    a_pt = array('d', [0])
    a_pz = array('d', [0])
    a_n_points = array('i', [0])
    a_trker_num = array('i', [0])
    out_tree.SetBranchAddress("pt", a_pt)
    out_tree.SetBranchAddress("pz", a_pz)
    out_tree.SetBranchAddress("n_points", a_n_points)
    out_tree.SetBranchAddress("trker_num", a_trker_num)

    # Momentum histos 
    mom_n_bins = 100

    t1_pt = ROOT.TH1D("t1_pt", "T1 pt", \
                      mom_n_bins, 0, 150)
    t1_pt.GetXaxis().SetTitle("p_{T}^{MC} (MeV/c)")

    t1_pz = ROOT.TH1D("t1_pz", "T1 pz", \
                      mom_n_bins, -300, -50)
    t1_pz.GetXaxis().SetTitle("p_{z}^{MC} (MeV/c)")

    t2_pt = ROOT.TH1D("t2_pt", "T2 pt", \
                      mom_n_bins, 0, 150)
    t2_pt.GetXaxis().SetTitle("p_{T}^{MC} (MeV/c)")

    t2_pz = ROOT.TH1D("t2_pz", "T2 pz", \
                      mom_n_bins, 50, 300)
    t2_pz.GetXaxis().SetTitle("p_{z}^{MC} (MeV/c)")

    # Momentum residual histos
    res_n_bins = 100

    t1_pt_res = ROOT.TH1D("t1_pt_res", "T1 pt Residual", \
                      res_n_bins, -5, 5)
    t1_pt_res.GetXaxis().SetTitle("p_{T}^{MC} - p_{T} (MeV/c)")

    t1_pz_res = ROOT.TH1D("t1_pz_res", "T1 pz Residual", \
                      res_n_bins, -30, 30)
    t1_pz_res.GetXaxis().SetTitle("p_{z}^{MC} - p_{z} (MeV/c)")

    t1_pz_res_log = ROOT.TH1D("t1_pz_res_log", "T1 pz Residual", \
                      res_n_bins, -500, 500)
    t1_pz_res_log.GetXaxis().SetTitle("p_{z}^{MC} - p_{z} (MeV/c)")

    t2_pt_res = ROOT.TH1D("t2_pt_res", "T2 pt Residual", \
                      res_n_bins, -5, 5)
    t2_pt_res.GetXaxis().SetTitle("p_{T}^{MC} - p_{T} (MeV/c)")

    t2_pz_res = ROOT.TH1D("t2_pz_res", "T2 pz Residual", \
                      res_n_bins, -30, 30)
    t2_pz_res.GetXaxis().SetTitle("p_{z}^{MC} - p_{z} (MeV/c)")

    t2_pz_res_log = ROOT.TH1D("t2_pz_res_log", "T1 pz Residual", \
                      res_n_bins, -500, 500)
    t2_pz_res_log.GetXaxis().SetTitle("p_{z}^{MC} - p_{z} (MeV/c)")

    print "Looping over spills"
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
            for evt in spill.GetReconEvents():
                trks = evt.GetSciFiEvent().helicalprtracks()
                for trk in trks:
                    if ( (trk.get_R() != 0.0) & (trk.get_dsdz() != 0.0) ):
                        # line_sz_chisq = trk.get_line_sz_chisq()
                        pt = 1.2 * trk.get_R()
                        pz = pt / trk.get_dsdz()
                        pt_mc = 0.0
                        pz_mc = 0.0
                        for seed in trk.get_spacepoints():
                            seed_px_mc = 0.0
                            seed_py_mc = 0.0
                            seed_pz_mc = 0.0
                            for clus in seed.get_channels():
                                p_mc = clus.get_true_momentum()
                                seed_px_mc = seed_px_mc + p_mc.x()
                                seed_py_mc = seed_py_mc + p_mc.y()
                                seed_pz_mc = seed_pz_mc + p_mc.z()
                            seed_px_mc = seed_px_mc / seed.get_channels().size()
                            seed_py_mc = seed_py_mc / seed.get_channels().size()
                            seed_pz_mc = seed_pz_mc / seed.get_channels().size()
                            pt_mc = pt_mc + math.sqrt(seed_px_mc*seed_px_mc + \
                                                      seed_py_mc*seed_py_mc)
                            pz_mc = pz_mc + seed_pz_mc
                        pt_mc = pt_mc / trk.get_spacepoints().size()
                        pz_mc = pz_mc / trk.get_spacepoints().size()
                        if trk.get_tracker() == 0:
                            t1_pt.Fill(pt)
                            t1_pz.Fill(pz)
                            t1_pt_res.Fill(pt_mc - pt)
                            t1_pz_res.Fill(pz_mc - (-pz)) # Note extra - sign
                            t1_pz_res_log.Fill(pz_mc - (-pz))
                        elif trk.get_tracker() == 1:
                            t2_pt.Fill(pt)
                            t2_pz.Fill(pz)
                            t2_pt_res.Fill(pt_mc - pt)
                            t2_pz_res.Fill(pz_mc - pz)
                            t2_pz_res_log.Fill(pz_mc - pz)
                    else:
                        print "Bad track, skipping"


    # Draw the histograms and write to disk
    print "Drawing histograms"

    # Transverse momentum histos
    cPt = ROOT.TCanvas("cPt","cPt")
    cPt.Divide(3, 2)

    cPt.cd(1)
    t1_pt.Draw()
    cPt.Update()
    cPt.cd(2)
    t1_pt_res.Draw()
    cPt.Update()
    p1 = cPt.cd(3)
    p1.SetLogy()
    t1_pt_res.Draw()
    cPt.Update()

    cPt.cd(4)
    t2_pt.Draw()
    cPt.Update()
    cPt.cd(5)
    t2_pt_res.Draw()
    cPt.Update()
    p1 = cPt.cd(6)
    p1.SetLogy()
    t2_pt_res.Draw()
    cPt.Update()

    # Longitudinal momentum histos
    cPz = ROOT.TCanvas("cPz","cPz")
    cPz.Divide(3, 2)

    cPz.cd(1)
    t1_pz.Draw()
    cPz.Update()
    cPz.cd(2)
    t1_pz_res.Draw()
    cPz.Update()
    p1 = cPz.cd(3)
    p1.SetLogy()
    t1_pz_res_log.Draw()
    cPz.Update()

    cPz.cd(4)
    t2_pz.Draw()
    cPz.Update()
    cPz.cd(5)
    t2_pz_res.Draw()
    cPz.Update()
    p1 = cPz.cd(6)
    p1.SetLogy()
    t2_pz_res_log.Draw()
    cPz.Update()

    # Save the histos
    cPt.Print('mc_pt_analysis.pdf')
    cPz.Print('mc_pz_analysis.pdf')

    # Save to a ROOT file too
    out_file = ROOT.TFile("momentum_analysis_out.root", "recreate")
    out_file.cd()
    t1_pt.Write()
    t1_pz.Write()
    t2_pt.Write()
    t2_pz.Write()
    cPt.Write()
    cPz.Write()
    out_file.Close()

    raw_input("Press any key when ready to close...")
    root_file.Close()

if __name__ == "__main__":
    main(sys.argv[1])
