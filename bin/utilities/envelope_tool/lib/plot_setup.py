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
GUI handler for setting up a plot
"""

# Currently hands around a dictionary that defines the setup options. This needs
# to be a class
# Currently only handles one plot element; plan is to support multiple plots on
# the same canvas

import ROOT

from xboa.Hit import Hit

from gui.window import Window

class PlotSetup():
    """
    GUI handler for defining data to be plotted
    """
    def __init__(self, main_window, parent, selected):
        """
        Initialise the window
        """
        self.main_window = main_window
        self.parent = parent
        self.selected = selected
        self.window = Window(ROOT.gClient.GetRoot(), # pylint: disable = E1101
                             parent,
                             self.share_dir+"plot_setup.json")
        for i, item in enumerate(self.selected):
            self.window.set_action("variable_type_"+str(i), "drop_down",
                                  "Selected(Int_t)", self.select_action)
            type_frame = self.window.get_frame("variable_type_"+str(i),
                                               "drop_down")
            type_frame.Select(item["variable_type"])
        self.window.set_button_action("&Okay", self.okay_action)
        self.window.set_button_action("&Cancel", self.cancel_action)

    def okay_action(self):
        """
        Handle Okay button press; get the plot selection and push it to the
        main window
        """
        type_int = self.window.get_frame_dict("variable_type_0",
                                             "drop_down")["frame"].GetSelected()
        first_int = self.window.get_frame_dict("first_var_0",
                                            "drop_down")["frame"].GetSelected()
        plot_apertures = self.window.get_frame("plot_apertures",
                                               "check_button").IsOn()
        self.main_window.plot_setup_options = [{
            "variable_type":type_int,
            "first_var":first_int,
            "plot_apertures":plot_apertures
        }]
        self.main_window.update_plot()
        self.window.close_window()
        self.main_window.plot_setup = None
    
    def cancel_action(self):
        """
        Handle Cancel button press; just close the window
        """
        self.window.close_window()
        self.main_window.plot_setup = None

    def select_action(self):
        """
        Dynamically change the list of items in first_var depending on
        variable_type
        """
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
        """
        Return a string corresponding to integer variable_type selected
        """
        var_type_int = options[0]["variable_type"]
        return PlotSetup.type_list[var_type_int]

    @staticmethod
    def get_first_var(options):
        """
        Return a string corresponding to integer first_var selected
        """
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

    share_dir = ""
