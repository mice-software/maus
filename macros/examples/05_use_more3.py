# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Show how to run Use() with the last week of runs
#

execfile('src/load.py', globals()) # don't remove this!

# One nice feature of Use() is that you can select runs
# in a certain time range.  For instance, the last month
# or week.  Below is an example for the last week, and
# another commented out example for the last month.
runs = "-1 week"
#runs = "-1 month"

# The next thing you may want to do is set a run type
# for the runs you've selected above.  Use() does not
# require that you do this, but if you don't set a run
# type you'll get all Tile runs.  You can set runType
# to any of "CIS" for bigain Charge Injection scans,
# "Las" for laser runs, "Ped" for pedestal runs for
# noise studies, "MonoCIS" for monoCIS, though there
# is currently no calibration that uses it.
run_type = "CIS"

selected_region = "EBC_m62_c37_highgain"

Go([ 
    Use(run=runs, region=selected_region, runType = run_type),  
    Print(),
    ])



