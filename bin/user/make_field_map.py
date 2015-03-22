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
Example to demonstrate how to make a field map.

In this example, MAUS will build a geometry specified in the usual way by the
"simulation_geometry_filename" datacard and then plot the magnetic (b) field
parallel to the beam axis (bz) in the region z=-8 m to z=8 m.
"""

import Configuration  # MAUS configuration (datacards) module
import maus_cpp.globals as maus_globals # MAUS C++ calls
import maus_cpp.field as field # MAUS field map calls
import xboa.Common  # xboa post-processor library

def main():
    """
    Make a plot of z, bz
    """
    # set up datacards
    print "Welcome to MAUS field map maker example"
    configuration = Configuration.Configuration().\
                                          getConfigJSON(command_line_args=True)

    # initialise field maps and geometry
    print "Building field maps and other initialisation (this can take a while)"
    maus_globals.birth(configuration)

    # make lists of z, bz points
    print "Saving field values"

    with open( "field_map.dat", 'w' ) as outfile :
        for z_pos in range(-4000, 4001, 50):
            z_pos = float(z_pos)
            (bx_field, by_field, bz_field, ex_field, ey_field, ez_field) = \
                                            field.get_field_value(0., 0., z_pos, 0.)
            outfile.write( str( z_pos ) + ' ' +\
                           str( bx_field ) + ' ' +\
                           str( by_field ) + ' ' +\
                           str( bz_field ) + ' ' +\
                           str( ex_field ) + ' ' +\
                           str( ey_field ) + ' ' +\
                           str( ez_field ) + '\n' )


    # Clean up
    maus_globals.death()
    # Finished
    print "Finished"

if __name__ == "__main__":
    main()
