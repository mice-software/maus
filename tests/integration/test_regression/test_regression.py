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

"""
Run the regression tests for reducers in root_data
"""

import unittest
import ROOT
from pprint import pprint
from regression import RegressionTest, AggregateRegressionTests, Test
try:
    import InputCppDAQOnlineData
    NO_ONLINE = False
except ImportError:
    NO_ONLINE = True

# pylint: disable=R0904, C0301,E1101,C0103,W0612,W0613

@unittest.skipIf(NO_ONLINE, "Online libraries not available")
class ReducerRegressionTestCase(unittest.TestCase):
    """
    Run the online reconstruction over a file; compare output root files with
    those in data folder
    """
    def setUp(self):
        """Does nothing"""
        pass
            
    def test_reducers(self):
        """
        An example unit test.
        """
        root_file = ROOT.TFile('test_mon.root', 'RECREATE')
        h1 = ROOT.TH1D("test1", "test1", 100, 0, 20)
        h2 = ROOT.TH1D("test2", "test2", 100, 0, 20)
        h3 = ROOT.TH1D("test3", "test3", 100, 0, 20)
        r  = ROOT.TRandom3(7765234724234)

        for i in range(10000):
            h1.Fill(r.Gaus(10))
            h2.Fill(r.Poisson(10))
            h3.Fill(r.Gaus(12))

        h1.Write()
        h2.Write()
        h3.Write()
        root_file.Close()

        pprint( RegressionTest('test_mon.root',
                               'test_ref.root',
                               {'test3':[Test("MyTest",myTest,0.5,0.1)]},
                               )
                )
        
        self.assertTrue(AggregateRegressionTests('test_mon.root',
                                                 'test_ref.root',
                                                 {'test3':[]},
                                                 printer = None
                                                 ),
                        'Regression tests failed.'
                        )
        
if __name__ == '__main__':
    unittest.main()
