"""Script to check for changes between G4MICE and MAUS and make patches
"""

import sys
import os
import copy
import fnmatch

#all_dirs = ['/Interface/', '/Analysis/', '/BeamTools/', '/Config/', '/DetModel/', '/Interface/', '/Persist/', '/Recon/', '/Simulation/', '/Visualization/',
#'/Applications/',  'Calib', '/DetResp/', '/EngModel/', '/GDML_3_0_0/', '/Optics/', '/RealData/']
EXCLUDEDIRS = ['Analysis',  'Persist', 'Simulation', 'Visualization',
'Applications', 'GDML_3_0_0', 'Optics', 'RealData', 'patches', 'test', 'patches2']

G4MICEROOT = '/tmp'  #  this gets overwritten later


def get_file_list(path, suffixes=['hh','cc']):
    """
    Get list of files with suffix from suffixes
      path = string with start path
      suffixes = list of suffixes (string file endings)
    Returns a list of files and paths
    """
    file_list = []
    for root, dirs, files in os.walk(path):
        for suffix in suffixes:
            files_suffix = filter(lambda x: x.find(suffix) == len(x)-len(suffix), files)
            for my_file in files_suffix:
                file_list.append(os.path.join(root, my_file))
    return file_list

def filters(file_list, exclude_dirs, exclude_files):
    """
    Filter out files with any of EXCLUDEDIRS in the path or with any of exclude_files in the file name
      file_list = list of file paths that will be filtered
      EXCLUDEDIRS = list of directories to exclude
      exclude_files = list of files to exclude
    Returns a list of file paths with the excluded files removed
    """
    _file_list = copy.deepcopy(file_list)
    for ex in exclude_dirs:
        _file_list = filter(lambda x: not ex in x.split('/'), _file_list)
    for ex in exclude_files:
        _file_list = filter(lambda x: not x.split('/')[-1] == ex, _file_list)
    return _file_list

def make_patches(maus_list, g4mice_list):
    """
    Make patches (diffs) of files that have changed between g4mice and maus, put them in patches dir
    """
    g4mice_files_dict = {}
    for my_file in g4mice_files:
        key = my_file.split('/')[-1]
        g4mice_files_dict[key] = my_file

    my_patches=[]

    os.system('mkdir -p patches')
    for my_file in maus_files:
        if my_file.split('/')[-1] in g4mice_files_dict:
            filename = my_file.split('/')[-1]
            diff_string = 'diff -rp %s %s' % (my_file, g4mice_files_dict[filename])
            os.system('%s > /dev/null || %s > patches/patch_%s' %  (diff_string, diff_string, filename))
            my_patches.append('patch_%s' % filename)

    

def find_missing_files(maus_list, g4mice_list):
    """
    Make a list of files that are either missing from maus or missing from g4mice
    """
    maus_cut = map(lambda x: x.split('/')[-1], maus_list)
    g4mice_cut = map(lambda x: x.split('/')[-1], g4mice_list)
    for my_file in maus_cut:
        if my_file not in g4mice_cut:
            print 'Found '+my_file+' in maus but not g4mice'
    print
    print
    for my_file in g4mice_cut:
        if my_file not in maus_cut:
            print 'Found '+my_file+' in g4mice but not maus'

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print 'usage: %s G4MICE_CVS_ROOT_DIR' % sys.argv[0]
        os.sys.exit()

    G4MICEROOT = sys.argv[1]

    maus_files   = get_file_list('./')
    g4mice_files = get_file_list(G4MICEROOT)
    maus_files   = filters(maus_files, EXCLUDEDIRS, [])
    g4mice_files = filters(g4mice_files, EXCLUDEDIRS, [])
    make_patches(maus_files, g4mice_files)
    find_missing_files(maus_files, g4mice_files)
    
    
