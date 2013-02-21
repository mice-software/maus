#!/usr/bin/env python

#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""
Utility script to download data files. For more information, use -h on the
command line.
"""

DESCRIPTION = \
"""
Download an input data file (i.e. output from MICE experiment) from the web
"""

import os
import sys
import argparse
import urllib
import time

# Dynamically set the docstring
__doc__ = DESCRIPTION #pylint: disable = W0622

DATA_SERVER = "http://www.hep.ph.ic.ac.uk/micedata/"
TEMP_DATA = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp/data_files")

def make_arg_parser():
    """
    Parse command line arguments.

    Use -h switch at the command line for information on command line args used.
    """
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument('-i', '--file_name_in', dest='file_name_in',
                        help='Name/location of the input data file on the '+\
                             'server (e.g. Step1/03900/03944.tar). If this '+\
                             'is just an integer, the appropriate prefix is '+\
                             'added',
                        required=True)
    parser.add_argument('--url_in', dest='url_in', \
                        help='URL of the input data file. Default is\n'+\
                        '    '+DATA_SERVER, default=DATA_SERVER)
    parser.add_argument('--directory_out', dest='directory_out', \
                        help='Output directory to which the data file should '+\
                        'be written. Default is\n'+\
                        '    $MAUS_ROOT_DIR/tmp/data_files', \
                        default=TEMP_DATA)
    parser.add_argument('--file_name_out', dest='file_name_out', \
                        help='Output file name to which the data file should '+\
                        'be written. Default is the input file name stripped '+\
                        'of all directories.', \
                        default='')
    parser.add_argument('--overwrite', dest='overwrite',
                         help='Set this flag to perform the download even if '+\
                         'the file already exists (default is to abort).',
                         action='store_true')
    parser.add_argument('--no_untar', dest='no_untar',
                         help='Set this flag to prevent extraction of the '+\
                         'downloaded file. If this flag is not set and '+\
                         'extraction fails, will return non-zero with an '+\
                         'error message (but download should complete).',
                         action='store_false')
    return parser

def wget_file(url_in, file_name_in, # pylint: disable=R0913
              directory_out, file_name_out,
              will_overwrite):
    """
    Get data from a file at a given URL and place it in the specified directory
    @param url_in (string) input url at which the file can be found
    @param file_name_in (string) input file name
    @param directory_out (string) output directory
    @param file_name_out (string) output file name
    @param will_overwrite (bool) set to true to overwrite the file if it is
           found
    Creates directory_out if it doesn't exist
    """
    if file_name_out == '':
        file_name_out = os.path.split(file_name_in)[1]
    target_out = os.path.join(directory_out, file_name_out)
    if not os.path.isdir(directory_out):
        os.mkdir(directory_out)
    if will_overwrite or not os.path.isfile(target_out):
        target_in = url_in+file_name_in
        print 'Getting ', target_in,
        try:
            urllib.urlretrieve(target_in, target_out, url_hook)
            print ' ... Succeeded'
        except IOError:
            print " ... Failed"
            raise
    else:
        print 'Found', target_out, 'so not going to download'
    return target_out

OLD_TIME = -20
TIME = 0
def url_hook(transferred, size, total):
    """
    Hook for monitoring download
    """
    global TIME, OLD_TIME # pylint: disable = W0603
    TIME = time.clock()
    if TIME-OLD_TIME > 1:
        print
        print 'Downloaded', round(transferred*size/1048576., 1), 'MB out of', \
                                                 round(total/1048576., 1), 'MB',
        OLD_TIME = TIME

def untar_file(file_name):
    """Does nothing - not implemented"""
    if file_name:
        pass

def main(argv):
    """
    Main loop. Parse any input arguments, then reformat and write the input
    file to some output file. Run less if required. if no output filename was
    specified assume the output isn't for keeping so delete it.
    """
    args = make_arg_parser().parse_args(argv)
    file_out = wget_file(args.url_in, args.file_name_in, args.directory_out,
                         args.file_name_out, args.overwrite)
    if not args.no_untar:
        untar_file(file_out)

if __name__ == "__main__":
    main(sys.argv[1:])

