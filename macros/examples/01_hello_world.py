# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# This macro just prints "hello world"
#
exec(compile(open('src/load.py').read(), 'src/load.py', 'exec'), globals()) # don't remove this!

Go([ 
    PrintHelloWorld(),    # Run the "hello world" worker at workers/Examples/PrintHellowWorld.py
    ])


