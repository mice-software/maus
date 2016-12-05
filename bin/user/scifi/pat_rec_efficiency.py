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
    eff_real = analysis.scifi_efficiency.PatternRecognitionEfficiencyReal()
    eff_real.check_helical = True
    eff_real.check_straight = True
    eff_real.cut_on_tof = False
    eff_real.cut_on_tof_time = False
    eff_real.cut_on_tracker_10spnt = False
    eff_real.use_mc_truth = False
    eff_real.tof_upper_cut = 50.0
    eff_real.tof_lower_cut = 27.0

    eff_mc = analysis.scifi_efficiency.PatternRecognitionEfficiencyMC(5, 5, 5, 5)
    eff_mc.n_hits_cut = 5

    eff_mc.run(args)

if __name__ == "__main__":
    args = sys.argv
    main(args)
