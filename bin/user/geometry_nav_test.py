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
Example to demonstrate how to use the geometry navigator

"""

import json
import Configuration  # MAUS configuration (datacards) module
import maus_cpp.globals as maus_globals # MAUS C++ calls
import maus_cpp.material # MAUS Geometry Navigator class

def main():
    """
    Make a plot of z, bz
    """
    # set up datacards
    print "Welcome to MAUS field map maker example"
    configuration = Configuration.Configuration().\
                                          getConfigJSON(command_line_args=True)
    conf_json = json.loads(configuration)
    conf_json["simulation_geometry_filename"] = "files/geometry/custom/geometry_nav_test.dat"
    configuration = json.dumps(conf_json)

    # initialise field maps and geometry
    print "Building field maps and other initialisation (this can take a while)"
    maus_globals.birth(configuration)
    
    print "Testing Functionality"
    print

    maus_cpp.material.set_position( 0.0, 0.0, -50.0 )

    print "Current Position:", maus_cpp.material.get_position()
    print


    for i in range( 100 ) :
      maus_cpp.material.step( 0.0, 0.0, 1.0 )
      print (-50.0 + float(i)),  maus_cpp.material.get_material_data()
      print

    print
    print
    print "Final Position:", maus_cpp.material.get_position()
    print



    # Clean up
    maus_globals.death()
    # Finished
    print "Finished"

if __name__ == "__main__":
    main()
