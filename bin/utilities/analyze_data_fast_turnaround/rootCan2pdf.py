"""
This utility takes a ROOT file containing TCanvas objects 
and outputs the canvas images to a PDF file.
These ROOT files are typically produced by a program 
e.g. from online reconstruction
Usage is rootCan2pdf.py <run-number>
output is <run-number>imageData.pdf

When used with -r <run-number>, 
  the output file will be Run<run-number>imageData.pdf
When used with -f <input-root-file>, 
  the output file will be Run0imageData.pdf

"""

# pylint:disable=C0103, E1101, W0621, W0702

import ROOT
import os
import sys
import getopt

ROOT.gROOT.SetBatch(ROOT.kTRUE)
run_number = 0

def imageify(inFile, outFile):
    """
    inFile is a root file with a list of canvases
    outFile is a pdf output containing canvas images
    """

    if not os.path.isfile(inFile):
        sys.exit(1)
    f1 = ROOT.TFile.Open(inFile,"r")
    if not len(f1.GetListOfKeys()):
        print "++ No keys in root file ", inFile
        sys.exit(1)

    c1 = ROOT.TCanvas()
    c1.Print("tmpFile(", "pdf")

    try:
        for key in f1.GetListOfKeys():
            cl = ROOT.gROOT.GetClass(key.GetClassName())
            if not cl.InheritsFrom("TCanvas"):
                continue
            c = key.ReadObj()
            c.Draw()
            imgFile = str(run_number) + "_" + key.GetName() + ".png"
            c.Print(imgFile)
            c.Print("tmpFile", "pdf")
    except:
        print "No keys in root file"
        sys.exit(2)
    c1.Print("tmpFile)", "pdf")
    os.rename("tmpFile", outFile)

if __name__ == "__main__":
    filename = None

    try:
        (opts, args) = \
          getopt.getopt(sys.argv[1:], "r:f:v", ["run_number=", "file="])
    except:
        print "Unrecognized option", sys.argv[1]
        print 'Usage: %s [-r run_number ] [-f filename]' % (sys.argv[0])
        sys.exit(1)
    for o, a in opts:
        if o in ('-r', '--run_number'):
            if a:
                run_number = int(a)
                inFile = "Run"+str(run_number)+"imageData.root"
        elif o in ('-f', '--file'):
            if a:
                inFile = "%s" % a
        else:
            print "Unrecognized option ", o
            sys.exit(2)
    outFile = "Run"+str(run_number)+"imageData.pdf"
    imageify(inFile, outFile)



