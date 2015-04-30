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
import shutil
import geometry
from geometry.GDMLtoCDB import Downloader
from geometry.GDMLtoMAUSModule import GDMLtomaus
from geometry.ConfigReader import Configreader
from geometry.GDMLFormatter import Formatter
from geometry.GDMLPacker import Unpacker
from geometry.LocationFit import ElementRotationTranslation

GDML_CACHE = 'gdml'
TMP_CACHE = 'tmp'

def main(): # pylint: disable = C0103
    """
    This is the code for the executable file which downloads the current valid
    geometry. It takes the arguments of the download directory. It will download
    the geometry from the database and translates the geometry into MAUS Module
    format.
    """
    # Read Config Arguments
    configuration = Configreader()
    dl_dir = configuration.geometry_download_directory
    # Set up target location for download
    gdml_cache = os.path.join(dl_dir, GDML_CACHE)
    try:
        os.mkdir(gdml_cache)
    except OSError:
        pass
    #Download file
    geometry_downloader = Downloader()

    if configuration.geometry_download_by == "run_number":
        geometry_downloader.download_geometry_by_run \
                        (configuration.geometry_download_run_number, gdml_cache)
    elif configuration.geometry_download_by == "current":
        geometry_downloader.download_current(gdml_cache)
    elif configuration.geometry_download_by == "id":
        geometry_downloader.download_geometry_by_id \
                                (configuration.geometry_download_id, gdml_cache)
    else:
        raise KeyError("Didn't recognise 'geometry_download_by' option '"+\
               configuration.geometry_download_by+"'. Should be on of\n"+\
               "run_number\ncurrent\nid\n")
    #Unzip file
    zip_filename = os.path.join(gdml_cache, geometry.GDMLtoCDB.GEOMETRY_ZIPFILE)
    zipped_geom = Unpacker(zip_filename, gdml_cache)
    zipped_geom.unzip_file()
    
    survey_fit = ElementRotationTranslation()
    survey_fit.execute()
    
    # format files
    gdmls = Formatter(gdml_cache, dl_dir)
    if gdmls.usegdml:
        gdmls.formatForGDML()
    else:
        gdmls.format()
    # convert to MAUS Modules
    maus_modules = GDMLtomaus(dl_dir)
    if gdmls.usegdml:
        maus_modules.generate_parent(dl_dir)
    else:
        maus_modules.convert_to_maus(dl_dir)
    # clean up if required
    if configuration.geometry_download_cleanup:
        shutil.rmtree(gdml_cache)
    print "Download Complete"

if __name__ == "__main__":
    main()
