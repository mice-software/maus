#!/usr/bin/env python

"""  Check tracker efficiency """

import sys
import os
import math
import matplotlib.pyplot as plt
import numpy as np
import ROOT
import libMausCpp #pylint: disable = W0611
import analysis.scifi_efficiency

def main(args):
    args.pop(0)
    results = efficiency_real(args)
    # efficiency_mc(args)

    print ' '
    print 'Plotting results...'
    print ' '
    print results
    plot_data(results)


def efficiency_real(args):
    eff_real = analysis.scifi_efficiency.PatternRecognitionEfficiencyReal()
    eff_real.check_helical = True
    eff_real.check_straight = True
    eff_real.cut_on_tof = True
    eff_real.cut_on_tof_time = True
    eff_real.tof_upper_cut = 50.0
    eff_real.tof_lower_cut = 27.0
    eff_real.cut_on_tracker_10spnt = False
    eff_real.run(args)
    return eff_real.data

def efficiency_mc(args):
    eff_mc = analysis.scifi_efficiency.PatternRecognitionEfficiencyMC()
    eff_mc.cut_on_tof = True
    eff_mc.cut_on_tof_time = True
    eff_mc.tof_upper_cut = 50.0
    eff_mc.tof_lower_cut = 27.0
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
    can.Update()
    raw_input("Press any key to finish...")

    return eff_mc, can

def plot_data(data):
    """ Plot the efficiency results vs run number """
    runs = data['run_numbers']
    xmin = np.amin(runs) - 1
    xmax = np.amax(runs) + 1
    ymin = 0.0
    ymax = 1.0

    f, ax = plt.subplots()
    ax.plot(runs, data['tkus_5spoint'], 'r+', label='TkUS 5pt')
    ax.plot(runs, data['tkus_3to5spoint'], 'm+', label='TkUS 3-5pt')
    ax.plot(runs, data['tkds_5spoint'], 'bo', label='TkDS 5pt')
    ax.plot(runs, data['tkds_3to5spoint'], 'co', label='TkDS 3-5pt')

    ax.set_title('Pattern Recognition Efficiency vs Run Number')
    ax.set_xlabel('Run Number')
    ax.set_ylabel('Efficiency')
    ax.set_xbound(xmin, xmax)
    ax.set_ybound(ymin, ymax)
    ax.get_xaxis().get_major_formatter().set_useOffset(False)
    ax.legend(loc='lower left')
    plt.show()
    plt.savefig('efficiency.pdf')

if __name__ == "__main__":
    args = sys.argv
    main(args)
