# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Show how Use works with a single run
#
exec(compile(open('src/load.py').read(), 'src/load.py', 'exec'), globals()) # don't remove this!

Go([
    Use(run=90555),     # Tell Use() to prepare TUCS to use run 90555
    Print(),            # Afterwards, dump the information from Use() for each detector region
    ]) 



