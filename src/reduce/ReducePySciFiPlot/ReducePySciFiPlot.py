"""
ReducePySciFiPlot fills Tracker histograms for online reconstruction.  It produces histograms of Hits per plane, Spacepoints per station and event displays.
It draws them, refreshes the canvases and prints to eps at the end of
run.
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
# Turn off false positives related to ROOT
# pylint: disable = E1101
# Disable messages about too many branches and too many lines.
# pylint: disable = R0912
# pylint: disable = R0915
# pylint: disable = C0103, C0301
# pylint: disable = W0105, W0612, W0201

import ROOT
from ReducePyROOTHistogram import ReducePyROOTHistogram

class ReducePySciFiPlot(ReducePyROOTHistogram): # pylint: disable=R0902
    """
    ReducePySciFiPlot plots several Tracker histograms.
    Currently the following histograms are filled:
    Digits per tracker, station and plane,
    Spacepoints analysis
    2D spacepoingt beam profile plots for triplets and oublets US and DS
    Kuno plots

    Histograms are drawn on different canvases.
    The canvases are refreshed every N spills where N = refresh_rate
    set via refresh_rate data card value (see below).

    The default is to run ROOT in batch mode
    To run in interactive mode, set root_batch_mode = 0 in the data card
    (see below).

    At the end of the run, the canvases are printed to eps files

    A sequence of 3 histograms are output as JSON documents of form:

    @verbatim
    {"image": {"keywords": [...list of image keywords...],
               "description":"...a description of the image...",
               "tag": TAG,
               "image_type": "eps", 
               "data": "...base 64 encoded image..."}}
    @endverbatim

    SciFi, Digits, PE Channel, Spacepoints

    If "histogram_auto_number" (see below) is "true" then the TAG will
    have a number N appended where N means that the histogram was
    produced as a consequence of the (N + 1)th spill processed  by the
    worker. The number will be zero-padded to form a six digit string
    e.g. "00000N". If "histogram_auto_number" is false then no such
    number is appended

    In cases where a spill is input that contains errors (e.g. is
    badly formatted or is missing the data needed to update a
    histogram) then a spill is output which is just the input spill
    with an "errors" field containing the error e.g.

    @verbatim
    {"errors": {..., "bad_json_document": "unable to do json.loads on input"}}
    {"errors": {..., "no_space_points": "no space points"}}
    @endverbatim

    The caller can configure the worker and specify:

    -Image type ("histogram_image_type"). Must be one of those
     supported by ROOT (currently "ps", "eps", "gif", "jpg", "jpeg",
     "pdf", "svg", "png"). Default "eps".
    -Auto-number ("histogram_auto_number"). Default: false. Flag
     that determines if the image tag (see above) has the spill count
     appended to it or not.
    -ROOT batch mode ("root_batch_mode"). Default: 0 (false). Flag
     indicating whether ROOT should be run in batch or interactive 
     mode.
    -Refresh rate ("refresh_rate"). Default: 5. Number of spills
     input before the next set of histograms are output.
    """

    def __init__(self):
        """
        Set initial attribute values.
        @param self Object reference.
        """
        ReducePyROOTHistogram.__init__(self)
        # Do initialisation specific to this class.
        # Refresh_rate determines how often (in spill counts) the
        # canvases are updated.
        self.refresh_rate = 5
        # Histogram initializations. they are defined explicitly in
        # init_histos.

        # Initializing static objects 
        self.eff_cont={}
        self.dig_ison=1   #digit counter
        self.spp_ison=1   #space point counter
        self.digitdict={}

        #Initializing dynamic objects
        self.trp_hist={} #triplet in channel
        self.dig_hist={} #digit in channel
        self.pos_hist={} #position
        self.spt_hist={} #spacepoint triplets- position of triplets
        self.spd_hist={} #spacepoint doublets
        self.sum_hist={} #digit channel sum
        self.npe_hist={} #triplet photoeletron-actual eletrons in fibre
        self.spa_hist={} #spacepoint
        self.sef_hist={} #number of triplets in an event
        self.dub_hist={} #number of doublets in an event
        self.eff_hist={} #space point type



        # Has an end_of_run been processed?
        self.run_ended = False

    def _configure_at_birth(self, config_doc):
        """
        Configure worker from data cards. Overrides super-class method.
        @param self Object reference.
        @param config_doc JSON document.
        @returns True if configuration succeeded.
        """
        # Read in configuration flags and parameters - these will
        # overwrite whatever defaults were set in __init__.
        if 'refresh_rate' in config_doc:
            self.refresh_rate = int(config_doc["refresh_rate"])
        # Initialize histograms, setup root canvases, and set root
        # styles.
        self.__init_histos()
        self.run_ended = False
        return True

    def _update_histograms(self, spill):
        """Update the Histograms """

        daq_evtype = spill.GetSpill().GetDaqEventType()
        if daq_evtype == "end_of_run":
            if (not self.run_ended):
                self.update_histos()
                self.run_ended = True
                return self.get_histogram_images()
            else:
                return []

        # do not try to get data from start/end spill markers
        data_spill = True
        if daq_evtype == "start_of_run" \
              or daq_evtype == "start_of_burst" \
              or daq_evtype == "end_of_burst":
           data_spill = False

        # Get SciFi digits & fill the relevant histograms.
        if data_spill and not self.get_SciFiDigits(spill): 
            raise ValueError("SciFi digits not in spill")

        # Get SciFi spacepoints & fill the relevant histograms.
        if data_spill and not self.get_SciFiSpacepoints(spill): 
            raise ValueError("SciFi spacepoints not in spill")

        if data_spill and not self.get_Spc2(spill): 
            raise ValueError("SciFi spacepoints not in spill")


        # Refresh canvases at requested frequency.
        #print self.refresh_rate
        if self.spill_count % self.refresh_rate == 0:
            self.update_histos()
            return self.get_histogram_images()
        else:
            return []


    def get_SciFiDigits(self, spill):

        """
        Get the SciFiDigits and update the histograms.
        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "digits" in
        the spill.
        """

        spill = spill.GetSpill()
        if spill.GetReconEventSize() == 0:
            raise ValueError("recon_events not in spill")
        reconevents = spill.GetReconEvents()
        # print '# recon events = ',reconevents[0].GetPartEventNumber()
        #for evn in range(spill.GetReconEventSize()):
        for evn in range(reconevents.size()):
            sci_fi_event = reconevents[evn].GetSciFiEvent()
            if sci_fi_event is None:
                raise ValueError("sci_fi_event not in recon_events")
            # Return if we cannot find sci fi digits
            digits = sci_fi_event.digits()
            if digits is None:
                return False

            # Gives information on cabling efficiency
            title = "Run "+str(spill.GetRunNumber())+" Spill "+str(spill.GetSpillNumber())
            #Checks for TOF digit hits in TOF1 and TOF2, if found
            #records the tracker, station, plane, and channel of digit
            for di in range(digits.size()):
                if True:
                    tr=digits[di].get_tracker()
                    st=digits[di].get_station()
                    pl=digits[di].get_plane()
                    ch=digits[di].get_channel()
                    #name = self.digit_hist_name(tr, st, pl)
                    self.dig_hist[tr][st][pl].Fill(ch)
                    if not tr in digitdict:
                        digitdict[tr]={}
                    if not st in digitdict[tr]:
                        digitdict[tr][st]=[]
                        digitdict[tr][st].append([pl,ch])
            #Takes the digit list and looks for channel sums
            for tra in digitdict:
                for sta in digitdict[tra]:
                    dlist=digitdict[tra][sta]
                    dsize=len(dlist)
                    fill_flag=0 
                    if dsize==3 and not dlist[0][0]==dlist[1][0] \
                                and not dlist[0][0]==dlist[2][0] \
                                and not dlist[1][0]==dlist[2][0]:
                        sum=dlist[0][1]+dlist[1][1]+dlist[2][1]
                        self.sum_hist[tra].Fill(float(sum))
                        continue
                    elif dsize==3:
                        sum=-1
                        self.sum_hist[tra].Fill(float(sum))
                        continue
                    if dsize<3:
                        sum=-1
                        self.sum_hist[tra].Fill(float(sum))
                        continue
                    if dsize>3:
                        for a in range(dsize):
                            for b in range(dsize):
                                for c in range(dsize):
                                    if a==b or a==c or b==c:
                                        continue
                                    sum=dlist[a][1]+dlist[b][1]+dlist[c][1]
                                    if sum > 317 and sum < 320:
                                        self.sum_hist[tra].Fill(float(sum))
                                        fill_flag=1
                                    if fill_flag==0:
                                        sum=641
                                        self.sum_hist[tra].Fill(float(sum))
                                        continue

        return True

    # Gives information on reconstruction efficiency
    def get_SciFiSpacepoints(self, spill_data):

        """
        Get the SciFiDigits and update the histograms.

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "digits" in
        the spill.
        """

        if spill_data.GetSpill().GetReconEvent().size() == 0:
            raise ValueError("recon_events not in spill")
        reconevents = spill_data.GetSpill().GetReconEvents()
        # print '# recon events = ',reconevents[0].GetPartEventNumber()
        for evn in range(reconevents.size()):
            sci_fi_event = reconevents[evn].GetSciFiEvent()
            if sci_fi_event is None:
                raise ValueError("sci_fi_event not in recon_events")
            # Return if we cannot find sci fi spacepionts
            SciFiSpacepoints = sci_fi_event.spacepoints()
            if SciFiSpacepoints is None:
                return False

            spill = spill_data.GetSpill()
            title = "Run "+str(spill.GetRunNumber())+" Spill "+str(spill.GetSpillNumber())
            tkcnt = {0:0,1:0}
            tkcnd = {0:0,1:0}
            sp_pos = {}
            for sp in range(SciFiSpacepoints.size()):
                tr = SciFiSpacepoints[sp].get_tracker()
                st = SciFiSpacepoints[sp].get_station()
                x  = SciFiSpacepoints[sp].get_position().x()
                y  = SciFiSpacepoints[sp].get_position().y()
                z  = SciFiSpacepoints[sp].get_position().z()
                type = SciFiSpacepoints[sp].get_type()
                if not tr in sp_pos:
                    sp_pos[tr]      = {}
                    sp_pos[tr]["x"] = []
                    sp_pos[tr]["y"] = []
                    sp_pos[tr]["z"] = []
                    sp_pos[tr]["s"] = []
                    sp_pos[tr]["t"] = []
                sp_pos[tr]["x"].append(x)
                sp_pos[tr]["y"].append(y)
                sp_pos[tr]["z"].append(z)
                sp_pos[tr]["s"].append(st)
                sp_pos[tr]["t"].append(ty)
                if type == "triplet":
                    n_type = 3
                    pe = SciFiSpacepoints[sp].get_npe()
                    for ch in range (0,3):
                        channel=SciFiSpacepoints[sp].get_channels()[ch]
                        c_pl=channel.get_plane()
                        c_ch=channel.get_channel()
                        self.trp_hist[tr][st][c_pl].Fill(c_ch)
                    self.npe_hist[tr].Fill(pe)
                    tkcnt[tr]+=1
                else:
                    n_type = 2
                    tkcnd[tr]+=1
                self.eff_plot.Fill(n_type)
                self.eff_hist[tr][st].Fill(n_type)
                self.spa_hist[tr].Fill(st)      
                if not tr in self.eff_cont:
                    self.eff_cont[tr]={}
                if not st in self.eff_cont[tr]:
                    self.eff_cont[tr][st] = []
                self.eff_cont[tr][st].append(n_type)
            self.sef_hist[0].Fill(tkcnt[0])
            self.sef_hist[1].Fill(tkcnt[1])
            self.dub_hist[0].Fill(tkcnd[0])
            self.dub_hist[1].Fill(tkcnd[1])
            for tra in sp_pos:
                q_flag = 1
                if not len(sp_pos[tra]["z"]) > 3 and len(sp_pos[tra]["z"]) < 6:
                    q_flag = 0
                for si in range(len(sp_pos[tra]["s"])):
                    if sp_pos[tra]["s"].count(sp_pos[tra]["s"][si]) > 1:
                        q_flag = 0
                    if sp_pos[tra]["t"][si] == "duplet":
                        q_flag = 0
                        self.spd_hist[tra][sp_pos[tra]["s"][si]].Fill(sp_pos[tra]["x"][si],sp_pos[tra]["y"][si])
                for ps in range(len(sp_pos[tra]["z"])):
                    if sp_pos[tra]["t"][ps] == "triplet":
                        self.spt_hist[tra][sp_pos[tra]["s"][ps]].Fill(sp_pos[tra]["x"][ps],sp_pos[tra]["y"][ps])

        return True

    def get_Spc2(self, spill_data):
        reconevents = spill_data.GetSpill().GetReconEvents()
        SciFiSpacepoints = sci_fi_event.spacepoints()
    #Sorts spacepoints into triplets and duplets
        for sp in range(SciFiSpacepoints.size()):
            type = SciFiSpacepoints[sp].get_type()
            if type == "triplet":
                m_type = 3
                tracker = SciFiSpacepoints[sp].get_tracker()
                station = SciFiSpacepoints[sp].get_station()
                if tracker==0:
                    self.track1_3Clus.Fill(station)
                if tracker==1:
                    self.track2_3Clus.Fill(station)
            else:
                m_type = 2
                tracker = SciFiSpacepoints[sp].get_tracker()
                station = SciFiSpacepoints[sp].get_station()
                if tracker==0:
                    self.track1_2Clus.Fill(station)
                if tracker==1:
                    self.track2_2Clus.Fill(station)

        return True

    def __init_histos(self): #pylint: disable=R0201, R0914
        """
        Initialise ROOT to display histograms.
        @param self Object reference.
        """ 
        # white canvas
        self.cnv = ROOT.gROOT.SetStyle("Plain")

        #turn off stat box
        self.style = ROOT.gStyle.SetOptStat(0)

        #sensible colour palette
        self.style = ROOT.gStyle.SetPalette(1)

        self.eff_plot=ROOT.TH1D("SP_Type","Space Point Type Across All Stations", 3, 1, 4)
        self.sp_tr_up=ROOT.TH1D("Tr1pStSp","Space Points in TkU per Station",7,0,6) #upstream tracker
        self.sp_tr_up.GetXaxis().SetTitle("Station Number ") 
        self.sp_tr_up.GetXaxis().CenterTitle()
        self.sp_tr_dn=ROOT.TH1D("Tr2pStSp","Space Points in TkD per Station",7,0,6) #downstream tracker
        self.sp_tr_dn.GetXaxis().SetTitle("Station Number ") 
        self.sp_tr_dn.GetXaxis().CenterTitle()
                       
        self.track1_2Clus=ROOT.TH1D("Tr1pStSp","Space Points (2 Clusters) in Tracker1 per Station",7,0,6)
        self.track1_2Clus.GetXaxis().SetTitle("Station Number ") 
        self.track1_2Clus.GetXaxis().CenterTitle()
        self.track2_2Clus=ROOT.TH1D("Tr2pStSp","Space Points (2 Clusters) in Tracker2 per Station",7,0,6)
        self.track2_2Clus.GetXaxis().SetTitle("Station Number ") 
        self.track2_2Clus.GetXaxis().CenterTitle()
        self.track1_3Clus=ROOT.TH1D("Tr1pStSp","Space Points (3 Clusters) in Tracker1 per Station",7,0,6)
        self.track1_3Clus.GetXaxis().SetTitle("Station Number ") 
        self.track1_3Clus.GetXaxis().CenterTitle()
        self.track2_3Clus=ROOT.TH1D("Tr2pStSp","Space Points (3 Clusters) in Tracker2 per Stacdtion",7,0,6)
        self.track2_3Clus.GetXaxis().SetTitle("Station Number ") 
        self.track2_3Clus.GetXaxis().CenterTitle()
   
        for tr in range(0,2):
          self.trp_hist[tr]={} #create two empty dict each for tracker up and down
          self.dig_hist[tr]={} 
          self.pos_hist[tr]={}
          self.spt_hist[tr]={}    
          self.spd_hist[tr]={} 
          self.eff_hist[tr]={}
          if tr == 0:
            trs = "U"
          if tr == 1:
            trs = "D"
          sum_name="Ch_Sum%s" %(trs)
          sum_titl="Digit Channel Sum Tk%s" %(trs)    
          self.sum_hist[tr]=ROOT.TH1F(sum_name,sum_titl, 670,-5,642)
          self.sum_hist[tr].GetXaxis().SetTitle("Channel Number")
          self.sum_hist[tr].GetXaxis().CenterTitle()
          npe_name="PE_Trp%s" %(trs)
          npe_titl="Triplet PE Tk%s" %(trs)
          self.npe_hist[tr]=ROOT.TH1D(npe_name,npe_titl,25,5,150)
          spa_name="SP_Tk%s" %(trs)
          spa_titl="Space Points per Station Tk%s" %(trs)
          self.spa_hist[tr]=ROOT.TH1D(spa_name,spa_titl,7,0,6)
          self.spa_hist[tr].GetXaxis().SetTitle("Station Number")
          self.spa_hist[tr].GetXaxis().CenterTitle()
          sef_name="SE_Tk%s" %(trs)
          sef_titl="Number of Triplets in an Event Tk%s" %(trs)
          self.sef_hist[tr]=ROOT.TH1D(sef_name,sef_titl,7,0,6)
          self.sef_hist[tr].GetXaxis().SetTitle("Station Number")
          self.sef_hist[tr].GetXaxis().CenterTitle()
          sef_name="SE_Tk%s" %(trs)
          sef_titl="Number of Doublets in an Event Tk%s" %(trs)
          self.dub_hist[tr]=ROOT.TH1D(sef_name,sef_titl,7,0,6)
          self.dub_hist[tr].GetXaxis().SetTitle("Station Number")
          self.dub_hist[tr].GetXaxis().CenterTitle()

          for st in range(1,6): #within each tracker create 5 stations
              self.trp_hist[tr][st]={} #{{{}}}
              self.dig_hist[tr][st]={}
              eff_name="Ef_Tk%s%d" %(trs,st)
              eff_titl="Space Point Type Tk%s S%d" %(trs,st)
              self.eff_hist[tr][st]=ROOT.TH1D(eff_name,eff_titl,3,1,4)
              pos_name="PS_Tk%s%d" %(trs,st)
              pos_titl="Tracker Offset Tk%s S%d" %(trs,st)
              self.pos_hist[tr][st]=ROOT.TH1D(pos_name,pos_titl,210,-200,200)
              spt_name="SP_Pos%s%d" %(trs,st)
              spt_titl="Space Point Triplets Tk%s S%d" %(trs,st)
              self.spt_hist[tr][st]=ROOT.TH2D(spt_name,spt_titl,50,-200,200,50,-200,200)
              self.spt_hist[tr][st].GetYaxis().SetTitle("y Position (mm)") 
              self.spt_hist[tr][st].GetXaxis().SetTitle("x Position (mm)")    
              self.spt_hist[tr][st].GetYaxis().CenterTitle()
              self.spt_hist[tr][st].GetXaxis().CenterTitle()
              spt_name="SP_Pos%s%d" %(trs,st)
              spt_titl="Space Point Doublets Tk%s S%d" %(trs,st)
              self.spd_hist[tr][st]=ROOT.TH2D(spt_name,spt_titl,50,-200,200,50,-200,200)
              self.spd_hist[tr][st].GetYaxis().SetTitle("y Position (mm)")
              self.spd_hist[tr][st].GetXaxis().SetTitle("x Position (mm)")
              self.spd_hist[tr][st].GetYaxis().CenterTitle()
              self.spd_hist[tr][st].GetXaxis().CenterTitle()

              for pl in range(0,3): #create 3 planes in each stations, draw histogram for each of them
                  trp_name="TC_Tk%s%d%d" %(trs,st,pl)
                  trp_titl="Triplets in Channel Tk%s S%d P%d" %(trs,st,pl)
                  self.trp_hist[tr][st][pl]=ROOT.TH1D(trp_name,trp_titl,218,0,215)
                  self.trp_hist[tr][st][pl].GetXaxis().SetTitle("Channel Number")
                  self.trp_hist[tr][st][pl].GetXaxis().CenterTitle()  
                  dig_name="DC_Tk%s%d%d" %(trs,st,pl)
                  dig_titl="Digits in Channel Tk%s S%d P%d" %(trs,st,pl)
                  self.dig_hist[tr][st][pl]=ROOT.TH1D(dig_name,dig_titl,215,0,215)
                  self.dig_hist[tr][st][pl].GetXaxis().SetTitle("Channel Number")
                  self.dig_hist[tr][st][pl].GetXaxis().CenterTitle()
       
        # Create canvases
        # Draw() of histos has to be done only once
        # for updating the histograms, just Modified() and Update() on canvases
        # the update/refresh is done in update_histos()

        self.canvas_SciFiDigitUS = ROOT.TCanvas("Digit_in_Channel_US","Digit_in_Channel_US",900, 900)
        self.canvas_SciFiDigitUS.Divide(3,5)
        self.canvas_SciFiDigitUS.cd(1)
        self.dig_hist[0][1][0].Draw() 
        self.canvas_SciFiDigitUS.cd(2)
        self.dig_hist[0][1][1].Draw()
        self.canvas_SciFiDigitUS.cd(3)
        self.dig_hist[0][1][2].Draw()
        self.canvas_SciFiDigitUS.cd(4)
        self.dig_hist[0][2][0].Draw()
        self.canvas_SciFiDigitUS.cd(5)
        self.dig_hist[0][2][1].Draw()
        self.canvas_SciFiDigitUS.cd(6)
        self.dig_hist[0][2][2].Draw()
        self.canvas_SciFiDigitUS.cd(7)
        self.dig_hist[0][3][0].Draw()
        self.canvas_SciFiDigitUS.cd(8)
        self.dig_hist[0][3][1].Draw()
        self.canvas_SciFiDigitUS.cd(9)
        self.dig_hist[0][3][2].Draw()
        self.canvas_SciFiDigitUS.cd(10)
        self.dig_hist[0][4][0].Draw()
        self.canvas_SciFiDigitUS.cd(11)
        self.dig_hist[0][4][1].Draw()
        self.canvas_SciFiDigitUS.cd(12)
        self.dig_hist[0][4][2].Draw()
        self.canvas_SciFiDigitUS.cd(13)
        self.dig_hist[0][5][0].Draw()
        self.canvas_SciFiDigitUS.cd(14)
        self.dig_hist[0][5][1].Draw()
        self.canvas_SciFiDigitUS.cd(15)
        self.dig_hist[0][5][2].Draw()
    

        self.canvas_SciFiDigitDS = ROOT.TCanvas("Digit_in_Channel_DS","Digit_in_Channel_DS",900, 900)
        self.canvas_SciFiDigitDS.Divide(3,5)
        self.canvas_SciFiDigitDS.cd(1)
        self.dig_hist[1][1][0].Draw()
        self.canvas_SciFiDigitDS.cd(2)
        self.dig_hist[1][1][1].Draw()
        self.canvas_SciFiDigitDS.cd(3)
        self.dig_hist[1][1][2].Draw()
        self.canvas_SciFiDigitDS.cd(4)
        self.dig_hist[1][2][0].Draw()
        self.canvas_SciFiDigitDS.cd(5)
        self.dig_hist[1][2][1].Draw()
        self.canvas_SciFiDigitDS.cd(6)
        self.dig_hist[1][2][2].Draw()
        self.canvas_SciFiDigitDS.cd(7)
        self.dig_hist[1][3][0].Draw()
        self.canvas_SciFiDigitDS.cd(8)
        self.dig_hist[1][3][1].Draw()
        self.canvas_SciFiDigitDS.cd(9)
        self.dig_hist[1][3][2].Draw()
        self.canvas_SciFiDigitDS.cd(10)
        self.dig_hist[1][4][0].Draw()
        self.canvas_SciFiDigitDS.cd(11)
        self.dig_hist[1][4][1].Draw()
        self.canvas_SciFiDigitDS.cd(12)
        self.dig_hist[1][4][2].Draw()
        self.canvas_SciFiDigitDS.cd(13)
        self.dig_hist[1][5][0].Draw()
        self.canvas_SciFiDigitDS.cd(14)
        self.dig_hist[1][5][1].Draw()
        self.canvas_SciFiDigitDS.cd(15)
        self.dig_hist[1][5][2].Draw()
      

        self.canvas_SciFiSpacepoints = ROOT.TCanvas("Spacepoint_Types_in_Stations","Spacepoint_Types_in_Stations", 800, 600)
        self.canvas_SciFiSpacepoints.Divide(3,2)
        self.canvas_SciFiSpacepoints.cd(1)
        self.eff_plot.Draw()
        self.canvas_SciFiSpacepoints.cd(2)
        self.track1_3Clus.Draw()
        self.canvas_SciFiSpacepoints.cd(3)
        self.track2_3Clus.Draw()
        self.canvas_SciFiSpacepoints.cd(4)
        self.track1_2Clus.Draw()
        self.canvas_SciFiSpacepoints.cd(5)
        self.track2_2Clus.Draw()
    
        self.canvas_US_trip = ROOT.TCanvas("Spacepoint_Triplets_Up","Spacepoint_Triplets_Up",900, 600)
        self.canvas_US_trip.Divide(3,2)
        self.canvas_US_trip.cd(1)
        self.spt_hist[0][1].Draw("COL")
        self.canvas_US_trip.cd(2)
        self.spt_hist[0][2].Draw("COL")
        self.canvas_US_trip.cd(3)
        self.spt_hist[0][3].Draw("COL")
        self.canvas_US_trip.cd(4)
        self.spt_hist[0][4].Draw("COL")
        self.canvas_US_trip.cd(5)
        self.spt_hist[0][5].Draw("COL")
      
        self.canvas_US_doub = ROOT.TCanvas("Spacepoint_Triplets_Down","Spacepoint_Triplets_Down",900, 600)
        self.canvas_US_doub.Divide(3,2)
        self.canvas_US_doub.cd(1)
        self.spt_hist[1][1].Draw("COL")
        self.canvas_US_doub.cd(2)
        self.spt_hist[1][2].Draw("COL")
        self.canvas_US_doub.cd(3)
        self.spt_hist[1][3].Draw("COL")
        self.canvas_US_doub.cd(4)
        self.spt_hist[1][4].Draw("COL")
        self.canvas_US_doub.cd(5)
        self.spt_hist[1][5].Draw("COL")

   
        self.canvas_DS_trip = ROOT.TCanvas("Spacepoint_Doublets_Up","Spacepoint_Doublets_Up",900, 600)
        self.canvas_DS_trip.Divide(3,2)
        self.canvas_DS_trip.cd(1)
        self.spd_hist[0][1].Draw("COL")
        self.canvas_DS_trip.cd(2)
        self.spd_hist[0][2].Draw("COL")
        self.canvas_DS_trip.cd(3)
        self.spd_hist[0][3].Draw("COL")
        self.canvas_DS_trip.cd(4)
        self.spd_hist[0][4].Draw("COL")
        self.canvas_DS_trip.cd(5)
        self.spd_hist[0][5].Draw("COL")
      
        self.canvas_DS_doub = ROOT.TCanvas("Spacepoint_Doublets_Down","Spacepoint_Doublets_Down",900, 600)
        self.canvas_DS_doub.Divide(3,2)  
        self.canvas_DS_doub.cd(1)
        self.spd_hist[1][1].Draw("COL")
        self.canvas_DS_doub.cd(2)
        self.spd_hist[1][2].Draw("COL")
        self.canvas_DS_doub.cd(3)
        self.spd_hist[1][3].Draw("COL")
        self.canvas_DS_doub.cd(4)
        self.spd_hist[1][4].Draw("COL")
        self.canvas_DS_doub.cd(5)
        self.spd_hist[1][5].Draw("COL")

        self.KunoCanv = ROOT.TCanvas("Kuno Plots","Kuno Plots", 800, 600)
        self.KunoCanv.Divide(2,1)
        self.KunoCanv.cd(1)
        self.sum_hist[0].Draw()
        self.KunoCanv.cd(2)
        self.sum_hist[1].Draw()


        return True

    def update_histos(self):
        """
        Updates histogram canvases. This is called only when the
        number of spills is divisible by the refresh rate.
        @param self Object reference.
        """

        self.canvas_SciFiDigitUS.Update()
        self.canvas_SciFiDigitDS.Update()
        self.canvas_SciFiSpacepoints.Update()
        self.canvas_US_trip.Update()
        self.canvas_US_doub.Update()
        self.canvas_DS_trip.Update()
        self.canvas_DS_doub.Update()
        self.KunoCanv.Update()

    def get_histogram_images(self):
        """
        Get histograms as JSON documents.
        @returns list of 1 JSON document containing the images.
        """

        image_list = []


        #Digits per channel 
        tag = "SciFi_DigitUS"
        keywords = ["SciFi", "Digits", "Plane", "Channel", "US"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiDigitUS)
        image_list.append(doc)

        #Digits per channel 
        tag = "SciFi_DigitDS"
        keywords = ["SciFi", "Digits", "Plane", "Channel", "DS"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiDigitDS)
        image_list.append(doc)

        # Spacepoint summary
        tag = "SciFi_Spacepoints"
        keywords = ["SciFi", "Spacepoints"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiSpacepoints)
        image_list.append(doc)

        # Spacepoint 2D historams summary
        tag = "SciFi_Spacepoints_2D_US_trip"
        keywords = ["SciFi", "Spacepoints", "US" "Triplets"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_US_trip)
        image_list.append(doc)

        # Spacepoint 2D historams summary
        tag = "SciFi_Spacepoints_2D_US_doub"
        keywords = ["SciFi", "Spacepoints", "US" "Doublets"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_US_doub)
        image_list.append(doc)

        # Spacepoint 2D historams summary
        tag = "SciFi_Spacepoints_2D_DS_Trip"
        keywords = ["SciFi", "Spacepoints", "DS" "Triplets"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_DS_trip)
        image_list.append(doc)

        # Spacepoint 2D historams summary
        tag = "SciFi_Spacepoints_2D_DS_Doub"
        keywords = ["SciFi", "Spacepoints", "DS" "Doublets"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_DS_doub)
        image_list.append(doc)

        # Kuno Plots
        tag_pref = "KunoCanv"
        keywords = ["SciFi", "Spacepoints", "Kuno"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag_pref, self.KunoCanv)
        image_list.append(doc)

        return image_list


    def _cleanup_at_death(self):
        """
        Reinitialise histograms at death and print out new (empty) images
        """
        self.__init_histos()
        self.get_histogram_images()
           


