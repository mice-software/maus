#!/usr/bin/env python

"""  Check tracker efficiency """

import sys
import os
import math
import ROOT
import libMausCpp #pylint: disable = W0611
import analysis.scifi_efficiency

def main(args):
    args.pop(0)
    # efficiency_real(args)
    efficiency_mc(args)
    raw_input("Press any key to finish...")

def efficiency_real(args):
    eff_real = analysis.scifi_efficiency.PatternRecognitionEfficiencyReal()
    eff_real.check_helical = True
    eff_real.check_straight = True
    eff_real.cut_on_tof = False
    eff_real.cut_on_tof_time = False
    eff_real.cut_on_tracker_10spnt = False
    eff_real.use_mc_truth = False
    eff_real.tof_upper_cut = 50.0
    eff_real.tof_lower_cut = 27.0
    eff_real.run(args)

def efficiency_mc(args):
    eff_mc = analysis.scifi_efficiency.PatternRecognitionEfficiencyMC()
    eff_mc.n_hits_cut = 5
    eff_mc.run(args)
    can = ROOT.TCanvas()
    can.Divide(2,2)
    can.cd(1)
    eff_mc.hpt_m_tku.Draw()
    can.cd(2)
    eff_mc.hpz_m_tku.Draw()
    can.cd(3)
    eff_mc.hpt_m_tkd.Draw()
    can.cd(4)
    eff_mc.hpz_m_tkd.Draw()
    return eff_mc, can

if __name__ == "__main__":
    args = sys.argv
    main(args)
