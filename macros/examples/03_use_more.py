# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Show how Use works with a single run, but only looking 
#
exec(compile(open('src/load.py').read(), 'src/load.py', 'exec'), globals()) # don't remove this!

# Here we set the string variable 'selected_region' to whatever
# region we want.  You have to use the naming convention of TUCS
# though.  So if you wanted partition EBC, module 62, channel 37,
# highgain, you would call the region EBC_m62_c37_highgain.
# Similarly, if you want to only look at a PMT, like PMT 2 in
# the same module, you could call the region EBC_m62_p02_highgain.
# So for this example, we're going to set a variable to equal this
# region name, then the "Use" worker below will be passed this
# variable as an argument.
selected_region = "EBC_m62_c37_highgain"

Go([ 
    Use(run=90555, region=selected_region),  # Tell Use() to only use our selected region
    Print(),
    ]) 



