#!/usr/bin/env python
"""Generate doxygen documentation for MAUS components"""
# This file should be run second, after generate_third_party_doc.py
# For more information see README.

import subprocess
import os
import sys
from generate_doc_index import create_index_html # pylint: disable=F0401

# pylint: disable = C0103, R0914
if os.environ.get('MAUS_THIRD_PARTY') == None:
    print 'MAUS environment variables not set.'
    print 'Please make sure to source env.sh before running this script.'
    print 'Exiting...'
    exit()

def main():
    """Generate list of tagfiles, then trigger doxygen for components"""
    prompt = 1
    if len(sys.argv) > 1 and str(sys.argv[1]) == '--noprompt':
        prompt = 0
    maus_thirdparty = os.environ['MAUS_THIRD_PARTY']
    maus_rootdir = os.environ['MAUS_ROOT_DIR']
    thirdpartylibs = ['root', 'geant4', 'clhep', 'jsoncpp']
    maus_components = ['framework', 'datastructure', 'input', 'map', 'reduce',
                      'output', 'utilities']
    print 'MAUS_THIRD_PARTY path is set to ' + maus_thirdparty
    print 'Checking for existing tag files...'
    tagfiles = []
    # Check for tagfiles. Note that this information isn't actually used as an
    # input to doxygen, just to let the user choose whether he wants to
    # continue if fewer tagfiles than expected were found. See below for more
    # info.
    for tplib in thirdpartylibs:
        tppath = (maus_thirdparty + '/doc/doxygen_' + tplib + '/' + tplib +
                  '.tag')
        if os.path.isfile(tppath):
            tagfiles.append(tppath)
            print 'Tagfile found: ' + tppath
    check_tagfiles(tagfiles, prompt)
    if prompt != 0:
        raw_input('If everything is correct, please press enter to compile ' +
                  'documentation')

    # To make things portable we need to get the relative path from the MAUS dir
    # to the third party dir, as we do in generate_doc_index.py
    paths = [maus_thirdparty, maus_rootdir]
    tppath_segment = paths[0][len(commonprefix(paths))+1:]
    maus_path_segment = paths[1][len(commonprefix(paths))+1:]
    # Need to deal with the case where the two paths are the same
    if tppath_segment == maus_path_segment:
        rel_tppath = ''
    # Generate the appropriate paths to link to the third party libaries.
    else:
        rel_tppath = tppath_segment + '/doc/'
        dir_prepend = '../'*(maus_path_segment.count('/')+2)
        rel_tppath = dir_prepend + rel_tppath

    # A list of all tagfiles with doc location. MAUS component tagfiles are
    # added one by one as they are created.
    tagfiles_dict = dict((i, '../' + rel_tppath + 'doxygen_' + i + '/' + i +
                        '.tag=' + '../../' + rel_tppath + 'doxygen_' + i +
                        '/html') for i in thirdpartylibs)
    tagfilesmaus_dict = dict((i, '../doxygen_' + i + '/' + i +
                            '.tag=' + '../../doxygen_' + i +
                            '/html') for i in maus_components)
    tagfiles = ' '.join(tagfiles_dict.values()) # pylint: disable = R0204

    for component in maus_components:
        if component == 'framework':
            run_doxygen_framework(tagfiles)
        elif component == 'datastructure':
            run_doxygen_datastructure(tagfiles)
        else:
            run_doxygen(component, tagfiles)
        tagfiles = tagfiles + ' ' + tagfilesmaus_dict[component]
    create_index_html()
    exit()


def check_tagfiles(tagfiles, prompt):
    """Check for tagfiles, if missing prompt user for how to proceed"""
    # Case where we have 0 or 0 < # < expected tagfiles found.
    if len(tagfiles) == 0:
        print ('No Tagfiles found. Most likely, third party documentation has '
              'not been compiled.')
    elif len(tagfiles) <= 3:
        print ('Only found tagfiles for ' + str(len(tagfiles)) +
              ' out of 4 third party libraries.')
        print ('Maybe there was a problem with compiling third party '
              'documentation?')
    # If fewer tagfiles than expected found, prompt whether documentation
    # should still be compiled.
    if len(tagfiles) <= 3:
        choice = ''
        while choice not in ['c', 'a'] and prompt != 0:
            choice = raw_input('Would you like to [c]ontinue anyway or '
                               '[a]bort ')
            if choice == 'a':
                exit()
            else:
                continue


# framework and datastructure need own snippets due to some special settings

def run_doxygen_framework(tagfiles):
    """Run doxygen for the MAUS framework"""
    script = """( cat Doxyfile.general ;
        echo 'PROJECT_NAME = "MAUS-framework"';
        echo INPUT = ../../src/common_*/ ../../src/py_cpp/ ../../src/legacy/;
        echo OUTPUT_DIRECTORY = ../doxygen_framework/;
        echo TAGFILES = $0;
        echo QUIET = YES;
        echo EXCLUDE_PATTERNS = */DataStructure/*;
        echo GENERATE_TAGFILE = ../doxygen_framework/framework.tag ) | doxygen -
        """
    subprocess.call([script, str(tagfiles)], shell=True)


def run_doxygen_datastructure(tagfiles):
    """Run doxygen for the MAUS DataStructure"""
    script = """( cat Doxyfile.general ;
        echo 'PROJECT_NAME = "MAUS-datastructure"';
        echo INPUT = ../../src/common_cpp/DataStructure/;
        echo OUTPUT_DIRECTORY = ../doxygen_datastructure/;
        echo TAGFILES = $0;
        echo QUIET = YES;
        echo EXCLUDE_PATTERNS = *MausDataStructure.*
        echo GENERATE_TAGFILE = ../doxygen_datastructure/datastructure.tag ) | doxygen -
        """
    subprocess.call([script, str(tagfiles)], shell=True)


def run_doxygen(component, tagfiles):
    """Run doxygen for other MAUS components"""
    script = """( cat Doxyfile.general ;
        echo PROJECT_NAME = MAUS-$0;
        echo INPUT = ../../src/$0/;
        echo OUTPUT_DIRECTORY = ../doxygen_$0/;
        echo TAGFILES = $1;
        echo QUIET = YES;
        echo GENERATE_TAGFILE = ../doxygen_$0/$0.tag ) | doxygen -
        """
    subprocess.call([script, str(component), str(tagfiles)], shell=True)

def commonprefix(*args):
    """Fix behaviour of commonprefix function"""
    return os.path.commonprefix(*args).rpartition('/')[0]

if __name__ == "__main__":
    main()
