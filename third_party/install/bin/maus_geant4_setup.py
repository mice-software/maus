#!/usr/bin/env python
## This script auto-configures Geant4 instead of prompting the user.
#  and to do this, we have to replace a few lines in all the scripts
#  in the .config folder

import sys
import os
import string

print sys.argv
assert len(sys.argv) == 4

print 'Looking in directory: ', sys.argv[1]
print 'Assuming Geant4 is located here: ', sys.argv[2]
print 'Assuming CLHEP is located here: ', sys.argv[3]

files = os.listdir(sys.argv[1])
print "Found these files: ", files
files_to_modify = []

for file in files:
   if 'old' in file:
      files_to_modify.append(os.path.join(sys.argv[1], file))

print 'Will modify these files: ', files_to_modify

for filename in files_to_modify:
   s = open(filename,"r+")
   
   print filename.replace('.old', '')
   t = open(filename.replace('.old', ''), 'w')
   
   for line in s.readlines():
      line = line.replace('/home/tunnell/src/geant4.9.2.p01', sys.argv[2])
      t.write(line.replace('/home/tunnell/usr', sys.argv[3]))
   s.close()
   t.close()

