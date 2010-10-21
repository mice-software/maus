import os
import glob

# Useful constants
try:
    MAUS_ROOT_DIR=os.environ["MAUS_ROOT_DIR"]
except KeyError:
    MAUS_ROOT_DIR=""

MAUSCONFIGDIR = os.path.join(MAUS_ROOT_DIR,"config")
MAUSENVFILE = os.path.join(MAUSCONFIGDIR, "MAUS.scons")
ROOTARCH = os.popen("root-config --arch").read().strip()
print MAUSENVFILE
def testenv(envname):
    'Test for the presence of an environment var and if it set to 1.'
    return os.environ.has_key(envname) and os.environ[envname] == '1'

