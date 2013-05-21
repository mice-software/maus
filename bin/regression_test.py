#!/usr/bin/env python
#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
""" Run the regression tests """
import argparse
from regression import RegressionTest, Chi2Test, KolmogorovTest


if __name__ == '__main__':
    PARSER = argparse.ArgumentParser(
        description='Generate regression ROOT files '\
            'from reference and monitoried files' )
    PARSER.add_argument('REF_FILE', 
                        help='Reference file.')
    PARSER.add_argument('MON_FILE',
                        help='Monitored file.')
    PARSER.add_argument('-m', '--mfail', action='store_true', default=False,
                        help='Whether the testing should fail if a histo '\
                            'defined in the reference is non-existent in the '\
                            'monitored file.')
    PARSER.add_argument('-c', '--chi2', nargs=2, type=float,
                        metavar=('WARNING', 'FAILURE'), default=[0.1, 0.05],
                        help='The pValues to use for the WARNING and FAILURE '\
                            'limit of the Chi2 test')
    PARSER.add_argument('-k', '--ks', nargs=2, type=float,
                        metavar=('WARNING', 'FAILURE'), default=[0.1, 0.05],
                        help='The pValues to use for the WARNING and FAILURE '\
                            'limit of the KS test')
    PARSER.add_argument('-t', '--tests', nargs='*', choices=['chi2', 'ks'],
                        default=['chi2', 'ks'],
                        help='The tests to run, chose from chi2 and ks')

    
    ARGS = PARSER.parse_args()

    DC = []
    if 'chi2' in ARGS.tests:
        DC.append(Chi2Test(ARGS.chi2[0], ARGS.chi2[1]))
    if 'ks' in ARGS.tests:
        DC.append(KolmogorovTest(ARGS.chi2[0], ARGS.chi2[1]))

    RegressionTest( ref = ARGS.REF_FILE,
                    mon = ARGS.MON_FILE,
                    default_config  = DC,
                    fail_on_missing = ARGS.mfail )
