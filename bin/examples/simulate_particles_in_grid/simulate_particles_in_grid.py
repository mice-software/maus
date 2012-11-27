
import subprocess
import os

import xboa.Bunch
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

def main():
    """
    Run the simulation and make a couple of plots showing output
    """
    run_simulation()
    plot_output()

if __name__ == "__main__":
    main()
    raw_input()
