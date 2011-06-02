import ROOT
import unittest
import tempfile

class RootPlotTestCase(unittest.TestCase):
    def test_geometries(self):
        c1 = ROOT.TCanvas()
        h = ROOT.TH1F("h", "", 100, 0, 100)
        try:
            print 'hi'
            h.Draw() # if crashes, test fails.  tests ROOT environment
            c1.Print(tempfile.mkstemp()[1])
        except:
            print 'bad'
            self.fail("ROOT raises exception")

if __name__ == '__main__':
    unittest.main()
