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

import Configuration  # MAUS configuration (datacards) module
import libMausCpp  # MAUS C++ calls
import xboa.Common  # xboa post-processor library

def main():
    print "Welcome to MAUS field map maker"
    configuration = Configuration.Configuration().\
                                          getConfigJSON(command_line_args=True)
    print "Building field maps and other initialisation (this can take a while)"
    libMausCpp.initialise(configuration)
    print "Getting field values"
    # "list comprehension" - pythonesque way of doing something to each item in 
    # a list
    z_list = [float(z)*1e-3 for z in range(-8000, 8001, 50)]
    # another list comprehension - this is where we generate the bz values
    bz_list = [libMausCpp.Field.get_field_value(0., 0., z*1e3, 0.)[2]*1e3 \
                                                                for z in z_list]
    print "Graphing field values"
    # now make a ROOT graph of bz against z
    canvas = xboa.Common.make_root_canvas("bz vs z")
    hist, graph = xboa.Common.make_root_graph("bz vs z", z_list, "z [m]",
                                                         bz_list, "B_{z} [T]")
    hist.Draw()
    graph.Draw('l')
    canvas.Update()

    print "Finished - press carriage return to end (closing all windows)"
    raw_input()

if __name__ == "__main__":
    main()

