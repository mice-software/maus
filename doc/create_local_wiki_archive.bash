#!/bin/bash
#
# This script creates a local .tar.gz copy
# of the redmine wiki of MAUS. -Tunnell
#
rm -Rf wiki
mkdir wiki
cd wiki
wget --no-directories --html-extension --level=1 --recursive --convert-links --no-parent -p http://micewww.pp.rl.ac.uk/projects/maus/wiki/Page_index
cd ..
tar cvfz wiki.tar.gz wiki
