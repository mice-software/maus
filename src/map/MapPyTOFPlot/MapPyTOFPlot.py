#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

""" Class for ploting the time-of-flight spectrum"""

import json
import ROOT
import ErrorHandler

class MapPyTOFPlot:
    """ Class for ploting the time-of-flight spectrum"""
    def __init__(self):
        """ Constructor """
        self._sp_tof0 = {}      
        self._sp_tof1 = {}
        self._sp_tof2 = {}
        self.canvas = ROOT.TCanvas("tof", "tof")
        self.tof_hist = ROOT.TH1F("tof", "tof", 280, 24, 38)
        
    def birth(self, json_configuration):
        """ Do nothing here """
        try:
            config_doc = json.loads(json_configuration)
            if config_doc:
                return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)

        return False

    def process(self, json_spill_doc):
        """Process the JSON data"""
        if self.get_space_points(json_spill_doc):
            for i in range(len(self._sp_tof1)):
                if self._sp_tof0[i] and self._sp_tof1[i] :
                    if len(self._sp_tof0[i])==1 and len(self._sp_tof1[i])==1:
                        t_0 = self._sp_tof0[i][0]["time"]
                        t_1 = self._sp_tof1[i][0]["time"]
                        #delta = self._sp_tof0[i][0]["dt"]
                        self.tof_hist.Fill(t_1-t_0)
                        print t_1 - t_0

        self.tof_hist.Draw()
        self.canvas.Update()

        return json_spill_doc

    def death(self):
        """ save the plot """
        try:
            self.tof_hist.Draw()
            self.canvas.Update()
            self.canvas.Print('tof.png')
            return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
            return False

    def get_space_points(self, doc):
        """ get the TOF space points """
        try:
            spill = json.loads(doc)
            if 'space_points' not in spill:
                return False

            if spill['space_points'] is None:
                return False

            space_points = spill['space_points']

            if 'tof0' not in space_points:
                return False
            self._sp_tof0 = space_points['tof0']

            if 'tof1' not in space_points:
                return False
            self._sp_tof1 = space_points['tof1']

            if 'tof2' not in space_points:
                return False
            self._sp_tof2 = space_points['tof2']

        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
            
        return True
