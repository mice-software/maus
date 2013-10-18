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
Example to demonstrate use of the optics routines to plot the optical beta 
function.

In this example, MAUS will build a set of solenoid field maps for MICE Step IV,
build the corresponding transfer matrices, one for each virtual plane, and then
propagate a covariance matrix (beam ellispe) through the transfer matrices, 
plotting the result.
"""

import os
import json

import ROOT
# Set to true to force PyROOT to ignore the command line
ROOT.PyConfig.IgnoreCommandLineOptions = True

import xboa.Common

import maus_cpp.field
import maus_cpp.globals
import maus_cpp.covariance_matrix
import maus_cpp.optics_model
import Configuration

class Lattice:
    """
    Magnetic lattice and functions to generate and plot transfer matrices on
    said lattice
    """

    def __init__(self, initial_momentum, z_start, lattice_file):
        """
        Initialises some data
        """
        self.mom = initial_momentum
        self.z_pos = z_start
        self.mass = xboa.Common.pdg_pid_to_mass[13]
        self.optics_model = None
        self.first = True
        self.lattice_file = lattice_file

    def setup_lattice(self):
        """
        Setup the lattice for optics work

        - sets up data cards and loads MAUS with those data cards
        - builds the geometry and field model
        - builds the transfer matrices
        """
        config_str = Configuration.Configuration().\
                                          getConfigJSON(command_line_args=False)
        config_json = json.loads(config_str)
        config_json["simulation_reference_particle"] = {
          "random_seed": 0,
          "energy":(self.mass**2+self.mom**2)**0.5,
          "particle_id":-13,
          "time": 0.0,
          "position":{"x":0.0, "y":0.0, "z":self.z_pos},
          "momentum":{"x":0.0, "y":0.0, "z":1.0}
        }
        print "Setting up the magnets now, using file", self.lattice_file
        config_json["simulation_geometry_filename"] = self.lattice_file
        config_json["verbose_level"] = 1
        config_str = json.dumps(config_json)
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(config_str)
        maus_cpp.globals.set_monte_carlo_mice_modules(
                             maus_cpp.mice_module.MiceModule(self.lattice_file))
        print """
MAUS is about to fire the reference particle through the fields. The reference
particle determines what transfer matrices are built; A transfer matrix is
created each time the reference particle crosses a VirtualPlane, by means of a
Taylor expansion about the reference particle. For this example, the following
reference particle will be used:
"""+json.dumps(config_json["simulation_reference_particle"], indent=2)+"""
where
  - random_seed is ignored.
  - energy is the total energy.
  - particle_id is the particle data group id (-11 for electrons, -13 for muons,
    2212 for protons).
  - time is the initial time of the reference particle. MAUS will set cavity
    phases accordingly, although there are none in this example.
  - position is the x, y, z position. In this example, z is along the beam axis.
  - momentum describes the direction of the particle, but will always be 
    normalised to match the total energy.
"""
        self.optics_model = maus_cpp.optics_model.OpticsModel()

    def get_beta(self, covariance_matrix):
        """
        Return the transverse beta function, given by (Var(x)+Var(y))*p/(2m)
        """
        return (covariance_matrix.get_element(3, 3)+\
                covariance_matrix.get_element(5, 5))*self.mom/2./self.mass

    def plot_beta(self, canvas=None, line_color=1):
        """
        Plot the beta function for some initial value and momentum
        """
        print """
Now we plot optical beta as a function of z along the axis. Transport is
performed from the position of the reference particle and each virtual plane.
The function

    OpticsModel.transport_covariance_matrix(ellipse_start, z_position)

transports ellipse_start to the nearest virtual plane to z_position (NOT to the
precise value of z_position).

In this example, we set up the ellipse using the Penn parameterisation for
cylindrically symmetric beams with kinetic angular momentum arising from a
solenoidal field. The beam is chosen to have no canonical angular momentum and
an optical beta function that is flat in the solenoid.

See MUCOOL note 71 for more details on this parameterisation.
"""

        z_list = range(int(self.z_pos), int(abs(self.z_pos)), 100)
        ellipse_list = [self.transport_covariance_matrix(z) for z in z_list]
        beta_list = [self.get_beta(ellipse) for ellipse in ellipse_list]
        hist, graph = xboa.Common.make_root_graph("beta vs z",
                                              z_list, "z [mm]",
                                              beta_list, "#beta_{#perp}   [mm]",
                                              ymin=0.,
                                              ymax=1000.)
        hist.GetYaxis().SetTitleOffset(1.5)
        if canvas == None:
            canvas = xboa.Common.make_root_canvas("beta")
            hist.Draw()
        canvas.cd()
        graph.Draw()
        graph.SetLineColor(line_color)
        canvas.Update()
        canvas.Print('beta_vs_z.png')
        return canvas

    def transport_covariance_matrix(self, z_position):
        """
        Transport the covariance matrix to nearest virtual plane to z_position
        """
        initial_bz = maus_cpp.field.get_field_value(0, 0, self.z_pos, 0)[2]
        # beta function for constant bz solenoid and dbeta/dz = 0.
        initial_beta = self.mom/initial_bz*2./xboa.Common.constants['c_light']
        ellipse_start = maus_cpp.covariance_matrix.create_from_penn_parameters(
             mass = self.mass,
             momentum = self.mom,
             emittance_t = 1.,
             beta_t = initial_beta,
             emittance_l = 1.,
             beta_l = 1.,
             bz = initial_bz,
        )
        return self.optics_model.transport_covariance_matrix(ellipse_start,
                                                             z_position)

def plot_field():
    """
    Plot the field on-axis
    """
    z_list = range(-5000, 5000, 50)
    bz_list = [maus_cpp.field.get_field_value(0, 0, z, 0)[2]*1e3 \
                                                            for z in z_list]
    hist, graph = xboa.Common.make_root_graph("bz on axis",
                                              z_list, "z [mm]",
                                              bz_list, "B_{z} [T]")
    canvas = xboa.Common.make_root_canvas("bz")
    hist.Draw()
    graph.Draw()
    canvas.Update()
    canvas.Print('bz_vs_z.png')
    return canvas, hist, graph

def main():
    """
    Make a plot of z, bz
    """
    print "Welcome to MAUS optics example"
    lattice_file = os.path.expandvars(
                "${MAUS_ROOT_DIR}/bin/examples/optics_examples/Stage4.Coils.dat"
              )
    lattice = Lattice(200., -2900., lattice_file)
    lattice.setup_lattice()
    print """
We plot Bz as a function of z along the axis. The maus_cpp.field module is used
to access the field map. In solenoids, optics can be derived from Bz(r=0, z) and
its derivatives - so it is interesting to see what is going on here.
"""
    plot_field()
    lattice.plot_beta()
    print "Press <Enter> to finish"
    raw_input()

if __name__ == "__main__":
    main()

