from src.GenericWorker import *

class ExampleCompareHarder(GenericWorker):
    "A harder example worker for showing how to compare constants"

    # This function gets called as TUCS is initializing as it constructs
    # all of the workers it needs to process data.  This happens before
    # any other workers get sent data.
    def __init__(self):
        print "Init: ExampleCompareHarder() worker has been constructed and is waiting to go!"

    # This function gets called immediatly before this worker gets sent data,
    # so this function can be used to zero counters if you want to rerun the same
    # worker many times.  After this function, regions start being passed to
    # ProcessRegion()
    def ProcessStart(self):
        print "Start: ExampleCompareHarder() worker is about to be sent data, so is zeroing it's counters"
        # Store these two variables to compute laser average
        self.laser_sum = 0
        self.laser_n   = 0

        # Store these two variables to compute CIS average
        self.cis_sum   = 0
        self.cis_n     = 0

    # This function gets called after all of the regions in the detector have
    # been sent to ProcessRegion().  It is the last function called in this class.
    def ProcessStop(self):
        print "Stop: ExampleCompareHarder() worker has finished being sent data, and is going to do it's analysis"

        if self.laser_n == 0:
            print "Didn't find laser events!"
            return

        if self.cis_n == 0:
            print "Didn't find CIS events!"
            return

        # Compute the mean CIS and laser response, then divide them
        print "For all the regions analyzed the average laser response divided by the average CIS response is:"
        print "\tLaser/CIS = %f" % ((self.laser_sum/self.laser_n)/(self.cis_sum/self.cis_n))
        

    def ProcessRegion(self, region):
        if region.GetEvents() == set():
            return
        
        print "Constants for %s" % region.GetHash()
        
        for event in region.GetEvents():
            if event.runType == 'Laser':
                if event.data.has_key("calib_const"):
                    print "\tLaser:",event.data['calib_const']

                    # Keep track of sum of constants and number of constants
                    # so we can compute an average in ProcessStop()   
                    self.laser_sum += event.data['calib_const']
                    self.laser_n   += 1

        for event in region.GetEvents():
            if event.runType == 'CIS':
                if event.data.has_key("calibration"):
                    print "\tCIS:",event.data['calibration']

                    # Keep track of sum of constants and number of constants
                    # so we can compute an average in ProcessStop()
                    self.cis_sum += event.data['calibration']
                    self.cis_n   += 1
                                        
        
                    
                                       
                
