#!/usr/bin/env python

"""
M. Littlefield
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

import os
import geometry
from geometry.GDMLtoCDB import Downloader
from geometry.GDMLtoMAUSModule import GDMLtomaus
from geometry.ConfigReader import Configreader
from geometry.GDMLFormatter import Formatter
from geometry.GDMLPacker import Unpacker

GDML_CACHE = 'gdml'
TMP_CACHE = 'tmp'

def main():
    """
    This is the code for the executable file which downloads the current valid
    geometry. It takes the arguments of the download directory. It will download
    the geometry from the database and translates the geometry into MAUS Module
    format.
    """
    #Read Config Arguments
    configuration = Configreader()
    configuration.readconfig()
    #Download file
    geometry_downloader = Downloader()
    gdml_cache = os.path.join(configuration.maus_dl_dir, GDML_CACHE)
    tmp_cache = os.path.join(configuration.maus_dl_dir, TMP_CACHE)
    try:
        os.mkdir(gdml_cache)
    except OSError:
        pass
    try:
        os.mkdir(tmp_cache)
    except OSError:
        pass
    geometry_downloader.download_current(gdml_cache)
    #Unzip file
    zip_filename = os.path.join(gdml_cache, geometry.GDMLtoCDB.GEOMETRY_ZIPFILE)
    zipped_geom = Unpacker(zip_filename, gdml_cache)
    zipped_geom.unzip_file()
    #Format Files
    gdmls = Formatter(gdml_cache, configuration.maus_dl_dir, tmp_cache)
    gdmls.format()
    #Convert to MAUS Modules
    maus_modules = GDMLtomaus(configuration.maus_dl_dir)
    maus_modules.convert_to_maus(configuration.maus_dl_dir)
    print "Download Complete"

if __name__ == "__main__":
    main()
