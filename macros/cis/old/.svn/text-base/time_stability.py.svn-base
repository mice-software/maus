# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Make time stability plots (calib v.s. time) for various runs
#
# March 05, 2009
#
execfile('src/load.py', globals()) # don't remove this!

selected_region = ''

runs = '-1 month'
#runs = [102208, 102217, 102501, 102893, 103322, 103347]

runs2 = []
#runs2 = [72652 , 72653 , 72660 , 72661,  73305, 73314 , 73316 , 73742 , 73768 , 78017 , 78023, 78026, 78020 , 79121 , 79786 , 79789 , 79797 , 80487 , 80534 , 81342 , 81363 , 81601 , 81608 , 81627 , 83078 , 84402 , 84450 , 84826 , 85889 , 79259, 87920 , 88366 , 90179 , 90364 , 90555 , 90860 , 92972 , 93399 , 93884 , 94115 , 99025 , 99026 , 99944 , 100008 , 100073 , 100266 , 101184 , 101441 , 101784 , 101785 , 102009 , 79781, #] up to calib\
 #        69467, 69484, 69493, 69497, 69612, 69620, 70959, 70964, 71103, 71248, 71521, 71534, 71538, 71544, 72029, 72217, 72229, 72230, 72243]

for run in runs:
    runs2.append(run)

#runs2 = [72652, 73305, 72653, 72661, 79259, 78023, 79781, 78026]

u1 = Use(run=runs, useDateProg=True, runType='CIS', region=selected_region)
x1 = ReadCIS()

extra = None
extra = Use(run=runs2, runType='CIS',)

Go([u1, x1, ReadDB(), CISRecalibrateProcedure(), CISFlagProcedure(), extra,x1, TimeStability(all=True)])

