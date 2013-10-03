#!/usr/bin/env python

""" Create momentum plots of mc vs recon for SciFi Pattern Recognition """

import sys
import math
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.backends.backend_pdf import PdfPages
from collections import deque

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

    t1_pt = deque()
    t1_pz = deque()
    t1_pt_res = deque()
    t1_pz_res = deque()
    t2_pt = deque()
    t2_pz = deque()
    t2_pt_res = deque()
    t2_pz_res = deque()

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
                            t1_pt.append(pt)
                            t1_pz.append(pz)
                            t1_pt_res.append(pt_mc - pt)
                            t1_pz_res.append(pz_mc - (-pz)) # Note extra - sign
                        elif trk.get_tracker() == 1:
                            t2_pt.append(pt)
                            t2_pz.append(pz)
                            t2_pt_res.append(pt_mc - pt)
                            t2_pz_res.append(pz_mc - pz)
                    else:
                        print "Bad track, skipping"

    # Convert our deques to numpy arrays
    t1_pt = np.array(t1_pt)
    t1_pt_res = np.array(t1_pt_res)
    t1_pz = np.array(t1_pz)
    t1_pz_res = np.array(t1_pz_res)
    t2_pt = np.array(t2_pt)
    t2_pt_res = np.array(t2_pt_res)
    t2_pz = np.array(t2_pz)
    t2_pz_res = np.array(t2_pz_res)

    font = {'family' : 'serif', 'size' : 7}
    mpl.rc('font', **font) # pylint: disable = W0142
    props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
    pp = PdfPages('test.pdf')

    fig = plt.figure(1)
    gs1 = gridspec.GridSpec(2, 3)

    ax = plt.subplot(gs1[0, 0])
    plt.hist(t1_pt, 50)
    plt.title('T1 pt')
    plt.xlabel(r'$p_T (MeV/c)$')
    textstr = set_stat_str(t1_pt)
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    ax = plt.subplot(gs1[0, 1])
    plt.hist(t1_pt_res, 50)
    plt.title('T1 pt res')
    plt.xlabel(r'$p_T ^{MC} - p_T (MeV/c)$')
    textstr = set_stat_str(t1_pt_res)
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    ax = plt.subplot(gs1[0, 2])
    plt.hist(t1_pt_res, 50, log=True)
    plt.title('T1 pt res')
    plt.xlabel(r'$p_T ^{MC} - p_T (MeV/c)$')
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    ax = plt.subplot(gs1[1, 0])
    plt.hist(t1_pz, 50)
    plt.title('T1 pz')
    plt.xlabel(r'$p_z (MeV/c)$')
    textstr = set_stat_str(t1_pz)
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    ax = plt.subplot(gs1[1, 1])
    plt.hist(t1_pz_res, 50)
    plt.title('T1 pz res')
    plt.xlabel(r'$p_z ^{MC} - p_z (MeV/c)$')
    textstr = set_stat_str(t1_pz_res)
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    ax = plt.subplot(gs1[1, 2])
    plt.hist(t1_pz_res, 50, log=True)
    plt.title('T1 pz res')
    plt.xlabel(r'$p_z ^{MC} - p_z (MeV/c)$')
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    # plt.tight_layout()
    gs1.tight_layout(fig)
    plt.savefig(pp, format='pdf')

    fig = plt.figure(2)
    gs2 = gridspec.GridSpec(2, 3)

    ax = plt.subplot(gs2[0, 0])
    plt.hist(t2_pt, 50)
    plt.title('T2 pt')
    plt.xlabel(r'$p_T (MeV/c)$')
    textstr = set_stat_str(t2_pt)
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    ax = plt.subplot(gs2[0, 1])
    plt.hist(t2_pt_res, 50)
    plt.title('T2 pt res')
    plt.xlabel(r'$p_T ^{MC} - p_T (MeV/c)$')
    textstr = set_stat_str(t2_pt_res)
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    ax = plt.subplot(gs2[0, 2])
    plt.hist(t2_pt_res, 50, log=True)
    plt.title('T2 pt res')
    plt.xlabel(r'$p_T ^{MC} - p_T (MeV/c)$')
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    ax = plt.subplot(gs2[1, 0])
    plt.hist(t2_pz, 50)
    plt.title('T2 pz')
    plt.xlabel(r'$p_z (MeV/c)$')
    textstr = set_stat_str(t2_pz)
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    ax = plt.subplot(gs2[1, 1])
    plt.hist(t2_pz_res, 50)
    plt.title('T2 pz res')
    plt.xlabel(r'$p_z ^{MC} - p_z (MeV/c)$')
    textstr = set_stat_str(t2_pz_res)
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    ax = plt.subplot(gs2[1, 2])
    plt.hist(t2_pz_res, 50, log=True)
    plt.title('T2 pz res')
    plt.xlabel(r'$p_z ^{MC} - p_z (MeV/c)$')
    ax.text(0.95, 0.95, textstr, transform=ax.transAxes, fontsize=9,
            ha='right', va='top', bbox=props)

    # plt.tight_layout()
    gs2.tight_layout(fig)
    plt.savefig(pp, format='pdf')
    pp.close()

def set_stat_str(data):
    """ Return a string of distribution stats """
    N = len(data)
    mu = data.mean()
    sigma = data.std()
    return '$N=%i$\n$\mu=%.2f$\n$\sigma=%.2f$' % (N, mu, sigma)

if __name__ == "__main__":
    main(sys.argv[1])
