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
    eff = analysis.scifi_efficiency.PatternRecognitionEfficiency()
    eff.check_helical = False
    eff.check_straight = True
    eff.cut_on_tof = True
    eff.cut_on_tof_time = True
    eff.cut_on_trackers = True
    eff.use_mc_truth = False
    eff.tof_upper_cut = 50.0
    eff.tof_lower_cut = 27.0
    eff.id_frequency_cut = 0.5
    eff.run(args)

if __name__ == "__main__":
    args = sys.argv
    main(args)
