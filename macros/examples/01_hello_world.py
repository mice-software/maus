# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# This macro just prints "hello world"
#
execfile('src/load.py', globals()) # don't remove this!

Go([ 
    PrintHelloWorld(),    # Run the "hello world" worker at workers/Examples/PrintHellowWorld.py
    ])


