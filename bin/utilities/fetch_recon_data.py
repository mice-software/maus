#!/usr/bin/env python
# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#
"""
  This script downloads copies and untars the reconstructed data form the
  official repository.
"""

# pylint: disable = W0311, C0103, W0611

import MAUS

import tarfile
import urllib
import argparse
import os
import shutil

MAUS_ROOT_DIR = os.environ["MAUS_ROOT_DIR"]

DEFAULT_SERVER = "http://reco.mice.rl.ac.uk/"
DEFAULT_MAUS_VERSION = "MAUS-v2.5.1"
DEFAULT_OUT_DIRECTORY = os.path.join(MAUS_ROOT_DIR, "files/recon/")

DEFAULT_POSTFIX = "_offline.tar"
DEFAULT_MAX_ATTEMPTS = 3



def format_run_numbers(numbers) :
  """
    Make sure the list of run numbers are correctly formatted.
  """
  run_strings = []

  for run_number in numbers :
    string = str(run_number).zfill(5)
    run_strings.append(string)

  return run_strings


def construct_filenames(numbers) :
  """
    Create a list of filenames using the run numbers.
  """
  filenames = []

  for run_number in numbers :
    filename = str(run_number).zfill(5) + DEFAULT_POSTFIX
    filenames.append(filename)

  return filenames

def construct_by_hundreds_name(run_number) :
  """
    Make a folder name to store runs sorted by hundreds
    e.g. 07400. Return this value as a string.
  """
  folder_name = run_number[0:3]
  folder_name = folder_name + '00'
  return folder_name

def get_file(server, version, file_name, out_path) :
  """
    Download the specified recon tar file from the SERVER.
  """
  url = server + version + '/' + file_name
  outputfile = os.path.join(out_path, file_name)
  try :
    urllib.urlretrieve(url, filename=outputfile)
  except Exception as ex :
    raise ex
  return outputfile


def extract_data(filename, outpath) :
  """
    Extract all files from the downloaded tar file
  """
  try :
    with tarfile.open(filename) as tar :
      tar.extractall(path=outpath)

  except Exception as ex :
    raise ex


if __name__ == "__main__" : 
  parser = argparse.ArgumentParser( description='A simple interface to the '+\
                                      'online reconstructed data repository.' )

  parser.add_argument( 'run_numbers', nargs='+', type=int, \
                                                  help='A list of run numbers')

  parser.add_argument( '--maus_version', metavar="VERSION", \
                                                default=DEFAULT_MAUS_VERSION, \
          help='Specify the version of MAUS you want the reconstruction from.'+\
                                              ' Should resemble: MAUS-v2.3.1' )

  parser.add_argument( '--server', metavar='server', default=DEFAULT_SERVER, \
                                           help='Specfiy the download server.')

  parser.add_argument( '-D', '--output_directory', metavar='OUT', \
                                               default=DEFAULT_OUT_DIRECTORY, \
     help='Specify the location of the directory in which to store the data.' )

  parser.add_argument( '-O', '--overwrite', action='store_true', \
      help='Flag to overwrite data if the directory structure alread exists.' )

  try :
    namespace = parser.parse_args()

    run_numbers = format_run_numbers(namespace.run_numbers)

    files = construct_filenames(namespace.run_numbers)

    outdir = namespace.output_directory

    error_list = []

    print
    if len(run_numbers) == 1 :
      print "Downlading", len(run_numbers), "Run"
    else :
      print "Downlading", len(run_numbers), "Runs"

    for run_id in range(len(files)) :
      try :
        print
        print "Run Number:", run_numbers[run_id]
        tar_outpath = os.path.join(outdir, namespace.maus_version)
        data_outpath = os.path.join(tar_outpath, \
          construct_by_hundreds_name(run_numbers[run_id]), run_numbers[run_id])
        if os.path.isdir(data_outpath) :
          if namespace.overwrite :
            print " - Removing existing directory..."
            shutil.rmtree(data_outpath)
          else :
            print " - Directory already exists. Skipping."
            continue

        os.makedirs(data_outpath)

        print " - Downloading..."
        outfile = get_file(namespace.server, namespace.maus_version, \
                                                    files[run_id], tar_outpath)
        print " - Installing..."
        try :
          extract_data(outfile, data_outpath)
        except BaseException as ex :
          # error_list.append(ex)
          os.rmdir(data_outpath)
        os.remove(outfile)

      except KeyboardInterrupt :
        print
        print "Stopping"
        print
        break
      except BaseException as ex :
        error_list.append(ex)

    print
    print "Complete"
    print

    if len(error_list) > 0 :
      print "There Were:", len(error_list), "Errors:"
      print
      for error in error_list :
        print error
        print

  except BaseException as ex:
    raise
  else :
    print
