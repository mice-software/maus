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

import json
import types
import os
import ROOT

class MapPyTOFPlot:
   def birth(self, configJSON):
      #self.h1 = TH1F("tof","tof",100,20,40)
      self.TOF0_sp = {}      
      self.TOF1_sp = {}
      self.TOF2_sp = {}
      self.canvas = ROOT.TCanvas("tof", "tof")
      self.tof_hist = ROOT.TH1F("tof","tof",150,23,38)

      return True

   def process(self, str):
      if self.get_space_points(str):
         #print'tof0 : ',len(self.TOF0_sp),'  tof1 : ',len(self.TOF1_sp) ,'  tof2 : ',len(self.TOF2_sp)
         for i in range(len(self.TOF1_sp)):
            #print i, 'sp : ', self.TOF0_sp[i], self.TOF1_sp[i]
            if self.TOF0_sp[i] and self.TOF1_sp[i] :
               #print self.TOF1_sp[i]
               if len(self.TOF0_sp[i])==1 and len(self.TOF1_sp[i])==1:
                  t_0 = self.TOF0_sp[i][0]["time"]
                  t_1 = self.TOF1_sp[i][0]["time"]
                  delta = self.TOF0_sp[i][0]["dt"]
                  self.tof_hist.Fill(t_1-t_0)
                  print t_1 - t_0

      return str

   def death(self):
      try:
         self.tof_hist.Draw()
         self.canvas.Update()
         self.canvas.Print('tof.png')
         return True
      except Exception: #pylint: disable=W0703
         ErrorHandler.HandleException({}, self)
         return False


   def get_space_points(self, doc):
      spill = json.loads(doc)
      if 'space_points' not in spill:
         return False

      if spill['space_points'] is None:
         return False

      space_points = spill['space_points']

      if 'tof0' not in space_points:
         return False
      self.TOF0_sp = space_points['tof0']

      if  'tof1' not in space_points:
         return False
      self.TOF1_sp = space_points['tof1']

      if  'tof2' not in space_points:
         return False
      self.TOF2_sp = space_points['tof2']

      return True