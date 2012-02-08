"""
Simple script to excercise the optics application and automatically test output
transverse beta functions.
"""

import os
import unittest
import xboa.Common as Common
import subprocess

TEST_NAMES = ['mapping', 'optimiser'] #, 'rf', '6d', 'polyfit']
TARGET_BETA = {'mapping':0.655994338, 'optimiser':0.652749967}

def run_optics(test):
    """
    Runs optics application and produces some output
    """
    os.chdir(os.getenv('MAUS_ROOT_DIR')+'/tests/integration/test_optics')
    run_command = './optics files/cards.'+test+' >& log.'+test
    print run_command
    proc = subprocess.Popen(['./optics', 'files/cards.mapping'])
    proc.wait()
    print proc.returncode

def get_data(test):
    """
    Extracts data from optics output
    """
    z_out = []
    beta = []
    fin = open('envelope-'+test+'.txt')
    line = ''
    for index in range(8): # pylint: disable=W0612
        line = fin.readline()
    while line != '':
        words = line.split()
        z_out.append(float(words[6]))
        beta.append(float(words[11])/1000.)
        for i in range(7): # pylint: disable=W0612
            line = fin.readline()
    return (z_out, beta)

def plot_data(test, z_out, beta, plot_dir):
    """
    Plots optics data
    """
    canvas = Common.make_root_canvas(test)
    test_heading = test
    (hist, graph) = Common.make_root_graph(test_heading, z_out, 'z [m]', \
                                           beta, 'beta [m]')
    hist.Draw()
    graph.Draw()
    canvas.Update()
    plot_dir = os.path.join(plot_dir, 'optics')
    try:
        os.mkdir(plot_dir)
    except OSError:
        pass # dir already exists
    canvas.Print(os.path.join(plot_dir, test+'_optics.png'))

class TestOptics(unittest.TestCase): # pylint: disable=R0904
    """
    Run optics for a few options
    """

    def test_optics(self):
        """
        Check that we can calculate optics beta function correctly; plot output
        beta function.
        """
        for test in TEST_NAMES:
            run_optics(test)
            (z_out, beta) = get_data(test)
            if 'MAUS_TEST_PLOT_DIR' in os.environ:
                plot_data(test, z_out, beta, os.environ['MAUS_TEST_PLOT_DIR'])
            #Common.clear_root()
            self.assertAlmostEqual(TARGET_BETA[test], beta[-1], 3)

if __name__ == '__main__':
    unittest.main()

