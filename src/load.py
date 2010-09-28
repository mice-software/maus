"""@package core
Documentation for this module.

More details.
"""
# This loads everything from features and calibrations
import os, sys, string, fnmatch

# Hide command line arguments from ROOT: part 1/2 
_argv = sys.argv        # Store command line args in a temp variable
sys.argv = sys.argv[:1] # then remove them from what ROOT sees. The macro uses commandline args

try:
    import ROOT  # pyroot
    ROOT.gROOT.SetBatch()   # Run in batch mode since we don't need X
    import PyCintex # allows for reflex dictionaries
except ImportError:
    print "FATAL ERROR: You need to setup pyROOT before proceeding"
    sys.exit(-1)

# Setup the PythonPath to include the Tucs modules and MySQL modules
sys.path.insert(0, '.') # TODO add appropriate path?

# I do a import * here to make macros easier to understand
from src.go import *

# Hide command line arguments from ROOT: part 2/2
sys.argv = _argv # recover command line arguments
del _argv        # and get rid of our temporary variable

# Load all python files in the directory passed
def execdir(dir, scope):
    for path, dirs, files in os.walk(dir):
        for py in [os.path.abspath(os.path.join(path, filename)) for filename in files if fnmatch.fnmatch(filename, '*.py')]:
            # this file is used by python, but not of interest to us 
            if '__init__.py' in py or '#' in py or '~' in py:
                continue

            execfile(py, scope)

execdir('workers', globals())
