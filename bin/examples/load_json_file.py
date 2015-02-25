#!/usr/bin/env python

"""
Example to load a json file and make a histogram showing the beam profile at
TOF1
"""

import os
import subprocess

# basic json definitions
import json 

# root for plotting
import ROOT

def generate_some_data(outfile):
    """
    Run the offline reconstruction to make a data file.

    Note the "offline" reconstruction needs an internet connection to access
    configuration and calibration data.
    """
    # generate a root file from MAUS output
    root_tmp = os.path.expandvars("$MAUS_ROOT_DIR/tmp/load_json_file_temp.root")
    analysis = os.path.join\
                 (os.environ["MAUS_ROOT_DIR"], "bin", "analyze_data_offline.py")
    proc = subprocess.Popen([analysis, "-output_root_file_name", root_tmp])
    proc.wait()

    # convert to json
    converter = os.path.join\
            (os.environ["MAUS_ROOT_DIR"], "bin", "utilities", "root_to_json.py")
    proc = subprocess.Popen([converter,
                             "-input_root_file_name", root_tmp,
                             "-output_json_file_name", outfile])
    proc.wait()

    # advanced users might consider making a script that natively writes out to
    # json by changing the Output module... saves doing things as a two stage
    # analysis


def main():
    """
    Generates some data and then attempts to load it and make a simple histogram
    """
    # first off, we try to generate some data based on some default data file
    # let's generate some data by running the reconstruction...
    print "Generating some data"
    my_file_name = os.path.join\
             (os.environ["MAUS_ROOT_DIR"], "tmp", "example_load_json_file.json")
    generate_some_data(my_file_name)
    
    # now open the json file
    print "Loading json file", my_file_name
    json_file = open(my_file_name)

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
    # get all events - nb: this is python preferred way of extracting a list
    # from another list
    events = [json.loads(line) for line in json_file.readlines()]
    # make a list of events with type 'Spill' (i.e. output from DAQ)
    spills = [event for event in events if event['maus_event_type'] == 'Spill']
    for spill in spills:
        # Print some basic information about the spill
        print "Found spill number", spill["spill_number"],
        print "in run number", spill["run_number"],
        # physics_events correspond to particle data. Everything else is DAQ
        # bureaucracy
        print "DAQ event type", spill["daq_event_type"]
        if spill["daq_event_type"] == "physics_event":
            # EMR adds extra events to the recon_event branch
            # These extra events do not have anything other than emr_event
            # i.e. no tof_event scifi_event etc
            for event in spill["recon_events"]:
                if "tof_event" not in event:
                    continue
                tof_event = event["tof_event"]
                digits = tof_event["tof_digits"]
                for tof1_digit in digits["tof1"]:
                    if tof1_digit["plane"] == 0:
                        tof1_digits_0_hist.Fill(tof1_digit["slab"])
                    else:
                        tof1_digits_1_hist.Fill(tof1_digit["slab"])

    # draw the histograms and write to disk
    print "Writing histogram files"
    canvas_0 = ROOT.TCanvas("tof1_digits_0", # pylint: disable = E1101
                            "tof1_digits_0")
    tof1_digits_0_hist.Draw()
    canvas_0.Draw()
    canvas_0.Print('tof1_digits_0_load_json_file.root')
    canvas_0.Print('tof1_digits_0_load_json_file.png')
    canvas_1 = ROOT.TCanvas("tof1_digits_1", # pylint: disable = E1101
                            "tof1_digits_1")
    tof1_digits_1_hist.Draw()
    canvas_1.Draw()
    canvas_1.Print('tof1_digits_1_load_json_file.root')
    canvas_1.Print('tof1_digits_1_load_json_file.png')

    # finally close the json file
    print "Closing json file"
    json_file.close()

    # note also that the utility bin/utilities/json_browser.py is useful for
    # reading the output json files...

if __name__ == "__main__":
    main()

