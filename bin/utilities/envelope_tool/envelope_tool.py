#!/usr/bin/env python

import time
import json
import os
import sys
import math
import numpy
import ROOT

import gui.window
from gui.window import Window

import xboa.Hit
from xboa.Hit import Hit
import xboa.Common as Common

import Configuration

import maus_cpp.field
import maus_cpp.globals
import maus_cpp.covariance_matrix
import maus_cpp.optics_model

class Lattice:
    def __init__(self):
        self.ref = None
        self.ellipse = None
        self.lattice = None
        self.mice_modules = None
        self.optics_model = None
        self.config = None
        self.z_list = []
        self._set_defaults()
        self.run_lattice()

    def run_lattice(self):
        """
        Setup the lattice for optics work
          - initial_momentum: beam momentum at lattice start
          - z_start: start position for tracking/beam propagation
        """
        self.config["simulation_reference_particle"] = {
          "random_seed": 0,
          "energy":self.ref["energy"],
          "particle_id":self.ref["pid"],
          "time":self.ref["t"],
          "position":{"x":self.ref["x"], "y":self.ref["y"], "z":self.ref["z"]},
          "momentum":{"x":self.ref["px"], "y":self.ref["py"], "z":self.ref["pz"]}
        }
        config_str = json.dumps(self.config)
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(config_str)
        maus_cpp.globals.set_monte_carlo_mice_modules(self.mice_modules)
        self.optics_model = maus_cpp.optics_model.OpticsModel()
        refs = []
        ellipses = []
        for z_pos in self.z_list:
            ellipses.append(self.optics_model.transport_covariance_matrix(
                                          self.ellipse,
                                          z_pos))
            refs.append(self.ref.deepcopy())
            refs[-1]['z'] = z_pos
        return refs, ellipses

    def _set_defaults(self):
        config_str = Configuration.Configuration().getConfigJSON(command_line_args=True)
        self.config = json.loads(config_str)
        ref_json = self.config["simulation_reference_particle"]
        self.ref = Hit.new_from_maus_object("maus_primary", ref_json, 0)
        ell_data = [[float(i == j) for i in range(6)] for j in range(6)]
        np_diag = numpy.array(ell_data)
        self.ellipse = maus_cpp.covariance_matrix.create_from_matrix(np_diag)
        self.lattice = self.config["simulation_geometry_filename"]
        self.mice_modules = maus_cpp.mice_module.MiceModule(self.lattice)
        self.z_list = self._setup_geometry(50., 10000.)

    def get_field_list(self):
        return self._get_fields_recursive(self.mice_modules, [])

    def set_fields(self, field_list):
        print field_list
        field_dict = {}
        for field in field_list:
            field_dict[field[0]] = field[2]
        self.mice_modules = self._set_fields_recursive(self.mice_modules, field_dict)
        
    def set_beam(self, reference, ellipse):
        self.ref = reference
        self.ellipse = ellipse

    def get_beam(self):
        return self.ref, self.ellipse

    def _setup_geometry(self, z_step, z_max):
        children = self.mice_modules.get_children()
        z_list = [z for z in numpy.arange(self.ref["z"], z_max+z_step/2, z_step)]
        for z_pos in z_list:
            children.append(self._virtual_plane(z_pos))
        self.mice_modules.set_children(children)
        print len(children)
        for child in children:
            try:
                print child.get_property("Position", "Hep3Vector"),
                print child.get_property("SensitiveDetector", "String")
            except KeyError:
                print
        return z_list

    def _set_fields_recursive(self, mice_mod, field_dict):
        try:
            my_name = mice_mod.get_property("FieldName", "string")
            if my_name in field_dict:
                mice_mod.set_property("ScaleFactor", "double", field_dict[my_name])
        except KeyError, ValueError:
            pass
        child_list = []
        for child in mice_mod.get_children():
            child_list.append(self._set_fields_recursive(child, field_dict))
        mice_mod.set_children(child_list)
        return mice_mod

    def _get_fields_recursive(self, mice_mod, field_list):
        try:
            my_type = mice_mod.get_property("FieldType", "string")
            my_name = mice_mod.get_property("FieldName", "string")
            my_scale_factor = mice_mod.get_property("ScaleFactor", "double")
            field_list.append((my_name, my_type, my_scale_factor))
        except KeyError, ValueError:
            pass
        for child in mice_mod.get_children():
            field_list = self._get_fields_recursive(child, field_list)
        return field_list

    @staticmethod
    def _virtual_plane(z):
        mod = maus_cpp.mice_module.MiceModule("VirtualPlane.dat").get_children()[0]
        mod.set_property("Position", "Hep3Vector", {"x":0., "y":0., "z":z})
        return mod

class TwissSetup:
    def __init__(self, beam_select, root_frame):
        self.window = Window(ROOT.gClient.GetRoot(),
                             root_frame,
                             "twiss_setup.json")
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
                             "penn_setup.json")
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
                             "beam_select.json")
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
            var_dict = self.window.get_frame_dict(var, "named_text_entry")
            var_text_entry = var_dict["text_entry"].text_entry
            var_text_entry.SetText(str(ref_hit[var]))
        pid_name = Common.pdg_pid_to_name[ref_hit["pid"]]
        var_text_entry.SetToolTipText("Particle identification (from pdg) - "+\
                                      pid_name)

class MagnetSetup:
    def __init__(self, main_window, parent):
        self.main_window = main_window
        self.parent = parent
        self.window = Window(ROOT.gClient.GetRoot(),
                             parent,
                             "magnet_setup.json",
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
                field_list_out.append((name, "", scale_factor))
            except KeyError:
                pass
        self.main_window.lattice.set_fields(field_list_out)
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
                    "name":"     No fields in lattice",
                    "label_length":30})
            return geometry_list
        for field in field_list:
            geometry_list["children"].append({
              "type":"named_text_entry",
              "default_text":str(field[2]),
              "label_length":10,
              "name":str(field[0]),
              "tool_tip":"Longitudinal beta function"
            })
        return geometry_list

class PlotSetup():
    def __init__(self, main_window, parent):
        self.main_window = main_window
        self.parent = parent
        self.options_list = {
            "type":"vertical_frame",
            "children":[
                 {"type":"horizontal_frame", "children":[]}
            ]
        }
        self.window = Window(ROOT.gClient.GetRoot(),
                             parent,
                             "plot_setup.json",
                             {"plot_setup_action":self.plot_setup_action})

        self.window.set_button_action("&Okay", self.okay_action)
        self.window.set_button_action("&Cancel", self.cancel_action)

    def okay_action(self):
        self.main_window.update_plot()
        self.window.close_window()
        self.main_window.plot_setup = None
    
    def cancel_action(self):
        self.window.close_window()
        self.main_window.plot_setup = None

    def plot_setup_action(self, item):
        for option in self.options_list["children"]:
            if len(option["children"]) == 0:
                option["children"].append({
                    "type":"drop_down",
                    "name":"variable_type",
                    "entries":self.variable_type_select,
                    "selected":0
                })
        return self.options_list

    variable_type_select = ["<Select plot variable>",
                            "mean",
                            "envelope",
                            "RMS",
                            "beta",
                            "alpha",
                            "gamma",
                            "emittance",
                            "dispersion",
                            "dispersion_prime"]
    

class MainWindow():
    def __init__(self):
        self.window = Window(ROOT.gClient.GetRoot(),
                             ROOT.gClient.GetRoot(),
                             "main_frame.json")
        self.lattice = Lattice()
        self.beam_select = None
        self.magnet_setup = None
        self.plot_setup = None
        self.window.set_button_action("&Beam Setup", self.beam_button_action)
        self.window.set_button_action("&Magnet Setup", self.magnet_button_action)
        self.window.set_button_action("&Plot Setup", self.plot_button_action)
        self.window.set_button_action("E&xit", self.exit_button_action)

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
            self.plot_setup = PlotSetup(self, self.window.main_frame)

    def exit_button_action(self):
        self.window.close_window()

    def update_plot(self):
        canvas = self.window.get_frame_dict("main_canvas", "canvas")["frame"].GetCanvas()
        canvas.cd()
        ref_list, ellipse_list = self.lattice.run_lattice()
        z_list = [ref["z"] for ref in ref_list]
        sigma_x_list = [ellipse.get_element(3, 3)**0.5 for ellipse in ellipse_list]
        hist, graph = Common.make_root_graph('sigma x', z_list, 'Z position', sigma_x_list, 'sigma(x)', ymin=0.)
        hist.Draw()
        graph.Draw()
        canvas.Update()

if __name__ == '__main__':
    try:
        main_window = MainWindow()
        while main_window.window.main_frame != None:
            time.sleep(0.1)
    except Exception:
        sys.excepthook(*sys.exc_info())
    finally:
        print "done"

