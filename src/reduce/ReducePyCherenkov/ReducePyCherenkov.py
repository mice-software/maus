import ROOT
from ReducePyROOTHistogram import ReducePyROOTHistogram
from math import sqrt
class ReducePyCherenkov(ReducePyROOTHistogram): # pylint: disable=R0902

    def __init__(self):
                
        ReducePyROOTHistogram.__init__(self)
    
        self.refresh_rate = 4
        self.root_batch_mode = 0
       
        self._hcharge = None
        self._htime = None
        self._htof = None
        self._htof_A = None
        self._htof_B = None
        self._hPMu = None
        self._hPPi = None
        self._hPMuA = None
        self._hPPiA = None
        self._hlight_Mu = None

        self.canvas_charge = None
        self.canvas_time = None
        self.canvas_tof = None
        self.canvas_tof_A = None
        self.canvas_tof_B = None
        self.canvas_hPMu = None
        self.canvas_hPPi = None
        self.canvas_hlight_Mu = None
        self.canvas_hPMuA = None
        self.canvas_hPPiA = None
        
    def _configure_at_birth(self, config_doc):

        if 'refresh_rate' in config_doc:
            self.refresh_rate = int(config_doc["refresh_rate"])
        self.__init_histos()
        return True
    
    def _update_histograms(self, spill):

        if "END_OF_RUN" in spill:
            self.update_histos()
            return self.get_histogram_images()
        
        if not self.get_space_points(spill):
            raise ValueError("space points not in spill")

        # Get charge points & fill histograms.
        if not self.get_charge(spill):
            raise ValueError("charge not in spill")
        
        # Refresh canvases at requested frequency.
    
        if self.spill_count % self.refresh_rate == 0:
            self.update_histos()
            return self.get_histogram_images()
        else:
            return [spill]
    
    def get_space_points(self, spill):

        if 'space_points' not in spill:
            return False
        
        # check for NoneType --there are events with no reconstructed SP
        if spill['space_points'] is None:
            return False
        
        space_points = spill['space_points']
        
        if 'tof0' not in space_points:
            return False
        sp_tof0 = space_points['tof0']
        
        if 'tof1' not in space_points:
            return False
        sp_tof1 = space_points['tof1']
        
        if 'tof2' not in space_points:
            return False
        sp_tof2 = space_points['tof2']

        if 'digits' not in spill:
            return False

        digits = spill['digits']

        #The cuts on e and mu TOFs are made first by 'eyeing' the 1D TOF distributions.
        
        tof_cut_e = 27.00
        tof_cut_mu = 29.00

        d = 7.8241 #Distance between TOF1 and TOF0
        
        MASS_MU = 105.658
        MASS_PI = 139.570

        m = 0
        p = 0

        #TOF vs. Total Charge
        
        for i in range(len(sp_tof1)):
            if sp_tof0[i] and sp_tof1[i] :
                if len(sp_tof0[i])==1 and len(sp_tof1[i])==1:
                    if sp_tof0[i][0]["part_event_number"]==digits[i]['B']['part_event_number']:
                        t_0 = sp_tof0[i][0]["time"]
                        t_1 = sp_tof1[i][0]["time"]
                        self._htof.Fill(t_1-t_0)
                        
                        charge_B = digits[i]["B"]["total_charge"] #This should be done pe and done in the mapper.
                        charge_A = digits[i]["A"]["total_charge"]
                        PE_B = charge_B/20.00 #Photoelectrons Normalized (can be done in mapper)
                        PE_A = charge_A/20.00 #Photoelectrons Normalized
                        if PE_B > 0:
                            self._htof_B.Fill(PE_B, (t_1 - t_0))
                        
                        if PE_A > 0:
                            self._htof_A.Fill(PE_A, (t_1-t_0))
                        
                        #Convert to momenta from TOF using m and d while making the appropriate cuts;
                        #fill momenta histograms
        
                        if ((t_1-t_0) < tof_cut_e):
                            p = -1.0
                        else:
                            if ((t_1-t_0) < tof_cut_mu):
                                m = MASS_MU
                            else:
                                m = MASS_PI
                            if (((t_1-t_0)*0.3) > d):
                                p = m / sqrt(((t_1-t_0)*0.3/d)*((t_1-t_0)*0.3/d)-1)
                            else:
                                p = -1.0
                        if (m==MASS_MU and p > 0.0):
                            self._hPMu.Fill(p)
                            self._hlight_Mu.Fill(p, PE_A)
                        if (m==MASS_PI and p > 0.0):
                            self._hPPi.Fill(p)                                
                        
        return True
    
    def get_charge(self, spill):

        if 'digits' not in spill:
            return False

        digits = spill['digits']
                
        for i in range(len(digits)):
            print i
            for pmt in range(1,9):
                pulse = "pulse_%d" % (pmt)
                arrival_time = "arrival_time_%d" % (pmt)

                #CkovB
                if pmt < 5:
                    charge = digits[i]['B'][pulse]
                    print charge
                    if charge > -1:
                        self._hcharge[i-1].Fill(charge)
                    time = digits[i]['B'][arrival_time]
                    print time
                    if time < 255:
                        self._htime[i-1].Fill(time)
                #CkovA
                if pmt > 4:
                    charge = digits[i]['A'][pulse]
                    if charge > -1:
                        self._hcharge[i-1].Fill(charge)
                    time = digits[i]['A'][arrival_time]
                    if time < 255:
                        self._htime[1-i].Fill(time)
               
        return True
    
    def __init_histos(self): #pylint: disable=R0201, R0914
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

        self._hcharge = []
        
        #c_x = 800

        #Make Canvas for TOF
        
        self.canvas_tof_A = ROOT.TCanvas("tof_A", "tof_A", 600, 600)
        self._htof_A = ROOT.TH2F("tof_A", "tof_A", 100, 0, 50, 200, 15, 35)
        self._htof_A.GetXaxis().SetTitle("#PE")
        self._htof_A.GetYaxis().SetTitle("TOF(CKOVA) (ns)")
        self.canvas_tof_A.cd()
        self._htof_A.Draw()

        self.canvas_tof_B = ROOT.TCanvas("tof_B", "tof_B", 600, 600)
        self._htof_B = ROOT.TH2F("tof_B", "tof_B", 100, 0, 50, 200, 15, 35)
        self._htof_B.GetXaxis().SetTitle("#PE")
        self._htof_B.GetYaxis().SetTitle("TOF(CKOVB) (ns)")
        self.canvas_tof_B.cd()
        self._htof_B.Draw()

        self.canvas_tof = ROOT.TCanvas("tof", "tof", 600, 600)
        self._htof =  ROOT.TH1F("htof", "htof", 100, 10, 40)
        self._htof.GetXaxis().SetTitle("TOF(ns)")
        self.canvas_tof.cd()
        self._htof.Draw()
                                        
        
        #Make Canvas for PMT Charges
        self.canvas_charge = ROOT.TCanvas("charge", "charge", 1200, 800)
        self.canvas_charge.Divide(4,2)
              
        for i in range(0,8):
            #ROOT.gPad.SetLogy()

            hname = "hPMT%d" % (i)
            htitle = "PMT%d" % (i)
            nbins = 200
            x_lo = 0
            x_hi = 200

            self._hcharge.append(ROOT.TH1F(hname, htitle, nbins, x_lo, x_hi))
            self._hcharge[i].GetXaxis().SetTitle("Charge")
            self.canvas_charge.cd(i+1)
            ROOT.gPad.SetLogy()
            self._hcharge[i].Draw()

        #Make Canvas for Arrival Times

        self._htime = []
        
        self.canvas_time = ROOT.TCanvas("time", "time", 1200, 800)
        self.canvas_time.Divide(4,2)
            
        for i in range(0,8):
            #ROOT.gPad.SetLogy()
            
            hname = "harr_time%d" % (i)
            htitle = "Arrival Times%d" % (i)
            nbins = 200
            x_lo = 0
            x_hi = 200
            
            self._htime.append(ROOT.TH1F(hname, htitle, nbins, x_lo, x_hi))
            self._htime[i].GetXaxis().SetTitle("Time (ns)")
            self.canvas_time.cd(i+1)
            self._htime[i].Draw()

        self.canvas_hPMu = ROOT.TCanvas("hPMu", "hPMu", 600, 600)
        self._hPMu = ROOT.TH1F("hPMu", "hPMu", 500, 0, 500)
        self._hPMu.GetXaxis().SetTitle("Momentum (MeV/c)")
        self.canvas_hPMu.cd()
        self._hPMu.Draw()

        self.canvas_hPPi = ROOT.TCanvas("hPPi", "hPPi", 600, 600)
        self._hPPi = ROOT.TH1F("hPPi", "hPPi", 500, 0, 500)
        self._hPPi.GetXaxis().SetTitle("Momentum (MeV/c)")
        self.canvas_hPPi.cd()
        self._hPPi.Draw()

        self.canvas_hlight_Mu = ROOT.TCanvas("hLight", "hLight", 600, 600)
        self._hlight_Mu = ROOT.TH2F("hLight", "hLight", 500, 0, 500, 100, 0, 30)
        self._hlight_Mu.GetXaxis().SetTitle("Momentum (MeV/c)")
        self._hlight_Mu.GetYaxis().SetTitle("#PE")
        self.canvas_hlight_Mu.cd()
        self._hlight_Mu.Draw()
                    
        return True
    
    def update_histos(self):

        self.canvas_charge.Update()
        self.canvas_time.Update()
        self.canvas_tof.Update()
        self.canvas_tof_A.Update()
        self.canvas_tof_B.Update()
        self.canvas_hPMu.Update()
        self.canvas_hPPi.Update()
        self.canvas_hlight_Mu.Update()
        
    def get_histogram_images(self):
        """
        Get histograms as JSON documents.
        @param self Object reference.
        @returns list of 3 JSON documents containing the images.
        """
        image_list = []
        
        # PMT Charge
        # file label = PTM1-8.eps
        tag = "PMT1-8"
        content = "PMTCharge"
        doc = ReducePyROOTHistogram.get_image_doc(self, content, tag, self.canvas_charge)
        image_list.append(doc)

        #ArrivalTimes
        #file label = ArrivalTime.eps
        tag = "ArrivalTime"
        content = "arrival_time"
        doc = ReducePyROOTHistogram.get_image_doc(self, content, tag, self.canvas_time)
        image_list.append(doc)

        #TOF
        #file label = "TOF"
        tag = "TOF"
        content = "TOF"
        doc = ReducePyROOTHistogram.get_image_doc(self, content, tag, self.canvas_tof)
        image_list.append(doc)                                

        #TOFA and Charge
        #file label = "TOF_A"
        tag = "TOF_A"
        content = "TOF_A"
        doc = ReducePyROOTHistogram.get_image_doc(self, content, tag, self.canvas_tof_A)
        image_list.append(doc)

        #TOFB and Charge
        #file label = "TOF_B"
        tag = "TOF_B"
        content = "TOF_B"
        doc = ReducePyROOTHistogram.get_image_doc(self, content, tag, self.canvas_tof_B)
        image_list.append(doc)

        #Muon Momentum
        #file label = hPMu
        tag = "hPMu"
        content = "hPMu"
        doc = ReducePyROOTHistogram.get_image_doc(self, content, tag, self.canvas_hPMu)
        image_list.append(doc)

        #Pion Momentum
        #file label = hPPi
        tag = "hPPi"
        content = "hPPi"
        doc = ReducePyROOTHistogram.get_image_doc(self, content, tag, self.canvas_hPPi)
        image_list.append(doc)

        #PE vs Momentum
        #file label = PE_MOM
        tag = "PE_MOM"
        content = "PE_MOM"
        doc = ReducePyROOTHistogram.get_image_doc(self, content, tag, self.canvas_hlight_Mu)
        image_list.append(doc)
                                
        return image_list
                                                                            
