import ROOT
import unittest

class RootPlotTestCase(unittest.TestCase):
    def test_geometries(self):
        h = ROOT.TH1F("h", "", 100, 0, 100)
        h.Draw() # if crashes, test fails.  tests ROOT environment

if __name__ == '__main__':
    unittest.main()
