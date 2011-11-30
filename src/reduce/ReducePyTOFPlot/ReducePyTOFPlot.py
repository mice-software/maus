"""
ReducePyTOFPlot fills TOF histograms for slab hits 
and space points, draws them, refreshes the canvases 
and prints to eps at the end of run.
"""
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
#
# turn off false positives related to ROOT
#pylint: disable = E1101
# messages about too many branches and too many lines??
#pylint: disable = R0912
#pylint: disable = R0915

import base64
import json
import ROOT

class ReducePyTOFPlot: # pylint: disable=R0902
    """
    ReducePyTOFPlots plots several TOF histograms for each spill
    Currently the following histograms are filled
    - x,y slab_hits from spill
    - for each slab hit, fill PMT(s) that is(are) hit 
    - number of reconstructed space points per particle event
    - 1d x and y of space points
    - 2d y vs x of space points
    
    Histograms are drawn on different canvases
    The canvases are refreshed every N spills 
     where N = refresh_rate = set via refresh_rate data card in driver script
     default refresh_rate = 5, ie every 5 spills
   
    The default is to run ROOT in interactive mode
    To run in batch mode, set root_batch_mode = 0 in the data card
    
    At the end of the run, the canvases are printed to eps files
    """

    def __init__(self): #pylint: disable=R0201
        """
        Set initial attribute values.
        @param self Object reference.
        """
        # Do initialisation specific to this class.

        # refresh_rate determines how often (in spill counts) 
        # the canvases are updated
        self.refresh_rate = 5
        
        # Set ROOT Batch Mode to be False by default
        # This can be set via root_batch_mode in the driver script
        self.root_batch_mode = 0
        
        # keep track of how many spills we process
        self.spill_count = 0

        self.filetype = "eps"

        # histogram initializations. they are defined explicitly in init_histos
        self._ht01 = None
        self._ht02 = None
        self._ht12 = None
        self.hspslabx_0 = None
        self.hspslabx_1 = None
        self.hspslabx_2 = None
        self.hspslaby_0 = None
        self.hspslaby_1 = None
        self.hspslaby_2 = None
        self.hpslabxy_0 = None
        self.hpslabxy_1 = None
        self.hpslabxy_2 = None
        self.hspxy_0 = None
        self.hspxy_1 = None
        self.hspxy_2 = None
        self.hnsp_0 = None
        self.hnsp_1 = None
        self.hnsp_2 = None
        self.hpmthits = None
        self.h2dprof = None
        self.hslabhits = None

        self.canvas_tof = None
        self.canvas_sp = None
        self.canvas_hits = None
        
 
    def birth(self, config_json):
        """
        Configure worker from data cards. Overrides super-class method. 
        @param self Object reference.
        @param config_doc JSON document.
        @returns True if successful.
        """
        config_doc = json.loads(config_json)

        # Read in configuration flags and parameters
        # these will overwrite whatever defaults were set in init
        if 'refresh_rate' in config_doc:
            self.refresh_rate = int(config_doc["refresh_rate"])

        if 'root_batch_mode' in config_doc:
            self.root_batch_mode = int(config_doc["root_batch_mode"])
             
        #initialize histograms, setup root canvases, and set root styles
        self.__init_histos()

        return True

    def process(self, json_doc):
        """
        process a spill, get slab hits via get_slab_hits
        get space points via get_space_points
        """
        try:
            spill = json.loads(json_doc)
        except ValueError:
            spill = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return json.dumps(spill)

        self.spill_count = self.spill_count + 1

        # get TOF slab hits & fill the relevant histograms
        if not self.get_slab_hits(spill): 
            if "errors" not in spill:
                spill["errors"] = {}
            spill["errors"]["no_slab_hits"] = "no slab hits"
            return json.dumps(spill)

        # get TOF space points & fill histograms
        if not self.get_space_points(spill):
            if "errors" not in spill:
                spill["errors"] = {}
            spill["errors"]["no_space_points"] = "no space points"
            return json.dumps(spill)

        # refresh canvases at requested frequency
        if self.spill_count % self.refresh_rate == 0:
            #print self.spill_count
            return self.update_histos()
        else:
            return json.dumps(spill)
          


    def get_slab_hits(self, spill):
        """ get the TOF slab hits """

        # return if we cannot find slab_hits in the spill
        if 'slab_hits' not in spill:
            return False

        slabhits = spill['slab_hits']

        # setup the detectors for which we want to look at hits
        dets = ['tof0', 'tof1', 'tof2']
                   
        # loop over detector stations ie tof0,tof1,tof2
        for index, station in enumerate(dets):
            # leave if we cannot find slab hits for this detector
            if station not in slabhits:
                return False
            dethits = slabhits[station]
            # loop over all slab hits for this detector station
            for i in range(len(dethits)):
                # make sure it is not null
                if (dethits[i]):
                    for j in range(len(dethits[i])): #loop over planes
                        pos = dethits[i][j]['slab']
                        plane_num = dethits[i][j]["plane"]
                        # make sure the plane number is valid so 
                        # we don't overflow bounds
                        if plane_num < 0 or plane_num > 1:
                            return False
                        self.hslabhits[index][plane_num].Fill(pos)
                        #plane 0, pmt0 hit for this slab
                        if ("pmt0" in dethits[i][j]):
                            self.hpmthits[index][plane_num][0].Fill(pos)
                        #plane 0, pmt1 hit for this slab
                        if ("pmt1" in dethits[i][j]):
                            self.hpmthits[index][plane_num][1].Fill(pos)
        
        return True



    def get_space_points(self, spill):
        """ get the TOF space points
        Go through the TOF0,1,2 space points
        Make sure it is not null
        Get the number of space points for each particle event
        For each space point, get the x & y
        Get the time & find the Time-of-Flight between 0->1, 0->2, 1->2,
        Fill histograms
        """

        if 'space_points' not in spill:
            return False

        # check for NoneType --there are events with no reconstructed SP
        if spill['space_points'] is None:
            return False

        space_points = spill['space_points']

        # if there are no TOF0,1,2 space point objects, return false
        # not sure if we require all 3, 
        # -- but currently returning false unless we get all 3 detectors
        if 'tof0' not in space_points:
            return False
        sp_tof0 = space_points['tof0']

        if 'tof1' not in space_points:
            return False
        sp_tof1 = space_points['tof1']

        if 'tof2' not in space_points:
            return False
        sp_tof2 = space_points['tof2']

        
        # TOF0 
        for i in range(len(sp_tof0)):
            if sp_tof0[i]:
                self.hnsp_0.Fill(len(sp_tof0[i]))
            else:
                self.hnsp_0.Fill(0)           
            if sp_tof0[i] and sp_tof1[i] :
                if len(sp_tof0[i])==1 and len(sp_tof1[i])==1:
                    t_0 = sp_tof0[i][0]["time"]
                    t_1 = sp_tof1[i][0]["time"]
                    spnt_x = sp_tof0[i][0]["slabX"]
                    spnt_y = sp_tof0[i][0]["slabY"]
                    self.hspxy_0.Fill(spnt_x, spnt_y)
                    self.hspslabx_0.Fill(spnt_x)
                    self.hspslaby_0.Fill(spnt_y)

                    
        # TOF 2
        for i in range(len(sp_tof2)):
            if sp_tof2[i]:
                self.hnsp_2.Fill(len(sp_tof2[i]))
            else:
                self.hnsp_2.Fill(0)
            if sp_tof2[i] and sp_tof1[i] :
                if len(sp_tof2[i])==1 and len(sp_tof1[i])==1:
                    t_2 = sp_tof2[i][0]["time"]
                    t_1 = sp_tof1[i][0]["time"]
                    spnt_x = sp_tof2[i][0]["slabX"]
                    spnt_y = sp_tof2[i][0]["slabY"]
                    self.hspxy_2.Fill(spnt_x, spnt_y)
                    self.hspslabx_2.Fill(spnt_x)
                    self.hspslaby_2.Fill(spnt_y)
                    self._ht12.Fill(t_2-t_1)
            if sp_tof2[i] and sp_tof0[i] :
                if len(sp_tof2[i])==1 and len(sp_tof0[i])==1:
                    t_2 = sp_tof2[i][0]["time"]
                    t_0 = sp_tof0[i][0]["time"]
                    self._ht02.Fill(t_2-t_0)

        # TOF 1
        for i in range(len(sp_tof1)):
            if sp_tof1[i]:
                self.hnsp_1.Fill(len(sp_tof1[i]))
            else:
                self.hnsp_1.Fill(0)           
            if sp_tof0[i] and sp_tof1[i] :
                if len(sp_tof0[i])==1 and len(sp_tof1[i])==1:
                    t_0 = sp_tof0[i][0]["time"]
                    t_1 = sp_tof1[i][0]["time"]
                    spnt_x = sp_tof1[i][0]["slabX"]
                    spnt_y = sp_tof1[i][0]["slabY"]
                    self._ht01.Fill(t_1-t_0)
                    self.hspxy_1.Fill(spnt_x, spnt_y)
                    self.h2dprof.Fill(spnt_x, spnt_y, len(sp_tof1), 1)
                    self.hspslabx_1.Fill(spnt_x)
                    self.hspslaby_1.Fill(spnt_y)

        return True





    def __init_histos(self): #pylint: disable=R0201
        """ 
        ROOT batch mode is set to false (i.e. interactive) by default
        unless it is set differently via the root_batch_mode data card
        """ 

        ROOT.gROOT.SetBatch(False)
        if self.root_batch_mode == 1:
            ROOT.gROOT.SetBatch(True)
        
        # have root run quietly without verbose informationals
        ROOT.gErrorIgnoreLevel = 1001
        
        # white canvas
        ROOT.gROOT.SetStyle("Plain")
        
        #turn off stat box
        ROOT.gStyle.SetOptStat(0)
        
        #sensible color palette
        ROOT.gStyle.SetPalette(1)
        
        # xy grid on canvas
        ROOT.gStyle.SetPadGridX(1)
        ROOT.gStyle.SetPadGridY(1)
        
        # define histograms
        self._ht01 = ROOT.TH1F("ht01", "TOF0->1;Time (ns);;", 200, 20, 40)
        self._ht12 = ROOT.TH1F("ht12", "TOF1->2;Time (ns);;", 200, 0, 20)
        self._ht02 = ROOT.TH1F("ht02", "TOF0->2;Time (ns);;", 200, 30, 50)
        
        
        self.hspslabx_0 = ROOT.TH1F("spx0", "SpacePoints X slabs hit;SlabX;",
                                     10, 0, 10)
        self.hspslaby_0 = ROOT.TH1F("spy0", "SpacePoints Y slabs hit;SlabY;",
                                     10, 0, 10)
        self.hspslabx_1 = ROOT.TH1F("spx1", "SpacePoints X slabs hit;SlabX;",
                                     10, 0, 10)
        self.hspslaby_1 = ROOT.TH1F("spy1", "SpacePoints Y slabs hit;SlabY;",
                                     10, 0, 10)
        self.hspslabx_1.SetFillColor(2)
        self.hspslaby_1.SetFillColor(2) 
        self.hspslabx_2 = ROOT.TH1F("spx2", "SpacePoints X slabs hit;SlabX;",
                                     10, 0, 10)
        self.hspslaby_2 = ROOT.TH1F("spy2", "SpacePoints Y slabs hit;SlabY;",
                                     10, 0, 10)
        self.hspslabx_2.SetFillColor(4)
        self.hspslaby_2.SetFillColor(4) 
        
        self.hspxy_0 = ROOT.TH2F("hspxy_0", "tof0: space points;SlabX;SlabY",
                                  10, 0, 10, 10, 0, 10)
        self.hspxy_1 = ROOT.TH2F("hspxy_1", "tof1: space points;SlabX;SlabY",
                                  10, 0, 10, 10, 0, 10)
        self.hspxy_2 = ROOT.TH2F("hspxy_2", "tof2: space points;SlabX;SlabY",
                                  10, 0, 10, 10, 0, 10)
        
        self.hnsp_0 = ROOT.TH1F("hnsp_0", ";#space points in particle event;;",
                                 4, 0., 4.)
        self.hnsp_1 = ROOT.TH1F("hnsp_1", ";#space points in particle event;;",
                                 4, 0., 4.)
        self.hnsp_1.SetFillColor(2)
        self.hnsp_2 = ROOT.TH1F("hnsp_2", ";#space points in particle event;;",
                                 4, 0., 4.)
        self.hnsp_2.SetFillColor(4)
                
        self.h2dprof = ROOT.TProfile2D("h2dprof", "Profile X Y;SlabX;SlabY",
                                        10, 0, 10, 10, 0, 10)

        self.hslabhits = [[]]
        for i in range (0, 3):
            self.hslabhits.append([])
            for j in range (0, 2):
                histname = "hslabhits%d%d" % (i, j)
                if (j == 0): 
                    title = "Raw Slab Hits X; SlabX;"
                if (j == 1): 
                    title = "Raw Slab Hits Y;SlabY;"
                self.hslabhits[i].append(ROOT.TH1F(histname,
                                                   title,
                                                   10, 0, 10))
                self.hslabhits[i][j].SetFillColor(2*i)

        self.hpmthits = [[[]]]
        for i in range (0, 3):
            self.hpmthits.append([])
            for j in range (0, 2):
                self.hpmthits[i].append([])
                for k in range (0, 2):
                    histname = "hpmthits%d%d%d" % (i, j, k)
                    title = "plane %d, pmt %d; Slab;" % (j, k)
                    self.hpmthits[i][j].append(ROOT.TH1F(histname, 
                                                         title,
                                                         10, 0, 10))
                    self.hpmthits[i][j][k].SetFillColor(2*i)


        # Create canvases
        #
        # Draw() of histos has to be done only once
        # for updating the histograms, just Modified() and Update() on canvases
        # the update/refresh is done in update_histos()
        
        # tof times between 0-1-2
        self.canvas_tof = ROOT.TCanvas("tof", "tof", 800, 800)
        self.canvas_tof.Divide(1, 3)
        self.canvas_tof.cd(1)
        self._ht01.Draw()
        self.canvas_tof.cd(2)
        self._ht12.Draw()
        self.canvas_tof.cd(3)
        self._ht02.Draw()
        
        # Space points, SP x & y 1d, SP y vs x 2d
        self.canvas_sp = ROOT.TCanvas("sp1d", "sp1d", 900, 900)
        self.canvas_sp.Divide(3, 2)
        self.canvas_sp.cd(1)
        self.hnsp_1.Draw()
        self.hnsp_0.Draw("same")
        self.hnsp_2.Draw("same")

        self.canvas_sp.cd(2)
        self.hspslabx_1.Draw()
        self.hspslabx_0.Draw("same")
        self.hspslabx_2.Draw("same")

        self.canvas_sp.cd(3)
        self.hspslaby_1.Draw()
        self.hspslaby_0.Draw("same")
        self.hspslaby_2.Draw("same")


        self.canvas_sp.cd(4)
        self.hspxy_0.Draw("text&&colz")
        self.canvas_sp.cd(5)
        self.hspxy_1.Draw("text&&colz")
        self.canvas_sp.cd(6)
        self.hspxy_2.Draw("text&&colz")
        
        # Slab Hits
        self.canvas_hits = ROOT.TCanvas("hits", "hits", 800, 800)
        self.canvas_hits.Divide(2, 3)
        self.canvas_hits.cd(1)
        self.hslabhits[1][0].Draw()
        for i in range (0, 3):
            self.hslabhits[i][0].Draw("same")

        self.canvas_hits.cd(2)
        self.hslabhits[1][1].Draw()
        for i in range (0, 3):
            self.hslabhits[i][1].Draw("same")

        # PMT Hits
        self.canvas_hits.cd(3)
        self.hpmthits[1][0][0].Draw()
        for i in range (0, 3):
            self.hpmthits[i][0][0].Draw("same")
        self.canvas_hits.cd(4)
        self.hpmthits[1][0][1].Draw()
        for i in range (0, 3):
            self.hpmthits[i][0][1].Draw("same")
        self.canvas_hits.cd(5)
        self.hpmthits[1][1][0].Draw()
        for i in range (0, 3):
            self.hpmthits[i][1][0].Draw("same")
        self.canvas_hits.cd(6)
        self.hpmthits[1][1][1].Draw()
        for i in range (0, 3):
            self.hpmthits[i][1][1].Draw("same")


        return True


    def update_histos(self):
       
        """
        Updates canvases
        This is called every @param refresh_rate spills
        Canvases are only updated - the writing is done @ death
        """
 
        self.canvas_tof.cd(1)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_tof.cd(2)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_tof.cd(3)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()

        self.canvas_sp.cd(1)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_sp.cd(2)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_sp.cd(3)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_sp.cd(4)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_sp.cd(5)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_sp.cd(6)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        
        
        self.canvas_hits.cd(1)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_hits.cd(2)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_hits.cd(3)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_hits.cd(4)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_hits.cd(5)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()
        self.canvas_hits.cd(6)
        ROOT.gPad.Modified()
        ROOT.gPad.Update()

        # How to save 3 different canvases?
        #   create a list of dictionaries?
        #   however -- OutputPyImage seems unable to recognize such a list
        # image_list = []

        filetag = "tof_times_%06d" % (self.spill_count)
        file_content = "TOF Times"
        outfile = "%s.%s" % (filetag, self.filetype)
        self.canvas_tof.Print(outfile)
        data_file = open(outfile,'r')
        data = data_file.read()
        data_bin = base64.b64encode(data)
        data_file.close()
        json_doc = {}
        json_doc["image"] = {}
        json_doc["image"]["content"] = file_content
        json_doc["image"]["tag"] = filetag
        json_doc["image"]["image_type"] = self.filetype
        json_doc["image"]["data"] = data_bin
	
	# here append the other 2 canvas images to image_list
        # but OPyIm does not like that -- seems to want just a dict
        # image_list.append(json_doc)
        # just print the eps files for now until we figure this out

        filetag = "tof_sp_%06d" % (self.spill_count)
        file_content = "TOF Space Points"
        outfile = "%s.%s" % (filetag, self.filetype)
        self.canvas_sp.Print(outfile)

        filetag = "tof_hits_%06d" % (self.spill_count)
        file_content = "TOF Hits"
        outfile = "%s.%s" % (filetag, self.filetype)
        self.canvas_sp.Print(outfile)
        
        return json.dumps(json_doc)	


    def death(self):
        """
        No sub-class-specific cleanup is needed.
        @param self Object reference.
        @returns True
        """
        # print final plots at end of run

        filetag = "tof_times"
        outfile = "%s.%s" % (filetag, self.filetype)
        self.canvas_tof.Print(outfile)
        filetag = "tof_sp"
        outfile = "%s.%s" % (filetag, self.filetype)
        self.canvas_sp.Print(outfile)
        filetag = "tof_hits"
        outfile = "%s.%s" % (filetag, self.filetype)
        self.canvas_hits.Print(outfile)

        # remove any zombie root objects
        ROOT.gDirectory.GetList().Delete()

        return True
