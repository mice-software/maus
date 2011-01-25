# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# This macro literally does nothing
#

# This line sets up TUCS and should be at the top of each macro
exec(compile(open('src/load.py').read(), 'src/load.py', 'exec'), globals()) # don't remove this!

Go([       # Put your workers as a comma seperated list after this line
    None,  # "None" means do nothing
    ])     # end's the worker list


