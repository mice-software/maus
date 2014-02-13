#!/bin/env python

"""Create index file to access documentation components."""

# Generate an html file ("index.html") to allow easy access to the
# documentation for the various components.
# This file is run automatically at the end of "generate_MAUS_doc.py" and
# generally does not need to be run by itself.
# For more information see README.

import os

def main():
    """Main"""
    create_index_html()

def commonprefix(*args):
    """Fix behaviour of commonprefix function"""
    return os.path.commonprefix(*args).rpartition('/')[0]

def create_index_html():
    """Create the html file"""
    paths = [os.environ.get('MAUS_THIRD_PARTY'),
             os.environ.get('MAUS_ROOT_DIR')]
    # Get the locations of the local MAUS installation and the location of the
    # third party libraries relative to the point where their absolute paths
    # branch.
    tppath_segment = paths[0][len(commonprefix(paths))+1:]
    maus_path_segment = paths[1][len(commonprefix(paths))+1:]
    # Need to deal with the case where the two paths are the same
    if not tppath_segment:
        rel_tppath = ''
    # Generate the appropriate paths to link to the third party libaries.
    else:
        rel_tppath = tppath_segment + '/doc/'
        dir_prepend = '../'*(maus_path_segment.count('/')+2)
        rel_tppath = dir_prepend + rel_tppath
    # These need to be defined manually because it is possible that third party
    # and MAUS documentation are in the same directory.
    root_segment = ('<li><a href="' + rel_tppath +
                    'doxygen_root/html/index.html">ROOT</a></li>\n')
    geant4_segment = ('<li><a href="' + rel_tppath +
                      'doxygen_geant4/html/index.html">Geant4</a></li>\n')
    clhep_segment = ('<li><a href="' + rel_tppath +
                     'doxygen_clhep/html/index.html">CLHEP</a></li>\n')
    jsoncpp_segment = ('<li><a href="' + rel_tppath +
                       'doxygen_jsoncpp/html/index.html">JsonCpp</a></li>\n')
    # Fixed components of the index file.
    upper_html_snippet = """
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8" />
<title>MAUS doxygen Documentation Index</title>
<style type="text/css">
body {font-family:Verdana, Arial, Helvetica, sans-serif; font-size:14px;}
li {line-height:20px;}
li.title {font-weight:bold;}
ul ul {margin-bottom:20px;}

div 
</style>
</head>
<body>
<div>
  <h1>MAUS doxygen Documentation Index</h1>
  <p>Use the links below to access the doxygen documentation for the various components of MAUS and some third party libraries</p>
  <p>If links don't work, the documentation likely hasn't been compiled yet.</p>
  <ul>
    <li class="title">MAUS Documentation</li>
      <ul>
        <li><a href="doxygen_framework/html/index.html">Framework</a></li>
        <li><a href="doxygen_datastructure/html/index.html">DataStructure</a></li>
        <li><a href="doxygen_input/html/index.html">Input</a></li>
        <li><a href="doxygen_map/html/index.html">Map</a></li>
        <li><a href="doxygen_reduce/html/index.html">Reduce</a></li>
        <li><a href="doxygen_output/html/index.html">Output</a></li>
      </ul>
     <li class="title">Third Party Documentation</li>
      <ul>
    """
    lower_html_snippet = """
      </ul>
 </ul>
</div>
</body>
</html>
    """
    file1 = open('../index.html', 'w')
    file1.write(upper_html_snippet + root_segment + geant4_segment +
                clhep_segment + jsoncpp_segment + lower_html_snippet)
    file1.close()

if __name__ == "__main__":
    print "Typically you don't need to run this file by itself."
    print "It is automatically executed at the end of generate_MAUS_doc.py."
    print "Only execute this file if documentation has already been compiled."
    raw_input('If you would still like to generate the index file, '
              'please press Enter.')
    main()
