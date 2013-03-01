"""make_canvas.py"""
#  Create TCanvas() according to ATLAS style guide
#

import ROOT

CANVAS_NUMBER = 0

def make_canvas():
    """Creates a plot according to the ATLAS style guide
    """
    global CANVAS_NUMBER # pylint:disable = W0603

    # Setup all the ROOT pretty-ing options
    ROOT.gROOT.SetStyle("Plain")# pylint:disable = E1101
    ROOT.gStyle.SetCanvasBorderMode(0)# pylint:disable = E1101
    ROOT.gStyle.SetPadBorderMode(0)# pylint:disable = E1101
    ROOT.gStyle.SetPadColor(0)# pylint:disable = E1101
    ROOT.gStyle.SetStatColor(0)# pylint:disable = E1101
    ROOT.gStyle.SetOptTitle(0)# pylint:disable = E1101
    ROOT.gStyle.SetLabelFont(42,"XYZ")# pylint:disable = E1101
    ROOT.gStyle.SetTextFont(42)# pylint:disable = E1101
    ROOT.gStyle.SetOptStat(111110)# pylint:disable = E1101
    ROOT.gStyle.SetTitleFont(42,"XYZ")# pylint:disable = E1101
    ROOT.gStyle.SetPadColor(0)# pylint:disable = E1101
    ROOT.gStyle.SetCanvasColor(0)# pylint:disable = E1101
    ROOT.gStyle.SetOptFit(0)# pylint:disable = E1101

    ROOT.gStyle.SetLegendBorderSize(0)# pylint:disable = E1101
    ROOT.gStyle.SetFillColor(0)# pylint:disable = E1101
    ROOT.gStyle.SetStatX(0.75)# pylint:disable = E1101
    ROOT.gStyle.SetStatY(0.8)# pylint:disable = E1101

    #hists
#    ROOT.gStyle.SetHistFillColor(1)
#    ROOT.gStyle.SetHistFillStyle(3001)

    tsize = 0.05
    ROOT.gStyle.SetTextSize(tsize)# pylint:disable = E1101
    ROOT.gStyle.SetLabelSize(tsize,"xyz")# pylint:disable = E1101
    ROOT.gStyle.SetTitleSize(tsize,"xyz")# pylint:disable = E1101

    ROOT.gStyle.SetNdivisions(509, "x")# pylint:disable = E1101
    ROOT.gStyle.SetNdivisions(305, "y")# pylint:disable = E1101

    #ROOT.gStyle.SetTitleOffset(1.03, "x")
    ROOT.gStyle.SetTitleOffset(1.46, "y")# pylint:disable = E1101

    ROOT.gStyle.SetPaperSize(20, 26)# pylint:disable = E1101
    ROOT.gStyle.SetPadTopMargin(0.05)# pylint:disable = E1101
    ROOT.gStyle.SetPadRightMargin(0.05)# pylint:disable = E1101
    ROOT.gStyle.SetPadBottomMargin(0.16)# pylint:disable = E1101
    ROOT.gStyle.SetPadLeftMargin(0.16)# pylint:disable = E1101
    ROOT.gStyle.SetFrameFillColor(0)# pylint:disable = E1101

    ROOT.gStyle.SetMarkerStyle(20)# pylint:disable = E1101
    ROOT.gStyle.SetMarkerSize(1.2)# pylint:disable = E1101
    ROOT.gStyle.SetHistLineWidth(2)# pylint:disable = E1101
    ROOT.gStyle.SetLineStyleString(2,"[12 12]")# pylint:disable = E1101
    ROOT.gStyle.SetPadTickX(1)# pylint:disable = E1101
    ROOT.gStyle.SetPadTickY(1)# pylint:disable = E1101

    canvas1 = ROOT.TCanvas("canvas1_n%d" % CANVAS_NUMBER,#pylint:disable=E1101
                           "canvas1", 0, 44, 800, 602)
    canvas1.Range(66.32653, -678.7319, 91.31868, 3835.625)

    canvas1.SetFrameBorderMode(0)
    canvas1.SetBorderSize(0)

    CANVAS_NUMBER += 1

    return canvas1
