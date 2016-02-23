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
import xboa.common  # xboa post-processor library

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
    canvas = None
    graph_list = []
    for r_pos, line_color in [(0., 1), (150., 2)]:
        z_list = [float(z_pos) for z_pos in range(12000, 18001, 50)]
        bz_list = []
        for z_pos in z_list:
            (bx_field, by_field, bz_field, ex_field, ey_field, ez_field) = \
                                     field.get_field_value(r_pos, 0., z_pos, 0.)
            bz_list.append(bz_field*1e3)  # bz in T
            print 'z:', z_pos, ' ** b:', bx_field, by_field, bz_field, \
                                   'e:', ex_field, ey_field, ez_field

        # now make a ROOT graph of bz against z
        print "Graphing field values"
        hist, graph = xboa.common.make_root_graph("x="+str(r_pos)+" mm", z_list, "z [m]",
                                                             bz_list, "B_{z} [T]")
        graph.SetLineColor(line_color)
        if canvas == None:
            canvas = xboa.common.make_root_canvas("bz vs z")
            hist.Draw()
        graph_list.append(graph)
        graph.Draw('l')
        canvas.Update()
    legend = xboa.common.make_root_legend(canvas, graph_list)
    print legend.GetX1NDC(), legend.GetX2NDC()
    legend.SetX1NDC(0.7)
    legend.SetX2NDC(0.9)
    legend.SetBorderSize(1)
    #legend.Draw()
    canvas.Print('plots/bfield_vs_z.png')
    # Clean up
    maus_globals.death()
    # Finished

if __name__ == "__main__":
    main()
    print "Finished - press <CR> to end"
    raw_input()
    
