"""
ReducePyTOFPlots fills TOF histograms for slab hits and space points, draws them,
refreshes the canvases and prints to eps at the end of run.
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

import json
import ROOT

class ReducePyTOFPlots:
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

    def __init__(self):
        """
        Set initial attribute values.
        @param self Object reference.
        """
        # Do initialisation specific to this class.

	#refresh_rate determines how often (in spill counts) the canvases are updated
	self.refresh_rate = 5
	
	""" 
	Set ROOT Batch Mode to be False by default
	This can be set via root_batch_mode in the driver script
	"""
	self.root_batch_mode = 0
	
	""" keep track of how many spills we process """
	self.spill_count = 0

	
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
	     
	""" initialize histograms, setup root canvases, and set root styles"""
	self.init_histos()

        return True

    def process(self, json_doc):

        try:
            spill = json.loads(json_doc)
        except ValueError:
            spill = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return json.dumps(spill)

	self.spill_count = self.spill_count + 1

	# get TOF slab hits & fill the relevant histograms
        if not self.get_slab_hits(spill): 
	    return False

	# get TOF space points & fill histograms
        if not self.get_space_points(spill):
	    return False

	# refresh canvases at requested frequency
	if self.spill_count % self.refresh_rate == 0:
	    if not self.update_histos():
	       return False
	  
        return None



    def get_slab_hits(self, spill):
        """ get the TOF slab hits """

	# return if we cannot find slab_hits in the spill
    	if 'slab_hits' not in spill:
	  return False

	slabhits = spill['slab_hits']

	# setup the detectors for which we want to look at hits
	dets = ['tof0','tof1','tof2']
	    	   
	# loop over detector stations ie tof0,tof1,tof2
    	for index,station in enumerate(dets):
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
		       planeNum = dethits[i][j]["plane"]
		       # make sure the plane number is valid so we don't overflow bounds
		       if planeNum < 0 or planeNum > 1:
		          return False
		       self.hSlabHits[index][planeNum].Fill(pos)
    		       if ("pmt0" in dethits[i][j]): #plane 0, pmt0 hit for this slab
		          self.hPmtHits[index][planeNum][0].Fill(pos)
    		       if ("pmt1" in dethits[i][j]): #plane 0, pmt1 hit for this slab
		          self.hPmtHits[index][planeNum][1].Fill(pos)
	
	return True



    def get_space_points(self, spill):
        """ get the TOF space points """

        if 'space_points' not in spill:
           return False

	# check for NoneType since there are particle events with no reconstructed SP
        if spill['space_points'] is None:
           return False

        space_points = spill['space_points']

	# if there are no TOF0,1,2 space point objects, return false
	# not sure if we require all 3, but currently returning false unless we get all 3 detectors
        if 'tof0' not in space_points:
           return False
        sp_tof0 = space_points['tof0']

        if 'tof1' not in space_points:
           return False
        sp_tof1 = space_points['tof1']

        if 'tof2' not in space_points:
           return False
        sp_tof2 = space_points['tof2']

	"""
	Go through the TOF0,1,2 space points
	Make sure it is not null
	Get the number of space points for each particle event
	For each space point, get the x & y
	Get the time & find the Time-of-Flight between 0->1, 0->2, 1->2,
	Fill histograms
	"""
	
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
		    sx = sp_tof0[i][0]["slabX"]
		    sy = sp_tof0[i][0]["slabY"]
		    self.hSpXY_0.Fill(sx,sy)
                    self.hSpSlabX_0.Fill(sx)
                    self.hSpSlabY_0.Fill(sy)

		    
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
		    sx = sp_tof2[i][0]["slabX"]
		    sy = sp_tof2[i][0]["slabY"]
		    self.hSpXY_2.Fill(sx,sy)
                    self.hSpSlabX_2.Fill(sx)
                    self.hSpSlabY_2.Fill(sy)
        	    self.ht12.Fill(t_2-t_1)
		    deltat = t_2-t_1
            if sp_tof2[i] and sp_tof0[i] :
        	if len(sp_tof2[i])==1 and len(sp_tof0[i])==1:
        	    t_2 = sp_tof2[i][0]["time"]
        	    t_0 = sp_tof0[i][0]["time"]
        	    self.ht02.Fill(t_2-t_0)
		    deltat = t_2-t_0

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
		    sx = sp_tof1[i][0]["slabX"]
		    sy = sp_tof1[i][0]["slabY"]
        	    self.ht01.Fill(t_1-t_0)
		    self.hSpXY_1.Fill(sx,sy)
		    self.h2dProf.Fill(sx,sy,len(sp_tof1),1)
                    self.hSpSlabX_1.Fill(sx)
                    self.hSpSlabY_1.Fill(sy)

        return True





    def init_histos(self):
    
	# ROOT batch mode is set to false (i.e. interactive) by default
	# unless it is set differently via the root_batch_mode data card
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
        self.ht01 = ROOT.TH1F("ht01", "TOF0->1;Time (ns);;", 200, 20, 40)
        self.ht12 = ROOT.TH1F("ht12", "TOF1->2;Time (ns);;", 200, 0, 20)
        self.ht02 = ROOT.TH1F("ht02", "TOF0->2;Time (ns);;", 200, 30, 50)
	
	
        self.hSpSlabX_0 = ROOT.TH1F("spx0", "SpacePoints X slabs hit;SlabX;", 10, 0, 10)
        self.hSpSlabY_0 = ROOT.TH1F("spy0", "SpacePoints Y slabs hit;SlabY;", 10, 0, 10)
        self.hSpSlabX_1 = ROOT.TH1F("spx1", "SpacePoints X slabs hit;SlabX;", 10, 0, 10)
        self.hSpSlabY_1 = ROOT.TH1F("spy1", "SpacePoints Y slabs hit;SlabY;", 10, 0, 10)
	self.hSpSlabX_1.SetFillColor(2)
	self.hSpSlabY_1.SetFillColor(2)	
        self.hSpSlabX_2 = ROOT.TH1F("spx2", "SpacePoints X slabs hit;SlabX;", 10, 0, 10)
        self.hSpSlabY_2 = ROOT.TH1F("spy2", "SpacePoints Y slabs hit;SlabY;", 10, 0, 10)
	self.hSpSlabX_2.SetFillColor(4)
	self.hSpSlabY_2.SetFillColor(4)	
	
        self.hSpXY_0 = ROOT.TH2F("hSpXY_0", "tof0: space points;SlabX;SlabY", 10,0,10, 10,0,10)
        self.hSpXY_1 = ROOT.TH2F("hSpXY_1", "tof1: space points;SlabX;SlabY", 10,0,10, 10,0,10)
        self.hSpXY_2 = ROOT.TH2F("hSPXY_2", "tof2: space points;SlabX;SlabY", 10,0,10, 10,0,10)
	
	self.hnsp_0 = ROOT.TH1F("hnsp_0",";#space points in particle event;;",4,0.,4.)
        self.hnsp_1 = ROOT.TH1F("hnsp_1",";#space points in particle event;;",4,0.,4.)
	self.hnsp_2 = ROOT.TH1F("hnsp_2",";#space points in particle event;;",4,0.,4.)
		
	self.hPmt000 = ROOT.TH1F("hPmt000","plane0:pmt0 hits;Slab;",10,0,10)
	self.hPmt001 = ROOT.TH1F("hPmt001","plane0:pmt1 hits;Slab",10,0,10)
	self.hPmt010 = ROOT.TH1F("hPmt010","plane1:pmt0 hits;Slab",10,0,10)
	self.hPmt011 = ROOT.TH1F("hPmt011","plane1:pmt1 hits;Slab",10,0,10)
	
	self.hPmt100 = ROOT.TH1F("hPmt100","plane0:pmt0 hits;Slab",10,0,10)
	self.hPmt100.SetFillColor(2)
	self.hPmt101 = ROOT.TH1F("hPmt101","plane0:pmt1 hits;Slab",10,0,10)
	self.hPmt101.SetFillColor(2)
	self.hPmt110 = ROOT.TH1F("hPmt110","plane1:pmt0 hits;Slab",10,0,10)
	self.hPmt110.SetFillColor(2)
	self.hPmt111 = ROOT.TH1F("hPmt111","plane1:pmt1 hits;Slab;",10,0,10)
	self.hPmt111.SetFillColor(2)
	
	self.hPmt200 = ROOT.TH1F("hPmt200","plane0:pmt0 hits;Slab;",10,0,10)
	self.hPmt200.SetFillColor(4)
	self.hPmt201 = ROOT.TH1F("hPmt201","plane0:pmt1 hits;Slab;",10,0,10)
	self.hPmt201.SetFillColor(4)
	self.hPmt210 = ROOT.TH1F("hPmt210","plane1:pmt0 hits;Slab;",10,0,10)
	self.hPmt210.SetFillColor(4)
	self.hPmt211 = ROOT.TH1F("hPmt211","plane1:pmt1 hits;Slab;",10,0,10)
	self.hPmt211.SetFillColor(4)

	self.h2dProf = ROOT.TProfile2D("h2dProf","Profile X Y;SlabX;SlabY",10,0,10,10,0,10)

	self.hSlabHits = [[]]
	for i in range (0,3):
	  self.hSlabHits.append([])
	  for j in range (0,2):
	      histname = "hSlabHits%d%d" % (i,j)
	      if (j == 0): title = "Raw Slab Hits X; SlabX;"
	      if (j == 1): title = "Raw Slab Hits Y;SlabY;"
	      self.hSlabHits[i].append(ROOT.TH1F(histname,title,10,0,10))
	      self.hSlabHits[i][j].SetFillColor(2*i)


	self.hPmtHits = [[[]]]
	for i in range (0,3):
	  self.hPmtHits.append([])
	  for j in range (0,2):
	      self.hPmtHits[i].append([])
	      for k in range (0,2):
	         histname = "hPmtHits%d%d%d" % (i,j,k)
	         title = "plane %d, pmt %d; Slab;" % (j,k)
	         self.hPmtHits[i][j].append(ROOT.TH1F(histname,title,10,0,10))
	         self.hPmtHits[i][j][k].SetFillColor(2*i)


	# Create canvases
	#
	# Draw() of histos has to be done only once
	# for updating the histograms, just Modified() and Update() on canvases
	# the update/refresh is done in update_histos()
	
	# tof times between 0-1-2
        self.canvas = ROOT.TCanvas("tof", "tof",800,800)
#       ROOT.SetOwnership(self.canvas,False)
	self.canvas.Divide(1,3)
	self.canvas.cd(1)
        self.ht01.Draw()
	self.canvas.cd(2)
	self.ht12.Draw()
	self.canvas.cd(3)
	self.ht02.Draw()
	
	# Space points y vs x 2d
        self.canvas_xy = ROOT.TCanvas("sp2d", "sp2d",900,900)
	self.canvas_xy.Divide(1,3)
	self.canvas_xy.cd(1)
        self.hSpXY_0.Draw("text&&colz")
	self.canvas_xy.cd(2)
        self.hSpXY_1.Draw("text&&colz")
	self.canvas_xy.cd(3)
        self.hSpXY_2.Draw("text&&colz")
	
	# Space points x & y 1d
        self.canvas_slabxy = ROOT.TCanvas("sp1d", "sp1d",800,800)
        self.canvas_slabxy.Divide(1,2)
        self.canvas_slabxy.cd(1)
        self.hSpSlabX_1.Draw()
	self.hSpSlabX_0.Draw("same")
	self.hSpSlabX_2.Draw("same")	
        self.canvas_slabxy.cd(2)
        self.hSpSlabY_1.Draw()
        self.hSpSlabY_0.Draw("same")
        self.hSpSlabY_2.Draw("same")
	leg = ROOT.TLegend(0.6,0.7,0.89,0.89)
	leg.AddEntry(self.hSpSlabX_0,"tof0","l")
	leg.AddEntry(self.hSpSlabX_1,"tof1","l")
	leg.AddEntry(self.hSpSlabX_2,"tof2","l")
	leg.SetFillColor(0)
	leg.SetBorderSize(0)
	leg.Draw()
	
	self.canvas_nsp = ROOT.TCanvas("nSP","nSP",800,800)
        ROOT.SetOwnership(self.canvas_nsp,False)
	self.hnsp_1.SetFillColor(2)
	self.hnsp_1.Draw()
	self.hnsp_0.Draw("same")
	self.hnsp_2.SetFillColor(4)
	self.hnsp_2.Draw("same")
	
	
	# Slab Hits
        self.canvas_hits = ROOT.TCanvas("hits","hits",800,800)
	self.canvas_hits.Divide(1,2)
	self.canvas_hits.cd(1)
	self.hSlabHits[1][0].Draw()
	for i in range (0,3):
	   self.hSlabHits[i][0].Draw("same")
	self.canvas_hits.cd(2)
	self.hSlabHits[1][1].Draw()
	for i in range (0,3):
	   self.hSlabHits[i][1].Draw("same")

	# PMT Hits
        self.canvas_pmthits = ROOT.TCanvas("pmt_hits","pmt_hits",800,800)
	self.canvas_pmthits.Divide(2,2)
	self.canvas_pmthits.cd(1)
	self.hPmtHits[1][0][0].Draw()
	for i in range (0,3):
	   self.hPmtHits[i][0][0].Draw("same")
	self.canvas_pmthits.cd(2)
	self.hPmtHits[1][0][1].Draw()
	for i in range (0,3):
	   self.hPmtHits[i][0][1].Draw("same")
	self.canvas_pmthits.cd(3)
	self.hPmtHits[1][1][0].Draw()
	for i in range (0,3):
	   self.hPmtHits[i][1][0].Draw("same")
	self.canvas_pmthits.cd(4)
	self.hPmtHits[1][1][1].Draw()
	for i in range (0,3):
	   self.hPmtHits[i][1][1].Draw("same")


	return True


    def update_histos(self):
    
        self.canvas.cd(1)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
        self.canvas.cd(2)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
        self.canvas.cd(3)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
	
        self.canvas_xy.cd(1)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
        self.canvas_xy.cd(2)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
        self.canvas_xy.cd(3)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
	
        self.canvas_slabxy.cd(1)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
        self.canvas_slabxy.cd(2)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
	
	self.canvas_nsp.Modified()
	self.canvas_nsp.Update()
	
        self.canvas_hits.cd(1)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
	self.canvas_hits.cd(2)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()

	self.canvas_pmthits.cd(1)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
	self.canvas_pmthits.cd(2)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
	self.canvas_pmthits.cd(3)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()
	self.canvas_pmthits.cd(4)
	ROOT.gPad.Modified()
	ROOT.gPad.Update()

	return True


    def death(self):
        """
        No sub-class-specific cleanup is needed.
        @param self Object reference.
        @returns True
        """
        # Do cleanup specific to this class, none in this case.
	self.canvas.Print("tof_times.eps")
	self.canvas_xy.Print("tof_sp2d.eps")
	self.canvas_slabxy.Print("tof_sp1d.eps")
	self.canvas_nsp.Print("tof_nsp.eps")
	self.canvas_pmthits.Print("tof_npmt.eps")
	self.canvas_hits.Print("tof_nrawhits.eps")
        ROOT.gDirectory.GetList().Delete()

        return True
