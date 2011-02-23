# Tunnell - script to check for changes between G4MICE and MAUS and make patches
#
# Rogers - list differences between the file tree (missing files from either
#          side)
#        - add filter function
#        - push to arbitrary depth recursion

import os
import copy

#all_dirs = ['/Interface/', '/Analysis/', '/BeamTools/', '/Config/', '/DetModel/', '/Interface/', '/Persist/', '/Recon/', '/Simulation/', '/Visualization/',
#'/Applications/',  'Calib', '/DetResp/', '/EngModel/', '/GDML_3_0_0/', '/Optics/', '/RealData/']
exclude_dirs = ['Analysis', 'BeamTools', 'Config', 'DetModel', 'Persist', 'Recon', 'Simulation', 'Visualization',
'Applications',  'Calib', 'DetResp', 'EngModel', 'GDML_3_0_0', 'Optics', 'RealData', 'patches', 'test']

mice_root = '/home/cr67/G4MICE/MICE-dev/'


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
      files = filter(lambda x: x.find(suffix) == len(x)-len(suffix), files)
      for file in files: 
        file_list.append(os.path.join(root, file))
  return file_list

def filters(file_list, exclude_dirs, exclude_files):
  """
  Filter out files with any of exclude_dirs in the path or with any of exclude_files in the file name 
    file_list = list of file paths that will be filtered
    exclude_dirs = list of directories to exclude
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
  for file in g4mice_files:
      key = file.split('/')[-1]
      g4mice_files_dict[key] = file

  os.system('mkdir -p patches')
  for file in maus_files:
      if file.split('/')[-1] in g4mice_files_dict:
        filename = file.split('/')[-1]
        diff_string = 'diff -up %s %s' % (file, g4mice_files_dict[filename])
        os.system('%s > /dev/null || %s > patches/patch_%s' %  (diff_string, diff_string, filename))

def find_missing_files(maus_list, g4mice_list):
  """
  Make a list of files that are either missing from maus or missing from g4mice
  """
  maus_cut = map(lambda x: x.split('/')[-1], maus_list)
  g4mice_cut = map(lambda x: x.split('/')[-1], g4mice_list)
  for f in maus_cut:
    if f not in g4mice_cut:
      print 'Found '+f+' in maus but not g4mice'
  print
  print
  for f in g4mice_cut:
    if f not in maus_cut:
      print 'Found '+f+' in g4mice but not maus'

if __name__=="__main__":
  maus_files   = get_file_list('./')
  g4mice_files = get_file_list(mice_root)
  maus_files   = filters(maus_files, exclude_dirs, [])
  g4mice_files = filters(g4mice_files, exclude_dirs, [])
  make_patches(maus_files, g4mice_files)
  find_missing_files(maus_files, g4mice_files)

