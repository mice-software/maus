from src.GenericWorker import *

# Define your worker class here, and be sure to change the name
class ExampleCompare(GenericWorker):
    "An example worker for showing how to compare constants"

    # This function gets called for every region (ie. channel, drawer, etc.) in
    # TileCal.  Define what you want to do with constants for a certain region
    # here.
    def ProcessRegion(self, region):
        # If there are no events for this region, do nothing
        if region.GetEvents() == set():
            return
        
        # Print out the region so the user knows.  The function GetHash()
        # returns the unique name of the region in the form:
        #
        #     TILECAL_LBA_m32_c03_highgain
        #
        # where this is LBA32 channel 3, highgain. One can also do:
        # region.GetHash(1) if one wants the PMT number instead.
        print "Constants for %s" % region.GetHash()
        
        # Loop over all events associated with this region
        for event in region.GetEvents():
            # and only look at laser runs
            if event.runType == 'Laser':
                # Make sure the calibration constant exists to avoid crashes
                if event.data.has_key("calib_const"):
                    # then print the calibration constants
                    print "\tLaser:",event.data['calib_const']

        for event in region.GetEvents():
            if event.runType == 'CIS':
                if event.data.has_key("calibration"):
                    print "\tCIS:",event.data['calibration']

        
                    
                                       
                
