"""
Query the CDB
Options are 
-- get the beamline and cooling channel information for a given run
-- or find runs which have cooling channel magnet(s) on
"""

# pylint: disable=C0103, C0111, R0902

import sys
import os
import argparse
import pprint
from cdb import Beamline
from cdb import CoolingChannel
_R_SERVER = 'http://cdb.mice.rl.ac.uk'

DESCRIPTION = """
This script queries the CDB by
and either:
    returns the beamline and cooling channel settings for a given run
or 
    returns a list of runs with a specific cooling channel or beamline setting
"""

def arg_parser():
    parser = argparse.ArgumentParser(description=DESCRIPTION, \
                           formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--ssu-on', dest='ssu_on', \
                        help='Returns runs where SSU-C > 10 amps',
                        action='store_true', default=False)
    parser.add_argument('--ssd-on', dest='ssd_on', \
                        help='Returns runs were SSD-C > 10 amps',
                        action='store_true', default=False)
    parser.add_argument('--both-ss', dest='both_ss', \
                        help='Returns runs were SSU-C and SSD-C > 10 amps',
                        action='store_true', default=False)
    parser.add_argument('--run-number', dest='run_number', type=int,
                        help='Run Number',
                        default=0)
    return parser

def main(argv):
    args = arg_parser()
    args_in = args.parse_args(argv)
    mycdb = QueryCDB(args_in)
    mycdb.process_request()


class QueryCDB:
    """
    Handles CDB requuests
    """

    def __init__(self, args_in):
        self._BL = Beamline(_R_SERVER)
        # print self._BL
        self._CC = CoolingChannel(_R_SERVER)
        self.run_number = args_in.run_number
        self.ssu_on = args_in.ssu_on
        self.ssd_on = args_in.ssd_on
        self.both_ss = args_in.both_ss
        self.pretty = pprint.PrettyPrinter(indent=4)
        self.oldstd = sys.stdout
        self.pout = open(os.devnull, 'w')

    def process_request(self):
        # if run number is specified, then just get the info for the run
        if self.run_number > 0:
            self.get_beamline_for_run()
            self.get_cc_for_run()
        # if cooling channel match is requested, then find runs
        if self.ssu_on or self.ssd_on or self.both_ss:
            self.get_runs_for_cc()


    def get_beamline_for_run(self):
        print '===== Beamline for Run# ', self.run_number
        blrun = self._BL.get_beamline_for_run(self.run_number)
        self.pretty.pprint(blrun)

    def get_cc_for_run(self):
        print '===== Cooling Channel for Run# ', self.run_number
        sys.stdout = self.pout
        ccrun = self._CC.get_coolingchannel_for_run(self.run_number)
        sys.stdout = None
        self.pretty.pprint(ccrun)

    def get_runs_for_cc(self):
        runlist = sorted(self._BL.get_run_numbers())
        #### No cooling channel before run ~7100, so don't bother
        for run in runlist:
            if run < 7400:
                continue
            sys.stdout = self.pout
            ccrun = self._CC.get_coolingchannel_for_run(run)
            sys.stdout = self.oldstd
            for mag in ccrun:
                if mag['name'] == 'SSU':
                    ssu_coils = mag['coils']
                    for coil in ssu_coils:
                        if coil['name'] == 'SSU-C':
                            ssu_c_current = coil['iset']
                if mag['name'] == 'SSD':
                    ssu_coils = mag['coils']
                    for coil in ssu_coils:
                        if coil['name'] == 'SSD-C':
                            ssd_c_current = coil['iset']
            if self.ssu_on and ssu_c_current > 10.0:
                print '++++ SSU ON. Run# ', run
                self.pretty.pprint(ccrun)
            if self.ssd_on and ssd_c_current > 10.0:
                print '++++ SSD ON. Run# ', run
                self.pretty.pprint(ccrun)
            if self.both_ss and ssd_c_current > 10.0 and ssu_c_current > 10.0:
                print '++++ SSU and SSD ON. Run# ', run
                self.pretty.pprint(ccrun)


if __name__ == "__main__":
    main(sys.argv[1:])
