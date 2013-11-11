#!/usr/bin/env python

import time
import json
import os
import sys
import math
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
    def __init__(self, reference_particle=None, beam_ellipse=None, lattice_file=None):
        self.ref = reference_particle
        self.ellipse = beam_ellipse
        self.lattice = lattice_file
        self.optics_model = None
        self.setup_lattice()

    def setup_lattice(self):
        """
        Setup the lattice for optics work
          - initial_momentum: beam momentum at lattice start
          - z_start: start position for tracking/beam propagation
        """
        config_str = Configuration.Configuration().getConfigJSON()
        config_json = json.loads(config_str)
        if self.ref != None:
            config_json["simulation_reference_particle"] = {
              "random_seed": 0,
              "energy":self.ref["energy"],
              "particle_id":self.ref["pid"],
              "time":self.ref["t"],
              "position":{"x":self.ref["x"], "y":self.ref["y"], "z":self.ref["z"]},
              "momentum":{"x":self.ref["px"], "y":self.ref["py"], "z":self.ref["pz"]}
            }
        if self.lattice != None:
            config_json["simulation_geometry_filename"] = self.lattice
        config_json["verbose_level"] = 5
        config_str = json.dumps(config_json)
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(config_str)

    def setup_beam(self, reference_particle, beam_ellipse):
        pass

    def setup_geometry(self, coil_currents):
        pass

class TwissSetup:
    def __init__(self, beam_select, root_frame):
        self.window = Window(ROOT.gClient.GetRoot(),
                             root_frame,
                             "twiss_setup.json")
        self.beam_select = beam_select
        self.window.set_action("&Okay", self.okay_action)
        self.window.set_action("&Cancel", self.cancel_action)

    def okay_action(self):
        self.window.main_frame.CloseWindow()
        self.beam_select.matrix_select = None

    def cancel_action(self):
        self.window.main_frame.CloseWindow()
        self.beam_select.matrix_select = None

class PennSetup:
    def __init__(self, beam_select, root_frame):
        self.window = Window(ROOT.gClient.GetRoot(),
                             root_frame,
                             "penn_setup.json")
        self.beam_select = beam_select
        self.window.set_action("&Okay", self.okay_action)
        self.window.set_action("&Cancel", self.cancel_action)
        self.window.set_action("Get &B0", self.get_b0_action)

    def okay_action(self):
        self.window.main_frame.CloseWindow()
        self.beam_select.matrix_select = None

    def cancel_action(self):
        self.window.main_frame.CloseWindow()
        self.beam_select.matrix_select = None

    def get_b0_action(self):
        ref = self.beam_select.get_reference()
        x, y, z, t = ref['x'], ref['y'], ref['z'], ref['t']
        b_vec = maus_cpp.field.get_field_value(x, y, z, t)
        b0 = (b_vec[0]**2+b_vec[1]**2+b_vec[2]**2)**0.5
        b_entry_dict = self.window.get_frame_dict('B0', 'named_text_entry')
        b_entry_dict['text_entry'].text_entry.SetText(str(b0*Common.units['T']))

class BeamSelect:
    def __init__(self, main_window, parent):
        self.window = Window(ROOT.gClient.GetRoot(),
                             parent,
                             "beam_select.json")
        self.main_window = main_window
        self.parent = parent
        self.matrix_select = None

        self.window.set_action("&Penn", self.penn_action)
        self.window.set_action("&Twiss", self.twiss_action)
        self.window.set_action("&Okay", self.okay_action)
        self.window.set_action("&Cancel", self.cancel_action)

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
        self.window.main_frame.CloseWindow()
        self.main_window.beam_select = None
    
    def cancel_action(self):
        if self.matrix_select != None:
            print "Close the matrix_select window"
            return
        self.window.main_frame.CloseWindow()
        self.main_window.beam_select = None

    def get_reference(self):
        pz_dict = self.window.get_frame_dict("pz", "named_text_entry")
        print pz_dict
        pz = float(pz_dict["text_entry"].text_entry.GetText())
        return Hit.new_from_dict({"pz":pz})

class MainWindow():
    def __init__(self):
        self.window = Window(ROOT.gClient.GetRoot(),
                             ROOT.gClient.GetRoot(),
                             "main_frame.json")
        sys.stdout.flush()
        self.beam_select = None
        self.window.set_action("&Beam Setup", self.beam_button_action)
        self.window.set_action("&Magnet Setup", self.magnet_button_action)
        self.window.set_action("E&xit", self.exit_button_action)

    def beam_button_action(self):
        print "beam button pressed ", self
        if self.beam_select == None:
            self.beam_select = BeamSelect(self, self.window.main_frame)

    def magnet_button_action(self):
        print "magnet button pressed ", self

    def exit_button_action(self):
        self.window.main_frame.CloseWindow()


class EnvelopeTool(ROOT.TGMainFrame):
    def __init__(self, parent, width, height):
        button_height = 100
        ROOT.TGMainFrame.__init__(self, parent)

        self.beam_select = None

        self.canvas = ROOT.TRootEmbeddedCanvas('canvas', self, width, height-button_height)
        self.canvas.Draw()
        self.AddFrame(self.canvas, ROOT.TGLayoutHints())
        self.buttons_frame = ROOT.TGHorizontalFrame(self, width, button_height)
        self.hints = ROOT.TGLayoutHints(ROOT.TGLayoutHints.kLHintsNormal, 10, 10, 10, 10)

        self.beam_button   = ROOT.TGTextButton(self.buttons_frame, '&Beam Setup', 20)
        self.beam_button_socket = ROOT.TPyDispatcher(self.beam_button_action)
        self.beam_button.Connect('Clicked()', 'TPyDispatcher', self.beam_button_socket, 'Dispatch()')
        self.buttons_frame.AddFrame(self.beam_button, self.hints)

        self.magnet_button   = ROOT.TGTextButton(self.buttons_frame, '&Magnet Setup', 20)
        self.magnet_button_socket = ROOT.TPyDispatcher(self.magnet_button_action)
        self.magnet_button.Connect('Clicked()', 'TPyDispatcher', self.magnet_button_socket, 'Dispatch()')
        self.buttons_frame.AddFrame(self.magnet_button, self.hints)

        self.exit_button   = ROOT.TGTextButton(self.buttons_frame, 'E&xit', 20)
        self.exit_button_socket = ROOT.TPyDispatcher(self.exit_button_action)
        self.exit_button.Connect('Clicked()', 'TPyDispatcher', self.exit_button_socket, 'Dispatch()')
        self.buttons_frame.AddFrame(self.exit_button, self.hints)

        self.AddFrame(self.buttons_frame, self.hints)

        self.SetWindowName('Envelope tool')
        self.MapSubwindows()
        self.Resize(self.GetDefaultSize())
        self.MapWindow()

    def __del__(self):
        self.Cleanup()

if __name__ == '__main__':
    try:
        lattice = Lattice()
        main_window = MainWindow()
        while main_window.window.main_frame != None:
            time.sleep(0.1)
    except Exception:
        sys.excepthook(*sys.exc_info())
    finally:
        print "done"


def penn(self):
    self.l1_penn = ROOT.TGVerticalFrame(self)
    self.l2_penn_trans = ROOT.TGHorizontalFrame(self.l1_penn)
    self.beta_t = TextEntry(self.l2_penn_trans, "beta trans", "0.0")
    self.emittance_t = TextEntry(self.l2_penn_trans, "epsilon trans", "0.0")
    self.alpha_t = TextEntry(self.l2_penn_trans, "alpha trans", "0.0")
    self.bz = TextEntry(self.l2_penn_trans, "Bz", "0.0")
    self.ltwiddle = TextEntry(self.l2_penn_trans, "Lc", "0.0")
    self.l1_penn.AddFrame(self.l2_penn_trans)
    self.l2_penn_long = ROOT.TGHorizontalFrame(self.l1_penn)
    self.beta_l = TextEntry(self.l2_penn_long, "#beta long", "0.0")
    self.emittance_l = TextEntry(self.l2_penn_long, "#epsilon long", "0.0")
    self.alpha_l = TextEntry(self.l2_penn_long, "#alpha long", "0.0")
    self.l1_penn.AddFrame(self.l2_penn_long)
    self.l2_penn_corr = ROOT.TGHorizontalFrame(self.l1_penn)
    self.disp_x = TextEntry(self.l2_penn_corr, "Dx", "0.0")
    self.disp_xp = TextEntry(self.l2_penn_corr, "Dx'", "0.0")
    self.disp_y = TextEntry(self.l2_penn_corr, "Dy", "0.0")
    self.disp_yp = TextEntry(self.l2_penn_corr, "Dy'", "0.0")
    self.l1_penn.AddFrame(self.l2_penn_corr)
    self.AddFrame(self.l1_penn, layout("normal"))


