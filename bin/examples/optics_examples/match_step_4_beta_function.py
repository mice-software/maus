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

#pylint: disable = W0613, E1101, R0913, R0914

"""
Example to demonstrate use of the optics routines for matching.

In this example, MAUS will build a set of solenoid field maps for MICE Step IV 
then find a "matched" solution for the field maps, given coil current limits. We
use the ROOT TMinuit routines for this.

"Matched" solution means that the beta function is flat in the upstream solenoid
and symmetric about the centre of the focus coil (flat at z=0 mm).

Two classes are defined:
  - Lattice handles the Lattice things (controls magnet currents, wraps optics
    routines)
  - LatticeOptimiser handles the optimisation
"""

import json
import os

import numpy
import ROOT

import xboa.Common

import maus_cpp.field
import maus_cpp.globals
import maus_cpp.covariance_matrix
import maus_cpp.optics_model
import Configuration

LEGEND_LIST = []
def make_root_legend(canvas, root_item_list, x_ll, y_ll, x_ur, y_ur):
    """
    Build a ROOT legend
      - canvas: the ROOT canvas
      - root_item_list: list of ROOT objects to include in the legend
      - x_ll, y_ll: lower left hand coordinate
      - x_ur, y_ur: upper right hand coordinate
    """
    canvas.cd()
    leg = ROOT.TLegend()
    leg = ROOT.TLegend(x_ll, y_ll, x_ur, y_ur)
    leg.SetEntrySeparation(0.6)
    for i, hist in enumerate(root_item_list):
        leg.AddEntry(hist, root_item_list[i].GetName())
    leg.SetFillColor(10)
    leg.SetBorderSize(0)
    leg.Draw()
    LEGEND_LIST.append(leg)
    canvas.Update()
    return leg



class Lattice:
    """
    Magnetic lattice and functions to generate and plot transfer matrices on
    said lattice
    """

    def __init__(self, momentum, z_start, coil_currents, lattice_file):
        """
        Initialises some data

        - momentum: beam momentum for the transfer matrix
        - z_start: start z position of the beam
        - coil_currents: python dictionary that maps the MiceModule name of the
                         coil to the scale factor that should be applied
        """
        self.mom = momentum
        self.z_in = z_start
        self.mass = xboa.Common.pdg_pid_to_mass[13]
        self.optics_model = None
        self.first = True
        self.coil_currents = coil_currents
        self.lattice_file = lattice_file
        self.setup_lattice(coil_currents)

    def setup_lattice(self, coil_currents):
        """
        Setup the lattice for optics work

        - coil_currents: python dictionary that maps the MiceModule name
          of the coil to the scale factor that should be applied

        Sets up data cards and loads MAUS with those data cards, then builds the
        geometry and field model, then builds the transfer matrices
        """
        config_str = Configuration.Configuration().\
                                          getConfigJSON(command_line_args=False)
        config_json = json.loads(config_str)
        config_json["simulation_reference_particle"] = {
          "random_seed": 0,
          "energy":(self.mass**2+self.mom**2)**0.5,
          "particle_id":-13,
          "time": 0.0,
          "position":{"x":0.0, "y":0.0, "z":self.z_in},
          "momentum":{"x":0.0, "y":0.0, "z":1.0}
        }
        config_json["simulation_geometry_filename"] = self.lattice_file
        config_json["verbose_level"] = 5
        config_str = json.dumps(config_json)
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(config_str)
        # now set up the mice modules
        self.set_magnet_scale_factors(coil_currents)

    def set_magnet_scale_factors(self, coil_currents):
        """
        Set magnet scale factors

        - coil_currents: python dictionary that maps the MiceModule name of the
                         coil to the scale factor that should be applied
        """
        if self.first:
            print """
Resetting the coil current densities to new values:
"""
            for key, value in coil_currents.iteritems():
                print key, value
            print """
We use the python API to MiceModules for this procedure - MiceModules are
generated from the lattice file """+self.lattice_file+""" but then we modify
the MiceModules defined therein with new values for ScaleFactor.
"""            
        self.coil_currents = coil_currents
        # first get the mice modules
        mice_modules = maus_cpp.mice_module.MiceModule(self.lattice_file)
        # get the children of the mice_modules
        # note this makes a copy (list) of the existing children on mice_modules
        children = mice_modules.get_children()
        # find the coil module and reset the scale factor property
        # note we enforce polarity; so if the scale factor was negative it
        # should stay negative
        for child in children:
            for coil_name in coil_currents.keys():
                new_scale_factor = coil_currents[coil_name]
                # force Read mode after first iteration (optimisation)
                if not self.first:
                    child.set_property("FieldMapMode", "string", "Read")
                if child.get_name().find(coil_name) > -1:
                    polarity = child.get_property("ScaleFactor", "double")
                    polarity /= abs(polarity)
                    coil_scale = abs(new_scale_factor)*polarity
                    child.set_property("ScaleFactor", "double", coil_scale)
                    #print "Setting", coil_name, "ScaleFactor to", coil_scale
        # set the mice_modules children to the new values
        mice_modules.set_children(children)
        # update the monte carlo with the new mice modules
        maus_cpp.globals.set_monte_carlo_mice_modules(mice_modules)
        # reinitialise the optics model (transfer matrices)
        self.optics_model = maus_cpp.optics_model.OpticsModel()
        self.first = False

    def plot_file(self, name):
        """
        Get the plot file name
        - name: describes what was plotted
        - image_format: format specifier e.g. png, eps, root
        Returns a string with the file name 
        """
        return name+'_p='+str(round(self.mom, 2))+\
               '_FC='+str(round(self.coil_currents["FocusCoil"], 2))+\
               '_M1='+str(round(self.coil_currents["MatchCoil1"], 2))+\
               '_M2='+str(round(self.coil_currents["MatchCoil2"], 2))+'.png'

    @staticmethod
    def plot_field(canvas=None, line_color=1, graph_name='bz on axis'):
        """
        Plot the field on-axis
        - canvas: if None, generate a new canvas. Else plot on the specified
                  canvas
        - line_color: specify a line_color for the graph
        - graph_name: specify a graph name - will show up in e.g. canvas legend
        """
        z_list = range(-5000, 5000, 50)
        bz_list = []
        for z_coord in z_list:
            bz_field = maus_cpp.field.get_field_value(0, 0, z_coord, 0)[2]*1e3
            bz_list.append(bz_field)
        hist, graph = xboa.Common.make_root_graph(graph_name,
                                                  z_list, "z [mm]",
                                                  bz_list, "B_{z} [T]")
        if canvas == None:
            canvas = xboa.Common.make_root_canvas("bz")
            hist.Draw()
        canvas.cd()
        graph.Draw()
        graph.SetLineColor(line_color)
        canvas.Update()
        return canvas, graph

    def get_beta(self, covariance_matrix):
        """
        Return the transverse beta function, given by (Var(x)+Var(y))*p/(2m)
        - covariance_matrix: use this matrix to calculate the 

        Note that for all calculations transverse emittance is 1 
        """
        return (covariance_matrix.get_element(3, 3)+\
                covariance_matrix.get_element(5, 5))*self.mom/2./self.mass

    def get_alpha(self, covariance_matrix):
        """
        Return the transverse alpha function
      
        Alpha is given by
            (Cov(x, px)+Cov(y, py))*p/(2m)

        Note that for all calculations transverse emittance is 1 
        """
        return -(covariance_matrix.get_element(3, 4)+\
                 covariance_matrix.get_element(5, 6))/2./self.mass

    def plot_beta(self, canvas=None, line_color=1, graph_name='beta on axis'):
        """
        Plot the beta function as a function of z-position
        - canvas: if None, generate a new canvas. Else plot on the specified
                  canvas
        - line_color: specify a line_color for the graph
        - graph_name: specify a graph name - will show up in e.g. canvas legend
        """
        z_list = range(int(self.z_in), int(abs(self.z_in)), 100)
        ellipse_list = [self.transport_covariance_matrix(z) for z in z_list]
        beta_list = [self.get_beta(ellipse) for ellipse in ellipse_list]
        hist, graph = xboa.Common.make_root_graph(graph_name,
                                              z_list, "z [mm]",
                                              beta_list, "#beta_{#perp}   [mm]",
                                              ymin=0., ymax=1000.)
        if canvas == None:
            canvas = xboa.Common.make_root_canvas("beta")
            hist.Draw()
        canvas.cd()
        hist.GetYaxis().SetTitleOffset(1.5)
        graph.Draw()
        graph.SetLineColor(line_color)
        canvas.Update()
        return canvas, graph

    def transport_covariance_matrix(self, z_position):
        """
        Transport the covariance matrix to nearest virtual plane to z_position
        """
        initial_bz = maus_cpp.field.get_field_value(0, 0, self.z_in, 0)[2]
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
        cov_mat = self.optics_model.transport_covariance_matrix(ellipse_start,
                                                                     z_position)
        return cov_mat

OPTIMISER = None

class LatticeOptimiser:
    """
    Wrapper for the ROOT TMinuit optimisation routines
    """

    def __init__(self, momentum, focus, match1, match2, lattice_file):
        """
        Initialise the LatticeOptimiser

        Just sets up some data
        """
        self.z_start = -2900.
        self.focus = focus
        self.match2 = match2
        self.optimiser = None
        self.lattice = Lattice(
                  momentum,
                  self.z_start,
                  {"MatchCoil1":match1, "MatchCoil2":match2, "FocusCoil":focus},
                  lattice_file
            )
        self.m1_limit = self.current_limit(281., 4830, 201.3, 46.2)
        self.m2_limit = self.current_limit(286., 3192, 199.5, 30.9)

    @staticmethod
    def current_limit(current_max, n_turns, length, radial_thickness):
        """
        Helper function to calculate the current density limit based on current
        and geometry parameters, using
        
            current density = current*turns/cross_sectional_area
        """
        current_turns = current_max*n_turns
        current_density = current_turns/length/radial_thickness
        return current_density


    def setup_minuit(self, m1_seed):
        """
        Setup minuit - set the parameters and the run function
        """
        self.optimiser = ROOT.TMinuit()
        # Set PrintLevel to -1 to make Minuit really quiet; set to 1 to make it
        # really noisy
        self.optimiser.SetPrintLevel(0)
        # We tell Minuit about coil currents for the match coils and the focus
        # coil; however, in this example we will only vary the Match coil, so
        # we fix the other two parameters. We put limits on m1 and m2 according
        # to maximum current densities with which the magnets can run
        self.optimiser.DefineParameter\
                                  (0, "MC1", m1_seed, 50., 0., self.m1_limit)
        self.optimiser.DefineParameter\
                                  (1, "MC2", self.match2, 1., 0., self.m2_limit)
        self.optimiser.DefineParameter\
                                  (2, "FC",  self.focus, 1., 0., 0.)
        self.optimiser.FixParameter(1)
        self.optimiser.FixParameter(2)
        # Because of the way Minuit works, the run_function does not know
        # anything about Lattice or LatticeOptimiser object that created it. We
        # do this by making run_function a staticmethod, i.e. it is the same for
        # every instance of LatticeOptimiser. To get at data from "self", we 
        # make a class-wide object called run_optimiser; before every 
        # optimisation attempt, we set this run_optimiser object to the 
        # LatticeOptimiser that tried to do the optimisation
        #
        # It's a bit of a fudge
        self.optimiser.SetFCN(self.run_function)

    def optimise(self):
        """
        Set up Minuit and run the code
        """
        # First set the run_optimiser object
        LatticeOptimiser.run_optimiser = self
        # We seek to run for up to (about) 500 optimisation steps; we seek to
        # optimise to Minuit score < 1e-6. In order to hand these off through
        # PyRoot, we need to wrap them in a numpy array (ROOT interprets this as
        # a C style array)
        args = numpy.array([500, 1e-6], dtype=numpy.float64)
        ierr = ROOT.Long()
        # We use SIMPLEX algorithm. Migrad is also another good one to try
        # ierr is filled with error if Minuit fails for some reason.
        #
        # At each optimisation set, SIMPLEX will call run_function(...)
        self.optimiser.mnexcm("SIMPLEX", args, 2, ierr)
        # Set it back to None - this stops someone inadvertently calling e.g.
        # run_function against the wrong object (user needs to know what he is
        # doing before calling run_function)
        LatticeOptimiser.run_optimiser = None

    @staticmethod
    def run_function(n_pars, pars, score, buff, err):
        """
        Recalculate the beam optics and compare with target value

        - n_pars: length of the parameter list
        - pars: array of parameter values (coil currents). We use the Minuit 
                GetParameter function rather than accessing this pars array
        - score: An empty float pointer; write the score (value that we are trying 
                 to minimise) to this variable. ROOT will attempt to vary the
                 parameters to make score as close to 0 as possible.
        - buff: ignored
        - err: ignored

        The run_function is executed by ROOT Minuit optimiser at each step of the
        minimisation. The algorithm is to:

        i. Reset the coil currents to values from latest Minuit step
        ii. Regenerate the beam ellipse at centre of the lattice
        iii. Check to see how close alpha is to 0 and report back to ROOT. Here we
             score using alpha^2, which is minimal when alpha is 0.
        """
        # read the coil currents from Minuit
        coils = {}
        minuit = LatticeOptimiser.run_optimiser.optimiser
        for i, name in enumerate(["MatchCoil1", "MatchCoil2", "FocusCoil"]):
            value = numpy.array((1,), dtype=numpy.float64)
            error = numpy.array((1,), dtype=numpy.float64)
            minuit.GetParameter(i, value, error)
            coils[name] = value[0]
        # hand the coil currents to the lattice object (resetting the fields)
        lattice = LatticeOptimiser.run_optimiser.lattice
        lattice.set_magnet_scale_factors(coils)
        # transport the beam ellipse
        cov_mat = lattice.transport_covariance_matrix(0.)
        # fill the score (we seek to optimise for alpha == 0, so use alpha^2)
        alpha = lattice.get_alpha(cov_mat)
        score[0] = alpha**2
        # make some print statement so we know how the optimisation is going
        print '   ',
        for coil_name, current_density in coils.iteritems():
            print coil_name, current_density,
        print '** alpha', alpha, '** score', score[0]

    run_optimiser = None

def main():
    """
    Plot beta and bz before and after matching the beamline
    """
    print """
Welcome to the MAUS optics matching example.

Setting up the lattice, as in the plot_step_4_beta_function.py example."""
    lattice_file = os.path.expandvars(
                "${MAUS_ROOT_DIR}/bin/examples/optics_examples/Stage4.Coils.dat"
              )

    lattice_optimiser = LatticeOptimiser(200., 113.95, 100., 100., lattice_file)

    print "Now plotting beta and Bz before doing any optimisation"
    bz_canvas, bz_graph_1 = lattice_optimiser.lattice.plot_field(
                                        line_color=2,
                                        graph_name='B_{z} before optimisation')
    beta_canvas, beta_graph_1 = lattice_optimiser.lattice.plot_beta(
                              line_color=2,
                              graph_name='#Beta_{#perp}   before optimisation')

    print """
Running the optimisation. We use the ROOT TMinuit optimiser which seeks to find
a minimum score value by varying a set of parameters. We vary M1 and seek to
find a minimum for alpha^2 (which should be minimal at alpha = 0).
"""
    lattice_optimiser.setup_minuit(100.)
    lattice_optimiser.optimise()

    print """
Now plotting beta and Bz following the optimisation. Beta should be symmetric
about z = 0.
"""
    bz_canvas, bz_graph_2 = lattice_optimiser.lattice.plot_field(
                                          canvas=bz_canvas,
                                          line_color=4,
                                          graph_name='B_{z} after optimisation')
    beta_canvas, beta_graph_2 = lattice_optimiser.lattice.plot_beta(
                                canvas=beta_canvas,
                                line_color=4,
                                graph_name='#Beta_{#perp}   after optimisation')
    make_root_legend(bz_canvas, [bz_graph_1, bz_graph_2], 0.15, 0.15, 0.5, 0.4)
    make_root_legend(beta_canvas, [beta_graph_1, beta_graph_2],
                                                          0.15, 0.15, 0.5, 0.35)

    lattice = lattice_optimiser.lattice
    beta_canvas.Print(lattice.plot_file('beta_vs_z'))
    bz_canvas.Print(lattice.plot_file('bz_vs_z'))
    
    ellipse_start = lattice.transport_covariance_matrix(-2900.)
    beta_start = lattice.get_beta(ellipse_start)
    ellipse_end = lattice.transport_covariance_matrix(2900.)
    beta_end = lattice_optimiser.lattice.get_beta(ellipse_end)
    print "beta at start is ", beta_start, "mm"
    print "beta at end is  ", beta_end, "mm"

    print "Finished - press <Enter> to end"
    raw_input()

if __name__ == "__main__":
    main()

