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
    print "Getting field values"
    z_list = []
    bz_list = []
    for z_pos in range(-8000, 8001, 50):
        z_pos = float(z_pos)
        (bx_field, by_field, bz_field, ex_field, ey_field, ez_field) = \
                                        field.get_field_value(0., 0., z_pos, 0.)
        z_list.append(z_pos*1e-3)  # z in metres
        bz_list.append(bz_field*1e3)  # bz in T
        print 'z:', z_pos, ' ** b:', bx_field, by_field, bz_field, \
                               'e:', ex_field, ey_field, ez_field

    # now make a ROOT graph of bz against z
    print "Graphing field values"
    canvas = xboa.Common.make_root_canvas("bz vs z")
    hist, graph = xboa.Common.make_root_graph("bz vs z", z_list, "z [m]",
                                                         bz_list, "B_{z} [T]")
    hist.Draw()
    graph.Draw('l')
    canvas.Update()
    canvas.Print('bfield_vs_z.png')
    # Clean up
    maus_globals.death()
    # Finished
    print "Finished"

if __name__ == "__main__":
    main()
