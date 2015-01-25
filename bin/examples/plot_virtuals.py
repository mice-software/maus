#!/usr/bin/env python


"""
R. Bayes

An example of extracting information from the special virtual planes
"""

# pylint: disable = E1101, C0103, C0301, W0611
import os
import sys
import ROOT
import math
import subprocess
import libMausCpp

def generate_simulation(outfile):
    """
    Run the simulation to make a data file for consideration
    """
    analysis = os.path.join\
               (os.environ["MAUS_ROOT_DIR"],"bin","simulate_mice.py")
    proc = subprocess.Popen(['python', analysis,\
                             '--output_root_file_name', outfile,
                             '--simulation_geometry_file', 'Stage4.dat'])
    proc.wait()

def main(args):

    """ extract information from the special virtual planes
    from the maus_output.root file in the current directory """
    
    Bfield = []
    zpos   = []
    h = ROOT.TH1D("hrate", \
                  ";Position along Z axis (m);Particles at Virtual Plane", \
                  240,0,24)
    hf = ROOT.TH2D("hfield", \
                   ";Position along Z axis (m); Field Magnitude", \
                   240,0,24,100,-5,5)
    hp = ROOT.TH2D("hp", \
                   ";Position along Z axis (m); Momentum (MeV/c)", \
                   240,0,24,100,0,400)
    hp200 = ROOT.TH2D("hp200", \
                      ";Position along Z axis (m); Momentum (MeV/c)", \
                      240,0,24,100,0,400)
    hr = ROOT.TH2D("hr", \
                   ";Position along Z axis (m); Radial Position (m)", \
                   240,0,24,100,0.0, 0.5)
    hbt = ROOT.TH2D("hbt", \
                    ";Position along Z axis (m); Transverse #beta", \
                    240,0,24,100,0, 0.5)
    maxsize = -999

    for fp in args:
        
        file = ROOT.TFile(str(fp),"R")
        if file.IsZombie():
            continue
        data = ROOT.MAUS.Data()
        tree = file.Get("Spill")
        if not tree:
            continue
        tree.SetBranchAddress("data", data) 
        print "Evaluating "+str(fp)
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
                        h.Fill(spill.GetMCEvents()[j] \
                               .GetVirtualHits()[k].GetPosition().z() / 1000)
                        r = math.sqrt((spill.GetMCEvents()[j] \
                                       .GetVirtualHits()[k].GetPosition().x())**2 +
                                      (spill.GetMCEvents()[j] \
                                       .GetVirtualHits()[k].GetPosition().y())**2 )
                        if r < 200.:
                            hf.Fill( \
                                spill.GetMCEvents()[j] \
                                .GetVirtualHits()[k].GetPosition().z() / 1000,
                                spill.GetMCEvents()[j] \
                                .GetVirtualHits()[k].GetBField().z() * 1000)
                            hp200.Fill( \
                                spill.GetMCEvents()[j] \
                                .GetVirtualHits()[k].GetPosition().z() / 1000,
                                spill.GetMCEvents()[j] \
                                .GetVirtualHits()[k].GetMomentum().mag())
                        hp.Fill(spill.GetMCEvents()[j] \
                                .GetVirtualHits()[k].GetPosition().z() / 1000,
                                spill.GetMCEvents()[j] \
                                .GetVirtualHits()[k].GetMomentum().mag())
                        hr.Fill(spill.GetMCEvents()[j]\
                                .GetVirtualHits()[k].GetPosition().z() / 1000,
                                math.sqrt((spill.GetMCEvents()[j] \
                                           .GetVirtualHits()[k].GetPosition().x())**2 +
                                          (spill.GetMCEvents()[j] \
                                           .GetVirtualHits()[k].GetPosition().y())**2 )/ 1000.)
                        hbt.Fill(spill.GetMCEvents()[j] \
                                 .GetVirtualHits()[k].GetPosition().z() / 1000,
                                 math.sqrt((spill.GetMCEvents()[j] \
                                            .GetVirtualHits()[k].GetMomentum().x())**2 +
                                           (spill.GetMCEvents()[j] \
                                            .GetVirtualHits()[k].GetMomentum().y())**2 )/
                                 math.sqrt((spill.GetMCEvents()[j] \
                                            .GetVirtualHits()[k].GetMomentum().mag())**2 \
                                           + 105.65**2) )
                    if maxsize < nvirthits:
                        maxsize = spill.GetMCEvents()[j].GetVirtualHits().size()
                        print maxsize
                        # reinitialize the Bfiled and zpos objects
                        Bfield = []
                        zpos   = []
                        for k in range(maxsize):
                            Bfield.append(spill.GetMCEvents()[j] \
                                          .GetVirtualHits()[k].GetBField().z() * 1000)
                            zpos.append(spill.GetMCEvents()[j] \
                                        .GetVirtualHits()[k].GetPosition().z() / 1000)

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
    hp.Draw('colz')
    c3.Print("Momentum_v_z_hist.eps")

    c3_1 = ROOT.TCanvas()
    hp_px = hp.ProjectionX()
    hp_px.Draw()
    c3_1.Print("Momentum_v_z_projx.eps")

    c3_2 = ROOT.TCanvas()
    hp_pfx = hp.ProfileX()
    hp_pfx.Draw()
    c3_2.Print("Momentum_v_z_prflx.eps")

    c3_3 = ROOT.TCanvas()
    c3_3.SetLogz()
    hp200.Draw('colz')
    c3_3.Print("Momentum_v_z_hist_lt200mm.eps")

    c3_4 = ROOT.TCanvas()
    hp200_px = hp200.ProjectionX()
    hp200_px.Draw()
    c3_4.Print("Momentum_v_z_projx_lt200mm.eps")

    c3_5 = ROOT.TCanvas()
    hp200_pfx = hp200.ProfileX()
    hp200_pfx.Draw()
    c3_5.Print("Momentum_v_z_prflx_lt200mm.eps")

    
    c4 = ROOT.TCanvas()
    c4.SetLogz()
    hr.Draw('colz')
    c4.Print("BeamRadius_v_z_hist.eps")
    
    c5 = ROOT.TCanvas()
    c5.SetLogz()
    hbt.Draw('colz')
    c5.Print("TransverseBeta_v_z_hist.eps")

    
    c6 = ROOT.TCanvas()
    c6.SetLogz()
    h.Draw()
    c6.Print("MuonRate_v_z_hist.eps")


if __name__ == "__main__":
    if len(sys.argv) > 1:
        main(sys.argv[1:])
    else:
        outfile = os.path.join\
                  (os.environ['MAUS_ROOT_DIR'],
                   'tmp','example_simulation_file.root')
        generate_simulation(outfile):
        main([outfile])
    
