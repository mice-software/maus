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

"""
In this simulation example, we pass a grid of particles in (x, y) through a
solenoid lens and examine the grid on the far side of the lens. Note the grid is
rotated, due to the x-y coupling, and grid lines have become wavy due to
spherical aberrations in the lens.
"""

import subprocess
import os

from xboa.Bunch import Bunch

def run_simulation():
    """
    Run simulate_mice.py passing configuration file as a command line argument
    """
    sim_exe = os.path.expandvars("$MAUS_ROOT_DIR/bin/simulate_mice.py")
    config_file = os.path.expandvars(
        "$MAUS_ROOT_DIR/bin/examples/simulate_particles_in_grid/"+\
        "simulate_particles_in_grid_configuration.py"
    )
    proc = subprocess.Popen([sim_exe, "--configuration_file", config_file])
    proc.wait()

def plot_output():
    """
    Plot output from the simulation
    """
    bunch_list = Bunch.new_list_from_read_builtin('maus_root_virtual_hit',
                                                  'maus_output.root')
    for bunch in bunch_list:
        canvas, hist, graph = bunch.root_scatter_graph('x', 'y', 'mm', 'mm',
                                      xmin=-600, xmax=600, ymin=-600, ymax=600,
                                     	hist_title_string='z='+str(bunch[0]['z']))
    return (canvas, hist, graph)

def main():
    """
    Run the simulation and make a couple of plots showing output
    """
    run_simulation()
    plot_output()

if __name__ == "__main__":
    main()
    raw_input()
