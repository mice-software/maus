# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# A very simple example that prints out CIS and laser
# constants for the last week of runs
#

execfile('src/load.py', globals()) # don't remove this!

runs = "-1 week"
selected_region = "EBC_m62_c37_highgain"

Go([ 
    Use(run=runs, region=selected_region, runType = "Las"), # Only use Laser runs
    ReadLaser(),                                            # Read in the laser constants
    Use(run=runs, region=selected_region, runType = "CIS"), # Only use Laser runs
    ReadCIS(),                                              # Read in the CIS constants
    ExampleCompareHarder(),                                 # Print consts with workers/Example/ExampleCompareHarder.py
    ])



