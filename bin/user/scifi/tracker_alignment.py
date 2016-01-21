#!/usr/bin/env python

""" Check tracker alignment vs TOFs """

import sys
import ROOT
import libMausCpp #pylint: disable = W0611
import MausReducedData

# pylint: disable = E1101
# pylint: disable = R0902
# pylint: disable = R0903
# pylint: disable = R0914
# pylint: disable = R0915
# pylint: disable = C0103
# pylint: disable = R0912
# pylint: disable = R0913
# pylint: disable = W0612
# pylint: disable = C0103

class KalmanAlignment:
    """ Class for checking tracker Kalman alignment vs TOFs"""

    def __init__(self):
        """ Constructor """
        self.num_good_events = 0
        self.spoints_npe_cut = 0
        self.nbins = 100
        self.llimit = -400
        self.ulimit = 400
        self.stations = []
        self.histos_x = []
        self.histos_y = []
        self.histos_xy = []

        self.tof1 = MausReducedData.TOF()
        self.tof2 = MausReducedData.TOF()

        for i in range(2):
            for j in range(1, 6):
                self.stations.append(MausReducedData.TrackerStation(i, j))
                for k in range(3):
                self.stations.append(MausReducedData.TrackerStation(i, j))
                self.histos_x.append(ROOT.TH1D("hx_" + str(i) + "_" + str(j)
                    + "_" + str(k),
                    "x residuals T" + str(i) + " S" + str(j) + " P" + str(k),
                    self.nbins, self.llimit, self.ulimit))
                self.histos_y.append(ROOT.TH1D("hy_" + str(i) + "_" + str(j)
                    + "_" + str(k),
                    "y residuals T" + str(i) + " S" + str(j) + " P" + str(k),
                    self.nbins, self.llimit, self.ulimit))
                self.histos_xy.append(ROOT.TH2D("hxy_" + str(i) + "_" + str(j)
                    + "_" + str(k),
                    "x-y residuals T" + str(i) + " S" + str(j) + " P" + str(k),
                    self.nbins, self.llimit, self.ulimit,
                    self.nbins, self.llimit, self.ulimit))

        self.canvas_x = \
            ROOT.TCanvas("canvas_x", "Trackpoint x Residuals", 0, 0, 700, 500)
        self.canvas_x.Divide(15, 2)
        self.canvas_y = \
            ROOT.TCanvas("canvas_y", "Trackpoint y Residuals", 0, 0, 700, 500)
        self.canvas_y.Divide(15, 2)
        self.canvas_xy = \
            ROOT.TCanvas("canvas_xy", "Trackpoint xy Residuals", 0, 0, 700, 500)
        self.canvas_xy.Divide(15, 2)

    def clear_data(self):
        """ Clear internal detector data containers"""
        for stat in self.stations:
            stat.clear_spacepoint_data()
        self.tof1.clear()
        self.tof2.clear()

    def process(self, spill):
        """ Accumulate a spill of data into internal containers and process """
        for i in range(spill.GetReconEvents().size()):
            self.clear_data()
            tk_evt = spill.GetReconEvents()[i].GetSciFiEvent()
            tof_evt = spill.GetReconEvents()[i].GetTOFEvent()

            # Only look at events with exactly 1 spacepoint in tof1, tof2
            # and each tracker station
            good_event = False
            self.tof1.accumulate_data(tof_evt, 1)
            self.tof2.accumulate_data(tof_evt, 2)
            if (self.tof1.num_spoints == 1 and self.tof2.num_spoints == 1):
                good_event = True
            if good_event:
                for stat in self.stations:
                    stat.accumulate_data(tk_evt)
                    if self.measure == "spacepoints":
                        if len(stat.spoints_global_x) != 1:
                            good_event = False
                            break
                    elif self.measure == "pr_straight_seeds":
                        if len(stat.straight_seeds_global_x) != 1:
                            good_event = False
                            break
                    else:
                        print "WARNING: Bad measure set"
                        return False
            if good_event:
                self.num_good_events += 1
                self.fill_histograms(tk_evt)

class PatternRecognitionAlignment:
    """ Class for checking tracker Pat Rec alignment vs TOFs"""

    def __init__(self):
        """ Constructor """
        self.measure = "spacepoints"
        self.num_good_events = 0
        self.spoints_npe_cut = 0
        self.nbins = 100
        self.llimit = -400
        self.ulimit = 400
        self.stations = []
        self.histos_x = []
        self.histos_y = []
        self.histos_xy = []
        self.histos_x0 = []
        self.histos_mx = []
        self.histos_y0 = []
        self.histos_my = []
        for i in range(2):
            for j in range(1, 6):
                self.stations.append(MausReducedData.TrackerStation(i, j))
                self.histos_x.append(ROOT.TH1D("hx_" + str(i) + "_" + str(j),
                    "x residuals T" + str(i) + " S" + str(j),
                    self.nbins, self.llimit, self.ulimit))
                self.histos_y.append(ROOT.TH1D("hy_" + str(i) + "_" + str(j),
                    "y residuals T" + str(i) + " S" + str(j),
                    self.nbins, self.llimit, self.ulimit))
                self.histos_xy.append(ROOT.TH2D("hxy_" + str(i) + "_" + str(j),
                    "x-y residuals T" + str(i) + " S" + str(j),
                    self.nbins, self.llimit, self.ulimit,
                    self.nbins, self.llimit, self.ulimit))
            self.histos_x0.append(ROOT.TH1D("hx0_" + str(i),
                "x0 residuals T" + str(i),
                self.nbins, self.llimit, self.ulimit))
            self.histos_mx.append(ROOT.TH1D("hmx_" + str(i),
                "mx residuals T" + str(i),
                self.nbins, -0.2, 0.2))
            self.histos_y0.append(ROOT.TH1D("hy0_" + str(i),
                "y0 residuals T" + str(i),
                self.nbins, -0.2, 0.2))
            self.histos_my.append(ROOT.TH1D("hmy_" + str(i),
                "my residuals T" + str(i),
                self.nbins, -0.2, 0.2))

        self.tof0 = MausReducedData.TOF()
        self.tof1 = MausReducedData.TOF()
        self.tof2 = MausReducedData.TOF()

        self.canvas_x = \
            ROOT.TCanvas("canvas_x", "Spacepoint x Residuals", 0, 0, 700, 500)
        self.canvas_x.Divide(5, 2)
        self.canvas_y = \
            ROOT.TCanvas("canvas_y", "Spacepoint y Residuals", 0, 0, 700, 500)
        self.canvas_y.Divide(5, 2)
        self.canvas_xy = \
            ROOT.TCanvas("canvas_xy", "Spacepoint xy Residuals", 0, 0, 700, 500)
        self.canvas_xy.Divide(5, 2)

        self.canvas_mxy = \
            ROOT.TCanvas("canvas_mxy", "Fit Gradient Residuals", 0, 0, 700, 500)
        self.canvas_mxy.Divide(2, 2)

    def process(self, spill):
        """ Accumulate a spill of data into internal containers and process """
        for i in range(spill.GetReconEvents().size()):
            self.clear_data()
            tk_evt = spill.GetReconEvents()[i].GetSciFiEvent()
            tof_evt = spill.GetReconEvents()[i].GetTOFEvent()

            # Only look at events with exactly 1 spacepoint in tof1, tof2
            # and each tracker station
            good_event = False
            self.tof0.accumulate_data(tof_evt, 0)
            self.tof1.accumulate_data(tof_evt, 1)
            self.tof2.accumulate_data(tof_evt, 2)
            if (self.tof1.num_spoints == 1 and self.tof2.num_spoints == 1):
                good_event = True
            if good_event:
                for stat in self.stations:
                    stat.accumulate_data(tk_evt)
                    if self.measure == "spacepoints":
                        if len(stat.spoints_global_x) != 1:
                            good_event = False
                            break
                    elif self.measure == "pr_straight_seeds":
                        if len(stat.straight_seeds_global_x) != 1:
                            good_event = False
                            break
                    else:
                        print "WARNING: Bad measure set"
                        return False
            if good_event:
                self.num_good_events += 1
                self.fill_histograms(tk_evt)

    def clear_data(self):
        """ Clear internal detector data containers"""
        for stat in self.stations:
            stat.clear_spacepoint_data()
        self.tof0.clear()
        self.tof1.clear()
        self.tof2.clear()

    def draw(self):
        """ Draw the histograms """
        for i in range(len(self.histos_x)):
            self.canvas_x.cd(i+1)
            self.histos_x[i].Draw()
        for i in range(len(self.histos_y)):
            self.canvas_y.cd(i+1)
            self.histos_y[i].Draw()
        for i in range(len(self.histos_xy)):
            self.canvas_xy.cd(i+1)
            self.histos_xy[i].Draw('col')
        for i in range(len(self.histos_mx)):
            self.canvas_mxy.cd(i+1)
            self.histos_mx[i].Draw()
        for i in range(len(self.histos_my)):
            self.canvas_mxy.cd(i+3)
            self.histos_my[i].Draw()
        self.canvas_x.Update()
        self.canvas_y.Update()
        self.canvas_xy.Update()
        self.canvas_mxy.Update()

    def fill_histograms(self, tk_evt):
        """ Fill all the histograms """

        # Find tof line in z - x and z - y, t = gradient, c = intercept
        c_zx, t_zx, c_zy, t_zy = self.draw_tof12_line()

        # Fill the straight pat rec track parameter residuals histos
        self.fill_spr_parameter_histograms(t_zx, t_zy, tk_evt.straightprtracks())

        # Now find the straight pat rec seed residuals and fill the histos
        self.fill_spr_seed_histograms(c_zx, t_zx, c_zy, t_zy)

    def draw_tof12_line(self):
        """ Find tof line in z - x and z - y, t = gradient, c = intercept """
        dx = self.tof2.spoints_global_x[0] - self.tof1.spoints_global_x[0]
        dy = self.tof2.spoints_global_y[0] - self.tof1.spoints_global_y[0]
        dz = self.tof2.spoints_global_z[0] - self.tof1.spoints_global_z[0]

        t_zx = dx / dz
        c_zx = self.tof1.spoints_global_x[0] - \
                (t_zx * self.tof1.spoints_global_z[0])
        t_zy = dy / dz
        c_zy = self.tof1.spoints_global_y[0] - \
                (t_zy * self.tof1.spoints_global_z[0])

        return [c_zx, t_zx, c_zy, t_zy]

    def fill_spr_parameter_histograms(self, t_zx, t_zy, tracks):
        """ Find the residuals of the gradients of the PatRec fits
            to the TOF line """
        for trk in tracks:
            tracker_id = trk.get_tracker()
            dmx = t_zx - trk.get_global_mx()
            dmy = t_zy - trk.get_global_my()
            self.histos_mx[tracker_id].Fill(dmx)
            self.histos_my[tracker_id].Fill(dmy)

    def fill_spr_seed_histograms(self, c_zx, t_zx, c_zy, t_zy):
        """ Find the straight pat rec seed residuals and fill the histos """
        for i in range(len(self.stations)):
            stat = self.stations[i]
            hx = self.histos_x[i]
            hy = self.histos_y[i]
            hxy = self.histos_xy[i]

            if self.measure == "spacepoints":
                spoints_x = stat.spoints_global_x
                spoints_y = stat.spoints_global_y
                spoints_z = stat.spoints_global_z
            elif self.measure == "pr_straight_seeds":
                spoints_x = stat.straight_seeds_global_x
                spoints_y = stat.straight_seeds_global_y
                spoints_z = stat.straight_seeds_global_z
            else:
                print "WARNING: Bad measure set"
                return False
            for j in range(len(spoints_z)):
                predicted_x = (spoints_z[j] * t_zx) + c_zx
                predicted_y = (spoints_z[j] * t_zy) + c_zy
                res_x = spoints_x[j] - predicted_x
                res_y = spoints_y[j] - predicted_y
                hx.Fill(res_x)
                hy.Fill(res_y)
                hxy.Fill(res_x, res_y)

    def save(self):
        """ Save the canvases as pdfs """
        self.canvas_x.SaveAs('x_residuals.pdf')
        self.canvas_y.SaveAs('y_residuals.pdf')
        self.canvas_xy.SaveAs('xy_residuals.pdf')
        self.canvas_mxy.SaveAs('mxy_residuals.pdf')

def main(file_name):
    """ Main function which performs analysis & produces the plots """

    # Load the ROOT file
    print "Loading ROOT file", file_name
    root_file = ROOT.TFile(file_name, "READ") # pylint: disable = E1101

    # Set up the data tree to be filled by ROOT IO
    print "Setting up data tree"
    data = ROOT.MAUS.Data() # pylint: disable = E1101
    tree = root_file.Get("Spill")
    tree.SetBranchAddress("data", data)

    # Set up Aligment object
    alignment = PatternRecognitionAlignmentAlignment()
    alignment.measure = "pr_straight_seeds"

    # Loop over spills
    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        spill = data.GetSpill()
        # Print some basic information about the spill
        print "Found spill number", spill.GetSpillNumber(),
        print "in run number", spill.GetRunNumber(),
        # physics_events -> to particle data. Everything else is DAQ.
        print "DAQ event type", spill.GetDaqEventType()
        if spill.GetDaqEventType() == "physics_event":
            alignment.process(spill)

    # Draw the histograms
    alignment.draw()

    # Wait for user confirmation to finish
    print 'Processed ' + str(alignment.num_good_events) + ' good events'
    raw_input("Press any key to finish...")

    # Save the histograms
    print 'Saving the histograms'
    alignment.save()

    # Close the ROOT file
    print "Closing root file"
    root_file.Close()

if __name__ == "__main__":
    main(sys.argv[1])
