#!/bin/env python

# Generate an html file ("index.html") to allow easy access to the documentation for the various components.
# This file is run automatically at the end of "generate_MAUS_doc.py" and generally does not need to be run by itself.
# For more information see README.

import os

def main():
    create_index_html()
    
def create_index_html():
    Paths = [os.environ.get('MAUS_THIRD_PARTY'),os.environ.get('MAUS_ROOT_DIR')]
    # Get the locations of the local MAUS installation and the location of the third party libraries
    # relative to the point where their absolute paths branch.
    TPPathSegment = Paths[0][len(os.path.commonprefix(Paths)):]
    MausPathSegment = Paths[1][len(os.path.commonprefix(Paths)):]
    # Need to deal with the case where the two paths are the same
    if not TPPathSegment:
        RelTPPath = ''
    # Generate the appropriate paths to link to the third party libaries.
    else:
        RelTPPath = TPPathSegment + '/doc/'
        DirPrepend = '../'
        for n in range(MausPathSegment.count('/')+1):
            DirPrepend = DirPrepend + '../'
        RelTPPath = DirPrepend + RelTPPath
    # These need to be defined manually because it is possible that third party and MAUS documentation are in the same directory.
    RootSegment = '<li><a href="' + RelTPPath + 'doxygen_root/html/index.html">ROOT</a></li>\n'
    Geant4Segment = '<li><a href="' + RelTPPath + 'doxygen_geant4/html/index.html">Geant4</a></li>\n'
    ClhepSegment = '<li><a href="' + RelTPPath + 'doxygen_clhep/html/index.html">CLHEP</a></li>\n'
    JsonCppSegment = '<li><a href="' + RelTPPath + 'doxygen_jsoncpp/html/index.html">JsonCpp</a></li>\n'
    # Fixed components of the index file.
    Upper = """
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
    Lower = """
      </ul>
 </ul>
</div>
</body>
</html>
    """
    f = open('../index.html', 'w')
    f.write(Upper + RootSegment + Geant4Segment + ClhepSegment + JsonCppSegment + Lower)
    f.close()

if __name__ == "__main__":
    print "Typically you don't need to run this file by itself."
    print "It is automatically executed at the end of generate_MAUS_doc.py."
    print "Only execute this file if the documentation has already been compiled."
    raw_input('If you would still like to generate the index file, please press Enter.')
    main()
