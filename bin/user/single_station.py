#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################


import io  #  generic python library for I/O
import sys # for command line arguments
import os
import MAUS

def run():
    """Analyze data from the MICE experiment

    This will read in and process data taken from the MICE experiment. It will
    eventually include things like cabling information, calibrations, and fits.
    """
# Set up data cards.
    data_cards_list = []
    # batch mode = runs ROOT in batch mode so that canvases are not displayed
    # 1 = True, Batch Mode
    # 0 = False, Interactive Mode
    # setting it to false/0 will cause canvases to pop up on screen and
    # will get refreshed every N spills set by the refresh_rate data
    # card.
    data_cards_list.append("root_batch_mode='%d'\n" % 0)
    data_cards_list.append("root_batch_mode='%d'\n" % 0)
    data_cards_list.append("Do_VLSB_Zero_Suppression=%s\n" % True)
    data_cards_list.append("Do_VLSB_Zero_Suppression_Threshold=%d\n" % 70)
    # setup = 'MayRun.dat'
    # data_cards_list.append("reconstruction_geometry_filename=%s\n" % Stage6.dat)
    data_path = '%s/src/map/MapCppSingleStationRecon/' % os.environ.get("MAUS_ROOT_DIR")
    # data_file = '3758.000'
    # data_cards_list.append("daq_data_path=%s\n" % data_path)
    data_file = '3944.000'
    # data_cards_list.append("daq_data_file=%s\n" % data_file)
    # refresh_rate = once in how many spills should canvases be updated
    # data_cards_list.append("refresh_rate='%d'\n" % 1)
    # Add auto-numbering to the image tags. If False then each
    # histogram output for successive spills will have the same tag
    # so there are no spill-specific histograms. This is the
    # recommended use for online reconstruction.
    # data_cards_list.append("histogram_auto_number=%s\n" % False)
    # Default image type is eps. For online use, use PNG.
    # data_cards_list.append("histogram_image_type=\"png\"\n")
    # Directory for images. Default: $MAUS_WEB_MEDIA_RAW if set
    # else the current directory is used.
    # Uncomment and change the following if you want to hard
    # code a different default path.
#    data_cards_list.append("image_directory='%s'\n" % os.getcwd())

    # Convert data_cards to string.
    data_cards = io.StringIO(unicode("".join(data_cards_list)))
    # Here you specify the path to the data and also the file you want to
    # analyze.

    my_input = MAUS.InputCppDAQOfflineData(data_path, data_file)

    # Create an empty array of mappers, then populate it
    # with the functionality you want to use.
    my_map = MAUS.MapPyGroup()
    my_map.append(MAUS.MapCppSingleStationRecon())

    reducer = MAUS.ReduceCppSingleStation()
    # reducer = MAUS.ReducePyDoNothing()
    # reducer = MAUS.ReduceCppTrackerErrorLog()

    output_file = open("test_unpack", 'w')  #  Uncompressed
    my_output = MAUS.OutputPyJSON(output_file)

    # The Go() drives all the components you pass in then put all the output
    # into a file called 'mausput'
    MAUS.Go(my_input, my_map, reducer, my_output, data_cards)


if __name__ == '__main__':
    if not os.environ.get("MAUS_ROOT_DIR"):
        raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')

    # Temporary values for the data path and data file

    data_path = '%s/src/map/MapCppSingleStationRecon/' % os.environ.get("MAUS_ROOT_DIR")
    data_file = '3944.000'
    
    # Check command line arguments for path to data and file name
    #if len(sys.argv) == 1:
     #   print
      #  print "WARNING: You did not pass in arguments.  I will pick"
      #  print "WARNING: a run for you..."
      #  print "WARNING:"
      #  print "WARNING: Usage: %s data_path data_file" % sys.argv[0]
      #  print
   # elif len(sys.argv) == 3:
    #    data_path = sys.argv[1]
   #     data_file = sys.argv[2]

    #print "Data path:", data_path
    #print "Data file", data_file
    #print

    run()
