#!/usr/bin/env python

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

""" Test the SciFi Kalman code with straight MC data """

#pylint: disable = E1101
#pylint: disable = C0103

import os
import subprocess as sub
import unittest
import numpy
import xboa.common
import ROOT
import libMausCpp #pylint: disable = W0611

mrd = os.environ["MAUS_ROOT_DIR"]
wrk_dir = os.path.join(mrd,
          "tests/integration/test_scifi/test_scifi_kalman_straight/")
plot_dir = os.path.join(mrd, "tmp")
straight_datacard_name = os.path.join(wrk_dir, "datacard_mc_kalman")
straight_root_file_name = os.path.join(wrk_dir, "maus_output_kalman.root")
simulation = os.path.join(wrk_dir, "simulate_scifi.py")
run_number = 7417
tolerance = 4.0
plane_sep = 0.1


def download_geometry():
    """ Download the geometry for the simulation """
    download_script = os.path.join(mrd, "bin/utilities/download_geometry.py")

    call_options = [download_script,
                  "--geometry_download_by", "run_number",
                  "--geometry_download_run_number", str(run_number),
                  "--geometry_download_directory",
                  os.path.join(wrk_dir, "geometry_07417")]

    log_file_name = os.path.join(mrd, "tmp/test_scifi_kalman_straight_geo.log")
    log_file = open(log_file_name, 'w')
    print 'Downloading geometry, logging in', log_file_name
    proc = sub.Popen(call_options, stdout=log_file, stderr=sub.STDOUT)
    proc.wait()
    return proc, log_file_name

def run_straight_simulation():
    """ Run the stright tracks simulation """

    print "Using " + simulation

    call_options = [simulation,
                    "-configuration_file", straight_datacard_name,
                    "-output_root_file_name", straight_root_file_name,
                    "-verbose_level", "0"]

    log_file_name = os.path.join(mrd, "tmp/test_scifi_kalman_straight.log")
    log_file = open(log_file_name, 'w') # pylint: disable=W0612
    # print 'Running simulate_scifi, logging in', log_file_name
    print 'Running simulate_scifi'
    proc = sub.Popen(call_options)
    proc.wait()
    return proc, log_file_name

class TestSciFiKalmanStraight(unittest.TestCase): # pylint: disable=R0904
    """ Run the scifi straight kalman and check output """

    def test_straight_kalman(self):
        """ TestSciFiKalmanStraight: histogram residuals & check properties """

        # Download the geometry
        geo_proc, geo_log_file_name = download_geometry()
        self.assertEqual(geo_proc.returncode, 0, \
          msg="Check log "+ geo_log_file_name)

        # Run the simulation and check it completes with return code 0
        proc, log_file_name = run_straight_simulation()
        self.assertEqual(proc.returncode, 0, msg="Check log "+log_file_name)

        print "Data generated - now check, tolerance = " + str(tolerance)
        dataloader = DataLoader(straight_root_file_name, 15063.)

        mean, std, entries = dataloader.plot_1d("residual", "x")
        errorOnMean = std/numpy.sqrt(entries)
        print str(errorOnMean) + ' ' + str(mean) + ' ' \
          + str(std) + ' ' + str(entries)
        self.assertLess(numpy.abs(mean), tolerance*errorOnMean)

        mean, std, entries = dataloader.plot_1d("residual", "y")
        errorOnMean = std/numpy.sqrt(entries)
        print str(errorOnMean) + ' ' + str(mean) + ' ' \
          + str(std) + ' ' + str(entries)
        self.assertLess(numpy.abs(mean), tolerance*errorOnMean)

        mean, std, entries = dataloader.plot_1d("residual", "xp")
        errorOnMean = std/numpy.sqrt(entries)
        print str(errorOnMean) + ' ' + str(mean) + ' ' \
          + str(std) + ' ' + str(entries)
        self.assertLess(numpy.abs(mean), tolerance*errorOnMean)

        mean, std, entries = dataloader.plot_1d("residual", "yp")
        errorOnMean = std/numpy.sqrt(entries)
        print str(errorOnMean) + ' ' + str(mean) + ' ' \
          + str(std) + ' ' + str(entries)
        self.assertLess(numpy.abs(mean), tolerance*errorOnMean)

class DataLoader(object):
    """ Load scifi kalman data for comparison with MC and produce residual
        data and plot """
    def __init__(self, file_name, target_z):
        """ Initialize the class """
        self.events = []
        self.target_z = target_z
        self.root_objects = []
        self.n_std = 5
        self.verbose = False
        self.load_data(file_name)

    def plot_1d(self, var_1, el_1): # pylint: disable = R0914
        """ Create the residuals histogram and fit with a gaussian """
        name = var_1 + " " + el_1
        values_1 = [event[var_1][el_1] for event in self.events]
        mean = numpy.mean(values_1)
        std = numpy.std(values_1)
        axis_1 = var_1+" "+el_1
        min_var = mean-self.n_std*std
        max_var = mean+self.n_std*std
        canvas = xboa.common.make_root_canvas(name)
        hist = xboa.common.make_root_histogram(name, values_1, axis_1, 100, \
          xmin=min_var, xmax=max_var)
        canvas.Draw()
        hist.Draw()
        fit = ROOT.TF1("fit", "gaus")
        hist.Fit(fit)
        text_box = ROOT.TPaveText(0.6, 0.7, 0.89, 0.89, "NDC")
        text_box.SetFillColor(0)
        text_box.SetBorderSize(0)
        text_box.SetTextSize(0.04)
        text_box.SetTextAlign(12)
        text_box.SetTextSize(0.03)
        text_box.AddText("Number: "+str(len(values_1)))
        text_box.AddText("Mean:   "+str(fit.GetParameter(1)))
        text_box.AddText("Std:    "+str(fit.GetParameter(2)))
        text_box.Draw()
        self.root_objects.append(fit)
        self.root_objects.append(text_box)
        canvas.Update()
        for ext in "root", "png", "pdf":
            name = name.replace(' ', '_')
            canvas.Print(plot_dir+"/"+name+"."+ext)
        return fit.GetParameter(1), fit.GetParameter(2), len(values_1)

    def load_data(self, file_name):
        """ Load the data produced by the simulation """
        print "Loading ROOT file", file_name
        root_file = ROOT.TFile(file_name, "READ") # pylint: disable = E1101

        print "Setting up data tree"
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)

        print "Getting some data"
        for spill_number in range(tree.GetEntries())[:]:
            tree.GetEntry(spill_number)
            spill = data.GetSpill()
            if spill.GetDaqEventType() == "physics_event":
                for ev_number, reco_event in enumerate(spill.GetReconEvents()):
                    scifi_event = reco_event.GetSciFiEvent()
                    recon_data = self.load_scifi_event(scifi_event)
                    mc_event = spill.GetMCEvents()[ev_number]
                    mc_data = self.load_virtual_hits(mc_event)
                    if recon_data == None or mc_data == None:
                        print "Failed for spill", spill_number, \
                          "event", ev_number
                        continue
                    residual = [(key, recon_data[key] - mc_data[key]) \
                      for key in mc_data.keys()]
                    residual = dict(residual)
                    self.events.append({"mc_data":mc_data, \
                      "recon_data":recon_data, "residual":residual})
                    if self.verbose or (spill_number == 0 and ev_number == 0):
                        print "virtual hit",
                        for key in sorted(mc_data.keys()):
                            print key, str(round(mc_data[key], 4)).rjust(8),
                        print "\nscifi hit",
                        for key in sorted(recon_data.keys()):
                            print key, str(round(recon_data[key], 4)).rjust(8),
                        print "\nresidual hit",
                        for key in sorted(residual.keys()):
                            print key, str(round(residual[key], 4)).rjust(8),
                        print

    def load_scifi_event(self, scifi_event):
        """ Extract scifi trackpoint position and momentum data
            in the form of a dictionary """
        z_list = []
        for track in scifi_event.scifitracks():
            for track_point in track.scifitrackpoints():
                pos = track_point.pos()
                z_list.append([abs(pos.z()-self.target_z), track_point])
                if self.verbose:
                    print "sf trackpoint:", pos.x(), pos.y(), pos.z()
        z_list = sorted(z_list)
        if len(z_list) > 0 and z_list[0][0] < 1.:
            track_point = z_list[0][1]
            pos = track_point.pos()
            mom = track_point.mom()
            return {"x":pos.x(), "y":pos.y(), "z":pos.z(), \
              "xp":mom.x()/mom.z(), "yp":mom.y()/mom.z()}

    def load_virtual_hits(self, mc_event):
        """ Extract MC scifi hit position and momentum data
            in the form of a dictionary """
        z_list = []
        for vhit in mc_event.GetVirtualHits():
            pos = vhit.GetPosition()
            if self.verbose:
                print "vhit", pos.x(), pos.y(), pos.z()
            z_list.append([abs(pos.z()-self.target_z), vhit])
        z_list = sorted(z_list)
        if len(z_list) > 0 and z_list[0][0] < plane_sep:
            vhit = z_list[0][1]
            pos = vhit.GetPosition()
            mom = vhit.GetMomentum()
            return {"x":pos.x(), "y":pos.y(), "z":pos.z(), \
              "xp":mom.x()/mom.z(), "yp":mom.y()/mom.z()}

if __name__ == "__main__":
    unittest.main()
