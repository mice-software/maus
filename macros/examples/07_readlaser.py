# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# This example shows you how to read in some laser events
#

exec(compile(open('src/load.py').read(), 'src/load.py', 'exec'), globals()) # don't remove this!

runs = "-1 week"
run_type = "Las"  # Change the run type from the previous example to "Las" for laser
selected_region = "EBC_m62_c37_highgain"

Go([ 
    Use(run=runs, region=selected_region, runType = run_type),
    Print(),
    ReadLaser(),  # Then run the ReadLaser() instead of ReadCIS()
    Print(),
    ])



