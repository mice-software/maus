execfile('src/load.py', globals()) # don't remove this!

#
# Example script providing the comparaison of two LASER calibration runs   
#
# SV (viret@in2p3.fr) : 20/04/09 
#
# For more info, have a look at the LASER webpage:
# 
# http://atlas-tile-laser.web.cern.ch

#
# This work only with run numbers larger than 107000 (new Laser Calib format) 
#

# First give the run numbers you want to compare (they have to have same filter and amplitude settings)

run1 = 107809
run2 = 108573

u = Use([run1, run2])
l = ReadLaser()
s = SelectRegion('highgain') # Adapt this to your condition (Filter 6 == lowgain / Filter 8 == highgain)
x = Stability('Laser', run1, run2)

processors = [u, l, s, x]

Go(processors)


