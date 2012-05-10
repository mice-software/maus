#!/usr/bin/env python

"""
Example to load a ROOT file and make a histogram showing the beam profile at
TOF1
"""

import os
import subprocess

# basic PyROOT definitions
import ROOT 

# definitions of MAUS data structure for PyROOT
import libMausCpp #pylint: disable = W0611

def generate_some_data(outfile):
    """
    Run the offline reconstruction to make a data file
    """
    analysis = os.path.join\
                 (os.environ["MAUS_ROOT_DIR"], "bin", "analyze_data_offline.py")
    proc = subprocess.Popen([analysis, "-output_root_file_name", outfile])
    proc.wait()

def main():
    """
    Generates some data and then attempts to load it and make a simple histogram
    """
    # first off, we try to generate some data based on some default data file
    # let's generate some data by running the reconstruction...
    print "Generating some data"
    my_file_name = os.path.join\
             (os.environ["MAUS_ROOT_DIR"], "tmp", "example_load_root_file.root")
    generate_some_data(my_file_name)
    
    # now load the ROOT file
    print "Loading ROOT file", my_file_name
    root_file = ROOT.TFile(my_file_name, "READ") # pylint: disable = E1101

    # and set up the data tree to be filled by ROOT IO
    print "Setting up data tree"
    data = ROOT.MAUS.Data() # pylint: disable = E1101
    tree = root_file.Get("Spill")
    tree.SetBranchAddress("data", data)

    # We will try to load the data now into a analysable format and use it to
    # make a profile of the beam by plotting the number of digits (PMT pulses)
    # in the TOF. Don't ask which one is horizontal and which one is vertical...
    print "Getting some data"
    tof1_digits_0_hist = ROOT.TH1D("tof1 digits_0", # pylint: disable = E1101
                                   "tof1 digits for plane 0;Slab number",
                                   7, -0.5, 6.5)
    tof1_digits_1_hist = ROOT.TH1D("tof1 digits_1", # pylint: disable = E1101
                                   "tof1 digits for plane 1;Slab number",
                                    7, -0.5, 6.5)
    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        spill = data.GetSpill()
        # Print some basic information about the spill
        print "Found spill number", spill.GetSpillNumber(),
        print "in run number", spill.GetRunNumber(),
        # physics_events correspond to particle data. Everything else is DAQ
        # bureaucracy
        print "DAQ event type", spill.GetDaqEventType()
        if spill.GetDaqEventType() == "physics_event":
            # note PyROOT gives a segmentation fault if we try to call the STL
            # vector directly
            for i in range(spill.GetReconEventSize()):
                tof_event = spill.GetAReconEvent(i).GetTOFEvent()
                digits = tof_event.GetTOFEventDigit()
                for i in range(digits.GetTOF1DigitArraySize()):
                    tof1_digit = digits.GetTOF1DigitArrayElement(i)
                    if tof1_digit.GetPlane() == 0:
                        tof1_digits_0_hist.Fill(tof1_digit.GetSlab())
                    else:
                        tof1_digits_1_hist.Fill(tof1_digit.GetSlab())

    # draw the histograms and write to disk
    print "Writing histogram files"
    canvas_0 = ROOT.TCanvas("tof1_digits_0", # pylint: disable = E1101
                            "tof1_digits_0")
    tof1_digits_0_hist.Draw()
    canvas_0.Draw()
    canvas_0.Print('tof1_digits_0.png')
    canvas_1 = ROOT.TCanvas("tof1_digits_1", # pylint: disable = E1101
                            "tof1_digits_1")
    tof1_digits_1_hist.Draw()
    canvas_1.Draw()
    canvas_1.Print('tof1_digits_1.png')
    # wait for user to say we are done
    # note it is a feature of ROOT that if you close the file it will delete
    # existing histograms...
    print "Closing root file"
    root_file.Close()

if __name__ == "__main__":
    main()

