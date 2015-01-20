#!/usr/bin/env python


"""
R. Bayes

A demonstration of extracting information from the special virtual planes
"""


# pylint: disable = C0103, E1101, C0301, W0611, W0622

import ROOT
import math

import libMausCpp

def main(): 

    """ extract information from the special virtual planes
    from the maus_output.root file in the current directory """

    file = ROOT.TFile("maus_eval20141104.root","R")

    data = ROOT.MAUS.Data()
    tree = file.Get("Spill")
    tree.SetBranchAddress("data", data)

    Bfield = []
    zpos   = []
    hf = ROOT.TH2D("hfield", \
                   ";Position along Z axis (m); Field Magnitude", \
                   120,0,24,100,-5,5)
    hp = ROOT.TH2D("hp", \
                   ";Position along Z axis (m); Momentum (MeV/c)", \
                   120,0,24,100,0,400)
    hr = ROOT.TH2D("hr", \
                   ";Position along Z axis (m); Radial Position (m)", \
                   120,0,24,100,-.5,.5)
    maxsize = -999
    
    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        spill = data.GetSpill()

        if spill.GetDaqEventType() == "physics_event":
            for j in range(spill.GetMCEvents().size()):
                # if this is not the largest set of virtual hit go to the
                # next particle
                if spill.GetMCEvents()[j].GetPrimary().GetParticleId() != -13:
                    continue
                nvirthits = spill.GetMCEvents()[j].GetVirtualHits().size()
                for k in range(nvirthits):
                    xtmp = spill.GetMCEvents()[j] \
                           .GetVirtualHits()[k].GetPosition().x()
                    ytmp = spill.GetMCEvents()[j] \
                           .GetVirtualHits()[k].GetPosition().y()
                    r = math.sqrt(math.pow(xtmp, 2) + math.pow(ytmp, 2))
                    if r < 1000.:
                        hf.Fill( \
                            spill.GetMCEvents()[j] \
                            .GetVirtualHits()[k].GetPosition().z() / 1000, \
                            spill.GetMCEvents()[j] \
                            .GetVirtualHits()[k].GetBField().z() * 1000)
                        hp.Fill( \
                            spill.GetMCEvents()[j] \
                            .GetVirtualHits()[k].GetPosition().z() / 1000, \
                            spill.GetMCEvents()[j] \
                            .GetVirtualHits()[k].GetMomentum().mag())
                        hr.Fill( \
                            spill.GetMCEvents()[j] \
                            .GetVirtualHits()[k].GetPosition().z() / 1000, \
                            spill.GetMCEvents()[j] \
                            .GetVirtualHits()[k].GetPosition().x() / 1000.)
                           
                if maxsize < nvirthits:
                    maxsize = spill.GetMCEvents()[j].GetVirtualHits().size()
                    print maxsize
                    # reinitialize the Bfiled and zpos objects
                    Bfield = []
                    zpos   = []
                    for k in range(maxsize):
                        Bfield.append( \
                            spill.GetMCEvents()[j] \
                            .GetVirtualHits()[k].GetBField().z() \
                            * 1000)
                        zpos.append( \
                            spill.GetMCEvents()[j] \
                            .GetVirtualHits()[k].GetPosition().z() \
                            / 1000)

    g = ROOT.TGraph(maxsize)
    for i in range(maxsize):
        g.SetPoint(i, zpos[i], Bfield[i])
    g.SetMarkerColor(2)
    g.SetMarkerStyle(21)
    g.SetTitle("; Position on Z axis (m); Field Magnitude (Tesla)")
    
    c = ROOT.TCanvas()
    g.Draw('ap')
    c.Print("Bfield_v_z.eps")

    c1 = ROOT.TCanvas()
    hf.Draw("colz")
    c1.Print("Bfield_v_z_hist.eps")

    c3 = ROOT.TCanvas()
    c3.SetLogz()
    hp.SetStats(0)
    hp.Draw('colz')
    c3.Print("Momentum_v_z_hist.eps")
    
    c4 = ROOT.TCanvas()
    c4.SetLogz()
    hr.SetStats(0)
    hr.Draw('colz')
    c4.Print("BeamRadius_v_z_hist.eps")


if __name__ == "__main__":
    main()
    
    
