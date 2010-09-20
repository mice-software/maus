# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Make the CIS public plot that shows the time stability of a single channel
# and also the detectorwide mean
#
# June 29, 2009
#

execfile('src/load.py', globals()) # don't remove this!

runs = [79781,79786,79789,79797,80487,80493,80494,80495,80512,80517,80529,80534,81342,81349,81355,81363,81601,81608,81627,81632,82395,82401,82405,82414,83033,83051,83066,83078,84402,84408,84432,84450,84826,85889,85895,85903,85920,85926,86523,87528,87920,88366,88843,89767,90170,90179,90364,90555,90860,91315,91567,91837,92016,92416,92971,92972]

u = Use(run=runs, runType='CIS')
x = ReadCIS()


# After you've run the program once, you can comment out this line since the data
# will be stored to /tmp/ and readout in the next Go()
Go([u,x,ReadDB(),CISFlagProcedure(dbCheck=False),SaveTree()])

Go([LoadTree(), 
    TimeVSMeanCalib(example="LBA_m60_c17"),\
    TimeVSMeanCalib(example="LBA_m30_c20"),\
    ])
   

