import os
import glob

mice_root = '/home/tunnell/mice/MICE'

files = []
files += glob.glob('*/*.cc')
files += glob.glob('*/*/*.cc')
files += glob.glob('*/*/*/*.cc')
files += glob.glob('*/*.hh')
files += glob.glob('*/*/*.hh')
files += glob.glob('*/*/*/*.hh')

old_files = []
old_files += glob.glob('%s/*/*/*/*.hh' % mice_root)
old_files += glob.glob('%s/*/*/*.hh' % mice_root)
old_files += glob.glob('%s/*/*.hh' % mice_root)
old_files += glob.glob('%s/*.hh' % mice_root)
old_files += glob.glob('%s/*/*/*/*.cc' % mice_root)
old_files += glob.glob('%s/*/*/*.cc' % mice_root)
old_files += glob.glob('%s/*/*.cc' % mice_root)
old_files += glob.glob('%s/*.cc' % mice_root)
print old_files

old_files_dict = {}

for file in old_files:
    key = file.split('/')[-1]
    old_files_dict[key] = file

os.system('mkdir -p patches')

i = 0
for file in files:
    filename = file.split('/')[-1]
    diff_string = 'diff -up %s %s' % (file, old_files_dict[filename])
    os.system('%s || %s > patches/patch_%s' %  (diff_string, diff_string, filename))
    i += 1

    
