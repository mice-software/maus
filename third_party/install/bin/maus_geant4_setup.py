#!/usr/bin/env python
## This script auto-configures Geant4 instead of prompting the user.
#  and to do this, we have to replace a few lines in all the scripts
#  in the .config folder

import glob
import sys
import os
import string

def find_file(file_name_globbed, search_root):
    """
    """
    for (dirpath, dirnames, filenames) in os.walk(search_root):
        globs = glob.glob(os.path.join(dirpath, file_name_globbed))
        if len(globs) > 0:
            return globs
    return []

def main():
    print sys.argv
    assert len(sys.argv) == 4 or len(sys.argv) == 5

    print 'Looking in directory: ', sys.argv[1]
    print 'Assuming Geant4 is located here: ', sys.argv[2]
    print 'Assuming CLHEP is located here: ', sys.argv[3]
    extras = False
    if len(sys.argv) == 5 and sys.argv[4] == 'extras':
        extras = True
        print 'Found extras command'

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
          line = line.replace('/home/tunnell/usr', sys.argv[3])
          if extras:
              line = line.replace \
                ("g4vis_build_openglx_driver='n'", "g4vis_build_openglx_driver='y'")
              line = line.replace \
              ("g4vis_build_openglxm_driver='n'", "g4vis_build_openglxm_driver='y'")
              line = line.replace \
                                  ("g4vis_use_openglx='n'", "g4vis_use_openglx='y'")
              line = line.replace \
                                ("g4vis_use_openglxm='n'", "g4vis_use_openglxm='y'")
          t.write(line)
       s.close()
       t.close()

if __name__ == "__main__":
    main()

