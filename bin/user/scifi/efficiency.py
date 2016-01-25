#!/usr/bin/env python

"""  Check tracker efficiency """

import sys
import ROOT
import libMausCpp #pylint: disable = W0611

#pylint: disable = R0912
#pylint: disable = R0914
#pylint: disable = R0915
#pylint: disable = C0103

def main(file_name):
    """ Main function which performs analysis & produces the plots """

    check_helical = False
    check_straight = True
    cut_on_tof = True
    cut_on_trackers = True

    # Load the ROOT file
    print "Loading ROOT file", file_name
    root_file = ROOT.TFile(file_name, "READ") # pylint: disable = E1101

    # Set up the data tree to be filled by ROOT IO
    print "Setting up data tree"
    data = ROOT.MAUS.Data() # pylint: disable = E1101
    tree = root_file.Get("Spill")
    tree.SetBranchAddress("data", data)

    num_total_events = 0
    num_12spoint_events = 0
    num_10spoint_events = 0
    num_5spoint_tkus_events = 0
    num_5spoint_tkds_events = 0

    num_10spoint_tracks = 0
    num_5spoint_tkus_tracks = 0
    num_5spoint_tkds_tracks = 0
    num_3to5spoint_tkus_tracks = 0
    num_3to5spoint_tkds_tracks = 0

    # Loop over spills
    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        spill = data.GetSpill()
        # Print some basic information about the spill
        print "Found spill number", spill.GetSpillNumber(),
        print "in run number", spill.GetRunNumber()
        if spill.GetDaqEventType() != "physics_event":
            continue
        for i in range(spill.GetReconEvents().size()):
            num_total_events += 1

            # Pull out tof data
            #------------------
            tof_evt = spill.GetReconEvents()[i].GetTOFEvent()
            tof1 = tof_evt.GetTOFEventSpacePoint().GetTOF1SpacePointArray()
            tof2 = tof_evt.GetTOFEventSpacePoint().GetTOF1SpacePointArray()

            bool_2tof_spoint_event = True
            if ((len(tof1) != 1) or (len(tof2) != 1)):
                bool_2tof_spoint_event = False
                if cut_on_tof:
                    continue

            # Pull out tracker data
            #----------------------

            # All spacepoint data
            tk_evt = spill.GetReconEvents()[i].GetSciFiEvent()
            tk_spoints = tk_evt.spacepoints()
            bool_10spoint_event = True
            bool_tkus_5spoint_event = True
            bool_tkds_5spoint_event = True
            for i in range(2):
                tracker = [sp for sp in tk_spoints if sp.get_tracker() == i]
                for j in range(1, 6):
                    station = [sp for sp in tracker if sp.get_station() == j]
                    if len(station) != 1:
                        bool_10spoint_event = False
                        if i == 0:
                            bool_tkus_5spoint_event = False
                        elif i == 1:
                            bool_tkds_5spoint_event = False
            
            if cut_on_trackers and (not bool_10spoint_event):
                continue
            
            if bool_2tof_spoint_event and bool_10spoint_event:
                num_12spoint_events += 1
            if bool_10spoint_event:
                num_10spoint_events += 1
            if bool_tkus_5spoint_event:
                num_5spoint_tkus_events += 1
            if bool_tkds_5spoint_event:
                num_5spoint_tkds_events += 1

            # Pat Rec seed data
            if (check_helical) and (not check_straight):
                prtracks = tk_evt.helicalprtracks()
                # print 'Looking at helical tracks'
            elif (not check_helical) and (check_straight):
                prtracks = tk_evt.straightprtracks()
                # print 'Looking at straight tracks'
            elif (check_helical) and (check_straight):
                prtracks = []
                prtracks.append(tk_evt.helicalprtracks())
                prtracks.append(tk_evt.straightprtracks())
                # print 'Looking at helical and straight tracks'
            else:
                print 'Both track options set, aborting'
                return

            # Is there at least 1 track present in either tracker
            bool_tkus_1track = False
            bool_tkds_1track = False
            if len(prtracks) < 1:
                continue

            num_tkus_tracks = 0
            num_tkds_tracks = 0
            # Require 1 and only 1 track in a tracker
            for trk in prtracks:
                if trk.get_tracker() == 0:
                    num_tkus_tracks += 1
                elif trk.get_tracker() == 1:
                    num_tkds_tracks += 1
            if num_tkus_tracks == 1:
                bool_tkus_1track = True
            if num_tkds_tracks == 1:
                bool_tkds_1track = True

            # If there were 5 spacepoints in a tracker and 1 track only was
            # found in it, increment the 3 to 5 spoint track counter
            if bool_tkus_5spoint_event and bool_tkus_1track:
                num_3to5spoint_tkus_tracks += 1
            if bool_tkds_5spoint_event and bool_tkds_1track:
                num_3to5spoint_tkds_tracks += 1

            # Now check the tracks found had 5 spoints, one from each station
            bool_tkus_5spoint_track = False
            bool_tkds_5spoint_track = False
            for trk in prtracks:
                if trk.get_tracker() == 0:
                    if (len(trk.get_spacepoints()) == 5) and bool_tkus_1track:
                        num_5spoint_tkus_tracks += 1
                        bool_tkus_5spoint_track = True
                elif trk.get_tracker() == 1:
                    if (len(trk.get_spacepoints()) == 5) and bool_tkds_1track:
                        num_5spoint_tkds_tracks += 1
                        bool_tkds_5spoint_track = True

                # Lastly see if we found one 5 spoint track in BOTH trackers
                if (len(prtracks) != 2):
                    continue
                if (bool_tkus_5spoint_track and bool_tkds_5spoint_track):
                    num_10spoint_tracks += 1


    print 'Total recon events: ' + str(num_total_events)
    print '12 spacepoint events: ' + str(num_12spoint_events)
    print '10 spacepoint events: ' + str(num_10spoint_events)
    print 'TkUS 5 spoint events: ' + str(num_5spoint_tkus_events)
    print 'TkDS 5 spoint events: ' + str(num_5spoint_tkds_events)
    print '10 spoint PR tracks: ' + str(num_10spoint_tracks)
    print 'TkUS 5 spoint PR tracks: ' + str(num_5spoint_tkus_tracks)
    print 'TkDS 5 spoint PR tracks: ' + str(num_5spoint_tkds_tracks)
    print 'TkUS 3 to 5 spoint PR tracks: ' + str(num_3to5spoint_tkus_tracks)
    print 'TkDS 3 to 5 spoint PR tracks: ' + str(num_3to5spoint_tkds_tracks)

    # Close the ROOT file
    print "Closing root file"
    root_file.Close()

if __name__ == "__main__":
    main(sys.argv[1])
