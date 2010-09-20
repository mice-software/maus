# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Show how Use works with a list of runs
#
execfile('src/load.py', globals()) # don't remove this!

# Instead of setting a run number for Use(), you can also
# set a list of runs as follows:
runs = [90555, 90666]

selected_region = "EBC_m62_c37_highgain"

Go([ 
    Use(run=runs, region=selected_region),  # Tell Use() to use our "runs" variable to know which runs to use
    Print(),
    ])



