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
GUI handler for setting up fields (primarily magnets)
"""

import ROOT

from gui.window import Window

class MagnetSetup:
    """
    GUI handler for field setup
    """
    def __init__(self, main_window, parent):
        """Initialise the GUI window"""
        self.main_window = main_window
        self.parent = parent
        manipulators = {"magnet_setup_action":self.magnet_setup_action}
        self.window = Window(ROOT.gClient.GetRoot(), # pylint: disable = E1101
                             parent,
                             data_file = self.share_dir+"magnet_setup.json",
                             manipulator_dict = manipulators)
        self.window.set_button_action("&Okay", self.okay_action)
        self.window.set_button_action("&Cancel", self.cancel_action)

    def okay_action(self):
        """Handle okay button press"""
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
        """Handle cancel button press"""
        self.window.close_window()
        self.main_window.magnet_setup = None

    def magnet_setup_action(self, item): # pylint: disable=W0613
        """Iterate over magnets and add a line per magnet (special)"""
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

    share_dir = ""

