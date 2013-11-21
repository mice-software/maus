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
The envelope tool enables the user to study the effects of lattice modifications
in an interactive environment. 

GUI tools provide functionality for setting up, propagating and plotting beam
envelopes through electromagnetic lattices.
"""

import time
import os
import sys
import ROOT

import gui.gui_exception_handler
from gui.window import Window

ENV_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/bin/utilities/envelope_tool/")
sys.path.append(ENV_DIR+"lib")

# pylint: disable=F0401
import beam_setup 
from beam_setup import BeamSetup
from lattice import Lattice
from plotter import Plotter
from plot_setup import PlotSetup
from magnet_setup import MagnetSetup

SHARE_DIR = (ENV_DIR+"share/")

def set_share_dirs():
    """
    Setup share directories in submodules (used for loading aux files)
    """
    Lattice.share_dir = SHARE_DIR
    PlotSetup.share_dir = SHARE_DIR
    MagnetSetup.share_dir = SHARE_DIR
    beam_setup.SHARE_DIR = SHARE_DIR

class MainWindow():
    """
    Defines the main window for the envelope tool GUI
    """
    def __init__(self):
        """Initialise the main window"""
        self.window = Window(ROOT.gClient.GetRoot(), # pylint: disable=E1101
                             ROOT.gClient.GetRoot(), # pylint: disable=E1101
                             SHARE_DIR+"main_frame.json")
        self.lattice = Lattice()
        self.beam_setup = None
        self.magnet_setup = None
        self.plot_setup = None
        self.plot_setup_options = [{"variable_type":0,
                                    "first_var":0,
                                    "plot_apertures":True}]
        self.window.set_button_action("&Beam Setup", self.beam_button_action)
        self.window.set_button_action("&Magnet Setup",
                                                      self.magnet_button_action)
        self.window.set_button_action("&Plot Setup", self.plot_button_action)
        self.window.set_button_action("E&xit", self.exit_button_action)
        self.update_plot()

    def beam_button_action(self):
        """Handle a Beam Setup button press"""
        if self.beam_setup == None:
            self.beam_setup = BeamSetup(self, self.window.main_frame)
            ref, ellipse = self.lattice.get_beam()
            self.beam_setup.set_reference(ref)
            self.beam_setup.set_matrix(ellipse)

    def magnet_button_action(self):
        """Handle a Magnet Setup button press"""
        if self.magnet_setup == None:
            self.magnet_setup = MagnetSetup(self, self.window.main_frame)

    def plot_button_action(self):
        """Handle a Plot Setup button press"""
        if self.plot_setup == None:
            self.plot_setup = PlotSetup(self, self.window.main_frame,
                                        self.plot_setup_options)

    def exit_button_action(self):
        """Handle a Exit button press"""
        self.window.close_window()

    def update_plot(self):
        """Update the plot"""
        canvas_frame = self.window.get_frame("main_canvas", "canvas")
        canvas = canvas_frame.GetCanvas()
        Plotter(self.plot_setup_options, canvas, self.lattice.ref_list,
                self.lattice.ellipse_list, self.lattice.get_field_list())

def main():
    """
    The main function - build a MainWindow and poll to see if it has been closed
    """
    set_share_dirs()
    try:
        main_window = MainWindow()
        gui.gui_exception_handler.set_error_level("exceptions")
        while main_window.window.main_frame != None:
            time.sleep(1)
    except KeyboardInterrupt:
        print "Pressed Ctrl-C"
    finally:
        print "done"
        return 0 # pylint: disable=W0150

if __name__ == '__main__':
    main()
