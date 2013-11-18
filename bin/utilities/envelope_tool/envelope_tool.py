#!/usr/bin/env python

import time
import json
import os
import sys
import math
import copy
import numpy
import ROOT

import gui.window
from gui.window import Window

import xboa.Hit
from xboa.Hit import Hit
import xboa.Bunch
from xboa.Bunch import Bunch
import xboa.Common as Common

import Configuration

import maus_cpp.field
import maus_cpp.globals
import maus_cpp.simulation
import maus_cpp.covariance_matrix
import maus_cpp.optics_model

SHARE_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/bin/utilities/envelope_tool/share/")

class Lattice:
    def __init__(self):
        self.lattice = None
        self.mice_modules = None
        self.optics_model = None
        self.config = None
        self.ref_list = []
        self.ellipse_list = []
        self._set_defaults()
        self.run_lattice()

    def run_lattice(self):
        """
        Setup the lattice for optics work
          - initial_momentum: beam momentum at lattice start
          - z_start: start position for tracking/beam propagation
        """
        self.config["simulation_reference_particle"] = \
                              self.ref_list[0].get_maus_dict('maus_primary')[0]
        self.config["simulation_reference_particle"]["random_seed"] = 0
        self.config["physics_processes"] = "mean_energy_loss"
        config_str = json.dumps(self.config)
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(config_str)
        maus_cpp.globals.set_monte_carlo_mice_modules(self.mice_modules)
        self._tracking()

    def _set_defaults(self):
        config_str = Configuration.Configuration().getConfigJSON(command_line_args=True)
        self.config = json.loads(config_str)
        ref_json = self.config["simulation_reference_particle"]
        self.ref_list = [Hit.new_from_maus_object("maus_primary", ref_json, 0)]
        ell_data = [[float(i == j) for i in range(6)] for j in range(6)]
        np_diag = numpy.array(ell_data)
        self.ellipse_list = [maus_cpp.covariance_matrix.create_from_matrix(np_diag)]
        self.lattice = self.config["simulation_geometry_filename"]
        self.mice_modules = maus_cpp.mice_module.MiceModule(self.lattice)
        children = self.mice_modules.get_children()
        children.append(self._virtual_plane(self.ref_list[0]['z']))
        self.mice_modules.set_children(children)

    def get_field_list(self):
        return self._get_fields_recursive(self.mice_modules, [])

    def set_fields(self, field_list):
        field_dict = {}
        for field in field_list:
            field_dict[field["field_name"]] = field["scale_factor"]
        self.mice_modules = self._set_fields_recursive(self.mice_modules, field_dict)
        
    def set_beam(self, reference, ellipse):
        self.ref_list = [reference]
        self.ellipse_list = [ellipse]
        print self.ref_list
        print self.ellipse_list


    def get_beam(self):
        return self.ref_list[0], self.ellipse_list[0]

    def _set_fields_recursive(self, mice_mod, field_dict):
        try:
            my_name = mice_mod.get_property("FieldName", "string")
            if my_name in field_dict:
                scale = field_dict[my_name]
                mice_mod.set_property("ScaleFactor", "double", scale)
        except KeyError, ValueError:
            pass
        child_list = []
        for child in mice_mod.get_children():
            child_list.append(self._set_fields_recursive(child, field_dict))
        mice_mod.set_children(child_list)
        return mice_mod

    def _get_fields_recursive(self, mice_mod, field_list):
        try:
            my_props = {}
            for key in self.property_dict:
                my_props[key] = mice_mod.get_property(*self.property_dict[key])
            field_list.append(my_props)
        except KeyError, ValueError:
            pass
        for child in mice_mod.get_children():
            field_list = self._get_fields_recursive(child, field_list)
        return field_list

    @staticmethod
    def _virtual_plane(z):
        mod = maus_cpp.mice_module.MiceModule(SHARE_DIR+"VirtualPlane.dat").get_children()[0]
        mod.set_property("Position", "Hep3Vector", {"x":0., "y":0., "z":z})
        return mod

    def _tracking(self):
        mc_event = [{"primary":self.ref_list[0].get_maus_dict('maus_primary')[0]}]
        mc_event[0]["primary"]["random_seed"] = 0
        mc_event = maus_cpp.simulation.track_particles(json.dumps(mc_event))
        mc_event = json.loads(mc_event)
        virtual_list = mc_event[0]["virtual_hits"]
        self.ref_list = [self.ref_list[0]]+[
                      Hit.new_from_maus_object('maus_virtual_hit', virtual, 0) \
                                                   for virtual in virtual_list]
        ellipse = self.ellipse_list[0]
        try:
            self.optics_model = maus_cpp.optics_model.OpticsModel()
            self.ellipse_list = [self.ellipse_list[0]]
            for ref in self.ref_list:
                ell = self.optics_model.transport_covariance_matrix(ellipse, 
                                                                    ref['z'])
                self.ellipse_list.append(ell)
        except RuntimeError:
            pass # beam transport failed
        return self.ref_list, self.ellipse_list

    property_dict = {
        "field_name":("FieldName", "string"),
        "field_type":("FieldType", "string"),
        "scale_factor":("ScaleFactor", "double"),
        "aperture":("NominalAperture", "hep3vector"),
        "outer":("NominalOuter", "hep3vector"),
        "position":("Position", "hep3vector"),
        "rotation":("Rotation", "hep3vector")
    }

class Plotter:
    def __init__(self, plot_options, canvas, ref_list, ellipse_list, magnets):
        self.ref_list = ref_list
        self.ellipse_list = ellipse_list
        self.var_type = PlotSetup.get_variable_type(plot_options)
        self.first_var = PlotSetup.get_first_var(plot_options)
        self.x_var = []
        self.y_var = []
        self.polylines = []
        self.magnets = magnets
        try:
            if self.var_type != "mean" and len(self.ellipse_list) <= 1:
                print "Failed to propagate ellipses"
            else:
                self.set_variables_function()()
        except KeyError:
            print "Did not recognise plot option "+str(self.var_type)
        if plot_options[0]["plot_apertures"]:
            self.get_apertures()
        plot_name = self.var_type+":"+self.first_var
        canvas.cd()
        hist, graph_list = Common.make_root_multigraph(plot_name,
                                    self.x_var, "z [mm]", self.y_var, plot_name)
        hist.Draw()
        for graph in graph_list:
            graph.SetLineColor(1)
            graph.Draw('l')
        canvas.Update()

    def get_apertures(self):
        if len(self.magnets) == 0:
            return
        axis = self._axis()
        magnet_x, magnet_y = [], []
        for a_magnet in self.magnets:
            x_list = [
                a_magnet["aperture"][axis],
                a_magnet["aperture"][axis],
                a_magnet["outer"][axis],
                a_magnet["outer"][axis],
                a_magnet["aperture"][axis],
            ]
            if self.position_is_start_dict[a_magnet["field_type"]]:
                z_list = [
                    0.,
                    +a_magnet["aperture"]["z"],
                    +a_magnet["outer"]["z"],
                    0.,
                    0.,
                ]
            else:
                z_list = [
                    -a_magnet["aperture"]["z"]/2.,
                    +a_magnet["aperture"]["z"]/2.,
                    +a_magnet["outer"]["z"]/2.,
                    -a_magnet["outer"]["z"]/2.,
                    -a_magnet["aperture"]["z"]/2.,
                ]
            if self._show_physical_apertures(): # as in physical scalings
                z_pair, x_pair = self._transform(z_list, x_list, a_magnet)
                magnet_x += z_pair
                magnet_y += x_pair
            else:
                z_list = [z_pos+a_magnet["position"]["z"] for z_pos in z_list]
                magnet_x.append(z_list)
                magnet_y.append(x_list)
        if not self._show_physical_apertures(): # non-physical scalings
            graph_max = max([max(y_temp) for y_temp in self.y_var])
            graph_min = min([min(y_temp) for y_temp in self.y_var])
            graph_delta = max([max(y_temp) for y_temp in self.y_var])-\
                          graph_min
            if graph_delta == 0.:
                graph_delta = 1.
            field_min = min([a_magnet["aperture"][axis] for a_magnet in self.magnets])
            field_max = max([a_magnet["outer"][axis] for a_magnet in self.magnets])
            field_delta = field_max-field_min
            if field_delta == 0.:
                field_delta = 1.
            scaling = self.scale_factor*graph_delta/field_delta
            offset = (graph_max-field_min*scaling)+graph_delta*self.stay_clear
            for this_magnet_y in magnet_y:
                for i in range(5):
                    this_magnet_y[i] = this_magnet_y[i]*scaling+offset
        self.x_var += magnet_x
        self.y_var += magnet_y

    def _axis(self):
        if self.first_var in ["x", "y"]:
            return self.first_var
        else:
            return "x"

    def _transform(self, points_x, points_y, a_magnet):
        axis = self._axis()
        phi = a_magnet["rotation"][{"x":"y", "y":"x"}[axis]]
        translation = [a_magnet["position"]["z"], a_magnet["position"][axis]]
        list_points_x = [points_x, copy.deepcopy(points_x)]
        list_points_y = [points_y, copy.deepcopy(points_y)]
        list_points_y[1] = [-y for y in list_points_y[1]]
        for i in range(2):
            for j in range(len(points_x)):
                x_temp = list_points_x[i][j]*math.cos(phi)+list_points_y[i][j]*math.sin(phi)
                y_temp = list_points_y[i][j]*math.cos(phi)-list_points_x[i][j]*math.sin(phi)
                list_points_x[i][j] = x_temp+translation[0]
                list_points_y[i][j] = y_temp+translation[1]
        return list_points_x, list_points_y

    def get_means(self):
        self.y_var = [[reference[self.first_var] for reference in self.ref_list]]
        self.x_var = [[reference['z'] for reference in self.ref_list]]

    def get_rms(self):
        my_var = self.ellipse_var.index(self.first_var)+1
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[ell.get_element(my_var, my_var)**0.5 \
                                                  for ell in self.ellipse_list]]

    def get_envelope(self):
        self.get_rms()
        self.x_var *= 2
        rms_list = self.y_var[0]
        self.y_var = [range(len(rms_list)), range(len(rms_list))]
        for i, item in enumerate(rms_list):
            if i < len(self.ref_list):
                ref_var = self.ref_list[i][self.first_var]
                self.y_var[0][i] = ref_var-rms_list[i]
                self.y_var[1][i] = ref_var+rms_list[i]

    def get_beta(self):
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            beta = 0
            axes = self.axis_dict[self.first_var]
            axes = [self.ellipse_var.index(var)+1 for var in axes]
            for axis in axes:
                beta += ell.get_element(axis, axis)/float(len(axes))
            beta /= ref['mass']*self._get_emittance(ref, ell)/ref['p']
            self.y_var[0].append(beta)

    def get_alpha(self):
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            alpha = 0
            axes = self.axis_dict[self.first_var]
            axes = [self.ellipse_var.index(var)+1 for var in axes]
            for axis in axes:
                alpha -= ell.get_element(axis+1, axis)/float(len(axes))
            alpha /= ref['mass']*self._get_emittance(ref, ell)
            self.y_var[0].append(alpha)

    def get_gamma(self):
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            gamma = 0
            axes = self.axis_dict[self.first_var]
            axes = [self.ellipse_var.index(var)+1 for var in axes]
            for axis in axes:
                gamma -= ell.get_element(axis+1, axis+1)/float(len(axes))
            gamma /= ref['mass']*self._get_emittance(ref, ell)*ref['p']
            self.y_var[0].append(gamma)

    def get_emittance(self):
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            self.y_var[0].append(self._get_emittance(self.ref_list[i], ell))

    def get_dispersion(self):
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            energy = self.ref_list[i]['energy']
            axis_str = self.axis_dict[self.first_var][0]
            axis_int = self.ellipse_var.index(axis_str)+1
            disp = ell.get_element(2, axis_int)*energy/ell.get_element(2, 2)
            self.y_var[0].append(disp)

    def get_dispersion_prime(self):
        self.x_var = [[ref['z'] for ref in self.ref_list]]
        self.y_var = [[]]
        for i, ell in enumerate(self.ellipse_list):
            energy = self.ref_list[i]['energy']
            axis_str = self.axis_dict[self.first_var][0]
            axis_int = self.ellipse_var.index(axis_str)+2
            disp_p = ell.get_element(2, axis_int)*energy/ell.get_element(2, 2)
            self.y_var[0].append(disp_p)

    def _get_emittance(self, ref, ellipse):
        matrix = []
        axis_list = self.axis_dict[self.first_var]
        el_list = self.el_dict[self.first_var]
        bunch = Bunch.new_from_hits([ref])
        cov_matrix = [None]*len(el_list)
        for cov_i, ell_i in enumerate(el_list):
            cov_matrix[cov_i] = [None]*len(el_list)
            for cov_j, ell_j in enumerate(el_list):
                cov_matrix[cov_i][cov_j] = ellipse.get_element(ell_i, ell_j)
        cov_matrix = numpy.array(cov_matrix)
        return bunch.get_emittance(axis_list, cov_matrix)

    def _show_physical_apertures(self):
        return self.var_type in ["envelope", "mean", "<Select plot type>"] and \
               self.first_var in ["x", "y", ""]

    def set_variables_function(self):
        plot_options_dict = {
            "mean":self.get_means,
            "envelope":self.get_envelope,
            "RMS":self.get_rms,
            "beta":self.get_beta,
            "alpha":self.get_alpha,
            "gamma":self.get_gamma,
            "emittance":self.get_emittance,
            "dispersion":self.get_dispersion,
            "dispersion_prime":self.get_dispersion_prime
        }
        return plot_options_dict[self.var_type]

    ellipse_var = ['t', 'energy', 'x', 'px', 'y', 'py']
    axis_dict = {'x':['x'], 'y':['y'], 'transverse':['x', 'y'],
                 'longitudinal':['t']}
    el_dict = {'x':range(3, 5), 'y':range(5, 7), 'transverse':range(3, 7),
               'longitudinal':range(1, 3)}
    scale_factor = 0.2
    stay_clear = 0.05

    position_is_start_dict = {
        "Multipole":True,
        "Solenoid":False
    }

class TwissSetup:
    def __init__(self, beam_select, root_frame):
        self.window = Window(ROOT.gClient.GetRoot(),
                             root_frame,
                             SHARE_DIR+"twiss_setup.json")
        self.beam_select = beam_select
        self.window.set_button_action("&Okay", self.okay_action)
        self.window.set_button_action("&Cancel", self.cancel_action)

    def okay_action(self):
        ref = self.beam_select.get_reference()
        get_dict = {}
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
          dispersion_prime_x = self.window.get_text_entry("disp_prime_x", type(1.)),
          dispersion_y = self.window.get_text_entry("disp_y", type(1.)),
          dispersion_prime_y = self.window.get_text_entry("disp_prime_y", type(1.))
        )
        self.beam_select.set_matrix(matrix)
        self.window.close_window()
        self.beam_select.matrix_select = None

    def cancel_action(self):
        self.window.close_window()
        self.beam_select.matrix_select = None

class PennSetup:
    def __init__(self, beam_select, root_frame):
        self.window = Window(ROOT.gClient.GetRoot(),
                             root_frame,
                             SHARE_DIR+"penn_setup.json")
        self.beam_select = beam_select
        self.window.set_button_action("&Okay", self.okay_action)
        self.window.set_button_action("&Cancel", self.cancel_action)
        self.window.set_button_action("Get &B0", self.get_b0_action)
        self.window.set_button_action("&Constant Beta",
                                      self.constant_beta_action)

    def okay_action(self):
        ref = self.beam_select.get_reference()
        get_dict = {}
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
          bz = -self.window.get_text_entry("B0", type(1.))*Common.units['T'], # BUG - multiply by -1 to fix BUG in source code
          ltwiddle = self.window.get_text_entry("Lc", type(1.)),
          dispersion_x = self.window.get_text_entry("disp_x", type(1.)),
          dispersion_prime_x = self.window.get_text_entry("disp_prime_x", type(1.)),
          dispersion_y = self.window.get_text_entry("disp_y", type(1.)),
          dispersion_prime_y = self.window.get_text_entry("disp_prime_y", type(1.))
        )
        self.beam_select.set_matrix(matrix)
        self.window.close_window()
        self.beam_select.matrix_select = None

    def cancel_action(self):
        self.window.close_window()
        self.beam_select.matrix_select = None

    def get_b0_action(self):
        ref = self.beam_select.get_reference()
        x, y, z, t = ref['x'], ref['y'], ref['z'], ref['t']
        b_vec = maus_cpp.field.get_field_value(x, y, z, t)
        b0 = (b_vec[0]**2+b_vec[1]**2+b_vec[2]**2)**0.5
        self.window.set_text_entry('B0', b0*Common.units['T'])

    def constant_beta_action(self):
        b0 = self.window.get_text_entry("B0", type(1.))*1e-3 # kT
        if abs(b0) < 1e-9:
            raise ValueError(
              "Constant beta requires B0 is not 0. "+\
              "Modify the B0 field and try again."
            )
        lc = self.window.get_text_entry("Lc", type(1.)) # kT
        mom = self.beam_select.get_reference()['p'] # MeV/c
        charge = abs(self.beam_select.get_reference()['charge']) # eplus
        beta = (1+lc**2)**0.5 * 2.*mom/Common.constants['c_light']/b0 # mm
        self.window.set_text_entry('alpha_trans', 0.0)
        self.window.set_text_entry('beta_trans', beta)


class BeamSelect:
    def __init__(self, main_window, parent):
        self.window = Window(ROOT.gClient.GetRoot(),
                             parent,
                             SHARE_DIR+"beam_select.json")
        self.main_window = main_window
        self.parent = parent
        self.matrix_select = None
        self.matrix = None

        self.window.set_button_action("&Penn", self.penn_action)
        self.window.set_button_action("&Twiss", self.twiss_action)
        self.window.set_button_action("&Okay", self.okay_action)
        self.window.set_button_action("&Cancel", self.cancel_action)

    def twiss_action(self):
        if self.matrix_select == None:
            self.matrix_select = TwissSetup(self, self.parent)

    def penn_action(self):
        if self.matrix_select == None:
            self.matrix_select = PennSetup(self, self.parent)

    def okay_action(self):
        if self.matrix_select != None:
            print "Close the matrix_select window"
            return
        self.main_window.lattice.set_beam(self.get_reference(),
                                          self.get_matrix())
        self.main_window.lattice.run_lattice()
        self.main_window.update_plot()
        self.window.close_window()
        self.main_window.beam_select = None
    
    def cancel_action(self):
        if self.matrix_select != None:
            print "Close the matrix_select window"
            return
        self.window.close_window()
        self.main_window.beam_select = None

    def set_matrix(self, matrix):
        self.matrix = matrix
        for i, var_i in enumerate(["t", "e", "x", "px", "y", "py"]):
            for j, var_j in enumerate(["t", "e", "x", "px", "y", "py"]):
                value = matrix.get_element(i+1, j+1)
                self.window.set_text_entry(var_i+var_j, value)

    def get_matrix(self):
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

    def get_reference(self):
        ref_dict = {}
        for var in ["x", "y", "z", "px", "py", "pz"]: 
            var_dict = self.window.get_frame_dict(var, "named_text_entry")
            ref_dict[var] = float(var_dict["text_entry"].text_entry.GetText())
        pid_dict = self.window.get_frame_dict("pid", "named_text_entry")
        ref_dict["pid"] = int(pid_dict["text_entry"].text_entry.GetText())
        ref_dict["mass"] = Common.pdg_pid_to_mass[abs(ref_dict["pid"])]
        ref_dict["charge"] = Common.pdg_pid_to_charge[abs(ref_dict["pid"])]
        return Hit.new_from_dict(ref_dict, "energy")

    def set_reference(self, ref_hit):
        for var in ["x", "y", "z", "px", "py", "pz", "pid"]:
            self.window.set_text_entry(var, ref_hit[var])
 
class MagnetSetup:
    def __init__(self, main_window, parent):
        self.main_window = main_window
        self.parent = parent
        self.window = Window(ROOT.gClient.GetRoot(),
                             parent,
                             SHARE_DIR+"magnet_setup.json",
                             {"magnet_setup_action":self.magnet_setup_action})
        self.window.set_button_action("&Okay", self.okay_action)
        self.window.set_button_action("&Cancel", self.cancel_action)

    def okay_action(self):
        field_list_out = []
        field_frame = self.window.get_frame_dict("field_list", "vertical_frame")
        for frame in field_frame["children"]:
            try:
                name = frame["name"]
                scale_factor = float(frame["text_entry"].text_entry.GetText())
                field_list_out.append({"field_name":name,
                                       "scale_factor":scale_factor})
            except KeyError:
                pass
        self.main_window.lattice.set_fields(field_list_out)
        self.main_window.lattice.run_lattice()
        self.main_window.update_plot()
        self.window.close_window()
        self.main_window.magnet_setup = None
    
    def cancel_action(self):
        self.window.close_window()
        self.main_window.magnet_setup = None

    def magnet_setup_action(self, item):
        field_list = self.main_window.lattice.get_field_list()
        geometry_list = {
        "type":"vertical_frame",
        "layout":"close",
        "name":"field_list",
        "children":[]}
        if len(field_list) == 0:
            geometry_list["children"].append({"type":"label",
                    "name":"No fields in lattice",
                    "label_length":30})
            return geometry_list
        for field in field_list:
            geometry_list["children"].append({
              "type":"named_text_entry",
              "default_text":str(field["scale_factor"]),
              "label_length":10,
              "name":str(field["field_name"]),
              "tool_tip":"Longitudinal beta function"
            })
        return geometry_list

class PlotSetup():
    def __init__(self, main_window, parent, selected):
        self.main_window = main_window
        self.parent = parent
        self.selected = selected
        self.window = Window(ROOT.gClient.GetRoot(),
                             parent,
                             SHARE_DIR+"plot_setup.json")
        for i, item in enumerate(self.selected):
            self.window.set_action("variable_type_"+str(i), "drop_down",
                                  "Selected(Int_t)", self.select_action)
            type_frame = self.window.get_frame("variable_type_"+str(i), "drop_down")
            type_frame.Select(item["variable_type"])
        self.window.set_button_action("&Okay", self.okay_action)
        self.window.set_button_action("&Cancel", self.cancel_action)

    def okay_action(self):
        type_int = self.window.get_frame_dict("variable_type_0",
                                             "drop_down")["frame"].GetSelected()
        first_int = self.window.get_frame_dict("first_var_0",
                                            "drop_down")["frame"].GetSelected()
        plot_apertures = self.window.get_frame("plot_apertures", "check_button").IsOn()
        self.main_window.plot_setup_options = [{
            "variable_type":type_int,
            "first_var":first_int,
            "plot_apertures":plot_apertures
        }]
        self.main_window.update_plot()
        self.window.close_window()
        self.main_window.plot_setup = None
    
    def cancel_action(self):
        self.window.close_window()
        self.main_window.plot_setup = None

    def select_action(self):
        for i, item in enumerate(self.selected):
            type_select = self.window.get_frame_dict("variable_type_"+str(i),
                                                     "drop_down")["frame"]
            first_var_select = self.window.get_frame_dict("first_var_"+str(i),
                                                    "drop_down")["frame"]
            selected_type_int = type_select.GetSelected()
            selected_type_str = self.type_list[selected_type_int]
            var_list_name = self.type_variables_dict[selected_type_str][0]
            var_list = self.select_lists[var_list_name]
            first_var_select.RemoveAll()
            for i, entry in enumerate(var_list):
                first_var_select.AddEntry(entry, i)
            first_var_select.Select(item["first_var"])

    @staticmethod
    def get_variable_type(options):
        var_type_int = options[0]["variable_type"]
        return PlotSetup.type_list[var_type_int]

    @staticmethod
    def get_first_var(options):
        var_type_string = PlotSetup.get_variable_type(options)
        select_list_key = PlotSetup.type_variables_dict[var_type_string][0]
        select_list = PlotSetup.select_lists[select_list_key]
        first_var_int = options[0]["first_var"]
        return select_list[first_var_int]

    type_list = [
        "<Select plot type>",
        "mean",
        "envelope",
        "RMS",
        "beta",
        "alpha",
        "gamma",
        "emittance",
        "dispersion",
        "dispersion_prime"
    ]

    select_lists = {
        "no_var":[
            "",
        ], "optics_var":[
            "<Select plot variable>",
            "x",
            "y",
            "transverse",
            "longitudinal"
        ], "physics_var":[
            "<Select plot variable>"
        ]+Hit.get_variables(), "kinematic_var":[
            "<Select plot variable>",
            "x",
            "y",
            "px",
            "py",
            "t",
            "energy"
        ], "disp_var":[
            "<Select plot variable>",
            "x",
            "y"
        ]
    }

    type_variables_dict = {
        "<Select plot type>":["no_var"],
        "mean":["physics_var"],
        "envelope":["kinematic_var"],
        "RMS":["kinematic_var"],
        "beta":["optics_var"],
        "alpha":["optics_var"],
        "gamma":["optics_var"],
        "emittance":["optics_var"],
        "dispersion":["disp_var"],
        "dispersion_prime":["disp_var"],
    }

class MainWindow():
    def __init__(self):
        self.window = Window(ROOT.gClient.GetRoot(),
                             ROOT.gClient.GetRoot(),
                             SHARE_DIR+"main_frame.json")
        self.lattice = Lattice()
        self.beam_select = None
        self.magnet_setup = None
        self.plot_setup = None
        self.plot_setup_options = [{"variable_type":0, "first_var":0, "plot_apertures":True}]
        self.window.set_button_action("&Beam Setup", self.beam_button_action)
        self.window.set_button_action("&Magnet Setup", self.magnet_button_action)
        self.window.set_button_action("&Plot Setup", self.plot_button_action)
        self.window.set_button_action("E&xit", self.exit_button_action)
        self.update_plot()

    def beam_button_action(self):
        if self.beam_select == None:
            self.beam_select = BeamSelect(self, self.window.main_frame)
            ref, ellipse = self.lattice.get_beam()
            self.beam_select.set_reference(ref)
            self.beam_select.set_matrix(ellipse)

    def magnet_button_action(self):
        if self.magnet_setup == None:
            self.magnet_setup = MagnetSetup(self, self.window.main_frame)

    def plot_button_action(self):
        if self.plot_setup == None:
            self.plot_setup = PlotSetup(self, self.window.main_frame,
                                        self.plot_setup_options)

    def exit_button_action(self):
        self.window.close_window()

    def update_plot(self):
        canvas = self.window.get_frame_dict("main_canvas", "canvas")["frame"].GetCanvas()
        Plotter(self.plot_setup_options, canvas, self.lattice.ref_list, self.lattice.ellipse_list, self.lattice.get_field_list())

if __name__ == '__main__':
    try:
        main_window = MainWindow()
        while main_window.window.main_frame != None:
            time.sleep(0.1)
    except Exception:
        sys.excepthook(*sys.exc_info())
    finally:
        print "done"

