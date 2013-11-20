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
GUI handler for setting up a beam
"""

import ROOT
from xboa.Hit import Hit

import xboa.Common as Common

import maus_cpp
import maus_cpp.field

from gui.window import Window

SHARE_DIR = ""

class TwissSetup:
    """GUI window to setup a beam according to 2D Twiss parameterisation"""
    def __init__(self, beam_setup, root_frame):
        """Initialise the window"""
        self.window = Window(ROOT.gClient.GetRoot(), # pylint: disable = E1101
                             root_frame,
                             SHARE_DIR+"twiss_setup.json")
        self.beam_setup = beam_setup
        self.window.set_button_action("&Okay", self.okay_action)
        self.window.set_button_action("&Cancel", self.cancel_action)

    def okay_action(self):
        """Handle a click on the Okay button, building the beam matrix"""
        ref = self.beam_setup.get_reference()
        matrix = maus_cpp.covariance_matrix.create_from_twiss_parameters(
          mass = ref['mass'],
          momentum = ref['p'],
          emittance_x = self.window.get_text_entry("emittance_x", type(1.)),
          beta_x = self.window.get_text_entry("beta_x", type(1.)),
          emittance_y = self.window.get_text_entry("emittance_y", type(1.)),
          beta_y = self.window.get_text_entry("beta_y", type(1.)),
          emittance_l = self.window.get_text_entry("emittance_l", type(1.)),
          beta_l = self.window.get_text_entry("beta_l", type(1.)),
          alpha_x = self.window.get_text_entry("alpha_x", type(1.)),
          alpha_y = self.window.get_text_entry("alpha_y", type(1.)),
          alpha_l = self.window.get_text_entry("alpha_l", type(1.)),
          dispersion_x = self.window.get_text_entry("disp_x", type(1.)),
          dispersion_prime_x = self.window.get_text_entry("disp_prime_x",
                                                          type(1.)),
          dispersion_y = self.window.get_text_entry("disp_y", type(1.)),
          dispersion_prime_y = self.window.get_text_entry("disp_prime_y",
                                                          type(1.))
        )
        self.beam_setup.set_matrix(matrix)
        self.window.close_window()
        self.beam_setup.matrix_select = None

    def cancel_action(self):
        """Handle a click on the Cancel button"""
        self.window.close_window()
        self.beam_setup.matrix_select = None

class PennSetup:
    """GUI window to setup a beam according to 4D Penn parameterisation"""
    def __init__(self, beam_setup, root_frame):
        """Initialise the window"""
        self.window = Window(ROOT.gClient.GetRoot(), # pylint: disable = E1101
                             root_frame,
                             SHARE_DIR+"penn_setup.json")
        self.beam_setup = beam_setup
        self.window.set_button_action("&Okay", self.okay_action)
        self.window.set_button_action("&Cancel", self.cancel_action)
        self.window.set_button_action("Get &B0", self.get_b0_action)
        self.window.set_button_action("&Constant Beta",
                                      self.constant_beta_action)

    def okay_action(self):
        """Handle a click on the Okay button, building the beam matrix"""
        ref = self.beam_setup.get_reference()
        matrix = maus_cpp.covariance_matrix.create_from_penn_parameters(
          mass = ref['mass'],
          momentum = ref['p'],
          emittance_t = self.window.get_text_entry("emittance_trans", type(1.)),
          beta_t = self.window.get_text_entry("beta_trans", type(1.)),
          emittance_l = self.window.get_text_entry("emittance_l", type(1.)),
          beta_l = self.window.get_text_entry("beta_l", type(1.)),
          alpha_t = self.window.get_text_entry("alpha_trans", type(1.)),
          alpha_l = self.window.get_text_entry("alpha_l", type(1.)),
          charge = ref["charge"],
          # BUG - multiply by -1 to fix BUG in source code
          bz = -self.window.get_text_entry("B0", type(1.))*Common.units['T'],
          ltwiddle = self.window.get_text_entry("Lc", type(1.)),
          dispersion_x = self.window.get_text_entry("disp_x", type(1.)),
          dispersion_prime_x = self.window.get_text_entry("disp_prime_x",
                                                          type(1.)),
          dispersion_y = self.window.get_text_entry("disp_y", type(1.)),
          dispersion_prime_y = self.window.get_text_entry("disp_prime_y",
                                                          type(1.))
        )
        self.beam_setup.set_matrix(matrix)
        self.window.close_window()
        self.beam_setup.matrix_select = None

    def cancel_action(self):
        """Handle a click on the Cancel button"""
        self.window.close_window()
        self.beam_setup.matrix_select = None

    def get_b0_action(self):
        """
        Handle a click on the Get B0 button, getting B magnitude at the location
        of the reference particle
        """
        ref = self.beam_setup.get_reference()
        b_vec = maus_cpp.field.get_field_value(ref['x'], ref['y'], ref['z'],
                                               ref['t'])
        _b0 = (b_vec[0]**2+b_vec[1]**2+b_vec[2]**2)**0.5
        self.window.set_text_entry('B0', _b0*Common.units['T'])

    def constant_beta_action(self):
        """
        Handle a click on the Constant Beta button, getting beta function that
        would be required to get a constant beta function, given other settings
        (reference particle, B0, Lc)
        """
        _b0 = abs(self.window.get_text_entry("B0", type(1.))*1e-3) # kT
        if abs(_b0) < 1e-9:
            raise ValueError(
              "Constant beta requires B0 is not 0. "+\
              "Modify the B0 field and try again."
            )
        _lc = self.window.get_text_entry("Lc", type(1.)) # kT
        mom = self.beam_setup.get_reference()['p'] # MeV/c
        charge = abs(self.beam_setup.get_reference()['charge']) # eplus
        beta = (1+_lc**2)**0.5 * 2.*charge*mom/Common.constants['c_light']/_b0
        self.window.set_text_entry('alpha_trans', 0.0)
        self.window.set_text_entry('beta_trans', beta) # beta in mm


class BeamSetup:
    """GUI window to setup a reference particle and beam ellipse"""
    def __init__(self, main_window, parent):
        """Initialise the window"""
        self.window = Window(ROOT.gClient.GetRoot(), # pylint: disable = E1101
                             parent,
                             SHARE_DIR+"beam_setup.json")
        self.main_window = main_window
        self.parent = parent
        self.matrix_select = None
        self.matrix = None

        self.window.set_button_action("&Penn", self.penn_action)
        self.window.set_button_action("&Twiss", self.twiss_action)
        self.window.set_button_action("&Okay", self.okay_action)
        self.window.set_button_action("&Cancel", self.cancel_action)

    def twiss_action(self):
        """Handle Twiss button - open a window to set up Twiss parameters"""
        if self.matrix_select == None:
            self.matrix_select = TwissSetup(self, self.parent)

    def penn_action(self):
        """Handle Penn button - open a window to set up Penn parameters"""
        if self.matrix_select == None:
            self.matrix_select = PennSetup(self, self.parent)

    def okay_action(self):
        """
        Handle Okay button - set the beam in lattice and then close the window
        """
        if self.matrix_select != None:
            print "Close the matrix_select window"
            return
        self.main_window.lattice.set_beam(self.get_reference(),
                                          self.get_matrix())
        self.main_window.lattice.run_lattice()
        self.main_window.update_plot()
        self.window.close_window()
        self.main_window.beam_setup = None
    
    def cancel_action(self):
        """
        Handle Cancel button - just close the window
        """
        if self.matrix_select != None:
            print "Close the matrix_select window"
            return
        self.window.close_window()
        self.main_window.beam_setup = None

    def set_matrix(self, matrix):
        """
        Set the beam matrix (updating GUI elements)
        - matrix a maus_cpp.covariance_matrix.CovarianceMatrix object
        """
        self.matrix = matrix
        for i, var_i in enumerate(["t", "e", "x", "px", "y", "py"]):
            for j, var_j in enumerate(["t", "e", "x", "px", "y", "py"]):
                value = matrix.get_element(i+1, j+1)
                self.window.set_text_entry(var_i+var_j, value)

    def get_matrix(self):
        """
        Get the beam matrix (reading from GUI elements)

        Returns a maus_cpp.covariance_matrix.CovarianceMatrix object
        """
        matrix = maus_cpp.covariance_matrix.CovarianceMatrix()
        for i, var_i in enumerate(["t", "e", "x", "px", "y", "py"]):
            for j, var_j in enumerate(["t", "e", "x", "px", "y", "py"]):
                value = self.window.get_text_entry(var_i+var_j, type(1.))
                if j > i:
                    delta = matrix.get_element(i+1, j+1) -\
                            matrix.get_element(j+1, i+1)
                    if abs(delta) > 1e-6:
                        err = "Matrix has non-symmetric element Cov("+\
                                var_i+","+var_j+") with delta "+str(delta)+"."
                        err += "This element must be symmetric - upper "+\
                                 "diagonal should be equal to lower diagonal."
                        raise ValueError(err)
                else:
                    matrix.set_element(i+1, j+1, value)
        return matrix

    def set_reference(self, ref_hit):
        """
        Get the reference particle (writing to GUI elements)
        - ref_hit an xboa.Hit.Hit object
        """
        for var in ["x", "y", "z", "px", "py", "pz", "pid"]:
            self.window.set_text_entry(var, ref_hit[var])

    def get_reference(self):
        """
        Get the reference particle (reading from GUI elements)

        Returns an xboa.Hit.Hit object
        """
        ref_dict = {}
        for var in ["x", "y", "z", "px", "py", "pz"]: 
            var_dict = self.window.get_frame_dict(var, "named_text_entry")
            ref_dict[var] = float(var_dict["text_entry"].text_entry.GetText())
        pid_dict = self.window.get_frame_dict("pid", "named_text_entry")
        ref_dict["pid"] = int(pid_dict["text_entry"].text_entry.GetText())
        ref_dict["mass"] = Common.pdg_pid_to_mass[abs(ref_dict["pid"])]
        ref_dict["charge"] = Common.pdg_pid_to_charge[abs(ref_dict["pid"])]
        return Hit.new_from_dict(ref_dict, "energy")

