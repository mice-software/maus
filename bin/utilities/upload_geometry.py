#!/usr/bin/env python

"""
upload_geometry: Uploads geometries to the configuration database


"""
#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

import os.path
import datetime
import geometry.GDMLtoCDB
from geometry.GDMLPacker import Packer
from geometry.GDMLtoCDB import Uploader
from geometry.ConfigReader import Configreader

def check_configuration(configuration):
    """
    Check that at least a geometry_upload_note has been defined and a 
    geometry_upload_valid_from string has been specified. Does not check that 
    the valid_from is of correct datetime format.
    """
    if configuration.geometry_upload_note == "":
        raise ValueError("geometry_upload_note must be specified")
    if configuration.geometry_upload_valid_from == "":
        raise ValueError("geometry_upload_valid_from must be specified like"+\
                        datetime.datetime.now())

def main():
    """
    Executable File
    
    This file is the executable file which uploads the chosen geometry to the 
    Configuration Database. It takes a command line argument of a txt file 
    which contains variables which will aid the uploading of the geometry. The
    main variables are the path to the folder which contains ONLY the fastrad
    outputted files. The second variable is the string which describes that
    geometry.   
    """
    #configreader class takes the arguments from the txt file so we can use them
    configuration = Configreader()
    ul_dir = configuration.geometry_upload_directory
    
    #upload the geometry
    upload_geometry = Uploader \
                       (ul_dir, str(configuration.geometry_upload_note))
    filelist_path = os.path.join(ul_dir, geometry.GDMLtoCDB.FILELIST)
    zfile = Packer(filelist_path)
    upload_file = zfile.zipfile(ul_dir)
    upload_geometry.upload_to_cdb(upload_file)
    #delete the text file produced by uploadtoCDB()
    #delete temporary files if specified
    if configuration.geometry_upload_cleanup:
        os.remove(filelist_path)
        os.remove(upload_file)

if __name__ == "__main__":
    main()
