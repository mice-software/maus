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
        ROOT.gROOT.SetBatch(True)
        self.canvas = ROOT.TCanvas("tof", "tof")
        self.root_file = ROOT.TFile("tofplot.root","update")
        self.tof_hist = ROOT.TH1F("tof_3505", "tof_3505", 300, 23, 53)
        # times are in ns, constants are in ps
        self.tof0_dt = ROOT.TH1F("tof0_dt", "tof0_dt", 200, -2, 2)
        self.tof1_dt = ROOT.TH1F("tof1_dt", "tof1_dt", 200, -2, 2)
        self.tof2_dt = ROOT.TH1F("tof2_dt", "tof2_dt", 200, -2, 2)

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
                        self.tof_hist.Fill(t_1-t_0)
                        delta_t0 = self._sp_tof0[i][0]["dt"]
                        self.tof0_dt.Fill(delta_t0)
                        delta_t1 = self._sp_tof1[i][0]["dt"]
                        self.tof1_dt.Fill(delta_t1)
                        print t_1 - t_0
            for i in range(len(self._sp_tof2)):
                if self._sp_tof2[i] :
                    if len(self._sp_tof2[i])==1 :
                        delta_t2 = self._sp_tof2[i][0]["dt"]
                        self.tof2_dt.Fill(delta_t2)

        self.tof_hist.Draw()
        self.tof0_dt.Draw()
        self.tof1_dt.Draw()
        self.tof2_dt.Draw()
        self.canvas.Update()

        return json_spill_doc

    def death(self):
        """ save the plot """
        try:
            self.tof_hist.Draw()
            self.tof0_dt.Draw()
            self.tof1_dt.Draw()
            self.tof2_dt.Draw()
            self.tof_hist.Write()
            self.tof0_dt.Write()
            self.tof1_dt.Write()
            self.tof2_dt.Write()
            self.canvas.Update()
            #self.canvas.Print('tof.png')
            self.root_file.Close()
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
