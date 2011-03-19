# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

import ROOT

CANVAS_NUMBER = 0

def canvas():
    global CANVAS_NUMBER

    # Setup all the ROOT pretty-ing options
    ROOT.gROOT.SetStyle("Plain")
    ROOT.gStyle.SetCanvasBorderMode(0)
    ROOT.gStyle.SetPadBorderMode(0)
    ROOT.gStyle.SetPadColor(0)
    ROOT.gStyle.SetStatColor(0)
    ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetLabelFont(42, "XYZ")
    ROOT.gStyle.SetTextFont(42)
    ROOT.gStyle.SetOptStat(111110)
    ROOT.gStyle.SetTitleFont(42, "XYZ")
    ROOT.gStyle.SetPadColor(0)
    ROOT.gStyle.SetCanvasColor(0)
    ROOT.gStyle.SetOptFit(0)

    ROOT.gStyle.SetLegendBorderSize(0)
    ROOT.gStyle.SetFillColor(0)
    ROOT.gStyle.SetStatX(0.75)
    ROOT.gStyle.SetStatY(0.8)

    tsize = 0.05
    ROOT.gStyle.SetTextSize(tsize)
    ROOT.gStyle.SetLabelSize(tsize, "xyz")
    ROOT.gStyle.SetTitleSize(tsize, "xyz")

    ROOT.gStyle.SetNdivisions(509, "x")
    ROOT.gStyle.SetNdivisions(305, "y")

    ROOT.gStyle.SetTitleOffset(1.03, "x")
    ROOT.gStyle.SetTitleOffset(1.46, "y")

    ROOT.gStyle.SetPaperSize(20, 26)
    ROOT.gStyle.SetPadTopMargin(0.05)
    ROOT.gStyle.SetPadRightMargin(0.05)
    ROOT.gStyle.SetPadBottomMargin(0.16)
    ROOT.gStyle.SetPadLeftMargin(0.16)
    ROOT.gStyle.SetFrameFillColor(0)

    ROOT.gStyle.SetMarkerStyle(20)
    ROOT.gStyle.SetMarkerSize(1.2)
    ROOT.gStyle.SetHistLineWidth(2)
    ROOT.gStyle.SetLineStyleString(2, "[12 12]")
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)

    my_canvas = ROOT.TCanvas("my_canvas_n%d" % CANVAS_NUMBER, \
                                 "my_canvas", 0, 44, 800, 602)
    my_canvas.Range(66.32653, -678.7319, 91.31868, 3835.625)
            
    my_canvas.SetFrameBorderMode(0)
    my_canvas.SetBorderSize(0)

    CANVAS_NUMBER += 1
                                                                
    return my_canvas
