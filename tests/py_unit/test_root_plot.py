"""test_root_plot.py"""
import ROOT
import unittest
import tempfile

class RootPlotTestCase(unittest.TestCase):#pylint: disable =R0904
    """RootPlotTestCase"""
    def test_geometries(self):
        """test_geometries"""
        tc1 = ROOT.TCanvas()#pylint: disable =E1101
        th1 = ROOT.TH1F("h", "", 100, 0, 100)#pylint: disable =E1101
        try:
            print 'hi'
            th1.Draw() # if crashes, test fails.  tests ROOT environment
            tc1.Print(tempfile.mkstemp()[1])
        except: #pylint: disable =W0702
            print 'bad'
            self.fail("ROOT raises exception")

if __name__ == '__main__':
    unittest.main()
